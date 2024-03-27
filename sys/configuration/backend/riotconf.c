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

static riotconf_slot_t _current = -EINVAL;

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
    return 0;
}

static int _be_riotconf_load(const struct conf_backend *be,
                             conf_key_buf_t *key, void *val, size_t *size,
                             size_t offset, bool *more)
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
    if (offset == 0) {
        riotconf_slot_start_read(_current, &sector, &sector_size);
        riotconf_hdr_t *p_hdr = sector;
        riotconf_hdr_t hdr = *p_hdr;
        riotconf_hdr_ntoh(&hdr);
        if (*size < hdr.size) {
            *size = hdr.size;
            *more = true;
        }
        else {
            rd = hdr.size;
            *size = rd;
        }
    }
    if ((ret = riotconf_slot_read(_current, val, offset, rd)) < 0) {
        goto fin;
    }
    if (!*more) {
fin:
        riotconf_slot_finish_read(_current);
    }
    return ret;

}

static int _be_riotconf_store(const struct conf_backend *be,
                              conf_key_buf_t *key, const void *val, size_t *size,
                              size_t offset, bool more)
{
    (void)be;
    (void)key;
    if (_current < 0) {
        return -ENOENT;
    }
    if (offset == 0) {
        riotconf_slot_start_write(_current);
    }
    riotconf_slot_write(_current, val, offset, *size);
    if (!more) {
        riotconf_slot_finish_write(_current, 0, 0, offset + *size);
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
