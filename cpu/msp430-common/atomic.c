/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu
 * @{
 *
 * @file        atomic.c
 * @brief       atomic set and return function
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include "atomic.h"
#include "cpu.h"

unsigned int atomic_set_return(unsigned int *val, unsigned int set)
{
    dINT();
    unsigned int old_val = *val;
    *val = set;
    eINT();
    return old_val;
}
