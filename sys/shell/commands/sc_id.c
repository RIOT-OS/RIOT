/**
 * Shell commands for configuring the node id 
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
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
#include <config.h>
#include <stdlib.h>

void _id_handler(char *id)
{
    long newid;

    newid = atoi(id + 3);

    if(strlen(id) < 3) {
#ifdef MODULE_CONFIG
        printf("Current id: %u\n", sysconfig.id);
#endif
    }
    else {
        printf("Setting new id %lu\n", newid);
#ifdef MODULE_CONFIG
        sysconfig.id = newid;

        if(!config_save()) {
            puts("ERROR setting new id");
        }

#endif
    }
}
