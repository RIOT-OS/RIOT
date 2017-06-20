/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Serial port configuration
 * @{
 */
#define UART_PORT (COM1_PORT) /* IO port to use for UART */
#define UART_IRQ (COM1_IRQ)   /* IRQ line to use for UART */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */

/**
 * @}
 */
