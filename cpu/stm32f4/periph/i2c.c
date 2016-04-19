/*
 * Copyright (C) 2014 FU Berlin
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
 * @auhtor      Thomas Eichinge <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "irq.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* guard file in case no I2C device is defined */
#if I2C_NUMOF

/* static function definitions */
static void _i2c_init(I2C_TypeDef *i2c, int ccr);
static void _toggle_pins(GPIO_TypeDef *port_scl, GPIO_TypeDef *port_sda, int pin_scl, int pin_sda);
static void _pin_config(GPIO_TypeDef *port_scl, GPIO_TypeDef *port_sda, int pin_scl, int pin_sda);
static void _start(I2C_TypeDef *dev, uint8_t address, uint8_t rw_flag);
static inline void _clear_addr(I2C_TypeDef *dev);
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
    int ccr;

    /* read speed configuration */
    switch (speed) {
        case I2C_SPEED_NORMAL:
            ccr = I2C_APBCLK / 200000;
            break;

        case I2C_SPEED_FAST:
            ccr = I2C_APBCLK / 800000;
            break;

        default:
            return -2;
    }

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

        default:
            return -1;
    }

    /* configure pins */
    _pin_config(port_scl, port_sda, pin_scl, pin_sda);

    /* configure device */
    _i2c_init(i2c, ccr);

    /* make sure the analog filters don't hang -> see errata sheet 2.14.7 */
    if (i2c->SR2 & I2C_SR2_BUSY) {
        DEBUG("LINE BUSY AFTER RESET -> toggle pins now\n");
        /* disable peripheral */
        i2c->CR1 &= ~I2C_CR1_PE;
        /* toggle both pins to reset analog filter */
        _toggle_pins(port_scl, port_sda, pin_scl, pin_sda);
        /* reset pins for alternate function */
        _pin_config(port_scl, port_sda, pin_scl, pin_sda);
        /* make peripheral soft reset */
        i2c->CR1 |= I2C_CR1_SWRST;
        i2c->CR1 &= ~I2C_CR1_SWRST;
        /* enable device */
        _i2c_init(i2c, ccr);
    }
    return 0;
}

static void _i2c_init(I2C_TypeDef *i2c, int ccr)
{
    /* disable device and set ACK bit */
    i2c->CR1 = I2C_CR1_ACK;
    /* configure I2C clock */
    i2c->CR2 = (I2C_APBCLK / 1000000) | I2C_CR2_ITERREN;
    i2c->CCR = ccr;
    i2c->TRISE = (I2C_APBCLK / 1000000) + 1;
    /* configure device */
    i2c->OAR1 = 0;              /* makes sure we are in 7-bit address mode */
    /* enable device */
    i2c->CR1 |= I2C_CR1_PE;
}

static void _pin_config(GPIO_TypeDef *port_scl, GPIO_TypeDef *port_sda, int pin_scl, int pin_sda)
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

static void _toggle_pins(GPIO_TypeDef *port_scl, GPIO_TypeDef *port_sda, int pin_scl, int pin_sda)
{
    /* Set GPIOs to General purpose output mode mode */
    port_scl->MODER &= ~(3 << (2 * pin_scl));
    port_scl->MODER |= (1 << (2 * pin_scl));
    port_sda->MODER &= ~(3 << (2 * pin_sda));
    port_sda->MODER |= (1 << (2 * pin_sda));

    /* Set speed high*/
    port_scl->OSPEEDR |= (3 << (2 * pin_scl));
    port_sda->OSPEEDR |= (3 << (2 * pin_sda));

    /* Set to push-pull configuration open drain*/
    port_scl->OTYPER |= (1 << pin_scl);
    port_sda->OTYPER |= (1 << pin_sda);

    /* set both to high */
    port_scl->ODR |= (1 << pin_scl);
    port_sda->ODR |= (1 << pin_sda);
    /* set SDA to low */
    port_sda->ODR &= ~(1 << pin_sda);
    /* set SCL to low */
    port_scl->ODR &= ~(1 << pin_scl);
    /* set SCL to high */
    port_scl->ODR |= (1 << pin_scl);
    /* set SDA to high */
    port_sda->ODR |= (1 << pin_sda);
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
    unsigned int state;
    int i = 0;
    I2C_TypeDef *i2c;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = I2C_0_DEV;
            break;
#endif

        default:
            return -1;
    }

    switch (length) {
        case 1:
            DEBUG("Send Slave address and wait for ADDR == 1\n");
            _start(i2c, address, I2C_FLAG_READ);

            DEBUG("Set ACK = 0\n");
            i2c->CR1 &= ~(I2C_CR1_ACK);

            DEBUG("Clear ADDR and set STOP = 1\n");
            state = irq_disable();
            _clear_addr(i2c);
            i2c->CR1 |= (I2C_CR1_STOP);
            irq_restore(state);

            DEBUG("Wait for RXNE == 1\n");

            while (!(i2c->SR1 & I2C_SR1_RXNE)) {}

            DEBUG("Read received data\n");
            *data = (char)i2c->DR;

            /* wait until STOP is cleared by hardware */
            while (i2c->CR1 & I2C_CR1_STOP) {}

            /* reset ACK to be able to receive new data */
            i2c->CR1 |= (I2C_CR1_ACK);
            break;

        case 2:
            DEBUG("Send Slave address and wait for ADDR == 1\n");
            _start(i2c, address, I2C_FLAG_READ);
            DEBUG("Set POS bit\n");
            i2c->CR1 |= (I2C_CR1_POS | I2C_CR1_ACK);
            DEBUG("Crit block: Clear ADDR bit and clear ACK flag\n");
            state = irq_disable();
            _clear_addr(i2c);
            i2c->CR1 &= ~(I2C_CR1_ACK);
            irq_restore(state);

            DEBUG("Wait for transfer to be completed\n");

            while (!(i2c->SR1 & I2C_SR1_BTF)) {}

            DEBUG("Crit block: set STOP and read first byte\n");
            state = irq_disable();
            i2c->CR1 |= (I2C_CR1_STOP);
            data[0] = (char)i2c->DR;
            irq_restore(state);

            DEBUG("read second byte\n");
            data[1] = (char)i2c->DR;

            DEBUG("wait for STOP bit to be cleared again\n");

            while (i2c->CR1 & I2C_CR1_STOP) {}

            DEBUG("reset POS = 0 and ACK = 1\n");
            i2c->CR1 &= ~(I2C_CR1_POS);
            i2c->CR1 |= (I2C_CR1_ACK);
            break;

        default:
            DEBUG("Send Slave address and wait for ADDR == 1\n");
            _start(i2c, address, I2C_FLAG_READ);
            _clear_addr(i2c);

            while (i < (length - 3)) {
                DEBUG("Wait until byte was received\n");

                while (!(i2c->SR1 & I2C_SR1_RXNE)) {}

                DEBUG("Copy byte from DR\n");
                data[i++] = (char)i2c->DR;
            }

            DEBUG("Reading the last 3 bytes, waiting for BTF flag\n");

            while (!(i2c->SR1 & I2C_SR1_BTF)) {}

            DEBUG("Disable ACK\n");
            i2c->CR1 &= ~(I2C_CR1_ACK);

            DEBUG("Crit block: set STOP and read N-2 byte\n");
            state = irq_disable();
            data[i++] = (char)i2c->DR;
            i2c->CR1 |= (I2C_CR1_STOP);
            irq_restore(state);

            DEBUG("Read N-1 byte\n");
            data[i++] = (char)i2c->DR;

            while (!(i2c->SR1 & I2C_SR1_RXNE)) {}

            DEBUG("Read last byte\n");

            data[i++] = (char)i2c->DR;

            DEBUG("wait for STOP bit to be cleared again\n");

            while (i2c->CR1 & I2C_CR1_STOP) {}

            DEBUG("reset POS = 0 and ACK = 1\n");
            i2c->CR1 &= ~(I2C_CR1_POS);
            i2c->CR1 |= (I2C_CR1_ACK);
    }

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

        default:
            return -1;
    }

    /* send start condition and slave address */
    DEBUG("Send slave address and clear ADDR flag\n");
    _start(i2c, address, I2C_FLAG_WRITE);
    _clear_addr(i2c);
    DEBUG("Write reg into DR\n");
    i2c->DR = reg;
    _stop(i2c);
    DEBUG("Now start a read transaction\n");
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

        default:
            return -1;
    }

    /* start transmission and send slave address */
    DEBUG("sending start sequence\n");
    _start(i2c, address, I2C_FLAG_WRITE);
    _clear_addr(i2c);
    /* send out data bytes */
    _write(i2c, data, length);
    /* end transmission */
    DEBUG("Ending transmission\n");
    _stop(i2c);
    DEBUG("STOP condition was send out\n");
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

        default:
            return -1;
    }

    /* start transmission and send slave address */
    _start(i2c, address, I2C_FLAG_WRITE);
    _clear_addr(i2c);
    /* send register address and wait for complete transfer to be finished*/
    _write(i2c, (char *)(&reg), 1);
    /* write data to register */
    _write(i2c, data, length);
    /* finish transfer */
    _stop(i2c);
    /* return number of bytes send */
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
    }
}

void i2c_poweroff(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            while (I2C_0_DEV->SR2 & I2C_SR2_BUSY) {}

            I2C_0_CLKDIS();
            break;
#endif
    }
}

static void _start(I2C_TypeDef *dev, uint8_t address, uint8_t rw_flag)
{
    /* wait for device to be ready */
    DEBUG("Wait for device to be ready\n");

    while (dev->SR2 & I2C_SR2_BUSY) {}

    /* generate start condition */
    DEBUG("Generate start condition\n");
    dev->CR1 |= I2C_CR1_START;
    DEBUG("Wait for SB flag to be set\n");

    while (!(dev->SR1 & I2C_SR1_SB)) {}

    /* send address and read/write flag */
    DEBUG("Send address\n");
    dev->DR = (address << 1) | rw_flag;
    /* clear ADDR flag by reading first SR1 and then SR2 */
    DEBUG("Wait for ADDR flag to be set\n");

    while (!(dev->SR1 & I2C_SR1_ADDR)) {}
}

static inline void _clear_addr(I2C_TypeDef *dev)
{
    dev->SR1;
    dev->SR2;
    DEBUG("Cleared address\n");
}

static inline void _write(I2C_TypeDef *dev, char *data, int length)
{
    DEBUG("Looping through bytes\n");

    for (int i = 0; i < length; i++) {
        /* write data to data register */
        dev->DR = (uint8_t)data[i];
        DEBUG("Written %i byte to data reg, now waiting for DR to be empty again\n", i);

        /* wait for transfer to finish */
        while (!(dev->SR1 & I2C_SR1_TXE)) {}

        DEBUG("DR is now empty again\n");
    }
}

static inline void _stop(I2C_TypeDef *dev)
{
    /* make sure last byte was send */
    DEBUG("Wait if last byte hasn't been sent\n");

    while (!(dev->SR1 & I2C_SR1_BTF)) {}

    /* send STOP condition */
    dev->CR1 |= I2C_CR1_STOP;
}

#if I2C_0_EN
void I2C_0_ERR_ISR(void)
{
    unsigned state = I2C_0_DEV->SR1;
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
    while (1) {}
}
#endif /* I2C_0_EN */

#endif /* I2C_NUMOF */
