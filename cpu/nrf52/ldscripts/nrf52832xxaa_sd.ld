/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_nrf52
 * @{
 *
 * @file
 * @brief           Memory definitions for the NRF52832XXAA
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

MEMORY
{
    rom (rx)    : ORIGIN =    0x1f000, LENGTH = 0x61000
    ram (w!rx)  : ORIGIN = 0x20002800, LENGTH = 0xD800
}

INCLUDE cortexm_base.ld
