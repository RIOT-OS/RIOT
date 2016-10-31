/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_pic32-clicker MikroE PIC32 Clicker
 * @ingroup     boards
 * @brief       peripheral configuration for the MikroE PIC32 Clicker
 * @{
 *
 * @file
 * @brief       peripheral configuration for the MikroE PIC32 Clicker
 *
 * @author      Imagination Technologies.
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
  * @brief   UART Definitions
  *          There are 4 UARTS available on this CPU.
  *          We route debug via UART3 on this board,
  *          this is the UART connected to the MikroBUS
  *
  *          Note Microchip number the UARTS 1->4
  * @{
  */
#define UART_NUMOF          (4)
#define DEBUG_VIA_UART      (3)
#define DEBUG_UART_BAUD     (9600)
/** @} */

#ifdef __cplusplus
}
#endif

#endif
/** @} */

