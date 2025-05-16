/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_at24cxxx
 * @{
 *
 * @file
 * @brief       Default configuration for the AT24CXXX driver
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */


#include "board.h"
#include "periph/gpio.h"
#include "at24cxxx_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the AT24CXXX driver
 * @{
 */
#ifndef AT24CXXX_PARAM_I2C
/**
 * @brief  I2C bus the EEPROM is connected to
 */
#define AT24CXXX_PARAM_I2C              (I2C_DEV(0))
#endif
#ifndef AT24CXXX_PARAM_ADDR
/**
 * @brief   I2C address of the EEPROM device
 */
#define AT24CXXX_PARAM_ADDR             (AT24CXXX_DEF_DEV_ADDR)
#endif
#ifndef AT24CXXX_PARAM_PIN_WP
/**
 * @brief   EEPROM write protect pin
 */
#define AT24CXXX_PARAM_PIN_WP           (GPIO_UNDEF)
#endif
#ifndef AT24CXXX_PARAM_EEPROM_SIZE
/**
 * @brief   EEPROM size
 */
#define AT24CXXX_PARAM_EEPROM_SIZE      (AT24CXXX_EEPROM_SIZE)
#endif
#ifndef AT24CXXX_PARAM_PAGE_SIZE
/**
 * @brief   Page size
 */
#define AT24CXXX_PARAM_PAGE_SIZE        (AT24CXXX_PAGE_SIZE)
#endif
#ifndef AT24CXXX_PARAM_MAX_POLLS
/**
 * @brief   Maximum poll poll
 */
#define AT24CXXX_PARAM_MAX_POLLS        (AT24CXXX_MAX_POLLS)
#endif
#ifndef AT24CXXX_PARAMS
/**
 * @brief   Default device configuration parameters
 */
#define AT24CXXX_PARAMS                {            \
    .i2c = AT24CXXX_PARAM_I2C,                      \
    .pin_wp = AT24CXXX_PARAM_PIN_WP,                \
    .eeprom_size = AT24CXXX_PARAM_EEPROM_SIZE,      \
    .dev_addr = AT24CXXX_PARAM_ADDR,                \
    .page_size = AT24CXXX_PARAM_PAGE_SIZE,          \
    .max_polls = AT24CXXX_PARAM_MAX_POLLS           \
}
#endif
/** @} */

/**
 * @brief Number of configured AT24CXXX EEPROM devices
 */
#define AT24CXXX_NUMOF ARRAY_SIZE(at24cxxx_params)

/**
 * @brief   AT24CXXX configuration
 */
static const at24cxxx_params_t at24cxxx_params[] =
{
    AT24CXXX_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
