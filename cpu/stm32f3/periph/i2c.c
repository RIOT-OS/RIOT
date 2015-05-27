/*
 * Copyright (C) 2015 Jan Pohlmann <jan-pohlmann@gmx.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  driver_periph
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @note This implementation only implements the 7-bit addressing mode.
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @auhtor      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Jan Pohlmann <jan-pohlmann@gmx.de>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "panic.h"
#include "irq.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* guard file in case no I2C device is defined */
#if I2C_NUMOF

/* static function definitions */
static void _i2c_init(I2C_TypeDef *i2c, uint32_t presc, uint32_t scll,
                      uint32_t sclh, uint32_t sdadel, uint32_t scldel,
                      uint32_t timing);
static void _pin_config(GPIO_TypeDef *port_scl, GPIO_TypeDef *port_sda,
                        int pin_scl, int pin_sda);
static void _start(I2C_TypeDef *dev, uint8_t address, uint8_t length,
                   uint8_t rw_flag);
static inline void _read(I2C_TypeDef *dev, char *data, int length);
static inline void _write(I2C_TypeDef *dev, char *data, int length);
static inline void _stop(I2C_TypeDef *dev);

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
    [I2C_2] = MUTEX_INIT
#endif
#if I2C_3_EN
    [I2C_3] = MUTEX_INIT
#endif
};

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    I2C_TypeDef *i2c;
    GPIO_TypeDef *port_scl;
    GPIO_TypeDef *port_sda;
    int pin_scl = 0, pin_sda = 0;
    uint32_t presc, scll, sclh, sdadel, scldel, timing;

    /*
     * Speed configuration:
     * Example values can be found in the STM32F3xx Reference manual RM0316
     * Chapter 28.4.9: Table 148 Examples of timings settings for f_I2CCLK = 8MHz
     * t_SCLL: SCL low level counter
     * t_SCLH: SCL high level counter
     * t_SDADEL: Delay before sending SDA output
     * t_SCLDEL: SCL low level during setup-time
     */
    switch (speed) {
        case I2C_SPEED_NORMAL:
            presc = 1;
            scll = 0x13;  /* t_SCLL   = 5.0us */
            sclh = 0xF;   /* t_SCLH   = 4.0us */
            sdadel = 0x2; /* t_SDADEL = 500ns */
            scldel = 0x4; /* t_SCLDEL = 1250ns */
            break;

        case I2C_SPEED_FAST:
            presc = 0;
            scll = 0x9;   /* t_SCLL   = 1250ns */
            sclh = 0x3;   /* t_SCLH   = 500ns */
            sdadel = 0x1; /* t_SDADEL = 125ns */
            scldel = 0x3; /* t_SCLDEL = 500ns */
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
            port_scl = I2C_0_SCL_PORT;
            pin_scl = I2C_0_SCL_PIN;
            port_sda = I2C_0_SDA_PORT;
            pin_sda = I2C_0_SDA_PIN;
            I2C_0_CLKEN();
            I2C_0_SCL_CLKEN();
            I2C_0_SDA_CLKEN();
            NVIC_SetPriority(I2C_0_ERR_IRQ, I2C_IRQ_PRIO);
            NVIC_EnableIRQ(I2C_0_ERR_IRQ);
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            i2c = I2C_1_DEV;
            port_scl = I2C_1_SCL_PORT;
            pin_scl = I2C_1_SCL_PIN;
            port_sda = I2C_1_SDA_PORT;
            pin_sda = I2C_1_SDA_PIN;
            I2C_1_CLKEN();
            I2C_1_SCL_CLKEN();
            I2C_1_SDA_CLKEN();
            NVIC_SetPriority(I2C_1_ERR_IRQ, I2C_IRQ_PRIO);
            NVIC_EnableIRQ(I2C_1_ERR_IRQ);
            break;
#endif

        default:
            return -1;
    }

    /* configure pins */
    _pin_config(port_scl, port_sda, pin_scl, pin_sda);

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

    /* enable device */
    i2c->CR1 |= I2C_CR1_PE;
}

static void _pin_config(GPIO_TypeDef *port_scl, GPIO_TypeDef *port_sda,
                        int pin_scl, int pin_sda)
{
    /* Set GPIOs to AF mode */
    port_scl->MODER &= ~(3 << (2 * pin_scl));
    port_scl->MODER |= (2 << (2 * pin_scl));
    port_sda->MODER &= ~(3 << (2 * pin_sda));
    port_sda->MODER |= (2 << (2 * pin_sda));

    /* Set speed high*/
    port_scl->OSPEEDR |= (3 << (2 * pin_scl));
    port_sda->OSPEEDR |= (3 << (2 * pin_sda));

    /* Set to push-pull configuration open drain*/
    port_scl->OTYPER |= (1 << pin_scl);
    port_sda->OTYPER |= (1 << pin_sda);

    /* Enable pull-up resistors */
    port_scl->PUPDR &= ~(3 << (2 * pin_scl));
    port_scl->PUPDR |= (1 << (2 * pin_scl));
    port_sda->PUPDR &= ~(3 << (2 * pin_sda));
    port_sda->PUPDR |= (1 << (2 * pin_sda));

    /* Configure GPIOs to for the I2C alternate function */
    if (pin_scl < 8) {
        port_scl->AFR[0] &= ~(0xf << (4 * pin_scl));
        port_scl->AFR[0] |= (I2C_0_SCL_AF << (4 * pin_scl));
    }
    else {
        port_scl->AFR[1] &= ~(0xf << (4 * (pin_scl - 8)));
        port_scl->AFR[1] |= (I2C_0_SCL_AF << (4 * (pin_scl - 8)));
    }

    if (pin_sda < 8) {
        port_sda->AFR[0] &= ~(0xf << (4 * pin_sda));
        port_sda->AFR[0] |= (I2C_0_SDA_AF << (4 * pin_sda));
    }
    else {
        port_sda->AFR[1] &= ~(0xf << (4 * (pin_sda - 8)));
        port_sda->AFR[1] |= (I2C_0_SDA_AF << (4 * (pin_sda - 8)));
    }
}

int i2c_init_slave(i2c_t dev, uint8_t address)
{
    /* TODO: implement slave mode */
    return -1;
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

int i2c_read_byte(i2c_t dev, uint8_t address, char *data)
{
    return i2c_read_bytes(dev, address, data, 1);
}

int i2c_read_bytes(i2c_t dev, uint8_t address, char *data, int length)
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

        default:
            return -1;
    }

    /* start reception and send slave address */
    _start(i2c, address, length, I2C_FLAG_READ);

    /* read the data bytes */
    _read(i2c, data, length);

    /* end transmission */
    _stop(i2c);

    return length;
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, char *data)
{
    return i2c_read_regs(dev, address, reg, data, 1);
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
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

        default:
            return -1;
    }

    /* send start sequence and slave address */
    _start(i2c, address, length, I2C_FLAG_WRITE);

    /* wait for ack */
    DEBUG("Waiting for ACK\n");
    while (!(i2c->ISR & I2C_ISR_TXIS));

    /* send register number */
    DEBUG("ACK received, write reg into DR\n");
    i2c->TXDR = reg;

    /* send repeated start sequence, read registers and end transmission */
    DEBUG("ACK received, send repeated start sequence\n");
    return i2c_read_bytes(dev, address, data, length);
}

int i2c_write_byte(i2c_t dev, uint8_t address, char data)
{
    return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_bytes(i2c_t dev, uint8_t address, char *data, int length)
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

        default:
            return -1;
    }

    /* start transmission and send slave address */
    _start(i2c, address, length, I2C_FLAG_WRITE);

    /* send out data bytes */
    _write(i2c, data, length);

    /* end transmission */
    _stop(i2c);

    return length;
}

int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, char data)
{
    return i2c_write_regs(dev, address, reg, &data, 1);
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
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

        default:
            return -1;
    }

    /* start transmission and send slave address */
    /* increase length because our data is register+data */
    _start(i2c, address, length+1, I2C_FLAG_WRITE);

    /* wait for ack */
    DEBUG("Waiting for ACK\n");
    while (!(i2c->ISR & I2C_ISR_TXIS));

    /* send register number */
    DEBUG("ACK received, write reg into DR\n");
    i2c->TXDR = reg;

    /* write out data bytes */
    _write(i2c, data, length);

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
    }
}

void i2c_poweroff(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            while (I2C_0_DEV->ISR & I2C_ISR_BUSY);

            I2C_0_CLKDIS();
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            while (I2C_1_DEV->ISR & I2C_ISR_BUSY);

            I2C_0_CLKDIS();
            break;
#endif
    }
}

static void _start(I2C_TypeDef *dev, uint8_t address, uint8_t length, uint8_t rw_flag)
{
    /* set address mode to 7-bit */
    dev->CR2 &= ~(I2C_CR2_ADD10);

    /* set slave address */
    dev->CR2 &= ~(I2C_CR2_SADD);
    dev->CR2 |= (address << 1);

    /* set transfer direction */
    dev->CR2 &= ~(I2C_CR2_RD_WRN);
    dev->CR2 |= (rw_flag << 10);

    /* set number of bytes */
    dev->CR2 &= ~(I2C_CR2_NBYTES);
    dev->CR2 |= (length << 16);

    /* configure autoend configuration */
    dev->CR2 &= ~(I2C_CR2_AUTOEND);

    /* generate start condition */
    DEBUG("Generate start condition\n");
    dev->CR2 |= I2C_CR2_START;
}

static inline void _read(I2C_TypeDef *dev, char *data, int length)
{
    for (int i = 0; i < length; i++) {
        /* wait for transfer to finish */
        DEBUG("Waiting for DR to be full\n");
        while (!(dev->ISR & I2C_ISR_RXNE));
        DEBUG("DR is now full\n");

        /* read data from data register */
        data[i] = (uint8_t)dev->RXDR;
        DEBUG("Read byte %i from DR\n", i);
    }
}

static inline void _write(I2C_TypeDef *dev, char *data, int length)
{
    for (int i = 0; i < length; i++) {
        /* wait for ack */
        DEBUG("Waiting for ACK\n");
        while (!(dev->ISR & I2C_ISR_TXIS));

        /* write data to data register */
        DEBUG("Write byte %i to DR\n", i);
        dev->TXDR = (uint8_t)data[i];
        DEBUG("Sending data\n");
    }
}

static inline void _stop(I2C_TypeDef *dev)
{
    /* make sure transfer is complete */
    DEBUG("Wait for transfer to be complete\n");
    while (!(dev->ISR & I2C_ISR_TC));

    /* send STOP condition */
    DEBUG("Generate stop condition\n");
    dev->CR2 |= I2C_CR2_STOP;
}

#if I2C_0_EN
void I2C_0_ERR_ISR(void)
{
    unsigned state = I2C_0_DEV->ISR;
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
    core_panic(0x0,"I2C FAULT");
}
#endif /* I2C_0_EN */

#endif /* I2C_NUMOF */
