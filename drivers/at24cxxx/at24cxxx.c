/*
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_at24cxxx
 * @{
 *
 * @file
 * @brief       Device driver implementation for at24cxxx EEPROM units.
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "assert.h"
#include "macros/utils.h"
#include "xtimer.h"

#include "at24cxxx_defines.h"
#include "at24cxxx.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Calculate x mod y, if y is a power of 2
 */
#define MOD_POW2(x, y) ((x) & ((y) - 1))

/**
 * @brief I2C bus number shortcut
 */
#define DEV_I2C_BUS                     (dev->params.i2c)
/**
 * @brief Pin wp shortcut
 */
#define DEV_PIN_WP                      (dev->params.pin_wp)
/**
 * @brief EEPROM size shortcut
 */
#define DEV_EEPROM_SIZE                 (dev->params.eeprom_size)
/**
 * @brief I2C device address shortcut
 */
#define DEV_I2C_ADDR                    (dev->params.dev_addr)
/**
 * @brief Page size shortcut
 */
#define DEV_PAGE_SIZE                   (dev->params.page_size)
/**
 * @brief Max polls shortcut
 */
#define DEV_MAX_POLLS                   (dev->params.max_polls)

#ifndef AT24CXXX_SET_BUF_SIZE
/**
 * @brief  Adjust to configure buffer size
 */
#define AT24CXXX_SET_BUF_SIZE           (32U)
#endif

static
int _read(const at24cxxx_t *dev, uint32_t pos, void *data, size_t len)
{
    int check;
    uint8_t polls = DEV_MAX_POLLS;
    uint8_t dev_addr;
    uint8_t flags = 0;

    if (DEV_EEPROM_SIZE > 2048) {
        /* 2 bytes word address length if more than 11 bits are
           used for addressing */
        /* append page address bits to device address (if any) */
        dev_addr  = (DEV_I2C_ADDR | ((pos & 0xFF0000) >> 16));
        pos &= 0xFFFF;
        flags = I2C_REG16;
    }
    else {
        /* append page address bits to device address (if any) */
        dev_addr = (DEV_I2C_ADDR | ((pos & 0xFF00) >> 8));
        pos &= 0xFF;
    }

    while (-ENXIO == (check = i2c_read_regs(DEV_I2C_BUS, dev_addr,
                                            pos, data, len, flags))) {
        if (--polls == 0) {
            break;
        }
        xtimer_usleep(AT24CXXX_POLL_DELAY_US);
    }

    DEBUG("[at24cxxx] i2c_read_regs(): %d; polls: %d\n", check, polls);

    return check;
}

static int _read_max(const at24cxxx_t *dev, uint32_t pos, void *data, size_t len)
{
#ifdef PERIPH_I2C_MAX_BYTES_PER_FRAME
    uint8_t *data_p = data;

    while (len) {
        size_t clen = MIN(len, PERIPH_I2C_MAX_BYTES_PER_FRAME);

        if (_read(dev, pos, data_p, clen) == AT24CXXX_OK) {
            len -= clen;
            pos += clen;
            data_p += clen;
        }
        else {
            return -EIO;
        }
    }

    return AT24CXXX_OK;
#else
    return _read(dev, pos, data, len);
#endif
}

static
int _write_page(const at24cxxx_t *dev, uint32_t pos, const void *data, size_t len)
{
    int check;
    uint8_t polls = DEV_MAX_POLLS;
    uint8_t dev_addr;
    uint8_t flags = 0;

    if (DEV_EEPROM_SIZE > 2048) {
        /* 2 bytes word address length if more than 11 bits are
           used for addressing */
        /* append page address bits to device address (if any) */
        dev_addr  = (DEV_I2C_ADDR | ((pos & 0xFF0000) >> 16));
        pos &= 0xFFFF;
        flags = I2C_REG16;
    }
    else {
        /* append page address bits to device address (if any) */
        dev_addr = (DEV_I2C_ADDR | ((pos & 0xFF00) >> 8));
        pos &= 0xFF;
    }

    while (-ENXIO == (check = i2c_write_regs(DEV_I2C_BUS, dev_addr,
                                             pos, data, len, flags))) {
        if (--polls == 0) {
            break;
        }
        xtimer_usleep(AT24CXXX_POLL_DELAY_US);
    }

    DEBUG("[at24cxxx] i2c_write_regs(): %d; polls: %d\n", check, polls);

    return check;
}

static
int _write(const at24cxxx_t *dev, uint32_t pos, const void *data, size_t len)
{
    int check = 0;
    const uint8_t *cdata = ((const uint8_t *)data);

    while (len) {
        size_t clen = MIN(len, DEV_PAGE_SIZE - MOD_POW2(pos, DEV_PAGE_SIZE));

        check = _write_page(dev, pos, cdata, clen);

        if (!check) {
            len -= clen;
            pos += clen;
            cdata += clen;
        }
        else {
            break;
        }
    }
    return check;
}

static
int _set(const at24cxxx_t *dev, uint32_t pos, uint8_t val, size_t len)
{
    int check = 0;
    uint8_t set_buffer[AT24CXXX_SET_BUF_SIZE];

    memset(set_buffer, val, sizeof(set_buffer));
    while (len) {
        size_t clen = MIN(sizeof(set_buffer), len);
        check = _write(dev, pos, set_buffer, clen);
        if (!check) {
            len -= clen;
            pos += clen;
        }
        else {
            break;
        }
    }
    return check;
}

int at24cxxx_init(at24cxxx_t *dev, const at24cxxx_params_t *params)
{
    if (!dev || !params) {
        return -EINVAL;
    }
    dev->params = *params;
    if (gpio_is_valid(DEV_PIN_WP)) {
        gpio_init(DEV_PIN_WP, GPIO_OUT);
        at24cxxx_disable_write_protect(dev);
    }
    /* Check I2C bus once */
    i2c_acquire(DEV_I2C_BUS);
    i2c_release(DEV_I2C_BUS);
    return AT24CXXX_OK;
}

int at24cxxx_read_byte(const at24cxxx_t *dev, uint32_t pos, void *dest)
{
    if (pos >= DEV_EEPROM_SIZE) {
        return -ERANGE;
    }

    i2c_acquire(DEV_I2C_BUS);
    int check = _read(dev, pos, dest, 1);
    i2c_release(DEV_I2C_BUS);
    return check;
}

int at24cxxx_read(const at24cxxx_t *dev, uint32_t pos, void *data, size_t len)
{
    if (pos + len > DEV_EEPROM_SIZE) {
        return -ERANGE;
    }

    int check = AT24CXXX_OK;
    if (len) {
        i2c_acquire(DEV_I2C_BUS);
        check = _read_max(dev, pos, data, len);
        i2c_release(DEV_I2C_BUS);
    }

    return check;
}

int at24cxxx_write_byte(const at24cxxx_t *dev, uint32_t pos, uint8_t data)
{
    if (pos >= DEV_EEPROM_SIZE) {
        return -ERANGE;
    }

    i2c_acquire(DEV_I2C_BUS);
    int  check = _write(dev, pos, &data, 1);
    i2c_release(DEV_I2C_BUS);
    return check;
}

int at24cxxx_write(const at24cxxx_t *dev, uint32_t pos, const void *data,
                       size_t len)
{
    if (pos + len > DEV_EEPROM_SIZE) {
        return -ERANGE;
    }

    int check = AT24CXXX_OK;
    if (len) {
        i2c_acquire(DEV_I2C_BUS);
        check = _write(dev, pos, data, len);
        i2c_release(DEV_I2C_BUS);
    }
    return check;
}

int at24cxxx_set(const at24cxxx_t *dev, uint32_t pos, uint8_t val,
                     size_t len)
{
    if (pos + len > DEV_EEPROM_SIZE) {
        return -ERANGE;
    }

    int check = AT24CXXX_OK;
    if (len) {
        i2c_acquire(DEV_I2C_BUS);
        check = _set(dev, pos, val, len);
        i2c_release(DEV_I2C_BUS);
    }
    return check;
}

int at24cxxx_clear(const at24cxxx_t *dev, uint32_t pos, size_t len)
{
    return at24cxxx_set(dev, pos, AT24CXXX_CLEAR_BYTE, len);
}

int at24cxxx_erase(const at24cxxx_t *dev)
{
    return at24cxxx_clear(dev, 0, DEV_EEPROM_SIZE);
}

int at24cxxx_enable_write_protect(const at24cxxx_t *dev)
{
    if (!gpio_is_valid(DEV_PIN_WP)) {
        return -ENOTSUP;
    }
    gpio_set(DEV_PIN_WP);
    return AT24CXXX_OK;
}

int at24cxxx_disable_write_protect(const at24cxxx_t *dev)
{
    if (!gpio_is_valid(DEV_PIN_WP)) {
        return -ENOTSUP;
    }
    gpio_clear(DEV_PIN_WP);
    return AT24CXXX_OK;
}

#ifdef MODULE_MTD_AT24CXXX
#include "mtd_at24cxxx.h"

#define DEV(mtd_ptr)        (((mtd_at24cxxx_t *)(mtd_ptr))->at24cxxx_eeprom)
#define PARAMS(mtd_ptr)     (((mtd_at24cxxx_t *)(mtd_ptr))->params)

static int _mtd_at24cxxx_init(mtd_dev_t *mtd)
{
    assert(mtd);
    assert(mtd->driver == &mtd_at24cxxx_driver);
    assert(DEV(mtd));
    assert(PARAMS(mtd));
    int init = at24cxxx_init(DEV(mtd), PARAMS(mtd));
    if (init != AT24CXXX_OK) {
        return init;
    }
    mtd->page_size = DEV(mtd)->params.page_size;
    mtd->pages_per_sector = 1;
    mtd->sector_count = DEV(mtd)->params.eeprom_size /
                        DEV(mtd)->params.page_size;
    mtd->write_size = 1;
    return 0;
}

static int _mtd_at24cxxx_read_page(mtd_dev_t *mtd, void *dest, uint32_t page,
                                   uint32_t offset, uint32_t size)
{
    const at24cxxx_t *dev = DEV(mtd);

    /* some i2c implementations have a limit on the transfer size */
#ifdef PERIPH_I2C_MAX_BYTES_PER_FRAME
    size = MIN(size, PERIPH_I2C_MAX_BYTES_PER_FRAME);
#endif

    i2c_acquire(DEV_I2C_BUS);
    int res = _read(dev, page * mtd->page_size + offset, dest, size);
    i2c_release(DEV_I2C_BUS);

    return res < 0 ? res : (int)size;
}

static int mtd_at24cxxx_write_page(mtd_dev_t *mtd, const void *src, uint32_t page,
                                   uint32_t offset, uint32_t size)
{
    const at24cxxx_t *dev = DEV(mtd);

    /* write no more than to the end of the current page to prevent wrap-around */
    size_t remaining = DEV_PAGE_SIZE - offset;
    size = MIN(size, remaining);

    i2c_acquire(DEV_I2C_BUS);
    int res = _write_page(dev, page * mtd->page_size + offset, src, size);
    i2c_release(DEV_I2C_BUS);

    return res < 0 ? res : (int)size;
}

static int _mtd_at24cxxx_erase(mtd_dev_t *mtd, uint32_t addr, uint32_t size)
{
    return at24cxxx_clear(DEV(mtd), addr, size) == AT24CXXX_OK ? 0 : -EIO;
}

static int _mtd_at24cxxx_power(mtd_dev_t *mtd, enum mtd_power_state power)
{
    (void)mtd;
    (void)power;
    return -ENOTSUP;
}

const mtd_desc_t mtd_at24cxxx_driver = {
    .init = _mtd_at24cxxx_init,
    .read_page = _mtd_at24cxxx_read_page,
    .write_page = mtd_at24cxxx_write_page,
    .erase = _mtd_at24cxxx_erase,
    .power = _mtd_at24cxxx_power,
    .flags = MTD_DRIVER_FLAG_DIRECT_WRITE,
};
#endif
