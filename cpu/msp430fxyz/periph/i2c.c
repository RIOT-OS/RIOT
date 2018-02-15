/*
 * Copyright (C) 2016 Pavol Malosek <malo@25cmsquare.io>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_msp430fxyz
 * @{
 *
 * @file
 * @brief       I2C functions implementation.
 *
 * @author      Pavol Malosek <malo@25cmsquare.io>
 *
 * @}
 */

#include "periph_conf.h"
#include "mutex.h"
#include "periph/i2c.h"
#include "periph/gpio.h"
#include "periph/i2c_sw.h"

#if (I2C_NUMOF > 2U)
#error "Only two sw I2C buses are supported!"
#endif

#if ((I2C_0_EN == 1) || (I2C_1_EN ==1))
static mutex_t locks[] =  {
    [I2C_DEV(0)] = MUTEX_INIT,
    [I2C_DEV(1)] = MUTEX_INIT,
};
#endif

int i2c_acquire(i2c_t dev)
{
#if I2C_0_EN
    if (dev == I2C_DEV(0)) {
        mutex_lock(&locks[dev]);
        return 0;
    }
#endif
#if I2C_1_EN
    if (dev == I2C_DEV(1)) {
        mutex_lock(&locks[dev]);
        return 0;
    }
#endif
    return -1;
}

int i2c_release(i2c_t dev)
{
#if I2C_0_EN
    if (dev == I2C_DEV(0)) {
        mutex_unlock(&locks[dev]);
        return 0;
    }
#endif
#if I2C_1_EN
    if (dev == I2C_DEV(1)) {
        mutex_unlock(&locks[dev]);
        return 0;
    }
#endif
    return -1;
}

#ifdef I2C_SOFTWARE

/* Do not have open drain outputs - config as input */
void i2c_sw_set_SCL(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_DEV(0):
            gpio_init(I2C_0_SCL_PIN, GPIO_IN);
            break;
#endif
#if I2C_1_EN
        case I2C_DEV(1):
            gpio_init(I2C_1_SCL_PIN, GPIO_IN);
            break;
#endif
        default:
            break;
    }
}

/* Active drive to low */
void i2c_sw_clear_SCL(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_DEV(0):
            gpio_init(I2C_0_SCL_PIN, GPIO_OUT);
            break;
#endif
#if I2C_1_EN
        case I2C_DEV(1):
            gpio_init(I2C_1_SCL_PIN, GPIO_OUT);
            break;
#endif
        default:
            break;
    }
}

/* Do not have open drain outputs - config as input */
void i2c_sw_set_SDA(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_DEV(0):
            gpio_init(I2C_0_SDA_PIN, GPIO_IN);
            break;
#endif
#if I2C_1_EN
        case I2C_DEV(1):
            gpio_init(I2C_1_SDA_PIN, GPIO_IN);
            break;
#endif
        default:
            break;
    }
}

/* Active drive to low */
void i2c_sw_clear_SDA(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_DEV(0):
            gpio_init(I2C_0_SDA_PIN, GPIO_OUT);
            break;
#endif
#if I2C_1_EN
        case I2C_DEV(1):
            gpio_init(I2C_1_SDA_PIN, GPIO_OUT);
            break;
#endif
        default:
            break;
    }
}

int i2c_sw_read_SCL(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_DEV(0):
            return gpio_read(I2C_0_SCL_PIN) ? 1 : 0;
#endif
#if I2C_1_EN
        case I2C_DEV(1):
            return gpio_read(I2C_1_SCL_PIN) ? 1 : 0;
#endif
        default:
            break;
    }

    return -1;
}

int i2c_sw_read_SDA(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_DEV(0):
            return gpio_read(I2C_0_SDA_PIN) ? 1 : 0;
#endif
#if I2C_1_EN
        case I2C_DEV(1):
            return gpio_read(I2C_1_SDA_PIN) ? 1 : 0;
#endif
        default:
            break;
    }

    return -1;
}

void i2c_sw_delay_2_35us(void)
{
    /* On slow platform there is no need for actual delay
     * since calling this function takes much longer than 2.35us */
}

void i2c_sw_delay_4_70us(void)
{
    i2c_sw_delay_2_35us();
    i2c_sw_delay_2_35us();
}

void i2c_sw_delay_4_00us(void)
{
    i2c_sw_delay_2_35us();
    i2c_sw_delay_2_35us();
}

int i2c_sw_init_gpio(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_DEV(0):
            gpio_periph_mode(I2C_0_SCL_PIN, false);
            gpio_periph_mode(I2C_0_SDA_PIN, false);
            break;
#endif
#if I2C_1_EN
        case I2C_DEV(1):
            gpio_periph_mode(I2C_1_SCL_PIN, false);
            gpio_periph_mode(I2C_1_SDA_PIN, false);
            break;
#endif
        default:
            return -1;
    }
    return 0;
}
#else
/* Here goes hw i2c implementation. */
 #endif
