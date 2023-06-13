/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430_x1xx
 * @{
 *
 * @file
 * @brief       Cortex CMSIS style definition of MSP430 registers
 *
 * @todo        This file is incomplete, not all registers are listed. Further
 *              There are probably some inconsistencies throughout the MSP430
 *              family which need to be addressed.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef X1XX_MSP430_REGS_H
#define X1XX_MSP430_REGS_H

#include <stdint.h>

#include "msp430_regs_common.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief   USART (UART, SPI and I2C) registers
 */
typedef struct {
    REG8    CTL;        /**< USART control */
    REG8    TCTL;       /**< transmit control */
    REG8    RCTL;       /**< receive control */
    REG8    MCTL;       /**< modulation control */
    REG8    BR0;        /**< baud rate control 0 */
    REG8    BR1;        /**< baud rate control 1 */
    REG8    RXBUF;      /**< receive buffer */
    REG8    TXBUF;      /**< transmit buffer */
} msp_usart_t;

/**
 * @name    USART control register bitmap
 * @{
 */
#define USART_CTL_SWRST             (0x01)
#define USART_CTL_MM                (0x02)
#define USART_CTL_SYNC              (0x04)
#define USART_CTL_LISTEN            (0x08)
#define USART_CTL_CHAR              (0x10)
#define USART_CTL_SPB               (0x20)
#define USART_CTL_PEV               (0x40)
#define USART_CTL_PENA              (0x80)
/** @} */

/**
 * @name    USART transmit control register bitmap
 * @{
 */
#define USART_TCTL_TXEPT            (0x01)
#define USART_TCTL_STC              (0x02)
#define USART_TCTL_TXWAKE           (0x04)
#define USART_TCTL_URXSE            (0x08)
#define USART_TCTL_SSEL_MASK        (0x30)
#define USART_TCTL_SSEL_UCLKI       (0x00)
#define USART_TCTL_SSEL_ACLK        (0x10)
#define USART_TCTL_SSEL_SMCLK       (0x20)
#define USART_TCTL_CKPL             (0x40)
#define USART_TCTL_CKPH             (0x80)
/** @} */

/**
 * @name    USART receive control register bitmap
 * @{
 */
#define USART_RCTL_RXERR            (0x01)
#define USART_RCTL_RXWAKE           (0x02)
#define USART_RCTL_URXWIE           (0x04)
#define USART_RCTL_URXEIE           (0x08)
#define USART_RCTL_BRK              (0x10)
#define USART_RCTL_OE               (0x20)
#define USART_RCTL_PE               (0x40)
#define USART_RCTL_FE               (0x80)
/** @} */

/**
 * @name    Base register address definitions
 * @{
 */
#define USART_0_BASE            ((uint16_t)0x0070)
#define USART_1_BASE            ((uint16_t)0x0078)
/** @} */

/**
 * @name    Typing of base register objects
 * @{
 */
#define USART_0                 ((msp_usart_t *)USART_0_BASE)
#define USART_1                 ((msp_usart_t *)USART_1_BASE)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* X1XX_MSP430_REGS_H */
/** @} */
