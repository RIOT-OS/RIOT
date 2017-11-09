/*
 * Copyright (C) 2017 we-sens.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32l0
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @note This implementation only implements the 7-bit addressing polling mode
 * (for now interrupt mode is not available)
 *
 * @author      Aurélien Fillau <aurelien.fillau@we-sens.com>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/i2c.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define ENABLE_DEBUG    (0)
#define TICK_TIMEOUT    (0xFFFF)
#include "debug.h"

#define CLEAR_FLAG(dev)            dev->ICR |= I2C_ICR_NACKCF   | \
                                               I2C_ICR_ARLOCF   | \
                                               I2C_ICR_BERRCF

#define ERROR_FLAG                 (I2C_ISR_NACKF | I2C_ISR_ARLO | I2C_ISR_BERR)

/* static function definitions */
static void _i2c_init(I2C_TypeDef *i2c, uint32_t presc, uint32_t scll,
                      uint32_t sclh, uint32_t sdadel, uint32_t scldel,
                      uint32_t timing);
static void _start(I2C_TypeDef *dev, uint8_t address, uint8_t length, uint8_t rw_flag);
static inline int _read(I2C_TypeDef *dev, uint8_t *data, int length);
static inline int _write(I2C_TypeDef *i2c, const uint8_t *data, int length);
static inline void _stop(I2C_TypeDef *i2c);

/**
 * @brief Array holding one pre-initialized mutex for each I2C device
 */
static mutex_t locks[] =  {
#if I2C_0_EN
    [I2C_0] = MUTEX_INIT,
#endif
#if I2C_1_EN
    [I2C_1] = MUTEX_INIT,
#endif
#if I2C_2_EN
    [I2C_2] = MUTEX_INIT,
#endif
#if I2C_3_EN
    [I2C_3] = MUTEX_INIT
#endif
};

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    I2C_TypeDef *i2c;
    gpio_t scl_pin;
    gpio_t sda_pin;
    gpio_af_t scl_af;
    gpio_af_t sda_af;
    uint32_t presc, scll, sclh, sdadel, scldel, timing;

    /*
     * Speed configuration:
     * Example values can be found in the STM32L0xx Reference manual RM0367
     * with f_I2CCLK = 16MHz
     * t_SCLL: SCL low level counter
     * t_SCLH: SCL high level counter
     * t_SDADEL: Delay before sending SDA output
     * t_SCLDEL: SCL low level during setup-time
     */
    switch (speed) {
        case I2C_SPEED_NORMAL:
            presc = 1;
            scll = 0x56;  /* t_SCLL   = 5.0us */
            sclh = 0x3E;  /* t_SCLH   = 4.0us */
            sdadel = 0x1; /* t_SDADEL = 500ns */
            scldel = 0xA; /* t_SCLDEL = 1250ns */
            break;

        case I2C_SPEED_FAST:
            presc = 0;
            scll = 0x2E;  /* t_SCLL   = 1250ns */
            sclh = 0x11;  /* t_SCLH   = 500ns */
            sdadel = 0x1; /* t_SDADEL = 125ns */
            scldel = 0xB; /* t_SCLDEL = 500ns */
            break;

        case I2C_SPEED_FAST_PLUS:
            presc = 0;
            scll = 0x6;   /* t_SCLL   = 875ns */
            sclh = 0x3;   /* t_SCLH   = 500ns */
            sdadel = 0x0; /* t_SDADEL = 0ns */
            scldel = 0x1; /* t_SCLDEL = 250ns */
            break;

        default:
            return -2;
    }

    /* prepare the timing register value */
    timing = ((presc << 28) | (scldel << 20) | (sdadel << 16) | (sclh << 8) | scll);

    /* read static device configuration */
    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = I2C_0_DEV;
            scl_pin = GPIO_PIN(I2C_0_SCL_PORT, I2C_0_SCL_PIN);     /* scl pin number */
            sda_pin = GPIO_PIN(I2C_0_SDA_PORT, I2C_0_SDA_PIN);     /* sda pin number */
            scl_af = I2C_0_SCL_AF;
            sda_af = I2C_0_SDA_AF;
            I2C_0_CLKEN();
            I2C_0_SCL_CLKEN();
            I2C_0_SDA_CLKEN();
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            i2c = I2C_1_DEV;
            scl_pin = GPIO_PIN(I2C_1_SCL_PORT, I2C_1_SCL_PIN);     /* scl pin number */
            sda_pin = GPIO_PIN(I2C_1_SDA_PORT, I2C_1_SDA_PIN);     /* sda pin number */
            scl_af = I2C_1_SCL_AF;
            sda_af = I2C_1_SDA_AF;
            I2C_1_CLKEN();
            I2C_1_SCL_CLKEN();
            I2C_1_SDA_CLKEN();
            break;
#endif
#if I2C_2_EN
        case I2C_2:
            i2c = I2C_2_DEV;
            scl_pin = GPIO_PIN(I2C_2_SCL_PORT, I2C_2_SCL_PIN);     /* scl pin number */
            sda_pin = GPIO_PIN(I2C_2_SDA_PORT, I2C_2_SDA_PIN);     /* sda pin number */
            scl_af = I2C_2_SCL_AF;
            sda_af = I2C_2_SDA_AF;
            I2C_2_CLKEN();
            I2C_2_SCL_CLKEN();
            I2C_2_SDA_CLKEN();
            break;
#endif

        default:
            return -1;
    }

    /* configure pins */
    gpio_init(scl_pin, GPIO_OD_PU);
    gpio_init_af(scl_pin, scl_af);
    gpio_init(sda_pin, GPIO_OD_PU);
    gpio_init_af(sda_pin, sda_af);

    /* configure device */
    _i2c_init(i2c, presc, scll, sclh, sdadel, scldel, timing);

    return 0;
}

static void _i2c_init(I2C_TypeDef *i2c, uint32_t presc, uint32_t scll,
                      uint32_t sclh, uint32_t sdadel, uint32_t scldel,
                      uint32_t timing)
{
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

    CLEAR_FLAG(i2c);

    /* enable device */
    i2c->CR1 |= I2C_CR1_PE;
}

int i2c_acquire(i2c_t dev)
{
    if (dev >= I2C_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[dev]);
    return 0;
}

int i2c_release(i2c_t dev)
{
    if (dev >= I2C_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[dev]);
    return 0;
}

int i2c_read_byte(i2c_t dev, uint8_t address, void *data)
{
    return i2c_read_bytes(dev, address, data, 1);
}

int i2c_read_bytes(i2c_t dev, uint8_t address, void *data, int length)
{
    I2C_TypeDef *i2c;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = I2C_0_DEV;
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            i2c = I2C_1_DEV;
            break;
#endif
#if I2C_2_EN
        case I2C_2:
            i2c = I2C_2_DEV;
            break;
#endif

        default:
            return -1;
    }

    /* start reception and send slave address */
    _start(i2c, address, length, I2C_FLAG_READ);

    /* read the data bytes */
    if (_read(i2c, data, length) < 0) {
        length = 0;
    }

    /* end transmission */
    _stop(i2c);

    return length;
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, void *data)
{
    return i2c_read_regs(dev, address, reg, data, 1);
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, void *data, int length)
{
    uint16_t tick = TICK_TIMEOUT;
    I2C_TypeDef *i2c;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = I2C_0_DEV;
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            i2c = I2C_1_DEV;
            break;
#endif
#if I2C_2_EN
        case I2C_2:
            i2c = I2C_2_DEV;
            break;
#endif

        default:
            return -1;
    }

    /* Check to see if the bus is busy */
    while ((i2c->ISR & I2C_ISR_BUSY) && tick--) {
        if ((i2c->ISR & ERROR_FLAG) || !tick) {
            /* end transmission */
            _stop(i2c);
            return -1;
        }
    }

    /* send start sequence and slave address */
    _start(i2c, address, 1, I2C_FLAG_WRITE);

    tick = TICK_TIMEOUT;
    /* wait for ack */
    DEBUG("Waiting for ACK\n");
    while (!(i2c->ISR & I2C_ISR_TXIS) && tick--) {
        if ((i2c->ISR & ERROR_FLAG) || !tick) {
            /* end transmission */
            _stop(i2c);
            return -1;
        }
    }

    DEBUG("Write register to read\n");
    i2c->TXDR = reg;

    /* send repeated start sequence, read registers and end transmission */
    DEBUG("ACK received, send repeated start sequence\n");
    return i2c_read_bytes(dev, address, data, length);
}

int i2c_write_byte(i2c_t dev, uint8_t address, uint8_t data)
{
    return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_bytes(i2c_t dev, uint8_t address, const void *data, int length)
{
    I2C_TypeDef *i2c;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = I2C_0_DEV;
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            i2c = I2C_1_DEV;
            break;
#endif
#if I2C_2_EN
        case I2C_2:
            i2c = I2C_2_DEV;
            break;
#endif

        default:
            return -1;
    }

    /* start transmission and send slave address */
    _start(i2c, address, length, I2C_FLAG_WRITE);

    /* send out data bytes */
    if (_write(i2c, data, length) < 0) {
        length = 0;
    }

    /* end transmission */
    _stop(i2c);

    return length;
}

int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, uint8_t data)
{
    return i2c_write_regs(dev, address, reg, &data, 1);
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, const void *data, int length)
{
    uint16_t tick = TICK_TIMEOUT;
    I2C_TypeDef *i2c;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = I2C_0_DEV;
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            i2c = I2C_1_DEV;
            break;
#endif
#if I2C_2_EN
        case I2C_2:
            i2c = I2C_2_DEV;
            break;
#endif

        default:
            return -1;
    }

    /* Check to see if the bus is busy */
    while ((i2c->ISR & I2C_ISR_BUSY) & tick--) {
        if ((i2c->ISR & ERROR_FLAG) || !tick) {
            return -1;
        }
    }

    /* start transmission and send slave address */
    /* increase length because our data is register+data */
    _start(i2c, address, length+1, I2C_FLAG_WRITE);

    /* send register number */
    DEBUG("ACK received, write reg into DR\n");
    i2c->TXDR = reg;

    /* write out data bytes */
    if (_write(i2c, data, length) < 0) {
        length = 0;
    }

    /* end transmission */
    _stop(i2c);

    return length;
}

void i2c_poweron(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            I2C_0_CLKEN();
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            I2C_1_CLKEN();
            break;
#endif
#if I2C_2_EN
        case I2C_2:
            I2C_2_CLKEN();
            break;
#endif
    }
}

void i2c_poweroff(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            while (I2C_0_DEV->ISR & I2C_ISR_BUSY) {}

            I2C_0_CLKDIS();
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            while (I2C_1_DEV->ISR & I2C_ISR_BUSY) {}

            I2C_1_CLKDIS();
            break;
#endif
#if I2C_2_EN
        case I2C_2:
            while (I2C_2_DEV->ISR & I2C_ISR_BUSY) {}

            I2C_2_CLKDIS();
            break;
#endif
    }
}

static void _start(I2C_TypeDef *dev, uint8_t address, uint8_t length, uint8_t rw_flag)
{
    dev->CR2 = 0;
    /* set address mode to 7-bit */
    dev->CR2 &= ~(I2C_CR2_ADD10);

    /* set slave address */
    dev->CR2 &= ~(I2C_CR2_SADD);
    dev->CR2 |= (address << 1);

    /* set transfer direction */
    dev->CR2 &= ~(I2C_CR2_RD_WRN);
    dev->CR2 |= (rw_flag << I2C_CR2_RD_WRN_Pos);

    /* set number of bytes */
    dev->CR2 &= ~(I2C_CR2_NBYTES);
    dev->CR2 |= (length << I2C_CR2_NBYTES_Pos);

    /* configure autoend configuration */
    dev->CR2 &= ~(I2C_CR2_AUTOEND);

    CLEAR_FLAG(dev);

    /* generate start condition */
    DEBUG("Generate start condition\n");
    dev->CR2 |= I2C_CR2_START;

    /* Wait for the start followed by the address to be sent */
    while (!(dev->CR2 & I2C_CR2_START)) {}
}

static inline int _read(I2C_TypeDef *dev, uint8_t *data, int length)
{
    uint16_t tick = TICK_TIMEOUT;

    for (int i = 0; i < length; i++) {
        /* wait for transfer to finish */
        DEBUG("Waiting for DR to be full\n");
        while (!(dev->ISR & I2C_ISR_RXNE) && tick--) {
            if (dev->ISR & ERROR_FLAG || !tick) {
                return -1;
            }
        }

        DEBUG("DR is now full\n");

        /* read data from data register */
        data[i] = dev->RXDR;
        DEBUG("Read byte %i from DR\n", i);
    }

    return 0;
}

static inline int _write(I2C_TypeDef *dev, const uint8_t *data, int length)
{
    uint16_t tick = TICK_TIMEOUT;

    for (int i = 0; i < length; i++) {
        /* wait for ack */
        DEBUG("Waiting for ACK\n");
        while (!(dev->ISR & I2C_ISR_TXIS) && tick--)  {
            if (dev->ISR & ERROR_FLAG || !tick) {
                return -1;
            }
        }

        /* write data to data register */
        DEBUG("Write byte %i to DR\n", i);
        dev->TXDR = data[i];
        DEBUG("Sending data\n");
    }

    return 0;
}

static inline void _stop(I2C_TypeDef *dev)
{
    uint16_t tick = TICK_TIMEOUT;

    /* make sure transfer is complete */
    DEBUG("Wait for transfer to be complete\n");
    while (!(dev->ISR & I2C_ISR_TC) && tick--) {
        if (dev->ISR & ERROR_FLAG || !tick) {
            break;
        }
    }

    /* send STOP condition */
    DEBUG("Generate stop condition\n");
    dev->CR2 |= I2C_CR2_STOP;
}
