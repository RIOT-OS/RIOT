/**
 * Shell commands for configuring the node id
 *
 * Copyright (C) 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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

void _id_handler(char *id)
{
    long newid;

    newid = atoi(id + 3);

    if (strlen(id) < 3) {
        printf("Current id: %u\n", sysconfig.id);
    }
    else {
        printf("Setting new id %lu\n", newid);
        sysconfig.id = newid;

        if (!config_save()) {
            puts("ERROR setting new id");
        }
    }
}
