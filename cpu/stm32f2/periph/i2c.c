/*
 * Copyright (C) 2014 FU Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32f2
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @note This implementation only implements the 7-bit addressing mode.
 *
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
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

#define I2C_MAX_LOOP_CNT 10000

/* static function definitions */
static int _read_bytes(I2C_TypeDef *i2c, uint8_t address, uint8_t *data, int length, uint8_t *err);
static void _i2c_init(I2C_TypeDef *i2c, int ccr);
static void _pin_config(GPIO_TypeDef *port_scl, GPIO_TypeDef *port_sda, int pin_scl, int pin_sda);
static int _start(I2C_TypeDef *dev, uint8_t address, uint8_t rw_flag, uint8_t *err);
static inline void _clear_addr(I2C_TypeDef *dev);
static inline int _write(I2C_TypeDef *dev, const uint8_t *data, int length, uint8_t *err);
static inline int _stop(I2C_TypeDef *dev, uint8_t *err);
static inline int _wait_ready(I2C_TypeDef *dev);

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

static uint8_t err_flag[] = {
#if I2C_0_EN
    [I2C_0] = 0x00,
#endif
#if I2C_1_EN
    [I2C_1] = 0x00,
#endif
#if I2C_2_EN
    [I2C_2] = 0x00,
#endif
#if I2C_3_EN
    [I2C_3] = 0x00
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
    i2c->OAR1 |= (1 << 14); /* datasheet: bit 14 should be kept 1 */
    i2c->OAR1 &= ~I2C_OAR1_ADDMODE; /* make sure we are in 7-bit address mode */
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
    port_sda->PUPDR &= ~(3 << (2 * pin_sda));
    if (I2C_0_SCL_PULLUP) {
        port_scl->PUPDR |= (1 << (2 * pin_scl));
    }
    if (I2C_0_SDA_PULLUP) {
        port_sda->PUPDR |= (1 << (2 * pin_sda));
    }

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

        default:
            return -1;
    }

    int res = _wait_ready(i2c);
    if (res != 0) {
        return res;
    }
    return _read_bytes(i2c, address, data, length, &err_flag[dev]);
}

static inline int _wait_ready(I2C_TypeDef *dev)
{
    /* wait for device to be ready */
    DEBUG("Wait for device to be ready\n");

    int cnt = 0;
    while ((dev->SR2 & I2C_SR2_BUSY) && cnt++ < I2C_MAX_LOOP_CNT) {}
    if (cnt == I2C_MAX_LOOP_CNT) {
        return -3;
    }

    return 0;
}

static int _read_bytes(I2C_TypeDef *i2c, uint8_t address, uint8_t *data, int length, uint8_t *err)
{
    unsigned int state;
    int i = 0;
    int cnt = 0;
    int res;

    switch (length) {
        case 1:
            DEBUG("Send Slave address and wait for ADDR == 1\n");
            res = _start(i2c, address, I2C_FLAG_READ, err);
            if (res != 0) {
                return res;
            }
            if (*err) {
                return -3;
            }

            DEBUG("Set ACK = 0\n");
            i2c->CR1 &= ~(I2C_CR1_ACK);

            DEBUG("Clear ADDR and set STOP = 1\n");
            state = irq_disable();
            _clear_addr(i2c);
            i2c->CR1 |= (I2C_CR1_STOP);
            irq_restore(state);

            DEBUG("Wait for RXNE == 1\n");

            cnt = 0;
            *err = 0;
            while (!(i2c->SR1 & I2C_SR1_RXNE) && cnt++ < I2C_MAX_LOOP_CNT && !(*err)) {}
            if (cnt == I2C_MAX_LOOP_CNT || *err) {
                return -3;
            }

            DEBUG("Read received data\n");
            *data = i2c->DR;

            /* wait until STOP is cleared by hardware */
            cnt = 0;
            *err = 0;
            while ((i2c->CR1 & I2C_CR1_STOP) && cnt++ < I2C_MAX_LOOP_CNT && !(*err)) {}
            if (cnt == I2C_MAX_LOOP_CNT) {
                return -3;
            }

            /* reset ACK to be able to receive new data */
            i2c->CR1 |= (I2C_CR1_ACK);
            break;

        case 2:
            DEBUG("Send Slave address and wait for ADDR == 1\n");
            res = _start(i2c, address, I2C_FLAG_READ, err);
            if (res != 0) {
                return res;
            }
            if (*err) {
                return -3;
            }
            DEBUG("Set POS bit\n");
            i2c->CR1 |= (I2C_CR1_POS | I2C_CR1_ACK);
            DEBUG("Crit block: Clear ADDR bit and clear ACK flag\n");
            state = irq_disable();
            _clear_addr(i2c);
            i2c->CR1 &= ~(I2C_CR1_ACK);
            irq_restore(state);

            DEBUG("Wait for transfer to be completed\n");

            cnt = 0;
            *err = 0;
            while (!(i2c->SR1 & I2C_SR1_BTF) && cnt++ < I2C_MAX_LOOP_CNT && !(*err)) {}
            if (cnt == I2C_MAX_LOOP_CNT || *err) {
                return -3;
            }

            DEBUG("Crit block: set STOP and read first byte\n");
            state = irq_disable();
            i2c->CR1 |= (I2C_CR1_STOP);
            data[0] = i2c->DR;
            irq_restore(state);

            DEBUG("read second byte\n");
            data[1] = i2c->DR;

            DEBUG("wait for STOP bit to be cleared again\n");

            cnt = 0;
            *err = 0;
            while ((i2c->CR1 & I2C_CR1_STOP) && cnt++ < I2C_MAX_LOOP_CNT && !(*err)) {}
            if (cnt == I2C_MAX_LOOP_CNT || *err) {
                return -3;
            }

            DEBUG("reset POS = 0 and ACK = 1\n");
            i2c->CR1 &= ~(I2C_CR1_POS);
            i2c->CR1 |= (I2C_CR1_ACK);
            break;

        default:
            DEBUG("Send Slave address and wait for ADDR == 1\n");
            res = _start(i2c, address, I2C_FLAG_READ, err);
            if (res != 0) {
                return res;
            }
            _clear_addr(i2c);

            while (i < (length - 3)) {
                DEBUG("Wait until byte was received\n");

                cnt = 0;
                *err = 0;
                while (!(i2c->SR1 & I2C_SR1_RXNE) && cnt++ < I2C_MAX_LOOP_CNT && !(*err)) {}
                if (cnt == I2C_MAX_LOOP_CNT || *err) {
                    return -3;
                }

                DEBUG("Copy byte from DR\n");
                data[i++] = i2c->DR;
            }

            DEBUG("Reading the last 3 bytes, waiting for BTF flag\n");

            cnt = 0;
            *err = 0;
            while (!(i2c->SR1 & I2C_SR1_BTF) && cnt++ < I2C_MAX_LOOP_CNT && !(*err));
            if (cnt == I2C_MAX_LOOP_CNT || *err) {
                return -3;
            }

            DEBUG("Disable ACK\n");
            i2c->CR1 &= ~(I2C_CR1_ACK);

            DEBUG("Crit block: set STOP and read N-2 byte\n");
            state = irq_disable();
            data[i++] = i2c->DR;
            i2c->CR1 |= (I2C_CR1_STOP);
            irq_restore(state);

            DEBUG("Read N-1 byte\n");
            data[i++] = i2c->DR;

            cnt = 0;
            *err = 0;
            while (!(i2c->SR1 & I2C_SR1_RXNE) && cnt++ < I2C_MAX_LOOP_CNT && !(*err)) {}
            if (cnt == I2C_MAX_LOOP_CNT || *err) {
                return -3;
            }

            DEBUG("Read last byte\n");

            data[i++] = i2c->DR;

            DEBUG("wait for STOP bit to be cleared again\n");

            cnt = 0;
            *err = 0;
            while ((i2c->CR1 & I2C_CR1_STOP) && cnt++ < I2C_MAX_LOOP_CNT && !(*err)) {}
            if (cnt == I2C_MAX_LOOP_CNT || *err) {
                return -3;
            }

            DEBUG("reset POS = 0 and ACK = 1\n");
            i2c->CR1 &= ~(I2C_CR1_POS);
            i2c->CR1 |= (I2C_CR1_ACK);
    }

    return length;
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, void *data)
{
    return i2c_read_regs(dev, address, reg, data, 1);
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, void *data, int length)
{
    I2C_TypeDef *i2c;
    int res;

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
    res = _wait_ready(i2c);
    if (res != 0) {
        return res;
    }
    res = _start(i2c, address, I2C_FLAG_WRITE, &err_flag[dev]);
    if (res != 0) {
        return res;
    }
    if (err_flag[dev]) {
        return -3;
    }
    _clear_addr(i2c);
    DEBUG("Write reg into DR\n");
    i2c->DR = reg;
    DEBUG("Now start a read transaction\n");
    return _read_bytes(i2c, address, data, length, &err_flag[dev]);
}

int i2c_write_byte(i2c_t dev, uint8_t address, uint8_t data)
{
    return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_bytes(i2c_t dev, uint8_t address, const void *data, int length)
{
    I2C_TypeDef *i2c;
    int res;

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
    res = _wait_ready(i2c);
    if (res != 0) {
        return res;
    }
    res = _start(i2c, address, I2C_FLAG_WRITE, &err_flag[dev]);
    if (res != 0) {
        return res;
    }
    if (err_flag[dev]) {
        return -3;
    }
    _clear_addr(i2c);
    /* send out data bytes */
    res = _write(i2c, data, length, &err_flag[dev]);
    if (res != 0) {
        return res;
    }
    if (err_flag[dev]) {
        return -3;
    }
    /* end transmission */
    DEBUG("Ending transmission\n");
    res = _stop(i2c, &err_flag[dev]);
    if (res != 0) {
        return res;
    }
    if (err_flag[dev]) {
        return -3;
    }
    DEBUG("STOP condition was send out\n");
    return length;
}

int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, uint8_t data)
{
    return i2c_write_regs(dev, address, reg, &data, 1);
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, const void *data, int length)
{
    I2C_TypeDef *i2c;
    int res;

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
    res = _wait_ready(i2c);
    if (res != 0) {
        return res;
    }
    res = _start(i2c, address, I2C_FLAG_WRITE, &err_flag[dev]);
    if (res != 0) {
        return res;
    }
    if (err_flag[dev]) {
        return -3;
    }
    _clear_addr(i2c);
    /* send register address and wait for complete transfer to be finished*/
    res = _write(i2c, &reg, 1, &err_flag[dev]);
    if (res != 0) {
        return res;
    }
    if (err_flag[dev]) {
        return -3;
    }
    /* write data to register */
    res = _write(i2c, data, length, &err_flag[dev]);
    if (res != 0) {
        return res;
    }
    if (err_flag[dev]) {
        return -3;
    }
    /* finish transfer */
    res = _stop(i2c, &err_flag[dev]);
    if (res != 0) {
        return res;
    }
    if (err_flag[dev]) {
        return -3;
    }
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
        for (int cnt = 0;
             ((I2C_0_DEV->SR2 & I2C_SR2_BUSY) && (cnt < I2C_MAX_LOOP_CNT));
             cnt++) {}

        I2C_0_CLKDIS();
        break;
#endif
    }
}

static int _start(I2C_TypeDef *dev, uint8_t address, uint8_t rw_flag, uint8_t *err)
{
    int cnt = 0;

    *err = 0;
    /* generate start condition */
    DEBUG("Generate start condition\n");
    dev->CR1 |= I2C_CR1_START;
    DEBUG("Wait for SB flag to be set\n");

    while (!(dev->SR1 & I2C_SR1_SB) && cnt++ < I2C_MAX_LOOP_CNT && !(*err));
    if (cnt == I2C_MAX_LOOP_CNT || *err) {
        return -3;
    }

    /* send address and read/write flag */
    DEBUG("Send address\n");
    dev->DR = (address << 1) | rw_flag;
    /* clear ADDR flag by reading first SR1 and then SR2 */
    DEBUG("Wait for ADDR flag to be set\n");

    cnt = 0;
    *err = 0;
    while (!(dev->SR1 & I2C_SR1_ADDR) && cnt++ < I2C_MAX_LOOP_CNT && !(*err)) {}
    if (cnt == I2C_MAX_LOOP_CNT) {
        return -3;
    }

    return 0;
}

static inline void _clear_addr(I2C_TypeDef *dev)
{
    dev->SR1;
    dev->SR2;
    DEBUG("Cleared address\n");
}

static inline int _write(I2C_TypeDef *dev, const uint8_t *data, int length, uint8_t *err)
{
    DEBUG("Looping through bytes\n");

    for (int i = 0; i < length; i++) {
        /* write data to data register */
        dev->DR = data[i];
        DEBUG("Written %i byte to data reg, now waiting for DR to be empty again\n", i);

        /* wait for transfer to finish */
        int cnt = 0;
        *err = 0;
        while (!(dev->SR1 & I2C_SR1_TXE) && cnt++ < I2C_MAX_LOOP_CNT && !(*err)) {}
        if (cnt == I2C_MAX_LOOP_CNT || *err) {
            return -3;
        }

        DEBUG("DR is now empty again\n");
    }

    return 0;
}

static inline int _stop(I2C_TypeDef *dev, uint8_t *err)
{
    /* make sure last byte was send */
    DEBUG("Wait if last byte hasn't been sent\n");

    int cnt = 0;
    *err = 0;
    while (!(dev->SR1 & I2C_SR1_BTF) && cnt++ < I2C_MAX_LOOP_CNT && !(*err)) {}
    if (cnt == I2C_MAX_LOOP_CNT) {
        return -3;
    }

    /* send STOP condition */
    dev->CR1 |= I2C_CR1_STOP;

    return 0;
}

static inline void i2c_irq_handler(i2c_t i2c_dev, I2C_TypeDef *dev)
{
    unsigned volatile state = dev->SR1;

    /* record and clear errors */
    err_flag[i2c_dev] = (state >> 8);
    dev->SR1 &= 0x00ff;

    DEBUG("\n\n### I2C %d ERROR OCCURED ###\n", i2c_dev);
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
}

#if I2C_0_EN
void I2C_0_ERR_ISR(void)
{
    i2c_irq_handler(I2C_0, I2C_0_DEV);
}
#endif
