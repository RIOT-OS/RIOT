/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 FU Berlin
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * This driver supports the STM32 F4 families.
 *
 * @note This implementation only implements the 7-bit addressing mode.
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "irq.h"
#include "mutex.h"
#include "pm_layered.h"

#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define I2C_IRQ_PRIO            (1)
#define I2C_FLAG_READ           (I2C_READ)
#define I2C_FLAG_WRITE          (0)

#define TICK_TIMEOUT            (0xFFFF)

/* static function definitions */
static void _i2c_init(I2C_TypeDef *i2c, uint32_t clk, uint32_t ccr);
static void _start(I2C_TypeDef *dev, uint8_t address, uint8_t rw_flag, uint8_t flags);
static inline void _clear_addr(I2C_TypeDef *dev);
static inline void _write(I2C_TypeDef *dev, const uint8_t *data, int length);
static inline void _stop(I2C_TypeDef *dev);
static inline int _wait_ready(I2C_TypeDef *dev);

/**
 * @brief Array holding one pre-initialized mutex for each I2C device
 */
static mutex_t locks[I2C_NUMOF];

void i2c_init(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    DEBUG("[i2c] init: initializing device\n");
    mutex_init(&locks[dev]);

    I2C_TypeDef *i2c = i2c_config[dev].dev;

    assert(i2c != NULL);

    uint32_t ccr;
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

        default:
            return;
    }

    periph_clk_en(i2c_config[dev].bus, i2c_config[dev].rcc_mask);
    NVIC_SetPriority(i2c_config[dev].irqn, I2C_IRQ_PRIO);
    NVIC_EnableIRQ(i2c_config[dev].irqn);

    /* configure pins */
    DEBUG("[i2c] init: configuring pins\n");
    gpio_init(i2c_config[dev].scl_pin, GPIO_OD_PU);
    gpio_init(i2c_config[dev].sda_pin, GPIO_OD_PU);
#ifdef CPU_FAM_STM32F1
    gpio_init_af(i2c_config[dev].scl_pin, GPIO_AF_OUT_OD);
    gpio_init_af(i2c_config[dev].sda_pin, GPIO_AF_OUT_OD);
#else
    gpio_init_af(i2c_config[dev].scl_pin, i2c_config[dev].scl_af);
    gpio_init_af(i2c_config[dev].sda_pin, i2c_config[dev].sda_af);
#endif

    /* configure device */
    DEBUG("[i2c] init: configuring device\n");
    _i2c_init(i2c, i2c_config[dev].clk, ccr);

#if defined(CPU_FAM_STM32F4)
    /* make sure the analog filters don't hang -> see errata sheet 2.14.7 */
    if (i2c->SR2 & I2C_SR2_BUSY) {
        DEBUG("[i2c] init: line busy after reset, toggle pins now\n");
        /* disable peripheral */
        i2c->CR1 &= ~I2C_CR1_PE;
        /* toggle both pins to reset analog filter */
        gpio_init(i2c_config[dev].scl_pin, GPIO_OD);
        gpio_init(i2c_config[dev].sda_pin, GPIO_OD);
        gpio_set(i2c_config[dev].sda_pin);
        gpio_set(i2c_config[dev].scl_pin);
        gpio_clear(i2c_config[dev].sda_pin);
        gpio_clear(i2c_config[dev].scl_pin);
        gpio_set(i2c_config[dev].sda_pin);
        gpio_set(i2c_config[dev].scl_pin);
        /* reset pins for alternate function */
        gpio_init(i2c_config[dev].scl_pin, GPIO_OD_PU);
        gpio_init(i2c_config[dev].sda_pin, GPIO_OD_PU);
#ifdef CPU_FAM_STM32F1
        gpio_init_af(i2c_config[dev].scl_pin, GPIO_AF_OUT_OD);
        gpio_init_af(i2c_config[dev].sda_pin, GPIO_AF_OUT_OD);
#else
        gpio_init_af(i2c_config[dev].scl_pin, i2c_config[dev].scl_af);
        gpio_init_af(i2c_config[dev].sda_pin, i2c_config[dev].sda_af);
#endif
        /* make peripheral soft reset */
        i2c->CR1 |= I2C_CR1_SWRST;
        i2c->CR1 &= ~I2C_CR1_SWRST;
        /* enable device */
        _i2c_init(i2c, i2c_config[dev].clk, ccr);
    }
#endif
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
    /* enable device */
    i2c->CR1 |= I2C_CR1_PE;
}

int i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_lock(&locks[dev]);

#ifdef STM32_PM_STOP
    /* block STOP mode */
    pm_block(STM32_PM_STOP);
#endif

    periph_clk_en(i2c_config[dev].bus, i2c_config[dev].rcc_mask);

    return 0;
}

int i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    uint16_t tick = TICK_TIMEOUT;
    while ((i2c_config[dev].dev->SR2 & I2C_SR2_BUSY) && tick--) {}

    periph_clk_dis(i2c_config[dev].bus, i2c_config[dev].rcc_mask);

#ifdef STM32_PM_STOP
    /* unblock STOP mode */
    pm_unblock(STM32_PM_STOP);
#endif

    mutex_unlock(&locks[dev]);
    return 0;
}

int i2c_read_bytes(i2c_t dev, uint16_t address, void *data, size_t length,
                   uint8_t flags)
{
    assert(dev < I2C_NUMOF);

    size_t n = length;
    char *in = (char *)data;

    I2C_TypeDef *i2c = i2c_config[dev].dev;

    assert(i2c != NULL);

    if (!(flags & I2C_NOSTART)) {
        DEBUG("[i2c] read_bytes: Send Slave address and wait for ADDR == 1\n");
        _start(i2c, address, I2C_FLAG_READ, flags);
    }

    if (length == 1) {
        DEBUG("[i2c] read_bytes: Set ACK = 0\n");
        i2c->CR1 &= ~(I2C_CR1_ACK);
    }
    else {
        i2c->CR1 |= I2C_CR1_ACK;
    }

    _clear_addr(i2c);

    while (n--) {
        /* wait for reception to complete */
        while (!(i2c->SR1 & I2C_SR1_RXNE)) {}

        if (n == 1) {
            /* disable ACK */
            i2c->CR1 &= ~(I2C_CR1_ACK);
        }

        /* read byte */
        *(in++) = i2c->DR;
    }

    if (!(flags & I2C_NOSTOP)) {
        /* set STOP */
        i2c->CR1 |= (I2C_CR1_STOP);

        while (i2c->CR1 & I2C_CR1_STOP) {}
    }

    return length;
}

int i2c_read_regs(i2c_t dev, uint16_t address, uint16_t reg, void *data,
                  size_t length, uint8_t flags)
{
    assert(dev < I2C_NUMOF);

    I2C_TypeDef *i2c = i2c_config[dev].dev;
    assert(i2c != NULL);


    int res = _wait_ready(i2c);
    if (res != 0) {
        return res;
    }

    /* send start condition and slave address */
    DEBUG("[i2c] read_regs: Send slave address and clear ADDR flag\n");
    _start(i2c, address, I2C_FLAG_WRITE, flags);
    DEBUG("[i2c] read_regs: Write reg into DR\n");
    _clear_addr(i2c);
    while (!(i2c->SR1 & I2C_SR1_TXE)) {}
    i2c->DR = reg;
    while (!(i2c->SR1 & I2C_SR1_TXE)) {}
    DEBUG("[i2c] read_regs: Now start a read transaction\n");
    return i2c_read_bytes(dev, address, data, length, flags);
}

int i2c_write_bytes(i2c_t dev, uint16_t address, const void *data,
                    size_t length, uint8_t flags)
{
    assert(dev < I2C_NUMOF);

    I2C_TypeDef *i2c = i2c_config[dev].dev;
    assert(i2c != NULL);

    int res = _wait_ready(i2c);
    if (res != 0) {
        return res;
    }

    if (!(flags & I2C_NOSTART)) {
        /* start transmission and send slave address */
        DEBUG("[i2c] write_bytes: sending start sequence\n");
        _start(i2c, address, I2C_FLAG_WRITE, flags);
    }

    _clear_addr(i2c);
    /* send out data bytes */
    _write(i2c, data, length);
    if (!(flags & I2C_NOSTOP)) {
        /* end transmission */
        DEBUG("[i2c] write_bytes: Ending transmission\n");
        _stop(i2c);
        DEBUG("[i2c] write_bytes: STOP condition was send out\n");
    }

    return length;
}

int i2c_write_regs(i2c_t dev, uint16_t address, uint16_t reg, const void *data,
                   size_t length, uint8_t flags)
{
    assert(dev < I2C_NUMOF);

    I2C_TypeDef *i2c = i2c_config[dev].dev;
    assert(i2c != NULL);

    int res = _wait_ready(i2c);
    if (res != 0) {
        return res;
    }

    /* start transmission and send slave address */
    _start(i2c, address, I2C_FLAG_WRITE, flags);
    _clear_addr(i2c);
    /* send register address and wait for complete transfer to be finished*/
    _write(i2c, (uint8_t *)&reg, 1);
    /* write data to register */
    _write(i2c, data, length);
    /* finish transfer */
    _stop(i2c);
    /* return number of bytes send */
    return length;
}

static void _start(I2C_TypeDef *i2c, uint8_t address, uint8_t rw_flag, uint8_t flags)
{
    (void)flags;
start:

    /* generate start condition */
    i2c->CR1 |= I2C_CR1_START;

    /* Wait for SB flag to be set */
    while (!(i2c->SR1 & I2C_SR1_SB)) {}

    /* send address and read/write flag */
    i2c->DR = (address << 1) | rw_flag;

    /* Wait for ADDR flag to be set */
    while (!(i2c->SR1 & I2C_SR1_ADDR)) {
        if (i2c->SR1 & I2C_SR1_AF) {
            /* if the device answers NACK on sending the address, retry */
            i2c->SR1 &= ~(I2C_SR1_AF);
            goto start;
        }
    }
}

static inline void _clear_addr(I2C_TypeDef *i2c)
{
    i2c->SR1;
    i2c->SR2;
}

static inline void _write(I2C_TypeDef *i2c, const uint8_t *data, int length)
{
    DEBUG("[i2c] write: Looping through bytes\n");

    for (int i = 0; i < length; i++) {
        /* write data to data register */
        i2c->DR = data[i];
        DEBUG("[i2c] write: Written %i byte to data reg, now waiting for DR "
              "to be empty again\n", i);

        /* wait for transfer to finish */
        while (!(i2c->SR1 & I2C_SR1_TXE)) {}

        DEBUG("[i2c] write: DR is now empty again\n");
    }
}

static inline void _stop(I2C_TypeDef *i2c)
{
    /* make sure last byte was send */
    DEBUG("[i2c] write: Wait if last byte hasn't been sent\n");

    while (!(i2c->SR1 & I2C_SR1_BTF)) {}

    /* send STOP condition */
    i2c->CR1 |= I2C_CR1_STOP;
}

static inline int _wait_ready(I2C_TypeDef *i2c)
{
    /* wait for device to be ready */
    DEBUG("[i2c] wait_ready: Wait for device to be ready\n");

    uint16_t tick = TICK_TIMEOUT;
    while ((i2c->SR2 & I2C_SR2_BUSY) && tick--) {
        if (!tick) {
            DEBUG("[i2c] wait_ready: timeout\n");
            return -3;
        }
    }

    return 0;
}

static inline void irq_handler(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    I2C_TypeDef *i2c = i2c_config[dev].dev;

    assert(i2c != NULL);

    unsigned state = i2c->SR1;
    DEBUG("\n\n### I2C ERROR OCCURED ###\n");
    DEBUG("status: %08x\n", state);
    if (state & I2C_SR1_OVR) {
        DEBUG("OVR\n");
    }
    if (state & I2C_SR1_AF) {
        DEBUG("AF\n");
    }
    if (state & I2C_SR1_ARLO) {
        DEBUG("ARLO\n");
    }
    if (state & I2C_SR1_BERR) {
        DEBUG("BERR\n");
    }
    if (state & I2C_SR1_PECERR) {
        DEBUG("PECERR\n");
    }
    if (state & I2C_SR1_TIMEOUT) {
        DEBUG("TIMEOUT\n");
    }
    if (state & I2C_SR1_SMBALERT) {
        DEBUG("SMBALERT\n");
    }
    core_panic(PANIC_GENERAL_ERROR, "I2C FAULT");
}

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
