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

rom_length = 512K;

INCLUDE multislot.ld

MEMORY
{
    rom (rx)    : ORIGIN = 0x00000000 + boot_offset, LENGTH = rom_length
    ram (w!rx)  : ORIGIN = 0x20000000,               LENGTH = 64K
}

INCLUDE cortexm_base.ld
