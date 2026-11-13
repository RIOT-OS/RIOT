// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
/*
 * hal_platform.c
 *
 * Description: contains the POSIX implementation of the hardware
 * abstraction layer interface for platform-specific functionality
 *
 */

#include "platform/hal_io.h"
#include "platform/hal_platform.h"
#include "platform/hal_time.h"
#include "platform/hal_task.h"

#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char **restart_args;

void hal_time_init(void); // not declared in public header

void hal_platform_init(int argc, char *argv[])
{
    hal_io_init();
    hal_time_init(); // required for logging
    restart_args = malloc(sizeof(char *) * argc);
    if (restart_args) {
        // Copy all commandline args to the restart argument buffer
        for (int i = 1; i < argc; i++) {
            restart_args[i - 1] = strdup(argv[i]);
        }
        // NULL-terminate the array
        restart_args[argc - 1] = NULL;
    }
    else {
        UD3TN_LOG_ERROR("Error: Cannot allocate memory for restart buffer");
    }
}
