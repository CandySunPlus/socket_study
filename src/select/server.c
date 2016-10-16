#include "../config.h"
#include <fcntl.h>
#include <sys/select.h>

int recv_message(void *fd) {
    int recv_len, sock_fd = *(int *)fd;
    char buf[MAX_LINE];
    memset(buf, 0, MAX_LINE);
    if ((recv_len = recv(sock_fd, buf, MAX_LINE, 0)) > 0) {
        send(sock_fd, buf, recv_len, 0);
    } else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
        printf("client exit.\n");
        return -1;
    }
    return 1;
}

int main(int argc, char *argv[])
{
    int flags, socket_fd, conn_fd, client_fds[FD_SETSIZE], maxfd, reval, on = 1;
    socklen_t client_len;
    struct sockaddr_in client_addr, server_addr;
    fd_set fds;
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;

    // 1) create socket
    if ((socket_fd = maxfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        perror("socket error");
        exit(1);
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        perror("setsocketopt error");
        exit(1);
    }

    // set socket with non block
    if ((flags = fcntl(socket_fd, F_GETFL)) < 0) {
        perror("F_GETFL error");
        exit(1);
    }

    if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("F_SETFL error");
        exit(1);
    }

    // 2) init server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 3) bind socket
    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind error");
        exit(1);
    }

    // 4) listen
    if (listen(socket_fd, LISTENQ) < 0) {
        perror("listen error");
        exit(1);
    }

    for (int i = 0; i < FD_SETSIZE; i++) {
        client_fds[i] = -1;
    }

    for ( ; ; ) {
        FD_ZERO(&fds);
        FD_SET(socket_fd, &fds);

        for (int i = 0; i <  FD_SETSIZE; i++) {
            conn_fd = client_fds[i];
            if (conn_fd < 0) {
                continue;
            }
            FD_SET(conn_fd, &fds);
            maxfd = maxfd > conn_fd ? maxfd : conn_fd;
        }

        if ((reval = select(maxfd + 1, &fds, NULL, NULL, &tv)) == -1) {
            perror("select error");
            exit(1);
        } else if (reval == 0) {
            continue;
        }

        if (FD_ISSET(socket_fd, &fds)) {
            client_len = sizeof(client_addr);
            if ((conn_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
                perror("accept error");
                exit(1);
            }
            printf("accept new client: %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

            int n;
            for (n = 0; n < FD_SETSIZE; n++) {
                if (client_fds[n] < 0) {
                    client_fds[n] = conn_fd;
                    break;
                }
            }
            if (n == FD_SETSIZE) {
                printf("too many clients.\n");
                exit(1);
            }
        }

        for (int i = 0; i < FD_SETSIZE; i ++) {
            conn_fd = client_fds[i];
            if (conn_fd < 0) {
                continue;
            }

            if (FD_ISSET(conn_fd, &fds)) {
                if (recv_message(&conn_fd) < 0) {
                    FD_CLR(conn_fd, &fds);
                    close(conn_fd);
                    client_fds[i] = -1;
                    continue;
                }
            }
        }
    }

    return 0;
}
