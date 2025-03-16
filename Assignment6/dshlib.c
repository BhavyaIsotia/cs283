#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

#define CMD_ERR_EXECUTE "error: failed to execute command"

/**
 * Parses a single command from cmd_line into cmd_buff.
 */
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    if (!cmd_line || !cmd_buff) return ERR_MEMORY;

    cmd_buff->_cmd_buffer = strdup(cmd_line);
    if (!cmd_buff->_cmd_buffer) return ERR_MEMORY;

    cmd_buff->argc = 0;
    char *p = cmd_buff->_cmd_buffer;
    bool in_quotes = false;

    while (*p) {
        while (*p == ' ') p++;
        if (*p == '\0') break;
        if (*p == '"') { in_quotes = true; p++; }

        cmd_buff->argv[cmd_buff->argc++] = p;
        
        while (*p && (in_quotes || (*p != ' '))) {
            if (*p == '"') { in_quotes = false; *p = '\0'; break; }
            p++;
        }
        
        if (*p) { *p = '\0'; p++; }
    }

    cmd_buff->argv[cmd_buff->argc] = NULL;
    return cmd_buff->argc > 0 ? OK : WARN_NO_CMDS;
}

/**
 * Parses a pipeline command into a command list.
 */
int parse_pipeline(char *cmd_line, command_list_t *cmd_list) {
    if (!cmd_line || !cmd_list) return ERR_MEMORY;

    char *token;
    char *saveptr;
    int count = 0;

    token = strtok_r(cmd_line, "|", &saveptr);
    while (token != NULL) {
        while (*token == ' ') token++;  

        if (strlen(token) == 0) return ERR_TOO_MANY_COMMANDS;
        if (count >= CMD_MAX) return ERR_TOO_MANY_COMMANDS;

        cmd_list->commands[count]._cmd_buffer = strdup(token);
        if (!cmd_list->commands[count]._cmd_buffer) return ERR_MEMORY;

        if (build_cmd_buff(cmd_list->commands[count]._cmd_buffer, &cmd_list->commands[count]) != OK) {
            return ERR_CMD_ARGS_BAD;
        }

        count++;
        token = strtok_r(NULL, "|", &saveptr);
    }

    cmd_list->num = count;
    return OK;
}

/**
 * Executes a single command.
 */
int exec_cmd(cmd_buff_t *cmd) {
    if (cmd->argc == 0) return WARN_NO_CMDS;

    pid_t pid = fork();
    if (pid < 0) {
        perror("[ERROR] Fork failed");
        return ERR_EXEC_CMD;
    } else if (pid == 0) {  
        execvp(cmd->argv[0], cmd->argv);
        perror(CMD_ERR_EXECUTE);
        exit(ERR_EXEC_CMD);
    }

    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : ERR_EXEC_CMD;
}

/**
 * Executes a pipeline of commands.
 */
 int execute_pipeline(command_list_t *cmd_list) {
    if (!cmd_list || cmd_list->num == 0) return ERR_MEMORY;

    int pipes[cmd_list->num - 1][2];
    pid_t pids[cmd_list->num];

    // Create pipes
    for (int i = 0; i < cmd_list->num - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("[ERROR] Pipe creation failed");
            return ERR_EXEC_CMD;
        }
    }

    // Fork children
    for (int i = 0; i < cmd_list->num; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("[ERROR] Fork failed");
            return ERR_EXEC_CMD;
        } else if (pids[i] == 0) {  
            // Redirect input
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            // Redirect output
            if (i < cmd_list->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close all pipes in child process
            for (int j = 0; j < cmd_list->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            
            fflush(stdout);

            if (execvp(cmd_list->commands[i].argv[0], cmd_list->commands[i].argv) == -1) {
                fprintf(stderr, "[ERROR] execvp failed: %s\n", cmd_list->commands[i].argv[0]);
                exit(ERR_EXEC_CMD);
            }
        }
    }

    // Close all pipes in parent process
    for (int i = 0; i < cmd_list->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all children
    for (int i = 0; i < cmd_list->num; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return OK;
}

/**
 * Main command loop.
 */
 int exec_local_cmd_loop() {
    char cmd_buff[SH_CMD_MAX];
    command_list_t cmd_list;
    int rc;
    bool is_interactive = isatty(STDIN_FILENO);

    while (1) {
    
        if (is_interactive) {
            printf("%s", SH_PROMPT);
            fflush(stdout);
        }

        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            break;
        }

        // Remove the trailing newline from input
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
        if (strlen(cmd_buff) == 0) continue;

        if (strcmp(cmd_buff, EXIT_CMD) == 0) break;

        rc = parse_pipeline(cmd_buff, &cmd_list);
        if (rc != OK) continue;

        if (cmd_list.num == 1) {
            if (strcmp(cmd_list.commands[0].argv[0], "cd") == 0) {
                const char *dir = (cmd_list.commands[0].argc > 1) ? cmd_list.commands[0].argv[1] : getenv("HOME");
                if (chdir(dir) != 0) {
                    perror("cd failed");
                }
            } else {
                exec_cmd(&cmd_list.commands[0]);
            }
        } else {
            execute_pipeline(&cmd_list);
        }

    
        if (is_interactive) {
            printf("%s", SH_PROMPT);
            fflush(stdout);
        }
    }

    return OK;
}
