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
 * @defgroup    boards_mips-malta MIPS MALTA
 * @ingroup     boards
 * @brief       peripheral configuration for the MIPS Malta FPGA system
 * @{
 *
 * @file
 * @brief       peripheral configuration for the MIPS Malta FPGA system
 *
 * @author      Neil Jones <neil.jones@imgtec.com>
 */

#ifndef _PERIPH_CONF_H_
#define _PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Timer definitions
 * @{
 */
#define TIMER_NUMOF         (1)
#define TIMER_0_CHANNELS    (3)
/** @} */

/**
 * @brief No UART driver for this board currently
 *        Note this value must be set though (to 0)
 */
#define UART_NUMOF          (0)

/**
 * @brief Enable DSP context save + restore.
 */
#define MIPS_DSP            (1)

/**
 * @brief Enable FPU context save + restore.
 */
#define MIPS_HARD_FLOAT     (1)

#ifdef __cplusplus
}
#endif

#endif /*_PERIPH_CONF_H_*/
/** @} */
