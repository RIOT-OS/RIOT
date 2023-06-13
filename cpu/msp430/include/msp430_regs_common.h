/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430
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

#ifndef MSP430_REGS_COMMON_H
#define MSP430_REGS_COMMON_H

#include <stdint.h>

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
 * @name    SFR interrupt enable 1 register bitmap
 * @{
 */
#define SFR_IE1_OFIE                (0x02)
#define SFR_IE1_URXIE0              (0x40)
#define SFR_IE1_UTXIE0              (0x80)
/** @} */

/**
 * @name    SFR interrupt enable 2 register bitmap
 * @{
 */
#define SFR_IE2_UCA0RXIE            (0x01)
#define SFR_IE2_UCA0TXIE            (0x02)
#define SFR_IE2_URXIE2              (0x10)
#define SFR_IE2_UTXIE2              (0x20)
/** @} */

/**
 * @name    SFR interrupt flag 1 register bitmap
 * @{
 */
#define SFR_IFG1_OFIFG              (0x02)
#define SFR_IFG1_URXIFG0            (0x40)
#define SFR_IFG1_UTXIFG0            (0x80)
/** @} */

/**
 * @name    SFR interrupt flag 2 register bitmap
 * @{
 */
#define SFR_IFG2_UCA0RXIFG          (0x01)
#define SFR_IFG2_UCA0TXIFG          (0x02)
#define SFR_IFG2_URXIFG1            (0x10)
#define SFR_IFG2_UTXIFG1            (0x20)
/** @} */

/**
 * @name    SFR module enable register 1
 * @{
 */
#define SFR_ME1_USPIE0              (0x40)
/** @} */

/**
 * @name    SFR module enable register 2
 * @{
 */
#define SFR_ME2_USPIE1              (0x10)
/** @} */

/**
 * @name    Timer Control register bitmap
 * @{
 */
#define TIMER_CTL_IFG                 (0x0001)
#define TIMER_CTL_IE                  (0x0002)
#define TIMER_CTL_CLR                 (0x0004)
#define TIMER_CTL_MC_MASK             (0x0030)
#define TIMER_CTL_MC_STOP             (0x0000)
#define TIMER_CTL_MC_UP               (0x0010)
#define TIMER_CTL_MC_CONT             (0x0020)
#define TIMER_CTL_MC_UPDOWN           (0x0030)
#define TIMER_CTL_ID_MASK             (0x00c0)
#define TIMER_CTL_ID_DIV1             (0x0000)
#define TIMER_CTL_ID_DIV2             (0x0040)
#define TIMER_CTL_ID_DIV4             (0x0080)
#define TIMER_CTL_ID_DIV8             (0x00c0)
#define TIMER_CTL_TASSEL_MASK         (0x0300)
#define TIMER_CTL_TASSEL_TCLK         (0x0000)
#define TIMER_CTL_TASSEL_ACLK         (0x0100)
#define TIMER_CTL_TASSEL_SMCLK        (0x0200)
#define TIMER_CTL_TASSEL_INV_TCLK     (0x0300)
/** @} */

/**
 * @name    Timer Channel Control register bitmap
 * @{
 */
#define TIMER_CCTL_CCIFG              (0x0001)
#define TIMER_CCTL_COV                (0x0002)
#define TIMER_CCTL_OUT                (0x0004)
#define TIMER_CCTL_CCI                (0x0008)
#define TIMER_CCTL_CCIE               (0x0010)
#define TIMER_CCTL_OUTMOD_MASK        (0x00e0)
#define TIMER_CCTL_OUTMOD_OUTVAL      (0x0000)
#define TIMER_CCTL_OUTMOD_SET         (0x0020)
#define TIMER_CCTL_OUTMOD_TOG_RESET   (0x0040)
#define TIMER_CCTL_OUTMOD_SET_RESET   (0x0060)
#define TIMER_CCTL_OUTMOD_TOGGLE      (0x0080)
#define TIMER_CCTL_OUTMOD_RESET       (0x00a0)
#define TIMER_CCTL_OUTMOD_TOG_SET     (0x00c0)
#define TIMER_CCTL_OUTMOD_RESET_SET   (0x00e0)
#define TIMER_CCTL_CAP                (0x0100)
#define TIMER_CCTL_CLLD_MASK          (0x0600)
#define TIMER_CCTL_SCS                (0x0800)
#define TIMER_CCTL_CCIS_MASK          (0x3000)
#define TIMER_CCTL_CM_MASK            (0xc000)
/** @} */

/**
 * @name    Base register address definitions
 * @{
 */
#define SFR_BASE                ((uint16_t)0x0000)
#define PORT_1_BASE             ((uint16_t)0x0020)
#define PORT_2_BASE             ((uint16_t)0x0028)
#define PORT_3_BASE             ((uint16_t)0x0018)
#define PORT_4_BASE             ((uint16_t)0x001c)
#define PORT_5_BASE             ((uint16_t)0x0030)
#define PORT_6_BASE             ((uint16_t)0x0034)
#define CLK_BASE                ((uint16_t)0x0053)
#define TIMER_IVEC_BASE         ((uint16_t)0x011e)
#define TIMER_A_BASE            ((uint16_t)0x0160)
#define TIMER_B_BASE            ((uint16_t)0x0180)
#define WD_BASE                 ((uint16_t)0x0120)
/** @} */

/**
 * @name    Typing of base register objects
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
#define TIMER_IVEC              ((msp_timer_ivec_t *)TIMER_IVEC_BASE)
#define TIMER_A                 ((msp_timer_t *)TIMER_A_BASE)
#define TIMER_B                 ((msp_timer_t *)TIMER_B_BASE)
#define WD                      ((msp_wd_t *)WD_BASE)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MSP430_REGS_COMMON_H */
/** @} */
