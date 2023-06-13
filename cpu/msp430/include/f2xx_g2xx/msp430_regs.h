/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430_f2xx_g2xx
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

#ifndef F2XX_G2XX_MSP430_REGS_H
#define F2XX_G2XX_MSP430_REGS_H

#include <stdint.h>

#include "msp430_regs_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   USCI universal serial control interface registers
 */
typedef struct {
    REG8    ABCTL;      /**< auto baud rate control */
    REG8    IRTCTL;     /**< IrDA transmit control */
    REG8    IRRCTL;     /**< IrDA receive control */
    REG8    ACTL0;      /**< A control 0 */
    REG8    ACTL1;      /**< A control 1 */
    REG8    ABR0;       /**< A baud rate control 0 */
    REG8    ABR1;       /**< A baud rate control 1 */
    REG8    AMCTL;      /**< A modulation control */
    REG8    ASTAT;      /**< A status */
    REG8    ARXBUF;     /**< A receive buffer */
    REG8    ATXBUF;     /**< A transmit buffer */
    REG8    BCTL0;      /**< B control 0 */
    REG8    BCTL1;      /**< B control 1 */
    REG8    BBR0;       /**< B baud rate 0 */
    REG8    BBR1;       /**< B baud rate 1 */
    REG8    BI2CIE;     /**< I2C interrupt enable */
    REG8    BSTAT;      /**< B status */
    REG8    BRXBUF;     /**< B receive buffer */
    REG8    BTXBUF;     /**< B transmit buffer */
} msp_usci_t;

/**
 * @brief   USCI SPI specific registers
 */
typedef struct {
    REG8    CTL0;      /**< control 0 */
    REG8    CTL1;      /**< control 1 */
    REG8    BR0;       /**< baud rate 0 */
    REG8    BR1;       /**< baud rate 1 */
    REG8    reserved;  /**< reserved */
    REG8    STAT;      /**< status */
    REG8    RXBUF;     /**< receive buffer */
    REG8    TXBUF;     /**< transmit buffer */
} msp_usci_spi_t;

/**
 * @name    USCI control A register 0 bitmap
 * @{
 */
#define USCI_ACTL0_UCSYNC           (0x01)
#define USCI_ACTL0_MODE_MASK        (0x06)
#define USCI_ACTL0_MODE_UART        (0x00)
#define USCI_ACTL0_MODE_ILMM        (0x02)
#define USCI_ACTL0_MODE_ABMM        (0x04)
#define USCI_ACTL0_MODE_UART_ABR    (0x06)
#define USCI_ACTL0_SPB              (0x08)
#define USCI_ACTL0_7BIT             (0x10)
#define USCI_ACTL0_MSB              (0x20)
#define USCI_ACTL0_PAR              (0x40)
#define USCI_ACTL0_PEN              (0x80)
/** @} */

/**
 * @name    USCI control register 0 bitmap SPI mode
 * @{
 */
#define USCI_SPI_CTL0_UCSYNC        (0x01)
#define USCI_SPI_CTL0_MODE_3        (0x06)
#define USCI_SPI_CTL0_MODE_0        (0x00)
#define USCI_SPI_CTL0_MODE_1        (0x02)
#define USCI_SPI_CTL0_MODE_2        (0x04)
#define USCI_SPI_CTL0_MST           (0x08)
#define USCI_SPI_CTL0_7BIT          (0x10)
#define USCI_SPI_CTL0_MSB           (0x20)
#define USCI_SPI_CTL0_CKPL          (0x40)
#define USCI_SPI_CTL0_CKPH          (0x80)
/** @} */

/**
 * @name    USCI status register bitmap SPI mode
 * @{
 */
#define USCI_SPI_STAT_UCBUSY        (0x01)
#define USCI_SPI_STAT_UCOE          (0x20)
#define USCI_SPI_STAT_UCFE          (0x40)
#define USCI_SPI_STAT_UCLISTEN      (0x80)
/** @} */

/**
 * @name    USCI control A register 1 bitmap
 * @{
 */
#define USCI_ACTL1_SWRST            (0x01)
#define USCI_ACTL1_TXBRK            (0x02)
#define USCI_ACTL1_TXADDR           (0x04)
#define USCI_ACTL1_DORM             (0x08)
#define USCI_ACTL1_BRKIE            (0x10)
#define USCI_ACTL1_RXEIE            (0x20)
#define USCI_ACTL1_SSEL_MASK        (0xc0)
#define USCI_ACTL1_SSEL_UCLK        (0x00)
#define USCI_ACTL1_SSEL_ACLK        (0x40)
#define USCI_ACTL1_SSEL_SMCLK       (0xc0)
/** @} */

/**
 * @name    USCI control register 1 bitmap SPI mode
 * @{
 */
#define USCI_SPI_CTL1_SWRST            (0x01)
#define USCI_SPI_CTL1_SSEL_MASK        (0xc0)
#define USCI_SPI_CTL1_SSEL_NA          (0x00)
#define USCI_SPI_CTL1_SSEL_ACLK        (0x40)
#define USCI_SPI_CTL1_SSEL_SMCLK       (0xc0)
/** @} */

/**
 * @name    USCI modulation A control register
 * @{
 */
#define USCI_AMCTL_OS16             (0x01)
#define USCI_AMCTL_BRS_MASK         (0xe0)
#define USCI_AMCTL_BRS_SHIFT        (1U)
#define USCI_AMCTL_BRF_MASK         (0xf0)
#define USCI_AMCTL_BRF_SHIFT        (4U)
/** @} */

/**
 * @name    USCI status A register bitmap
 * @{
 */
#define USCI_ASTAT_BUSY             (0x01)
#define USCI_ASTAT_IDLE             (0x02)
#define USCI_ASTAT_ADDR             (0x02)
#define USCI_ASTAT_RXERR            (0x04)
#define USCI_ASTAT_BRK              (0x08)
#define USCI_ASTAT_PE               (0x10)
#define USCI_ASTAT_OE               (0x20)
#define USCI_ASTAT_FE               (0x40)
#define USCI_ASTAT_LISTEN           (0x80)
/** @} */
/**
 * @name    Base register address definitions
 * @{
 */
#define USCI_0_BASE             ((uint16_t)0x005d)
#define USCI_0_A_BASE           ((uint16_t)0x0060)
#define USCI_0_B_BASE           ((uint16_t)0x0068)
#define USCI_1_BASE             ((uint16_t)0x00cd)
#define USCI_1_A_BASE           ((uint16_t)0x00d0)
#define USCI_1_B_BASE           ((uint16_t)0x00d8)
/** @} */

/**
 * @name    Typing of base register objects
 * @{
 */
#define USCI_0                  ((msp_usci_t *)USCI_0_BASE)
#define USCI_1                  ((msp_usci_t *)USCI_1_BASE)
#define USCI_0_A_SPI            ((msp_usci_spi_t *)USCI_0_A_BASE)
#define USCI_0_B_SPI            ((msp_usci_spi_t *)USCI_0_B_BASE)
#define USCI_1_A                ((msp_usci_t *)USCI_1_A_BASE)
#define USCI_1_B                ((msp_usci_t *)USCI_1_B_BASE)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* F2XX_G2XX_MSP430_REGS_H */
/** @} */
