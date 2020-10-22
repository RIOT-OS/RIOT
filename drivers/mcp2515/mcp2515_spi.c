/*
 * Copyright (C) 2016 OTA keys
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp2515
 * @{
 *
 * @file
 * @brief       MCP2515 can spi driver
 *
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @}
 */

#include <stdio.h>

#include "mcp2515_spi.h"
#include "mcp2515_defines.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#include "xtimer.h"
#include "irq.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int mcp2515_spi_init(const candev_mcp2515_t *dev)
{
    int res;

    /* Configure SPI */
    res = spi_init_cs(dev->conf->spi, dev->conf->cs_pin);
    if (res != SPI_OK) {
        DEBUG("spi_init_master: error initializing SPI_%i device (code %i)\n",
              dev->conf->spi, res);
        return -1;
    }
    return 0;
}

int mcp2515_spi_reset(const candev_mcp2515_t *dev)
{
    spi_acquire(dev->conf->spi, dev->conf->cs_pin, dev->conf->spi_mode,
                dev->conf->spi_clk);
    spi_transfer_byte(dev->conf->spi, dev->conf->cs_pin, false,
                      MCP2515_SPI_RESET);
    spi_release(dev->conf->spi);
    return 0;
}

int mcp2515_spi_read(const candev_mcp2515_t *dev, uint8_t addr, uint8_t *buf,
                     unsigned int len)
{
    spi_acquire(dev->conf->spi, dev->conf->cs_pin, dev->conf->spi_mode,
                dev->conf->spi_clk);
    spi_transfer_byte(dev->conf->spi, dev->conf->cs_pin, true,
                      MCP2515_SPI_READ);
    spi_transfer_regs(dev->conf->spi, dev->conf->cs_pin, addr, NULL,
                      (void *)buf, len);
    spi_release(dev->conf->spi);
    return 0;
}

int mcp2515_spi_read_rxbuf(const candev_mcp2515_t *dev, uint8_t mailbox,
                           void *buf, uint8_t len)
{
    /* See TABLE 12-1:SPI INSTRUCTION SET in mcp2515 datasheet */
    spi_acquire(dev->conf->spi, dev->conf->cs_pin, dev->conf->spi_mode,
                dev->conf->spi_clk);
    spi_transfer_byte(dev->conf->spi, dev->conf->cs_pin, true,
                      MCP2515_SPI_READ_RXBUF | (mailbox << 2));
    spi_transfer_bytes(dev->conf->spi, dev->conf->cs_pin, false, NULL,
                       (void *)buf, len);
    spi_release(dev->conf->spi);
    return 0;
}

int mcp2515_spi_write(const candev_mcp2515_t *dev, uint8_t addr, uint8_t *buf,
                      unsigned int len)
{
    spi_acquire(dev->conf->spi, dev->conf->cs_pin, dev->conf->spi_mode,
                dev->conf->spi_clk);
    spi_transfer_byte(dev->conf->spi, dev->conf->cs_pin, true,
                      MCP2515_SPI_WRITE);
    spi_transfer_regs(dev->conf->spi, dev->conf->cs_pin, addr, (void *)buf,
                      NULL, len);
    spi_release(dev->conf->spi);
    return 0;
}

int mcp2515_spi_write_txbuf(const candev_mcp2515_t *dev, uint8_t mailbox,
                            void *buf, uint8_t len)
{
    spi_acquire(dev->conf->spi, dev->conf->cs_pin, dev->conf->spi_mode,
                dev->conf->spi_clk);
    spi_transfer_byte(dev->conf->spi, dev->conf->cs_pin, true,
                      MCP2515_SPI_LOAD_TXBUF | (mailbox << 1));
    spi_transfer_bytes(dev->conf->spi, dev->conf->cs_pin, false, (void *)buf,
                       NULL, len);
    spi_release(dev->conf->spi);
    return 0;
}

int mcp2515_spi_rts(const candev_mcp2515_t *dev, uint8_t mailbox)
{
    spi_acquire(dev->conf->spi, dev->conf->cs_pin, dev->conf->spi_mode,
                dev->conf->spi_clk);
    spi_transfer_byte(dev->conf->spi, dev->conf->cs_pin, false,
                      MCP2515_SPI_RTS | (1 << mailbox));
    spi_release(dev->conf->spi);
    return 0;
}

uint8_t mcp2515_spi_read_status(const candev_mcp2515_t *dev)
{
    uint8_t status;

    spi_acquire(dev->conf->spi, dev->conf->cs_pin, dev->conf->spi_mode,
                dev->conf->spi_clk);
    spi_transfer_byte(dev->conf->spi, dev->conf->cs_pin, true,
                      MCP2515_SPI_READ_STATUS);
    status = spi_transfer_byte(dev->conf->spi, dev->conf->cs_pin, false, 0);
    spi_release(dev->conf->spi);
    return status;
}

int mcp2515_spi_rx_status(const candev_mcp2515_t *dev)
{
    uint8_t status;

    spi_acquire(dev->conf->spi, dev->conf->cs_pin, dev->conf->spi_mode,
                dev->conf->spi_clk);
    spi_transfer_byte(dev->conf->spi, dev->conf->cs_pin, true,
                      MCP2515_SPI_RX_STATUS);
    status = spi_transfer_byte(dev->conf->spi, dev->conf->cs_pin, false, 0);
    spi_release(dev->conf->spi);
    return status;
}

int mcp2515_spi_bitmod(const candev_mcp2515_t *dev, uint8_t addr, uint8_t mask,
                       uint8_t buf)
{
    uint8_t msg[2];

    spi_acquire(dev->conf->spi, dev->conf->cs_pin, dev->conf->spi_mode,
                dev->conf->spi_clk);
    spi_transfer_byte(dev->conf->spi, dev->conf->cs_pin, true,
                      MCP2515_SPI_BITMOD);

    msg[0] = mask;
    msg[1] = buf;

    spi_transfer_regs(dev->conf->spi, dev->conf->cs_pin, addr,
                      (const void *)msg, NULL, sizeof(msg));
    spi_release(dev->conf->spi);
    return 0;
}
