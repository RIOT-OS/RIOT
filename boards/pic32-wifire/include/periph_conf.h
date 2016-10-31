/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 */

/**
 * @defgroup    boards_pic32-wifire Digilent PIC32 Wifire
 * @ingroup     boards
 * @brief       peripheral configuration for the Digilent PIC32 Wifire
 * @{
 *
 * @file
 * @brief       peripheral configuration for the Digilent PIC32 Wifire
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
  *          There are 6 UARTS available on this CPU.
  *          We route debug via UART4 on this board,
  *          this is the UART connected to the FTDI USB <-> UART device.
  *
  *          Note Microchip number the UARTS 1->4.
  * @{
  */
#define UART_NUMOF          (6)
#define DEBUG_VIA_UART      (4)
#define DEBUG_UART_BAUD     (9600)
/** @} */

#ifdef __cplusplus
}
#endif

#endif
/** @} */

