#include "../config.h"

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr, client_addr;
    int listen_fd, conn_fd, opt = 1;
    pid_t child_pid;
    socklen_t client_len;

    // 1) init socket
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        perror("socket error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // 2) set socket reuse address
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("socket set error");
        exit(1);
    }

    // 3) bind socket to address and port
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind error");
        exit(1);
    }

    // 4) listen
    if (listen(listen_fd, LISTENQ) < 0) {
        perror("listen error");
        exit(1);
    }

    // 5) receive client request
    for ( ; ; ) {
        client_len = sizeof(client_addr);
        if ((conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            perror("accept error");
            exit(1);
        }

        printf("accept new client: %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

        if ((child_pid = fork()) == 0) {
            // 子进程关闭监听句柄
            close(listen_fd);
            ssize_t n;
            char buff[MAX_LINE];
            while ((n = read(conn_fd, buff, MAX_LINE)) > 0) {
                write(conn_fd, buff, n);
            }
            exit(0);
        }
        close(conn_fd);
    }
    close(listen_fd);
    return 0;
}
