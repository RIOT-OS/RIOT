/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 *
 * @{
 *
 * @file    Implementation of test backend in RAM
 *
 * @author  Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "configuration.h"

#include "container.h"
#include "persist_types.h"

struct configuration persist_conf = {
    .food = {
        .bread = {
            .white = {"1.00"},
            .whole_grain = {"1.20"},
        },
        .cake = {
            .cheesecake = {"1.99"},
            . donut = {"1.00"},
        },
    },
    .drinks = {
        .coffee = {"0.50"},
        .tea = {"0.60"},
        .cocoa = {"1.00"},
    },
    .orders = {
        { .items = { {"sugar"}, {"tomatoes"} } },
        { .items = { {"coffee"}, {"milk"} } },
        { .items = { {"bread"}, {"coffee"} } },
    },
};

struct kv {
    const char *key;
    void *value;
    bool deleted;
};

/* A real backend would not have to store the keys statically */
static struct kv _kv[] = {
    {CONFIGURATION_RIOT_ROOT"/food/bread/white", &persist_conf.food.bread.white, false},
    {CONFIGURATION_RIOT_ROOT"/food/bread/whole_grain", &persist_conf.food.bread.whole_grain, false},

    {CONFIGURATION_RIOT_ROOT"/food/cake/cheesecake", &persist_conf.food.cake.cheesecake, false},
    {CONFIGURATION_RIOT_ROOT"/food/cake/donut", &persist_conf.food.cake.donut, false},

    {CONFIGURATION_RIOT_ROOT"/drinks/coffee", &persist_conf.drinks.coffee, false},
    {CONFIGURATION_RIOT_ROOT"/drinks/tea", &persist_conf.drinks.tea, false},
    {CONFIGURATION_RIOT_ROOT"/drinks/cocoa", &persist_conf.drinks.cocoa, false},

    {CONFIGURATION_RIOT_ROOT"/orders/0", &persist_conf.orders[0], false},
    {CONFIGURATION_RIOT_ROOT"/orders/1", &persist_conf.orders[1], false},
    {CONFIGURATION_RIOT_ROOT"/orders/2", &persist_conf.orders[2], false},
};

static int _be_ram_load(const struct conf_backend *be,
                        conf_key_buf_t *key, void *val, size_t *size)
{
    (void)be;
    for (unsigned i = 0; i < ARRAY_SIZE(_kv); i++) {
        if (!strcmp(key->buf, _kv[i].key) && !_kv[i].deleted) {
            memcpy(val, _kv[i].value, *size);
            return 0;
        }
    }
    return -ENOENT;
}

static int _be_ram_store(const struct conf_backend *be,
                         conf_key_buf_t *key, const void *val, size_t *size)
{
    (void)be;
    for (unsigned i = 0; i < ARRAY_SIZE(_kv); i++) {
        if (!strcmp(key->buf, _kv[i].key)) {
            memcpy(_kv[i].value, val, *size);
            _kv[i].deleted = false;
            return 0;
        }
    }
    return -ENOENT;
}

static int _be_ram_delete(const struct conf_backend *be,
                          conf_key_buf_t *key)
{
    (void)be;
    for (unsigned i = 0; i < ARRAY_SIZE(_kv); i++) {
        if (!strcmp(key->buf, _kv[i].key)) {
            _kv[i].deleted = true;
            return 0;
        }
    }
    return -ENOENT;
}

static const conf_backend_ops_t _be_ram_ops = {
    .be_load = _be_ram_load,
    .be_store = _be_ram_store,
    .be_delete = _be_ram_delete,
};

static conf_backend_t _be_ram =  {
    .ops = &_be_ram_ops,
};

const conf_backend_t *configuration_backend_ram_get(void)
{
    return &_be_ram;
}
