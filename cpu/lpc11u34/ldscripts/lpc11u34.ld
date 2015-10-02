/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_lpc11u34
 * @{
 *
 * @file
 * @brief           Memory definitions for the LPC11U34
 *
 * @author          Paul RATHGEB <paul.rathgeb@skynet.be>
 *
 * @}
 */

MEMORY
{
  rom (rx)	: ORIGIN = 0x0, LENGTH = 0xc000 /* 48K bytes */
  ram (rwx)	: ORIGIN = 0x10000000, LENGTH = 0x2000 /* 8K bytes */
  usb_ram (rwx) : ORIGIN = 0x20004000, LENGTH = 0x800 /* 2K bytes */
}

INCLUDE cortexm_base.ld
