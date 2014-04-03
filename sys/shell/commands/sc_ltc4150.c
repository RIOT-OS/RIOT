/**
 * Shell commands for coulomb counter
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup shell_commands
 * @{
 * @file    sc_ltc4150.c
 * @brief   provides shell commands to access ltc4150
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include "ltc4150.h"

void _get_current_handler(char *unused)
{
    (void) unused;

    printf("Power usage: %.4f mA (%.4f mA avg/ %.4f mAh total / %i usec)\n", ltc4150_get_current_mA(), ltc4150_get_avg_mA(), ltc4150_get_total_mAh(), ltc4150_get_interval());
}

void _reset_current_handler(char *unused)
{
    (void) unused;

    ltc4150_start();
}
