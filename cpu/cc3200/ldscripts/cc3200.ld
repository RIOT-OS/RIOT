/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc3200
 * @ingroup         drivers_periph_uart
 * @{
 *
 * @file
 * @brief           Sections definitions for the CC3200
 *
* @author           Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 * @}
 */

/** The CC3200 bootloader does not support executing user binaries directly from flash. Instead 
 * the entire executable is copied to ram for execution by the bootloader from mcuimg.bin file stored on the MCUs filesystem. 
 */ 
MEMORY
{
    rom (rx)    : ORIGIN = 0x20004000, LENGTH = 32K
    ram (w!rx)  : ORIGIN = 0x2000C000, LENGTH = 240K - 32K
}

INCLUDE cortexm_base.ld