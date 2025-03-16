#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "dshlib.h"
#include "rshlib.h"

int exec_remote_cmd_loop(char *address, int port) {
    char *cmd_buff = malloc(RDSH_COMM_BUFF_SZ);
    char *rsp_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (!cmd_buff || !rsp_buff) {
        return client_cleanup(-1, cmd_buff, rsp_buff, ERR_MEMORY);
    }

    int cli_socket = start_client(address, port);
    if (cli_socket < 0) {
        perror("start client");
        return client_cleanup(cli_socket, cmd_buff, rsp_buff, ERR_RDSH_CLIENT);
    }

    while (1) {
        printf("rsh> ");
        if (!fgets(cmd_buff, RDSH_COMM_BUFF_SZ, stdin)) break;

        size_t len = strlen(cmd_buff);
        if (cmd_buff[len - 1] == '\n') cmd_buff[len - 1] = '\0';

        if (send(cli_socket, cmd_buff, len, 0) < 0) {
            perror("send");
            break;
        }

        ssize_t recv_bytes;
        while (1) {
            recv_bytes = recv(cli_socket, rsp_buff, RDSH_COMM_BUFF_SZ, 0);

            if (recv_bytes < 0) {
                perror("recv");
                break;
            } else if (recv_bytes == 0) {
                // Server closed the connection, exit cleanly
                printf("\nServer closed connection.\n");
                return client_cleanup(cli_socket, cmd_buff, rsp_buff, OK);
            }

            int is_eof = (rsp_buff[recv_bytes - 1] == RDSH_EOF_CHAR);
            printf("%.*s", (int)recv_bytes, rsp_buff);
            if (is_eof) break;
        }

        if (strcmp(cmd_buff, "exit") == 0 || strcmp(cmd_buff, "stop-server") == 0) {
            break;
        }
    }

    return client_cleanup(cli_socket, cmd_buff, rsp_buff, OK);
}


int client_cleanup(int cli_socket, char *cmd_buff, char *rsp_buff, int rc) {
    if (cli_socket > 0) {
        close(cli_socket);
    }
    free(cmd_buff);
    free(rsp_buff);
    return rc;
}


int start_client(char *server_ip, int port) {
    int cli_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (cli_socket < 0) {
        perror("socket");
        return ERR_RDSH_CLIENT;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(cli_socket);
        return ERR_RDSH_CLIENT;
    }

    if (connect(cli_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(cli_socket);
        return ERR_RDSH_CLIENT;
    }
    return cli_socket;
}
