#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dshlib.h"

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    memset(clist, 0, sizeof(command_list_t));

    if (cmd_line == NULL || strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }

    char *cmd, *saveptr;
    int cmd_count = 0;

    cmd = strtok_r(cmd_line, "|", &saveptr);
    while (cmd != NULL) {
        while (*cmd == ' ') cmd++; 

        char *end = cmd + strlen(cmd) - 1;
        while (end > cmd && *end == ' ') {
            *end = '\0';
            end--;
        }

        if (cmd_count >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        if (strlen(cmd) == 0) {
            return WARN_NO_CMDS;
        }

        command_t *current_cmd = &clist->commands[cmd_count];

        char *token, *arg_saveptr;
        token = strtok_r(cmd, " ", &arg_saveptr);
        if (token == NULL) {
            return WARN_NO_CMDS;
        }

        if (strlen(token) >= EXE_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        strncpy(current_cmd->exe, token, EXE_MAX - 1);
        current_cmd->exe[EXE_MAX - 1] = '\0';

        if (arg_saveptr != NULL && strlen(arg_saveptr) >= ARG_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        if (arg_saveptr != NULL) {
            strncpy(current_cmd->args, arg_saveptr, ARG_MAX - 1);
            current_cmd->args[ARG_MAX - 1] = '\0';
        } else {
            current_cmd->args[0] = '\0';
        }

        cmd_count++;
        cmd = strtok_r(NULL, "|", &saveptr);
    }

    clist->num = cmd_count;
    return OK;
}
