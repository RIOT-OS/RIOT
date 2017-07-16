/*
 * Copyright (C) 2017 Inria
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
 * @brief           Memory definitions for the STM32F103RE Slot 1
 *
 * @author          Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

MEMORY
{
    rom (rx)        : ORIGIN = 0x08004100, LENGTH = 0x3BF00
    ram (xrw)       : ORIGIN = 0x20000000, LENGTH = 64K
    cpuid (r)       : ORIGIN = 0x1ffff7e8, LENGTH = 12
}

_cpuid_address = ORIGIN(cpuid);

INCLUDE cortexm_base.ld
