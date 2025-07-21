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
 * @file
 * @author  Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "compiler_hints.h"
#include "configuration.h"
#include "container.h"
#include "embUnit.h"

#include "configuration_backend_ram.h"
#include "configuration_backend_flashdb.h"
#include "include/persist_types.h"
#include "persist_types.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#if defined(TEST_CONFIGURATION_BACKEND_RAM)
extern struct configuration persist_conf;
#else
static struct configuration persist_conf; /* dummy */
#endif

#if IS_USED(MODULE_CONFIGURATION_STRINGS)
#define _TEST_ASSERT_KEY_STREQ(k, s)    TEST_ASSERT(!strcmp(configuration_key_buf(k), s))
#else
#define _TEST_ASSERT_KEY_STREQ(k, s)
#endif

static CONF_KEY(key_buf, UINT64_MAX, 40);

struct configuration _conf = {
    .food = {
        .bread = {
            .white = {"2.00"},
            .whole_grain = {"2.20"}
        },
        .cake = {
            .cheesecake = {"2.99"},
            .donut = {"2.00"},
        },
    },
    .drinks = {
        .coffee = {"1.00"},
        .tea = {"1.00"},
        .cocoa = {"1.50"},
    }
};

static inline bool _verify_price(const char *price)
{
    const char *start = price;
    char *end = NULL;
    long num = strtoll(start, &end, 10);
    if (start == end || (errno == ERANGE && (num == LONG_MAX || num == LONG_MIN))) {
        return false;
    }
    if (*end && *end != '.') {
        return false;
    }
    if (*end) {
        start = end + 1;
        end = NULL;
        num = strtoll(start, &end, 10);
        if (start == end || (errno == ERANGE && (num == LONG_MAX || num == LONG_MIN))) {
            return false;
        }
    }
    return true;
}

int _any_food_verify(const struct conf_handler *handler,
                     conf_key_buf_t *key,
                     const food_t *food)
{
    assert(handler); (void)handler;
    assert(key);
    assert(food);

    if (!_verify_price(food->price)) {
        return -EINVAL;
    }

    return 0;
}

int _any_food_apply(const struct conf_handler *handler,
                    conf_key_buf_t *key,
                    const food_t *food)
{
    assert(handler); (void)handler;
    assert(key);
    assert(food);

    printf("test configuration: Applying %s to %s\n",
           food->price, configuration_key_buf(key));

    return 0;
}

int _any_drinks_verify(const struct conf_handler *handler,
                       conf_key_buf_t *key,
                       const drink_t *drink)
{
    assert(handler); (void)handler;
    assert(key);
    assert(drink);

    if (!_verify_price(drink->price)) {
        return -EINVAL;
    }

    return 0;
}

int _any_drinks_apply(const struct conf_handler *handler,
                      conf_key_buf_t *key,
                      const drink_t *drink)
{
    assert(handler); (void)handler;
    assert(key);
    assert(drink);

    printf("test configuration: Applying %s to %s\n",
           drink->price, configuration_key_buf(key));

    return 0;
}

static int _food_white_verify(const conf_handler_t *handler,
                              conf_key_buf_t *key)
{
    return _any_food_verify(handler, key, &_conf.food.bread.white);
}

static int _food_white_apply(const conf_handler_t *handler,
                             conf_key_buf_t *key)
{
    return _any_food_apply(handler, key, &_conf.food.bread.white);
}

static const conf_handler_data_ops_t _bread_white_data_ops = {
    .verify = _food_white_verify,
    .apply = _food_white_apply,
};

static int _food_whole_grain_verify(const conf_handler_t *handler,
                                    conf_key_buf_t *key)
{
    return _any_food_verify(handler, key, &_conf.food.bread.whole_grain);
}

static int _food_whole_grain_apply(const conf_handler_t *handler,
                                  conf_key_buf_t *key)
{
    return _any_food_apply(handler, key, &_conf.food.bread.whole_grain);
}

static const conf_handler_data_ops_t _bread_whole_grain_data_ops = {
    .verify = _food_whole_grain_verify,
    .apply = _food_whole_grain_apply,
};

static int _food_cheesecake_verify(const conf_handler_t *handler,
                                   conf_key_buf_t *key)
{
    return _any_food_verify(handler, key, &_conf.food.cake.cheesecake);
}

static int _food_cheesecake_apply(const conf_handler_t *handler,
                                  conf_key_buf_t *key)
{
    return _any_food_apply(handler, key, &_conf.food.cake.cheesecake);
}

static const conf_handler_data_ops_t _cake_cheesecake_data_ops = {
    .verify = _food_cheesecake_verify,
    .apply = _food_cheesecake_apply,
};

static int _food_donut_verify(const conf_handler_t *handler,
                                conf_key_buf_t *key)
{
    return _any_food_verify(handler, key, &_conf.food.cake.donut);
}

static int _food_donut_apply(const conf_handler_t *handler,
                               conf_key_buf_t *key)
{
    return _any_food_apply(handler, key, &_conf.food.cake.donut);
}

static const conf_handler_data_ops_t _cake_donut_data_ops = {
    .verify = _food_donut_verify,
    .apply = _food_donut_apply,
};

static int _drink_coffee_verify(const conf_handler_t *handler,
                                conf_key_buf_t *key)
{
    return _any_drinks_verify(handler, key, &_conf.drinks.coffee);
}

static int _drink_coffee_apply(const conf_handler_t *handler,
                               conf_key_buf_t *key)
{
    return _any_drinks_apply(handler, key, &_conf.drinks.coffee);
}

static const conf_handler_data_ops_t _drinks_coffee_data_ops = {
    .verify = _drink_coffee_verify,
    .apply = _drink_coffee_apply,
};

static int _drink_tea_verify(const conf_handler_t *handler,
                             conf_key_buf_t *key)
{
    return _any_drinks_verify(handler, key, &_conf.drinks.tea);
}

static int _drink_tea_apply(const conf_handler_t *handler,
                            conf_key_buf_t *key)
{
    return _any_drinks_apply(handler, key, &_conf.drinks.tea);
}

static const conf_handler_data_ops_t _drinks_tea_data_ops = {
    .verify = _drink_tea_verify,
    .apply = _drink_tea_apply,
};

static int _drink_cocoa_verify(const conf_handler_t *handler,
                               conf_key_buf_t *key)
{
    return _any_drinks_verify(handler, key, &_conf.drinks.cocoa);
}

static int _drink_cocoa_apply(const conf_handler_t *handler,
                              conf_key_buf_t *key)
{
    return _any_drinks_apply(handler, key, &_conf.drinks.cocoa);
}

static const conf_handler_data_ops_t _drinks_cocoa_data_ops = {
    .verify = _drink_cocoa_verify,
    .apply = _drink_cocoa_apply,
};

static int _orders_verify(const conf_handler_t *handler,
                          conf_key_buf_t *key)
{
    (void)handler; (void)key;
    printf("test configuration: Verifying %s\n", configuration_key_buf(key));
    return 0;
}

static int _orders_apply(const conf_handler_t *handler,
                         conf_key_buf_t *key)
{
    (void)handler; (void)key;
    printf("test configuration: Applying %s\n", configuration_key_buf(key));
    return 0;
}

static const conf_handler_data_ops_t _orders_data_ops = {
    .verify = _orders_verify,
    .apply = _orders_apply,
};

static CONF_HANDLER_NODE_ID(_products_food_conf_handler_id,
                            TEST_FOOD_LOWER_SID, TEST_FOOD_UPPER_SID,
                            "food");

static CONF_HANDLER(_products_food_conf_handler,
                    &_products_food_conf_handler_id,
                    NULL,
                    &configuration_default_backend_ops,
                    &configuration_default_backend_data_ops,
                    0,
                    sizeof(_conf.food),
                    &_conf.food);

static CONF_HANDLER_NODE_ID(_products_bread_handler_node_id,
                            TEST_FOOD_BREAD_LOWER_SID, TEST_FOOD_BREAD_UPPER_SID,
                            "bread");

static CONF_HANDLER(_products_bread_handler,
                     &_products_bread_handler_node_id,
                     &_bread_white_data_ops,
                     &configuration_default_backend_ops,
                     &configuration_default_backend_data_ops,
                     0,
                     sizeof(_conf.food.bread),
                     &_conf.food.bread);

static CONF_HANDLER_ID(_products_bread_white_handler_node_id,
                       TEST_FOOD_BREAD_WHITE_SID,
                       "white");

static CONF_PRIMITIVE_HANDLER(_products_bread_white_handler,
                              &_products_bread_white_handler_node_id,
                              &_bread_white_data_ops,
                              &configuration_default_backend_ops,
                              &configuration_default_backend_data_ops,
                              CONF_PRIM_TYPE_TSTR,
                              sizeof(_conf.food.bread.white),
                              &_conf.food.bread.white);

static CONF_HANDLER_ID(_products_bread_whole_grain_handler_node_id,
                       TEST_FOOD_BREAD_WHOLE_GRAIN_SID,
                       "whole_grain");

static CONF_PRIMITIVE_HANDLER(_products_bread_whole_grain_handler,
                              &_products_bread_whole_grain_handler_node_id,
                              &_bread_whole_grain_data_ops,
                              &configuration_default_backend_ops,
                              &configuration_default_backend_data_ops,
                              CONF_PRIM_TYPE_TSTR,
                              sizeof(_conf.food.bread.whole_grain),
                              &_conf.food.bread.whole_grain);

static CONF_HANDLER_NODE_ID(_products_cake_handler_node_id,
                            TEST_FOOD_CAKE_LOWER_SID, TEST_FOOD_CAKE_UPPER_SID,
                            "cake");

static CONF_HANDLER(_products_cake_handler,
                    &_products_cake_handler_node_id,
                    NULL,
                    &configuration_default_backend_ops,
                    &configuration_default_backend_data_ops,
                    0,
                    sizeof(_conf.food.cake),
                    &_conf.food.cake);

static CONF_HANDLER_ID(_products_cake_cheesecake_handler_node_id,
                       TEST_FOOD_CAKE_CHEESECAKE_SID,
                       "cheesecake");

static CONF_PRIMITIVE_HANDLER(_products_cake_cheesecake_handler,
                              &_products_cake_cheesecake_handler_node_id,
                              &_cake_cheesecake_data_ops,
                              &configuration_default_backend_ops,
                              &configuration_default_backend_data_ops,
                              CONF_PRIM_TYPE_TSTR,
                              sizeof(_conf.food.cake.cheesecake),
                              &_conf.food.cake.cheesecake);

static CONF_HANDLER_ID(_products_cake_donut_handler_node_id,
                       TEST_FOOD_CAKE_DONUT_SID,
                       "donut");

static CONF_PRIMITIVE_HANDLER(_products_cake_donut_handler,
                              &_products_cake_donut_handler_node_id,
                              &_cake_donut_data_ops,
                              &configuration_default_backend_ops,
                              &configuration_default_backend_data_ops,
                              CONF_PRIM_TYPE_TSTR,
                              sizeof(_conf.food.cake.donut),
                              &_conf.food.cake.donut);

static CONF_HANDLER_NODE_ID(_products_drinks_conf_handler_node_id,
                            TEST_DRINKS_LOWER_SID, TEST_DRINKS_UPPER_SID,
                            "drinks");

static CONF_HANDLER(_products_drinks_conf_handler,
                    &_products_drinks_conf_handler_node_id,
                    NULL,
                    &configuration_default_backend_ops,
                    &configuration_default_backend_data_ops,
                    0,
                    sizeof(_conf.drinks),
                    &_conf.drinks);

static CONF_HANDLER_ID(_products_drinks_coffee_conf_handler_node_id,
                       TEST_DRINKS_COFFEE_SID,
                       "coffee");

static CONF_PRIMITIVE_HANDLER(_products_drinks_coffee_conf_handler,
                              &_products_drinks_coffee_conf_handler_node_id,
                              &_drinks_coffee_data_ops,
                              &configuration_default_backend_ops,
                              &configuration_default_backend_data_ops,
                              CONF_PRIM_TYPE_TSTR,
                              sizeof(_conf.drinks.coffee),
                              &_conf.drinks.coffee);

static CONF_HANDLER_ID(_products_drinks_tea_conf_handler_node_id,
                       TEST_DRINKS_TEA_SID,
                       "tea");

static CONF_PRIMITIVE_HANDLER(_products_drinks_tea_conf_handler,
                              &_products_drinks_tea_conf_handler_node_id,
                              &_drinks_tea_data_ops,
                              &configuration_default_backend_ops,
                              &configuration_default_backend_data_ops,
                              CONF_PRIM_TYPE_TSTR,
                              sizeof(_conf.drinks.tea),
                              &_conf.drinks.tea);

static CONF_HANDLER_ID(_products_drinks_cocoa_conf_handler_node_id,
                       TEST_DRINKS_COCOA_SID,
                       "cocoa");

static CONF_PRIMITIVE_HANDLER(_products_drinks_cocoa_conf_handler,
                              &_products_drinks_cocoa_conf_handler_node_id,
                              &_drinks_cocoa_data_ops,
                              &configuration_default_backend_ops,
                              &configuration_default_backend_data_ops,
                              CONF_PRIM_TYPE_TSTR,
                              sizeof(_conf.drinks.cocoa),
                              &_conf.drinks.cocoa);

static CONF_HANDLER_ARRAY_ID(_products_orders_conf_handler_id,
                             TEST_ORDERS_LOWER_SID, TEST_ORDERS_UPPER_SID, TEST_ORDERS_INDEX_STRIDE,
                             "orders");

static CONF_ARRAY_HANDLER(_products_orders_conf_handler,
                          &_products_orders_conf_handler_id,
                          &_orders_data_ops,
                          &configuration_default_backend_ops,
                          &configuration_default_backend_data_ops,
                          0,
                          sizeof(_conf.orders[0]),
                          &_conf.orders,
                          ARRAY_SIZE(_conf.orders));

static CONF_HANDLER_ARRAY_ID(_products_orders_items_conf_handler_id,
                            TEST_ORDERS_ITEMS_LOWER_SID, TEST_ORDERS_ITEMS_UPPER_SID, TEST_ORDERS_ITEMS_INDEX_STRIDE,
                            "items");

static CONF_ARRAY_HANDLER(_products_orders_items_conf_handler,
                          &_products_orders_items_conf_handler_id,
                          NULL,
                          &configuration_default_backend_ops,
                          &configuration_default_backend_data_ops,
                          0,
                          sizeof(_conf.orders[0].items[0]),
                          &_conf.orders[0].items,
                          ARRAY_SIZE(_conf.orders[0].items));

static CONF_HANDLER_ID(_products_orders_items_item_conf_handler_id,
                        TEST_ORDERS_ITEMS_ITEM_SID,
                        "item");

static CONF_PRIMITIVE_HANDLER(_products_orders_items_item_conf_handler,
                              &_products_orders_items_item_conf_handler_id,
                              NULL,
                              NULL,
                              &configuration_default_backend_data_ops,
                              CONF_PRIM_TYPE_TSTR,
                              sizeof(_conf.orders[0].items[0]),
                              &_conf.orders[0].items[0]);

#ifndef TEST_CONFIGURATION_BACKEND_OPS
#if defined(TEST_CONFIGURATION_BACKEND_RAM)
#define TEST_CONFIGURATION_BACKEND_OPS  &conf_backend_ram_ops
#elif defined(TEST_CONFIGURATION_BACKEND_FLASHDB_MTD)
#define TEST_CONFIGURATION_BACKEND_OPS  &conf_backend_flashdb_ops
#else
#define TEST_CONFIGURATION_BACKEND_OPS  NULL
#endif
#endif

static const CONF_BACKEND(_products_backend,
                          TEST_CONFIGURATION_BACKEND_OPS,
                          CONF_FMT_RAW
);

static void _init_backend(void)
{
    *configuration_get_src_backend(&_products_orders_conf_handler.handler) = &_products_backend;
    *configuration_get_dst_backend(&_products_orders_conf_handler.handler) = &_products_backend;
    *configuration_get_src_backend(&_products_orders_items_conf_handler.handler) = &_products_backend;
    *configuration_get_dst_backend(&_products_orders_items_conf_handler.handler) = &_products_backend;
    *configuration_get_src_backend(&_products_drinks_cocoa_conf_handler) = &_products_backend;
    *configuration_get_dst_backend(&_products_drinks_cocoa_conf_handler) = &_products_backend;
    *configuration_get_src_backend(&_products_drinks_tea_conf_handler) = &_products_backend;
    *configuration_get_dst_backend(&_products_drinks_tea_conf_handler) = &_products_backend;
    *configuration_get_src_backend(&_products_drinks_coffee_conf_handler) = &_products_backend;
    *configuration_get_dst_backend(&_products_drinks_coffee_conf_handler) = &_products_backend;
    *configuration_get_src_backend(&_products_cake_donut_handler) = &_products_backend;
    *configuration_get_dst_backend(&_products_cake_donut_handler) = &_products_backend;
    *configuration_get_src_backend(&_products_cake_cheesecake_handler) = &_products_backend;
    *configuration_get_dst_backend(&_products_cake_cheesecake_handler) = &_products_backend;
    *configuration_get_src_backend(&_products_bread_whole_grain_handler) = &_products_backend;
    *configuration_get_dst_backend(&_products_bread_whole_grain_handler) = &_products_backend;
    *configuration_get_src_backend(&_products_bread_white_handler) = &_products_backend;
    *configuration_get_dst_backend(&_products_bread_white_handler) = &_products_backend;
}

static void _setup(void)
{
    configuration_register(configuration_get_root(), &_products_food_conf_handler);
    configuration_register(configuration_get_root(), &_products_drinks_conf_handler);
    configuration_register(configuration_get_root(), &_products_orders_conf_handler.handler);
    configuration_register(&_products_food_conf_handler, &_products_bread_handler);
    configuration_register(&_products_food_conf_handler, &_products_cake_handler);
    configuration_register(&_products_bread_handler, &_products_bread_white_handler);
    configuration_register(&_products_bread_handler, &_products_bread_whole_grain_handler);
    configuration_register(&_products_cake_handler, &_products_cake_cheesecake_handler);
    configuration_register(&_products_cake_handler, &_products_cake_donut_handler);
    configuration_register(&_products_drinks_conf_handler, &_products_drinks_coffee_conf_handler);
    configuration_register(&_products_drinks_conf_handler, &_products_drinks_tea_conf_handler);
    configuration_register(&_products_drinks_conf_handler, &_products_drinks_cocoa_conf_handler);
    configuration_register(&_products_orders_conf_handler.handler,
                           &_products_orders_items_conf_handler.handler);
    configuration_register(&_products_orders_items_conf_handler.handler,
                           &_products_orders_items_item_conf_handler);
}

static void test_configuration_get(void)
{
    struct configuration conf = { 0 };
    size_t conf_size;

    conf_size = sizeof(conf.food);
    key_buf.sid = UINT64_MAX;
    TEST_ASSERT(configuration_get(&key_buf, &conf.food, &conf_size));

    memset(&conf, 0, sizeof(conf));
    conf_size = sizeof(conf.food);
    key_buf.sid = TEST_FOOD_LOWER_SID;
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.food, &_conf.food, conf_size));

    memset(&conf, 0, sizeof(conf));
    conf_size = sizeof(conf.food.bread);
    key_buf.sid = TEST_FOOD_BREAD_LOWER_SID;
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food.bread, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.food.bread, &_conf.food.bread, conf_size));

    memset(&conf, 0, sizeof(conf));
    conf_size = sizeof(conf.food.bread.white);
    key_buf.sid = TEST_FOOD_BREAD_WHITE_SID;
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food.bread.white, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread/white");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.food.bread.white, &_conf.food.bread.white, conf_size));

    memset(&conf, 0, sizeof(conf));
    conf_size = sizeof(conf.food.bread.whole_grain);
    key_buf.sid = TEST_FOOD_BREAD_WHOLE_GRAIN_SID;
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food.bread.whole_grain, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread/whole_grain");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.food.bread.whole_grain, &_conf.food.bread.whole_grain, conf_size));

    memset(&conf, 0, sizeof(conf));
    conf_size = sizeof(conf.food.cake);
    key_buf.sid = TEST_FOOD_CAKE_LOWER_SID;
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food.cake, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/cake");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.food.cake, &_conf.food.cake, conf_size));

    memset(&conf, 0, sizeof(conf));
    conf_size = sizeof(conf.food.cake.cheesecake);
    key_buf.sid = TEST_FOOD_CAKE_CHEESECAKE_SID;
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food.cake.cheesecake, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/cake/cheesecake");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.food.cake.cheesecake, &_conf.food.cake.cheesecake, conf_size));

    memset(&conf, 0, sizeof(conf));
    conf_size = sizeof(conf.food.cake.donut);
    key_buf.sid = TEST_FOOD_CAKE_DONUT_SID;
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food.cake.donut, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/cake/donut");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.food.cake.donut, &_conf.food.cake.donut, conf_size));

    conf_size = sizeof(conf.drinks);
    key_buf.sid = TEST_DRINKS_LOWER_SID;
    TEST_ASSERT(!configuration_get(&key_buf, &conf.drinks, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.drinks, &_conf.drinks, conf_size));
}

static void test_configuration_set(void)
{
    struct configuration new_conf = {
        .food = {
            .bread = {
                {"2.50"},
                {"2.70"}
            },
            .cake = {
                {"3.50"},
                {"2.50"},
            }
        },
        .drinks = {
            {"1.50"},
            {"1.50"},
            {"2.00"},
        }
    };
    size_t conf_size;

    struct configuration conf_backup = _conf;

    conf_size = sizeof(new_conf.food);
    key_buf.sid = UINT64_MAX;
    TEST_ASSERT(configuration_set(&key_buf, &new_conf.food, &conf_size));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.food.bread.white);
    key_buf.sid = TEST_FOOD_BREAD_WHITE_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food.bread.white, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread/white");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food.bread.white, &_conf.food.bread.white, sizeof(_conf.food.bread.white)));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.food.bread.whole_grain);
    key_buf.sid = TEST_FOOD_BREAD_WHOLE_GRAIN_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food.bread.whole_grain, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread/whole_grain");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food.bread.whole_grain, &_conf.food.bread.whole_grain, sizeof(_conf.food.bread.whole_grain)));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.food.bread);
    key_buf.sid = TEST_FOOD_BREAD_LOWER_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food.bread, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food.bread, &_conf.food.bread, sizeof(_conf.food.bread)));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.food.cake.cheesecake);
    key_buf.sid = TEST_FOOD_CAKE_CHEESECAKE_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food.cake.cheesecake, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/cake/cheesecake");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food.cake.cheesecake, &_conf.food.cake.cheesecake, sizeof(_conf.food.cake.cheesecake)));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.food.cake.donut);
    key_buf.sid = TEST_FOOD_CAKE_DONUT_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food.cake.donut, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/cake/donut");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food.cake.donut, &_conf.food.cake.donut, sizeof(_conf.food.cake.donut)));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.food.cake);
    key_buf.sid = TEST_FOOD_CAKE_LOWER_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food.cake, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/cake");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food.cake, &_conf.food.cake, sizeof(_conf.food.cake)));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.food);
    key_buf.sid = TEST_FOOD_LOWER_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food, &_conf.food, sizeof(_conf.food)));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.drinks);
    key_buf.sid = TEST_DRINKS_LOWER_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.drinks, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.drinks, &_conf.drinks, sizeof(_conf.drinks)));

    _conf = conf_backup;
}

MAYBE_UNUSED
static void test_configuration_import(void)
{
    struct configuration conf_backup = _conf;

    key_buf.sid = UINT64_MAX;
    TEST_ASSERT(configuration_import(&key_buf));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_FOOD_LOWER_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");
    TEST_ASSERT(!memcmp(&persist_conf.food, &_conf.food, sizeof(_conf.food)));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_FOOD_BREAD_LOWER_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread");
    TEST_ASSERT(!memcmp(&persist_conf.food.bread, &_conf.food.bread, sizeof(_conf.food.bread)));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_FOOD_BREAD_WHITE_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread/white");
    TEST_ASSERT(!memcmp(&persist_conf.food.bread.white, &_conf.food.bread.white, sizeof(_conf.food.bread.white)));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_FOOD_BREAD_WHOLE_GRAIN_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread/whole_grain");
    TEST_ASSERT(!memcmp(&persist_conf.food.bread.whole_grain, &_conf.food.bread.whole_grain, sizeof(_conf.food.bread.whole_grain)));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_FOOD_CAKE_LOWER_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/cake");
    TEST_ASSERT(!memcmp(&persist_conf.food.cake, &_conf.food.cake, sizeof(_conf.food.cake)));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_FOOD_CAKE_CHEESECAKE_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/cake/cheesecake");
    TEST_ASSERT(!memcmp(&persist_conf.food.cake.cheesecake, &_conf.food.cake.cheesecake, sizeof(_conf.food.cake.cheesecake)));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_FOOD_CAKE_DONUT_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/cake/donut");
    TEST_ASSERT(!memcmp(&persist_conf.food.cake.donut, &_conf.food.cake.donut, sizeof(_conf.food.cake.donut)));

    key_buf.sid = TEST_DRINKS_LOWER_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");
    TEST_ASSERT(!memcmp(&persist_conf.drinks, &_conf.drinks, sizeof(_conf.drinks)));

    _conf = conf_backup;
}

MAYBE_UNUSED
static void test_configuration_export(void)
{
    struct configuration new_conf = {
        .food = {
            .bread = {
                {"2.50"},
                {"2.70"}
            },
            .cake = {
                {"3.50"},
                {"2.50"},
            }
        },
        .drinks = {
            {"1.50"},
            {"1.50"},
            {"2.00"},
        }
    };
    struct configuration conf_backup = _conf;
    struct configuration persist_conf_backup = persist_conf;

    _conf = new_conf;

    key_buf.sid = UINT64_MAX;
    TEST_ASSERT(configuration_export(&key_buf));
    memset(&persist_conf, 0, sizeof(persist_conf));

    key_buf.sid = TEST_FOOD_BREAD_WHITE_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread/white");
    TEST_ASSERT(!memcmp(&persist_conf.food.bread.white, &_conf.food.bread.white, sizeof(_conf.food.bread.white)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_FOOD_BREAD_WHOLE_GRAIN_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread/whole_grain");
    TEST_ASSERT(!memcmp(&new_conf.food.bread.whole_grain, &_conf.food.bread.whole_grain, sizeof(_conf.food.bread.whole_grain)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_FOOD_BREAD_LOWER_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread");
    TEST_ASSERT(!memcmp(&new_conf.food.bread, &_conf.food.bread, sizeof(_conf.food.bread)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_FOOD_CAKE_CHEESECAKE_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/cake/cheesecake");
    TEST_ASSERT(!memcmp(&new_conf.food.cake.cheesecake, &_conf.food.cake.cheesecake, sizeof(_conf.food.cake.cheesecake)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_FOOD_CAKE_DONUT_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/cake/donut");
    TEST_ASSERT(!memcmp(&new_conf.food.cake.donut, &_conf.food.cake.donut, sizeof(_conf.food.cake.donut)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_FOOD_CAKE_LOWER_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/cake");
    TEST_ASSERT(!memcmp(&new_conf.food.cake, &_conf.food.cake, sizeof(_conf.food.cake)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_FOOD_LOWER_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");
    TEST_ASSERT(!memcmp(&new_conf.food, &_conf.food, sizeof(_conf.food)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_DRINKS_LOWER_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");
    TEST_ASSERT(!memcmp(&new_conf.drinks, &_conf.drinks, sizeof(_conf.drinks)));

    _conf = conf_backup;
    persist_conf = persist_conf_backup;
}

static void test_configuration_import_modify_export_import(void)
{
    struct configuration new_conf = {
        .food = {
            .bread = {
                {"2.50"},
                {"2.70"}
            },
            .cake = {
                {"3.50"},
                {"2.50"},
            }
        },
        .drinks = {
            {"1.50"},
            {"1.50"},
            {"2.00"},
        }
    };
    struct configuration conf_backup = _conf;
    struct configuration persist_conf_backup = persist_conf;
    size_t conf_size;

    memset(&_conf, 0, sizeof(_conf));

    key_buf.sid = TEST_FOOD_LOWER_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");

    key_buf.sid = TEST_DRINKS_LOWER_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");

    conf_size = sizeof(new_conf.food);
    key_buf.sid = TEST_FOOD_LOWER_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");
    TEST_ASSERT(conf_size == 0);

    conf_size = sizeof(new_conf.drinks);
    key_buf.sid = TEST_DRINKS_LOWER_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.drinks, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");
    TEST_ASSERT(conf_size == 0);

    key_buf.sid = TEST_FOOD_LOWER_SID;
    configuration_delete(&key_buf);
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");

    key_buf.sid = TEST_DRINKS_LOWER_SID;
    configuration_delete(&key_buf);
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");

    key_buf.sid = TEST_FOOD_LOWER_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");

    key_buf.sid = TEST_DRINKS_LOWER_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");

    memset(&_conf, 0, sizeof(_conf));

    key_buf.sid = TEST_FOOD_LOWER_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");
    TEST_ASSERT(!memcmp(&_conf.food, &new_conf.food, sizeof(_conf.food)));

    key_buf.sid = TEST_DRINKS_LOWER_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");
    TEST_ASSERT(!memcmp(&_conf.drinks, &new_conf.drinks, sizeof(new_conf.drinks)));

    _conf = conf_backup;
    persist_conf = persist_conf_backup;
}

static void test_configuration_delete(void)
{
    struct configuration new_conf = {
        .food = {
            .bread = {
                {"2.50"},
                {"2.70"}
            },
            .cake = {
                {"3.50"},
                {"2.50"},
            }
        },
        .drinks = {
            {"1.50"},
            {"1.50"},
            {"2.00"},
        }
    };
    struct configuration conf_backup = _conf;
    struct configuration persist_conf_backup = persist_conf;
    size_t conf_size;

    key_buf.sid = UINT64_MAX;
    TEST_ASSERT(configuration_delete(&key_buf));

    key_buf.sid = TEST_FOOD_BREAD_LOWER_SID;
    TEST_ASSERT(!configuration_delete(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread");

    conf_size = sizeof(new_conf.food);
    key_buf.sid = TEST_FOOD_LOWER_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");
    TEST_ASSERT(conf_size == 0);

    conf_size = sizeof(new_conf.drinks);
    key_buf.sid = TEST_DRINKS_LOWER_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.drinks, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");
    TEST_ASSERT(conf_size == 0);

    key_buf.sid = TEST_FOOD_LOWER_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");

    key_buf.sid = TEST_DRINKS_LOWER_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");

    key_buf.sid = TEST_FOOD_LOWER_SID;
    TEST_ASSERT(!configuration_delete(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");

    key_buf.sid = TEST_FOOD_BREAD_WHITE_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread/white");

    key_buf.sid = TEST_DRINKS_LOWER_SID;
    TEST_ASSERT(!configuration_delete(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");

    key_buf.sid = TEST_DRINKS_COFFEE_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks/coffee");

    _conf = conf_backup;
    persist_conf = persist_conf_backup;
}

static void test_configuration_verify_apply(void)
{
    struct configuration new_conf = {
        .food = {
            .bread = {
                .white = {"sale"},
                .whole_grain = {"2.70"}
            },
            .cake = {
                .cheesecake = {"3.50"},
                .donut = {"2.50"},
            }
        },
        .drinks = {
            .coffee = {"free"},
            .tea = {"1.50"},
            .cocoa = {"2.00"},
        }
    };
    size_t conf_size;

    struct configuration conf_backup = _conf;

    key_buf.sid = TEST_FOOD_LOWER_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");

    conf_size = sizeof(new_conf.food.bread.white);
    key_buf.sid = TEST_FOOD_BREAD_WHITE_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food.bread.white, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread/white");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food.bread.white, &_conf.food.bread.white, sizeof(_conf.food.bread.white)));

    key_buf.sid = TEST_FOOD_BREAD_LOWER_SID;
    TEST_ASSERT(configuration_verify(&key_buf, false));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread");
    TEST_ASSERT(!configuration_verify(&key_buf, true));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread");
    TEST_ASSERT(!configuration_apply(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food/bread");

    key_buf.sid = TEST_DRINKS_LOWER_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");

    conf_size = sizeof(new_conf.drinks.coffee);
    key_buf.sid = TEST_DRINKS_COFFEE_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.drinks.coffee, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks/coffee");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.drinks.coffee, &_conf.drinks.coffee, sizeof(_conf.drinks.coffee)));

    TEST_ASSERT(configuration_verify(&key_buf, false));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks/coffee");
    TEST_ASSERT(!configuration_verify(&key_buf, true));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks/coffee");
    TEST_ASSERT(!configuration_apply(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks/coffee");

    _conf = conf_backup;
}

static void test_configuration_array_set(void)
{

    order_t orders[TEST_CONFIGURATION_MAX_ORDERS] = {
        {
            .items = {
                { .item = "donut", },
                { .item = "coffee", },
            },
        },
        {
            .items = {
                { .item = "cheesecake", },
                { .item = "coffee", },
            },
        },
        {
            .items = {
                { .item = "coffee", },
                { .item = "coffee", },
            },
        },
    };

    struct configuration conf_backup = _conf;

    size_t conf_size = sizeof(orders);
    key_buf.sid = TEST_ORDERS_LOWER_SID;
    TEST_ASSERT(!configuration_set(&key_buf, orders, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&_conf.orders, &orders, sizeof(orders)));

    conf_size = sizeof(orders[0]);
    key_buf.sid = CONFIG_TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (3 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(configuration_set(&key_buf, orders, &conf_size));

    conf_size = sizeof(orders[0]);
    key_buf.sid = CONFIG_TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_set(&key_buf, NULL, NULL));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0");
    TEST_ASSERT(!memcmp(&_conf.orders[0], &((order_t){0}), sizeof(orders[0])));

    conf_size = sizeof(orders[1]);
    key_buf.sid = CONFIG_TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (1 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_set(&key_buf, NULL, NULL));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/1");
    TEST_ASSERT(!memcmp(&_conf.orders[1], &((order_t){0}), sizeof(orders[1])));

    conf_size = sizeof(orders[2]);
    key_buf.sid = CONFIG_TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (2 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_set(&key_buf, NULL, NULL));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/2");
    TEST_ASSERT(!memcmp(&_conf.orders[0], &((order_t){0}), sizeof(orders[2])));


    order_t order_item_0 = {
        .items = { {"juice"}, {"tea"} },
    };

    conf_size = sizeof(order_item_0.items[0]);
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + TEST_ORDERS_ITEMS_INDEX_LOWER_SID + (0 * TEST_ORDERS_ITEMS_INDEX_STRIDE)
                                                                                  + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_set(&key_buf, &order_item_0.items[0], &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0/items/0");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&_conf.orders[0].items[0], &order_item_0.items[0], sizeof(order_item_0.items[0])));

    conf_size = sizeof(order_item_0.items[1]);
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + TEST_ORDERS_ITEMS_INDEX_LOWER_SID + (1 * TEST_ORDERS_ITEMS_INDEX_STRIDE)
                                                                                  + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_set(&key_buf, &order_item_0.items[1], &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0/items/1");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&_conf.orders[0].items[1], &order_item_0.items[1], sizeof(order_item_0.items[1])));

    order_t order_item_1 = {
        .items = { {"fish"}, {"chips"} },
    };

    conf_size = sizeof(order_item_1);
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + (2 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_set(&key_buf, &order_item_1.items, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/2/items");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&_conf.orders[2].items, &order_item_1.items, sizeof(order_item_1.items)));

    conf_size = sizeof(order_item_1);
    key_buf.sid = UINT64_MAX;
    TEST_ASSERT(configuration_set(&key_buf, &order_item_1.items, &conf_size));

    _conf = conf_backup;
}

static void test_configuration_array_get(void)
{
    struct configuration conf = { 0 };
    size_t conf_size;

    conf_size = sizeof(conf.orders[0]);
    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (3 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(configuration_get(&key_buf, &conf.orders[0], &conf_size));

    conf_size = sizeof(conf.orders);
    key_buf.sid = UINT64_MAX;
    TEST_ASSERT(configuration_get(&key_buf, &conf.orders, &conf_size));

    conf_size = sizeof(conf.orders[0].items[0]);
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + TEST_ORDERS_ITEMS_INDEX_LOWER_SID + (0 * TEST_ORDERS_ITEMS_INDEX_STRIDE)
                                                                                  + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_get(&key_buf, &conf.orders[0].items[0], &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0/items/0");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.orders[0].items[0], &_conf.orders[0].items[0], conf_size));

    conf_size = sizeof(conf.orders[0].items[1]);
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + TEST_ORDERS_ITEMS_INDEX_LOWER_SID + (1 * TEST_ORDERS_ITEMS_INDEX_STRIDE) +
                                                                                    (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_get(&key_buf, &conf.orders[0].items[1], &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0/items/1");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.orders[0].items[1], &_conf.orders[0].items[1], conf_size));

    conf_size = sizeof(conf.orders[1].items[0]);
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + TEST_ORDERS_ITEMS_INDEX_LOWER_SID + (0 * TEST_ORDERS_ITEMS_INDEX_STRIDE)
                                                                                  + (1 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_get(&key_buf, &conf.orders[1].items[0], &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/1/items/0");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.orders[1].items[0], &_conf.orders[1].items[0], conf_size));

    conf_size = sizeof(conf.orders[1].items[1]);
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + TEST_ORDERS_ITEMS_INDEX_LOWER_SID + (1 * TEST_ORDERS_ITEMS_INDEX_STRIDE)
                                                                                  + (1 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_get(&key_buf, &conf.orders[1].items[1], &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/1/items/1");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.orders[1].items[1], &_conf.orders[1].items[1], conf_size));

    conf_size = sizeof(conf.orders[2].items[0]);
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + TEST_ORDERS_ITEMS_INDEX_LOWER_SID + (0 * TEST_ORDERS_ITEMS_INDEX_STRIDE) +
                                                                                  + (2 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_get(&key_buf, &conf.orders[2].items[0], &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/2/items/0");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.orders[2].items[0], &_conf.orders[2].items[0], conf_size));

    conf_size = sizeof(conf.orders[2].items[1]);
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + TEST_ORDERS_ITEMS_INDEX_LOWER_SID + (1 * TEST_ORDERS_ITEMS_INDEX_STRIDE)
                                                                                  + (2 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_get(&key_buf, &conf.orders[2].items[1], &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/2/items/1");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.orders[2].items[1], &_conf.orders[2].items[1], conf_size));
}

MAYBE_UNUSED
static void test_configuration_array_import(void)
{
    struct configuration conf_backup = _conf;

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_ORDERS_LOWER_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!memcmp(&_conf.orders, &persist_conf.orders, sizeof(_conf.orders)));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = UINT64_MAX;
    TEST_ASSERT(configuration_import(&key_buf));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0");
    TEST_ASSERT(!memcmp(&_conf.orders[0], &persist_conf.orders[0],
                        sizeof(_conf.orders[0])));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (1 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/1");
    TEST_ASSERT(!memcmp(&_conf.orders[1], &persist_conf.orders[1],
                        sizeof(_conf.orders[1])));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (2 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/2");
    TEST_ASSERT(!memcmp(&_conf.orders[2], &persist_conf.orders[2],
                        sizeof(_conf.orders[2])));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0/items");
    /* /orders/0/items has no import handler */
    TEST_ASSERT(memcmp(&_conf.orders[0].items, &persist_conf.orders[0].items,
                       sizeof(_conf.orders[0].items)));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + (1 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/1/items");
    /* /orders/1/items has no import handler */
    TEST_ASSERT(memcmp(&_conf.orders[1].items, &persist_conf.orders[1].items,
                sizeof(_conf.orders[1].items)));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + (2 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/2/items");
    /* /orders/2/items has no import handler */
    TEST_ASSERT(memcmp(&_conf.orders[2].items, &persist_conf.orders[2].items,
                       sizeof(_conf.orders[2].items)));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + TEST_ORDERS_ITEMS_INDEX_LOWER_SID + (0 * TEST_ORDERS_ITEMS_INDEX_STRIDE)
                                                                                  + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0/items/0");
    /* /orders/0/items/0 has no import handler */
    TEST_ASSERT(memcmp(&_conf.orders[0].items[0], &persist_conf.orders[0].items[0],
                       sizeof(_conf.orders[0].items[0])));

    memset(&_conf, 0, sizeof(_conf));
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + TEST_ORDERS_ITEMS_INDEX_LOWER_SID + (1 * TEST_ORDERS_ITEMS_INDEX_STRIDE)
                                                                                  + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0/items/1");
    /* /orders/0/items/1 has no import handler */
    TEST_ASSERT(memcmp(&_conf.orders[0].items[1], &persist_conf.orders[0].items[1],
                       sizeof(_conf.orders[0].items[1])));

    _conf = conf_backup;
}

MAYBE_UNUSED
static void test_configuration_array_export(void)
{
    struct configuration new_conf = {
        .orders = {
            {
                .items = {
                    { .item = "donut", },
                    { .item = "coffee", },
                },
            },
            {
                .items = {
                    { .item = "cheesecake", },
                    { .item = "coffee", },
                },
            },
            {
                .items = {
                    { .item = "coffee", },
                    { .item = "coffee", },
                },
            },
        },
    };
    struct configuration conf_backup = _conf;
    struct configuration persist_conf_backup = persist_conf;

    _conf = new_conf;

    key_buf.sid = UINT64_MAX;
    TEST_ASSERT(configuration_export(&key_buf));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_ORDERS_LOWER_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders");
    TEST_ASSERT(!memcmp(&persist_conf.orders, &_conf.orders, sizeof(_conf.orders)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0");
    TEST_ASSERT(!memcmp(&persist_conf.orders[0], &_conf.orders[0], sizeof(_conf.orders[0])));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (1 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/1");
    TEST_ASSERT(!memcmp(&persist_conf.orders[1], &_conf.orders[1], sizeof(_conf.orders[1])));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (2 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/2");
    TEST_ASSERT(!memcmp(&persist_conf.orders[2], &_conf.orders[2], sizeof(_conf.orders[2])));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + (1 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/1/items");
    /* /orders/1/items has no export handler */
    TEST_ASSERT(memcmp(&persist_conf.orders[1].items, &_conf.orders[1].items,
                       sizeof(_conf.orders[1].items)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0/items");
    /* /orders/1/items has no export handler */
    TEST_ASSERT(memcmp(&persist_conf.orders[0].items, &_conf.orders[0].items,
                       sizeof(_conf.orders[0].items)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + (1 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/1/items");
    /* /orders/1/items has no export handler */
    TEST_ASSERT(memcmp(&persist_conf.orders[1].items, &_conf.orders[1].items,
                       sizeof(_conf.orders[1].items)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + (2 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/2/items");
    /* /orders/1/items has no export handler */
    TEST_ASSERT(memcmp(&persist_conf.orders[2].items, &_conf.orders[2].items,
                       sizeof(_conf.orders[2].items)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + TEST_ORDERS_ITEMS_INDEX_LOWER_SID + (0 * TEST_ORDERS_ITEMS_INDEX_STRIDE)
                                                                                  + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0/items/0");
    /* /orders/1/items has no export handler */
    TEST_ASSERT(memcmp(&persist_conf.orders[0].items[0], &_conf.orders[0].items[0],
                       sizeof(_conf.orders[0].items[0])));

    memset(&persist_conf, 0, sizeof(persist_conf));
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + TEST_ORDERS_ITEMS_INDEX_LOWER_SID + (1 * TEST_ORDERS_ITEMS_INDEX_STRIDE)
                                                                                  + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0/items/1");
    /* /orders/1/items has no export handler */
    TEST_ASSERT(memcmp(&persist_conf.orders[0].items[1], &_conf.orders[0].items[1],
                       sizeof(_conf.orders[0].items[1])));

    _conf = conf_backup;
    persist_conf = persist_conf_backup;
}

static void test_configuration_array_delete(void)
{
    struct configuration new_conf = {
        .orders = {
            {
                .items = {
                    { .item = "donut", },
                    { .item = "coffee", },
                },
            },
            {
                .items = {
                    { .item = "cheesecake", },
                    { .item = "coffee", },
                },
            },
            {
                .items = {
                    { .item = "coffee", },
                    { .item = "coffee", },
                },
            },
        },
    };
    struct configuration conf_backup = _conf;
    struct configuration persist_conf_backup = persist_conf;

    _conf = new_conf;

    key_buf.sid = UINT64_MAX;
    TEST_ASSERT(configuration_delete(&key_buf));

    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_delete(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0");
    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (1 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_delete(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/1");
    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (2 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_delete(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/2");

    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0");
    TEST_ASSERT(!memcmp(&_conf.orders, &new_conf.orders, sizeof(_conf.orders)));
    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (1 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/1");
    TEST_ASSERT(!memcmp(&_conf.orders, &new_conf.orders, sizeof(_conf.orders)));
    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (2 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/2");
    TEST_ASSERT(!memcmp(&_conf.orders, &new_conf.orders, sizeof(_conf.orders)));

    key_buf.sid = TEST_ORDERS_LOWER_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders");

    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_delete(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0/items");
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + (1 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_delete(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/1/items");
    key_buf.sid = TEST_ORDERS_ITEMS_LOWER_SID + (2 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_delete(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/2/items");

    key_buf.sid = TEST_ORDERS_LOWER_SID;
    TEST_ASSERT(!configuration_set(&key_buf, NULL, NULL));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders");

    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (0 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/0");
    TEST_ASSERT(!memcmp(&_conf.orders[0], &new_conf.orders[0], sizeof(_conf.orders[0])));
    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (1 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/1");
    TEST_ASSERT(!memcmp(&_conf.orders[1], &new_conf.orders[1], sizeof(_conf.orders[1])));
    key_buf.sid = TEST_ORDERS_LOWER_SID + TEST_ORDERS_INDEX_LOWER_SID + (2 * TEST_ORDERS_INDEX_STRIDE);
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders/2");
    TEST_ASSERT(!memcmp(&_conf.orders[2], &new_conf.orders[2], sizeof(_conf.orders[2])));

    persist_conf = persist_conf_backup;
    _conf = conf_backup;
}

static void test_configuration_array_import_modify_export_import(void)
{
    struct configuration new_conf = {
        .orders = {
            {
                .items = {
                    { .item = "donut", },
                    { .item = "coffee", },
                },
            },
            {
                .items = {
                    { .item = "cheesecake", },
                    { .item = "coffee", },
                },
            },
            {
                .items = {
                    { .item = "coffee", },
                    { .item = "coffee", },
                },
            },
        },
    };
    struct configuration conf_backup = _conf;
    struct configuration persist_conf_backup = persist_conf;
    size_t conf_size;

    memset(&_conf, 0, sizeof(_conf));

    key_buf.sid = TEST_ORDERS_LOWER_SID;
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders");

    conf_size = sizeof(new_conf.orders);
    key_buf.sid = TEST_ORDERS_LOWER_SID;
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.orders, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&_conf.orders, &new_conf.orders, sizeof(_conf.orders)));

    key_buf.sid = TEST_ORDERS_LOWER_SID;
    TEST_ASSERT(!configuration_delete(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders");

    key_buf.sid = TEST_ORDERS_LOWER_SID;
    TEST_ASSERT(!configuration_export(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders");
    TEST_ASSERT(!configuration_import(&key_buf));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders");
    TEST_ASSERT(!memcmp(&_conf.orders, &new_conf.orders, sizeof(_conf.orders)));

    _conf = conf_backup;
    persist_conf = persist_conf_backup;
}

static void test_configuration_encode_decode(void)
{
    struct configuration conf_backup = _conf;
    struct configuration persist_conf_backup = persist_conf;
    memset(&persist_conf, 0, sizeof(persist_conf));
    struct configuration conf = { 0 }; (void)conf;

    key_buf.sid = 0;
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!configuration_import(&key_buf));
    key_buf.sid = TEST_FOOD_LOWER_SID;
    size_t conf_size = sizeof(conf.food);
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/food");
    TEST_ASSERT(conf_size == 0);
    key_buf.sid = TEST_DRINKS_LOWER_SID;
    conf_size = sizeof(conf.drinks);
    TEST_ASSERT(!configuration_get(&key_buf, &conf.drinks, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/drinks");
    TEST_ASSERT(conf_size == 0);
    key_buf.sid = TEST_ORDERS_LOWER_SID;
    conf_size = sizeof(conf.orders);
    TEST_ASSERT(!configuration_get(&key_buf, &conf.orders, &conf_size));
    _TEST_ASSERT_KEY_STREQ(&key_buf, "/orders");
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf, &_conf, sizeof(conf)));

    _conf = conf_backup;
    persist_conf = persist_conf_backup;
}

Test* test_configuration(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_configuration_get),
        new_TestFixture(test_configuration_set),
#if defined(TEST_CONFIGURATION_BACKEND_RAM)
        /* we only know what is stored on the pseudo backend */
        new_TestFixture(test_configuration_import),
        new_TestFixture(test_configuration_export),
#endif
        new_TestFixture(test_configuration_delete),
        new_TestFixture(test_configuration_import_modify_export_import),
        new_TestFixture(test_configuration_verify_apply),
        new_TestFixture(test_configuration_array_set),
        new_TestFixture(test_configuration_array_get),
#if defined(TEST_CONFIGURATION_BACKEND_RAM)
        /* we only know what is stored on the pseudo backend */
        new_TestFixture(test_configuration_array_import),
        new_TestFixture(test_configuration_array_export),
#endif
        new_TestFixture(test_configuration_array_delete),
        new_TestFixture(test_configuration_array_import_modify_export_import),
        new_TestFixture(test_configuration_encode_decode),
    };

    EMB_UNIT_TESTCALLER(tests_configuration, NULL, NULL, fixtures);
    return (Test *)&tests_configuration;
}

static const CONF_BACKEND(_root_backend,
                          TEST_CONFIGURATION_BACKEND_OPS,
                          CONF_FMT_CBOR
);

/* override weak function */
void auto_init_configuration_root_backend_init(void)
{
    *configuration_get_src_backend(configuration_get_root()) = &_root_backend;
    *configuration_get_dst_backend(configuration_get_root()) = &_root_backend;
}

int main(void)
{
    _setup();
    _init_backend();

    TESTS_START();
    TESTS_RUN(test_configuration());
    TESTS_END();

    puts("Tests done");
}
