/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx150x
 * @{
 *
 * @file
 * @brief       SX150X driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <string.h>

#include "assert.h"
#include "sx150x.h"

#ifdef MODULE_SX1509
#include "sx1509_regs.h"
#else
#include "sx15078_regs.h"
#endif

#define ENABLE_DEBUG (1)
#include "debug.h"

#define I2C_CLK             I2C_SPEED_FAST

#define CHECK(expr)         if (expr != SX150X_OK) { goto err; }

/* define device module specific configuration */
#if defined(MODULE_SX1507)
#define PIN_NUMOF           (4U)
#define MASK(pin)           (1 << pin)
#elif defined(MODULE_SX1508)
#define PIN_NUMOF           (8U)
#define MASK(pin)           (1 << pin)
#elif defined(MODULE_SX1509)
#define PIN_NUMOF           (16U)
#define MASK(pin)           (1 << (pin & 0x7))
#endif

static int _reg_read(const sx150x_t *dev, uint8_t reg, uint8_t *val)
{
    if (i2c_read_reg(dev->bus, dev->addr, reg, val) != 1) {
        return SX150X_BUSERR;
    }
    return SX150X_OK;
}

static int _reg_write(const sx150x_t *dev, uint8_t reg, uint8_t val)
{
    // DEBUG("write reg 0x%02x, val 0x%02x\n", (int)reg, (int)val);
    DEBUG("a\n");
    if (i2c_write_reg(dev->bus, dev->addr, reg, val) != 1) {
        return SX150X_BUSERR;
    }
    return SX150X_OK;
}

static int _reg_write_u16(const sx150x_t *dev, uint8_t reg, uint16_t val)
{
    if (i2c_write_regs(dev->bus, dev->addr, reg, &val, 2) != 2) {
        return SX150X_BUSERR;
    }
    return SX150X_OK;
}

static int reg_set(const sx150x_t *dev, uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    if (_reg_read(dev, reg, &tmp) != SX150X_OK) {
        return SX150X_BUSERR;
    }
    // DEBUG("SET, before 0x%02x\n", (int)tmp);
    tmp |= mask;
    // DEBUG("SET, after  0x%02x\n", (int)tmp);
    return _reg_write(dev, reg, tmp);
}

static int reg_clr(const sx150x_t *dev, uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    if (_reg_read(dev, reg, &tmp) != SX150X_OK) {
        return SX150X_BUSERR;
    }
    tmp &= ~(mask);
    return _reg_write(dev, reg, tmp);
}

static int reg_tgl(const sx150x_t *dev, uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    if (_reg_read(dev, reg, &tmp) != SX150X_OK) {
        return SX150X_BUSERR;
    }
    tmp ^= mask;
    return _reg_write(dev, reg, tmp);
}

int sx150x_init(sx150x_t *dev, const sx150x_params_t *params)
{
    assert(dev && params);

    int res = SX150X_OK;
    uint8_t tmp;

    memcpy(dev, params, sizeof(sx150x_params_t));

    DEBUG("[sx150x] init: initializing driver now\n");

    /* initialize I2C bus */
    if (i2c_init_master(dev->bus, I2C_CLK) != 0) {
        DEBUG("[sx150x] init: error - unable to initialize I2C bus\n");
        res = SX150X_NOBUS;
        goto exit;
    }

    DEBUG("[sx150x] init master ok\n");

    /* try to get access to the bus */
    if (i2c_acquire(dev->bus) != 0) {
        DEBUG("[sx150x] init: unable to acquire bus\n");
        res = SX150X_BUSERR;
        goto exit;
    }

    DEBUG("[sx150x] acquire ok\n");

    /* do SW reset and read input disable register for testing the connection */
    res = _reg_write(dev, REG_RESET, RESET_SEQ_1);
    if (res != SX150X_OK) {
        goto exit;
    }
    res = _reg_write(dev, REG_RESET, RESET_SEQ_2);
    if (res != SX150X_OK) {
        goto exit;
    }

    res = _reg_read(dev, REG_INPUT_DISABLE(0), &tmp);
    if ((res != SX150X_OK) || (tmp != 0)) {
        DEBUG("[sx150x] init: error - unable to read from device\n");
        res = SX150X_NODEV;
        goto exit;
    }

#if ENABLE_DEBUG
    if (res == SX150X_OK) {
        DEBUG("[sx150x] init: initialization successful\n");
    }
    else {
        DEBUG("[sx150x] init: initialization failed (%i)\n", res);
    }
#endif

exit:
    i2c_release(dev->bus);
    return res;
}

// #include "xtimer.h"
// static void sleep(void)
// {
//     xtimer_usleep(250 * 1000);
// }
/* REMOVE */
// static void dump(const sx150x_t *dev)
// {
//     for (uint8_t reg = 0x0a; reg <= 0x11; reg++) {
//         uint8_t tmp;
//         _reg_read(dev, reg, &tmp);
//         printf("Reg 0x%02x := 0x%02x\n", (int)reg, (int)tmp);
//     }
// }

static void val(sx150x_t *dev, uint8_t reg, const char *out)
{
    uint8_t tmp;
    _reg_read(dev, reg, &tmp);
    DEBUG("%s (0x%02x) is 0x%02x\n", out, (int)reg, (int)tmp);
}

int sx150x_gpio_init(sx150x_t *dev, unsigned pin, gpio_mode_t mode)
{
    assert(dev && (pin < PIN_NUMOF));

    (void)mode;
    i2c_acquire(dev->bus);

    DEBUG("init pin %i\n", (int)pin);

    val(dev, REG_INPUT_DISABLE(pin), "INPUT_DISABLE");
    val(dev, REG_DIR(pin), "DIR");

    // _reg_write(dev, REG_INPUT_DISABLE(pin), 0x53);

    // _reg_read(dev, REG_INPUT_DISABLE(pin), &tmp);
    // DEBUG("reg 0x%02x is 0x%02x\n", (int)REG_INPUT_DISABLE(pin), (int)tmp);

    // DEBUG("[sx] gpio init: pin is %i (mask 0x%02x)\n", (int)pin, (int)MASK(pin));
    // DEBUG("[sx] gpio init: INPUT DISABLE reg is 0x%02x\n", (int)REG_INPUT_DISABLE(pin));

    CHECK(reg_set(dev, REG_INPUT_DISABLE(pin), MASK(pin)));
    CHECK(reg_clr(dev, REG_DIR(pin), MASK(pin)));

    val(dev, REG_INPUT_DISABLE(pin), "INPUT_DISABLE");
    val(dev, REG_DIR(pin), "DIR");

    // DEBUG("play with data reg\n");
    // val(dev, REG_DATA(pin), "DATA");
    // reg_set(dev, REG_DATA(pin), MASK(pin));
    // val(dev, REG_DATA(pin), "DATA");
    // reg_clr(dev, REG_DATA(pin), MASK(pin));
    // val(dev, REG_DATA(pin), "DATA");


//     val(dev, IND, "IND");
//     _reg_write(dev, DIR, 0x1f);
//     val(dev, DIR, "DIR");
//     _reg_write(dev, OD, 0xe0);
//     val(dev, OD, "OD ");

//     for (int s = 5; s <= 7; s++) {
//         for (int i = 0; i < 3; i++) {
//             _reg_write(dev, DAT, ~(1 << s) & 0xe0);
//             val(dev, DAT, "DAT");
//             sleep();
//             reg_set(dev, DAT, 0xe0);
//             val(dev, DAT, "DAT");
//             sleep();
//         }
//     }

// #define IND2     0x01
// #define DIR2     0x0f
// #define OD2      0x0b
// #define DAT2     0x11

//     val(dev, IND2, "IND2");
//     _reg_write(dev, DIR2, 0x1f);
//     val(dev, DIR2, "DIR2");
//     _reg_write(dev, OD2, 0xe0);
//     val(dev, OD2, "OD2 ");

//     for (int s = 5; s <= 7; s++) {
//         for (int i = 0; i < 3; i++) {
//             reg_clr(dev, DAT2, (1 << s));
//             val(dev, DAT2, "DAT2");
//             sleep();
//             reg_set(dev, DAT2, 0xe0);
//             val(dev, DAT2, "DAT2");
//             sleep();
//         }
//     }


    // _reg_write(dev, 0x0f, 0x1f);
    // _reg_write(dev, 0x0b, 0xe0);


    // for (int i = 0; i < 4; i++) {
    //     reg_set(dev, 0x11, 0x20);
    //     sleep();
    //     reg_clr(dev, 0x11, 0x20);
    //     sleep();
    // }
    // DEBUG("ON 0x80\n");
    // dump(dev);
    // sleep();
    // _reg_write(dev, 0x11, 0x40);
    // DEBUG("ON 0x40\n");
    // dump(dev);
    // sleep();
    // _reg_write(dev, 0x11, 0x20);
    // DEBUG("ON 0x20\n");
    // dump(dev);
    // sleep();
    // _reg_write(dev, 0x11, 0xe0);
    // DEBUG("ON 0xe\n");
    // dump(dev);
    // sleep();
    // _reg_write(dev, 0x11, 0x00);
    // DEBUG("ON 0x00\n");
    // dump(dev);
    // sleep();
    // _reg_write(dev, 0x11, 0x00);


// exit:
    DEBUG("[sx150x] gpio_init: done\n");
    i2c_release(dev->bus);
    return SX150X_OK;

err:
    i2c_release(dev->bus);
    return SX150X_BUSERR;
}

int sx150x_gpio_set(sx150x_t *dev, unsigned pin)
{
    assert(dev && (pin < PIN_NUMOF));

    DEBUG("-> gpio set pin %i (mask 0x%02x)\n", (int)pin, (int)MASK(pin));
    i2c_acquire(dev->bus);
    dev->data |= (1 << pin);
    _reg_write_u16(dev, REG_DATA(15), dev->data);
    i2c_release(dev->bus);
    // DEBUG("--- RELEASED ---\n");
    return SX150X_OK;
}

int sx150x_gpio_clear(sx150x_t *dev, unsigned pin)
{
    assert(dev && (pin < PIN_NUMOF));

    DEBUG("-> gpio clear pin %i (mask 0x%02x)\n", (int)pin, (int)MASK(pin));
    i2c_acquire(dev->bus);
    dev->data &= ~(1 << pin);
    _reg_write_u16(dev, REG_DATA(15), dev->data);

    // dump(dev);
    // int ret = reg_clr(dev, REG_DATA(pin), MASK(pin));
    // dump(dev);

    i2c_release(dev->bus);
    // DEBUG("--- RELEASED ---\n");
    return SX150X_OK;
}

int sx150x_gpio_toggle(sx150x_t *dev, unsigned pin)
{
    assert(dev && (pin < PIN_NUMOF));

    i2c_acquire(dev->bus);
    CHECK(reg_tgl(dev, REG_DATA(pin), MASK(pin)));

    i2c_release(dev->bus);
    return SX150X_OK;
err:
    i2c_release(dev->bus);
    return SX150X_BUSERR;
}


// int sx150x_write(sx150x_t *dev, unsigned pin, int value)
// {
//     if (value) {
//         return sx150x_set(dev, pin);
//     }
//     else {
//         return sx150x_clear(dev, pin);
//     }
// }
