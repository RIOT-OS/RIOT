/*
 * SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       EEPROM mock
 *
 * @author      Martine S. Lenders <mlenders@riot-os.org>
 *
 * @}
 */

#include <string.h>
#include <sys/types.h>

#include "periph/eeprom.h"

#ifdef CPU_NATIVE
#  include "eeprom_native.h"

char eeprom_file[EEPROM_FILEPATH_MAX_LEN];

void eeprom_native_read(void)
{
    return;
}
#endif

static uint8_t _eeprom_mock[EEPROM_SIZE];

uint8_t eeprom_read_byte(uint32_t pos)
{
    if (pos >= EEPROM_SIZE) {
        return 0;
    }
    return _eeprom_mock[pos];
}

size_t eeprom_read(uint32_t pos, void *data, size_t len)
{
    if ((pos >= EEPROM_SIZE) || ((pos + len) >= EEPROM_SIZE)) {
        return 0;
    }
    memcpy(data, &_eeprom_mock[pos], len);
    return len;
}

void eeprom_write_byte(uint32_t pos, uint8_t data)
{
    if (pos >= EEPROM_SIZE) {
        return;
    }
    _eeprom_mock[pos] = data;
}

size_t eeprom_write(uint32_t pos, const void *data, size_t len)
{
    if ((pos >= EEPROM_SIZE) || ((pos + len) >= EEPROM_SIZE)) {
        return 0;
    }
    memcpy(&_eeprom_mock[pos], data, len);
    return len;
}
