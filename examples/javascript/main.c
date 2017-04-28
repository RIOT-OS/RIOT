/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Showing an example of scripting (javascript) from command line
 *
 * @author      Emmanuel Baccelli <emmanuel.baccelli@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include "shell.h"
#include "jerryscript.h"

int shell_script(int argc, char **argv)
{
    if (argc < 2) {
        puts("Usage: script <your script here!> \n"
        "For example, try: \n"
        "script var txt = \\'\\'; txt += Math.PI; print (\\'Pi=\\'+txt); \n"
        "Note: you need to substitute usual quotes with \\' \n");
        return -1;
    }

    jerry_char_t script[(2 * SHELL_DEFAULT_BUFSIZE + 1)];
    *script = '\0';
    for(int i = 1; i < argc; i++) {
        if (i>1) {
            strcat((char *)script, " ");
        }
        strcat((char *)script, argv[i]);
    }

    size_t script_size = strlen((char *) script);
    printf("Executing script: [%s]\n\n", script);
    bool ret_value = jerry_run_simple(script, script_size, JERRY_INIT_EMPTY);

    return (ret_value != 0);
}

const shell_command_t shell_commands[] = {
    { "script", "Shell scripting ", shell_script },
    { NULL, NULL, NULL }
};

int main(void)
{
    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    /* start the shell */
    char line_buf[2 * SHELL_DEFAULT_BUFSIZE];
    /* for longer script support shell buffer should be bigger */
    shell_run(shell_commands, line_buf, sizeof(line_buf));

    return 0;
}
