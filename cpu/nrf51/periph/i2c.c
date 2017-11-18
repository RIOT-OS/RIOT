/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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
#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/i2c.h"
#include "periph_conf.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/**
 * @brief   If any of the 4 lower bits are set, the speed value is invalid
 */
#define INVALID_SPEED_MASK  (0x0f)

/**
 * @brief   Initialized bus locks (we have a maximum of two devices...)
 */
static mutex_t locks[] = {
    MUTEX_INIT,
    MUTEX_INIT
};

static inline NRF_TWI_Type *dev(i2c_t bus)
{
    return i2c_config[bus].dev;
}

static int error(i2c_t bus)
{
    DEBUG("[i2c] error 0x%02x\n", (int)dev(bus)->ERRORSRC);
    dev(bus)->ERRORSRC = (TWI_ERRORSRC_DNACK_Clear |
                          TWI_ERRORSRC_ANACK_Clear |
                          TWI_ERRORSRC_OVERRUN_Clear);
    dev(bus)->EVENTS_ERROR = 0;
    return -1;
}

static int write(i2c_t bus, uint8_t addr, const void *data, int len, int stop)
{
    assert(len > 0);
    if (!(bus < I2C_NUMOF)) {
        return -1;
    }

    uint8_t *buf = (uint8_t *)data;

    DEBUG("i2c: writing %i byte to the bus\n", len);

    dev(bus)->ADDRESS = (addr & 0x7f);

    for (int i = 0; i < len; i++) {
        dev(bus)->TXD = *buf++;
        dev(bus)->EVENTS_TXDSENT = 0;
        dev(bus)->TASKS_STARTTX = 1;
        while (!(dev(bus)->EVENTS_TXDSENT) && !(dev(bus)->EVENTS_ERROR)) {}
        if (dev(bus)->EVENTS_ERROR) {
            return error(bus);
        }
    }

    if (stop) {
        dev(bus)->EVENTS_STOPPED = 0;
        dev(bus)->TASKS_STOP = 1;
        while (!(dev(bus)->EVENTS_STOPPED) && !(dev(bus)->EVENTS_ERROR)) {}
        if (dev(bus)->EVENTS_ERROR) {
            return error(bus);
        }
    }

    return len;
}

int i2c_init_master(i2c_t bus, i2c_speed_t speed)
{
    if (!(bus < I2C_NUMOF)) {
        return -1;
    }
    if (speed & INVALID_SPEED_MASK) {
        return -2;
    }

    /* power on the bus */
    dev(bus)->POWER = TWI_POWER_POWER_Enabled;

    /* pin configuration */
    NRF_GPIO->PIN_CNF[i2c_config[bus].pin_scl] = (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos);
    NRF_GPIO->PIN_CNF[i2c_config[bus].pin_scl] = (GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos);

    dev(bus)->PSELSCL = i2c_config[bus].pin_scl;
    dev(bus)->PSELSDA = i2c_config[bus].pin_sda;

    NRF_PPI->CHENCLR = (1 << i2c_config[bus].ppi);
    NRF_PPI->CH[i2c_config[bus].ppi].EEP = (uint32_t)&dev(bus)->EVENTS_BB;

    /* bus clock speed configuration */
    dev(bus)->FREQUENCY = speed;
    /* enable the device */
    dev(bus)->ENABLE = TWI_ENABLE_ENABLE_Enabled;

    return 0;
}

int i2c_acquire(i2c_t bus)
{
    if (!(bus < I2C_NUMOF)) {
        return -1;
    }
    mutex_lock(&locks[bus]);
    return 0;
}

int i2c_release(i2c_t bus)
{
    if (!(bus < I2C_NUMOF)) {
        return -1;
    }
    mutex_unlock(&locks[bus]);
    return 0;
}

int i2c_read_byte(i2c_t bus, uint8_t address, void *data)
{
    return i2c_read_bytes(bus, address, data, 1);
}

int i2c_read_bytes(i2c_t bus, uint8_t address, void *data, int length)
{
    assert(length > 0);
    if (!(bus < I2C_NUMOF)) {
        return -1;
    }
    uint8_t *in_buf = (uint8_t *)data;

    DEBUG("[i2c] reading %i byte from the bus\n", length);

    /* set the client address */
    dev(bus)->ADDRESS = (address & 0x7f);

    /* setup PPI channel as alternative to the broken SHORTS
     * -> see PAN notice #36: "Shortcuts described in nRF51 Reference Manual are
     *                         not functional." */
    if (length == 1) {
        NRF_PPI->CH[i2c_config[bus].ppi].TEP = (uint32_t)&dev(bus)->TASKS_STOP;
    }
    else {
        NRF_PPI->CH[i2c_config[bus].ppi].TEP = (uint32_t)&dev(bus)->TASKS_SUSPEND;
    }
    NRF_PPI->CHENSET = (1 << i2c_config[bus].ppi);

    dev(bus)->EVENTS_RXDREADY = 0;
    dev(bus)->EVENTS_STOPPED = 0;
    dev(bus)->TASKS_STARTRX = 1;

    for (int i = (length - 1); i >= 0; i--) {
        while (!(dev(bus)->EVENTS_RXDREADY) && !(dev(bus)->EVENTS_ERROR)) {}
        if (dev(bus)->EVENTS_ERROR) {
            return error(bus);
        }

        *in_buf++ = (uint8_t)dev(bus)->RXD;

        if (i == 1) {
            NRF_PPI->CH[i2c_config[bus].ppi].TEP = (uint32_t)&dev(bus)->TASKS_STOP;
        }

        dev(bus)->EVENTS_RXDREADY = 0;
        dev(bus)->TASKS_RESUME = 1;
    }

    /* wait for the device to finish up */
    while (dev(bus)->EVENTS_STOPPED == 0) {}
    NRF_PPI->CHENCLR = (1 << i2c_config[bus].ppi);

    return length;
}

int i2c_read_reg(i2c_t bus, uint8_t address, uint8_t reg, void *data)
{
    write(bus, address, &reg, 1, 0);
    return i2c_read_bytes(bus, address, data, 1);
}

int i2c_read_regs(i2c_t bus, uint8_t address, uint8_t reg,
                  void *data, int length)
{
    write(bus, address, &reg, 1, 0);
    return i2c_read_bytes(bus, address, data, length);
}

int i2c_write_byte(i2c_t bus, uint8_t address, uint8_t data)
{
    return write(bus, address, &data, 1, 1);
}

int i2c_write_bytes(i2c_t bus, uint8_t address, const void *data, int length)
{
    return write(bus, address, data, length, 1);
}

int i2c_write_reg(i2c_t bus, uint8_t address, uint8_t reg, uint8_t data)
{
    write(bus, address, &reg, 1, 0);
    return write(bus, address, &data, 1, 1);
}

int i2c_write_regs(i2c_t bus, uint8_t address, uint8_t reg,
                   const void *data, int length)
{
    write(bus, address, &reg, 1, 0);
    return write(bus, address, data, length, 1);
}
