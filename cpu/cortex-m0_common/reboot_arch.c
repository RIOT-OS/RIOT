/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm0_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels reboot interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "arch/reboot_arch.h"
#include "cpu.h"


int reboot_arch(int mode)
{
    printf("Going into reboot, mode %i\n", mode);

    NVIC_SystemReset();

    return 0;
}
