/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_xmc1000
 * @{
 *
 * @file
 * @brief           Memory definitions for the XMC1100
 *
 * @author          Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

MEMORY
{
        rom(rx)  : ORIGIN = 0x10001000, LENGTH = 64K
        ram(rwx) : ORIGIN = 0x20000000, LENGTH = 16K
}

INCLUDE cortexm_base.ld
