#include <sys/select.h>
#include <fcntl.h>
#include "../config.h"

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
    int socket_fd, reval, maxfd, flags, send_len;
    struct sockaddr_in server_addr;
    fd_set fds;
    struct timeval tv;
    char send_line[MAX_LINE];

    tv.tv_sec = 3;
    tv.tv_usec = 0;

    if (argc < 2) {
        printf("usage: select_cli <ip_address>\n");
        exit(1);
    }

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        perror("socket error");
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

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, argv[1], &(server_addr.sin_addr)) < 0) {
        perror("inet_pton error");
        exit(1);
    }

    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        if (errno != EINPROGRESS) {
            perror("connect error");
            exit(1);
        }
    }

    for ( ; ; ) {
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        FD_SET(socket_fd, &fds);

        maxfd = STDIN_FILENO > socket_fd ? STDIN_FILENO + 1 : socket_fd + 1;

        if ((reval = select(maxfd, &fds, NULL, NULL, &tv)) < 0) {
            perror("select error");
            exit(1);
        } else if (reval == 0) {
            continue;
        }

        if (FD_ISSET(socket_fd, &fds)) {
            if (recv_message(&socket_fd) < 0) {
                FD_CLR(socket_fd, &fds);
                close(socket_fd);
                exit(1);
            }
        }

        if (FD_ISSET(STDIN_FILENO, &fds)) {
            if ((send_len = read(STDIN_FILENO, send_line, MAX_LINE)) > 0) {
                if (memcmp(send_line, "exit\n", send_len) == 0) {
                    printf("bye.\n");
                    break;
                }
                send_line[send_len - 1] = '\0';
                write(socket_fd, send_line, send_len);
            }
        }
    }

    close(socket_fd);
    return 0;
}
