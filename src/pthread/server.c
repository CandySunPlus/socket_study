#include "../config.h"
#include <pthread.h>

void* recv_message(void *fd) {
    int recv_len, sock_fd = *(int *)fd;
    char buf[MAX_LINE];
    memset(buf, 0, MAX_LINE);
    for ( ; ; ) {
        if ((recv_len = recv(sock_fd, buf, MAX_LINE, 0)) > 0) {
            send(sock_fd, buf, recv_len, 0);
        } else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
            printf("client exit.\n");
            break;
        }
    }
    close(sock_fd);
    return NULL;
}

int main(int argc, char *argv[])
{
    int listen_fd, conn_fd;
    socklen_t client_addr_len;
    // 线程ID
    pthread_t recv_tid, send_tid;

    struct sockaddr_in server_addr, client_addr;

    // 1) create socket
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        perror("socket error");
        exit(1);
    }

    // 2) init server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 3) bind socket
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind error");
        exit(1);
    }

    // 4) listen
    if (listen(listen_fd, LISTENQ) < 0) {
        perror("listen error");
        exit(1);
    }

    // 5) accept client and create thread
    client_addr_len = sizeof(client_addr);
    for ( ; ; ) {
        if ((conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("accept error");
            exit(1);
        }
        printf("accept new client: %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

        if (pthread_create(&recv_tid, NULL, recv_message, &conn_fd) == -1) {
            perror("pthread create error");
            exit(1);
        }
    }

    close(listen_fd);
    return 0;
}
