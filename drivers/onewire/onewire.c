/*
 * Copyright (C) 2016-2020 Freie Universität Berlin
 * Copyright (C) 2023 Prime Controls
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_onewire
 * @{
 *
 * @file
 * @brief       1-Wire bus driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 *
 * @}
 */

#include "onewire.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "bitfield.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _reset(onewire_t *bus)
{
    const onewire_driver_t *driver = bus->params->driver;
    void *lldev = bus->params->lldev;

    const int res = driver->reset(lldev);
    if (res == -ENXIO) {
        return res;
    }
    else if (res < 0) {
        return -EIO;
    }

    return 0;
}

static int _read_bits(onewire_t *bus, void *buf, size_t len)
{
    const onewire_driver_t *driver = bus->params->driver;
    void *lldev = bus->params->lldev;

    const int res = driver->read_bits(lldev, buf, len);
    if (res < 0) {
        return -EIO;
    }

    return 0;
}

static int _write_bits(onewire_t *bus, const void *buf, size_t len)
{
    const onewire_driver_t *driver = bus->params->driver;
    void *lldev = bus->params->lldev;

    const int res = driver->write_bits(lldev, buf, len);
    if (res < 0) {
        return -EIO;
    }

    return 0;
}

int onewire_select(onewire_t *bus, const onewire_rom_t *rom)
{
    assert(bus);

    int res = 0;

    DEBUG("%s\n", DEBUG_FUNC);

    res = _reset(bus);
    if (res < 0) {
        return res;
    }

    if (rom) {
        res = onewire_write_byte(bus, ONEWIRE_ROM_MATCH);
        if (res < 0) {
            return res;
        }

        res = onewire_write(bus, rom, sizeof(*rom));
        if (res < 0) {
            return res;
        }
    }
    else {
        res = onewire_write_byte(bus, ONEWIRE_ROM_SKIP);
        if (res < 0) {
            return res;
        }
    }

    return 0;
}

int onewire_read(onewire_t *bus, void *data, size_t len)
{
    assert(bus);
    assert(data);

    uint8_t *buf = data;

    const int res = _read_bits(bus, data, len * 8);

    DEBUG("%s: ", DEBUG_FUNC);
    for (size_t pos = 0; pos < len; pos++) {
        DEBUG("%02x ", buf[pos]);
    }
    DEBUG("\n");

    return res;
}

int onewire_write(onewire_t *bus, const void *data, size_t len)
{
    assert(bus);
    assert(data);

    const uint8_t *buf = data;

    DEBUG("%s: ", DEBUG_FUNC);
    for (size_t pos = 0; pos < len; pos++) {
        DEBUG("%02x ", buf[pos]);
    }
    DEBUG("\n");

    return _write_bits(bus, data, len * 8);
}

int onewire_read_rom(onewire_t *bus, onewire_rom_t *rom)
{
    assert(bus);
    assert(rom);

    int res;

    res = _reset(bus);
    if (res < 0) {
        return res;
    }

    res = onewire_write_byte(bus, ONEWIRE_ROM_READ);
    if (res < 0) {
        return res;
    }

    res = onewire_read(bus, rom, sizeof(*rom));
    if (res < 0) {
        return res;
    }

    if (onewire_rom_is_valid(rom) == false) {
        return -EBADMSG;
    }

    return 0;
}

int onewire_search(onewire_t *bus, onewire_rom_t *rom, int ld)
{
    assert(bus);
    assert(rom);

    int res;

    /* Initialize the search state. */
    if (ld == ONEWIRE_SEARCH_FIRST) {
        memset(rom, 0, sizeof(rom->u8));
    }

    /* Reset the bus. */
    res = _reset(bus);
    if (res < 0) {
        return res;
    }

    /* Send the search ROM command. */
    res = onewire_write_byte(bus, ONEWIRE_ROM_SEARCH);
    if (res < 0) {
        return res;
    }

    /* Start search. Refer to e.g. to
     * https://pdfserv.maximintegrated.com/en/an/AN937.pdf, page 51 and 52 for
     * a detailed description on the search algorithm. */
    int marker = 0;
    int pos = 1;
    for (unsigned i = 0; i < sizeof(rom->u8) * 8; i++) {
        uint8_t bits;

        /* Read two bits from the bus. */
        res = _read_bits(bus, &bits, 2);
        if (res < 0) {
            return res;
        }

        /* Both bits will be high if for some reason no device matches what has
           been sent so far. This could happen if a device was removed during
           the search process. */
        const bool no_dev = (bits & 3) == 3;
        if (no_dev) {
            return -ENXIO;
        }

        /* At least two devices exist with the next ROM bit in conflict. */
        const bool conflict = (bits & 3) == 0;
        if (conflict) {
            if (pos < ld) {
                bits = (bf_isset(rom->u8, i)) ? 1 : 0;
                if (bits == 0) {
                    marker = pos;
                }
            }
            else if (pos == ld) {
                bits = 1;
            }
            else {
                marker = pos;
                bits = 0;
            }
        }

        /* Send next bit of ROM we wish to match. */
        res = _write_bits(bus, &bits, 1);
        if (res < 0) {
            return res;
        }

        if (bits & 1) {
            bf_set(rom->u8, i);
        }
        else {
            bf_unset(rom->u8, i);
        }

        pos++;
    }

    if (onewire_rom_is_valid(rom) == false) {
        return -EBADMSG;
    }

    return marker;
}

int onewire_init(onewire_t *bus, const onewire_params_t *params)
{
    assert(bus);
    assert(params);
    assert(params->driver);
    assert(params->driver->init);
    assert(params->driver->reset);
    assert(params->driver->read_bits);
    assert(params->driver->write_bits);

    bus->params = params;

#ifndef MODULE_ONEWIRE_ONESLAVE
    mutex_init(&bus->lock);
#endif

    DEBUG_PUTS(DEBUG_FUNC);

    return params->driver->init(params->lldev, params->lldev_params);
}
