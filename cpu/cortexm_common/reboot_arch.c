/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels reboot interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "arch/reboot_arch.h"
#include "cpu.h"

int reboot_arch(int mode)
{
    printf("Going into reboot, mode %i\n", mode);
    /* wait a while to make sure the printf is finished */
    volatile int n = 100000;
    while(n--);
    NVIC_SystemReset();
    return -1;
}
