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
#include <stddef.h>

#include "msp430_regs_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Offset of the USCI B registers in an USCI A peripheral
 *
 * USCI A peripheral contains the USCI B peripheral registers, but has a few
 * USCI A specific register in front. Adding this offset to the USCI A base
 * address results in the base address of the shared registers.
 *
 * @see     MSP430_USCI_B_FROM_USCI_A
 */
#define MSP430_USCI_A_B_OFFSET  3U

/**
 * @brief   "Convert" an USCI A to an USCI B
 *
 * This returns the pointer to the part of the USCI A registers that matches
 * the USCI B register layout, so that an USCI A can be treated as if it is
 * an USCI B.
 *
 * This can be used instead of @ref msp430_usci_b_from_usci_a when needing a
 * constant initializer. Otherwise @ref msp430_usci_b_from_usci_a is
 * preferred as it includes type checks this macro doesn't.
 */
#define MSP430_USCI_B_FROM_USCI_A(usci_a) \
    ((msp430_usci_b_t *)((uintptr_t)(usci_a) + MSP430_USCI_A_B_OFFSET))

/**
 * @brief   GPIO Port 1/2 (with interrupt functionality)
 */
typedef struct {
    msp430_port_t base; /**< common GPIO port registers */
    REG8    IFG;        /**< interrupt flag */
    REG8    IES;        /**< interrupt edge select */
    REG8    IE;         /**< interrupt enable */
    REG8    SEL;        /**< alternative function select */
    REG8    REN;        /**< pull resistor enable */
} msp430_port_p1_p2_t;

/**
 * @brief   GPIO Port 7/8 (different register layout than Ports 1-6)
 */
typedef struct {
    REG8    IN;         /**< input data */
    uint8_t _padding1;  /**< unrelated I/O */
    REG8    OD;         /**< output data */
    uint8_t _padding2;  /**< unrelated I/O */
    REG8    DIR;        /**< pin direction */
    uint8_t _padding3;  /**< unrelated I/O */
    REG8    SEL;        /**< alternative function select */
} msp430_port_p7_p8_t;

/**
 * @brief   Universal Serial Control Interface Type A (USCI_A) Registers
 */
typedef struct {
    REG8    ABCTL;      /**< auto baud rate control */
    REG8    IRTCTL;     /**< IrDA transmit control */
    REG8    IRRCTL;     /**< IrDA receive control */
    REG8    CTL0;       /**< control 0 */
    REG8    CTL1;       /**< control 1 */
    REG8    BR0;        /**< baud rate control 0 */
    REG8    BR1;        /**< baud rate control 1 */
    REG8    MCTL;       /**< modulation control */
    REG8    STAT;       /**< status register */
    REG8    RXBUF;      /**< receive buffer */
    REG8    TXBUF;      /**< transmit buffer */
} msp430_usci_a_t;

/**
 * @brief   Universal Serial Control Interface Type B (USCI_B) Registers
 */
typedef struct {
    REG8    CTL0;       /**< control 0 */
    REG8    CTL1;       /**< control 1 */
    REG8    BR0;        /**< baud rate control 0 */
    REG8    BR1;        /**< baud rate control 1 */
    REG8    MCTL;       /**< modulation control */
    REG8    STAT;       /**< status register */
    REG8    RXBUF;      /**< receive buffer */
    REG8    TXBUF;      /**< transmit buffer */
} msp430_usci_b_t;

/**
 * @name    USCI clock selection
 *
 * The vendor header files expose clock configurations with hard to read
 * names. So we provide alias with better names
 *
 * @{
 */
#define UCSSEL_UCLKI    UCSSEL_0    /**< Clock USCI using CLKI (n/a in SPI mode) */
#define UCSSEL_ACLK     UCSSEL_1    /**< Clock USCI using auxiliary clock */
#define UCSSEL_SMCLK    UCSSEL_2    /**< Clock USCI using sub-system master clock */

#if (UCSSEL0 == 0x40) || DOXYGEN
#  define UCSSEL_Pos 6      /**< Position of the UCSSEL field in the USCI CTL1 register */
#else
#  error "USSEL field in USCI CTL1 register is at unexpected position"
#endif
/** @} */

/**
 * @name    USCI Modulation Control fields
 *
 * The vendor header files provides bitmasks for each of the possible values
 * from 0..7. But rather than creating a look up table, we can just shift the
 * number in place, if there only would be a UCRBS_SHIFT macro.
 * @{
 */
#define UCBRS_MASK  UCBRS_7     /**< Bitmask to retrieve the UCRBS field of the
                                     USCI modulation control register */
#if (UCBRS_7 == 0x0E) || defined(DOXYGEN)
#  define UCBRS_Pos     1       /**< Position of the UCRBS field in the
                                     UCAxMCTL register */
#else
/* The datasheet for the whole MCU family states the field is in bits 3-1,
 * but let's better be safe than sorry here */
#  error  "UCBRS field in the UCAxMCTL register at unexpected position."
#endif

/** @} */

/**
 * @brief   "Convert" a USCI A into an USCI B interface
 *
 * The USCI peripheral come in two flavors: USCI A and USCI B with the following
 * feature set. When comparing the memory map USCI B is a subset of USCI A
 * registers, which conveniently have the same offsets. It is therefore possible
 * to address USCI A registers as if it was a USCI B interface when operated
 * in SPI mode. This makes implementation of SPI easier.
 *
 * | Name   | UART  | SPI   | I2C   | IrDA  |
 * |:------ |:----- |:----- |:----- |:----- |
 * | USCI A | ✔     | ✔     | ✗     | ✔     |
 * | USCI B | ✗     | ✔     | ✔     | ✗     |
 */
static inline msp430_usci_b_t * msp430_usci_b_from_usci_a(msp430_usci_a_t *usci_a)
{
    uintptr_t usci_b = (uintptr_t)usci_a + offsetof(msp430_usci_a_t, CTL0);
    return (msp430_usci_b_t *)usci_b;
}

/**
 * @name    Typing of base register objects
 * @{
 */
/**
 * @brief   USCI_A0 register map
 *
 * @details Provided by linker
 */
extern msp430_usci_a_t USCI_A0;
/**
 * @brief   USCI_A1 register map
 *
 * @details Provided by linker
 */
extern msp430_usci_a_t USCI_A1;
/**
 * @brief   USCI_B0 register map
 *
 * @details Provided by linker
 */
extern msp430_usci_b_t USCI_B0;
/**
 * @brief   USCI_B1 register map
 *
 * @details Provided by linker
 */
extern msp430_usci_b_t USCI_B1;
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* F2XX_G2XX_MSP430_REGS_H */
