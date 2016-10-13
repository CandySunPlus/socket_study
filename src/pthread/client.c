#include "../config.h"
#include <pthread.h>

void *recv_message(void *fd) {
    int socket_fd = *(int *)fd, recv_len;
    char buf[MAX_LINE];
    memset(buf, 0, MAX_LINE);

    for ( ; ; ) {
        if ((recv_len = recv(socket_fd, buf, MAX_LINE, 0)) == -1) {
            perror("receive error");
            return NULL;
        }

        buf[recv_len] = '\0';

        printf("%s", buf);
    }

    close(socket_fd);
    return NULL;
}

int main(int argc, char *argv[])
{

    int socket_fd;
    pthread_t recv_tid;
    struct sockaddr_in server_addr;

    if (argc != 2) {
        printf("usage: pthread_cli <ip_address>\n");
        exit(1);
    }

    // 1) create socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        perror("socket error");
        exit(1);
    }

    // 2) init server address
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) < 0) {
        perror("inet_pton error");
        exit(1);
    }

    // 3) connect to server
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect error");
        exit(1);
    }

    if (pthread_create(&recv_tid, NULL, recv_message, &socket_fd) == -1) {
        perror("pthread error");
        exit(1);
    }

    char msg[MAX_LINE];
    memset(msg, 0, MAX_LINE);

    while(fgets(msg, MAX_LINE, stdin) != NULL) {
        if (memcmp(msg, "exit\n", strlen(msg)) == 0) {
            // exit client
            printf("bye.\n");
            pthread_cancel(recv_tid);
            exit(0);
        }

        if (send(socket_fd, msg, strlen(msg), 0) == -1) {
            perror("send error.");
            exit(1);
        }
    }

    return 0;
}
