/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_kw2xrf
 * @{
 * @file
 * @brief       Implementation of SPI-functions for the kw2xrf driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include "log.h"
#include "kw2xrf.h"
#include "kw2xrf_reg.h"
#include "kw2xrf_spi.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "cpu_conf.h"
#include "irq.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define SPIDEV              (dev->params.spi)
#define SPICLK              (dev->params.spi_clk)
#define CSPIN               (dev->params.cs_pin)
#define SPIMODE             (SPI_MODE_0)

#define KW2XRF_IBUF_LENGTH  (9)

static uint8_t ibuf[KW2XRF_IBUF_LENGTH];

void kw2xrf_spi_transfer_head(kw2xrf_t *dev)
{
    spi_acquire(SPIDEV, CSPIN, SPIMODE, SPICLK);
#if KW2XRF_SHARED_SPI
    gpio_clear(CSPIN);
#endif
}

void kw2xrf_spi_transfer_tail(kw2xrf_t *dev)
{
#if KW2XRF_SHARED_SPI
    gpio_set(CSPIN);
#endif
    spi_release(SPIDEV);
}

int kw2xrf_spi_init(kw2xrf_t *dev)
{
    DEBUG("[kw2xrf_spi] kw2xrf_spi_init\n");
    int res;
#if KW2XRF_SHARED_SPI
    spi_acquire(SPIDEV, CSPIN, SPIMODE, SPICLK);
#endif
    res = spi_init_cs(SPIDEV, CSPIN);
#if KW2XRF_SHARED_SPI
    spi_release(SPIDEV);
    gpio_init(CSPIN, GPIO_OUT);
    gpio_set(CSPIN);
#endif

    if (res != SPI_OK) {
        LOG_ERROR("[kw2xrf_spi] failed to init SPI_%u device (code %i)\n",
                  (unsigned)SPIDEV, res);
        return 1;
    }
    /* verify SPI params */
    res = spi_acquire(SPIDEV, CSPIN, SPIMODE, SPICLK);
    if (res == SPI_NOMODE) {
        LOG_ERROR("[kw2xrf_spi] given SPI mode is not supported");
        return 1;
    }
    else if (res == SPI_NOCLK) {
        LOG_ERROR("[kw2xrf_spi] targeted clock speed is not supported");
        return 1;
    }
    else if (res != SPI_OK) {
        LOG_ERROR("[kw2xrf_spi] unable to acquire bus with given parameters");
        return 1;
    }
    spi_release(SPIDEV);

    DEBUG("[kw2xrf_spi] SPI_DEV(%u) initialized: mode: %u, clk: %u, cs_pin: %u\n",
          (unsigned)SPIDEV, (unsigned)SPIMODE, (unsigned)SPICLK, (unsigned)CSPIN);
    return 0;
}

void kw2xrf_write_dreg(kw2xrf_t *dev, uint8_t addr, uint8_t value)
{
    DEBUG("[kw2xrf_spi] kw2xrf_write_dreg, addr %u, value %u\n", addr, value);
    kw2xrf_spi_transfer_head(dev);
    spi_transfer_reg(SPIDEV, CSPIN, addr, value);
    kw2xrf_spi_transfer_tail(dev);
    return;
}

uint8_t kw2xrf_read_dreg(kw2xrf_t *dev, uint8_t addr)
{
    uint8_t value;
    kw2xrf_spi_transfer_head(dev);
    value = spi_transfer_reg(SPIDEV, CSPIN, (addr | MKW2XDRF_REG_READ), 0x0);
    kw2xrf_spi_transfer_tail(dev);
    DEBUG("[kw2xrf_spi] kw2xrf_read_dreg, addr %u, value %u\n", addr, value);
    return value;
}

void kw2xrf_write_dregs(kw2xrf_t *dev, uint8_t addr, uint8_t *buf, uint8_t length)
{
    DEBUG("[kw2xrf_spi] kw2xrf_write_dregs, addr %u, length %u\n", addr, length);
    kw2xrf_spi_transfer_head(dev);
    spi_transfer_regs(SPIDEV, CSPIN, addr, buf, NULL, length);
    kw2xrf_spi_transfer_tail(dev);
}

void kw2xrf_read_dregs(kw2xrf_t *dev, uint8_t addr, uint8_t *buf, uint8_t length)
{
    kw2xrf_spi_transfer_head(dev);
    spi_transfer_regs(SPIDEV, CSPIN, (addr | MKW2XDRF_REG_READ), NULL, buf, length);
    DEBUG("[kw2xrf_spi] kw2xrf_read_dregs, addr %u, length %u\n", addr, length);
    kw2xrf_spi_transfer_tail(dev);
}


void kw2xrf_write_iregs(kw2xrf_t *dev, uint8_t addr, uint8_t *buf, uint8_t length)
{
    if (length > (KW2XRF_IBUF_LENGTH - 1)) {
        length = KW2XRF_IBUF_LENGTH - 1;
    }

    ibuf[0] = addr;

    for (uint8_t i = 0; i < length; i++) {
        ibuf[i + 1] = buf[i];
    }

    kw2xrf_spi_transfer_head(dev);
    spi_transfer_regs(SPIDEV, CSPIN, MKW2XDM_IAR_INDEX, ibuf, NULL, length + 1);
    kw2xrf_spi_transfer_tail(dev);

    return;
}

void kw2xrf_read_iregs(kw2xrf_t *dev, uint8_t addr, uint8_t *buf, uint8_t length)
{
    if (length > (KW2XRF_IBUF_LENGTH - 1)) {
        length = KW2XRF_IBUF_LENGTH - 1;
    }

    ibuf[0] = addr;

    kw2xrf_spi_transfer_head(dev);
    spi_transfer_regs(SPIDEV, CSPIN, (MKW2XDM_IAR_INDEX | MKW2XDRF_REG_READ),
                      ibuf, ibuf, length + 1);
    kw2xrf_spi_transfer_tail(dev);

    for (uint8_t i = 0; i < length; i++) {
        buf[i] = ibuf[i + 1];
    }

    return;
}

void kw2xrf_write_fifo(kw2xrf_t *dev, uint8_t *data, uint8_t length)
{
    kw2xrf_spi_transfer_head(dev);
    spi_transfer_regs(SPIDEV, CSPIN, MKW2XDRF_BUF_WRITE, data, NULL, length);
    kw2xrf_spi_transfer_tail(dev);
}

void kw2xrf_read_fifo(kw2xrf_t *dev, uint8_t *data, uint8_t length)
{
    kw2xrf_spi_transfer_head(dev);
    spi_transfer_regs(SPIDEV, CSPIN, MKW2XDRF_BUF_READ, NULL, data, length);
    kw2xrf_spi_transfer_tail(dev);
}
