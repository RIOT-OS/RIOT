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
 * @brief           Memory definitions for the STM32F103RE with bootloader
 *
 * @author          Gaëtan Harter <gaetan.harter@inria.fr>
 *
 * @}
 */

MEMORY
{
    rom (rx)        : ORIGIN = 0x08000000, LENGTH = 16K
    romslot1 (rx)   : ORIGIN = 0x08004000, LENGTH = 0x3C000
    romslot2 (rx)   : ORIGIN = 0x08040000, LENGTH = 0x3C000
    ram (xrw)       : ORIGIN = 0x20000000, LENGTH = 64K
    cpuid (r)       : ORIGIN = 0x1ffff7e8, LENGTH = 12
}

_cpuid_address = ORIGIN(cpuid);

INCLUDE cortexm_base.ld

SECTIONS
{
    . = ALIGN(0x1000);
    .slot.1 :
    {
        KEEP(*(.slot.1.*))
    } > romslot1

    . = ALIGN(0x1000);
    .slot.2 :
    {
        KEEP(*(.slot.2.*))
    } > romslot2
}
