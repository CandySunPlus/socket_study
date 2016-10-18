#include <poll.h>
#include "../config.h"

int create_and_listen_socket() {
    int socket_fd;
    struct sockaddr_in server_addr;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        perror("socket error");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind error");
        return -1;
    }

    if (listen(socket_fd, LISTENQ) < 0) {
        perror("listen error");
        return -1;
    }

    return socket_fd;
}

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
    int socket_fd, conn_fd, reval;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    struct pollfd fds[MAX_CONNECT];

    if ((socket_fd = create_and_listen_socket()) < 0) {
        exit(1);
    }

    fds[0].fd = socket_fd;
    fds[0].events = POLLIN;

    for (int i = 1; i < MAX_CONNECT; i ++) {
        fds[i].fd = -1;
    }

    for ( ; ; ) {
        if ((reval = poll(fds, sizeof(fds) / sizeof(fds[0]), -1)) < 0) {
            perror("poll error");
            exit(1);
        } else if (reval == 0) {
            continue;
        }

        if (fds[0].revents & POLLIN) {
            if ((conn_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len))< 0) {
                perror("accept error");
                exit(1);
            }
            printf("accept new client: %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
            int i;
            for (i = 1; i < MAX_CONNECT; i++) {
                if (fds[i].fd < 0) {
                    fds[i].fd = conn_fd;
                    fds[i].events = POLLIN;
                    break;
                }
            }
            if (i == MAX_CONNECT) {
                printf("too many connections");
                close(conn_fd);
                continue;
            }
        }

        for (int n = 1; n < MAX_CONNECT; n++) {
            if (fds[n].fd < 0) {
                continue;
            }
            if (fds[n].revents & POLLIN) {
                if (recv_message(&(fds[n].fd)) < 0) {
                    close(fds[n].fd);
                    fds[n].fd = -1;
                    continue;
                }
            }
        }
    }
    close(socket_fd);

    return 0;
}
