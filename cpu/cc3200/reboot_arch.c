/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file
 * @brief       Implementation of the kernels reboot interface
 *
 * @author      Attilio Dona'
 *
 * @}
 */

#include <stdio.h>

#include "arch/reboot_arch.h"
#include "cpu.h"

int reboot_arch(int mode) {
    printf("Going into reboot, mode %i\n", mode);
    /* wait a while to make sure the printf is finished */
    volatile int n = 100000;
    while (n--)
        ;
    PRCMMCUReset(1);
    return -1;
}
