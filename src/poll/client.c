#include <poll.h>
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

    if ((recv_len = recv(socket_fd, buf, MAX_LINE, 0)) == -1) {
        perror("receive error");
        return -1;
    }

    buf[recv_len] = '\0';
    printf("%s\n", buf);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: poll_cli <ip_address>\n");
        exit(1);
    }

    int socket_fd, reval, send_len;
    struct pollfd fds[2];
    char send_message[MAX_LINE];

    if ((socket_fd = create_and_connect_socket(argv[1])) < 0) {
        exit(1);
    }

    fds[0].fd = socket_fd;
    fds[0].events = POLLIN;

    fds[1].fd = fileno(stdin);
    fds[1].events = POLLIN;

    for ( ; ; ) {
        if ((reval = poll(fds, 2, -1)) < 0) {
            perror("poll error");
            exit(1);
        } else if (reval == 0) {
            continue;
        }

        if (fds[0].revents & POLLIN) {
            if (recv_message(&(fds[0].fd)) < 0) {
                exit(1);
            }
        }

        if (fds[1].revents & POLLIN) {
            if ((send_len = read(fds[1].fd, send_message, MAX_LINE)) > 0) {
                if (memcmp(send_message, "exit\n", send_len) == 0) {
                    printf("bye.\n");
                    exit(0);
                }
                send_message[send_len - 1] = '\0';
                write(socket_fd, send_message, send_len);
            }
        }
    }
    close(socket_fd);
    return 0;
}
