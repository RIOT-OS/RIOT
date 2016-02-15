/*
 * Copyright (C) 2016 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32_common
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"

#include "periph_conf.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

/* emlib uses the same flags, undefine fist */
#undef I2C_FLAG_WRITE
#undef I2C_FLAG_READ

#include "em_cmu.h"
#include "em_i2c.h"

/* guard file in case no I2C device is defined */
#if I2C_NUMOF

volatile static I2C_TransferReturn_TypeDef i2c_progress[I2C_NUMOF];

static mutex_t i2c_lock[I2C_NUMOF] = {
#if I2C_0_EN
    [I2C_0] = MUTEX_INIT,
#endif
#if I2C_1_EN
    [I2C_1] = MUTEX_INIT,
#endif
};

/**
 * @brief Convert speeds to integers
 */
static uint32_t speed_to_freq(i2c_speed_t speed)
{
    uint32_t freq = 100000;

    switch (speed) {
        case I2C_SPEED_LOW:
            freq = 10000;
            break;
        case I2C_SPEED_NORMAL:
            freq = 100000;
            break;
        case I2C_SPEED_FAST:
            freq = 400000;
            break;
        case I2C_SPEED_FAST_PLUS:
            freq = 1000000;
            break;
        case I2C_SPEED_HIGH:
            freq = 3400000;
            break;
    }

    return freq;
}

/**
 * @brief   Start and track an I2C transfer.
 */
static void _transfer(i2c_t dev, I2C_TransferSeq_TypeDef *transfer)
{
    i2c_progress[dev] = I2C_TransferInit(i2c_config[dev].dev, transfer);

    while (i2c_progress[dev] == i2cTransferInProgress) {
        /* the transfer progresses via the interrupt handler */
        __WFI();
    }
}

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    /* check if device is valid */
    if (dev >= I2C_NUMOF) {
        return -1;
    }

    /* enable clocks */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(i2c_config[dev].cmu, true);

    /* configure the pins */
    gpio_init(i2c_config[dev].scl_pin, GPIO_DIR_BI, GPIO_NOPULL);
    gpio_init(i2c_config[dev].sda_pin, GPIO_DIR_BI, GPIO_NOPULL);

    /* ensure slave is in a known state, which it may not after a reset */
    for (int i = 0; i < 9; i++) {
        gpio_set(i2c_config[dev].scl_pin);
        gpio_clear(i2c_config[dev].scl_pin);
    }

    /* reset and initialize the peripheral */
    I2C_Init_TypeDef init = I2C_INIT_DEFAULT;

    init.enable = false;
    init.freq = speed_to_freq(speed);

    I2C_Reset(i2c_config[dev].dev);
    I2C_Init(i2c_config[dev].dev, &init);

    /* configure pin functions */
#ifdef _SILICON_LABS_32B_PLATFORM_1
    i2c_config[dev].dev->ROUTE = (i2c_config[dev].loc |
                                  I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN);
#else
    i2c_config[dev].dev->ROUTEPEN = I2C_ROUTEPEN_SDAPEN | I2C_ROUTEPEN_SCLPEN;
    i2c_config[dev].dev->ROUTELOC0 = i2c_config[dev].loc;
#endif

    /* enable interrupts */
    NVIC_ClearPendingIRQ(i2c_config[dev].irq);
    NVIC_EnableIRQ(i2c_config[dev].irq);

    /* enable peripheral */
    I2C_Enable(i2c_config[dev].dev, true);

    return 0;
}

int i2c_acquire(i2c_t dev)
{
    mutex_lock((mutex_t *) &i2c_lock[dev]);

    return 0;
}

int i2c_release(i2c_t dev)
{
    mutex_unlock((mutex_t *) &i2c_lock[dev]);

    return 0;
}

int i2c_read_byte(i2c_t dev, uint8_t address, char *data)
{
    return i2c_read_bytes(dev, address, data, 1);
}

int i2c_read_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    I2C_TransferSeq_TypeDef transfer;

    transfer.addr = address;
    transfer.flags = I2C_FLAG_READ;
    transfer.buf[0].data = (uint8_t *) data;
    transfer.buf[0].len = length;

    /* start a transfer */
    _transfer(dev, &transfer);

    if (i2c_progress[dev] != i2cTransferDone) {
        return -2;
    }

    return length;
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, char *data)
{
    return i2c_read_regs(dev, address, reg, data, 1);
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg,
                  char *data, int length)
{
    I2C_TransferSeq_TypeDef transfer;

    transfer.addr = address;
    transfer.flags = I2C_FLAG_WRITE_READ;
    transfer.buf[0].data = &reg;
    transfer.buf[0].len = 1;
    transfer.buf[1].data = (uint8_t *) data;
    transfer.buf[1].len = length;

    /* start a transfer */
    _transfer(dev, &transfer);

    if (i2c_progress[dev] != i2cTransferDone) {
        return -2;
    }

    return length;
}

int i2c_write_byte(i2c_t dev, uint8_t address, char data)
{
    return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    I2C_TransferSeq_TypeDef transfer;

    transfer.addr = address;
    transfer.flags = I2C_FLAG_WRITE;
    transfer.buf[0].data = (uint8_t *) data;
    transfer.buf[0].len = length;

    /* start a transfer */
    _transfer(dev, &transfer);

    if (i2c_progress[dev] != i2cTransferDone) {
        return -2;
    }

    return length;
}

int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, char data)
{
    return i2c_write_regs(dev, address, reg, &data, 1);
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg,
                   char *data, int length)
{
    I2C_TransferSeq_TypeDef transfer;

    transfer.addr = address;
    transfer.flags = I2C_FLAG_WRITE_WRITE;
    transfer.buf[0].data = &reg;
    transfer.buf[0].len = 1;
    transfer.buf[1].data = (uint8_t *) data;
    transfer.buf[1].len = length;

    /* start a transfer */
    _transfer(dev, &transfer);

    if (i2c_progress[dev] != i2cTransferDone) {
        return -2;
    }

    return length;
}

void i2c_poweron(i2c_t dev)
{
    CMU_ClockEnable(i2c_config[dev].cmu, true);
}

void i2c_poweroff(i2c_t dev)
{
    CMU_ClockEnable(i2c_config[dev].cmu, false);
}

#ifdef I2C_0_ISR
void I2C_0_ISR(void)
{
    i2c_progress[0] = I2C_Transfer(i2c_config[0].dev);
}
#endif

#ifdef I2C_1_ISR
void I2C_1_ISR(void)
{
    i2c_progress[1] = I2C_Transfer(i2c_config[1].dev);
}
#endif

#endif /* I2C_NUMOF */
