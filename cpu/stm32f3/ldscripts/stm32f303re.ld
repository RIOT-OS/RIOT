/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2014 Hamburg University of Applied Sciences
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
 * @brief           Memory definitions for the STM32F303RE
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Katja Kirstein <katja.kirstein@haw-hamburg.de>
 *
 * @}
 */

MEMORY
{
    rom (rx)    : ORIGIN = 0x08000000, LENGTH = 512K
    ram (rwx)   : ORIGIN = 0x20000000, LENGTH = 64K
    ccmram (rwx): ORIGIN = 0x10000000, LENGTH = 16K
    cpuid (r)   : ORIGIN = 0x1ffff7ac, LENGTH = 12
}

_cpuid_address = ORIGIN(cpuid);

INCLUDE cortexm_base.ld
