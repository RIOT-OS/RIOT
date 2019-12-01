/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p
 * @{
 *
 * @file
 * @brief     Functions to communicate with the NRF24L01P transceiver via SPI
 *
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include <errno.h>
#include <string.h>

#include "periph/gpio.h"
#include "periph/spi.h"
#include "xtimer.h"
#include "nrf24l01p_constants.h"
#include "nrf24l01p_communication.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define SPI_BUS     (dev->params.spi)
#define SPI_PIN_CS  (dev->params.pin_cs)
#define DUMMY       NULL

#define NRF24L01P_REG_MAX_WIDTH NRF24L01P_MAX_ADDR_WIDTH

int nrf24l01p_acquire(nrf24l01p_t *dev)
{
    DEBUG("[nrf24l01p] acquire ...\n");
    int a = spi_acquire(dev->params.spi, dev->params.pin_cs, SPI_MODE_0,
                        dev->params.spi_clk);
#ifndef NDEBUG
    dev->have_spi_access = 1;
#endif
    DEBUG("[nrf24l01p] acquired!\n");
    return a;
}

void nrf24l01p_release(nrf24l01p_t *dev)
{
    spi_release(dev->params.spi);
#ifndef NDEBUG
    dev->have_spi_access = 0;
#endif
    DEBUG("[nrf24l01p] released\n");
}

uint8_t nrf24l01p_read_reg(const nrf24l01p_t *dev, uint8_t reg,
                           uint8_t *dest, size_t len)
{
    uint8_t cmd = NRF24L01P_CMD_R_REGISTER(reg);
    uint8_t status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
    spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, DUMMY, dest, len);
    return status;
}

uint8_t nrf24l01p_write_reg(const nrf24l01p_t *dev, uint8_t reg,
                            const uint8_t *src, size_t len)
{
    uint8_t cmd = NRF24L01P_CMD_W_REGISTER(reg);
    len = MIN(len, NRF24L01P_REG_MAX_WIDTH);
    uint8_t status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
    spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, src, DUMMY, len);
    return status;
}

uint8_t nrf24l01p_read_rx_payload(const nrf24l01p_t *dev, void *dest,
                                  size_t len)
{
    uint8_t cmd = NRF24L01P_CMD_R_RX_PAYLOAD;
    uint8_t status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
    spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, &cmd, dest, len);
    return status;
}

uint8_t nrf24l01p_write_tx_payload(const nrf24l01p_t *dev, const void *src,
                                   size_t len)
{
    uint8_t cmd = NRF24L01P_CMD_W_TX_PAYLOAD;
    len = MIN(len, NRF24L01P_MAX_PAYLOAD_WIDTH);
    uint8_t status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
    spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, src, DUMMY, len);
    return status;
}

uint8_t nrf24l01p_flush_tx(const nrf24l01p_t *dev)
{
    uint8_t cmd = NRF24L01P_CMD_FLUSH_TX;
    return spi_transfer_byte(SPI_BUS, SPI_PIN_CS, false, cmd);
}

uint8_t nrf24l01p_flush_rx(const nrf24l01p_t *dev)
{
    uint8_t cmd = NRF24L01P_CMD_FLUSH_RX;
    return spi_transfer_byte(SPI_BUS, SPI_PIN_CS, false, cmd);
}

uint8_t nrf24l01p_reuse_tx_pl(const nrf24l01p_t *dev)
{
    uint8_t cmd = NRF24L01P_CMD_REUSE_TX_PL;
    return spi_transfer_byte(SPI_BUS, SPI_PIN_CS, false, cmd);
}

uint8_t nrf24l01p_read_rx_pl_width(const nrf24l01p_t *dev, uint8_t *dest)
{
    uint8_t cmd = NRF24L01P_CMD_R_RX_PL_WID;
    uint8_t status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
    spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, DUMMY, dest, 1);
    return status;
}

uint8_t nrf24l01p_write_ack_pl(const nrf24l01p_t *dev, const void *src,
                               size_t len, uint8_t pipe)
{
    uint8_t cmd = NRF24L01P_CMD_W_ACK_PAYLOAD(pipe);
    len = MIN(len, NRF24L01P_MAX_PAYLOAD_WIDTH);
    uint8_t status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
    spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, src, DUMMY, len);
    return status;
}

uint8_t nrf24l01p_write_tx_pl_no_ack(const nrf24l01p_t *dev, const void *src,
                                     size_t len)
{
    uint8_t cmd = NRF24L01P_CMD_W_TX_PAYLOAD_NO_ACK;
    len = MIN(len, NRF24L01P_MAX_PAYLOAD_WIDTH);
    uint8_t status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
    spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, src, DUMMY, len);
    return status;
}

uint8_t nrf24l01p_get_status(const nrf24l01p_t *dev)
{
    uint8_t cmd = NRF24L01P_CMD_NOP;
    return spi_transfer_byte(SPI_BUS, SPI_PIN_CS, false, cmd);
}
