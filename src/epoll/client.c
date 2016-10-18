// osx not support epoll, use kqueue
#include <sys/event.h>
#include "../config.h"

int create_and_connect_socket(char *addr) {
    int socket_fd;
    struct sockaddr_in server_addr;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        perror("socket error");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, addr, &server_addr.sin_addr) < 0) {
        perror("inet_pton error");
        return -1;
    }

    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect error");
        return -1;
    }
    return socket_fd;
}

int recv_message(void *fd) {
    int socket_fd = *(int *)fd, recv_len;
    char buf[MAX_LINE];
    memset(buf, 0, MAX_LINE);

    if ((recv_len = read(socket_fd, buf, MAX_LINE)) < 0) {
        perror("receive error.");
        return -1;
    } else if (recv_len == 0) {
        printf("server has been closed.\n");
        return -1;
    }
    buf[recv_len] = '\0';
    printf("%s\n", buf);
    return 0;
}

int main(int argc, char *argv[])
{
    const static int FD_NUM = 2;
    int socket_fd, kq, kq_len, send_len;
    struct kevent changes[FD_NUM];
    struct kevent events[FD_NUM];
    char send_line[ MAX_LINE ];

    if (argc < 2) {
        printf("usage: epoll_cli <ip_address>\n");
        exit(1);
    }

    if ((socket_fd = create_and_connect_socket(argv[1])) < 0) {
        exit(1);
    }

    kq = kqueue();
    EV_SET(changes, STDIN_FILENO, EVFILT_READ, EV_ADD, 0, 0, NULL);
    EV_SET(changes + 1, socket_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, changes, FD_NUM, NULL, 0, NULL) < 0) {
        perror("kevent error");
        exit(1);
    }

    for ( ; ; ) {
        if ((kq_len = kevent(kq, NULL, 0, events, FD_NUM, NULL)) < 0) {
            perror("kevent error");
            exit(1);
        }
        for (int i = 0; i < kq_len; i++) {
            if (events[i].ident == socket_fd) {
                if (recv_message(&(events[i].ident)) < 0) {
                    close(socket_fd);
                    exit(1);
                }
            } else if (events[i].ident == STDIN_FILENO) {
                if ((send_len = read(events[i].ident, send_line, MAX_LINE)) > 0) {
                    if (memcmp(send_line, "exit\n", send_len) == 0) {
                        printf("bye.\n");
                        close(socket_fd);
                        exit(0);
                    }
                    send_line[send_len - 1] = '\0';
                    send(socket_fd, send_line, send_len, 0);
                }
            }
        }
    }
    close(socket_fd);
    return 0;
}
