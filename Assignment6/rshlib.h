#ifndef __RSH_LIB_H__
#define __RSH_LIB_H__

#include "dshlib.h"

// Common remote shell client and server constants and definitions

// Constants for communication
#define RDSH_DEF_PORT           1234        // Default port number
#define RDSH_DEF_SVR_INTFACE    "0.0.0.0"   // Default bind to all interfaces
#define RDSH_DEF_CLI_CONNECT    "127.0.0.1" // Default server address (localhost)

// Buffer size for communication
#define RDSH_COMM_BUFF_SZ       (1024 * 64)  // 64 KB buffer

// Command execution return codes
#define STOP_SERVER_SC          200         // Returned if `stop-server` is executed

// End of message delimiter (ASCII EOF, 0x04)
static const char RDSH_EOF_CHAR = 0x04;    

// RSH-specific error codes
#define ERR_RDSH_COMMUNICATION  -50 // Communication error (send/recv failure)
#define ERR_RDSH_SERVER         -51 // General server error
#define ERR_RDSH_CLIENT         -52 // General client error
#define ERR_RDSH_CMD_EXEC       -53 // Command execution error
#define WARN_RDSH_NOT_IMPL      -99 // Feature not implemented

// Output messages for server
#define CMD_ERR_RDSH_COMM   "rdsh-error: communication error\n"
#define CMD_ERR_RDSH_EXEC   "rdsh-error: command execution error\n"
#define CMD_ERR_RDSH_ITRNL  "rdsh-error: internal server error - %d\n"
#define CMD_ERR_RDSH_SEND   "rdsh-error: partial send. Sent %d, expected %d\n"
#define RCMD_SERVER_EXITED  "server appeared to terminate - exiting\n"

// Output messages for client
#define RCMD_MSG_CLIENT_EXITED  "client exited: getting next connection...\n"
#define RCMD_MSG_SVR_STOP_REQ   "client requested server to stop, stopping...\n"
#define RCMD_MSG_SVR_EXEC_REQ   "rdsh-exec:  %s\n"
#define RCMD_MSG_SVR_RC_CMD     "rdsh-exec:  rc = %d\n"

// Client function prototypes (from `rsh_sli.c`)
int start_client(char *address, int port);
int client_cleanup(int cli_socket, char *cmd_buff, char *rsp_buff, int rc);
int exec_remote_cmd_loop(char *address, int port);

// Server function prototypes (from `rsh_server.c`)
int start_server(char *ifaces, int port, int is_threaded);
int boot_server(char *ifaces, int port);
int stop_server(int svr_socket);
int send_message_eof(int cli_socket);
int send_message_string(int cli_socket, char *buff);
int process_cli_requests(int svr_socket);
int exec_client_requests(int cli_socket);
int rsh_execute_pipeline(int socket_fd, command_list_t *clist);

// Optional built-in command handling (from `rsh_server.c`)
Built_In_Cmds rsh_match_command(const char *input);
Built_In_Cmds rsh_built_in_cmd(cmd_buff_t *cmd);

#endif // __RSH_LIB_H__
