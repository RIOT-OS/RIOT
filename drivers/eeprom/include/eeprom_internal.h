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
 * @brief     Internal helpers functions and macros used in the EEPROM API
 *
 * @author    Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef EEPROM_INTERNAL_H
#define EEPROM_INTERNAL_H

#include "eeprom_types.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(EEPROM_SET_BUF) || defined(DOXYGEN)
/**
 * @brief   Stack allocated buffer to implement the set fallback function
 */
#define EEPROM_SET_BUF              (32U)
#endif

#if !defined(EEPROM_NUMOF) || defined(DOXYGEN)
/**
 * @brief   Number of EEPROM devices present
 */
#define EEPROM_NUMOF                (1U)
#endif

/**
 * @brief   States of the registered EEPROM devices
 */
extern eeprom_dev_t _eeprom_devs[EEPROM_NUMOF];

/**
 * @brief       Check if the given device is valid and initialized
 *
 * @param[in]   dev     The device to check
 *
 * @retval      0       The device is not valid
 * @retval      1       The device is valid
 */
static inline int _eeprom_is_dev_valid(eeprom_t dev)
{
    return ((dev < EEPROM_NUMOF) && (_eeprom_devs[dev].size)) ? 1 : 0;
}

/**
 * @brief   Get the EEPROM driver for the given devices
 *
 * @param[in]   dev     The device to get the driver from
 *
 * @return  The driver of the EEPROM device identified by @p dev
 */
static inline const eeprom_driver_t * _eeprom_driver(eeprom_t dev)
{
    return _eeprom_devs[dev].driver;
}

/**
 * @brief   Get the EEPROM device handle for the given devices
 *
 * @param[in]   dev     The device to get the driver from
 *
 * @return  The device handle of the EEPROM device identified by @p dev
 */
static inline void * _eeprom_handle(eeprom_t dev)
{
    return _eeprom_devs[dev].handle;
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
static inline int _eeprom_is_range_valid(eeprom_t dev, eeprom_offset_t pos,
                                         size_t len)
{
    /* We basically want to check for "pos + len < size", but we have to
     * be a bit careful to prevent integer overflows */
    if (len > _eeprom_devs[dev].size) {
        return 0;
    }

    if (_eeprom_devs[dev].size - len < pos) {
        return 0;
    }

    return 1;
}

#ifdef __cplusplus
}
#endif

#endif /* EEPROM_INTERNAL_H */
