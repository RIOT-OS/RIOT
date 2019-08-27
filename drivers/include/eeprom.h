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

#ifndef EEPROM_H
#define EEPROM_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_EEPROM_T
/**
 * @brief   Device handle type for EEPROM devices
 */
typedef uint_fast8_t eeprom_t;
#endif

/**
 * @brief   Return the size of the EEPROM device
 *
 * @param[in]       dev     Device handle of the EEPROM device to get the size
 *
 * @return  The size of the EEPROM device in bytes
 * @retval  0               No EEPROM device for parameter @p dev
 */
size_t eeprom_size(eeprom_t dev);

/**
 * @brief   Read @p len bytes from the given position
 *
 * @param[in]   dev     Device handle of the EEPROM device to read from
 * @param[in]   pos     Start position in EEPROM
 * @param[out]  dest    Output memory location to write to
 * @param[in]   len     The number of bytes to read
 *
 * @retval  0           Success
 * @retval  -ENODEV     No device for given handle in @p dev
 * @retval  -EFAULT     Bad address given in @p pos
 * @retval  -EINVAL     Parameter @p dest is `NULL`
 * @retval  -EIO        I/O error while reading
 *
 * @details If this functions fails with `-EIO`, the contents of @p dest are
 *          undefined. For every other error the content is unchanged.
 */
int eeprom_read(eeprom_t dev, size_t pos, void *dest, size_t len);

/**
 * @brief   Write @p len bytes at the given position
 *
 * @param[in]   dev     Device handle of the EEPROM device to write to
 * @param[in]   pos     Start position in EEPROM
 * @param[in]   data    Data to write to the device
 * @param[in]   len     The number of bytes to write
 *
 * @retval  0           Success
 * @retval  -ENODEV     No device for given handle in @p dev
 * @retval  -EFAULT     Bad address given in @p pos
 * @retval  -EINVAL     Parameter @p dest is `NULL`
 * @retval  -EIO        I/O error while writing to the device
 *
 * @details If this functions fails with `-EIO`, the contents in the range
 *          `[pos; pos + len[` on the device are undefined. For every other
 *          error the EEPROM is not changed.
 */
int eeprom_write(eeprom_t dev, size_t pos, const void *data, size_t len);

/**
 * @brief   Set @p len bytes from the given position @p pos with value @p val
 *
 * @param[in]   dev     Device handle of the EEPROM device to write to
 * @param[in]   pos     Start position in EEPROM
 * @param[in]   val     The value to set
 * @param[in]   len     The number of bytes to set
 *
 * @retval  0           Success
 * @retval  -ENODEV     No device for given handle in @p dev
 * @retval  -EFAULT     Bad address given in @p pos
 * @retval  -EINVAL     Parameter @p dest is `NULL`
 * @retval  -EIO        I/O error while writing to the device
 *
 * @details If this functions fails with `-EIO`, the contents in the range
 *          `[pos; pos + len[` on the device are undefined. For every other
 *          error the EEPROM is not changed.
 */
int eeprom_set(eeprom_t dev, size_t pos, uint8_t val, size_t len);

/**
 * @brief   Erase the whole EEPROM content
 *
 * @param[in]   dev     Device handle of the EEPROM device to write to
 *
 * @retval  0           Success
 * @retval  -ENODEV     No device for given handle in @p dev
 * @retval  -EIO        I/O error while writing to the device
 */
int eeprom_erase(eeprom_t dev);

/**
 * @brief   Read a byte at the given position in eeprom
 *
 * @param[in]       dev     Device handle of the EEPROM device to read from
 * @param[in]       pos     Position to read
 * @param[out]      dest    Destination to store the byte
 *
 * @retval  0               Success
 * @retval  -ENODEV         No device for given handle in @p dev
 * @retval  -EFAULT         Bad address given in @p pos
 * @retval  -EINVAL         Parameter @p dest is `NULL`
 */
static inline int eeprom_read_byte(eeprom_t dev, uint32_t pos, uint8_t *dest)
{
    return eeprom_read(dev, pos, dest, 1);
}

/**
 * @brief   Write a byte at the given position
 *
 * @param[in]       dev     Device handle of the EEPROM device to read from
 * @param[in]       pos     Position to write to
 * @param[in]       data    Data to write
 *
 * @retval  0               Success
 * @retval  -ENODEV         No device for given handle in @p dev
 * @retval  -EFAULT         Bad address given in @p pos
 * @retval  -EINVAL         Parameter @p dest is `NULL`
 */
static inline int eeprom_write_byte(eeprom_t dev, uint32_t pos, uint8_t data)
{
    return eeprom_write(dev, pos, &data, 1);
}

/**
 * @brief   Clear @p len bytes from the given position @p pos
 *
 * Clearing a byte in EEPROM simply consists in setting it to 0
 *
 * @retval  0           Success
 * @retval  -ENODEV     No device for given handle in @p dev
 * @retval  -EFAULT     Bad address given in @p pos
 * @retval  -EINVAL     Parameter @p dest is `NULL`
 * @retval  -EIO        I/O error while writing to the device
 *
 * @details If this functions fails with `-EIO`, the contents in the range
 *          `[pos; pos + len[` on the device are undefined. For every other
 *          error the EEPROM is not changed.
 */
static inline int eeprom_clear(eeprom_t dev, size_t pos, size_t len)
{
    return eeprom_set(dev, pos, 0, len);
}

#ifdef __cplusplus
}
#endif

#endif /* EEPROM_H */
/** @} */
