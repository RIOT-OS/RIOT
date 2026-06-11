/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_nrf54l
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C (TWI) driver implementation for the nRF54L
 *              family, based on the TWIM peripheral
 *
 * Compared to the older nRF families, the TWIM peripheral of the nRF54L
 * moved the EasyDMA related tasks and buffer registers into dedicated DMA
 * register groups, hence the dedicated driver implementation.
 *
 * As with the other nRF TWIM implementations, a read following a read (or a
 * write following a write) without a (repeated) start condition is not
 * supported by the hardware, so any operations with I2C_NOSTART fail.
 * Reads with I2C_NOSTOP are not supported either (see the nRF52 driver for
 * the details).
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "byteorder.h"
#include "cpu.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/**
 * @brief   If any of the 8 lower bits are set, the speed value is invalid
 */
#define INVALID_SPEED_MASK  (0xff)

/**
 * @brief   Allocate a tx buffer
 */
static uint8_t tx_buf[256];

/**
 * @brief   Mutex for locking the TX buffer
 */
static mutex_t buffer_lock;

/**
 * @brief   Initialized dev locks
 */
static mutex_t locks[I2C_NUMOF];

/**
 * @brief   array with a busy mutex for each I2C device, used to block the
 *          thread until the transfer is done
 */
static mutex_t busy[I2C_NUMOF];

/* use an enum to count the number of I2C ISR macro names defined by the
 * board */
enum {
#ifdef I2C_0_ISR
    I2C_0_ISR_NUM,
#endif
#ifdef I2C_1_ISR
    I2C_1_ISR_NUM,
#endif
    I2C_ISR_NUMOF,
};

static inline NRF_TWIM_Type *bus(i2c_t dev)
{
    return i2c_config[dev].dev;
}

static IRQn_Type _irqn(i2c_t dev)
{
    if (bus(dev) == NRF_TWIM20) {
        return SERIAL20_IRQn;
    }
    else if (bus(dev) == NRF_TWIM21) {
        return SERIAL21_IRQn;
    }
    else if (bus(dev) == NRF_TWIM22) {
        return SERIAL22_IRQn;
    }
    else {
        assert(bus(dev) == NRF_TWIM30);
        return SERIAL30_IRQn;
    }
}

/**
 * @brief   Like i2c_write_bytes, but with the constraint (created by the
 *          hardware) that data is in RAM
 */
static int direct_i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data,
                                  size_t len, uint8_t flags);

/**
 * Block until the interrupt described by inten_success_flag or
 * TWIM_INTEN_ERROR_Msk fires.
 *
 * Allowed values for inten_success_flag are
 * * TWIM_INTEN_STOPPED_Msk (when a stop condition is to be set and the short
 *   circuit will pull TWIM into the stopped condition)
 * * TWIM_INTEN_LASTTX_Msk (when sending without a stop condition)
 */
static int finish(i2c_t dev, int inten_success_flag)
{
    DEBUG("[i2c] waiting for success (STOPPED/LASTTX) or ERROR event\n");
    /* Unmask interrupts */
    bus(dev)->INTENSET = inten_success_flag | TWIM_INTEN_ERROR_Msk;
    mutex_lock(&busy[dev]);

    if ((bus(dev)->EVENTS_STOPPED)) {
        bus(dev)->EVENTS_STOPPED = 0;
        DEBUG("[i2c] finish: stop event occurred\n");
    }

    if (inten_success_flag & TWIM_INTEN_LASTTX_Msk) {
        /* The interrupt is raised already when the last TX is started, but we
         * have to wait until it was actually transmitted lest the
         * transmission would be suppressed immediately by the next following
         * write */
        while (bus(dev)->DMA.TX.AMOUNT != bus(dev)->DMA.TX.MAXCNT &&
                !bus(dev)->EVENTS_ERROR) {}
    }

    if (bus(dev)->EVENTS_ERROR) {
        bus(dev)->EVENTS_ERROR = 0;
        if (bus(dev)->ERRORSRC & TWIM_ERRORSRC_ANACK_Msk) {
            bus(dev)->ERRORSRC = TWIM_ERRORSRC_ANACK_Msk;
            DEBUG("[i2c] check_error: NACK on address byte\n");
            return -ENXIO;
        }
        if (bus(dev)->ERRORSRC & TWIM_ERRORSRC_DNACK_Msk) {
            bus(dev)->ERRORSRC = TWIM_ERRORSRC_DNACK_Msk;
            DEBUG("[i2c] check_error: NACK on data byte\n");
            return -EIO;
        }
    }

    return 0;
}

static void _init_pins(i2c_t dev)
{
    gpio_init(i2c_config[dev].scl, GPIO_IN_OD_PU);
    gpio_init(i2c_config[dev].sda, GPIO_IN_OD_PU);
}

void i2c_init(i2c_t dev)
{
    /* ensure the ISR names have been defined as needed */
    static_assert(I2C_NUMOF == I2C_ISR_NUMOF, "Define(s) of I2C ISR name(s) missing");

    assert(dev < I2C_NUMOF);

    /* Initialize mutex */
    mutex_init(&busy[dev]);
    mutex_lock(&busy[dev]);

    /* disable device during initialization, will be enabled when acquire is
     * called */
    bus(dev)->ENABLE = TWIM_ENABLE_ENABLE_Disabled;

    /* configure pins */
    _init_pins(dev);

    /* configure dedicated pins and speed */
    bus(dev)->PSEL.SCL = i2c_config[dev].scl;
    bus(dev)->PSEL.SDA = i2c_config[dev].sda;
    bus(dev)->FREQUENCY = i2c_config[dev].speed;

    NVIC_EnableIRQ(_irqn(dev));

    bus(dev)->ENABLE = TWIM_ENABLE_ENABLE_Enabled;
}

void i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_lock(&locks[dev]);

    bus(dev)->ENABLE = TWIM_ENABLE_ENABLE_Enabled;

    DEBUG("[i2c] acquired dev %i\n", (int)dev);
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    bus(dev)->ENABLE = TWIM_ENABLE_ENABLE_Disabled;

    mutex_unlock(&locks[dev]);

    DEBUG("[i2c] released dev %i\n", (int)dev);
}

int i2c_write_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                   const void *data, size_t len, uint8_t flags)
{
    assert((dev < I2C_NUMOF) && data && (len > 0) && (len < 253));

    if (flags & (I2C_NOSTART | I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }

    /* the TWI device does not support to do two consecutive transfers
     * without a repeated start condition in between. So we have to put all
     * data to be transferred into a buffer (tx_buf). */

    uint8_t reg_addr_len; /* Length in bytes of the register address */

    /* Lock tx_buf */
    mutex_lock(&buffer_lock);

    if (flags & (I2C_REG16)) {
        reg_addr_len = 2;
        /* Prepare the 16-bit register transfer  */
        tx_buf[0] = reg >> 8; /* AddrH in the first byte  */
        tx_buf[1] = reg & 0xFF; /* AddrL in the second byte  */
    }
    else {
        reg_addr_len = 1;
        tx_buf[0] = reg;
    }

    memcpy(&tx_buf[reg_addr_len], data, len);
    int ret = direct_i2c_write_bytes(dev, addr, tx_buf, reg_addr_len + len, flags);

    /* Release tx_buf */
    mutex_unlock(&buffer_lock);

    return ret;
}

int i2c_read_bytes(i2c_t dev, uint16_t addr, void *data, size_t len,
                   uint8_t flags)
{
    assert((dev < I2C_NUMOF) && data && (len > 0) && (len < 256));

    if (flags & (I2C_NOSTART | I2C_ADDR10 | I2C_NOSTOP)) {
        return -EOPNOTSUPP;
    }
    DEBUG("[i2c] read_bytes: %i bytes from addr 0x%02x\n", (int)len, (int)addr);

    bus(dev)->ADDRESS = addr;
    bus(dev)->DMA.RX.PTR = (uint32_t)data;
    bus(dev)->DMA.RX.MAXCNT = (uint8_t)len;

    bus(dev)->SHORTS = TWIM_SHORTS_LASTRX_STOP_Msk;

    /* Start reception */
    bus(dev)->TASKS_DMA.RX.START = 1;

    return finish(dev, TWIM_INTEN_STOPPED_Msk);
}

int i2c_read_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                  void *data, size_t len, uint8_t flags)
{
    assert((dev < I2C_NUMOF) && data && (len > 0) && (len < 256));

    if (flags & (I2C_NOSTART | I2C_ADDR10 | I2C_NOSTOP)) {
        return -EOPNOTSUPP;
    }

    DEBUG("[i2c] read_regs: %i byte(s) from reg 0x%02x at addr 0x%02x\n",
           (int)len, (int)reg, (int)addr);

    /* Prepare transfer */
    bus(dev)->ADDRESS = addr;
    if (flags & (I2C_REG16)) {
        /* Register endianness for 16 bit */
        reg = htons(reg);
        bus(dev)->DMA.TX.MAXCNT = 2;
    }
    else {
        bus(dev)->DMA.TX.MAXCNT = 1;
    }
    bus(dev)->DMA.TX.PTR = (uint32_t)&reg;
    bus(dev)->DMA.RX.PTR = (uint32_t)data;
    bus(dev)->DMA.RX.MAXCNT = (uint8_t)len;

    bus(dev)->SHORTS = TWIM_SHORTS_LASTTX_DMA_RX_START_Msk |
                       TWIM_SHORTS_LASTRX_STOP_Msk;

    /* Start transfer */
    bus(dev)->TASKS_DMA.TX.START = 1;

    return finish(dev, TWIM_INTEN_STOPPED_Msk);
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len,
                    uint8_t flags)
{
    /* EasyDMA can only transfer data from RAM */
    if ((uint32_t)data & RAM_MASK) {
        return direct_i2c_write_bytes(dev, addr, data, len, flags);
    }

    /* These are critical for the memcpy; direct_i2c_write_bytes makes some
     * more */
    assert((len > 0) && (len < 256));

    /* Lock tx_buf */
    mutex_lock(&buffer_lock);

    memcpy(tx_buf, data, len);

    int result = direct_i2c_write_bytes(dev, addr, tx_buf, len, flags);

    /* Release tx_buf */
    mutex_unlock(&buffer_lock);

    return result;
}

static int direct_i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data,
                                  size_t len, uint8_t flags)
{
    assert((dev < I2C_NUMOF) && data && (len > 0) && (len < 256));

    if (flags & (I2C_NOSTART | I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }
    DEBUG("[i2c] write_bytes: %i byte(s) to addr 0x%02x\n", (int)len, (int)addr);

    bus(dev)->ADDRESS = addr;
    bus(dev)->DMA.TX.PTR = (uint32_t)data;
    bus(dev)->DMA.TX.MAXCNT = (uint8_t)len;
    int inten_success_flag;
    if (!(flags & I2C_NOSTOP)) {
        bus(dev)->SHORTS = TWIM_SHORTS_LASTTX_STOP_Msk;
        inten_success_flag = TWIM_INTEN_STOPPED_Msk;
    }
    else {
        bus(dev)->SHORTS = 0;
        inten_success_flag = TWIM_INTEN_LASTTX_Msk;
    }
    bus(dev)->TASKS_DMA.TX.START = 1;

    return finish(dev, inten_success_flag);
}

static void irq_handler(i2c_t dev)
{
    /* Mask interrupts to ensure that they only trigger once */
    bus(dev)->INTENCLR = TWIM_INTEN_STOPPED_Msk | TWIM_INTEN_ERROR_Msk |
                         TWIM_INTEN_LASTTX_Msk;

    mutex_unlock(&busy[dev]);

    cortexm_isr_end();
}

#ifdef I2C_0_ISR
void I2C_0_ISR(void)
{
    irq_handler(I2C_DEV(0));
}
#endif

#ifdef I2C_1_ISR
void I2C_1_ISR(void)
{
    irq_handler(I2C_DEV(1));
}
#endif
