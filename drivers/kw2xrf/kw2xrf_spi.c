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
 * @}
 */
#include "kw2xrf.h"
#include "kw2xrf_reg.h"
#include "kw2xrf_spi.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "cpu_conf.h"
#include "irq.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define SPI_MODE                (SPI_MODE_0)
#define KW2XRF_IBUF_LENGTH      (9)

static uint8_t ibuf[KW2XRF_IBUF_LENGTH];

void kw2xrf_spi_transfer_head(kw2xrf_t *dev)
{
#if KW2XRF_SHARED_SPI
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
#endif
}

void kw2xrf_spi_transfer_tail(kw2xrf_t *dev)
{
#if KW2XRF_SHARED_SPI
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);
#endif
}

int kw2xrf_spi_init(kw2xrf_t *dev)
{
    int res;

#if KW2XRF_SHARED_SPI
    spi_acquire(dev->params.spi);
#endif
    res = spi_init_master(dev->params.spi, SPI_CONF_FIRST_RISING, dev->params.spi_speed);
#if KW2XRF_SHARED_SPI
    spi_release(dev->params.spi);

    gpio_init(dev->params.cs_pin, GPIO_OUT);
    gpio_set(dev->params.cs_pin);
#endif

    if (res < 0) {
        DEBUG("[kw2xrf]: error initializing SPI_%i device (code %i)\n",
              kw2xrf_spi, res);
        return -1;
    }

    return 0;
}

void kw2xrf_write_dreg(kw2xrf_t *dev, uint8_t addr, uint8_t value)
{
    kw2xrf_spi_transfer_head(dev);
    spi_transfer_reg(dev->params.spi, dev->params.cs_pin, addr, value);
    kw2xrf_spi_transfer_tail(dev);
    return;
}

uint8_t kw2xrf_read_dreg(kw2xrf_t *dev, uint8_t addr)
{
    uint8_t value;
    kw2xrf_spi_transfer_head(dev);
    value = spi_transfer_reg(dev->params.spi, dev->params.cs_pin,
                             (addr | MKW2XDRF_REG_READ), 0x0);
    kw2xrf_spi_transfer_tail(dev);
    return value;
}

size_t kw2xrf_write_dregs(kw2xrf_t *dev, uint8_t addr, uint8_t *buf, uint8_t length)
{
    kw2xrf_spi_transfer_head(dev);
    size_t i = spi_transfer_regs(dev->params.spi, addr, (char *)buf, NULL, length);
    kw2xrf_spi_transfer_tail(dev);
    return i;
}

size_t kw2xrf_read_dregs(kw2xrf_t *dev, uint8_t addr, uint8_t *buf, uint8_t length)
{
    kw2xrf_spi_transfer_head(dev);
    size_t i = spi_transfer_regs(dev->params.spi, (addr | MKW2XDRF_REG_READ),
                                 NULL, (char *)buf, length);
    kw2xrf_spi_transfer_tail(dev);
    return i;
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
    spi_transfer_regs(dev->params.spi, dev->params.cs_pin, MKW2XDM_IAR_INDEX,
                      ibuf, NULL, length + 1);
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
    spi_transfer_regs(dev->params.spi, dev->params.cs_pin,
                      (MKW2XDM_IAR_INDEX | MKW2XDRF_REG_READ),
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
    spi_transfer_regs(dev->params.spi, dev->params.cs_pin,
                      MKW2XDRF_BUF_WRITE, data, NULL, length);
    kw2xrf_spi_transfer_tail(dev);
}

void kw2xrf_read_fifo(kw2xrf_t *dev, uint8_t *data, uint8_t length)
{
    kw2xrf_spi_transfer_head(dev);
    spi_transfer_regs(dev->params.spi, dev->params.cs_pin,
                      MKW2XDRF_BUF_READ, NULL, data, length);
    kw2xrf_spi_transfer_tail(dev);
}
