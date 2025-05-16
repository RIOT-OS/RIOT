/*
 * Copyright (C) 2018,2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    ble_bluetil_ad BLE Advertising Data (AD) Processing
 * @ingroup     ble_bluetil
 * @brief       Generic BLE advertising and scan request data processing
 *
 * This module provides functionality for user friendly reading and writing of
 * BLE advertising and scan request buffers.
 *
 * This module is independent from any BLE stack.
 *
 * @{
 *
 * @file
 * @brief       Interface for the generic BLE advertising data processing module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "net/ble.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Static initializer for the advertising data structure
 */
#define BLUETIL_AD_INIT(b,p,s)          { .buf = b, .pos = p, .size = s }

/**
 * @brief   Default flags to set when advertising BLE devices
 */
#define BLUETIL_AD_FLAGS_DEFAULT        (BLE_GAP_DISCOVERABLE | \
                                         BLE_GAP_FLAG_BREDR_NOTSUP)

/**
 * @brief   Return values used by the bluetil_ad module
 */
enum {
    BLUETIL_AD_OK       =  0,   /**< everything went as expected */
    BLUETIL_AD_NOTFOUND = -1,   /**< entry not found */
    BLUETIL_AD_NOMEM    = -2,   /**< insufficient memory to write field */
};

/**
 * @brief   Struct used for returning the contents of a selected field
 */
typedef struct {
    uint8_t *data;              /**< pointer a field's payload */
    size_t len;                 /**< length of the payload */
} bluetil_ad_data_t;

/**
 * @brief   Descriptor for a buffer containing advertising data
 */
typedef struct {
    uint8_t *buf;               /**< buffer containing the advertising data */
    size_t pos;                 /**< current write position in the buffer */
    size_t size;                /**< overall length of the buffer */
} bluetil_ad_t;

/**
 * @brief   Initialize the given advertising data descriptor
 *
 * @param[out] ad       advertising data descriptor
 * @param[in]  buf      buffer to point to
 * @param[in]  pos      current fill position of @p buf
 * @param[in]  size     size of @p buf
 */
void bluetil_ad_init(bluetil_ad_t *ad, void *buf, size_t pos, size_t size);

/**
 * @brief   Find a specific field in the given advertising data
 *
 * @param[in]  ad       advertising data descriptor
 * @param[in]  type     field type to look for
 * @param[out] data     position and length of the field's payload
 *
 * @return  BLUETIL_AD_OK if field was found
 * @return  BLUETIL_AD_NOTFOUND if field was not found
 */
int bluetil_ad_find(const bluetil_ad_t *ad,
                    uint8_t type, bluetil_ad_data_t *data);

/**
 * @brief   Find a specific field and compare its value against the given data
 *
 * @param[in] ad        advertising data descriptor
 * @param[in] type      field to search for
 * @param[in] val       data to compare against
 * @param[in] val_len   size of @p val in byte
 *
 * @return  true if the field was found and its data is equal to the given data
 * @return  false if the field was not found or the data is not equal
 */
int bluetil_ad_find_and_cmp(const bluetil_ad_t *ad, uint8_t type,
                            const void *val, size_t val_len);

/**
 * @brief   Find the given field and copy its payload into a string
 *
 * The resulting string is `\0` terminated. If the resulting string is too large
 * to fit into the given buffer, it will be truncated to the maximum possible
 * size (but still including the `\0` at the end).
 *
 * @param[in]  ad       advertising data descriptor
 * @param[in]  type     field type to look for
 * @param[out] str      resulting string is written to this buffer
 * @param[in]  str_len  maximum number of bytes to write to @p str, including
 *                      the `\0` character
 *
 * @return  BLUETIL_AD_OK if the field was found and copied
 * @return  BLUETIL_AD_NOTFOUND if the given field was not found
 */
int bluetil_ad_find_str(const bluetil_ad_t *ad, uint8_t type,
                        char *str, size_t str_len);

/**
 * @brief   Add a new field to the given advertising data
 *
 * @param[out] ad       advertising data descriptor
 * @param[in] type      field type to add
 * @param[in] data      payload for the field
 * @param[in] data_len  length of the payload in bytes
 *
 * @return  BLUETIL_AD_OK if the new field was added
 * @return  BLUETIL_AD_NOMEM if there is not enough space to write add field
 */
int bluetil_ad_add(bluetil_ad_t *ad, uint8_t type,
                   const void *data, size_t data_len);

/**
 * @brief   Convenience function to add the "flags" field
 *
 * @param[out] ad       advertising data descriptor
 * @param[in]  flags    flags to set
 *
 * @return  BLUETIL_AD_OK if the flags field was added
 * @return  BLUETIL_AD_NOMEM if there is not enough space to write add field
 */
static inline int bluetil_ad_add_flags(bluetil_ad_t *ad, uint8_t flags)
{
    return bluetil_ad_add(ad, BLE_GAP_AD_FLAGS, &flags, 1);
}

/**
 * @brief   Convenience function to add the "full name" field
 *
 * While the given name must be `\0` terminated, the termination character is
 * not written to the advertising data.
 *
 * @param[out] ad       advertising data descriptor
 * @param[in]  name     name to set
 *
 * @return  BLUETIL_AD_OK if the name field was added
 * @return  BLUETIL_AD_NOMEM if there is not enough space to add the name field
 */
static inline int bluetil_ad_add_name(bluetil_ad_t *ad, const char *name)
{
    return bluetil_ad_add(ad, BLE_GAP_AD_NAME, name, strlen(name));
}

/**
 * @brief   Convenience function for initializing the advertising data
 *          descriptor and directly adding the flags field
 *
 * Most users will want to set the (mandatory) flags field right after
 * initializing the advertising context, so this function provides a small
 * shortcut.
 *
 * @param[out] ad       advertising data descriptor
 * @param[out] buf      buffer to write advertising data into
 * @param[in]  buf_len  size of @p buf in bytes
 * @param[in]  flags    flags to set, typically BLUETIL_AD_FLAGS_DEFAULT
 *
 * @return  BLUETIL_AD_OK on successful initialization with flags field
 * @return  BLUETIL_AD_NOMEM if given buffer is too small
 */
static inline int bluetil_ad_init_with_flags(bluetil_ad_t *ad,
                                             void *buf, size_t buf_len,
                                             uint8_t flags)
{
    bluetil_ad_init(ad, buf, 0, buf_len);
    return bluetil_ad_add_flags(ad, flags);
}

/* add more convenience functions on demand... */

#ifdef __cplusplus
}
#endif

/** @} */
