/*
 * Copyright (C) 2016 OTA keys
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_stm32f0
 * @{
 *
 * @file
 * @brief           Memory definitions for the STM32F042K6
 *
 * @author          Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

MEMORY
{
    rom (rx)    : ORIGIN = 0x08000000, LENGTH = 32K
    ram (rwx)   : ORIGIN = 0x20000000, LENGTH = 6K
}

INCLUDE cortexm_base.ld
