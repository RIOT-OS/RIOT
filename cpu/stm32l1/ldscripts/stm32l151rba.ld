/*
 * Copyright (C) 2015 Engineering-Spirit
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
 * @brief           Memory definitions for the STM32L151RB-A
 *
 * @author          Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

MEMORY
{
    rom (rx)        : ORIGIN = 0x08000000, LENGTH = 128K
    ram (rw)        : ORIGIN = 0x20000000, LENGTH = 32K

    /* see STM32L1 Reference Manual (31.2 Unique device ID registers (96 bits))
     *    Base address:
     *      - 0x1FF80050 for Cat.1 and Cat.2 devices
     *      - 0x1FF800D0 for Cat.3, Cat.4, Cat.5 and Cat.6 devices
     */
    cpuid (r)       : ORIGIN = 0x1ff80050, LENGTH = 12
}

_cpuid_address = ORIGIN(cpuid);

INCLUDE cortexm_base.ld
