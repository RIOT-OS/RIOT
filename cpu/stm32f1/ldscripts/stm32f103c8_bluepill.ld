/*
 * Copyright (C) 2015 Alexander Melnikov <avmelnikoff@gmail.com>
 *               2017 HAW Hamburg
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
 * @brief           Memory definitions for the STM32F103C8
 *
 * @author          Alexander Melnikov <avmelnikoff@gmail.com>
 * @author          Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

MEMORY
{
    rom (rx)    : ORIGIN = 0x08002000, LENGTH = 64K-0x2000
    ram (xrw)   : ORIGIN = 0x20000000, LENGTH = 20K
}

INCLUDE cortexm_base.ld
