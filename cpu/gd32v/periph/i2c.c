/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 FU Berlin
 *               2018 Inria
 *               2018 HAW Hamburg
 *               2023 Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32v
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * This driver is a modified copy of the I2C driver for the STM32F1 family.
 *
 * @note This implementation only implements the 7-bit addressing polling mode.
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <errno.h>

#include "cpu.h"
#include "irq.h"
#include "mutex.h"
#include "pm_layered.h"
#include "panic.h"

#include "periph/i2c.h"
#include "periph/gpio.h"
#include "periph_conf.h"

/* Some DEBUG statements may cause delays that alter i2c functionality */
#define ENABLE_DEBUG        0
#include "debug.h"

#define TICK_TIMEOUT        (0xFFFF)

#define I2C_IRQ_PRIO        (1)
#define I2C_FLAG_READ       (I2C_READ)
#define I2C_FLAG_WRITE      (0)

#define ERROR_FLAGS         (I2C_STAT0_AERR_Msk | I2C_STAT0_LOSTARB_Msk | I2C_STAT0_BERR_Msk)

/* static function definitions */
static void _init(i2c_t dev);
static void _init_pins(i2c_t dev);
static void _init_clk(I2C_Type *i2c, uint32_t speed);
static void _deinit_pins(i2c_t dev);

static int _start(I2C_Type *dev, uint8_t address_byte, uint8_t flags,
                  size_t length);
static int _stop(I2C_Type *dev);

static int _is_sr1_mask_set(I2C_Type *i2c, uint32_t mask, uint8_t flags);
static inline int _wait_for_bus(I2C_Type *i2c);

/**
 * @brief Array holding one pre-initialized mutex for each I2C device
 */
static mutex_t locks[I2C_NUMOF];

void i2c_init(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_init(&locks[dev]);

    assert(i2c_config[dev].dev != NULL);

    /* Configure pins in idle state as open drain outputs to keep the bus lines
     * in HIGH state */
    _deinit_pins(dev);

    periph_clk_en(APB1, i2c_config[dev].rcu_mask);

    _init(dev);

    periph_clk_dis(APB1, i2c_config[dev].rcu_mask);
}

static void _init_pins(i2c_t dev)
{
    /* This is needed in case the remapped pins are used */
    if (i2c_config[dev].scl_pin == GPIO_PIN(PORT_B, 8) ||
        i2c_config[dev].sda_pin == GPIO_PIN(PORT_B, 9)) {
        /* The remapping periph clock must first be enabled */
        RCU->APB2EN |= RCU_APB2EN_AFEN_Msk;
        /* Then the remap can occur */
        AFIO->PCF0 |= AFIO_PCF0_I2C0_REMAP_Msk;
    }
    gpio_init_af(i2c_config[dev].scl_pin, GPIO_AF_OUT_OD);
    gpio_init_af(i2c_config[dev].sda_pin, GPIO_AF_OUT_OD);
}

static void _init_clk(I2C_Type *i2c, uint32_t speed)
{
    /* disable device and set ACK bit */
    i2c->CTL0 = I2C_CTL0_ACKEN_Msk;
    /* configure I2C clock */
    i2c->CTL1 = (CLOCK_APB1 / MHZ(1)) | I2C_CTL1_ERRIE_Msk;
    i2c->CKCFG = CLOCK_APB1 / (2 * speed);
    i2c->RT = (CLOCK_APB1 / 1000000) + 1;
    /* configure device */
    i2c->SADDR0 |= (1 << 14); /* datasheet: bit 14 should be kept 1 */
    i2c->SADDR0 &= ~I2C_SADDR0_ADDFORMAT_Msk; /* make sure we are in 7-bit address mode */
    /* Clear flags */
    i2c->STAT0 &= ~ERROR_FLAGS;
    /* enable device */
    i2c->CTL0 |= I2C_CTL0_I2CEN_Msk;
}

static void _init(i2c_t dev)
{
    I2C_Type *i2c = i2c_config[dev].dev;

    /* make peripheral soft reset */
    i2c->CTL0 |= I2C_CTL0_SRESET_Msk;
    i2c->CTL0 &= ~I2C_CTL0_SRESET_Msk;

    /* configure I2C clock */
    _init_clk(i2c, i2c_config[dev].speed);
}

static void _deinit_pins(i2c_t dev)
{
    /* GD32V doesn't support GPIO_OD_PU mode, i.e. external pull-ups required */
    gpio_init(i2c_config[dev].scl_pin, GPIO_OD);
    gpio_init(i2c_config[dev].sda_pin, GPIO_OD);
    gpio_set(i2c_config[dev].scl_pin);
    gpio_set(i2c_config[dev].sda_pin);
}

void i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_lock(&locks[dev]);

    /* block DEEP_SLEEP mode */
    pm_block(GD32V_PM_DEEPSLEEP);

    periph_clk_en(APB1, i2c_config[dev].rcu_mask);

    /* set the alternate function of the pins */
    _init_pins(dev);

    /* enable device */
    i2c_config[dev].dev->CTL0 |= I2C_CTL0_I2CEN_Msk;
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    /* disable device */
    i2c_config[dev].dev->CTL0 &= ~(I2C_CTL0_I2CEN_Msk);

    _wait_for_bus(i2c_config[dev].dev);

    /* Disabling the clock switches off the I2C controller, which results in
     * LOW bus lines. To avoid that the used GPIOs then draw some milliamps
     * of current via the pull-up resistors, the used GPIOs are set back to
     * GPIO_OD mode and HIGH. */
    _deinit_pins(dev);

    periph_clk_dis(APB1, i2c_config[dev].rcu_mask);

    /* unblock DEEP_SLEEP mode */
    pm_unblock(GD32V_PM_DEEPSLEEP);

    mutex_unlock(&locks[dev]);
}

int i2c_read_bytes(i2c_t dev, uint16_t address, void *data, size_t length,
                   uint8_t flags)
{
    assert(dev < I2C_NUMOF);

    I2C_Type *i2c = i2c_config[dev].dev;
    DEBUG("[i2c] read_bytes: Starting\n");

    /* Repeated start of read operations is not supported. This is exactly the
     * case if the previous transfer was a read operation (I2C_STAT1_TR == 0)
     * and was not terminated by a STOP condition (I2C_STAT1_I2CBSY == 1) and
     * the START condition is to be used (I2C_NOSTART == 0).
     */
    if (((i2c->STAT1 & (I2C_STAT1_I2CBSY_Msk | I2C_STAT1_TR_Msk)) == I2C_STAT1_I2CBSY_Msk) &&
        !(flags & I2C_NOSTART)) {
        return -EOPNOTSUPP;
    }

    int ret = _start(i2c, (address << 1) | I2C_FLAG_READ, flags, length);
    if (ret < 0) {
        if (ret == -ETIMEDOUT) {
            _init(dev);
        }
        return ret;
    }

    for (size_t i = 0; i < length; i++) {
        if (i + 1 == length && !(flags & I2C_NOSTOP)) {
            /* If data is already in the buffer we must clear before sending
             * a stop. If I2C_NOSTOP was called up to two extra bytes may be
             * clocked out on the line however they get ignored in the firmware.*/
            if ((i2c->STAT0 & I2C_STAT0_RBNE_Msk) && (length == 1)) {
                ((uint8_t*)data)[i] = i2c->DATA;
                return _stop(i2c);
            }
            /* STOP must also be sent before final read */
            ret = _stop(i2c);
            if (ret < 0) {
                return ret;
            }
        }
        /* Wait for reception to complete */
        ret = _is_sr1_mask_set(i2c, I2C_STAT0_RBNE_Msk, flags);
        if (ret < 0) {
            return ret;
        }
        ((uint8_t*)data)[i] = i2c->DATA;
    }
    DEBUG("[i2c] read_bytes: Finished reading bytes\n");
    if (flags & I2C_NOSTOP) {
        return 0;
    }
    return _wait_for_bus(i2c);
}

int i2c_write_bytes(i2c_t dev, uint16_t address, const void *data,
                    size_t length, uint8_t flags)
{
    assert(dev < I2C_NUMOF);

    int ret;

    I2C_Type *i2c = i2c_config[dev].dev;
    assert(i2c != NULL);
    DEBUG("[i2c] write_bytes: Starting\n");
    /* Length is 0 in start since we don't need to preset the stop bit */
    ret = _start(i2c, (address << 1) | I2C_FLAG_WRITE, flags, 0);
    if (ret < 0) {
        if (ret == -ETIMEDOUT) {
            _init(dev);
        }
        return ret;
    }

    /* Send out data bytes */
    for (size_t i = 0; i < length; i++) {
        DEBUG("[i2c] write_bytes: Waiting for TX reg to be free\n");
        ret = _is_sr1_mask_set(i2c, I2C_STAT0_TBE_Msk, flags);
        if (ret < 0) {
            return ret;
        }
        DEBUG("[i2c] write_bytes: TX is free so send byte\n");
        i2c->DATA = ((uint8_t*)data)[i];
    }
    /* Wait for tx reg to be empty so other calls will no interfere */
    ret = _is_sr1_mask_set(i2c, I2C_STAT0_TBE_Msk, flags);
    if (ret < 0) {
        return ret;
    }
    if (flags & I2C_NOSTOP) {
        return 0;
    }
    else {
        /* End transmission */
        DEBUG("[i2c] write_bytes: Ending transmission\n");
        ret = _stop(i2c);
        if (ret < 0) {
            return ret;
        }
        DEBUG("[i2c] write_bytes: STOP condition was send out\n");
    }

    return _wait_for_bus(i2c);
}

static int _start(I2C_Type *i2c, uint8_t address_byte, uint8_t flags,
                  size_t length)
{
    assert(i2c != NULL);

    if ((flags & I2C_ADDR10) ||
        (!(i2c->STAT1 & I2C_STAT1_I2CBSY_Msk) && (flags & I2C_NOSTART))) {
        return -EOPNOTSUPP;
    }

    /* Clear flags */
    i2c->STAT0 &= ~ERROR_FLAGS;

    if (!(flags & I2C_NOSTART)) {
        DEBUG("[i2c] start: Generate start condition\n");
        /* Generate start condition */
        i2c->CTL0 |= I2C_CTL0_START_Msk | I2C_CTL0_ACKEN_Msk;

        /* Wait for SB flag to be set */
        int ret = _is_sr1_mask_set(i2c, I2C_STAT0_SBSEND_Msk, flags & ~I2C_NOSTOP);
        if (ret < 0) {
            return ret;
        }
        DEBUG("[i2c] start: Start condition generated\n");

        DEBUG("[i2c] start: Generating address\n");
        /* Send address and read/write flag */
        if ((i2c->STAT0 & I2C_STAT0_SBSEND_Msk)) {
            i2c->DATA = (address_byte);
        }
        if (!(flags & I2C_NOSTOP) && length == 1) {
            i2c->CTL0 &= ~(I2C_CTL0_ACKEN_Msk);
        }
        /* Wait for ADDR flag to be set */
        ret = _is_sr1_mask_set(i2c, I2C_STAT0_ADDSEND_Msk, flags & ~I2C_NOSTOP);
        if (ret == -EIO){
            /* Since NACK happened during start it means no device connected */
            return -ENXIO;
        }

        /* Wait until I2C_STAT0_ADDSEND is cleared. To clear I2C_STAT0_ADDSEND
         * it is necessary to read STAT0 followed by reading STAT1 */
        while ((i2c->STAT0 & I2C_STAT0_ADDSEND_Msk) && i2c->STAT1) { }

        if (!(flags & I2C_NOSTOP) && length == 1) {
            /* Stop must also be sent before final read */
            i2c->CTL0 |= (I2C_CTL0_STOP_Msk);
        }
        DEBUG("[i2c] start: Address generated\n");
        return ret;
    }
    return 0;
}

static int _is_sr1_mask_set(I2C_Type *i2c, uint32_t mask, uint8_t flags)
{
    DEBUG("[i2c] _is_sr1_mask_set: waiting to set %04X\n", (uint16_t)mask);
    uint16_t tick = TICK_TIMEOUT;
    while (tick--) {
        uint32_t sr1 = i2c->STAT0;
        if (sr1 & I2C_STAT0_AERR_Msk) {
            DEBUG("[i2c] is_sr1_mask_set: NACK received\n");
            i2c->STAT0 &= ~ERROR_FLAGS;
            if (!(flags & I2C_NOSTOP)) {
                _stop(i2c);
            }
            return -EIO;
        }
        if ((sr1 & I2C_STAT0_LOSTARB_Msk) || (sr1 & I2C_STAT0_BERR_Msk)) {
            DEBUG("[i2c] is_sr1_mask_set: arb lost or bus ERROR_FLAGS\n");
            i2c->STAT0 &= ~ERROR_FLAGS;
            _stop(i2c);
            return -EAGAIN;
        }
        if (sr1 & mask) {
            i2c->STAT0 &= ~ERROR_FLAGS;
            return 0;
        }
    }
    /*
    * If timeout occurs this means a problem that must be handled on a higher
    * level.  A SWRST is recommended by the datasheet.
    */
    i2c->STAT0 &= ~ERROR_FLAGS;
    _stop(i2c);
    return -ETIMEDOUT;
}

static int _stop(I2C_Type *i2c)
{
    /* send STOP condition */
    DEBUG("[i2c] stop: Generate stop condition\n");
    i2c->CTL0 &= ~(I2C_CTL0_ACKEN_Msk);
    i2c->CTL0 |= I2C_CTL0_STOP_Msk;
    uint16_t tick = TICK_TIMEOUT;
    while ((i2c->CTL0 & I2C_CTL0_STOP_Msk) && tick--) {}
    if (!tick) {
        return -ETIMEDOUT;
    }
    DEBUG("[i2c] stop: Stop condition succeeded\n");
    if (_wait_for_bus(i2c) < 0) {
        return -ETIMEDOUT;
    }
    DEBUG("[i2c] stop: Bus is free\n");
    return 0;
}

static inline int _wait_for_bus(I2C_Type *i2c)
{
    uint16_t tick = TICK_TIMEOUT;
    while ((i2c->STAT1 & I2C_STAT1_I2CBSY_Msk) && tick--) {}
    if (!tick) {
        return -ETIMEDOUT;
    }
    return 0;
}
