/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 FU Berlin
 *               2018 Inria
 *               2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * This driver supports the STM32 F1, F2, L1, and F4 families.
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
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <errno.h>

#include "cpu.h"
#include "mutex.h"
#include "pm_layered.h"
#include "panic.h"

#include "periph/i2c.h"
#include "periph/gpio.h"
#include "periph_conf.h"

/* Some DEBUG statements may cause delays that alter i2c functionality.
 * E.g. on STM32F1 the delay can cause issues in the state machine that
 * prevent communication. Using faster stdio than UART can mitigate this. */
#define ENABLE_DEBUG        0
#include "debug.h"

#define TICK_TIMEOUT        (0xFFFF)

#define I2C_IRQ_PRIO        (1)
#define I2C_FLAG_READ       (I2C_READ)
#define I2C_FLAG_WRITE      (0)

#define ERROR_FLAG          (I2C_SR1_AF | I2C_SR1_ARLO | I2C_SR1_BERR)

/* static function definitions */
static void _init(i2c_t dev);
static void _i2c_init(I2C_TypeDef *i2c, uint32_t clk, uint32_t ccr);
static int _i2c_start(I2C_TypeDef *dev, uint8_t address_byte, uint8_t flags,
                  size_t length);
static int _stop(I2C_TypeDef *dev);
static int _is_sr1_mask_set(I2C_TypeDef *i2c, uint32_t mask, uint8_t flags);
static inline int _wait_for_bus(I2C_TypeDef *i2c);
static void _init_pins(i2c_t dev);
static void _deinit_pins(i2c_t dev);
static void _disable_periph(i2c_t dev);
static void _enable_periph(i2c_t dev);

/**
 * @brief Array holding one pre-initialized mutex for each I2C device
 */
static mutex_t locks[I2C_NUMOF];

void i2c_init(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_init(&locks[dev]);

    assert(i2c_config[dev].dev != NULL);

    periph_clk_en(i2c_config[dev].bus, i2c_config[dev].rcc_mask);
    NVIC_SetPriority(i2c_config[dev].irqn, I2C_IRQ_PRIO);
    NVIC_EnableIRQ(i2c_config[dev].irqn);

    _init(dev);

#if defined(CPU_FAM_STM32F4)
    /* make sure the analog filters don't hang -> see errata sheet 2.14.7 */
    if (i2c_config[dev].dev->SR2 & I2C_SR2_BUSY) {
        /* disable peripheral */
        i2c_config[dev].dev->CR1 &= ~I2C_CR1_PE;
        /* toggle both pins to reset analog filter */
        gpio_init(i2c_config[dev].scl_pin, GPIO_OD);
        gpio_init(i2c_config[dev].sda_pin, GPIO_OD);
        gpio_set(i2c_config[dev].sda_pin);
        gpio_set(i2c_config[dev].scl_pin);
        gpio_clear(i2c_config[dev].sda_pin);
        gpio_clear(i2c_config[dev].scl_pin);
        gpio_set(i2c_config[dev].sda_pin);
        gpio_set(i2c_config[dev].scl_pin);
        _init(dev);
    }
#endif
    _disable_periph(dev);
}

static void _init_pins(i2c_t dev)
{
    /* configure pins */
#ifdef CPU_FAM_STM32F1
    /* This is needed in case the remapped pins are used */
    if (i2c_config[dev].scl_pin == GPIO_PIN(PORT_B, 8) ||
        i2c_config[dev].sda_pin == GPIO_PIN(PORT_B, 9)) {
        /* The remapping periph clock must first be enabled */
        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
        /* Then the remap can occur */
        afio_mapr_write(afio_mapr_read() | AFIO_MAPR_I2C1_REMAP);
    }
    gpio_init_af(i2c_config[dev].scl_pin, GPIO_AF_OUT_OD);
    gpio_init_af(i2c_config[dev].sda_pin, GPIO_AF_OUT_OD);
#else
    gpio_init(i2c_config[dev].scl_pin, GPIO_OD_PU);
    gpio_init(i2c_config[dev].sda_pin, GPIO_OD_PU);
    gpio_init_af(i2c_config[dev].scl_pin, i2c_config[dev].scl_af);
    gpio_init_af(i2c_config[dev].sda_pin, i2c_config[dev].sda_af);
#endif
}

static void _deinit_pins(i2c_t dev)
{
    /* Releasing pins as open drain and as set, so that the pull ups can pull
     * the signal high. If we would release the pins as push-pull output, this
     * could be unpleasant when an I2C device drives the signal low (e.g. for
     * clock stretching) while the MCU would driving the same signal high.
     */
    gpio_set(i2c_config[dev].scl_pin);
    gpio_set(i2c_config[dev].sda_pin);
    gpio_init(i2c_config[dev].scl_pin, GPIO_OD);
    gpio_init(i2c_config[dev].sda_pin, GPIO_OD);
}

static void _disable_periph(i2c_t dev)
{
    /* Clearing PE will not abort ongoing transfer, but only kick in when any
     * current transfer is done. So we can do this at any point in time */
    i2c_config[dev].dev->CR1 &= ~(I2C_CR1_PE);

    /* Wait for bus being cleared */
    _wait_for_bus(i2c_config[dev].dev);

    /* On STM32F1: Detach pins from I2C peripheral before disabling the clock
     * to it, otherwise SCL and SDA will be driven down and lots of battery
     * charge is used to heat up the pull up resistors */
    if (IS_ACTIVE(CPU_FAM_STM32F1)) {
        _deinit_pins(dev);
    }

    /* Finally, disable the clock to the I2C peripheral */
    periph_clk_dis(i2c_config[dev].bus, i2c_config[dev].rcc_mask);
}

static void _enable_periph(i2c_t dev)
{
    /* First, clock the I2C peripheral so that registers can be written to */
    periph_clk_en(i2c_config[dev].bus, i2c_config[dev].rcc_mask);

    /* On STM32F1: We had to detach pins to work around a h/w limitations, so
     * re-attach them now */
    if (IS_ACTIVE(CPU_FAM_STM32F1)) {
        _init_pins(dev);
    }

    /* Finally: Enable peripheral again */
    i2c_config[dev].dev->CR1 |= I2C_CR1_PE;
}

static void _i2c_init(I2C_TypeDef *i2c, uint32_t clk, uint32_t ccr)
{
    /* disable device and set ACK bit */
    i2c->CR1 = I2C_CR1_ACK;
    /* configure I2C clock */
    i2c->CR2 = (clk / 1000000) | I2C_CR2_ITERREN;
    i2c->CCR = ccr;
    i2c->TRISE = (clk / 1000000) + 1;
    /* configure device */
    /* configure device */
    i2c->OAR1 |= (1 << 14); /* datasheet: bit 14 should be kept 1 */
    i2c->OAR1 &= ~I2C_OAR1_ADDMODE; /* make sure we are in 7-bit address mode */
    /* Clear flags */
    i2c->SR1 &= ~ERROR_FLAG;
    /* enable device */
    i2c->CR1 |= I2C_CR1_PE;
}

static void _init(i2c_t dev)
{
    I2C_TypeDef *i2c = i2c_config[dev].dev;

    uint32_t ccr = 0;
    /* read speed configuration */
    switch (i2c_config[dev].speed) {
        case I2C_SPEED_LOW:
            /* 10Kbit/s */
            ccr = i2c_config[dev].clk / 20000;
            break;

        case I2C_SPEED_NORMAL:
            /* 100Kbit/s */
            ccr = i2c_config[dev].clk / 200000;
            break;

        case I2C_SPEED_FAST:
            ccr = i2c_config[dev].clk / 800000;
            break;
    }

    /* make peripheral soft reset */
    i2c->CR1 |= I2C_CR1_SWRST;

    _init_pins(dev);

    i2c->CR1 &= ~I2C_CR1_SWRST;

    /* configure device */
    _i2c_init(i2c, i2c_config[dev].clk, ccr);

    /* go to low power */
    _disable_periph(dev);
}

void i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_lock(&locks[dev]);

#ifdef STM32_PM_STOP
    /* block STOP mode */
    pm_block(STM32_PM_STOP);
#endif

    _enable_periph(dev);
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    _disable_periph(dev);

#ifdef STM32_PM_STOP
    /* unblock STOP mode */
    pm_unblock(STM32_PM_STOP);
#endif

    mutex_unlock(&locks[dev]);
}

int i2c_read_bytes(i2c_t dev, uint16_t address, void *data, size_t length,
                   uint8_t flags)
{
    assert(dev < I2C_NUMOF);

    I2C_TypeDef *i2c = i2c_config[dev].dev;
    DEBUG_PUTS("[i2c] i2c_read_bytes(): Starting");

    /* Do not support repeated start reading
     * The repeated start read requires the bus to be busy (I2C_SR2_BUSY == 1)
     * the previous R/W state to be a read (I2C_SR2_TRA == 0)
     * and for the command not to be split frame (I2C_NOSTART == 0)
    */
    if (((i2c->SR2 & (I2C_SR2_BUSY | I2C_SR2_TRA)) == I2C_SR2_BUSY) &&
        !(flags & I2C_NOSTART)) {
        return -EOPNOTSUPP;
    }
    int ret = _i2c_start(i2c, (address << 1) | I2C_FLAG_READ, flags, length);
    if (ret < 0) {
        if (ret == -ETIMEDOUT) {
            _init(dev);
        }
        return ret;
    }

    for (size_t i = 0; i < length; i++) {
        if (i + 1 == length && !(flags & I2C_NOSTOP)) {
            /* If data is already in the buffer we must clear before sending
            a stop.  If I2C_NOSTOP was called up to two extra bytes may be
            clocked out on the line however they get ignored in the firmware.*/
            if ((i2c->SR1 & I2C_SR1_RXNE) && (length == 1)) {
                ((uint8_t*)data)[i] = i2c->DR;
                return _stop(i2c);
            }
            /* Stop must also be sent before final read */
            ret = _stop(i2c);
            if (ret < 0) {
                return ret;
            }
        }
        /* Wait for reception to complete */
        ret = _is_sr1_mask_set(i2c, I2C_SR1_RXNE, flags);
        if (ret < 0) {
            DEBUG_PUTS("[i2c] i2c_read_bytes(): Waiting for I2C_SR1_RXNE failed");
            return ret;
        }
        ((uint8_t*)data)[i] = i2c->DR;
    }
    DEBUG_PUTS("[i2c] i2c_read_bytes(): Finished reading bytes");
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

    I2C_TypeDef *i2c = i2c_config[dev].dev;
    assert(i2c != NULL);
    DEBUG_PUTS("[i2c] i2c_write_bytes(): Starting");
    /* Length is 0 in start since we don't need to preset the stop bit */
    ret = _i2c_start(i2c, (address << 1) | I2C_FLAG_WRITE, flags, 0);
    if (ret < 0) {
        if (ret == -ETIMEDOUT) {
            _init(dev);
        }
        return ret;
    }

    /* Send out data bytes */
    for (size_t i = 0; i < length; i++) {
        DEBUG_PUTS("[i2c] i2c_write_bytes(): Waiting for TX reg to be free");
        ret = _is_sr1_mask_set(i2c, I2C_SR1_TXE, flags);
        if (ret < 0) {
            DEBUG_PUTS("[i2c] i2c_write_bytes(): Waiting for I2C_SR1_TXE failed");
            return ret;
        }
        DEBUG_PUTS("[i2c] i2c_write_bytes(): TX is free so send byte");
        i2c->DR = ((uint8_t*)data)[i];
    }
    /* Wait for tx reg to be empty so other calls will no interfere */
    ret = _is_sr1_mask_set(i2c, I2C_SR1_TXE, flags);
    if (ret < 0) {
        DEBUG_PUTS("[i2c] i2c_write_bytes(): Waiting for I2C_SR1_TXE failed");
        return ret;
    }
    if (flags & I2C_NOSTOP) {
        return 0;
    }
    else {
        /* End transmission */
        DEBUG_PUTS("[i2c] i2c_write_bytes(): Ending transmission");
        ret = _stop(i2c);
        if (ret < 0) {
            return ret;
        }
        DEBUG_PUTS("[i2c] i2c_write_bytes(): STOP condition was send out");
    }

    return _wait_for_bus(i2c);
}

static int _i2c_start(I2C_TypeDef *i2c, uint8_t address_byte, uint8_t flags,
                  size_t length)
{
    assert(i2c != NULL);

    if ((flags & I2C_ADDR10) ||
        (!(i2c->SR2 & I2C_SR2_BUSY) && (flags & I2C_NOSTART))) {
        return -EOPNOTSUPP;
    }

    /* Clear flags */
    i2c->SR1 &= ~ERROR_FLAG;

    if (!(flags & I2C_NOSTART)) {
        DEBUG_PUTS("[i2c] _i2c_start(): Generate start condition");
        /* Generate start condition */
        i2c->CR1 |= I2C_CR1_START | I2C_CR1_ACK;

        /* Wait for SB flag to be set */
        int ret = _is_sr1_mask_set(i2c, I2C_SR1_SB, flags & ~I2C_NOSTOP);
        if (ret < 0) {
            DEBUG_PUTS("[i2c] _i2c_start(): Waiting for I2C_SR1_SB failed");
            return ret;
        }
        DEBUG_PUTS("[i2c] _i2c_start(): Start condition generated");

        DEBUG_PUTS("[i2c] _i2c_start(): Generating address");
        /* Send address and read/write flag */
        i2c->DR = (address_byte);
        if (!(flags & I2C_NOSTOP) && length == 1) {
            i2c->CR1 &= ~(I2C_CR1_ACK);
        }
        /* Wait for ADDR flag to be set */
        ret = _is_sr1_mask_set(i2c, I2C_SR1_ADDR, flags & ~I2C_NOSTOP);
        if (ret == -EIO) {
            /* Since NACK happened during start it means no device connected */
            DEBUG_PUTS("[i2c] _i2c_start(): Address NACKED");
            return -ENXIO;
        }
        /* Needed to clear address bit */
        i2c->SR2;
        if (!(flags & I2C_NOSTOP) && length == 1) {
            /* Stop must also be sent before final read */
            i2c->CR1 |= (I2C_CR1_STOP);
        }
        if (ret) {
            DEBUG_PUTS("[i2c] _i2c_start(): Waiting for I2C_SR1_ADDR failed");
        }
        else {
            DEBUG_PUTS("[i2c] _i2c_start(): Address generated");
        }
        return ret;
    }
    return 0;
}

static int _is_sr1_mask_set(I2C_TypeDef *i2c, uint32_t mask, uint8_t flags)
{
    DEBUG("[i2c] _is_sr1_mask_set: waiting to set %04X\n", (uint16_t)mask);
    uint16_t tick = TICK_TIMEOUT;
    while (tick--) {
        uint32_t sr1 = i2c->SR1;
        if (sr1 & I2C_SR1_AF) {
            DEBUG_PUTS("[i2c] _is_sr1_mask_set(): NACK received");
            i2c->SR1 &= ~ERROR_FLAG;
            if (!(flags & I2C_NOSTOP)) {
                _stop(i2c);
            }
            return -EIO;
        }
        if ((sr1 & I2C_SR1_ARLO) || (sr1 & I2C_SR1_BERR)) {
            DEBUG_PUTS("[i2c] _is_sr1_mask_set(): arb lost or bus ERROR_FLAG");
            i2c->SR1 &= ~ERROR_FLAG;
            _stop(i2c);
            return -EAGAIN;
        }
        if (sr1 & mask) {
            i2c->SR1 &= ~ERROR_FLAG;
            return 0;
        }
    }
    /*
    * If timeout occurs this means a problem that must be handled on a higher
    * level.  A SWRST is recommended by the datasheet.
    */
    i2c->SR1 &= ~ERROR_FLAG;
    _stop(i2c);
    DEBUG_PUTS("[i2c] _is_sr1_mask_set(): Timed out");
    return -ETIMEDOUT;
}

static int _stop(I2C_TypeDef *i2c)
{
    /* send STOP condition */
    DEBUG_PUTS("[i2c] _stop(): Generate stop condition");
    i2c->CR1 &= ~(I2C_CR1_ACK);
    i2c->CR1 |= I2C_CR1_STOP;
    uint16_t tick = TICK_TIMEOUT;
    while ((i2c->CR1 & I2C_CR1_STOP) && tick--) {}
    if (!tick) {
        DEBUG_PUTS("[i2c] _stop(): Stop condition timed out");
        return -ETIMEDOUT;
    }
    DEBUG_PUTS("[i2c] _stop(): Stop condition succeeded");
    if (_wait_for_bus(i2c) < 0) {
        DEBUG_PUTS("[i2c] _stop(): Bus free timed out");
        return -ETIMEDOUT;
    }
    DEBUG_PUTS("[i2c] _stop(): Bus is free");
    return 0;
}

static inline int _wait_for_bus(I2C_TypeDef *i2c)
{
    uint16_t tick = TICK_TIMEOUT;
    while ((i2c->SR2 & I2C_SR2_BUSY) && tick--) {}
    if (!tick) {
        DEBUG_PUTS("[i2c] _wait_for_bus(): Timed out");
        return -ETIMEDOUT;
    }
    return 0;
}

#if I2C_0_ISR || I2C_1_ISR
static inline void irq_handler(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    I2C_TypeDef *i2c = i2c_config[dev].dev;

    assert(i2c != NULL);

    unsigned state = i2c->SR1;
    DEBUG_PUTS("\n\n### I2C ERROR OCCURRED ###");
    DEBUG("status: %08x\n", state);
    if (state & I2C_SR1_OVR) {
        DEBUG_PUTS("OVR");
    }
    if (state & I2C_SR1_AF) {
        DEBUG_PUTS("AF");
    }
    if (state & I2C_SR1_ARLO) {
        DEBUG_PUTS("ARLO");
    }
    if (state & I2C_SR1_BERR) {
        DEBUG_PUTS("BERR");
    }
    if (state & I2C_SR1_PECERR) {
        DEBUG_PUTS("PECERR");
    }
    if (state & I2C_SR1_TIMEOUT) {
        DEBUG_PUTS("TIMEOUT");
    }
    if (state & I2C_SR1_SMBALERT) {
        DEBUG_PUTS("SMBALERT");
    }
    core_panic(PANIC_GENERAL_ERROR, "I2C FAULT");
}
#endif

#if I2C_0_ISR
void I2C_0_ISR(void)
{
    irq_handler(I2C_DEV(0));
}
#endif /* I2C_0_ISR */

#if I2C_1_ISR
void I2C_1_ISR(void)
{
    irq_handler(I2C_DEV(1));
}
#endif /* I2C_1_ISR */
