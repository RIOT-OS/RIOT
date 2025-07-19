/*
 * Copyright (C) Baptiste Le Duc <baptiste.leduc38@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_ads1115
 * @{
 *
 * @file
 * @brief       Internal definitions for ADS1115 Analog-to-digital converter
 *
 * @author      Baptiste Le Duc <baptiste.leduc38@gmail.com>
 * @}
 */


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name    ADS1115 registers
 * @{
 */

#define ADS1115_REG_CONVERSION  (0x00)  /**< Conversion register */
#define ADS1115_REG_CONFIG      (0x01)  /**< Configuration register */
/** @} */


/**
 * @name    ADS1115 configuration register bits
 * @{
 */
#define ADS1115_CONF_OS_BIT            (15) /**< Operational status */
#define ADS1115_CONF_MUX_BIT           (12) /**< Input multiplexer configuration */
#define ADS1115_CONF_PGA_BIT           (9)  /**< Programmable gain amplifier configuration */
#define ADS1115_CONF_MODE_BIT          (8)  /**< Device mode */
#define ADS1115_CONF_DR_BIT            (5)  /**< Data rate configuration */
#define ADS1115_CONF_COMP_MODE_BIT     (4)  /**< Comparator mode */
#define ADS1115_CONF_COMP_POLARITY_BIT (3)  /**< Comparator polarity */
#define ADS1115_CONF_COMP_LATCH_BIT    (2)  /**< Comparator latch */
#define ADS1115_CONF_COMP_QUEUE_BIT    (0)  /**< Comparator queue */
/** @} */


#ifdef __cplusplus
}
#endif
