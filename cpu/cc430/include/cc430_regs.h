/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc430
 * @{
 *
 * @file
 * @brief       Cortex CMSIS style definition of CC430 registers
 *
 * @todo        This file is incomplete, not all registers are listed. Further
 *              There are probably some inconsistencies throughout the MSP430
 *              family which need to be addressed.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef CC430_REGS_H
#define CC430_REGS_H

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
 * @brief   Timer module registers
 */
typedef struct {
    REG16   CTL;            /**< timer control */
    REG16   CCTL[7];        /**< capture compare channel control */
    REG16   R;              /**< current counter value */
    REG16   CCR[7];         /**< capture compare channel values */
    REG16   reserved[7];    /**< reserved */
    REG16   IV;             /**< interrupt vector */
    REG16   EX0;            /**< expansion 0 */
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
#define TIMER_A0_BASE           ((uint16_t)0x0340)
#define TIMER_A1_BASE           ((uint16_t)0x0380)
/** @} */

/**
 * @brief   Typing of base register objects
 * @{
 */
#define TIMER_A0                ((msp_timer_t *)TIMER_A0_BASE)
#define TIMER_A1                ((msp_timer_t *)TIMER_A1_BASE)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CC430_REGS_H */
/** @} */
