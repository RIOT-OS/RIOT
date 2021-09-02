/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p_ng
 * @{
 *
 * @file
 * @brief     Functions to communicate with the NRF24L01+ (NG) transceiver
 *            via SPI
 *
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include <errno.h>
#include <assert.h>

#include "periph/gpio.h"
#include "periph/spi.h"
#include "nrf24l01p_ng_constants.h"
#include "nrf24l01p_ng_communication.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define SPI_BUS     (dev->params.spi)
#define SPI_PIN_CS  (dev->params.pin_cs)

#define NRF24L01P_NG_REG_MAX_WIDTH          NRF24L01P_NG_MAX_ADDR_WIDTH

/**
 * @brief Reverse @p len bytes in @p buffer
 *
 * The NRF24L01+ expects data to be send over SPI from LSByte to MSByte
 * and will output data bytes also from LSByte to MSByte!
 * See the datasheet page 47, 8.3.1 SPI comands.
 *
 * This shall be the address register of pipe 0 inside the transceiver:
 * [ x,  x,  x,  x,  x] (x = undefined content)
 *
 * Suppose you want to give pipe 0 the address 01:02:03:04.
 * If you send:
 * 01 ---> [ x,  x,  x,  x, 01]
 * 02 ---> [ x,  x,  x, 02, 01]
 * 03 ---> [ x,  x, 03, 02, 02]
 * 04 ---> [ x, 04, 03, 02, 01]
 *
 * The transceiver does not have the address you expected.
 * But over SPI you would read:
 * [ x, 04, 03, 02, 01] --->  01:02:03:04
 * because it sends the LSByte first. But it will not
 * receice frames that are actually addressed to 01:02:03:04.
 *
 * So if you want to assign the address 01:02:03:04
 * you first have to swap the bytes, before you send data over SPI.
 *
 */
static void _nrf24l01p_ng_swap_bytes(uint8_t* buffer, size_t len) {
    for (size_t i = 0; i < len / 2; i++) {
        uint8_t tmp = buffer[i];
        buffer[i] = buffer[len - 1 - i];
        buffer[len - 1 - i] = tmp;
    }
}
static void _nrf24l01p_ng_copy_and_swap_bytes(uint8_t* dst, const uint8_t* src, size_t len) {
    const uint8_t* end = dst + len;
    src += len;
    while (dst != end) {
        *dst++ = *--src;
    }
}

void nrf24l01p_ng_acquire(nrf24l01p_ng_t *dev)
{
    spi_acquire(dev->params.spi, dev->params.pin_cs, SPI_MODE_0, dev->params.spi_clk);
}

void nrf24l01p_ng_release(nrf24l01p_ng_t *dev)
{
    spi_release(dev->params.spi);
}

uint8_t nrf24l01p_ng_read_reg(const nrf24l01p_ng_t *dev, uint8_t reg,
                              uint8_t *dest, size_t len)
{
    uint8_t cmd = NRF24L01P_NG_CMD_R_REGISTER(reg);
    uint8_t status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
    spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, NULL, dest, len);
    if (len > 1) {
        _nrf24l01p_ng_swap_bytes(dest, len);
    }
    return status;
}

uint8_t nrf24l01p_ng_write_reg(const nrf24l01p_ng_t *dev, uint8_t reg,
                               const uint8_t *src, size_t len)
{
    assert(len <= NRF24L01P_NG_REG_MAX_WIDTH);
    uint8_t cmd = NRF24L01P_NG_CMD_W_REGISTER(reg);
    uint8_t status;
    if (len > 1) {
        uint8_t lsrc[NRF24L01P_NG_REG_MAX_WIDTH];
        _nrf24l01p_ng_copy_and_swap_bytes(lsrc, src, len);
        status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
        spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, lsrc, NULL, len);
    }
    else {
        status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
        spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, src, NULL, len);
    }
    return status;
}

uint8_t nrf24l01p_ng_read_rx_payload(const nrf24l01p_ng_t *dev, void *dest,
                                     size_t len)
{
    uint8_t cmd = NRF24L01P_NG_CMD_R_RX_PAYLOAD;
    uint8_t status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
    spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, &cmd, dest, len);
    if (len > 1) {
        _nrf24l01p_ng_swap_bytes(dest, len);
    }
    return status;
}

uint8_t nrf24l01p_ng_write_tx_payload(const nrf24l01p_ng_t *dev,
                                      const void *src, size_t len)
{
    assert(len <= NRF24L01P_NG_MAX_PAYLOAD_WIDTH);
    uint8_t cmd = NRF24L01P_NG_CMD_W_TX_PAYLOAD;
    uint8_t status;
    if (len > 1) {
        uint8_t lsrc[NRF24L01P_NG_MAX_PAYLOAD_WIDTH];
        _nrf24l01p_ng_copy_and_swap_bytes(lsrc, src, len);
        status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
        spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, lsrc, NULL, len);
    }
    else {
        status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
        spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, src, NULL, len);
    }
    return status;
}

uint8_t nrf24l01p_ng_flush_tx(const nrf24l01p_ng_t *dev)
{
    uint8_t cmd = NRF24L01P_NG_CMD_FLUSH_TX;
    return spi_transfer_byte(SPI_BUS, SPI_PIN_CS, false, cmd);
}

uint8_t nrf24l01p_ng_flush_rx(const nrf24l01p_ng_t *dev)
{
    uint8_t cmd = NRF24L01P_NG_CMD_FLUSH_RX;
    return spi_transfer_byte(SPI_BUS, SPI_PIN_CS, false, cmd);
}

uint8_t nrf24l01p_ng_reuse_tx_pl(const nrf24l01p_ng_t *dev)
{
    uint8_t cmd = NRF24L01P_NG_CMD_REUSE_TX_PL;
    return spi_transfer_byte(SPI_BUS, SPI_PIN_CS, false, cmd);
}

uint8_t nrf24l01p_ng_read_rx_pl_width(const nrf24l01p_ng_t *dev, uint8_t *dest)
{
    uint8_t cmd = NRF24L01P_NG_CMD_R_RX_PL_WID;
    uint8_t status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
    spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, NULL, dest, 1);
    return status;
}

uint8_t nrf24l01p_ng_write_ack_pl(const nrf24l01p_ng_t *dev, const void *src,
                                  size_t len, uint8_t pipe)
{
    assert(len <= NRF24L01P_NG_MAX_PAYLOAD_WIDTH);
    assert(pipe < NRF24L01P_NG_PX_NUM_OF);
    uint8_t cmd = NRF24L01P_NG_CMD_W_ACK_PAYLOAD(pipe);
    uint8_t status;
    if (len > 1) {
        uint8_t lsrc[NRF24L01P_NG_MAX_PAYLOAD_WIDTH];
        _nrf24l01p_ng_copy_and_swap_bytes(lsrc, src, len);
        status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
        spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, lsrc, NULL, len);
    }
    else {
        status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
        spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, src, NULL, len);
    }
    return status;
}

uint8_t nrf24l01p_ng_write_tx_pl_no_ack(const nrf24l01p_ng_t *dev,
                                        const void *src, size_t len)
{
    assert(len <= NRF24L01P_NG_MAX_PAYLOAD_WIDTH);
    uint8_t cmd = NRF24L01P_NG_CMD_W_TX_PAYLOAD_NO_ACK;
    uint8_t status;
    if (len > 1) {
        uint8_t lsrc[NRF24L01P_NG_MAX_PAYLOAD_WIDTH];
        _nrf24l01p_ng_copy_and_swap_bytes(lsrc, src, len);
        status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
        spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, lsrc, NULL, len);
    }
    else {
        status = spi_transfer_byte(SPI_BUS, SPI_PIN_CS, true, cmd);
        spi_transfer_bytes(SPI_BUS, SPI_PIN_CS, false, src, NULL, len);
    }
    return status;
}

uint8_t nrf24l01p_ng_get_status(const nrf24l01p_ng_t *dev)
{
    uint8_t cmd = NRF24L01P_NG_CMD_NOP;
    return spi_transfer_byte(SPI_BUS, SPI_PIN_CS, false, cmd);
}
