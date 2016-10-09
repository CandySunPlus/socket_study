#include "config.h"

ssize_t readline(int fd, char *ptr_v, ssize_t maxlen) {
    ssize_t length, rc;
    char c, *ptr;

    ptr = ptr_v;
    for (length = 1; length < maxlen; length++) {
        if ((rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;
            if (c == '\n') break;
        } else if (rc == 0) {
            *ptr = 0;
            return (length - 1);
        } else {
            return -1;
        }
    }
    *ptr = 0;
    return length;
}

int main(int argc, char *argv[])
{
    int sock_fd;
    char send_line[MAX_LINE], recv_line[MAX_LINE];
    struct sockaddr_in server_addr;

    if (argc != 2) {
        printf("usage: client <IP>\n");
        exit(1);
    }

    // 1) init socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP))< 0) {
        perror("socket error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // 2) set server addr info
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) < 0) {
        printf("inet_pton error for %s\n", argv[1]);
        exit(1);
    }

    // 3) connect to server
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect error");
        exit(1);
    }

    // 4) send and receive from server
    while (fgets(send_line, MAX_LINE, stdin) != NULL) {
        write(sock_fd, send_line, strlen(send_line));
        if (readline(sock_fd, recv_line, MAX_LINE) == 0) {
            perror("server terminated prematurely");
            exit(1);
        }
        if (fputs(recv_line, stdout) == EOF) {
            perror("fputs error");
            exit(1);
        }
    }
    close(sock_fd);
    return 0;
}
