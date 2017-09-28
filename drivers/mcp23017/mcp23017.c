/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp23017
 * @{
 *
 * @file
 * @brief       Device driver implementation for the MCP23017
 *
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 *
 * @}
 */

#include <stdint.h>

#include "assert.h"
#include "periph/i2c.h"
#include "mcp23017.h"
#include "mcp23017_regs.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#define I2C_SPEED           I2C_SPEED_NORMAL

#define BUS                 (dev->i2c)
#define ADDR                (dev->addr)

int mcp23017_init(mcp23017_t *dev, const mcp23017_params_t* params)
{
    assert(dev);

    /* default values - input */
    dev->port_a_dir = 0xff;
    dev->port_b_dir = 0xff;

    /* Acquire exclusive access */
    i2c_acquire(BUS);

    /* Initialize I2C interface */
    if (i2c_init_master(BUS, I2C_SPEED) < 0) {
        i2c_release(BUS);
        DEBUG("[MCP23017]: init - error: unable to initialize I2C bus\n");
        return MCP23017_NOI2C;
    }

    /* Release the bus */
    i2c_release(BUS);

    DEBUG("[MCP23017]: init: successful\n");
    return MCP23017_OK;
}

int mcp23017_read_port(mcp23017_t *dev, uint8_t port)
{
    uint8_t res = 0, reg;

    assert(dev);

    /* read value of given port */
    i2c_acquire(BUS);
    if (port == PORT_A) {
        res = i2c_read_reg(BUS, ADDR, MCP23017_REG_GPIOA, &reg);
    }
    else if (port == PORT_B) {
        res = i2c_read_reg(BUS, ADDR, MCP23017_REG_GPIOB, &reg);
    }
    i2c_release(BUS);

    if (res > 0) {
        DEBUG("[MCP23017]: read: successful\n");
        return reg;
    }
    else {
        DEBUG("[MCP23017]: read: not successful\n");
        return MCP23017_NODEV;
    }
}

int mcp23017_set_dir(mcp23017_t *dev, uint8_t port, uint8_t pin, uint8_t dir)
{
    uint8_t res = 0, reg;

    assert(dev);

    /* set the given pin direction */
    i2c_acquire(BUS);
    if (port == PORT_A) {
        if (dir) {
            dev->port_a_dir |= pin;
        }
        else {
            dev->port_a_dir &= ~pin;
        }
        reg = dev->port_a_dir;
        res = i2c_write_reg(BUS, ADDR, MCP23017_REG_IODIRA, reg);
    }
    else if (port == PORT_B) {
        if (dir) {
            dev->port_b_dir |= pin;
        }
        else {
            dev->port_b_dir &= ~pin;
        }
        reg = dev->port_b_dir;
        res = i2c_write_reg(BUS, ADDR, MCP23017_REG_IODIRB, reg);
    }
    i2c_release(BUS);

    if (res > 0) {
        DEBUG("[MCP23017]: set direction: successful\n");
        return MCP23017_OK;
    }
    else {
        DEBUG("[MCP23017]: set direction: not successful\n");
        return MCP23017_NODEV;
    }
}

int mcp23017_set_pull_up(mcp23017_t *dev, uint8_t port, uint8_t pin, uint8_t pull_up)
{
    uint8_t res = 0, reg;

    assert(dev);

    /* set the pin pull-up */
    i2c_acquire(BUS);
    if (port == PORT_A) {
        if (pull_up) {
            dev->port_a_pull_up |= pin;
        }
        else {
            dev->port_a_pull_up &= ~pin;
        }
        reg = dev->port_a_pull_up;
        res = i2c_write_reg(BUS, ADDR, MCP23017_REG_GPPUA, reg);
    }
    else if (port == PORT_B) {
        if (pull_up) {
            dev->port_b_pull_up |= pin;
        }
        else {
            dev->port_b_pull_up &= ~pin;
        }
        reg = dev->port_b_pull_up;
        res = i2c_write_reg(BUS, ADDR, MCP23017_REG_GPPUB, reg);
    }
    i2c_release(BUS);

    if (res > 0) {
        DEBUG("[MCP23017]: set pin pull-up: successful\n");
        return MCP23017_OK;
    }
    else {
        DEBUG("[MCP23017]: set pin pull-up: not successful\n");
        return MCP23017_NODEV;
    }
}

int mcp23017_set_pin_value(mcp23017_t *dev, uint8_t port, uint8_t pin, uint8_t value)
{
    uint8_t res = 0, reg;

    assert(dev);

    /* set the given pin value */
    i2c_acquire(BUS);
    if (port == PORT_A) {
        if (value) {
            dev->port_a_value |= pin;
        }
        else {
            dev->port_a_value &= ~pin;
        }
        reg = dev->port_a_value;
        res = i2c_write_reg(BUS, ADDR, MCP23017_REG_OLATA, reg);
    }
    else if (port == PORT_B) {
        if (value) {
            dev->port_b_value |= pin;
        }
        else {
            dev->port_b_value &= ~pin;
        }
        reg = dev->port_b_value;
        res = i2c_write_reg(BUS, ADDR, MCP23017_REG_OLATB, reg);
    }
    i2c_release(BUS);

    if (res > 0) {
        DEBUG("[MCP23017]: set pin value: successful\n");
        return MCP23017_OK;
    }
    else {
        DEBUG("[MCP23017]: set pin value: not successful\n");
        return MCP23017_NODEV;
    }
}

int mcp23017_set_int(mcp23017_t *dev, uint8_t port, uint8_t pin, uint8_t state)
{
    uint8_t res = 0, reg;

    assert(dev);

    /* set the pin Interrupt */
    i2c_acquire(BUS);
    if (port == PORT_A) {
        if (state) {
            dev->port_a_inten |= pin;
        }
        else {
            dev->port_a_inten &= ~pin;
        }
        reg = dev->port_a_inten;
        res = i2c_write_reg(BUS, ADDR, MCP23017_REG_GPINTENA, reg);
    }
    else if (port == PORT_B) {
        if (state) {
            dev->port_b_inten |= pin;
        }
        else {
            dev->port_b_inten &= ~pin;
        }
        reg = dev->port_b_inten;
        res = i2c_write_reg(BUS, ADDR, MCP23017_REG_GPINTENB, reg);
    }
    i2c_release(BUS);

    if (res > 0) {
        DEBUG("[MCP23017]: set pin interrupt: successful\n");
        return MCP23017_OK;
    }
    else {
        DEBUG("[MCP23017]: set pin interrupt: not successful\n");
        return MCP23017_NODEV;
    }
}

int mcp23017_capture_interrupt(mcp23017_t *dev, uint8_t port)
{
    uint8_t res = 0, reg;

    assert(dev);

    /* read value of given port */
    i2c_acquire(BUS);
    if (port == PORT_A) {
        res = i2c_read_reg(BUS, ADDR, MCP23017_REG_INTCAPA, &reg);
    }
    else if (port == PORT_B) {
        res = i2c_read_reg(BUS, ADDR, MCP23017_REG_INTCAPB, &reg);
    }
    i2c_release(BUS);

    if (res > 0) {
        DEBUG("[MCP23017]: read: successful\n");
        return reg;
    }
    else {
        DEBUG("[MCP23017]: read: not successful\n");
        return MCP23017_NODEV;
    }
}
