/**
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_stm32f3
 * @{
 *
 * @file
 * @brief           Memory definitions for the STM32F303CC
 *
 * @author          Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

MEMORY
{
    rom (rx)    : ORIGIN = 0x08000000, LENGTH = 256K
    ram (rwx)   : ORIGIN = 0x20000000, LENGTH = 40K
    ccmram (rwx): ORIGIN = 0x10000000, LENGTH = 8K
    cpuid (r)   : ORIGIN = 0x1FFFF7AC, LENGTH = 12
}

_cpuid_address = ORIGIN(cpuid);

INCLUDE cortexm_base.ld
