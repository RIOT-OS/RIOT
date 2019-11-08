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
 * @brief     Implementation of the EEPROM API (parts that can be inlined)
 *
 * @author    Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef EEPROM_IMPLEMENTATION_H
#define EEPROM_IMPLEMENTATION_H

#include "eeprom_types.h"
#include "eeprom_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline eeprom_off_t eeprom_size(eeprom_t dev)
{
    if (_eeprom_is_dev_valid(dev)) {
        return _eeprom_devs[dev].size;
    }

    return 0;
}

static inline int eeprom_read(eeprom_t dev, eeprom_off_t pos, void *dest,
                              size_t len)
{
    if (!_eeprom_is_dev_valid(dev)) {
        return -ENODEV;
    }

    if (!_eeprom_is_range_valid(dev, pos, len)) {
        return -EFAULT;
    }

    if (!dest) {
        return -EINVAL;
    }

    return _eeprom_driver(dev)->read(_eeprom_handle(dev), pos, dest, len);
}

static inline int eeprom_write(eeprom_t dev, eeprom_off_t pos, const void *data,
                               size_t len)
{
    if (!_eeprom_is_dev_valid(dev)) {
        return -ENODEV;
    }

    if (!_eeprom_is_range_valid(dev, pos, len)) {
        return -EFAULT;
    }

    if (!data) {
        return -EINVAL;
    }

    return _eeprom_driver(dev)->write(_eeprom_handle(dev), pos, data, len);
}

static inline int eeprom_erase(eeprom_t dev)
{
    if (!_eeprom_is_dev_valid(dev)) {
        return -ENODEV;
    }

    if (!_eeprom_driver(dev)->erase) {
        return eeprom_set(dev, 0, _eeprom_driver(dev)->clear_byte,
                          eeprom_size(dev));
    }

    return _eeprom_driver(dev)->erase(_eeprom_handle(dev));
}

static inline int eeprom_read_byte(eeprom_t dev, eeprom_off_t pos,
                                   uint8_t *dest)
{
    return eeprom_read(dev, pos, dest, 1);
}

static inline int eeprom_write_byte(eeprom_t dev, eeprom_off_t pos,
                                    uint8_t data)
{
    return eeprom_write(dev, pos, &data, 1);
}

static inline int eeprom_clear(eeprom_t dev, eeprom_off_t pos, size_t len)
{
    return eeprom_set(dev, pos, 0, len);
}

#ifdef __cplusplus
}
#endif

#endif /* EEPROM_IMPLEMENTATION_H */
