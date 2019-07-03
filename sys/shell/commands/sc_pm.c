/*
 * Copyright (C) 2019 Thomas Stilwell <stilwellt@openlabs.co>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell command to interact with the PM subsystem
 *
 * @author      Thomas Stilwell <stilwellt@openlabs.co>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include "pm_layered.h"

/* TODO deduplicate this definition with the one in sys/pm_layered/pm.c */
typedef union {
    uint32_t val_u32;
    uint8_t val_u8[PM_NUM_MODES];
} pm_blocker_t;

extern volatile pm_blocker_t pm_blocker; /* sys/pm_layered/pm.c */

static void _print_usage(void) {
    printf("usage: pm show: display current blockers for each power mode\n");
}

int _pm(int argc, char **argv)
{
    if (argc != 2) {
        _print_usage();
        return 1;
    }

    if (strcmp(argv[1], "show")) {
        _print_usage();
        return 2;
    }

    uint8_t lowest_allowed_mode = 0;

    for (unsigned i = 0; i < PM_NUM_MODES; i++) {
        printf("mode %u blockers: %u \n", i, pm_blocker.val_u8[i]);
        if (pm_blocker.val_u8[i] == 1) {
            lowest_allowed_mode = i + 1;
        }
    }

    printf("lowest allowed mode: %u\n", lowest_allowed_mode);

    return 0;
}
