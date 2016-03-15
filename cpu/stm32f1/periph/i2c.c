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
 * For implementation details please refer to STM application note AN2824.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "irq.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* guard file in case no I2C device is defined */
#if I2C_0_EN || I2C_1_EN

/* static function definitions */
static void _i2c_init(I2C_TypeDef *i2c, int ccr);
static void _pin_config(gpio_t pin_scl, gpio_t pin_sda);
static void _start(I2C_TypeDef *dev, uint8_t address, uint8_t rw_flag, char *err);
static inline void _clear_addr(I2C_TypeDef *dev);
static inline void _write(I2C_TypeDef *dev, char *data, int length, char *err);
static inline void _stop(I2C_TypeDef *dev, char *err);

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

static char err_flag[] = {
#if I2C_0_EN
    [I2C_0] = 0x00,
#endif
#if I2C_1_EN
    [I2C_1] = 0x00,
#endif
#if I2C_2_EN
    [I2C_2] = 0x00
#endif
#if I2C_3_EN
    [I2C_3] = 0x00
#endif
};

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    I2C_TypeDef *i2c;
    gpio_t pin_scl, pin_sda;
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
            pin_scl = I2C_0_SCL_PIN;
            pin_sda = I2C_0_SDA_PIN;
            I2C_0_CLKEN();
            NVIC_SetPriority(I2C_0_ERR_IRQ, I2C_IRQ_PRIO);
            NVIC_EnableIRQ(I2C_0_ERR_IRQ);
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            i2c = I2C_1_DEV;
            pin_scl = I2C_1_SCL_PIN;
            pin_sda = I2C_1_SDA_PIN;
            I2C_1_CLKEN();
            NVIC_SetPriority(I2C_1_ERR_IRQ, I2C_IRQ_PRIO);
            NVIC_EnableIRQ(I2C_1_ERR_IRQ);
            break;
#endif
        default:
            return -1;
    }

    /* disable peripheral */
    i2c->CR1 &= ~I2C_CR1_PE;

    /* configure pins */
    _pin_config(pin_scl, pin_sda);
    /* configure device */
    _i2c_init(i2c, ccr);

    /* make sure the analog filters don't hang -> see errata sheet 2.14.7 */
    if (i2c->SR2 & I2C_SR2_BUSY) {
        DEBUG("LINE BUSY AFTER RESET -> toggle pins now\n");
        /* disable peripheral */
        i2c->CR1 &= ~I2C_CR1_PE;
        /* re-run pin config to toggle and re-configure pins */
        _pin_config(pin_scl, pin_sda);
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
    /* configure I2C clock and enable error interrupts */
    i2c->CR2 = (I2C_APBCLK / 1000000) | I2C_CR2_ITERREN;
    i2c->CCR = ccr;
    i2c->TRISE = (I2C_APBCLK / 1000000) + 1;
    /* configure device */
    i2c->OAR1 = 0;              /* makes sure we are in 7-bit address mode */
    /* enable device */
    i2c->CR1 |= I2C_CR1_PE;
}

static void _pin_config(gpio_t scl, gpio_t sda)
{
    /* toggle pins to reset analog filter -> see datasheet */
    /* set as output */
    gpio_init(scl, GPIO_OUT);
    gpio_init(sda, GPIO_OUT);
    /* run through toggling sequence */
    gpio_set(scl);
    gpio_set(sda);
    gpio_clear(sda);
    gpio_clear(scl);
    gpio_set(scl);
    gpio_set(sda);
    /* configure the pins alternate function */
    gpio_init_af(scl, GPIO_AF_OUT_OD);
    gpio_init_af(sda, GPIO_AF_OUT_OD);
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
    int i = 0;
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

    DEBUG("Send Slave address and wait for ADDR == 1\n");
    _start(i2c, address, I2C_FLAG_READ, &err_flag[dev]);

    if (err_flag[dev]) {
        return -2;
    }

    DEBUG("Clear ADDR\n");
    _clear_addr(i2c);

    switch (length) {
        case 1:
            break;

        case 2:
            DEBUG("Set POS and ACK bit\n");
            i2c->CR1 |= (I2C_CR1_POS | I2C_CR1_ACK);
            DEBUG("Crit block: clear ACK flag\n");
            i2c->CR1 &= ~(I2C_CR1_ACK);
            DEBUG("Wait for transfer to be completed\n");
            while (!(i2c->SR1 & I2C_SR1_BTF)) {}

            break;

        default:
            i2c->CR1 |= (I2C_CR1_ACK);

            while (i < (length - 3)) {
                DEBUG("Wait until byte was received\n");
                while (!(i2c->SR1 & I2C_SR1_RXNE)) {}
                DEBUG("Copy byte from DR\n");
                data[i++] = (char)i2c->DR;
            }

            DEBUG("Reading the last 3 bytes, waiting for BTF flag\n");
            while (!(i2c->SR1 & I2C_SR1_BTF)) {}

            DEBUG("Read N-3 byte\n");
            data[i++] = (char)i2c->DR;
    }

    DEBUG("Clear ACK\n");
    i2c->CR1 &= ~(I2C_CR1_ACK);

    DEBUG("Setting STOP=1\n");
    i2c->CR1 |= (I2C_CR1_STOP);

    while (i < length) {
        DEBUG("Wait for RXNE == 1\n");
        while (!(i2c->SR1 & I2C_SR1_RXNE)) {}

        DEBUG("Read byte\n");
        data[i++] = (char)i2c->DR;
    }

    DEBUG("wait for STOP bit to be cleared again\n");
    while (i2c->CR1 & I2C_CR1_STOP) {}

    DEBUG("reset POS = 0 and ACK = 1\n");
    i2c->CR1 &= ~(I2C_CR1_POS);
    i2c->CR1 |= (I2C_CR1_ACK);

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

    /* send start condition and slave address */
    DEBUG("Send slave address and clear ADDR flag\n");
    _start(i2c, address, I2C_FLAG_WRITE, &err_flag[dev]);
    _clear_addr(i2c);
    DEBUG("Write reg into DR\n");
    i2c->DR = reg;
    _stop(i2c, &err_flag[dev]);

    if (err_flag[dev]) {
        return -2;
    }

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
#if I2C_1_EN
        case I2C_1:
            i2c = I2C_1_DEV;
            break;
#endif
        default:
            return -1;
    }

    /* start transmission and send slave address */
    DEBUG("sending start sequence\n");
    _start(i2c, address, I2C_FLAG_WRITE, &err_flag[dev]);
    _clear_addr(i2c);
    /* send out data bytes */
    _write(i2c, data, length, &err_flag[dev]);
    /* end transmission */
    DEBUG("Ending transmission\n");
    _stop(i2c, &err_flag[dev]);
    DEBUG("STOP condition was send out\n");

    if (err_flag[dev]) {
        return -2;
    }
    else {
        return length;
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
#if I2C_1_EN
        case I2C_1:
            i2c = I2C_1_DEV;
            break;
#endif
        default:
            return -1;
    }

    /* start transmission and send slave address */
    _start(i2c, address, I2C_FLAG_WRITE, &err_flag[dev]);
    _clear_addr(i2c);
    /* send register address and wait for complete transfer to be finished*/
    _write(i2c, (char *)(&reg), 1, &err_flag[dev]);
    /* write data to register */
    _write(i2c, data, length, &err_flag[dev]);
    /* finish transfer */
    _stop(i2c, &err_flag[dev]);

    if (err_flag[dev]) {
        return -2;
    }
    else {
        /* return number of bytes send */
        return length;
    }
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
            while (I2C_0_DEV->SR2 & I2C_SR2_BUSY) {}
            I2C_0_CLKDIS();
            break;
#endif
#if I2C_1_EN
        case I2C_1:
            while (I2C_1_DEV->SR2 & I2C_SR2_BUSY) {}
            I2C_1_CLKDIS();
            break;
#endif
    }
}

static void _start(I2C_TypeDef *dev, uint8_t address, uint8_t rw_flag, char *err)
{
    /* flag that there's no error (yet) */
    *err = 0x00;
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
    while (!(dev->SR1 & I2C_SR1_ADDR) && !(*err)) {}
}

static inline void _clear_addr(I2C_TypeDef *dev)
{
    dev->SR1;
    dev->SR2;
}

static inline void _write(I2C_TypeDef *dev, char *data, int length, char *err)
{
    DEBUG("Looping through bytes\n");
    for (int i = 0; i < length && !(*err); i++) {
        /* write data to data register */
        dev->DR = (uint8_t)data[i];
        DEBUG("Written %i byte to data reg, now waiting for DR to be empty again\n", i);
        /* wait for transfer to finish */
        while (!(dev->SR1 & I2C_SR1_TXE) && !(*err)) {}
        DEBUG("DR is now empty again\n");
    }

}

static inline void _stop(I2C_TypeDef *dev, char *err)
{
    /* make sure last byte was send */
    while (!(dev->SR1 & I2C_SR1_BTF) && !(*err)) {}
    /* send STOP condition */
    dev->CR1 |= I2C_CR1_STOP;
    /* wait until transmission is complete */
    while (dev->SR2 & I2C_SR2_BUSY) {}
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

#if I2C_1_EN
void I2C_1_ERR_ISR(void)
{
    i2c_irq_handler(I2C_1, I2C_1_DEV);
}
#endif

#endif /* I2C_0_EN || I2C_1_EN */
