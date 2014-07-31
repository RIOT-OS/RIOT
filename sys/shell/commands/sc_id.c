/**
 * Shell commands for configuring the node id
 *
 * Copyright (C) 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup shell_commands
 * @{
 * @file    sc_id.c
 * @brief   provides shell commands to configure node id
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

void _id_handler(int argc, char **argv)
{
    if (argc < 2) {
        printf("Current id: %u\n", sysconfig.id);
    }
    else {
        long newid = atoi(argv[1]);
        printf("Setting new id %ld\n", newid);
        sysconfig.id = newid;

        if (!config_save()) {
            puts("ERROR setting new id");
        }
    }
}
