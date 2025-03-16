#ifndef __DSHLIB_H__
#define __DSHLIB_H__

#include <stdbool.h>

// Constants for command structure sizes
#define EXE_MAX 64
#define ARG_MAX 256
#define CMD_MAX 10
#define CMD_ARGV_MAX (CMD_MAX + 1)
#define SH_CMD_MAX (EXE_MAX + ARG_MAX)

// Special character defines
#define SPACE_CHAR  ' '
#define PIPE_CHAR   '|'
#define PIPE_STRING "|"
#define REDIRECT_INPUT_CHAR '<'
#define REDIRECT_OUTPUT_CHAR '>'
#define APPEND_OUTPUT_CHAR '>>'

// Shell prompt and exit command
#define SH_PROMPT       "dsh4> "
#define EXIT_CMD        "exit"

// Standard return codes
#define OK                       0
#define OK_EXIT                 -7
#define WARN_NO_CMDS            -1
#define ERR_TOO_MANY_COMMANDS   -2
#define ERR_CMD_OR_ARGS_TOO_BIG -3
#define ERR_CMD_ARGS_BAD        -4      // For extra credit
#define ERR_MEMORY              -5
#define ERR_EXEC_CMD            -6

// Remote shell specific error codes
#define ERR_RDSH_COMMUNICATION  -50
#define ERR_RDSH_SERVER         -51
#define ERR_RDSH_CLIENT         -52
#define ERR_RDSH_CMD_EXEC       -53

// Command buffer structure
typedef struct cmd_buff {
    int  argc;
    char *argv[CMD_ARGV_MAX];
    char *_cmd_buffer;
    char *input_file;   // Extra credit: Stores input redirection file `<`
    char *output_file;  // Extra credit: Stores output redirection file `>`
    bool append_mode;   // Extra credit: Sets append mode for output file
} cmd_buff_t;

// Command list structure
typedef struct command_list {
    int num;
    cmd_buff_t commands[CMD_MAX];
} command_list_t;

// Built-in commands enumeration
typedef enum {
    BI_CMD_EXIT,
    BI_CMD_DRAGON,
    BI_CMD_CD,
    BI_CMD_RC,              // Extra credit command
    BI_CMD_STOP_SVR,        // New command: "stop-server"
    BI_NOT_BI,
    BI_EXECUTED
} Built_In_Cmds;

// Function prototypes for command parsing and execution
int alloc_cmd_buff(cmd_buff_t *cmd_buff);
int free_cmd_buff(cmd_buff_t *cmd_buff);
int clear_cmd_buff(cmd_buff_t *cmd_buff);
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff);
int close_cmd_buff(cmd_buff_t *cmd_buff);
int build_cmd_list(char *cmd_line, command_list_t *clist);
int free_cmd_list(command_list_t *cmd_list);
int parse_pipeline(char *cmd_line, command_list_t *cmd_list);
int execute_pipeline(command_list_t *clist);
int exec_local_cmd_loop();
int exec_cmd(cmd_buff_t *cmd);

// Built-in command handling
Built_In_Cmds match_command(const char *input);
Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd);

// Remote shell function prototypes
int exec_remote_cmd_loop(char *address, int port);
int start_client(char *server_ip, int port);
int client_cleanup(int cli_socket, char *cmd_buff, char *rsp_buff, int rc);

#endif  // __DSHLIB_H__
