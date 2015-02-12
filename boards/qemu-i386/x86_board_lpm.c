/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
        asm volatile ("hlt");
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
