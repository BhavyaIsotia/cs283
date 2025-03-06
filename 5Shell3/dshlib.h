//dshlib.h
#ifndef __DSHLIB_H__
    #define __DSHLIB_H__


// Command structure sizes
#define EXE_MAX 64
#define ARG_MAX 256
#define CMD_ARGV_MAX (CMD_MAX + 1) // Adjust as needed


#define SH_CMD_MAX (EXE_MAX + ARG_MAX) 

// Constants for return codes and errors
#define OK 0
#define ERR_EXEC_CMD 1
#define ERR_MEMORY 2
#define WARN_NO_CMDS 3
#define ERR_TOO_MANY_COMMANDS 4

// Command characters
#define PIPE_CHAR '|'
#define REDIRECT_INPUT_CHAR '<'
#define REDIRECT_OUTPUT_CHAR '>'
#define APPEND_OUTPUT_CHAR '>>'

// Prompt and shell-specific strings
#define SH_PROMPT "dsh> "
#define EXIT_CMD "exit"

// Limit the maximum number of commands in the pipeline
#define CMD_MAX 10



typedef struct cmd_buff
{
    int  argc;
    char *argv[CMD_ARGV_MAX];
    char *_cmd_buffer;
} cmd_buff_t;

typedef struct {
    int num_cmds;
    cmd_buff_t commands[CMD_MAX];  // ✅ Use `cmd_buff_t` instead of anonymous struct
    int cmd_count;
} command_list_t;



// Function declarations
int handle_redirection(cmd_buff_t *cmd);
int parse_pipeline(char *cmd_line, command_list_t *cmd_list);
int execute_pipeline(command_list_t *cmd_list);

int alloc_cmd_buff(cmd_buff_t *cmd_buff);
int free_cmd_buff(cmd_buff_t *cmd_buff);
int clear_cmd_buff(cmd_buff_t *cmd_buff);
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff);


// Built-in command stuff
typedef enum {
    BI_CMD_EXIT,
    BI_CMD_DRAGON,
    BI_CMD_CD,
    BI_NOT_BI,
    BI_EXECUTED,
    BI_RC,
} Built_In_Cmds;
Built_In_Cmds match_command(const char *input); 
Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd);

// Main execution context
int exec_local_cmd_loop();
int exec_cmd(cmd_buff_t *cmd);

// Output constants
#define CMD_OK_HEADER       "PARSED COMMAND LINE - TOTAL COMMANDS %d\n"
#define CMD_WARN_NO_CMD     "warning: no commands provided\n"
#define CMD_ERR_PIPE_LIMIT  "error: piping limited to %d commands\n"

#endif