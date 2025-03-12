/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @ingroup cpu_native
 * @ingroup drivers_periph_eeprom
 * @brief   Low-level EEPROM driver implementation for native
 * @author  Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "native_internal.h"

#include "cpu.h"
#include "mutex.h"
#include "periph/eeprom.h"
#include "eeprom_native.h"

#define ENABLE_DEBUG 0
#include "debug.h"

char eeprom_file[EEPROM_FILEPATH_MAX_LEN];

static uint8_t _eeprom_buf[EEPROM_SIZE] = { EEPROM_CLEAR_BYTE };

static mutex_t lock = MUTEX_INIT;

static inline void eeprom_native_write(void)
{
    FILE *fp = real_fopen(eeprom_file, "w");
    if (!fp) {
        return;
    }
    DEBUG("Writing data to EEPROM file %s:\n", eeprom_file);
    fwrite(_eeprom_buf, 1, ARRAY_SIZE(_eeprom_buf), fp);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        for (size_t i = 0; i < ARRAY_SIZE(_eeprom_buf); i++) {
            DEBUG("0x%02X ", _eeprom_buf[i]);
            if ((i + 1) % 16 == 0) {
                DEBUG("\n");
            }
        }
        DEBUG("\n");
    }
    fclose(fp);
}

void eeprom_native_read(void)
{
    FILE *fp = real_fopen(eeprom_file, "r");
    if (!fp) {
        return;
    }

    DEBUG("Reading data from EEPROM file %s:\n", eeprom_file);
    fread(_eeprom_buf, 1, ARRAY_SIZE(_eeprom_buf), fp);
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        for (size_t i = 0; i < ARRAY_SIZE(_eeprom_buf); i++) {
            DEBUG("0x%02X ", _eeprom_buf[i]);
            if ((i + 1) % 16 == 0) {
                DEBUG("\n");
            }
        }
        DEBUG("\n");
    }
    fclose(fp);
}

size_t eeprom_read(uint32_t pos, void *data, size_t len)
{
    assert(pos + len <= EEPROM_SIZE);

    mutex_lock(&lock);
    memcpy(data, &_eeprom_buf[pos], len);
    mutex_unlock(&lock);

    return len;
}

size_t eeprom_write(uint32_t pos, const void *data, size_t len)
{
    assert(pos + len <= EEPROM_SIZE);

    mutex_lock(&lock);
    memcpy(&_eeprom_buf[pos], data, len);
    mutex_unlock(&lock);

    /* Persist to the file */
    eeprom_native_write();

    return len;
}
