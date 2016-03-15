/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_stm32f1
 * @{
 *
 * @file
 * @brief           Memory definitions for the STM32F101RD
 *
 * @author          Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

MEMORY
{
    rom (rx)    : ORIGIN = 0x08000000, LENGTH = 384K
    ram (xrw)   : ORIGIN = 0x20000000, LENGTH = 48K
    cpuid (r)   : ORIGIN = 0x1ffff7e8, LENGTH = 12
}

_cpuid_address = ORIGIN(cpuid);

INCLUDE cortexm_base.ld
