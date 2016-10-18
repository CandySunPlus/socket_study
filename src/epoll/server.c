// osx not support epoll, use kqueue
#include <sys/event.h>
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
    /* } else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) { */
    /*     printf("client exit.\n"); */
    /*     return -1; */
    }
    return 1;
}

int main(int argc, char *argv[])
{
    int socket_fd, conn_fd, kq, kq_len;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    struct kevent changes[1];
    struct kevent events[MAX_CONNECT];

    if ((socket_fd = create_and_listen_socket()) < 0) {
        exit(1);
    }

    // register fd to kqueue;
    kq = kqueue();
    EV_SET(changes, socket_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);

    if (kevent(kq, changes, 1, NULL, 0, NULL) < 0) {
        perror("kevent error");
    }

    for ( ; ; ) {
        if((kq_len = kevent(kq, NULL, 0, events, MAX_CONNECT, NULL)) < 1) {
            perror("kevent error");
        }
        for (int i = 0; i < kq_len; i++) {
            if (events[i].flags & EV_EOF) {
                printf("client exit.\n");
                conn_fd = events[i].ident;
                EV_SET(changes, conn_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
                if (kevent(kq, changes, 1, NULL, 0, NULL) < 0) {
                    perror("kevent error");
                }
                close(conn_fd);

            } else if (events[i].ident == socket_fd) {
                if ((conn_fd = accept(events[i].ident, (struct sockaddr*)&client_addr, &client_len)) < 0) {
                    perror("accept error");
                    exit(1);
                }
                printf("accept new client:%s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
                EV_SET(changes, conn_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, changes, 1, NULL, 0, NULL) < 0) {
                    perror("kevent error");
                }
            } else if (events[i].flags & EVFILT_READ) {
                recv_message(&(events[i].ident));
            }
        }
    }
    close(socket_fd);
    return 0;
}
