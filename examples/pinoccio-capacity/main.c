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
 * @brief       Test Measurement of a capacity
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @}
 */
#include "shell.h"
#include <stdio.h>
#include "board.h"

#include "stdlib.h"

extern int capacity_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
	{ "capacity", "measure capacity of a capacitor", capacity_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
	LED_PORT &= ~RED;
	char line_buf[SHELL_DEFAULT_BUFSIZE];

	shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    puts("Error: END of main().");
	/* should be never reached */
}
