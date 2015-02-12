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
 * @defgroup    boards_qemu-i386 qemu-i386
 * @ingroup     boards
 * @brief       Dummy board to run x86 port in QEMU
 * @{
 *
 * @file
 * @brief       Board specific defines for qemu-i386.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef __RIOT__BOARDS__QEMU_I386__BOARD__H
#define __RIOT__BOARDS__QEMU_I386__BOARD__H

#ifdef __cplusplus
extern "C" {
#endif

#define UART_PORT (COM1_PORT) /* IO port to use for UART */
#define UART_IRQ (COM1_IRQ)   /* IRQ line to use for UART */

#define LED_RED_ON          /* not available */
#define LED_RED_OFF         /* not available */
#define LED_RED_TOGGLE      /* not available */

#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 */
