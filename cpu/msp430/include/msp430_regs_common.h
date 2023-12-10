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
 * @name    Timer SSEL Values
 *
 * @details When using the macros in the vendor header files such as TASSEL_0
 *          the actually used clock is non-obvious. Hence, provide aliases
 *          with obvious names.
 * @{
 */
#define TXSSEL_TXCLK    TASSEL_0    /**< External TxCLK as clock source */
#define TXSSEL_ACLK     TASSEL_1    /**< Auxiliary clock as clock source */
#define TXSSEL_SMCLK    TASSEL_2    /**< Sub-system master clock as clock source */
#define TXSSEL_INCLK    TASSEL_3    /**< External INCLK as clock source */
/** @} */

/**
 * @name    Timer Input Divider Values
 *
 * @details The vendor header macros are again non-obvious in their naming, so
 *          provide better alias names.
 * @{
 */
#define TXID_DIV_1      ID_0        /**< Input Divider: Divide by 1 */
#define TXID_DIV_2      ID_1        /**< Input Divider: Divide by 2 */
#define TXID_DIV_4      ID_2        /**< Input Divider: Divide by 4 */
#define TXID_DIV_8      ID_3        /**< Input Divider: Divide by 8 */
#define TXID_DIV_Msk    ID_3        /**< Mask to get the TXID field */
#define TXID_DIV_Pos    6U          /**< Position of the TXID field */
#define TXID_DIV_MAX    3           /**< Maximum configuration value in the TXID field */
/** @} */

/**
 * @name    Timer Mode Control Values
 *
 * @details The vendor header macros are again non-obvious in their naming, so
 *          provide better alies names.
 * @{
 */
#define TXMC_STOP       MC_0        /**< Stop Mode */
#define TXMC_UP         MC_1        /**< Up to CCR0 Mode*/
#define TXMC_CONT       MC_2        /**< Continuous Mode */
#define TXMC_UP_DOWN    MC_3        /**< Up/Down Mode */
#define TXMC_MASK       MC_3        /**< Bitmask to retrieve MC field */
/** @} */

/**
 * @brief   Common MSP GPIO Port Registers
 */
typedef struct {
    REG8    IN;         /**< input data */
    REG8    OD;         /**< output data */
    REG8    DIR;        /**< pin direction */
} msp430_port_t;

/**
 * @brief   GPIO Port 1/2 (with interrupt functionality)
 */
typedef struct {
    msp430_port_t base; /**< common GPIO port registers */
    REG8    IFG;        /**< interrupt flag */
    REG8    IES;        /**< interrupt edge select */
    REG8    IE;         /**< interrupt enable */
    REG8    SEL;        /**< alternative function select */
} msp430_port_p1_p2_t;

/**
 * @brief   GPIO Port 3..6 (without interrupt functionality)
 */
typedef struct {
    msp430_port_t base; /**< common GPIO port registers */
    REG8    SEL;        /**< alternative function select */
} msp430_port_p3_p6_t;

/**
 * @brief   Timer peripheral registers
 *
 * @note    The TIMER_A timer only has 3 CC channels instead of the 8 channels
 *          the TIMER_B has, the memory layout is the same nonetheless.
 */
typedef struct {
    REG16   CTL;        /**< timer control */
    REG16   CCTL[7];    /**< capture compare channel control */
    REG16   R;          /**< current counter value */
    REG16   CCR[7];     /**< capture compare channel values */
} msp430_timer_t;

/**
 * @name    MSP430 Common Peripheral Register Maps
 *
 * @details The addresses will be provided by the linker script using the
 *          vendor files.
 * @{
 */
/**
 * @brief   Register map of GPIO PORT 1
 */
extern msp430_port_p1_p2_t PORT_1;
/**
 * @brief   Register map of GPIO PORT 2
 */
extern msp430_port_p1_p2_t PORT_2;
/**
 * @brief   Register map of GPIO PORT 3
 */
extern msp430_port_p3_p6_t PORT_3;
/**
 * @brief   Register map of GPIO PORT 4
 */
extern msp430_port_p3_p6_t PORT_4;
/**
 * @brief   Register map of GPIO PORT 5
 */
extern msp430_port_p3_p6_t PORT_5;
/**
 * @brief   Register map of GPIO PORT 6
 */
extern msp430_port_p3_p6_t PORT_6;

/**
 * @brief   Register map of the timer A control registers
 */
extern msp430_timer_t TIMER_A;

/**
 * @brief   IRQ flags for TIMER_A
 *
 * Called TAIV in the data sheet / vendor files. This shallow alias
 * makes the name more readable and does impedance matching for the type
 * (`volatile uint16_t` vs `volatile short`).
 */
extern REG16 TIMER_A_IRQFLAGS;

/**
 * @brief   IRQ flags for TIMER_B
 *
 * Called TBIV in the data sheet / vendor files. This shallow alias
 * makes the name more readable and does impedance matching for the type
 * (`volatile uint16_t` vs `volatile short`).
 */
extern REG16 TIMER_B_IRQFLAGS;

/**
 * @brief   Register map of the timer B control registers
 */
extern msp430_timer_t TIMER_B;
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MSP430_REGS_COMMON_H */
/** @} */
