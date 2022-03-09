/*
 * Copyright (C) 2016-2017 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "cpu.h"
#include "mutex.h"
#include "byteorder.h"

#include "periph_conf.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

#include "em_cmu.h"
#include "em_i2c.h"

/**
 * @brief   Large-enough value to have some timeout value for rogue I2C
 *          transfers. Value based on kit driver (shipped with Simplicity
 *          Studio).
 */
#define I2C_TIMEOUT (300000)

/**
 * @brief   Holds the I2C transfer progress.
 */
static volatile I2C_TransferReturn_TypeDef i2c_progress[I2C_NUMOF];

/**
 * @brief   Initialized bus locks (we have a maximum of three devices)
 */
static mutex_t i2c_lock[I2C_NUMOF];

/**
 * @brief   Start and track an I2C transfer.
 */
static int _transfer(i2c_t dev, I2C_TransferSeq_TypeDef *transfer)
{
    bool busy = true;
    uint32_t timeout = I2C_TIMEOUT;

    /* start the i2c transaction */
    i2c_progress[dev] = I2C_TransferInit(i2c_config[dev].dev, transfer);

    /* the transfer progresses via the interrupt handler */
    while (busy) {
        unsigned int cpsr = irq_disable();

        if (i2c_progress[dev] == i2cTransferInProgress && timeout--) {
            cortexm_sleep_until_event();
        }
        else {
            busy = false;
        }

        irq_restore(cpsr);
    }

    /* check for timeout */
    if (!timeout) {
        return -ETIMEDOUT;
    }

    /* transfer finished, interpret the result */
    switch (i2c_progress[dev]) {
        case i2cTransferDone:
            return 0;
        case i2cTransferUsageFault:
            return -EINVAL;
        case i2cTransferAddrNack:
            return -ENXIO;
        case i2cTransferArbLost:
            return -EAGAIN;
        default:
            return -EIO;
    }
}

void i2c_init(i2c_t dev)
{
    /* check if device is valid */
    assert(dev < I2C_NUMOF);

    /* initialize lock */
    mutex_init(&i2c_lock[dev]);

    /* enable clocks */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(i2c_config[dev].cmu, true);

    /* configure the pins */
    gpio_init(i2c_config[dev].scl_pin, GPIO_OD);
    gpio_init(i2c_config[dev].sda_pin, GPIO_OD);

    /* ensure slave is in a known state, which it may not be after a reset */
    for (int i = 0; i < 9; i++) {
        gpio_set(i2c_config[dev].scl_pin);
        gpio_clear(i2c_config[dev].scl_pin);
    }

    /* reset and initialize the peripheral */
    I2C_Init_TypeDef init = I2C_INIT_DEFAULT;

    init.enable = false;
    init.freq = (uint32_t) i2c_config[dev].speed;

    I2C_Reset(i2c_config[dev].dev);
    I2C_Init(i2c_config[dev].dev, &init);

    /* configure pin functions */
#if defined(_SILICON_LABS_32B_SERIES_0)
    i2c_config[dev].dev->ROUTE = (i2c_config[dev].loc |
                                  I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN);
#elif defined(_SILICON_LABS_32B_SERIES_1)
    i2c_config[dev].dev->ROUTEPEN = I2C_ROUTEPEN_SDAPEN | I2C_ROUTEPEN_SCLPEN;
    i2c_config[dev].dev->ROUTELOC0 = i2c_config[dev].loc;
#endif

    /* enable interrupts */
    NVIC_ClearPendingIRQ(i2c_config[dev].irq);
    NVIC_EnableIRQ(i2c_config[dev].irq);

    /* enable peripheral */
    I2C_Enable(i2c_config[dev].dev, true);
}

void i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    /* acquire lock */
    mutex_lock(&i2c_lock[dev]);

    /* power peripheral */
    CMU_ClockEnable(i2c_config[dev].cmu, true);
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    /* disable peripheral */
    CMU_ClockEnable(i2c_config[dev].cmu, false);

    /* release lock */
    mutex_unlock(&i2c_lock[dev]);
}

int i2c_read_bytes(i2c_t dev, uint16_t address, void *data, size_t length, uint8_t flags)
{
    if (flags & (I2C_NOSTART | I2C_NOSTOP)) {
        return -EOPNOTSUPP;
    }

    /* prepare transfer */
    I2C_TransferSeq_TypeDef transfer;

    transfer.addr = (address << 1);
    transfer.flags = I2C_FLAG_READ | ((flags & I2C_ADDR10) ? I2C_FLAG_10BIT_ADDR : 0);
    transfer.buf[0].data = (uint8_t *) data;
    transfer.buf[0].len = length;

    /* start a transfer */
    return _transfer(dev, &transfer);
}

int i2c_read_regs(i2c_t dev, uint16_t address, uint16_t reg,
                  void *data, size_t length, uint8_t flags)
{
    uint16_t reg_end = reg;

    if (flags & (I2C_NOSTART | I2C_NOSTOP)) {
        return -EOPNOTSUPP;
    }

    /* Handle endianness of register if 16 bit */
    if (flags & I2C_REG16) {
        reg_end = htons(reg); /* Make sure register is in big-endian on I2C bus */
    }

    /* prepare transfer */
    I2C_TransferSeq_TypeDef transfer;

    transfer.addr = (address << 1);
    transfer.flags = I2C_FLAG_WRITE_READ | ((flags & I2C_ADDR10) ? I2C_FLAG_10BIT_ADDR : 0);
    transfer.buf[0].data = (uint8_t *) &reg_end;
    transfer.buf[0].len = (flags & I2C_REG16) ? 2 : 1;
    transfer.buf[1].data = (uint8_t *) data;
    transfer.buf[1].len = length;

    /* start a transfer */
    return _transfer(dev, &transfer);
}

int i2c_write_bytes(i2c_t dev, uint16_t address, const void *data, size_t length, uint8_t flags)
{
    if (flags & (I2C_NOSTART | I2C_NOSTOP)) {
        return -EOPNOTSUPP;
    }

    /* prepare transfer */
    I2C_TransferSeq_TypeDef transfer;

    transfer.addr = (address << 1);
    transfer.flags = I2C_FLAG_WRITE | ((flags & I2C_ADDR10) ? I2C_FLAG_10BIT_ADDR : 0);
    transfer.buf[0].data = (uint8_t *) data;
    transfer.buf[0].len = length;

    /* start a transfer */
    return _transfer(dev, &transfer);
}

int i2c_write_regs(i2c_t dev, uint16_t address, uint16_t reg,
                   const void *data, size_t length, uint8_t flags)
{
    uint16_t reg_end = reg;

    if (flags & (I2C_NOSTART | I2C_NOSTOP)) {
        return -EOPNOTSUPP;
    }

    /* Handle endianness of register if 16 bit */
    if (flags & I2C_REG16) {
        reg_end = htons(reg); /* Make sure register is in big-endian on I2C bus */
    }

    /* prepare transfer */
    I2C_TransferSeq_TypeDef transfer;

    transfer.addr = (address << 1);
    transfer.flags = I2C_FLAG_WRITE_WRITE | ((flags & I2C_ADDR10) ? I2C_FLAG_10BIT_ADDR : 0);
    transfer.buf[0].data = (uint8_t *) &reg_end;
    transfer.buf[0].len = (flags & I2C_REG16) ? 2 : 1;
    transfer.buf[1].data = (uint8_t *) data;
    transfer.buf[1].len = length;

    /* start a transfer */
    return _transfer(dev, &transfer);
}

#ifdef I2C_0_ISR
void I2C_0_ISR(void)
{
    i2c_progress[0] = I2C_Transfer(i2c_config[0].dev);
    cortexm_isr_end();
}
#endif

#ifdef I2C_1_ISR
void I2C_1_ISR(void)
{
    i2c_progress[1] = I2C_Transfer(i2c_config[1].dev);
    cortexm_isr_end();
}
#endif

#ifdef I2C_2_ISR
void I2C_2_ISR(void)
{
    i2c_progress[2] = I2C_Transfer(i2c_config[2].dev);
    cortexm_isr_end();
}
#endif
