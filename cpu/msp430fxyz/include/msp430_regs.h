/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430-common
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

#ifndef MSP430_REGS_H
#define MSP430_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Shortcut to specify 8-bit wide registers
 */
#define REG8                volatile uint8_t

/**
 * @brief   Shortcut to specify 16-bit wide registers
 */
#define REG16               volatile uint16_t

/**
 * @brief   Special function registers
 */
typedef struct {
    REG8    IE1;        /**< interrupt enable 1 */
    REG8    IE2;        /**< interrupt enable 2 */
    REG8    IFG1;       /**< interrupt flag 1 */
    REG8    IFG2;       /**< interrupt flag 2 */
    REG8    ME1;        /**< module enable 1 */
    REG8    ME2;        /**< module enable 2 */
} msp_sfr_t;

/**
 * @brief   Digital I/O Port w/o interrupt functionality (P3-P6)
 */
typedef struct {
    REG8    IN;         /**< input data */
    REG8    OD;         /**< output data */
    REG8    DIR;        /**< pin direction */
    REG8    SEL;        /**< alternative function select */
} msp_port_t;

/**
 * @brief   Digital I/O Port with interrupt functionality (P1 & P2)
 */
typedef struct {
    REG8    IN;         /**< input data */
    REG8    OD;         /**< output data */
    REG8    DIR;        /**< pin direction */
    REG8    IFG;        /**< interrupt flag */
    REG8    IES;        /**< interrupt edge select */
    REG8    IE;         /**< interrupt enable */
    REG8    SEL;        /**< alternative function select */
} msp_port_isr_t;

/**
 * @brief   USART (UART, SPI and I2C) registers
 */
typedef struct {
    REG8    CTL;        /**< USART control */
    REG8    TCTL;       /**< transmit control */
    REG8    RCTL;       /**< receive control */
    REG8    MCTL;       /**< modulation control */
    REG8    BR0;        /**< baud rate control 0 */
    REG8    RR1;        /**< baud rate control 1 */
    REG8    RXBUF;      /**< receive buffer */
    REG8    TXBUF;      /**< transmit buffer */
} msp_usart_t;

/**
 * @brief   System clock module configuration registers
 */
typedef struct {
    REG8    DCOCTL;     /**< digital controlled oscillator control */
    REG8    BCSCTL1;    /**< basic clock system control 1 */
    REG8    BCSCTL2;    /**< basic clock system control 2 */
} msp_clk_t;

/**
 * @brief   Watchdog configuration registers
 */
typedef struct {
    REG16   TCTL;       /**< watchdog time control */
} msp_wd_t;

/**
 * @brief   Timer interrupt status registers
 */
typedef struct {
    REG16   TBIV;       /**< TIMER_A interrupt status */
    REG16   reserved[7];    /**< reserved */
    REG16   TAIV;       /**< TIMER_B interrupt status */
} msp_timer_ivec_t;

/**
 * @brief   Timer module registers
 */
typedef struct {
    REG16   CTL;        /**< timer control */
    REG16   CCTL[7];    /**< capture compare channel control */
    REG16   R;          /**< current counter value */
    REG16   CCR[7];     /**< capture compare channel values */
} msp_timer_t;

/**
 * @brief   Timer Control register bitmap
 * @{
 */
#define CTL_IFG                 (0x0001)
#define CTL_IE                  (0x0002)
#define CTL_CLR                 (0x0004)
#define CTL_MC_MASK             (0x0030)
#define CTL_MC_STOP             (0x0000)
#define CTL_MC_UP               (0x0010)
#define CTL_MC_CONT             (0x0020)
#define CTL_MC_UPDOWN           (0x0030)
#define CTL_ID_MASK             (0x00c0)
#define CTL_ID_DIV1             (0x0000)
#define CTL_ID_DIV2             (0x0040)
#define CTL_ID_DIV4             (0x0080)
#define CTL_ID_DIV8             (0x00c0)
#define CTL_TASSEL_MASK         (0x0300)
#define CTL_TASSEL_TCLK         (0x0000)
#define CTL_TASSEL_ACLK         (0x0100)
#define CTL_TASSEL_SMCLK        (0x0200)
#define CTL_TASSEL_INV_TCLK     (0x0300)
/** @} */

/**
 * @brief   Timer Channel Control register bitmap
 * @{
 */
#define CCTL_CCIFG              (0x0001)
#define CCTL_COV                (0x0002)
#define CCTL_OUT                (0x0004)
#define CCTL_CCI                (0x0008)
#define CCTL_CCIE               (0x0010)
#define CCTL_OUTMOD_MASK        (0x00e0)
#define CCTL_OUTMOD_OUTVAL      (0x0000)
#define CCTL_OUTMOD_SET         (0x0020)
#define CCTL_OUTMOD_TOG_RESET   (0x0040)
#define CCTL_OUTMOD_SET_RESET   (0x0060)
#define CCTL_OUTMOD_TOGGLE      (0x0080)
#define CCTL_OUTMOD_RESET       (0x00a0)
#define CCTL_OUTMOD_TOG_SET     (0x00c0)
#define CCTL_OUTMOD_RESET_SET   (0x00e0)
#define CCTL_CAP                (0x0100)
#define CCTL_CLLD_MASK          (0x0600)
#define CCTL_SCS                (0x0800)
#define CCTL_CCIS_MASK          (0x3000)
#define CCTL_CM_MASK            (0xc000)
/** @} */

/**
 * @brief   Base register address definitions
 * @{
 */
#define SFR_BASE                ((uint16_t)0x0000)
#define PORT_1_BASE             ((uint16_t)0x0020)
#define PORT_2_BASE             ((uint16_t)0x0028)
#define PORT_3_BASE             ((uint16_t)0x0018)
#define PORT_4_BASE             ((uint16_t)0x001c)
#define PORT_5_BASE             ((uint16_t)0x0030)
#define PORT_6_BASE             ((uint16_t)0x0034)
#define CLK_BASE                ((uint16_t)0x0056)
#define USART_0_BASE            ((uint16_t)0x0070)
#define USART_1_BASE            ((uint16_t)0x0078)
#define TIMER_IVEC_BASE         ((uint16_t)0x011e)
#define TIMER_A_BASE            ((uint16_t)0x0160)
#define TIMER_B_BASE            ((uint16_t)0x0180)
#define WD_BASE                 ((uint16_t)0x0120)
/** @} */

/**
 * @brief   Typing of base register objects
 * @{
 */
#define SFR                     ((msp_sfr_t *)SFR_BASE)
#define PORT_1                  ((msp_port_t *)PORT_1_BASE)
#define PORT_2                  ((msp_port_t *)PORT_2_BASE)
#define PORT_3                  ((msp_port_t *)PORT_3_BASE)
#define PORT_4                  ((msp_port_t *)PORT_4_BASE)
#define PORT_5                  ((msp_port_t *)PORT_5_BASE)
#define PORT_6                  ((msp_port_t *)PORT_6_BASE)
#define CLK                     ((msp_clk_t *)CLK_BASE)
#define USART_0                 ((msp_usart_t *)USART_0_BASE)
#define USART_1                 ((msp_usart_t *)USART_1_BASE)
#define TIMER_IVEC              ((msp_timer_ivec_t *)TIMER_IVEC_BASE)
#define TIMER_A                 ((msp_timer_t *)TIMER_A_BASE)
#define TIMER_B                 ((msp_timer_t *)TIMER_B_BASE)
#define WD                      ((msp_wd_t *)WD_BASE)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MSP430_REGS_H */
/** @} */
