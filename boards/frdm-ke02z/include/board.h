/*
 * Copyright (C) 2018 Ishraq Ibne Ashraf
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_frdm-ke02z NXP FRDM-KE02Z Board
 * @ingroup     boards
 * @brief       Support for the NXP FRDM-KE02Z
 * @{
 *
 * @file
 * @brief       Board specific definitions for the FRDM-KE02Z
 *
 * @author      Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Use the second UART for STDIO on this board
 */
#define UART_STDIO_DEV UART_DEV(1)

/**
 * @brief Initialize board specific hardware
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
