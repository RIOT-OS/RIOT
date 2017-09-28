/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mcp23017
 * @ingroup     drivers_actuators
 * @brief       Device driver interface for the MCP23017
 * @{
 *
 * @file
 * @brief       Interface definition for the MCP23017
 *
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 */

#ifndef MCP23017_H
#define MCP23017_H

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/i2c.h"
#include "periph/gpio.h"

/**
 * @brief   Possible MCP23017 hardware addresses (wiring specific)
 */
enum {
    MCP23017_ADDR_20 = 0x20, /**< I2C device address: A2=0, A1=0, A0=0 */
    MCP23017_ADDR_21 = 0x21, /**< I2C device address: A2=0, A1=0, A0=1 */
    MCP23017_ADDR_22 = 0x22, /**< I2C device address: A2=0, A1=1, A0=0 */
    MCP23017_ADDR_23 = 0x23, /**< I2C device address: A2=0, A1=1, A0=1 */
    MCP23017_ADDR_24 = 0x24, /**< I2C device address: A2=1, A1=0, A0=0 */
    MCP23017_ADDR_25 = 0x25, /**< I2C device address: A2=1, A1=0, A0=1 */
    MCP23017_ADDR_26 = 0x26, /**< I2C device address: A2=1, A1=1, A0=0 */
    MCP23017_ADDR_27 = 0x27, /**< I2C device address: A2=1, A1=1, A0=1 */
};

/**
 * @brief   Output Interrupt selection
 */
enum {
    INTA,  /**< Output interrupt on INTA pin */
    INTB   /**< Output interrupt on INTB pin */
};

/**
 * @brief   Named return values
 */
enum {
    MCP23017_OK          =  0,       /**< everything was fine */
    MCP23017_NOI2C       = -1,       /**< I2C communication failed */
    MCP23017_NODEV       = -2        /**< no MCP23017 device found on the bus */
};

/**
 * @brief Device descriptor for the MCP23017 sensor
 */
typedef struct {
    i2c_t i2c;                       /**< I2C device which is used */
    uint8_t addr;                    /**< I2C address */
    uint8_t port_a_dir;
    uint8_t port_b_dir;
    uint8_t port_a_pull_up;
    uint8_t port_b_pull_up;
    uint8_t port_a_value;
    uint8_t port_b_value;
    uint8_t port_a_inten;
    uint8_t port_b_inten;
} mcp23017_t;

/**
 * @brief   Struct containing the needed peripheral configuration
 */
typedef struct {
    gpio_t int_a_pin;                /**< interrupt a pin */
    gpio_t int_b_pin;                /**< interrupt b pin */
} mcp23017_params_t;

/**
 * @brief   Initialize the MCP23017 driver.
 *
 * @param[out] dev          device descriptor of MCP23017 to initialize
 *
 * @return                  MCP23017_OK on success
 * @return                  MCP23017_NOI2C if initialization of I2C bus failed
 */
int mcp23017_init(mcp23017_t *dev, const mcp23017_params_t* params);

/**
 * @brief   Read port of MCP23017
 *
 * @param[in]  dev          device descriptor of MCP23017
 * @param[in]  port         device port
 *
 * @return                  value of port
 * @return                  MCP23017_NODEV no device found
 */
int mcp23017_read_port(mcp23017_t *dev, uint8_t port);

/**
 * @brief   Set pin direction of MCP23017
 *
 * @param[in]  dev          device descriptor of MCP23017
 * @param[in]  port         device port
 * @param[in]  pin          device pin
 * @param[in]  direction    pin direction
 *
 * @return                  MCP23017_OK on success
 * @return                  MCP23017_NODEV no device found
 */
int mcp23017_set_dir(mcp23017_t *dev, uint8_t port, uint8_t pin, uint8_t dir);

/**
 * @brief   Set pull-up for pin of MCP23017
 *
 * @param[in]  dev          device descriptor of MCP23017
 * @param[in]  port         device port
 * @param[in]  pin          device pin
 * @param[in]  pull_up      pin pull-up
 *
 * @return                  MCP23017_OK on success
 * @return                  MCP23017_NODEV no device found
 */
int mcp23017_set_pull_up(mcp23017_t *dev, uint8_t port, uint8_t pin, uint8_t pull_up);

/**
 * @brief   Set pin value of MCP23017
 *
 * @param[in]  dev          device descriptor of MCP23017
 * @param[in]  port         device port
 * @param[in]  pin          device pin
 * @param[in]  value        pin value
 *
 * @return                  MCP23017_OK on success
 * @return                  MCP23017_NODEV no device found
 */
int mcp23017_set_pin_value(mcp23017_t *dev, uint8_t port, uint8_t pin, uint8_t value);

/**
 * @brief   Set pin interrupt of MCP23017
 *
 * @param[in]  dev          device descriptor of MCP23017
 * @param[in]  port         device port
 * @param[in]  pin          device pin
 * @param[in]  status       pin interrupt status
 *
 * @return                  MCP23017_OK on success
 * @return                  MCP23017_NODEV no device found
 */
int mcp23017_set_int(mcp23017_t *dev, uint8_t port, uint8_t pin, uint8_t status);

/**
 * @brief   Capture interrupt of MCP23017
 *
 * @param[in]  dev          device descriptor of MCP23017
 * @param[in]  port         device port
 *
 * @return                  value of port
 * @return                  MCP23017_NODEV no device found
 */
int mcp23017_capture_interrupt(mcp23017_t *dev, uint8_t port);

#ifdef __cplusplus
}
#endif

#endif /* MCP23017_H */
/** @} */
