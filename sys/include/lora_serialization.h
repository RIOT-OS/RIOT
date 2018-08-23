/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_lora_serialization
 * @{
 * @file
 * @brief       Lora serialization interface and definitions
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @author      Jose Ignacio Alamos <jose.alamos@haw-hamburg.de>
 */

#ifndef LORA_SERIALIZATION_H
#define LORA_SERIALIZATION_H

#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LORA_SERIALIZATION_MAX_BUFFER_SIZE
#define LORA_SERIALIZATION_MAX_BUFFER_SIZE (42U)
#endif

/**
 * @name Lora Serialization data size
 * @{
 */
#define LORA_SERIALIZATION_UNIX_TIME_SIZE    (4U)
#define LORA_SERIALIZATION_LATITUDE_SIZE     (4U)
#define LORA_SERIALIZATION_LONGITUDE_SIZE    (4U)
#define LORA_SERIALIZATION_UINT8_SIZE        (1U)
#define LORA_SERIALIZATION_UINT16_SIZE       (2U)
#define LORA_SERIALIZATION_TEMPERATURE_SIZE  (2U)
#define LORA_SERIALIZATION_HUMIDITY_SIZE     (2U)
#define LORA_SERIALIZATION_BITMAP_SIZE       (1U)

#define LORA_SERIALIZATION_GPS_SIZE            \
    LORA_SERIALIZATION_LATITUDE_SIZE + LORA_SERIALIZATION_LONGITUDE_SIZE
/** @} */

/**
 * @brief Lora Serialization descriptor
 */
typedef struct {
    uint8_t buffer[LORA_SERIALIZATION_MAX_BUFFER_SIZE]; /**< data buffer */
    uint8_t cursor;                                     /**< data cursor */
} lora_serialization_t;

/**
 * @brief Clears the buffer and resets the cursor
 *
 * @param[in] serialization     lora serialization descriptor
 */
void lora_serialization_reset(lora_serialization_t *serialization);

/**
 * @brief Adds the encoded unix time to the buffer
 *
 * @param[in] serialization     lora serialization descriptor
 * @param[in] unixtime          the unix time value
 */
int lora_serialization_write_unix_time(lora_serialization_t *serialization,
                                       uint32_t unixtime);

/**
 * @brief Adds encoded coordinates to the buffer
 *
 * @param[in] serialization     lora serialization descriptor
 * @param[in] latitude          latitude value
 * @param[in] longitude         longitude value
 */
int lora_serialization_write_coordinates(lora_serialization_t *serialization,
                                         double latitude, double longitude);

/**
 * @brief Adds an unsigned 16 bits integer to the buffer
 *
 * @param[in] serialization     lora serialization descriptor
 * @param[in] value             integer value
 */
int lora_serialization_write_uint16(lora_serialization_t *serialization,
                                    uint16_t value);

/**
 * @brief Adds an unsigned 8 bits integer to the buffer
 *
 * @param[in] serialization     lora serialization descriptor
 * @param[in] value             integer value
 */
int lora_serialization_write_uint8(lora_serialization_t *serialization,
                                   uint8_t value);

/**
 * @brief Adds encoded humidity value to the buffer
 *
 * @param[in] serialization     lora serialization descriptor
 * @param[in] humidity          humidity value
 */
int lora_serialization_write_humidity(lora_serialization_t *serialization,
                                      float humidity);

/**
 * @brief Adds encoded temperature value to the buffer
 *
 * @param[in] serialization     lora serialization descriptor
 * @param[in] temperature       temperature value
 */
int lora_serialization_write_temperature(lora_serialization_t *serialization,
                                         float temperature);

/**
 * @brief Adds a bitmap to the buffer. The bits are represented
 * from letter `a` to `h`, with `a` being the most significant bit.
 *
 * @param[in] serialization     lora serialization descriptor
 * @param[in] flags             flags encoded in a byte
 */
int lora_serialization_write_bitmap(lora_serialization_t *serialization,
                                    uint8_t flags);

#ifdef __cplusplus
}
#endif

#endif /* LORA_SERIALIZATION_H */
/** @} */
