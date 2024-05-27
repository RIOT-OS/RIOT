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
 * @brief   USART (UART, SPI and I2C) Registers
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
} msp430_usart_t;

/**
 * @brief   USART Special Function Registers (SFR)
 *
 * Despite being part of the USART peripheral, the SFR registers location is
 * completely different. Even more confusing, the IE register of USART 1
 * follows the IE register of USART 0. Hence, the SFR register map of the two
 * USART peripherals will overlap (with the IE register of the second USART
 * being located at the first padding of the first USART). The padding bytes
 * are intentionally declared as `const` to aid in preventing accidentally
 * overwriting registers of the other USART's SFR registers.
 */
typedef struct {
    REG8    IE;         /**< USART Interrupt Enable Register */
    const uint8_t _pad1;/**< Padding */
    REG8    IFG;        /**< USART Interrupt Flag Register */
    const uint8_t _pad2;/**< Padding */
    REG8    ME;         /**< Module Enable Register */
} msp430_usart_sfr_t;

/**
 * @name    USART clock selection
 *
 * The vendor header files expose clock configurations selection field as one
 * bitmask per bit. This is pretty hard to read in the code, so we provide
 * alias with better names.
 *
 * @{
 */
#define UXTCTL_SSEL_UCLKI   0               /**< Clock USART using UCLKI clock */
#define UXTCTL_SSEL_ACLK    SSEL0           /**< Clock USART using auxiliary clock */
#define UXTCTL_SSEL_SMCLK   SSEL1           /**< Clock USART using sub-system master clock */
#define UXTCTL_SSEL_MASK    (SSEL0 | SSEL1) /**< Mask to retrieve SSEL field */
/** @} */

/**
 * @name    Typing of base register objects
 * @{
 */
/**
 * @brief   USART 0 SFR register map
 */
extern msp430_usart_sfr_t USART_0_SFR;
/**
 * @brief   USART 1 SFR register map
 */
extern msp430_usart_sfr_t USART_1_SFR;
/**
 * @brief   USART 0 register map
 *
 * @details The address is provided by the linker script
 */
extern msp430_usart_t USART_0;
/**
 * @brief   USART 1 register map
 *
 * @details The address is provided by the linker script
 */
extern msp430_usart_t USART_1;
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* X1XX_MSP430_REGS_H */
/** @} */
