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
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "configuration.h"
#include "configuration_backend_ram.h"

#include "container.h"
#include "persist_types.h"

#ifndef CSTRLEN
#define CSTRLEN(str)    (ARRAY_SIZE((const char[]){str}) - 1)
#endif

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

static uint8_t _enc_dec_buffer[CONFIG_CONFIGURATION_BACKEND_RAM_BUF_SIZE];

struct kv {
    const char *key;
    conf_sid_t sid;
    void *value;
    bool deleted;
    size_t len;
};

/* A real backend would not have to store the keys statically */
static struct kv _kv[] = {
    {"/", 0, _enc_dec_buffer, true, 0},
    {"/food/bread/white", TEST_FOOD_BREAD_WHITE_SID, &persist_conf.food.bread.white, false, CSTRLEN("1.00")},
    {"/food/bread/whole_grain", TEST_FOOD_BREAD_WHOLE_GRAIN_SID, &persist_conf.food.bread.whole_grain, false, CSTRLEN("1.20")},

    {"/food/cake/cheesecake", TEST_FOOD_CAKE_CHEESECAKE_SID, &persist_conf.food.cake.cheesecake, false, CSTRLEN("1.99")},
    {"/food/cake/donut", TEST_FOOD_CAKE_DONUT_SID, &persist_conf.food.cake.donut, false, CSTRLEN("1.00")},

    {"/drinks/coffee", TEST_DRINKS_COFFEE_SID, &persist_conf.drinks.coffee, false, CSTRLEN("0.50")},
    {"/drinks/tea", TEST_DRINKS_TEA_SID, &persist_conf.drinks.tea, false, CSTRLEN("0.60")},
    {"/drinks/cocoa", TEST_DRINKS_COCOA_SID, &persist_conf.drinks.cocoa, false, CSTRLEN("1.00")},

    {"/orders/0", TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (0 * TEST_ORDERS_INDEX_STRIDE), &persist_conf.orders[0], false, sizeof(persist_conf.orders[0]) },
    {"/orders/1", TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (1 * TEST_ORDERS_INDEX_STRIDE), &persist_conf.orders[1], false, sizeof(persist_conf.orders[1]) },
    {"/orders/2", TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (2 * TEST_ORDERS_INDEX_STRIDE), &persist_conf.orders[2], false, sizeof(persist_conf.orders[2]) },
};

static int _be_ram_load(const struct conf_backend *be,
                        conf_key_buf_t *key, void *val, size_t *size,
                        size_t offset, conf_backend_flags_t *flg)
{
    (void)be;
    (void)flg;

    assert(offset + *size <= CONFIG_CONFIGURATION_BACKEND_RAM_BUF_SIZE);
    for (unsigned i = 0; i < ARRAY_SIZE(_kv); i++) {
        if ((!strcmp(configuration_key_buf(key), _kv[i].key) ||
            _kv[i].sid == key->sid) && !_kv[i].deleted) {

            size_t rd = *size;
            if (!offset) {
                if (*size < _kv[i].len) {
                    *size = _kv[i].len;
                    *flg |= CONF_BACKEND_FLAG_MORE;
                }
                else {
                    rd = _kv[i].len;
                    *size = rd;
                }
            }
            memcpy(val, (const uint8_t *)_kv[i].value + offset, rd);
            return 0;
        }
    }
    return -ENOENT;
}

static int _be_ram_store(const struct conf_backend *be,
                         conf_key_buf_t *key, const void *val, size_t *size,
                         size_t offset, conf_backend_flags_t *flg)
{
    (void)be;
    (void)flg;

    assert(offset + *size <= CONFIG_CONFIGURATION_BACKEND_RAM_BUF_SIZE);
    for (unsigned i = 0; i < ARRAY_SIZE(_kv); i++) {
        if (!strcmp(configuration_key_buf(key), _kv[i].key) ||
            _kv[i].sid == key->sid) {
            memcpy((uint8_t *)_kv[i].value + offset, val, *size);
            if (*flg & CONF_BACKEND_FLAG_FINISH) {
                _kv[i].deleted = false;
                _kv[i].len = offset + *size;
                *flg &= ~CONF_BACKEND_FLAG_FINISH;
            }
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
        if (!strcmp(configuration_key_buf(key), _kv[i].key) ||
            _kv[i].sid == key->sid) {
            _kv[i].deleted = true;
            return 0;
        }
    }
    return -ENOENT;
}

const conf_backend_ops_t conf_backend_ram_ops = {
    .be_load = _be_ram_load,
    .be_store = _be_ram_store,
    .be_delete = _be_ram_delete,
};
