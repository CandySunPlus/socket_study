#include <sys/select.h>
#include <fcntl.h>
#include "../config.h"

int recv_message(void *fd) {
    int socket_fd = *(int *)fd, recv_len;
    char buf[MAX_LINE];
    memset(buf, 0, MAX_LINE);

    if ((recv_len = recv(socket_fd, buf, MAX_LINE, 0)) == -1) {
        perror("receive error");
        return -1;
    }

    buf[recv_len] = '\0';
    printf("%s", buf);
    return 0;
}


int main(int argc, char *argv[])
{
    int socket_fd, reval, stdin_fp = fileno(stdin), maxfd, flags;
    struct sockaddr_in server_addr;
    fd_set fds;
    struct timeval tv;
    char recv_line[MAX_LINE], send_line[MAX_LINE];

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

    flags = fcntl(socket_fd, F_GETFL);
    if (fcntl(socket_fd, F_SETFL, flags|O_NONBLOCK) < 0) {
        perror("fcntl error");
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
        FD_SET(stdin_fp, &fds);
        FD_SET(socket_fd, &fds);

        maxfd = stdin_fp > socket_fd ? stdin_fp + 1 : socket_fd + 1;

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

        if (FD_ISSET(stdin_fp, &fds)) {
            if (fgets(send_line, sizeof(MAX_LINE), stdin) != NULL) {
                send_line[strlen(send_line) - 1] = '\0';
                write(socket_fd, send_line, strlen(send_line));
            }
        }
    }

    close(socket_fd);
    return 0;
}
