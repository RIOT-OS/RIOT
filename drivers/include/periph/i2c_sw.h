/*
 * Copyright (C) 2016 Pavol Malosek <malo@25cmsquare.io>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph
 * @brief       Software I2C functions declarations.
 *
 * @{
 *
 * @file
 * @brief       Software I2C functions declarations.
 *
 * @author      Pavol Malosek <malo@25cmsquare.io>
 */

#ifndef I2C_SW_H
#define I2C_SW_H

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize GPIOs as general purpose pins.
 *
 * @param[in] dev           device descriptor
 * @return                  zero on success
 * @return                  -1 on unknown device
 */
int i2c_sw_init_gpio(i2c_t dev);

/**
 * @brief   Release SCL line.
 *
 * @param[in] dev           device descriptor
 */
void i2c_sw_set_SCL(i2c_t dev);

/**
 * @brief   Drive SCL line to 0.
 *
 * @param[in] dev           device descriptor
 */
void i2c_sw_clear_SCL(i2c_t dev);

/**
 * @brief   Release SDA line.
 *
 * @param[in] dev           device descriptor
 */
void i2c_sw_set_SDA(i2c_t dev);

/**
 * @brief   Drive SDA line to 0.
 *
 * @param[in] dev           device descriptor
 */
void i2c_sw_clear_SDA(i2c_t dev);

/**
 * @brief   Read SCL line.
 *
 * @param[in] dev           device descriptor
 * @return                  1 on SCL HI
 * @return                  0 on SCL LOW
 * @return                  -1 on unknown device
 */
int i2c_sw_read_SCL(i2c_t dev);

/**
 * @brief   Read SCL line.
 *
 * @param[in] dev           device descriptor
 * @return                  1 on SDA HI
 * @return                  0 on SDA LOW
 * @return                  -1 on unknown device
 */
int i2c_sw_read_SDA(i2c_t dev);

/**
 * @brief   Delay half of the LOW period of the SCL clock.
 */
void i2c_sw_delay_2_35us(void);
/**
 * @brief   Delay LOW period of the SCL clock.
 */
void i2c_sw_delay_4_70us(void);
/**
 * @brief   Delay HIGH period of the SCL clock.
 */
void i2c_sw_delay_4_00us(void);

#ifdef __cplusplus
}
#endif

#endif /* I2C_SW_H */
/** @} */
