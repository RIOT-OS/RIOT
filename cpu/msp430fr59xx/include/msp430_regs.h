/*
 * Copyright (C) 2018 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430fr59xx
 * @{
 *
 * @file
 * @brief       Cortex CMSIS style definition of MSP430 registers
 *
 * @todo        This file is incomplete, not all registers are listed. Further
 *              There are probably some inconsistencies throughout the MSP430
 *              family which need to be addressed.
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef MSP430_REGS_H
#define MSP430_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Shortcut to specify 8-bit wide registers
 */
typedef volatile unsigned char REG8;

/**
 * @brief   Shortcut to specify 16-bit wide registers
 */
typedef volatile unsigned int REG16;

/**
 * @brief   Shortcut to specify 32-bit wide registers
 */
typedef volatile unsigned long int REG32;

/**
 * @brief   Digital I/O Port
 */
typedef struct msp_port {
    REG8 IN[2];         /**< input (00-01) */
    REG8 OD[2];         /**< output (02-03) */
    REG8 DIR[2];        /**< direction (04-05) */
    REG8 REN[2];        /**< resistor enable (06-07) */
    char pad_0[2];      /**< padding (08-09) */
    REG8 SEL0[2];       /**< resistor enable (0A-0B) */
    REG8 SEL1[2];       /**< resistor enable (0C-0D) */
    REG16 IV1;          /**< interrupt vector word 1 (0E-0F) */
    char pad_1[6];      /**< padding (10-15) */
    REG8 SELC[2];       /**< complement selection (16-17) */
    REG8 IES[2];        /**< interrupt edge select (18-19) */
    REG8 IE[2];         /**< interrupt enable (1A-1B) */
    REG8 IFG[2];        /**< interrupt flag (1C-1D) */
    REG16 IV2;          /**< interrupt vector word 2 (1E-1F) */
} msp_port_t;

/**
 * @brief   USCI universal serial control interface registers
 */
typedef struct msp_usci {
    REG16 CTLW0;        /**< A control word 0 (00-01) */
    REG16 CTLW1;        /**< A control word 1 (02-03) */
    char pad_0[2];      /**< padding (04-05) */
    REG8 BR0;           /**< A baud rate 0 (06) */
    REG8 BR1;           /**< A baud rate 1 (07) */
    REG16 MCTLW;        /**< A modulation control (08-09) */
    REG16 STATW;        /**< A status word (0A-0B) */
    REG16 RXBUF;        /**< A receive buffer (0C-0D) */
    REG16 TXBUF;        /**< A transmit buffer (0E-0F) */
    REG16 ABCTL;        /**< A LIN control (10-11) */
    REG8 IRTCTL;        /**< A IrDA transmit control (12) */
    REG8 IRRCTL;        /**< A IrDA receive control (13) */
    char pad_1[6];      /**< padding (14-19) */
    REG16 IE;           /**< A interrupt enable (1A-1B) */
    REG16 IFG;          /**< A interrupt flags (1C-1D) */
    REG16 IV;           /**< A interrupt vector word (1E-1F) */
} msp_usci_t;

/**
 * @brief   Timer module registers
 */
typedef struct msp_timer {
    REG16 CTL;          /**< timer control */
    REG16 CCTL[5];      /**< capture compare channel control */
    char pad_0[4];      /**< padding */
    REG16 R;            /**< current counter value */
    REG16 CCR[5];       /**< capture compare channel values */
    REG16 EX0;          /**< expansion 0 */
    char pad_1[12];     /**< padding */
    REG16 IV;           /**< interrupt vector */
} msp_timer_t;

/**
 * @brief   DMA channel registers
 */
typedef struct msp_dma_channel {
    REG16 CTL;  /**< channel control (00-01) */
    REG32 SA;   /**< source address (02-05) */
    REG32 DA;   /**< destination address (06-09) */
    REG16 SZ;   /**< transfer size (0A-0B) */
} msp_dma_channel_t;

/**
 * @brief   DMA module registers
 */
typedef struct msp_dma {
    REG16 CTL0;     /**< module control 0 (00-01) */
    REG16 CTL1;     /**< module control 1 (02-03) */
    REG16 CTL2;     /**< module control 2 (04-05) */
    REG16 CTL3;     /**< module control 3 (05-06) */
    REG16 CTL4;     /**< module control 4 (07-08) */
    char pad_0[5];  /**< padding (09-0D) */
    REG16 IV;       /**< interrupt vector (0E-0F) */
} msp_dma_t;

/**
 * @brief   USCI control register 0 bitmap
 * @{
 */
#define USCI_CTL0_SWRST UCSWRST
#define USCI_CTL0_SSEL_SMCLK    UCSSEL__SMCLK
/** @} */

/**
 * @brief   USCI interrupt vector register bitmap
 * @{
 */
#define USCI_IV_RXIFG   USCI_UART_UCRXIFG
#define USCI_IV_TXCPTIFG USCI_UART_UCTXCPTIFG
/** @} */

/**
 * @brief   Timer Control register bitmap
 * @{
 */
#define TIMER_CTL_CLR   TACLR
#define TIMER_CTL_MC_CONT   MC__CONTINUOUS
#define TIMER_CTL_ID_DIV1   ID_0
#define TIMER_CTL_ID_DIV2   ID_1
#define TIMER_CTL_ID_DIV4   ID_2
#define TIMER_CTL_ID_DIV8   ID_3
#define TIMER_CTL_TASSEL_SMCLK  TASSEL__SMCLK
/** @} */

/**
 * @brief   Timer Channel Control register bitmap
 * @{
 */
#define TIMER_CCTL_CCIFG    CCIFG
#define TIMER_CCTL_CCIE CCIE
/** @} */

/**
 * @brief   Typing of base register objects
 * @{
 */
#define PORT_A  ((msp_port_t *)PA_BASE)
#define PORT_B  ((msp_port_t *)PB_BASE)
#define PORT_J  ((msp_port_t *)PJ_BASE)
/** @} */

/**
 * @brief   DMA Control register 0 bitmap
 * @{
 */
#define DMA_CTL0_SEL_REQ    DMA0TSEL__DMAREQ
/** @} */

/**
 * @brief   DMA Channel Control register bitmap
 * @{
 */
#define DMA_CC_DT_BLOCK DMADT_1
#define DMA_CC_DST_INCR   DMADSTINCR_3
#define DMA_CC_SRC_INCR DMASRCINCR_3
#define DMA_CC_SRC_CONST   DMASRCINCR_0
#define DMA_CC_EN    DMAEN
#define DMA_CC_REQ    DMAREQ
#define DMA_CC_DST_BYTE    DMADSTBYTE
#define DMA_CC_SRC_BYTE    DMASRCBYTE
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MSP430_REGS_H */
/** @} */
