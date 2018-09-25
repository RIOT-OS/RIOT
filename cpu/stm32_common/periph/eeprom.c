/*
 * Copyright (C) 2018 Inria
 * Copyright (C) 2018 Unwired Devices LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_eeprom
 * @{
 *
 * @file
 * @brief       Low-level eeprom driver implementation
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Oleg Artamonov <oleg@unwds.com>
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "cpu.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#include "periph/eeprom.h"

extern void _lock(void);
extern void _unlock(void);
extern void _wait_for_pending_operations(void);

#ifndef EEPROM_START_ADDR
#error "periph/eeprom: EEPROM_START_ADDR is not defined"
#endif

static union {
    uint32_t data_word;
    uint8_t  data_bytes[4];
} eeprom_data;

static void _eeprom_write_byte(uint32_t pos, uint8_t byte)
{
    assert(pos < EEPROM_SIZE);

    _wait_for_pending_operations();
    *(__IO uint8_t *)(EEPROM_START_ADDR + pos) = byte;
}

static void _eeprom_write_word(uint32_t pos, uint32_t word)
{
    assert(pos <= EEPROM_SIZE - sizeof(uint32_t));

    _wait_for_pending_operations();
    *(__IO uint32_t *)(EEPROM_START_ADDR + pos) = word;
}

static uint8_t _eeprom_read_byte(uint32_t pos)
{
    assert(pos < EEPROM_SIZE);

    _wait_for_pending_operations();
    return *(__IO uint8_t *)(EEPROM_START_ADDR + pos);
}

static uint32_t _eeprom_read_word(uint32_t pos)
{
    assert(pos <= EEPROM_SIZE - sizeof(uint32_t));

    _wait_for_pending_operations();
    return *(__IO uint32_t *)(EEPROM_START_ADDR + pos);
}

void eeprom_write_byte(uint32_t pos, uint8_t data)
{
    assert(pos < EEPROM_SIZE);

    DEBUG("Writing data '%c' to EEPROM at pos %" PRIu32 "\n", data, pos);
    _unlock();
    _eeprom_write_byte(pos, data);
    _lock();
}

size_t eeprom_write(uint32_t pos, const uint8_t *data, size_t len)
{
    assert((pos + len) <= EEPROM_SIZE);

    DEBUG("[EEPROM] write %d bytes\n", len);
    _unlock();
    uint32_t i;
    uint32_t bytes = 0;

    /* write first bytes in case of unaligned access */
    uint32_t shift = pos & 0x3;
    if (shift > 0) {
        /* read-modify-write */
        eeprom_data.data_word = _eeprom_read_word(pos - shift);
        bytes = 4 - shift;
        if (len < bytes) {
            bytes = len;
        }
        if (data) {
            memcpy(&eeprom_data.data_bytes[shift], data, bytes);
        } else {
            /* if data is NULL just clear EEPROM */
            memset(&eeprom_data.data_bytes[shift], 0, bytes);
        }
        _eeprom_write_word(pos - shift, eeprom_data.data_word);
    }

    /* write by words to speed up the process */
    uint32_t words = (len - bytes) / 4;
    uint32_t remnant = (len - bytes) % 4;
    for (i = 0; i < words; i++) {
        if (data) {
            /* memcpy to fix possible unaligned access to data array */
            memcpy((void *)&eeprom_data.data_word, (void *)&data[bytes], 4);
            _eeprom_write_word(pos + bytes, eeprom_data.data_word);
        } else {
            /* if data is NULL just clear EEPROM */
            _eeprom_write_word(pos + bytes, 0);
        }
        bytes += 4;
    }

    /* write last bytes in case of unaligned access */
    if (remnant) {
        /* read-modify-write */
        eeprom_data.data_word = _eeprom_read_word(pos + bytes);
        if (data) {
            memcpy(eeprom_data.data_bytes, &data[bytes], remnant);
        } else {
            memset(eeprom_data.data_bytes, 0, remnant);
        }
        _eeprom_write_word(pos + bytes, eeprom_data.data_word);
        bytes += remnant;
    }
    _lock();

    return bytes;
}

uint8_t eeprom_read_byte(uint32_t pos)
{
    assert(pos < EEPROM_SIZE);

    DEBUG("Reading data from EEPROM at pos %" PRIu32 "\n", pos);
    _unlock();
    uint8_t byte = _eeprom_read_byte(pos);
    _lock();

    return byte;
}

size_t eeprom_read(uint32_t pos, uint8_t *data, size_t len)
{
    assert((pos + len) <= EEPROM_SIZE);

    DEBUG("[EEPROM] read %d bytes\n", len);
    _unlock();
    uint32_t i;
    uint32_t bytes = 0;

    /* read first bytes in case of unaligned access */
    uint32_t shift = pos & 0x3;
    if (shift > 0) {
        /* read a word and discard unneded bytes */
        eeprom_data.data_word = _eeprom_read_word(pos - shift);
        bytes = (4 - shift);
        if (len < bytes) {
            bytes = len;
        }
        memcpy(data, &eeprom_data.data_bytes[shift], bytes);
    }

    /* read by words to speed up the process */
    uint32_t words = (len - bytes) / 4;
    uint32_t remnant = (len - bytes) % 4;
    for (i = 0; i < words; i++) {
        eeprom_data.data_word = _eeprom_read_word(pos + bytes);
        /* memcpy to fix possible unaligned access to data array */
        memcpy((void *)&data[bytes], (void *)&eeprom_data.data_word, 4);
        bytes += 4;
    }

    /* read last bytes in case of unaligned access */
    if (remnant) {
        /* read a word and discard unneded bytes */
        eeprom_data.data_word = _eeprom_read_word(pos + bytes);
        memcpy((void *)&data[bytes], (void *)&eeprom_data.data_word, remnant);
        bytes += remnant;
    }
    _lock();

    return bytes;
}
