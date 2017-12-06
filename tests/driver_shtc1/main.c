/*
 * Copyright (C) 2017 RWTH Aachen, Steffen Robertz, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the SHTC1 temperature and humidity sensor
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */
#include <stdio.h>
#include "shell.h"

extern int shtc1_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "shtc", "read values from a shtc1 sensor", shtc1_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{

    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    puts("Error: END of main().");
    /* should be never reached */
    return 0;
}
