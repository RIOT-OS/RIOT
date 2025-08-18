/*
 * SPDX-FileCopyrightText: 2020 Christian Amsüss
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @brief       Management of bootloader resets
 *
 * This implements a reset procedure read from the code of the particle device
 * OS at https://github.com/particle-iot/device-os/ (version
 * c21d5b8f39c8a013e19f904800808673f369c6e3), primarily from
 * hal/inc/syshealth_hal.h and its linker script.
 *
 * @file
 * @author      Christian Amsüss <chrysn@fsfe.org>
 *
 * @}
 */

#include <stdint.h>
#include "cpu.h"

/* from the eSystemHealth definition of hal/inc/syshealth_hal.h */
#define ENTER_DFU_APP_REQUEST (0xEDFA)

/* Start of the backup registers as per
 * build/arm/linker/nrf52840/platform_ram.ld and hal/src/nRF52840/core_hal.c */
#define BACKUP_REGISTERS ((volatile int32_t*)0x2003f380)

void usb_board_reset_in_bootloader(void)
{
    /* This *is* a write into an arbitrary memory location -- as RIOT does not
     * use any of the Particle system interrupt handlers any more, it's not
     * abiding by its RAM layout either. The write immediately preceding the
     * reboot should not have any effect on being-restarted system, but leaves
     * the flag in memory for the starting-up system to introspect.
     * */
    BACKUP_REGISTERS[0] = ENTER_DFU_APP_REQUEST;
    /* Going with a hard reset rather than a pm_off, as that might be altered
     * to do *anything* -- which is not safe any more now that we've forsaken
     * the RAM content */
    NVIC_SystemReset();
}
