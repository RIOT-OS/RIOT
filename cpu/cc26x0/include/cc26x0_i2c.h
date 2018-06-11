/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x0_definitions
 * @{
 *
 * @file
 * @brief           CC26x0 MCU I/O register definitions
 *
 * @author          Leon George <leon@georgemail.eu>
 */

#ifndef CC26X0_I2C_H
#define CC26X0_I2C_H

#include "cc26x0.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * I2C registers
 */
typedef struct {
    reg32_t SOAR; /**< slave own address */
    union {
        reg32_t SSTAT; /**< slave status */
        reg32_t SCTL; /**< slave control */
    };
    reg32_t SDR; /**< slave data */
    reg32_t SIMR; /**< slave interrupt mask */
    reg32_t SRIS; /**< slave raw interrupt status */
    reg32_t SMIS; /**< slave masked interrupt status */
    reg32_t SICR; /**< slave interrupt clear */
    reg32_t __reserved[0x1F9]; /**< meh */
    reg32_t MSA; /**< master slave address */
    union {
        reg32_t MSTAT; /**< master status */
        reg32_t MCTRL; /**< master control */
    };
    reg32_t MDR; /**< master data */
    reg32_t MTPR; /**< master timer period */
    reg32_t MIMR; /**< master interrupt mask */
    reg32_t MRIS; /**< master raw interrupt status */
    reg32_t MMIS; /**< master masked interrupt statues */
    reg32_t MICR; /**< master interrupt clear */
    reg32_t MCR; /**< master configuration */
} i2c_regs_t;

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define I2C_BASE           (PERIPH_BASE + 0x2000) /**< I2C base address */
/** @} */

#define I2C ((i2c_regs_t *) (I2C_BASE)) /**< I2C register bank */

#ifdef __cplusplus
}
#endif

#endif /* CC26X0_I2C_H */

/*@}*/
