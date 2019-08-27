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

#include "log.h"
#include "eeprom.h"
#include "eeprom_driver.h"
#include "eeprom_params.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifndef EEPROM_SET_BUF
/**
 * @brief   Stack allocated buffer to implement the set fallback function
 */
#define EEPROM_SET_BUF              (32)
#endif

static eeprom_off_t _sizes[EEPROM_NUMOF];

/**
 * @brief       Check if the given device is valid and initialized
 *
 * @param[in]   dev     The device to check
 *
 * @retval      0       The device is not valid
 * @retval      1       The device is valid
 */
static inline int _is_dev_valid(eeprom_t dev)
{
    return ((dev < EEPROM_NUMOF) && (_sizes[dev])) ? 1 : 0;
}

/**
 * @brief       Check if the given range is valid
 *
 * @param[in]   dev     The device the range refers to
 * @param[in]   pos     Start of the range to check
 * @param[in]   len     Length of the range to check
 *
 * @retval      0       The range is not valid
 * @retval      1       The range is valid
 */
static inline int _is_range_valid(eeprom_t dev, eeprom_off_t pos, size_t len)
{
    /* We basically want to check for "pos + len < _sizes[dev]", but we have to
     * be a bit careful to prevent integer overflows */
    if (len > _sizes[dev]) {
        return 0;
    }

    if (_sizes[dev] - len < pos) {
        return 0;
    }

    return 1;
}

void eeprom_init(void)
{
    for (unsigned i = 0; i < EEPROM_NUMOF; i++) {
        _sizes[i] = eeprom_devs[i].driver->init(eeprom_devs[i].handle,
                                                eeprom_devs[i].params);
        if (!_sizes[i]) {
            LOG_ERROR("[eeprom] Failed to initialize EEPROM device %u\n", i);
        }
    }
}

eeprom_off_t eeprom_size(eeprom_t dev)
{
    if (_is_dev_valid(dev)) {
        return _sizes[dev];
    }

    return 0;
}

int eeprom_read(eeprom_t dev, eeprom_off_t pos, void *dest, size_t len)
{
    if (!_is_dev_valid(dev)) {
        return -ENODEV;
    }

    if (!_is_range_valid(dev, pos, len)) {
        return -EFAULT;
    }

    if (!dest) {
        return -EINVAL;
    }

    return eeprom_devs[dev].driver->read(eeprom_devs[dev].handle, pos, dest, len);
}

int eeprom_write(eeprom_t dev, eeprom_off_t pos, const void *data, size_t len)
{
    if (!_is_dev_valid(dev)) {
        return -ENODEV;
    }

    if (!_is_range_valid(dev, pos, len)) {
        return -EFAULT;
    }

    if (!data) {
        return -EINVAL;
    }

    return eeprom_devs[dev].driver->write(eeprom_devs[dev].handle, pos, data, len);
}

int eeprom_set(eeprom_t dev, eeprom_off_t pos, uint8_t val, size_t len)
{
    if (!_is_dev_valid(dev)) {
        return -ENODEV;
    }

    if (!_is_range_valid(dev, pos, len)) {
        return -EFAULT;
    }

    uint8_t buf[EEPROM_SET_BUF];
    memset(buf, val, sizeof(buf));
    while (len > EEPROM_SET_BUF) {
        int retval = eeprom_devs[dev].driver->write(eeprom_devs[dev].handle,
                                                   pos, buf, len);
        if (retval) {
            /* pass through error code */
            return retval;
        }
        pos += EEPROM_SET_BUF;
        len -= EEPROM_SET_BUF;
    }
    return eeprom_devs[dev].driver->write(eeprom_devs[dev].handle,
                                          pos, buf, len);
}

int eeprom_erase(eeprom_t dev)
{
    if (!_is_dev_valid(dev)) {
        return -ENODEV;
    }

    if (!eeprom_devs[dev].driver->erase) {
        return eeprom_set(dev, 0, eeprom_devs[dev].driver->clear_byte,
                          _sizes[dev]);
    }

    return eeprom_devs[dev].driver->erase(eeprom_devs[dev].handle);
}
