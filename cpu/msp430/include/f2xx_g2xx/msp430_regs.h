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
} msp_usci_a_t;

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
} msp_usci_b_t;

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
#define UCBRS_POS       1       /**< Position of the UCRBS field in the
                                     UCAxMCTL register */
#else
/* The datasheet for the whole MCU family states the field is in bits 3-1,
 * but let's better be safe than sorry here */
#error  "UCBRS field in the UCAxMCTL register at unexpected position."
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
static inline msp_usci_b_t * msp430_usci_b_from_usci_a(msp_usci_a_t *usci_a)
{
    uintptr_t usci_b = (uintptr_t)usci_a + offsetof(msp_usci_a_t, CTL0);
    return (msp_usci_b_t *)usci_b;
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
extern msp_usci_a_t USCI_A0;
/**
 * @brief   USCI_A1 register map
 *
 * @details Provided by linker
 */
extern msp_usci_a_t USCI_A1;
/**
 * @brief   USCI_B0 register map
 *
 * @details Provided by linker
 */
extern msp_usci_a_t USCI_B0;
/**
 * @brief   USCI_B1 register map
 *
 * @details Provided by linker
 */
extern msp_usci_a_t USCI_B1;
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* F2XX_G2XX_MSP430_REGS_H */
/** @} */
