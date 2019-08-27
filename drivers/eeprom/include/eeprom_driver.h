/*
 * Copyright (C) 2018 Inria
 *               2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_eeprom Common EEPROM interface
 * @ingroup     drivers_storage
 *
 * This module provides a common API for accessing EEPROM (both internal and
 * external), as well as common code and convenience interfaces.
 *
 * @{
 *
 * @file
 * @brief       Interface definition of EEPROM drivers
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef EEPROM_DRIVER_H
#define EEPROM_DRIVER_H

#include "eeprom.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Driver interface for EEPROM devices
 *
 * This interface must be implemented by every EEPROM device driver. The
 * function pointers in @ref eeprom_driver_t::set and
 * @ref eeprom_driver_t::erase can be `NULL`.
 */
typedef struct {
    /**
     * @brief   Initialize the data structures for the EEPROM device
     *
     * @param[in,out]   handle  Device handle to initialize
     * @param[in]       params  Initialization parameters of the device
     *
     * @retval  0       Failed to initialize the device
     * @return          The size of the device in bytes
     */
    eeprom_off_t (*init)(void *handle, const void *params);
    /**
     * @brief   Read data from the EEPROM device
     *
     * @param[in,out]   handle  Device handle of the EEPROM to read from
     * @param[in]       pos     Position to start to read from (in bytes)
     * @param[out]      dest    Buffer to write the data to
     * @param[in]       len     Number of bytes to read
     *
     * @retval  0       Success
     * @retval  -EIO    I/O error occurred while reading
     */
    int (*read)(void *handle, eeprom_off_t pos, void *dest, size_t len);
    /**
     * @brief   Write data from the EEPROM device
     *
     * @param[in,out]   handle  Device handle of the EEPROM to write to
     * @param[in]       pos     Position to start to write to (in bytes)
     * @param[in]       data    Buffer to write to the EEPROM device
     * @param[in,out]   len     Number of bytes to write
     *
     * @retval  0       Success
     * @retval  -EIO    I/O error occurred while writen
     *
     * If `-EFAULT` has been returned, the content of the EEPROM device is
     * unchanged. If `0` is returned, all data has been written successfully
     * to the device. If `-EIO` is returned, the contents in `[pos; pos + len[`
     * of the device are undefined, but the remaining content of the device
     * is unchanged.
     */
    int (*write)(void *handle, eeprom_off_t pos, const void *data, size_t len);
    /**
     * @brief   Set all bytes in `[pos; pos + len[` to the value in @p val
     *
     * @param[in,out]   handle  Device handle of the EEPROM to write to
     * @param[in]       pos     Position to start to write to (in bytes)
     * @param[in]       val     Write this in every byte in `[pos; pos + len[`
     * @param[in        len     Number of bytes to set to @p val
     *
     * @retval  0       Success
     * @retval  -EIO    I/O error occurred while writen
     *
     * If `-EFAULT` has been returned, the content of the EEPROM device is
     * unchanged. If `0` is returned, all data has been written successfully
     * to the device. If `-EIO` is returned, the contents in `[pos; pos + len[`
     * of the device are undefined, but the remaining content of the device
     * is unchanged.
     *
     * This function is optional to implement by the driver. If the function
     * pointer is `NULL`, a fallback implementation that calls
     * @ref eeprom_driver_t::write is used instead.
     */
    int (*set)(void *handle, eeprom_off_t pos, uint8_t val, size_t len);
    /**
     * @brief   Erase the whole EEPROM
     *
     * @param[in,out]   handle  Device handle of the EEPROM to erase
     *
     * @retval  0       Success
     * @retval  -EIO    I/O error occurred during erase
     *
     * If `0` is returned a subsequent read for any valid address must return
     * @ref eeprom_driver_t::clear_byte. If `-EIO` is returned, the whole
     * content of the EEPROM is undefined.
     *
     * This function is optional to implement by the driver. If the function
     * pointer is `NULL`, a fallback implementation that calls
     * @ref eeprom_driver_t::set is used instead.
     */
    int (*erase)(void *handle);
    /**
     * @brief   The EEPROM is filled with this byte after being erased
     */
    uint8_t clear_byte;
} eeprom_driver_t;

/**
 * @brief   The current state of an EEPROM device
 */
typedef struct {
    const eeprom_driver_t *driver;  /**< The driver of the EEPROM device */
    const void *params;             /**< The initialization parameters */
    void *handle;                   /**< The handle to pass to the driver */
} eeprom_dev_t;

/**
 * @brief   Initialize all EEPROM devices
 *
 * This function is to be called only once and during boot.
 */
void eeprom_init(void);


#ifdef __cplusplus
}
#endif

#endif /* EEPROM_DRIVER_H */
/** @} */
