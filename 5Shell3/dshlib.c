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


int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    if (!cmd_line || !cmd_buff) return ERR_MEMORY;

    printf("[DEBUG] Parsing command: %s\n", cmd_line);  

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

    printf("[DEBUG] Parsed command: ");
    for (int i = 0; i < cmd_buff->argc; i++) {
        printf("'%s' ", cmd_buff->argv[i]);
    }
    printf("\n");

    return cmd_buff->argc > 0 ? OK : WARN_NO_CMDS;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff == NULL) return -1;
    if (cmd_buff->_cmd_buffer != NULL) free(cmd_buff->_cmd_buffer);
    return 0;
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, EXIT_CMD) == 0) return BI_CMD_EXIT;
    if (strcmp(input, "cd") == 0) return BI_CMD_CD;
    return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (cmd->argc == 0) return BI_NOT_BI;
    
    printf("[DEBUG] Checking for built-in command: %s\n", cmd->argv[0]);

    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);
    if (cmd_type == BI_CMD_EXIT) {
        printf("[DEBUG] Exiting shell\n");
        exit(0);
    }
    else if (cmd_type == BI_CMD_CD) {
        if (cmd->argc > 1) {
            if (chdir(cmd->argv[1]) != 0) {
                perror("[ERROR] cd failed");
            }
        }
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}

int exec_local_cmd_loop() {
    char cmd_buff[SH_CMD_MAX];
    cmd_buff_t cmd;
    int rc;
    
    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n[DEBUG] EOF received, exiting shell.\n");
            break;
        }
        
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';  
        if (strlen(cmd_buff) == 0) continue;

        printf("[DEBUG] Received command: %s\n", cmd_buff);

        rc = build_cmd_buff(cmd_buff, &cmd);
        if (rc == WARN_NO_CMDS || rc == ERR_TOO_MANY_COMMANDS) continue;

        if (exec_built_in_cmd(&cmd) == BI_EXECUTED) {
            free_cmd_buff(&cmd);
            continue;
        }

        if (strchr(cmd_buff, PIPE_CHAR) != NULL) {
            printf("[DEBUG] Pipeline detected\n");
            command_list_t cmd_list;
            rc = parse_pipeline(cmd_buff, &cmd_list);
            if (rc != OK) continue;
            execute_pipeline(&cmd_list);
        } else {
            pid_t pid = fork();
            if (pid < 0) {
                perror("[ERROR] Fork failed");
                free_cmd_buff(&cmd);
                exit(ERR_EXEC_CMD);
            } else if (pid == 0) {  
                printf("[DEBUG] Executing command: %s\n", cmd.argv[0]);
                execvp(cmd.argv[0], cmd.argv);
                perror(CMD_ERR_EXECUTE);
                exit(ERR_EXEC_CMD);
            } else {
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status)) {
                    rc = WEXITSTATUS(status);
                    printf("[DEBUG] Child process exited with status: %d\n", rc);
                }
            }
        }
        free_cmd_buff(&cmd);
    }
    return OK;
}

int parse_pipeline(char *cmd_line, command_list_t *cmd_list) {
    if (!cmd_line || !cmd_list) return ERR_MEMORY;

    printf("[DEBUG] Parsing pipeline: %s\n", cmd_line);

    char *token;
    char *saveptr;
    int count = 0;

    token = strtok_r(cmd_line, "|", &saveptr);
    while (token != NULL) {
        while (*token == ' ') token++;  
        if (strlen(token) == 0) return ERR_TOO_MANY_COMMANDS;

        printf("[DEBUG] Pipeline command: %s\n", token);

        cmd_list->commands[count]._cmd_buffer = strdup(token);
        if (!cmd_list->commands[count]._cmd_buffer) return ERR_MEMORY;

        cmd_list->commands[count].argc = 0;
        char *arg = strtok(cmd_list->commands[count]._cmd_buffer, " ");
        while (arg != NULL) {
            cmd_list->commands[count].argv[cmd_list->commands[count].argc++] = arg;
            arg = strtok(NULL, " ");
        }
        cmd_list->commands[count].argv[cmd_list->commands[count].argc] = NULL;

        count++;
        if (count >= CMD_MAX) return ERR_TOO_MANY_COMMANDS;
        token = strtok_r(NULL, "|", &saveptr);
    }

    cmd_list->cmd_count = count;
    return OK;
}

int execute_pipeline(command_list_t *cmd_list) {
    if (!cmd_list || cmd_list->cmd_count == 0) return ERR_MEMORY;

    printf("[DEBUG] Executing pipeline with %d commands\n", cmd_list->cmd_count);

    int pipes[cmd_list->cmd_count - 1][2];
    pid_t pids[cmd_list->cmd_count];

    for (int i = 0; i < cmd_list->cmd_count - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("[ERROR] Pipe creation failed");
            return ERR_EXEC_CMD;
        }
    }

    for (int i = 0; i < cmd_list->cmd_count; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("[ERROR] Fork failed");
            return ERR_EXEC_CMD;
        } else if (pids[i] == 0) {  
            if (i > 0) dup2(pipes[i - 1][0], STDIN_FILENO);
            if (i < cmd_list->cmd_count - 1) dup2(pipes[i][1], STDOUT_FILENO);

            for (int j = 0; j < cmd_list->cmd_count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            printf("[DEBUG] Executing pipeline command: %s\n", cmd_list->commands[i].argv[0]);
            execvp(cmd_list->commands[i].argv[0], cmd_list->commands[i].argv);
            perror("[ERROR] execvp failed");
            exit(1);
        }
    }

    for (int i = 0; i < cmd_list->cmd_count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < cmd_list->cmd_count; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    return OK;
}
