/*
 * SPDX-FileCopyrightText: 2018 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_cc26xx_cc13xx
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 * @note        This CPU has weak pullups, external pullup resistors may be
 *              required.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "mutex.h"

#include "cpu.h"
#include "periph/i2c.h"

#include "cc26xx_cc13xx_power.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define PREG(x) DEBUG("%s=0x%08x\n", #x, (unsigned)x);

/**
 * @brief Mutex lock for the only available I2C periph
 * @note  If multiple I2C devices are added locks must be an array for each one.
 */
static mutex_t _lock;

static int _check_errors(void)
{
    int ret = 0;

    /* The reference manual (SWCU117H) is ambiguous on how to wait:
     *
     * 1. 21.4 8. says "wait until BUSBUSY is cleared"
     * 2. command flow diagrams (e.g., 21.3.5.1) indicate to wait while
     *    BUSY is set
     *
     * (3. 21.5.1.10 says BUSY is only valid after 4 SYSBUS clock cycles)
     *
     * Waiting first for cleared IDLE and then for cleared BUSY works fine.
     */

    /* wait for transfer to be complete, this also could be a few nops... */
    while (I2C->MSTAT & MSTAT_IDLE) {}
    while (I2C->MSTAT & MSTAT_BUSY) {}
    /* check if there was an error */
    if (I2C->MSTAT & MSTAT_ERR) {
        DEBUG("%s\n", __FUNCTION__);
        PREG(I2C->MSTAT);
        ret = -ETIMEDOUT;
        if (I2C->MSTAT & MSTAT_ADRACK_N) {
            DEBUG("ADDRESS NACK\n");
            return -ENXIO;
        }
        else if (I2C->MSTAT & MSTAT_DATACK_N) {
            DEBUG("DATA NACK\n");
            ret = -EIO;
        }
        else if (I2C->MSTAT & MSTAT_ARBLST) {
            DEBUG("ARBITRATION LOSS\n");
            ret = -EAGAIN;
        }
        /*
         * If a non-NACK error occurs we must reinit or lock up.
         * dev = 0 since it is the only one, if more are added it should be
         * the dev num, this is done to avoid passing in arguments and
         * increasing code size.
         */
        i2c_init(0);
        return ret;
    }
    return ret;
}

void i2c_init(i2c_t devnum)
{
    (void)devnum;
    assert(devnum < I2C_NUMOF);

    /* Make sure everything is shut off in case of reinit */
    I2C->MCR = 0;

    /* Enable serial power domain */
    if (!power_is_domain_enabled(POWER_DOMAIN_SERIAL)) {
        power_enable_domain(POWER_DOMAIN_SERIAL);
    }

    /* enable I2C clock in run mode */
    power_clock_enable_i2c();

    /* configure pins */
    IOC->CFG[I2C_SDA_PIN] = (IOCFG_PORTID_I2C_MSSDA
                             | IOCFG_INPUT_ENABLE
                             | IOCFG_IOMODE_OPEN_DRAIN
                             | IOCFG_PULLCTL_UP);
    IOC->CFG[I2C_SCL_PIN] = (IOCFG_PORTID_I2C_MSSCL
                             | IOCFG_INPUT_ENABLE
                             | IOCFG_IOMODE_OPEN_DRAIN
                             | IOCFG_PULLCTL_UP);

    /* initialize I2C master */
    I2C->MCR = MCR_MFE;

    /* configure clock speed
     * {PERDMACLK / [2 × (SCL_LP + SCL_HP) × SCL_CLK]} – 1
     * with SCL_LP==6 && SCL_HP==4 use 0x17 for 100kHZ with 48MHZ CPU clock */
    I2C->MTPR = MTPR_TPR_100KHZ;
}

void i2c_acquire(i2c_t dev)
{
    (void)dev;
    assert(dev < I2C_NUMOF);
    mutex_lock(&_lock);
}

void i2c_release(i2c_t dev)
{
    (void)dev;
    assert(dev < I2C_NUMOF);
    mutex_unlock(&_lock);
}

int i2c_read_bytes(i2c_t dev, uint16_t addr,
                   void *data, size_t len, uint8_t flags)
{
    (void)dev;
    int ret = 0;
    char *bufpos = data;

    DEBUG("%s %u\n", __FUNCTION__, len);
    PREG(I2C->MSTAT);

    assert(dev < I2C_NUMOF);
    assert(data != NULL);

    /* Check for unsupported operations */
    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }

    /* Check for wrong arguments given */
    if (len == 0) {
        return -EINVAL;
    }
    if (!(I2C->MSTAT & MSTAT_BUSBSY) && (flags & I2C_NOSTART)) {
        return -EINVAL;
    }

    /* Sequence may be omitted in a single master system */
    while (I2C->MSTAT & MSTAT_BUSY) {}

    I2C->MSA = ((uint32_t)addr << 1) | MSA_RS;

    while (len--) {
        DEBUG("LOOP %u\n", len);
        /* setup transfer */
        uint32_t mctrl = MCTRL_RUN;
        if (!(flags & I2C_NOSTART)) {
            DEBUG("START\n");
            mctrl |= MCTRL_START;

            /* make note not to generate START from second byte onwards */
            flags |= I2C_NOSTART;
        }

        /* after last byte, generate STOP unless told not to */
        if (!len && !(flags & I2C_NOSTOP)) {
            DEBUG("STOP\n");
            mctrl |= MCTRL_STOP;
        }
        else {
            DEBUG("ACK\n");
            mctrl |= MCTRL_ACK;
        }

        while (I2C->MSTAT & MSTAT_BUSY) {}
        /* initiate transfer */
        I2C->MCTRL = mctrl;

        /* check if there was an error */
        ret = _check_errors();
        if (ret != 0) {
            return ret;
        }
        /* copy next byte from I2C data register */
        DEBUG("IN=0x%02x\n", (unsigned)I2C->MDR);
        *bufpos++ = I2C->MDR;
    }

    return ret;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len,
                    uint8_t flags)
{
    (void)dev;
    int ret = 0;
    const unsigned char *bufpos = data;

    DEBUG("%s %u\n", __FUNCTION__, len);
    PREG(I2C->MSTAT);

    assert(dev < I2C_NUMOF);
    assert(data != NULL);

    /* Check for unsupported operations */
    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }

    /* Check for wrong arguments given */
    if (len == 0) {
        return -EINVAL;
    }
    if (!(I2C->MSTAT & MSTAT_BUSBSY) && (flags & I2C_NOSTART)) {
        return -EINVAL;
    }

    /* Since write is 0 we just need shift the address in */
    I2C->MSA = (uint32_t)addr << 1;

    /* Sequence may be omitted in a single master system. */
    while (I2C->MSTAT & MSTAT_BUSY) {}

    while (len--) {
        DEBUG("LOOP %u 0x%2x\n", len, (unsigned)*bufpos);
        /* copy next byte into I2C data register */
        I2C->MDR = *bufpos++;

        /* setup transfer */
        uint32_t mctrl = MCTRL_RUN;
        if (!(flags & I2C_NOSTART)) {
            DEBUG("START\n");
            mctrl |= MCTRL_START;

            /* make note not to generate START from second byte onwards */
            flags |= I2C_NOSTART;
        }

        /* after last byte, generate STOP unless told not to */
        if (!len && !(flags & I2C_NOSTOP)) {
            DEBUG("STOP\n");
            mctrl |= MCTRL_STOP;
        }

        /* initiate transfer */
        I2C->MCTRL = mctrl;

        /* check if there was an error */
        ret = _check_errors();
        if (ret != 0) {
            return ret;
        }
    }

    return ret;
}
