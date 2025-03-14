/*
 * Copyright (C) 2024 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_configuration
 * @{
 *
 * @file
 * @brief       Implementation of the default riotconf configuration backend
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "configuration.h"
#include "macros/utils.h"
#include "mutex.h"
#include "mtd_default.h"
#include "riotconf/slot.h"
#include "riotconf/storage.h"

#include "configuration_backend_riotconf.h"

static riotconf_slot_t _current = -EINVAL;
static riotconf_hdr_t _current_hdr;

__attribute__((weak))
int configuration_backend_riotconf_accept(const riotconf_hdr_t *hdr)
{
#ifdef CONFIGURATION_BACKENT_RIOTCONF_VERSION
    return hdr->version == CONFIGURATION_BACKENT_RIOTCONF_VERSION;
#endif
    (void)hdr;
    return 1;
}

__attribute__((weak))
void configuration_backend_riotconf_storage_init(void)
{
}

static int _be_riotconf_init(void)
{
    riotconf_slot_t current = riotconf_slot_highest_seq(configuration_backend_riotconf_accept);
    if (current < 0) {
        current = 0;
    }
    _current = current;
    return 0;
}

static int _be_riotconf_reset(void)
{
    return riotconf_slot_invalidate(configuration_backend_riotconf_slot_current());
}

static int _be_riotconf_load(const struct conf_backend *be,
                             conf_key_buf_t *key, void *val, size_t *size,
                             size_t offset, conf_backend_flags_t *flg)
{
    (void)be;
    (void)key;

    int ret;
    void *sector;
    size_t sector_size;
    if (_current < 0) {
        return -ENOENT;
    }
    size_t rd = *size;
    if (!(*flg & CONF_BACKEND_FLAG_START)) {
        riotconf_slot_start_read(_current, &sector, &sector_size);
        *flg |= CONF_BACKEND_FLAG_START;
        riotconf_hdr_t *p_hdr = sector;
        riotconf_hdr_t hdr = *p_hdr;
        riotconf_hdr_ntoh(&hdr);
        if (*size < hdr.size) {
            *size = hdr.size;
            *flg |= CONF_BACKEND_FLAG_MORE;
        }
        else {
            rd = hdr.size;
            *size = rd;
            *flg |= CONF_BACKEND_FLAG_FINISH;
        }
    }
    if ((ret = riotconf_slot_read(_current, val, offset, rd)) < 0) {
        goto fin;
    }
    if (*flg & CONF_BACKEND_FLAG_FINISH) {
fin:
        riotconf_slot_finish_read(_current);
        *flg &= ~CONF_BACKEND_FLAG_FINISH;
    }
    return ret;

}

static int _be_riotconf_store(const struct conf_backend *be,
                              conf_key_buf_t *key, const void *val, size_t *size,
                              size_t offset, conf_backend_flags_t *flg)
{
    (void)be;
    (void)key;
    if (_current < 0) {
        return -ENOENT;
    }
    if (!(*flg & CONF_BACKEND_FLAG_START)) {
        riotconf_slot_start_write(_current, &_current_hdr);
        *flg |= CONF_BACKEND_FLAG_START;
    }
    riotconf_slot_write(_current, val, offset, *size);
    if ((*flg & CONF_BACKEND_FLAG_FINISH)) {
        uint32_t version = _current_hdr.magic == RIOTCONF_MAGIC ? _current_hdr.version : 0;
        uint32_t sequence = _current_hdr.magic == RIOTCONF_MAGIC ? _current_hdr.sequence : 0;
        riotconf_slot_finish_write(_current, sequence, version, offset + *size);
        *flg &= ~CONF_BACKEND_FLAG_FINISH;
    }
    return 0;
}

static int _be_riotconf_delete(const struct conf_backend *be, conf_key_buf_t *key)
{
    (void)be;
    (void)key;
    if (_current < 0) {
        return -ENOENT;
    }
    return 0;
}

const conf_backend_ops_t conf_backend_riotconf_ops = {
    .be_load = _be_riotconf_load,
    .be_store = _be_riotconf_store,
    .be_delete = _be_riotconf_delete,
};

riotconf_slot_t configuration_backend_riotconf_slot_current(void)
{
    return _current;
}

riotconf_hdr_t configuration_backend_riotconf_header_current(void)
{
    riotconf_hdr_t hdr;
    riotconf_storage_read(riotconf_storage_get(_current), &hdr, 0, sizeof(hdr));
    return hdr;
}

int configuration_backend_riotconf_reset(void)
{
    return _be_riotconf_reset();
}

int configuration_backend_riotconf_init(void)
{
    return _be_riotconf_init();
}

void auto_init_configuration_backend_riotconf(void)
{
    configuration_backend_riotconf_storage_init();
    int fail = configuration_backend_riotconf_init();
    assert(!fail); (void)fail;
}

#ifndef AUTO_INIT_PRIO_MOD_CONFIGURATION_BACKEND_RIOTCONF
#define AUTO_INIT_PRIO_MOD_CONFIGURATION_BACKEND_RIOTCONF    5679
#endif

AUTO_INIT_CONFIGURATION(auto_init_configuration_backend_riotconf,
                        AUTO_INIT_PRIO_MOD_CONFIGURATION_BACKEND_RIOTCONF);
