/*
 * Copyright (C) 2015 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_stm32l1
 * @{
 *
 * @file
 * @brief           Memory definitions for the STM32L151RC
 *
 * @author          Katja Kirstein <katja.kirstein@haw-hamburg.de>
 *
 * @}
 */

MEMORY
{
    rom (rx)        : ORIGIN = 0x08000000, LENGTH = 256K
    ram (xrw)       : ORIGIN = 0x20000000, LENGTH = 32K
    cpuid (r)       : ORIGIN = 0x1ff800d0, LENGTH = 12
}

_cpuid_address = ORIGIN(cpuid);

INCLUDE cortexm_base.ld
