/**
 * Shell commands for ps
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup shell_commands
 * @{
 * @file    sc_ps.c
 * @brief   shows all thread information
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include "ps.h"

int _ps_handler(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    thread_print_all();

    return 0;
}
