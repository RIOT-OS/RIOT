/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  cpu_stm32f4
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
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

#define RW_BIT(flag)        (flag & 0x01)
#define ADDR7(addr)         (addr << 1)
#define ADDR10_HEAD(addr)   (0xf | ((addr >> 7) & 0x06))
#define ADDR10_TAIL(addr)   (addr & 0xff)

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

static inline I2C_TypeDef *dev(i2c_t bus)
{
    return i2c_config[bus].dev;
}

static inline void prep(i2c_t bus)
{
    mutex_lock(&locks[bus]);
    periph_clk_en(APB1, i2c_config[bus].rcc_mask);
}

static inline void done(i2c_t bus)
{
    periph_clk_dis(APB1, i2c_config[bus].rcc_mask);
    mutex_unlock(&locks[bus]);
}

int i2c_init(i2c_t bus)
{
    I2C_TypeDef *i2c;
    GPIO_TypeDef *port_scl;
    GPIO_TypeDef *port_sda;
    int pin_scl = 0, pin_sda = 0;
    int ccr;

    /* make sure the bus is valid */
    if (bus >= I2C_NUMOF) {
        return -1;
    }

    /* lock the bus for the duration of initialization and enable clock */
    prep(bus);

    /* disable and reset the device */
    dev(bus)->CR1 = 0;
    dev(bus)->OAR1 = 0;
    dev(bus)->OAR2 = 0;

    /* figure out the clock speed */
    i2c->CCR = i2c_config[bus].speed;
    i2c->TRISE = (CLOCK_APB1 / 1000000) + 1;
    i2c->CR2 = (CLOCK_APB1 / 1000000) | I2C_CR2_ITERREN;

    /* configure the pins */
    gpio_init(i2c_config[bus].scl_pin, i2c_config[bus].pin_mode);
    gpio_init(i2c_config[bus].sda_pin, i2c_config[bus].pin_mode);
    gpio_init_af(i2c_config[bus].scl_pin, i2c_config[bus].scl_af);
    gpio_init_af(i2c_config[bus].sda_pin, i2c_config[bus].sda_af);

    /* enable the error interrupt */
    NVIC_EnableIRQ(i2c_config[bus].irqn + 1);

    /* disable device clock again */
    done(bus);

    return 0;
}

void i2c_acquire(i2c_t bus)
{
    prep(bus);
    dev(bus)->CR1 = I2C_CR1_PE;
}

void i2c_release(i2c_t dev)
{
    dev(bus)->CR1 = 0;
    done(bus);
}

static int seq_addr(i2c_t bus, uint16_t addr, uint8_t flags)
{
    /* generate start condition */
    DEBUG("[i2c] generating start condition\n");
    dev->CR1 |= I2C_CR1_START;
    while (!(dev->SR1 & I2C_SR1_SB)) {}

    /* send address and read/write flag */
    if (flags & I2C_ADDR10) {
        DEBUG("[i2c] sending 10-bit address\n");
        dev(bus)->DR = (ADDR10_HEAD(addr) | RW_BIT(flag));
        while (!(dev(bus)->SR1 & I2C_SR1_ADD10)) {}
        if (err[bus]) {
            return;
        }
        dev(bus)->DR = ADDR10_TAIL(addr);
    }
    else {
        DEBUG("[i2c] sending 7-bit address\n");
        dev(bus)->DR = (ADDR7(addr) | RW_BIT(flags));
    }

    while (!(dev(bus)->SR1 & I2C_SR1_ADDR));
    if (err[bus]) {
        return;
    }
}

static inline void seq_addr_clear(i2c_t bus)
{
    DEBUG("[i2c] clearing ADDR flag\n");
    (void)dev(bus)->SR2;
}

static inline void seq_stop(i2c_t bus)
{
    /* make sure last byte was send */
    DEBUG("Wait if last byte hasn't been sent\n");

    /* send STOP condition */
    dev->CR1 |= I2C_CR1_STOP;
}


int i2c_transfer(i2c_t bus, uint16_t addr,
                 uint8_t *data, size_t len, uint8_t flags)
{
    /* send start condition */
    int stat = seq_addr(bus, addr, flags);
    if (stat != I2C_ACK) {
        seq_stop(bus);
        return stat;
    }

    if (flags & I2C_READ) {
        if (len == 1) {

        }
        seq_addr_clear(bus);

    }
    else {
        /* clears the ADDR bit */
        seq_addr_clear(bus);

        if (len == 1) {
            dev(bus)->DR = data[i];
            /* wait for last byte to be finished */
            /* send stop if not requested otherwise */
            if (!(flags & I2C_NOSTOP)) {
                seq_stop(bus);
            }
            while (!(dev->SR1 & I2C_SR1_BTF)) {}
            if (err[bus]) {
                return I2C_DATA_NACK;
            }
        }
        else {
            /* use DMA */
            ...

        }
    }

    return I2C_ACK;
}

int i2c_read_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    unsigned int state;
    int i = 0;



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
