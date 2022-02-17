/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at24xxxx
 * @{
 *
 * @file
 * @brief       Default configuration for AT24XXXX
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT24XXXX_PARAMS_H
#define AT24XXXX_PARAMS_H

#include "board.h"
#include "periph/gpio.h"
#include "at24xxxx_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the AT24XXXX driver
 * @{
 */
#ifndef AT24XXXX_PARAM_I2C
/**
 * @brief  I2C bus the EEPROM is connected to
 */
#define AT24XXXX_PARAM_I2C              (I2C_DEV(0))
#endif
#ifndef AT24XXXX_PARAM_ADDR
/**
 * @brief   I2C address of the EEPROM device
 */
#define AT24XXXX_PARAM_ADDR             (AT24XXXX_DEF_DEV_ADDR)
#endif
#ifndef AT24XXXX_PARAM_PIN_WP
/**
 * @brief   EEPROM write protect pin
 */
#define AT24XXXX_PARAM_PIN_WP           (GPIO_UNDEF)
#endif
#ifndef AT24XXXX_PARAM_EEPROM_SIZE
/**
 * @brief   EEPROM size
 */
#define AT24XXXX_PARAM_EEPROM_SIZE      (AT24XXXX_EEPROM_SIZE)
#endif
#ifndef AT24XXXX_PARAM_PAGE_SIZE
/**
 * @brief   Page size
 */
#define AT24XXXX_PARAM_PAGE_SIZE        (AT24XXXX_PAGE_SIZE)
#endif
#ifndef AT24XXXX_PARAM_MAX_POLLS
/**
 * @brief   Maximum poll poll
 */
#define AT24XXXX_PARAM_MAX_POLLS        (AT24XXXX_MAX_POLLS)
#endif
#ifndef AT24XXXX_PARAMS
/**
 * @brief   Default device configuration parameters
 */
#define AT24XXXX_PARAMS                {            \
    .i2c = AT24XXXX_PARAM_I2C,                      \
    .pin_wp = AT24XXXX_PARAM_PIN_WP,                \
    .eeprom_size = AT24XXXX_PARAM_EEPROM_SIZE,      \
    .dev_addr = AT24XXXX_PARAM_ADDR,                \
    .page_size = AT24XXXX_PARAM_PAGE_SIZE,          \
    .max_polls = AT24XXXX_PARAM_MAX_POLLS           \
}
#endif
/** @} */

/**
 * @brief Number of configured AT24XXXX EEPROM devices
 */
#define AT24XXXX_NUMOF ARRAY_SIZE(at24xxxx_params)

/**
 * @brief   AT24XXXX configuration
 */
static const at24xxxx_params_t at24xxxx_params[] =
{
    AT24XXXX_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* AT24XXXX_PARAMS_H */
/** @} */
