/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   drivers_eeprom
 * @{
 *
 * @file
 * @brief     Implementation for EEPROM driver API
 *
 * @author    Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <errno.h>
#include <string.h>

#include "assert.h"
#include "log.h"
#include "eeprom.h"
#include "eeprom_types.h"
#include "eeprom_internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

eeprom_dev_t _eeprom_devices[EEPROM_NUMOF] = { { 0 } };

int eeprom_set(eeprom_t dev, eeprom_off_t pos, uint8_t val, size_t len)
{
    if (!_eeprom_is_dev_valid(dev)) {
        return -ENODEV;
    }

    if (!_eeprom_is_range_valid(dev, pos, len)) {
        return -EFAULT;
    }

    uint8_t buf[EEPROM_SET_BUF] = { 0 };
    while (len > EEPROM_SET_BUF) {
        int retval;
        retval = _eeprom_driver(dev)->write(_eeprom_handle(dev), pos, buf, len);
        if (retval) {
            /* pass through error code */
            return retval;
        }
        pos += EEPROM_SET_BUF;
        len -= EEPROM_SET_BUF;
    }
    return _eeprom_driver(dev)->write(_eeprom_handle(dev), pos, buf, len);
}

void eeprom_register(const eeprom_driver_t *driver, void *handle,
                     eeprom_off_t size)
{
    static size_t idx = 0;
    /* Check for overflow of array in _eeprom_devices */
    assert(idx < EEPROM_NUMOF);
    /* Check for valid arguments */
    assert(driver && (size > 0));

    _eeprom_devices[idx].driver = driver;
    _eeprom_devices[idx].handle = handle;
    _eeprom_devices[idx].size = size;
    idx++;
}
