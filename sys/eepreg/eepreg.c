/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   sys_eepreg
 * @{
 *
 * @file
 * @brief   eepreg implementation
 *
 * @author  Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include <stdint.h>

#include "assert.h"
#include "eepreg.h"
#include "eepreg_conf.h"
#include "periph/eeprom.h"

/* basic compile-time sanity check for eepreg_conf.h */
#define EEPREG_S_TEST    (EEPREG_MAX_ENTRY_NUM == \
                          sizeof(eepreg_sizes) / sizeof(eepreg_sizes[0]))
#define EEPREG_N_TEST    (EEPREG_MAX_ENTRY_NUM == \
                          sizeof(eepreg_names) / sizeof(eepreg_names[0]))
static_assert(EEPREG_S_TEST, "[eepreg] wrong num of entries in eepreg_sizes");
static_assert(EEPREG_N_TEST, "[eepreg] wrong num of entries in eepreg_names");

static uint32_t _find_entry(unsigned num)
{
    uint32_t addr = 0;

    for (unsigned i = 0; i <= num; i++) {
        /* check if correct entry is found */
        if (i == num) {
            return addr;
        }

        /* skip entry name */
        addr += sizeof(eepreg_names[i]);

        /* skip entry content */
        addr += eepreg_sizes[i];
    }

    return (uint32_t)(-1);
}

static int _check_name_at_addr(unsigned num, uint32_t addr)
{
    size_t len = sizeof(eepreg_names[num]);
    uint8_t data[len];

    eeprom_read(addr, data, len);

    for (unsigned i = 0; i <= len; i++) {
        if (eepreg_names[num][i] != data[i]) {
            return -1;
        }
    }

    return 0;
}

uint32_t eepreg_addr(unsigned num)
{
    uint32_t addr;

    if (num >= EEPREG_MAX_ENTRY_NUM) {
        return (uint32_t)(-1);
    }

    addr = _find_entry(num);

    if (_check_name_at_addr(num, addr) < 0) {
        return (uint32_t)(-1);
    }

    /* skip name */
    addr += sizeof(eepreg_names[num]);

    return addr;
}

int eepreg_check_name(unsigned num)
{
    uint32_t addr = _find_entry(num);

    return _check_name_at_addr(num, addr);
}

int eepreg_write_name(unsigned num)
{
    size_t len = sizeof(eepreg_names[num]);
    uint32_t addr = _find_entry(num);

    if (eeprom_write(addr, eepreg_names[num], len) != len) {
        return -1;
    }

    return 0;
}
