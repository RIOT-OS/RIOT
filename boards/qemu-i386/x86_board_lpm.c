/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_qemu-i386
 * @{
 *
 * @file
 * @brief       Low-power mode emulation for qemu-i386.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "lpm.h"
#include "x86_reboot.h"

void lpm_init(void)
{
    /* void */
}

enum lpm_mode lpm_set(enum lpm_mode target)
{
    if (target != LPM_ON) {
        if (target == LPM_POWERDOWN) {
            x86_shutdown();
        }
        __asm__ volatile ("hlt");
    }
    return LPM_UNKNOWN;
}

void lpm_awake(void)
{
    /* void */
}

void lpm_begin_awake(void)
{
    /* void */
}

void lpm_end_awake(void)
{
    /* void */
}
