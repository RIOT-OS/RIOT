/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     boards_mips-malta
 * @{
 *
 * @file
 * @brief       Board specific definitions for the MIPS Malta FPGA System.
 *
 * @author      Neil Jones <neil.jones@imgtec.com>
 */

#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Set how many increments of the count register per uS
 *              needed by timer code
 */
#define TICKS_PER_US (15)

/**
 * @brief       Board level initialization
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
