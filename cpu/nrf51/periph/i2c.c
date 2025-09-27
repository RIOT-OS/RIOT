/*
 * SPDX-FileCopyrightText: 2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_nrf51
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */
/**
 * @ingroup     cpu_nrf51
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2V driver implementation
 *
 * @}
 */
#include <assert.h>
#include <errno.h>
#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/i2c.h"
#include "periph_conf.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/**
 * @brief   If any of the 4 lower bits are set, the speed value is invalid
 */
#define INVALID_SPEED_MASK  (0x0f)

/**
 * @brief   Initialized bus locks
 */
static mutex_t locks[I2C_NUMOF];

static inline NRF_TWI_Type *i2c(i2c_t dev)
{
    return i2c_config[dev].dev;
}

static int error(i2c_t dev)
{
    i2c(dev)->EVENTS_ERROR = 0;
    DEBUG("[i2c] error 0x%02x\n", (int)i2c(dev)->ERRORSRC);
    if (i2c(dev)->ERRORSRC & TWI_ERRORSRC_ANACK_Msk) {
        i2c(dev)->ERRORSRC = TWI_ERRORSRC_ANACK_Msk;
        DEBUG("[i2c] check_error: NACK on address byte\n");
        return -ENXIO;
    }
    if (i2c(dev)->ERRORSRC & TWI_ERRORSRC_DNACK_Msk) {
        i2c(dev)->ERRORSRC = TWI_ERRORSRC_DNACK_Msk;
        DEBUG("[i2c] check_error: NACK on data byte\n");
        return -EIO;
    }

    return 0;
}

static int write(i2c_t dev, uint16_t addr, const void *data, int len,
                 uint8_t flags)
{
    assert(len > 0);
    assert(dev < I2C_NUMOF);

    uint8_t *buf = (uint8_t *)data;

    DEBUG("[i2c] writing %i byte to the bus\n", len);

    i2c(dev)->ADDRESS = (addr & 0x7f);

    for (int i = 0; i < len; i++) {
        i2c(dev)->TXD = *buf++;
        i2c(dev)->EVENTS_TXDSENT = 0;
        i2c(dev)->TASKS_STARTTX = 1;
        while (!(i2c(dev)->EVENTS_TXDSENT) && !(i2c(dev)->EVENTS_ERROR)) {}
        if (i2c(dev)->EVENTS_ERROR) {
            return error(dev);
        }
    }

    if (!(flags & I2C_NOSTOP)) {
        i2c(dev)->EVENTS_STOPPED = 0;
        i2c(dev)->TASKS_STOP = 1;
        while (!(i2c(dev)->EVENTS_STOPPED) && !(i2c(dev)->EVENTS_ERROR)) {}
        if (i2c(dev)->EVENTS_ERROR) {
            return error(dev);
        }
    }

    return 0;
}

void i2c_init(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    /* Initialize mutex */
    mutex_init(&locks[dev]);

    /* power on the bus */
    i2c(dev)->POWER = TWI_POWER_POWER_Enabled;

    /* pin configuration */
    NRF_GPIO->PIN_CNF[i2c_config[dev].pin_scl] = (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos);
    NRF_GPIO->PIN_CNF[i2c_config[dev].pin_scl] = (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos);

    i2c(dev)->PSELSCL = i2c_config[dev].pin_scl;
    i2c(dev)->PSELSDA = i2c_config[dev].pin_sda;

    NRF_PPI->CHENCLR = (1 << i2c_config[dev].ppi);
    NRF_PPI->CH[i2c_config[dev].ppi].EEP = (uint32_t)&i2c(dev)->EVENTS_BB;

    /* bus clock speed configuration */
    i2c(dev)->FREQUENCY = i2c_config[dev].speed;
    /* enable the device */
    i2c(dev)->ENABLE = TWI_ENABLE_ENABLE_Enabled;
}

void i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_lock(&locks[dev]);
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_unlock(&locks[dev]);
}

int i2c_read_bytes(i2c_t dev, uint16_t address, void *data, size_t length,
                   uint8_t flags)
{
    assert(length > 0);
    assert(dev < I2C_NUMOF);

    if (flags & (I2C_NOSTART | I2C_REG16 | I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }

    uint8_t *in_buf = (uint8_t *)data;

    DEBUG("[i2c] reading %i byte from the bus\n", length);

    /* set the client address */
    i2c(dev)->ADDRESS = (address & 0x7f);

    /* setup PPI channel as alternative to the broken SHORTS
     * -> see PAN notice #36: "Shortcuts described in nRF51 Reference Manual are
     *                         not functional." */
    if (length == 1) {
        NRF_PPI->CH[i2c_config[dev].ppi].TEP = (uint32_t)&i2c(dev)->TASKS_STOP;
    }
    else {
        NRF_PPI->CH[i2c_config[dev].ppi].TEP = (uint32_t)&i2c(dev)->TASKS_SUSPEND;
    }
    NRF_PPI->CHENSET = (1 << i2c_config[dev].ppi);

    i2c(dev)->EVENTS_RXDREADY = 0;
    i2c(dev)->EVENTS_STOPPED = 0;
    i2c(dev)->TASKS_STARTRX = 1;

    for (int i = (length - 1); i >= 0; i--) {
        while (!(i2c(dev)->EVENTS_RXDREADY) && !(i2c(dev)->EVENTS_ERROR)) {}
        if (i2c(dev)->EVENTS_ERROR) {
            return error(dev);
        }

        *in_buf++ = (uint8_t)i2c(dev)->RXD;

        if (i == 1) {
            NRF_PPI->CH[i2c_config[dev].ppi].TEP = (uint32_t)&i2c(dev)->TASKS_STOP;
        }

        i2c(dev)->EVENTS_RXDREADY = 0;
        i2c(dev)->TASKS_RESUME = 1;
    }

    /* wait for the device to finish up */
    while (i2c(dev)->EVENTS_STOPPED == 0) {}
    NRF_PPI->CHENCLR = (1 << i2c_config[dev].ppi);

    return 0;
}

int i2c_read_regs(i2c_t dev, uint16_t address, uint16_t reg,
                  void *data, size_t length, uint8_t flags)
{
    if (flags & (I2C_NOSTART | I2C_REG16 | I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }

    write(dev, address, &reg, 1, flags | I2C_NOSTOP);
    return i2c_read_bytes(dev, address, data, length, flags);
}

int i2c_write_bytes(i2c_t dev, uint16_t address, const void *data, size_t length,
                    uint8_t flags)
{
    if (flags & (I2C_NOSTART | I2C_REG16 | I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }

    return write(dev, address, data, length, flags);
}

int i2c_write_regs(i2c_t dev, uint16_t address, uint16_t reg,
                   const void *data, size_t length, uint8_t flags)
{
    if (flags & (I2C_NOSTART | I2C_REG16 | I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }

    write(dev, address, &reg, 1, flags | I2C_NOSTOP);
    return write(dev, address, data, length, flags);
}
