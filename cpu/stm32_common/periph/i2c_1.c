/*
 * Copyright (C) 2015 Jan Pohlmann <jan-pohlmann@gmx.de>
 *               2017 we-sens.com
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
 * This driver supports the STM32 F0, F3, F7, L0 and L4 families.
 * @note This implementation only implements the 7-bit addressing polling mode
 * (for now interrupt mode is not available)
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @auhtor      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Jan Pohlmann <jan-pohlmann@gmx.de>
 * @author      Aurélien Fillau <aurelien.fillau@we-sens.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdint.h>
#include <errno.h>

#include "cpu.h"
#include "mutex.h"

#include "cpu_conf_stm32_common.h"

#include "periph/i2c.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define TICK_TIMEOUT    (0xFFFF)

#define I2C_IRQ_PRIO            (1)
#define I2C_FLAG_READ           (I2C_READ)
#define I2C_FLAG_WRITE          (0)

#define CLEAR_FLAG              (I2C_ICR_NACKCF | I2C_ICR_ARLOCF | I2C_ICR_BERRCF)
#define ERROR_FLAG              (I2C_ISR_NACKF | I2C_ISR_ARLO | I2C_ISR_BERR)

/* static function definitions */
static inline void _i2c_init(I2C_TypeDef *i2c, uint32_t timing);
static inline int _start(I2C_TypeDef *dev, uint16_t address, size_t length,
                         uint8_t rw_flag, uint8_t flags);
static inline int _read(I2C_TypeDef *dev, uint8_t *data, size_t length);
static inline int _write(I2C_TypeDef *i2c, const uint8_t *data, size_t length);
static inline int _stop(I2C_TypeDef *i2c);
static inline int _check_bus(I2C_TypeDef *i2c);

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

int i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    uint16_t tick = TICK_TIMEOUT;

    while ((i2c_config[dev].dev->ISR & I2C_ISR_BUSY) && tick--) {}

    periph_clk_dis(i2c_config[dev].bus, i2c_config[dev].rcc_mask);

    mutex_unlock(&locks[dev]);
    return 0;
}

int i2c_read_bytes(i2c_t dev, uint16_t address, void *data,
                   size_t length, uint8_t flags)
{
    assert(dev < I2C_NUMOF);

    I2C_TypeDef *i2c = i2c_config[dev].dev;

    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }

    int ret = 0;

    if (!(flags & I2C_NOSTART)) {
        DEBUG("[i2c] read_bytes: send start condition\n");
        /* start reception and send slave address */
        ret = _start(i2c, address, length, I2C_FLAG_READ, flags);
        if (ret < 0) {
            _stop(i2c);
            return ret;
        }
    }

    DEBUG("[i2c] read_bytes: read the data\n");
    /* read the data bytes */
    ret = _read(i2c, data, length);
    if (ret < 0) {
        _stop(i2c);
        DEBUG("[i2c] read_bytes: error while reading\n");
        return ret;
    }

    if (!(flags & I2C_NOSTOP)) {
        DEBUG("[i2c] read_bytes: end transmission\n");
        ret = _stop(i2c);
        if (ret < 0) {
            return ret;
        }
    }

    return ret;
}

int i2c_read_regs(i2c_t dev, uint16_t address, uint16_t reg, void *data,
                  size_t length, uint8_t flags)
{
    assert(dev < I2C_NUMOF);

    DEBUG("[i2c] read_regs: addr: %04X, reg: %04X\n", address, reg);

    if ((flags & I2C_REG16) || (flags & I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }

    uint16_t tick = TICK_TIMEOUT;

    I2C_TypeDef *i2c = i2c_config[dev].dev;

    /* Check to see if the bus is busy */
    while ((i2c->ISR & I2C_ISR_BUSY) && tick--) {}
    if (!tick) {
        return -ETIMEDOUT;
    }

    if (!(flags & I2C_NOSTART)) {
        DEBUG("[i2c] read_regs: send start sequence\n");
        /* send start sequence and slave address */
        int ret = _start(i2c, address, 1, 0, flags);
        if (ret < 0) {
            _stop(i2c);
            return ret;
        }
    }

    tick = TICK_TIMEOUT;
    /* wait for ack */
    while (!(i2c->ISR & I2C_ISR_TXIS) && tick--) {
        if ((i2c->ISR & ERROR_FLAG) || !tick) {
            /* end transmission */
            _stop(i2c);
            return -ENXIO;
        }
    }

    DEBUG("[i2c] read_regs: Write register to read\n");
    i2c->TXDR = reg;

    /* send repeated start sequence, read registers and end transmission */
    DEBUG("[i2c] read_regs: ACK received, send repeated start sequence\n");
    return i2c_read_bytes(dev, address, data, length, 0);
}

int i2c_write_bytes(i2c_t dev, uint16_t address, const void *data,
                    size_t length, uint8_t flags)
{
    assert(dev < I2C_NUMOF);

    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }

    I2C_TypeDef *i2c = i2c_config[dev].dev;

    int ret = 0;

    if (!(flags & I2C_NOSTART)) {
        DEBUG("[i2c] write_bytes: start transmission\n");
        /* start transmission and send slave address */
        ret = _start(i2c, address, length, I2C_FLAG_WRITE, flags);
        if (ret < 0) {
            _stop(i2c);
            return ret;
        }
    }

    DEBUG("[i2c] write_bytes: write the data (%d bytes)\n", length);
    /* send out data bytes */
    ret = _write(i2c, data, length);
    if (ret < 0) {
        _stop(i2c);
        DEBUG("[i2c] write_bytes: nothing was written\n");
        return ret;
    }

    if (!(flags & I2C_NOSTOP)) {
        DEBUG("[i2c] write_bytes: end transmission\n");
        /* end transmission */
        ret = _stop(i2c);
        if (ret < 0) {
            return ret;
        }
    }

    return ret;
}

int i2c_write_regs(i2c_t dev, uint16_t address, uint16_t reg, const void *data,
                   size_t length, uint8_t flags)
{
    assert(dev < I2C_NUMOF);

    uint16_t tick = TICK_TIMEOUT;
    I2C_TypeDef *i2c = i2c_config[dev].dev;

    int ret = 0;

    if ((flags & I2C_REG16) || (flags & I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }

    /* Check to see if the bus is busy */
    while ((i2c->ISR & I2C_ISR_BUSY) && tick--) {}
    if (!tick) {
        return -ETIMEDOUT;
    }

    if (!(flags & I2C_NOSTART)) {
        /* start transmission and send slave address */
        /* increase length because our data is register+data */
        ret = _start(i2c, address, length + 1, I2C_FLAG_WRITE, flags);
        if (ret < 0) {
            _stop(i2c);
            return ret;
        }
    }

    /* send register number */
    DEBUG("[i2c] write_regs: ACK received, write reg into DR\n");
    i2c->TXDR = reg;

    /* write out data bytes */
    ret = _write(i2c, data, length);
    if (ret < 0) {
        _stop(i2c);
        return ret;
    }

    if (!(flags & I2C_NOSTOP)) {
        /* end transmission */
        ret = _stop(i2c);
        if (ret < 0) {
            return ret;
        }
    }

    return ret;
}

static inline int _start(I2C_TypeDef *i2c, uint16_t address,
                         size_t length, uint8_t rw_flag, uint8_t flags)
{
    /* 10 bit address not supported for now */
    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }

    assert(i2c != NULL);

    i2c->CR2 = 0;

    DEBUG("[i2c] start: set address mode\n");
    /* set address mode to 7-bit */
    i2c->CR2 &= ~(I2C_CR2_ADD10);

    DEBUG("[i2c] start: set slave address\n");
    /* set slave address */
    i2c->CR2 &= ~(I2C_CR2_SADD);
    i2c->CR2 |= (address << 1);

    DEBUG("[i2c] start: set transfert direction\n");
    /* set transfer direction */
    i2c->CR2 &= ~(I2C_CR2_RD_WRN);
    i2c->CR2 |= (rw_flag << I2C_CR2_RD_WRN_Pos);

    DEBUG("[i2c] start: set number of bytes\n");
    /* set number of bytes */
    i2c->CR2 &= ~(I2C_CR2_NBYTES);
    i2c->CR2 |= (length << I2C_CR2_NBYTES_Pos);

    /* configure autoend configuration */
    i2c->CR2 &= ~(I2C_CR2_AUTOEND);

    /* Clear interrupt */
    i2c->ICR |= CLEAR_FLAG;

    /* generate start condition */
    DEBUG("[i2c] start: generate start condition\n");
    i2c->CR2 |= I2C_CR2_START;

    /* Wait for the start followed by the address to be sent */
    uint16_t tick = TICK_TIMEOUT;
    while ((i2c->CR2 & I2C_CR2_START) && tick--) {}
    if (!tick) {
        return -ETIMEDOUT;
    }

    return _check_bus(i2c);
}

static inline int _read(I2C_TypeDef *i2c, uint8_t *data, size_t length)
{
    assert(i2c != NULL);

    for (size_t i = 0; i < length; i++) {
        /* wait for transfer to finish */
        DEBUG("[i2c] read: Waiting for DR to be full\n");
        uint16_t tick = TICK_TIMEOUT;
        while (!(i2c->ISR & I2C_ISR_RXNE) && tick--) {}
        if (i2c->ISR & ERROR_FLAG || !tick) {
            return -ETIMEDOUT;
        }

        DEBUG("[i2c] read: DR is now full\n");

        /* read data from data register */
        data[i] = i2c->RXDR;
        DEBUG("[i2c] read: Read byte %i from DR\n", i);
    }

    return _check_bus(i2c);
}

static inline int _write(I2C_TypeDef *i2c, const uint8_t *data, size_t length)
{
    assert(i2c != NULL);

    for (size_t i = 0; i < length; i++) {
        /* wait for ack */
        DEBUG("[i2c] write: Waiting for ACK\n");
        uint16_t tick = TICK_TIMEOUT;
        while (!(i2c->ISR & I2C_ISR_TXIS) && tick--)  {}
        if (i2c->ISR & ERROR_FLAG || !tick) {
            DEBUG("[i2c] write: TXIS timeout\n");
            return -ENXIO;
        }
        /* write data to data register */
        DEBUG("[i2c] write: Write byte %02X to DR\n", data[i]);
        i2c->TXDR = data[i];
        DEBUG("[i2c] write: Sending data\n");

        tick = TICK_TIMEOUT;
        while (!(i2c->ISR & I2C_ISR_TC) && tick--) {}
        if (!tick) {
            return -ETIMEDOUT;
        }

        int ret = _check_bus(i2c);
        if (ret < 0) {
            return ret;
        }
    }

    DEBUG("[i2c] write: Waiting for write to complete\n");
    uint16_t tick = TICK_TIMEOUT;
    while (!(i2c->ISR & I2C_ISR_TC) && tick--)  {}
    if (i2c->ISR & ERROR_FLAG || !tick) {
        DEBUG("[i2c] write: write didn't complete\n");
        return -ENXIO;
    }

    return 0;
}

static inline int _stop(I2C_TypeDef *i2c)
{
    assert(i2c != NULL);

    uint16_t tick = TICK_TIMEOUT;

    /* make sure transfer is complete */
    DEBUG("[i2c] stop: Wait for transfer to be complete\n");
    while (!(i2c->ISR & I2C_ISR_TC) && tick--) {}
    if (i2c->ISR & ERROR_FLAG || !tick) {
        return -EIO;
    }

    /* send STOP condition */
    DEBUG("[i2c] stop: Generate stop condition\n");
    i2c->CR2 |= I2C_CR2_STOP;

    /* Wait for the stop to complete */
    tick = TICK_TIMEOUT;
    while ((i2c->CR2 & I2C_CR2_STOP) && tick--) {}
    if (!tick) {
        return -ETIMEDOUT;
    }

    return 0;
}

static inline int _check_bus(I2C_TypeDef *i2c)
{
    assert(i2c != NULL);

    if (i2c->ISR & I2C_ISR_NACKF) {
        DEBUG("[i2c] check_bus: NACK received\n");
        return -ENXIO;
    }

    if (i2c->ISR & I2C_ISR_ARLO) {
        DEBUG("[i2c] check_bus: arbitration lost\n");
        return -EAGAIN;
    }

    if (i2c->ISR & I2C_ISR_BERR) {
        DEBUG("[i2c] check_bus: bus error\n");
        return -EIO;
    }

    return 0;
}

static inline void irq_handler(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    I2C_TypeDef *i2c = i2c_config[dev].dev;

    unsigned state = i2c->ISR;
    DEBUG("\n\n### I2C ERROR OCCURED ###\n");
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
