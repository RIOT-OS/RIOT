/*
 * Copyright (C) 2017 Inria
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
 * @brief           Memory definitions for the STM32F303ZE
 *
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

MEMORY
{
    rom (rx)    : ORIGIN = 0x08000000, LENGTH = 512K
    ram (rwx)   : ORIGIN = 0x20000000, LENGTH = 64K
    ccmram (rwx): ORIGIN = 0x10000000, LENGTH = 16K
}

INCLUDE cortexm_base.ld
