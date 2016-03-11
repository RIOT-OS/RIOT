/*
 * Copyright (C) 2015 TriaGnoSys GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_nucleo-f103 Nucleo-F103
 * @ingroup     boards
 * @brief       Board specific files for the nucleo-f103 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nucleo-f103 board
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Use the 2nd UART for STDIO on this board
 */
#define UART_STDIO_DEV      UART_DEV(1)

/**
 * @name xtimer configuration
 */
#define XTIMER_MASK         (0xffff0000)
#define XTIMER_BACKOFF      5
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
