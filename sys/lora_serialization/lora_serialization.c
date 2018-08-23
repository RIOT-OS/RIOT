/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief       Lora-serialization format implementation
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @author      Jose Ignacio Alamos <jose.alamos@haw-hamburg.de>
 */

#include "byteorder.h"
#include "assert.h"
#include "lora_serialization.h"

void lora_serialization_reset(lora_serialization_t *serialization)
{
    memset(serialization->buffer, 0, LORA_SERIALIZATION_MAX_BUFFER_SIZE);
    serialization->cursor = 0;
}

void lora_serialization_write_unix_time(lora_serialization_t *serialization,
                                        uint32_t unixtime)
{
    le_uint32_t *value = (le_uint32_t *)
                         (serialization->buffer + serialization->cursor);

    assert((serialization->cursor + LORA_SERIALIZATION_UNIX_TIME_SIZE) <
           LORA_SERIALIZATION_MAX_BUFFER_SIZE);

    *value = byteorder_btoll(byteorder_htonl(unixtime));
    serialization->cursor += LORA_SERIALIZATION_UNIX_TIME_SIZE;
}

void lora_serialization_write_coordinates(lora_serialization_t *serialization,
                                          double latitude, double longitude)
{
    int32_t lat = latitude * 1e6;
    int32_t lng = longitude * 1e6;
    le_uint32_t *lat_value = (le_uint32_t *)
                             (serialization->buffer + serialization->cursor);
    le_uint32_t *lng_value = (le_uint32_t *)
                             (serialization->buffer + serialization->cursor
                              + LORA_SERIALIZATION_LATITUDE_SIZE);

    assert((serialization->cursor + LORA_SERIALIZATION_GPS_SIZE) <
           LORA_SERIALIZATION_MAX_BUFFER_SIZE);

    *lat_value = byteorder_btoll(byteorder_htonl(lat));
    *lng_value = byteorder_btoll(byteorder_htonl(lng));

    serialization->cursor += LORA_SERIALIZATION_GPS_SIZE;
}

void lora_serialization_write_uint16(lora_serialization_t *serialization,
                                     uint16_t i)
{
    le_uint16_t *value = (le_uint16_t *)
                         (serialization->buffer + serialization->cursor);

    assert((serialization->cursor + LORA_SERIALIZATION_UINT16_SIZE) <
           LORA_SERIALIZATION_MAX_BUFFER_SIZE);

    *value = byteorder_btols(byteorder_htons(i));
    serialization->cursor += LORA_SERIALIZATION_UINT16_SIZE;
}

void lora_serialization_write_uint8(lora_serialization_t *serialization,
                                    uint8_t i)
{
    assert((serialization->cursor + LORA_SERIALIZATION_UINT8_SIZE) <
           LORA_SERIALIZATION_MAX_BUFFER_SIZE);

    (serialization->buffer)[serialization->cursor] = i;
    serialization->cursor += LORA_SERIALIZATION_UINT8_SIZE;
}

void lora_serialization_write_humidity(lora_serialization_t *serialization,
                                       float humidity)
{
    int16_t h = (int16_t)(humidity * 100);
    le_uint16_t *value = (le_uint16_t *)
                         (serialization->buffer + serialization->cursor);

    assert((serialization->cursor + LORA_SERIALIZATION_HUMIDITY_SIZE) <
           LORA_SERIALIZATION_MAX_BUFFER_SIZE);

    *value = byteorder_btols(byteorder_htons(h));
    serialization->cursor += LORA_SERIALIZATION_HUMIDITY_SIZE;
}

/* The temperature has to be  written in Big Endian */
void lora_serialization_write_temperature(lora_serialization_t *serialization,
                                          float temperature)
{
    int16_t t = (int16_t) (temperature * 100);
    network_uint16_t *value = (network_uint16_t *)
                              (serialization->buffer + serialization->cursor);

    assert((serialization->cursor + LORA_SERIALIZATION_TEMPERATURE_SIZE) <
           LORA_SERIALIZATION_MAX_BUFFER_SIZE);

    if (temperature < 0) {
        t = ~-t;
        t++;
    }

    *value = byteorder_htons((uint16_t) t);
    serialization->cursor += LORA_SERIALIZATION_TEMPERATURE_SIZE;
}

void lora_serialization_write_bitmap(lora_serialization_t *serialization,
                                     bool a, bool b, bool c, bool d, bool e,
                                     bool f, bool g, bool h)
{
    uint8_t bitmap = 0;

    assert((serialization->cursor + 1U) < LORA_SERIALIZATION_MAX_BUFFER_SIZE);

    bitmap |= (a & 1) << 7;
    bitmap |= (b & 1) << 6;
    bitmap |= (c & 1) << 5;
    bitmap |= (d & 1) << 4;
    bitmap |= (e & 1) << 3;
    bitmap |= (f & 1) << 2;
    bitmap |= (g & 1) << 1;
    bitmap |= (h & 1) << 0;

    serialization->buffer[serialization->cursor] = bitmap;
}
