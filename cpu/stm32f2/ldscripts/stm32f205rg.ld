/*
 * Copyright (C) 2015 Engineering-Spirit
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_stm32f2
 * @{
 *
 * @file
 * @brief           Memory definitions for the STM32F205RG
 *
 * @author          Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author          Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

MEMORY
{
    rom (rx)        : ORIGIN = 0x08000000, LENGTH = 1024K
    ram (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
}

INCLUDE cortexm_base.ld
