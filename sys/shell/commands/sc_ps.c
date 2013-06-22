/**
 * Shell commands for ps
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup shell_commands
 * @{
 * @file    sc_ps.c
 * @brief   shows all thread information 
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include "ps.h"

void _ps_handler(char *unnused)
{
    thread_print_all();
}

