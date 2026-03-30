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

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "eepreg.h"
#include "mtd.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* EEPREG magic number */
static const char eepreg_magic[] = "RIOTREG";

/* constant lengths */
#define MAGIC_SIZE      (sizeof(eepreg_magic) - 1)    /* -1 to remove null */
#define ENT_LEN_SIZ     (1U)

/* constant locations */
#define REG_START          (EEPROM_RESERV_CPU_LOW + EEPROM_RESERV_BOARD_LOW)
#define REG_MAGIC_LOC      (REG_START)
#define REG_END_PTR_LOC    (REG_MAGIC_LOC + MAGIC_SIZE)
#define REG_ENT1_LOC(mtd)  (REG_END_PTR_LOC + eepreg_ptr_len(mtd))
#define DAT_START(mtd)     (eepreg_size(mtd) - EEPROM_RESERV_CPU_HI \
                            - EEPROM_RESERV_BOARD_HI - 1)

static inline uint32_t _read_meta_uint(mtd_dev_t *dev, uint32_t loc)
{
    uint8_t data[4];
    uint32_t ret;

    mtd_read(dev, data, loc, eepreg_ptr_len(dev));

    /* unused array members will be discarded */
    ret = ((uint32_t)data[0] << 24)
          | ((uint32_t)data[1] << 16)
          | ((uint32_t)data[2] << 8)
          | ((uint32_t)data[3]);

    /* bit shift to discard unused array members */
    ret >>= 8 * (4 - eepreg_ptr_len(dev));

    return ret;
}

static inline void _write_meta_uint(mtd_dev_t *dev, uint32_t loc, uint32_t val)
{
    uint8_t data[4];

    val <<= 8 * (4 - eepreg_ptr_len(dev));

    data[0] = (uint8_t)(val >> 24);
    data[1] = (uint8_t)(val >> 16);
    data[2] = (uint8_t)(val >> 8);
    data[3] = (uint8_t)val;

    mtd_write(dev, data, loc, eepreg_ptr_len(dev));
}

static inline uint32_t _get_reg_end(mtd_dev_t *dev)
{
    return _read_meta_uint(dev, REG_END_PTR_LOC);
}

static inline void _set_reg_end(mtd_dev_t *dev, uint32_t loc)
{
    _write_meta_uint(dev, REG_END_PTR_LOC, loc);
}

static inline uint32_t _get_last_loc(mtd_dev_t *dev, uint32_t reg_end)
{
    if (reg_end == REG_ENT1_LOC(dev)) {
        /* no entries yet */
        return DAT_START(dev);
    }

    return _read_meta_uint(dev, reg_end - eepreg_ptr_len(dev));
}

static inline uint32_t _calc_free_space(uint32_t reg_end, uint32_t last_loc)
{
    return last_loc - reg_end;
}

static inline uint8_t _get_meta_len(mtd_dev_t *dev, uint32_t meta_loc)
{
    uint8_t byte;
    mtd_read(dev, &byte, meta_loc, 1);
    return byte;
}

static inline void _set_meta_len(mtd_dev_t *dev, uint32_t meta_loc, uint8_t meta_len)
{
    mtd_write(dev, &meta_len, meta_loc, 1);
}

static inline uint32_t _get_data_loc(mtd_dev_t *dev, uint32_t meta_loc, uint8_t meta_len)
{
    /* data location is at the end of meta-data */
    return _read_meta_uint(dev, meta_loc + meta_len - eepreg_ptr_len(dev));
}

static inline void _set_data_loc(mtd_dev_t *dev, uint32_t meta_loc, uint8_t meta_len,
                                 uint32_t data_loc)
{
    /* data location is at the end of meta-data */
    _write_meta_uint(dev, meta_loc + meta_len - eepreg_ptr_len(dev), data_loc);
}

static inline uint8_t _calc_name_len(mtd_dev_t *dev, uint8_t meta_len)
{
    /* entry contents: meta-data length, name, data pointer */
    return meta_len - ENT_LEN_SIZ - eepreg_ptr_len(dev);
}

static inline void _get_name(mtd_dev_t *dev, uint32_t meta_loc, char *name, uint8_t meta_len)
{
    /* name is after entry length */
    mtd_read(dev, name, meta_loc + ENT_LEN_SIZ,
             _calc_name_len(dev, meta_len));
}

static inline int _cmp_name(mtd_dev_t *dev, uint32_t meta_loc, const char *name,
                            uint8_t meta_len)
{
    /* name is after entry length */
    uint32_t loc = meta_loc + ENT_LEN_SIZ;

    uint8_t len = _calc_name_len(dev, meta_len);

    uint8_t offset;
    for (offset = 0; offset < len; offset++) {
        if (name[offset] == '\0') {
            /* entry name is longer than name */
            return 0;
        }

        uint8_t byte;
        mtd_read(dev, &byte, loc + offset, 1);
        if (byte != (uint8_t)name[offset]) {
            /* non-matching character */
            return 0;
        }
    }

    if (name[offset] == '\0') {
        /* entry name is the same length as name */
        return 1;
    }

    /* entry name is shorter than name */
    return 0;
}

static inline uint32_t _get_meta_loc(mtd_dev_t *dev, const char *name)
{
    uint32_t meta_loc = REG_ENT1_LOC(dev);
    uint32_t reg_end = _get_reg_end(dev);

    while (meta_loc < reg_end) {
        uint8_t meta_len = _get_meta_len(dev, meta_loc);

        if (_cmp_name(dev, meta_loc, name, meta_len)) {
            return meta_loc;
        }

        meta_loc += meta_len;
    }

    /* no meta-data found */
    return (uint32_t)UINT_MAX;
}

static inline uint32_t _get_data_len(mtd_dev_t *dev, uint32_t meta_loc, uint32_t data_loc)
{
    uint32_t prev_loc;
    if (meta_loc == REG_ENT1_LOC(dev)) {
        prev_loc = DAT_START(dev);
    }
    else {
        /* previous entry data pointer is just before this entry */
        prev_loc = _read_meta_uint(dev, meta_loc - eepreg_ptr_len(dev));
    }

    return prev_loc - data_loc;
}

static inline int _new_entry(mtd_dev_t *dev, const char *name, uint32_t data_len)
{
    uint32_t reg_end = _get_reg_end(dev);
    uint32_t last_loc = _get_last_loc(dev, reg_end);
    uint32_t free_space = _calc_free_space(reg_end, last_loc);

    uint8_t name_len = (uint8_t)strlen(name);
    uint8_t meta_len = ENT_LEN_SIZ + name_len + eepreg_ptr_len(dev);

    /* check to see if there is enough room */
    if (free_space < meta_len + data_len) {
        return -ENOSPC;
    }

    /* set the length of the meta-data */
    _set_meta_len(dev, reg_end, meta_len);

    /* write name of entry */
    mtd_write(dev, name, reg_end + ENT_LEN_SIZ, name_len);

    /* set the location of the data */
    _set_data_loc(dev, reg_end, meta_len, last_loc - data_len);

    /* update end of the registry */
    _set_reg_end(dev, reg_end + meta_len);

    return 0;
}

static inline void _move_data(mtd_dev_t *dev, uint32_t oldpos, uint32_t newpos, uint32_t len)
{
    for (uint32_t count = 0; count < len; count++) {
        uint32_t offset;

        if (newpos < oldpos) {
            /* move from beginning of data */
            offset = count;
        }
        else {
            /* move from end of data */
            offset = len - count;
        }

        uint8_t byte;
        mtd_read(dev, &byte, oldpos + offset, 1);
        mtd_write(dev, &byte, newpos + offset, 1);
    }
}

int eepreg_add(mtd_dev_t *dev, uint32_t *pos, const char *name, uint32_t len)
{
    int ret = eepreg_check(dev);
    if (ret == -ENOENT) {
        /* reg does not exist, so make a new one */
        eepreg_reset(dev);
    }
    else if (ret < 0) {
        DEBUG("[eepreg_add] eepreg_check failed\n");
        return ret;
    }

    uint32_t reg_end = _get_reg_end(dev);

    uint32_t meta_loc = _get_meta_loc(dev, name);

    if (meta_loc == (uint32_t)UINT_MAX) {
        /* entry does not exist, so make a new one */

        /* location of the new data */
        *pos = _get_last_loc(dev, reg_end) - len;

        if (_new_entry(dev, name, len) < 0) {
            DEBUG("[eepreg_add] not enough space for %s\n", name);
            return -ENOSPC;
        }

        return 0;
    }

    *pos = _get_data_loc(dev, meta_loc, _get_meta_len(dev, meta_loc));

    if (len != _get_data_len(dev, meta_loc, *pos)) {
        DEBUG("[eepreg_add] %s already exists with different length\n", name);
        return -EADDRINUSE;
    }

    return 0;
}

int eepreg_read(mtd_dev_t *dev, uint32_t *pos, const char *name)
{
    int ret = eepreg_check(dev);
    if (ret < 0) {
        DEBUG("[eepreg_read] eepreg_check failed\n");
        return ret;
    }

    uint32_t meta_loc = _get_meta_loc(dev, name);

    if (meta_loc == (uint32_t)UINT_MAX) {
        DEBUG("[eepreg_read] no entry for %s\n", name);
        return -ENOENT;
    }

    *pos = _get_data_loc(dev, meta_loc, _get_meta_len(dev, meta_loc));

    return 0;
}

int eepreg_write(mtd_dev_t *dev, uint32_t *pos, const char *name, uint32_t len)
{
    uint32_t reg_end = _get_reg_end(dev);

    int ret = eepreg_check(dev);
    if (ret == -ENOENT) {
        /* reg does not exist, so make a new one */
        eepreg_reset(dev);
    }
    else if (ret < 0) {
        DEBUG("[eepreg_write] eepreg_check failed\n");
        return ret;
    }

    /* location of the new data */
    *pos = _get_last_loc(dev, reg_end) - len;

    if (_new_entry(dev, name, len) < 0) {
        DEBUG("[eepreg_write] not enough space for %s\n", name);
        return -ENOSPC;
    }

    return 0;
}

int eepreg_rm(mtd_dev_t *dev, const char *name)
{
    int ret = eepreg_check(dev);
    if (ret < 0) {
        DEBUG("[eepreg_rm] eepreg_check failed\n");
        return ret;
    }

    uint32_t meta_loc = _get_meta_loc(dev, name);

    if (meta_loc == (uint32_t)UINT_MAX) {
        DEBUG("[eepreg_rm] no entry for %s\n", name);
        return -ENOENT;
    }

    uint32_t reg_end = _get_reg_end(dev);
    uint32_t last_loc = _get_last_loc(dev, reg_end);

    uint8_t meta_len = _get_meta_len(dev, meta_loc);
    uint32_t tot_meta_len = reg_end - meta_loc;

    uint32_t data_loc = _get_data_loc(dev, meta_loc, meta_len);
    uint32_t data_len = _get_data_len(dev, meta_loc, data_loc);

    /* data_loc is above last_loc due to descending order */
    uint32_t tot_data_len = data_loc - last_loc;

    _move_data(dev, meta_loc + meta_len, meta_loc, tot_meta_len);

    _move_data(dev, last_loc, last_loc + data_len, tot_data_len);

    reg_end -= meta_len;
    _set_reg_end(dev, reg_end);

    /* update data locations */
    while (meta_loc < reg_end) {
        meta_len = _get_meta_len(dev, meta_loc);
        data_loc = _get_data_loc(dev, meta_loc, meta_len);

        /* addition due to descending order */
        _set_data_loc(dev, meta_loc, meta_len, data_loc + data_len);

        meta_loc += meta_len;
    }

    return 0;
}

int eepreg_iter(mtd_dev_t *dev, eepreg_iter_cb_t cb, void *arg)
{
    uint32_t reg_end = _get_reg_end(dev);

    int ret = eepreg_check(dev);
    if (ret < 0) {
        DEBUG("[eepreg_len] eepreg_check failed\n");
        return ret;
    }

    uint32_t meta_loc = REG_ENT1_LOC(dev);
    while (meta_loc < reg_end) {
        uint8_t meta_len = _get_meta_len(dev, meta_loc);

        /* size of memory allocation */
        uint8_t name_len = _calc_name_len(dev, meta_len);

        char name[name_len + 1];

        /* terminate string */
        name[name_len] = '\0';

        _get_name(dev, meta_loc, name, meta_len);

        /* execute callback */
        ret = cb(dev, name, arg);

        if (ret < 0) {
            DEBUG("[eepreg_iter] callback reports failure\n");
            return ret;
        }

        /* only advance if cb didn't delete entry */
        if (_cmp_name(dev, meta_loc, name, meta_len)) {
            meta_loc += meta_len;
        }
    }

    return 0;
}

int eepreg_check(mtd_dev_t *dev)
{
    char magic[MAGIC_SIZE];

    /* get magic number from EEPROM */
    if (mtd_read(dev, magic, REG_MAGIC_LOC, MAGIC_SIZE)) {

        DEBUG("[eepreg_check] EEPROM read error\n");
        return -EIO;
    }

    /* check to see if magic number is the same */
    if (strncmp(magic, eepreg_magic, MAGIC_SIZE) != 0) {
        DEBUG("[eepreg_check] No registry detected\n");
        return -ENOENT;
    }

    return 0;
}

int eepreg_reset(mtd_dev_t *dev)
{
    /* write new registry magic number */
    if (mtd_write(dev, eepreg_magic, REG_MAGIC_LOC, MAGIC_SIZE)) {

        DEBUG("[eepreg_reset] EEPROM write error\n");
        return -EIO;
    }

    /* new registry has no entries */
    _set_reg_end(dev, REG_ENT1_LOC(dev));

    return 0;
}

int eepreg_len(mtd_dev_t *dev, uint32_t *len, const char *name)
{
    int ret = eepreg_check(dev);
    if (ret < 0) {
        DEBUG("[eepreg_len] eepreg_check failed\n");
        return ret;
    }

    uint32_t meta_loc = _get_meta_loc(dev, name);

    if (meta_loc == (uint32_t)UINT_MAX) {
        DEBUG("[eepreg_len] no entry for %s\n", name);
        return -ENOENT;
    }

    uint32_t data_loc = _get_data_loc(dev, meta_loc, _get_meta_len(dev, meta_loc));

    *len = _get_data_len(dev, meta_loc, data_loc);

    return 0;
}

int eepreg_free(mtd_dev_t *dev, uint32_t *len)
{
    int ret = eepreg_check(dev);
    if (ret < 0) {
        DEBUG("[eepreg_free] eepreg_check failed\n");
        return ret;
    }

    uint32_t reg_end = _get_reg_end(dev);
    uint32_t last_loc = _get_last_loc(dev, reg_end);
    *len = _calc_free_space(reg_end, last_loc);

    return 0;
}
