/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_lpc1768
 * @{
 *
 * @file
 * @brief           Memory definitions for the LPC1768
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

MEMORY
{
    usb_ram         : ORIGIN = 0x2007C000, LENGTH = 16K
    eth_ram         : ORIGIN = 0x20080000, LENGTH = 16K
}

SECTIONS
{
    .usb_ram (NOLOAD) :
    {
        *(.usb_ram .usb_ram.*)
    } > usb_ram

    .eth_ram (NOLOAD) :
    {
        *(.eth_ram .eth_ram.*)
    } > eth_ram
}

INCLUDE cortexm.ld
