/*
 * Copyright (C) 2016 RWTH Aachen, Steffen Robertz
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
 * @brief       Hello World application
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @}
 */
#include <stdio.h>

#include "board.h"
#include "rgbled.h"
#include "shell.h"

extern int rgb_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "rgbled", "set RGB LED to different colors", rgb_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Pinoccio PWM LED Shell!");
    PORTB &= ~BLUE;
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    puts("Error: END of main().");
    /* should be never reached */


    return 0;
}
