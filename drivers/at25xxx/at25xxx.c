/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at25xxx
 * @{
 *
 * @file
 * @brief       Driver for the AT25xxx family of SPI-EEPROMs.
 *              This also includes M95xxx, 25AAxxx, 25LCxxx,
 *              CAT25xxx & BR25Sxxx.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include "at25xxx.h"
#include "at25xxx_constants.h"
#include "at25xxx_params.h"
#include "bitarithm.h"
#include "byteorder.h"

#include "ztimer.h"
#define POLL_DELAY_US   (1000)

#ifndef min
#define min(a, b) ((a) > (b) ? (b) : (a))
#endif

#define AT25_PAGE_SIZE   (dev->params.page_size)
#define ADDR_LEN    (AT25XXX_PARAM_ADDR_LEN)
#define ADDR_MSK    ((1UL << ADDR_LEN) - 1)

#ifndef AT25XXXX_SET_BUF_SIZE
/**
 * @brief  Adjust to configure buffer size
 */
#define AT225XXXX_SET_BUF_SIZE      (64)
#endif

static inline void getbus(const at25xxx_t *dev)
{
    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0, dev->params.spi_clk);
}

static inline uint32_t _pos(uint8_t cmd, uint32_t pos)
{
    /* first byte is CMD, then addr with MSB first */
    pos = htonl((pos & ADDR_MSK) | ((uint32_t)cmd << ADDR_LEN));
    pos >>= 8 * sizeof(pos) - (ADDR_LEN + 8);
    return pos;
}

static inline bool _write_in_progress(const at25xxx_t *dev)
{
    return spi_transfer_reg(dev->params.spi, dev->params.cs_pin, CMD_RDSR, 0) & SR_WIP;
}

static inline bool _write_enabled(const at25xxx_t *dev)
{
    return spi_transfer_reg(dev->params.spi, dev->params.cs_pin, CMD_RDSR, 0) & SR_WEL;
}

static inline int _wait_until_eeprom_ready(const at25xxx_t *dev)
{
    uint8_t tries = 10;
    while (_write_in_progress(dev) && --tries) {
        spi_release(dev->params.spi);
        ztimer_sleep(ZTIMER_USEC, POLL_DELAY_US);
        getbus(dev);
    }

    return tries == 0 ? -ETIMEDOUT : 0;
}

static int _at25xxx_write_page(const at25xxx_t *dev, uint32_t page, uint32_t offset, const void *data, size_t len)
{
    assert(offset < AT25_PAGE_SIZE);

    /* write no more than to the end of the current page to prevent wrap-around */
    size_t remaining = AT25_PAGE_SIZE - offset;
    len = min(len, remaining);
    uint32_t pos = _pos(CMD_WRITE, page * AT25_PAGE_SIZE + offset);

    /* wait for previous write to finish - may take up to 5 ms */
    int res = _wait_until_eeprom_ready(dev);
    if (res) {
        return res;
    }

    /* set write enable and wait for status change */
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, false, CMD_WREN);

    unsigned tries = 1000;
    while (!_write_enabled(dev) && --tries) {}

    if (tries == 0) {
        return -ETIMEDOUT;
    }

    /* write the data */
    spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, true, &pos, NULL, 1 + ADDR_LEN / 8);
    spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, false, data, NULL, len);

    return len;
}

int at25xxx_write_page(const at25xxx_t *dev, uint32_t page, uint32_t offset, const void *data, size_t len)
{
    int res;

    getbus(dev);
    res = _at25xxx_write_page(dev, page, offset, data, len);
    spi_release(dev->params.spi);

    return res;
}

int at25xxx_write(const at25xxx_t *dev, uint32_t pos, const void *data, size_t len)
{
    int res = 0;
    const uint8_t *d = data;

    if (pos + len > dev->params.size) {
        return -ERANGE;
    }

    /* page size is always a power of two */
    const uint32_t page_shift = bitarithm_msb(AT25_PAGE_SIZE);
    const uint32_t page_mask = AT25_PAGE_SIZE - 1;

    uint32_t page   = pos >> page_shift;
    uint32_t offset = pos & page_mask;

    getbus(dev);

    while (len) {
        ssize_t written = _at25xxx_write_page(dev, page, offset, d, len);

        if (written < 0) {
            res = written;
            break;
        }

        len -= written;

        if (len == 0) {
            break;
        }

        d      += written;
        page   += (offset + written) >> page_shift;
        offset  = (offset + written) & page_mask;
    }

    spi_release(dev->params.spi);

    return res;
}

void at25xxx_write_byte(const at25xxx_t *dev, uint32_t pos, uint8_t data)
{
    at25xxx_write(dev, pos, &data, sizeof(data));
}

int at25xxx_read(const at25xxx_t *dev, uint32_t pos, void *data, size_t len)
{
    if (pos + len > dev->params.size) {
        return -ERANGE;
    }

    getbus(dev);

    /* wait for previous write to finish - may take up to 5 ms */
    int res = _wait_until_eeprom_ready(dev);
    if (res) {
        return res;
    }

    pos = _pos(CMD_READ, pos);
    spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, true, &pos, NULL, 1 + ADDR_LEN / 8);
    spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, false, NULL, data, len);

    spi_release(dev->params.spi);

    return 0;
}

uint8_t at25xxx_read_byte(const at25xxx_t *dev, uint32_t pos)
{
    uint8_t b;
    at25xxx_read(dev, pos, &b, sizeof(b));
    return b;
}

int at25xxx_set(const at25xxx_t *dev, uint32_t pos, uint8_t val, size_t len)
{
    uint8_t data[AT225XXXX_SET_BUF_SIZE];

    if (pos + len > dev->params.size) {
        return -ERANGE;
    }

    memset(data, val, sizeof(data));

    /* page size is always a power of two */
    const uint32_t page_shift = bitarithm_msb(AT25_PAGE_SIZE);
    const uint32_t page_mask = AT25_PAGE_SIZE - 1;

    uint32_t page   = pos >> page_shift;
    uint32_t offset = pos & page_mask;

    getbus(dev);

    while (len) {
        size_t written = _at25xxx_write_page(dev, page, offset, data, min(len, sizeof(data)));
        len    -= written;
        page   += (offset + written) >> page_shift;
        offset  = (offset + written) & page_mask;
    }

    spi_release(dev->params.spi);

    return 0;
}

int at25xxx_clear(const at25xxx_t *dev, uint32_t pos, size_t len)
{
    return at25xxx_set(dev, pos, 0, len);
}

int at25xxx_init(at25xxx_t *dev, const at25xxx_params_t *params)
{
    dev->params = *params;
    spi_init_cs(dev->params.spi, dev->params.cs_pin);

    if (gpio_is_valid(dev->params.wp_pin)) {
        gpio_init(dev->params.wp_pin, GPIO_OUT);
        gpio_set(dev->params.wp_pin);
    }

    if (gpio_is_valid(dev->params.hold_pin)) {
        gpio_init(dev->params.hold_pin, GPIO_OUT);
        gpio_set(dev->params.hold_pin);
    }

    if (!IS_ACTIVE(NDEBUG)) {
        /* if assertions are on, trigger an assert on incorrect SPI settings
         * right on initialization to ease debugging */
        getbus(dev);
        spi_release(dev->params.spi);
    }
    return 0;
}
