/*
 * Copyright (C) 2015 Jan Pohlmann <jan-pohlmann@gmx.de>
 *               2017 we-sens.com
 *               2018 Inria
 *               2018 HAW Hamburg
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
 * This driver supports the STM32 F0, F3, F7, L0 and L4 families.
 * @note This implementation only implements the 7-bit addressing polling mode
 * (for now interrupt mode is not available)
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Jan Pohlmann <jan-pohlmann@gmx.de>
 * @author      Aurélien Fillau <aurelien.fillau@we-sens.com>
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
#include "byteorder.h"

#include "cpu_conf_stm32_common.h"

#include "periph/i2c.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define TICK_TIMEOUT    (0xFFFF)
#define MAX_BYTES_PER_FRAME (256)

#define I2C_IRQ_PRIO    (1)
#define I2C_FLAG_READ   (I2C_READ << I2C_CR2_RD_WRN_Pos)
#define I2C_FLAG_WRITE  (0)

#define CLEAR_FLAG      (I2C_ICR_NACKCF | I2C_ICR_ARLOCF | I2C_ICR_BERRCF | I2C_ICR_ADDRCF)

/* static function definitions */
static inline void _i2c_init(I2C_TypeDef *i2c, uint32_t timing);
static int _write(I2C_TypeDef *i2c, uint16_t addr, const void *data,
                  size_t length, uint8_t flags, uint32_t cr2_flags);
static int _start(I2C_TypeDef *i2c, uint32_t cr2, uint8_t flags);
static int _stop(I2C_TypeDef *i2c);
static int _wait_isr_set(I2C_TypeDef *i2c, uint32_t mask, uint8_t flags);
static inline int _wait_for_bus(I2C_TypeDef *i2c);

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

    periph_clk_en(i2c_config[dev].bus, i2c_config[dev].rcc_mask);

    NVIC_SetPriority(i2c_config[dev].irqn, I2C_IRQ_PRIO);
    NVIC_EnableIRQ(i2c_config[dev].irqn);

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3)
    /* Set I2CSW bits to enable I2C clock source */
    RCC->CFGR3 |= i2c_config[dev].rcc_sw_mask;
#endif

    DEBUG("[i2c] init: configuring pins\n");
    /* configure pins */
    gpio_init(i2c_config[dev].scl_pin, GPIO_OD_PU);
    gpio_init_af(i2c_config[dev].scl_pin, i2c_config[dev].scl_af);
    gpio_init(i2c_config[dev].sda_pin, GPIO_OD_PU);
    gpio_init_af(i2c_config[dev].sda_pin, i2c_config[dev].sda_af);

    DEBUG("[i2c] init: configuring device\n");
    /* set the timing register value from predefined values */
    i2c_timing_param_t tp = timing_params[i2c_config[dev].speed];
    uint32_t timing = (( (uint32_t)tp.presc << I2C_TIMINGR_PRESC_Pos) |
                       ( (uint32_t)tp.scldel << I2C_TIMINGR_SCLDEL_Pos) |
                       ( (uint32_t)tp.sdadel << I2C_TIMINGR_SDADEL_Pos) |
                       ( (uint16_t)tp.sclh << I2C_TIMINGR_SCLH_Pos) |
                       tp.scll);
    _i2c_init(i2c, timing);
}

static void _i2c_init(I2C_TypeDef *i2c, uint32_t timing)
{
    assert(i2c != NULL);

    /* disable device */
    i2c->CR1 &= ~(I2C_CR1_PE);

    /* configure analog noise filter */
    i2c->CR1 |= I2C_CR1_ANFOFF;

    /* configure digital noise filter */
    i2c->CR1 |= I2C_CR1_DNF;

    /* set timing registers */
    i2c->TIMINGR = timing;

    /* configure clock stretching */
    i2c->CR1 &= ~(I2C_CR1_NOSTRETCH);

    /* Clear interrupt */
    i2c->ICR |= CLEAR_FLAG;

    /* enable device */
    i2c->CR1 |= I2C_CR1_PE;
}

int i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_lock(&locks[dev]);

    periph_clk_en(i2c_config[dev].bus, i2c_config[dev].rcc_mask);

    return 0;
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    periph_clk_dis(i2c_config[dev].bus, i2c_config[dev].rcc_mask);

    mutex_unlock(&locks[dev]);
}

int i2c_write_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                   const void *data, size_t len, uint8_t flags)
{
    assert(dev < I2C_NUMOF);
    if (flags & (I2C_NOSTOP | I2C_NOSTART)) {
        return -EOPNOTSUPP;
    }

    I2C_TypeDef *i2c = i2c_config[dev].dev;
    assert(i2c != NULL);
    DEBUG("[i2c] write_regs: Starting\n");
    /* As a higher level function we know the bus should be free */
    if (i2c->ISR & I2C_ISR_BUSY) {
        return -EAGAIN;
    }
    /* Handle endianness of register if 16 bit */
    if (flags & I2C_REG16) {
        reg = htons(reg); /* Make sure register is in big-endian on I2C bus */
    }
    /* First set ADDR and register with no stop */
    /* No RELOAD should be set so repeated start is valid */
    int ret = _write(i2c, addr, &reg, (flags & I2C_REG16) ? 2 : 1,
                     flags | I2C_NOSTOP, I2C_CR2_RELOAD);
    if (ret < 0) {
        return ret;
    }
    /* Then get the data from device */
    return _write(i2c, addr, data, len, I2C_NOSTART, 0);
}

int i2c_read_bytes(i2c_t dev, uint16_t address, void *data,
                   size_t length, uint8_t flags)
{
    assert(dev < I2C_NUMOF && length < MAX_BYTES_PER_FRAME);

    I2C_TypeDef *i2c = i2c_config[dev].dev;
    assert(i2c != NULL);

    /* If reload was set, cannot send a repeated start */
    if ((i2c->ISR & I2C_ISR_TCR) && !(flags & I2C_NOSTART)) {
        return -EOPNOTSUPP;
    }
    DEBUG("[i2c] read_bytes: Starting\n");
    /* RELOAD is needed because we don't know the full frame */
    int ret = _start(i2c, (address << 1) | (length << I2C_CR2_NBYTES_Pos) |
                     I2C_CR2_RELOAD | I2C_FLAG_READ, flags);
    if (ret < 0) {
        return ret;
    }

    for (size_t i = 0; i < length; i++) {
        /* wait for transfer to finish */
        DEBUG("[i2c] read_bytes: Waiting for DR to be full\n");
        ret = _wait_isr_set(i2c, I2C_ISR_RXNE, flags);
        if (ret < 0) {
            return ret;
        }
        /* read data from data register */
        ((uint8_t*)data)[i]= i2c->RXDR;
        DEBUG("[i2c] read_bytes: DR full, read 0x%02X\n", ((uint8_t*)data)[i]);
    }
    if (flags & I2C_NOSTOP) {
        /* With NOSTOP, the TCR indicates that the next command is ready */
        /* TCR is needed because RELOAD is set preventing a NACK on last byte */
        return _wait_isr_set(i2c, I2C_ISR_TCR, flags);
    }
    /* Wait until stop before other commands are sent */
    ret = _wait_isr_set(i2c, I2C_ISR_STOPF, flags);
    if (ret < 0) {
        return ret;
    }

    return _wait_for_bus(i2c);
}

/**
 * Cannot support continuous writes or frame splitting at this level.  If an
 * I2C_NOSTOP has been sent it must be followed by a repeated start or stop.
 */
int i2c_write_bytes(i2c_t dev, uint16_t address, const void *data,
                    size_t length, uint8_t flags)
{
    assert(dev < I2C_NUMOF);
    I2C_TypeDef *i2c = i2c_config[dev].dev;
    DEBUG("[i2c] write_bytes: Starting\n");
    return _write(i2c, address, data, length, flags, 0);
}

static int _write(I2C_TypeDef *i2c, uint16_t addr, const void *data,
                    size_t length, uint8_t flags, uint32_t cr2_flags)
{
    assert(i2c != NULL && length < MAX_BYTES_PER_FRAME);

    /* If reload was NOT set, must either stop or start */
    if ((i2c->ISR & I2C_ISR_TC) && (flags & I2C_NOSTART)) {
        return -EOPNOTSUPP;
    }
    int ret = _start(i2c, (addr << 1) | (length << I2C_CR2_NBYTES_Pos) |
                     cr2_flags, flags);
    if (ret < 0) {
        return ret;
    }

    for (size_t i = 0; i < length; i++) {
        DEBUG("[i2c] write_bytes: Waiting for TX reg to be free\n");
        ret = _wait_isr_set(i2c, I2C_ISR_TXIS, flags);
        if (ret < 0) {
            return ret;
        }
        DEBUG("[i2c] write_bytes: TX is free so send byte\n");
        /* write data to data register */
        i2c->TXDR = ((uint8_t*)data)[i];
    }

    if (flags & I2C_NOSTOP) {
        if (cr2_flags & I2C_CR2_RELOAD) {
            DEBUG("[i2c] write_bytes: Waiting for TCR\n");
            /* With NOSTOP, the TCR indicates that the next command is ready */
            /* TCR is needed because RELOAD allows loading more bytes */
            return _wait_isr_set(i2c, I2C_ISR_TCR, flags);
        }
        else {
            DEBUG("[i2c] write_bytes: Waiting for TC\n");
            /* With NOSTOP, the TC indicates that the next command is ready */
            /* TC is needed because no reload is set for repeated start */
            return _wait_isr_set(i2c, I2C_ISR_TC, flags);
        }
    }
    DEBUG("[i2c] write_bytes: Waiting for stop\n");
    /* Wait until stop before other commands are sent */
    ret = _wait_isr_set(i2c, I2C_ISR_STOPF, flags);
    if (ret < 0) {
        return ret;
    }
    return _wait_for_bus(i2c);
}


static int _start(I2C_TypeDef *i2c, uint32_t cr2, uint8_t flags)
{
    assert(i2c != NULL);
    assert((i2c->ISR & I2C_ISR_BUSY) || !(flags & I2C_NOSTART));

    i2c->ICR |= CLEAR_FLAG;
    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }

    if (!(flags & I2C_NOSTART)) {
        DEBUG("[i2c] start: Generate start condition\n");
        /* Generate start condition */
        cr2 |= I2C_CR2_START;
    }
    if (!(flags & I2C_NOSTOP)) {
        cr2 |= I2C_CR2_AUTOEND;
        cr2 &= ~(I2C_CR2_RELOAD);
    }
    DEBUG("[i2c] start: Setting CR2=0x%08x\n", (unsigned int)cr2);
    i2c->CR2 = cr2;
    if (!(flags & I2C_NOSTART)) {
        uint16_t tick = TICK_TIMEOUT;
        while ((i2c->CR2 & I2C_CR2_START) && tick--) {
            if (!tick) {
                /* Try to stop for state error recovery */
                _stop(i2c);
                return -ETIMEDOUT;
            }
        }
        DEBUG("[i2c] start: Start condition and address generated\n");
        /* Check if the device is there */
        if ((i2c->ISR & I2C_ISR_NACKF)) {
            i2c->ICR |= I2C_ICR_NACKCF;
            _stop(i2c);
            return -ENXIO;
        }
    }
    return 0;
}

static int _stop(I2C_TypeDef *i2c)
{
    /* Send stop condition */
    DEBUG("[i2c] stop: Generate stop condition\n");
    i2c->CR2 |= I2C_CR2_STOP;

    /* Wait for the stop to complete */
    uint16_t tick = TICK_TIMEOUT;
    while ((i2c->CR2 & I2C_CR2_STOP) && tick--) {}
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

static int _wait_isr_set(I2C_TypeDef *i2c, uint32_t mask, uint8_t flags)
{
    uint16_t tick = TICK_TIMEOUT;
    while (tick--) {
        uint32_t isr = i2c->ISR;

        if (isr & I2C_ISR_NACKF) {
            DEBUG("[i2c] wait_isr_set: NACK received\n");

            /* Some devices have a valid data nack, if indicated don't stop */
            if (!(flags & I2C_NOSTOP)) {
                _stop(i2c);
            }
            i2c->ICR |= CLEAR_FLAG;
            return -EIO;
        }
        if ((isr & I2C_ISR_ARLO) || (isr & I2C_ISR_BERR)) {
            DEBUG("[i2c] wait_isr_set: Arbitration lost or bus error\n");
            _stop(i2c);
            i2c->ICR |= CLEAR_FLAG;
            return -EAGAIN;
        }
        if (isr & mask) {
            DEBUG("[i2c] wait_isr_set: ISR 0x%08x set\n", (unsigned int)mask);
            return 0;
        }
    }
    /*
    * If timeout occurs this means a problem that must be handled on a higher
    * level.  A SWRST is recommended by the datasheet.
    */
    return -ETIMEDOUT;
}

static inline int _wait_for_bus(I2C_TypeDef *i2c)
{
    uint16_t tick = TICK_TIMEOUT;
    while (tick-- && (i2c->ISR & I2C_ISR_BUSY)) {}
    if (!tick) {
        return -ETIMEDOUT;
    }
    return 0;
}

static inline void irq_handler(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    I2C_TypeDef *i2c = i2c_config[dev].dev;

    unsigned state = i2c->ISR;
    DEBUG("\n\n### I2C ERROR OCCURRED ###\n");
    DEBUG("status: %08x\n", state);
    if (state & I2C_ISR_OVR) {
        DEBUG("OVR\n");
    }
    if (state & I2C_ISR_NACKF) {
        DEBUG("AF\n");
    }
    if (state & I2C_ISR_ARLO) {
        DEBUG("ARLO\n");
    }
    if (state & I2C_ISR_BERR) {
        DEBUG("BERR\n");
    }
    if (state & I2C_ISR_PECERR) {
        DEBUG("PECERR\n");
    }
    if (state & I2C_ISR_TIMEOUT) {
        DEBUG("TIMEOUT\n");
    }
    if (state & I2C_ISR_ALERT) {
        DEBUG("SMBALERT\n");
    }
    core_panic(PANIC_GENERAL_ERROR, "I2C FAULT");
}

#ifdef I2C_0_ISR
void I2C_0_ISR(void)
{
    irq_handler(I2C_DEV(0));
}
#endif /* I2C_0_ISR */

#ifdef I2C_1_ISR
void I2C_1_ISR(void)
{
    irq_handler(I2C_DEV(1));
}
#endif /* I2C_1_ISR */
