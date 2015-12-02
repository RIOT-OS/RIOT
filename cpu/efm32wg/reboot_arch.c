/*
 * reboot_arch.c
 *
 *  Created on: 8 wrz 2015
 *      Author: kkk
 */

/**
 * @ingroup     cpu_efm32
 * @{
 *
 * @file
 * @brief       Implementation of the kernels reboot interface
 *
 * @author      Mateusz Kubaszek <mathir.km.riot@gmail.com>
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


