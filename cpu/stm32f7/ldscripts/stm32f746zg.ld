/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_stm32l4
 * @{
 *
 * @file
 * @brief           Memory definitions for the STM32F746ZG
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

MEMORY
{
    rom (rx)    : ORIGIN = 0x08000000, LENGTH = 1024K
    ram (rwx)   : ORIGIN = 0x20000000, LENGTH = 320K
}

INCLUDE cortexm_base.ld
