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
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "configuration.h"
#include "container.h"
#include "embUnit.h"

#include "configuration_backend_ram.h"
#include "configuration_backend_flashdb.h"
#include "persist_types.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#if defined(TEST_CONFIGURATION_BACKEND_RAM)
extern struct configuration persist_conf;
#else
static struct configuration persist_conf; /* dummy */
#endif

static CONF_KEY_T(40) key_buf
    = CONF_KEY_INITIALIZER(40, "");

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

    printf("test configuration: Applying %s to %s\n", food->price, key->buf);

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

    printf("test configuration: Applying %s to %s\n", drink->price, key->buf);

    return 0;
}

static int _food_white_set(const conf_handler_t *handler,
                           conf_key_buf_t *key, const void *val, size_t *size)
{
    return configuration_set_handler_default(handler, key, val, size);
}

static int _food_white_get(const conf_handler_t *handler,
                                 conf_key_buf_t *key, void *val, size_t *size)
{
    return configuration_get_handler_default(handler, key, val, size);
}

static int _food_white_import(const conf_handler_t *handler,
                              conf_key_buf_t *key)
{
    return configuration_import_handler_default(handler, key);
}

static int _food_white_export(const conf_handler_t *handler,
                              conf_key_buf_t *key)
{
    return configuration_export_handler_default(handler, key);
}

static int _food_white_delete(const conf_handler_t *handler,
                              conf_key_buf_t *key)
{
    return configuration_delete_handler_default(handler, key);
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

static const conf_handler_ops_t _bread_white_handler_ops = {
    .set = _food_white_set,
    .get = _food_white_get,
    .import = _food_white_import,
    .export = _food_white_export,
    .delete = _food_white_delete,
};

static const conf_handler_data_ops_t _bread_white_handler_data_ops = {
    .verify = _food_white_verify,
    .apply = _food_white_apply,
};

static int _food_whole_grain_set(const conf_handler_t *handler,
                                 conf_key_buf_t *key, const void *val, size_t *size)
{
    return configuration_set_handler_default(handler, key, val, size);
}

static int _food_whole_grain_get(const conf_handler_t *handler,
                                 conf_key_buf_t *key, void *val, size_t *size)
{
    return configuration_get_handler_default(handler, key, val, size);
}

static int _food_whole_grain_import(const conf_handler_t *handler,
                                    conf_key_buf_t *key)
{
    return configuration_import_handler_default(handler, key);
}

static int _food_whole_grain_export(const conf_handler_t *handler,
                                    conf_key_buf_t *key)
{
    return configuration_export_handler_default(handler, key);
}

static int _food_whole_grain_delete(const conf_handler_t *handler,
                                    conf_key_buf_t *key)
{
    return configuration_delete_handler_default(handler, key);
}

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

static const conf_handler_ops_t _bread_whole_grain_handler_ops = {
    .set = _food_whole_grain_set,
    .get = _food_whole_grain_get,
    .import = _food_whole_grain_import,
    .export = _food_whole_grain_export,
    .delete = _food_whole_grain_delete,
};

static const conf_handler_data_ops_t _bread_whole_grain_handler_data_ops = {
    .verify = _food_whole_grain_verify,
    .apply = _food_whole_grain_apply,
};

static int _food_cheesecake_set(const conf_handler_t *handler,
                                conf_key_buf_t *key, const void *val, size_t *size)
{
    return configuration_set_handler_default(handler, key, val, size);
}

static int _food_cheesecake_get(const conf_handler_t *handler,
                                conf_key_buf_t *key, void *val, size_t *size)
{
    return configuration_get_handler_default(handler, key, val, size);
}

static int _food_cheesecake_import(const conf_handler_t *handler,
                                   conf_key_buf_t *key)
{
    return configuration_import_handler_default(handler, key);
}

static int _food_cheesecake_export(const conf_handler_t *handler,
                                   conf_key_buf_t *key)
{
    return configuration_export_handler_default(handler, key);
}

static int _food_cheesecake_delete(const conf_handler_t *handler,
                                   conf_key_buf_t *key)
{
    return configuration_delete_handler_default(handler, key);
}

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

static const conf_handler_ops_t _cake_cheesecake_handler_ops = {
    .set = _food_cheesecake_set,
    .get = _food_cheesecake_get,
    .import = _food_cheesecake_import,
    .export = _food_cheesecake_export,
    .delete = _food_cheesecake_delete,
};

static const conf_handler_data_ops_t _cake_cheesecake_handler_data_ops = {
    .verify = _food_cheesecake_verify,
    .apply = _food_cheesecake_apply,
};

static int _food_donut_set(const conf_handler_t *handler,
                           conf_key_buf_t *key, const void *val, size_t *size)
{
    return configuration_set_handler_default(handler, key, val, size);
}

static int _food_donut_get(const conf_handler_t *handler,
                             conf_key_buf_t *key, void *val, size_t *size)
{
    return configuration_get_handler_default(handler, key, val, size);
}

static int _food_donut_import(const conf_handler_t *handler,
                              conf_key_buf_t *key)
{
    return configuration_import_handler_default(handler, key);
}

static int _food_donut_export(const conf_handler_t *handler,
                              conf_key_buf_t *key)
{
    return configuration_export_handler_default(handler, key);
}

static int _food_donut_delete(const conf_handler_t *handler,
                              conf_key_buf_t *key)
{
    return configuration_delete_handler_default(handler, key);
}

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

static const conf_handler_ops_t _cake_donut_handler_ops = {
    .set = _food_donut_set,
    .get = _food_donut_get,
    .import = _food_donut_import,
    .export = _food_donut_export,
    .delete = _food_donut_delete,
};

static const conf_handler_data_ops_t _cake_donut_handler_data_ops = {
    .verify = _food_donut_verify,
    .apply = _food_donut_apply,
};

static int _drink_coffee_set(const conf_handler_t *handler,
                             conf_key_buf_t *key, const void *val, size_t *size)
{
    return configuration_set_handler_default(handler, key, val, size);
}

static int _drink_coffee_get(const conf_handler_t *handler,
                             conf_key_buf_t *key, void *val, size_t *size)
{
    return configuration_get_handler_default(handler, key, val, size);
}

static int _drink_coffee_import(const conf_handler_t *handler,
                                conf_key_buf_t *key)
{
    return configuration_import_handler_default(handler, key);
}

static int _drink_coffee_export(const conf_handler_t *handler,
                                conf_key_buf_t *key)
{
    return configuration_export_handler_default(handler, key);
}

static int _drink_coffee_delete(const conf_handler_t *handler,
                                conf_key_buf_t *key)
{
    return configuration_delete_handler_default(handler, key);
}

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

static const conf_handler_ops_t _drinks_coffee_handler_ops = {
    .set = _drink_coffee_set,
    .get = _drink_coffee_get,
    .import = _drink_coffee_import,
    .export = _drink_coffee_export,
    .delete = _drink_coffee_delete,
};

static const conf_handler_data_ops_t _drinks_coffee_handler_data_ops = {
    .verify = _drink_coffee_verify,
    .apply = _drink_coffee_apply,
};

static int _drink_tea_set(const conf_handler_t *handler,
                          conf_key_buf_t *key, const void *val, size_t *size)
{
    return configuration_set_handler_default(handler, key, val, size);
}

static int _drink_tea_get(const conf_handler_t *handler,
                          conf_key_buf_t *key, void *val, size_t *size)
{
    return configuration_get_handler_default(handler, key, val, size);
}

static int _drink_tea_import(const conf_handler_t *handler,
                             conf_key_buf_t *key)
{
    return configuration_import_handler_default(handler, key);
}

static int _drink_tea_export(const conf_handler_t *handler,
                             conf_key_buf_t *key)
{
    return configuration_export_handler_default(handler, key);
}

static int _drink_tea_delete(const conf_handler_t *handler,
                             conf_key_buf_t *key)
{
    return configuration_delete_handler_default(handler, key);
}

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

static const conf_handler_ops_t _drinks_tea_handler_ops = {
    .set = _drink_tea_set,
    .get = _drink_tea_get,
    .import = _drink_tea_import,
    .export = _drink_tea_export,
    .delete = _drink_tea_delete,
};

static const conf_handler_data_ops_t _drinks_tea_handler_data_ops = {
    .verify = _drink_tea_verify,
    .apply = _drink_tea_apply,
};

static int _drink_cocoa_set(const conf_handler_t *handler,
                            conf_key_buf_t *key, const void *val, size_t *size)
{
    return configuration_set_handler_default(handler, key, val, size);
}

static int _drink_cocoa_get(const conf_handler_t *handler,
                            conf_key_buf_t *key, void *val, size_t *size)
{
    return configuration_get_handler_default(handler, key, val, size);
}

static int _drink_cocoa_import(const conf_handler_t *handler,
                               conf_key_buf_t *key)
{
    return configuration_import_handler_default(handler, key);
}

static int _drink_cocoa_export(const conf_handler_t *handler,
                               conf_key_buf_t *key)
{
    return configuration_export_handler_default(handler, key);
}

static int _drink_cocoa_delete(const conf_handler_t *handler,
                               conf_key_buf_t *key)
{
    return configuration_delete_handler_default(handler, key);
}

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

static const conf_handler_ops_t _drinks_cocoa_handler_ops = {
    .set = _drink_cocoa_set,
    .get = _drink_cocoa_get,
    .import = _drink_cocoa_import,
    .export = _drink_cocoa_export,
    .delete = _drink_cocoa_delete,
};

static const conf_handler_data_ops_t _drinks_cocoa_handler_data_ops = {
    .verify = _drink_cocoa_verify,
    .apply = _drink_cocoa_apply,
};

static int _orders_set(const conf_handler_t *handler,
                       conf_key_buf_t *key, const void *val, size_t *size)
{
    return configuration_set_handler_default(handler, key, val, size);
}

static int _orders_get(const conf_handler_t *handler,
                       conf_key_buf_t *key, void *val, size_t *size)
{
    return configuration_get_handler_default(handler, key, val, size);
}

static int _orders_import(const conf_handler_t *handler,
                          conf_key_buf_t *key)
{
    return configuration_import_handler_default(handler, key);
}

static int _orders_export(const conf_handler_t *handler,
                          conf_key_buf_t *key)
{
    return configuration_export_handler_default(handler, key);
}

static int _orders_delete(const conf_handler_t *handler,
                          conf_key_buf_t *key)
{
    return configuration_delete_handler_default(handler, key);
}

static int _orders_verify(const conf_handler_t *handler,
                          conf_key_buf_t *key)
{
    (void)handler; (void)key;
    printf("test configuration: Verifying %s\n", key->buf);
    return 0;
}

static int _orders_apply(const conf_handler_t *handler,
                         conf_key_buf_t *key)
{
    (void)handler; (void)key;
    printf("test configuration: Applying %s\n", key->buf);
    return 0;
}

static const conf_handler_ops_t _orders_handler_ops = {
    .set = _orders_set,
    .get = _orders_get,
    .import = _orders_import,
    .export = _orders_export,
    .delete = _orders_delete,
};

static const conf_handler_data_ops_t _orders_handler_data_ops = {
    .verify = _orders_verify,
    .apply = _orders_apply,
};

static int _orders_items_set(const conf_handler_t *handler,
                             conf_key_buf_t *key, const void *val, size_t *size)
{
    return configuration_set_handler_default(handler, key, val, size);
}

static int _orders_items_get(const conf_handler_t *handler,
                             conf_key_buf_t *key, void *val, size_t *size)
{
    return configuration_get_handler_default(handler, key, val, size);
}


static const conf_handler_ops_t _orders_items_handler_ops = {
    .set = _orders_items_set,
    .get = _orders_items_get,
    .import = NULL,
    .export = NULL,
    .delete = NULL,
};

static conf_handler_node_t _products_food_conf_handler
    = CONF_HANDLER_NODE_INITIALIZER("food");

static conf_handler_node_t _products_bread_handler
    = CONF_HANDLER_NODE_INITIALIZER("bread");

static conf_handler_t _products_bread_white_handler
    = CONF_HANDLER_INITIALIZER("white",
                               &_bread_white_handler_ops,
                               &_bread_white_handler_data_ops,
                               sizeof(_conf.food.bread.white),
                               &_conf.food.bread.white);

static conf_handler_t _products_bread_whole_grain_handler
    = CONF_HANDLER_INITIALIZER("whole_grain",
                               &_bread_whole_grain_handler_ops,
                               &_bread_whole_grain_handler_data_ops,
                               sizeof(_conf.food.bread.whole_grain),
                               &_conf.food.bread.whole_grain);

static conf_handler_node_t _products_cake_handler
    = CONF_HANDLER_NODE_INITIALIZER("cake");

static conf_handler_t _products_cake_cheesecake_handler
    = CONF_HANDLER_INITIALIZER("cheesecake",
                               &_cake_cheesecake_handler_ops,
                               &_cake_cheesecake_handler_data_ops,
                               sizeof(_conf.food.cake.cheesecake),
                               &_conf.food.cake.cheesecake);

static conf_handler_t _products_cake_donut_handler
    = CONF_HANDLER_INITIALIZER("donut",
                               &_cake_donut_handler_ops,
                               &_cake_donut_handler_data_ops,
                               sizeof(_conf.food.cake.donut),
                               &_conf.food.cake.donut);

static conf_handler_node_t _products_drinks_conf_handler
    = CONF_HANDLER_NODE_INITIALIZER("drinks");

static conf_handler_t _products_drinks_coffee_conf_handler
    = CONF_HANDLER_INITIALIZER("coffee",
                               &_drinks_coffee_handler_ops,
                               &_drinks_coffee_handler_data_ops,
                               sizeof(_conf.drinks.coffee),
                               &_conf.drinks.coffee);

static conf_handler_t _products_drinks_tea_conf_handler
    = CONF_HANDLER_INITIALIZER("tea",
                               &_drinks_tea_handler_ops,
                               &_drinks_tea_handler_data_ops,
                               sizeof(_conf.drinks.tea),
                               &_conf.drinks.tea);

static conf_handler_t _products_drinks_cocoa_conf_handler
    = CONF_HANDLER_INITIALIZER("cocoa",
                               &_drinks_cocoa_handler_ops,
                               &_drinks_cocoa_handler_data_ops,
                               sizeof(_conf.drinks.cocoa),
                               &_conf.drinks.cocoa);

static conf_array_handler_t _products_orders_conf_handler
    = CONF_ARRAY_HANDLER_INITIALIZER("orders",
                                     &_orders_handler_ops,
                                     &_orders_handler_data_ops,
                                     sizeof(_conf.orders[0]),
                                     &_conf.orders,
                                     ARRAY_SIZE(_conf.orders),
                                     0);

static conf_array_handler_t _products_orders_items_conf_handler
    = CONF_ARRAY_HANDLER_INITIALIZER("items",
                                     &_orders_items_handler_ops, NULL,
                                     sizeof(_conf.orders[0].items[0]),
                                     &_conf.orders[0].items,
                                     ARRAY_SIZE(_conf.orders[0].items),
                                     0);

static void _init_backend(void)
{
#if defined(TEST_CONFIGURATION_BACKEND_RAM)
    _products_orders_conf_handler.handler.src_backend = configuration_backend_ram_get();
    _products_orders_items_conf_handler.handler.src_backend = configuration_backend_ram_get();
    _products_drinks_cocoa_conf_handler.src_backend = configuration_backend_ram_get();
    _products_drinks_tea_conf_handler.src_backend = configuration_backend_ram_get();
    _products_drinks_coffee_conf_handler.src_backend = configuration_backend_ram_get();
    _products_cake_donut_handler.src_backend = configuration_backend_ram_get();
    _products_cake_cheesecake_handler.src_backend = configuration_backend_ram_get();
    _products_bread_whole_grain_handler.src_backend = configuration_backend_ram_get();
    _products_bread_white_handler.src_backend = configuration_backend_ram_get();
#elif defined(TEST_CONFIGURATION_BACKEND_FLASHDB_MTD) || \
      defined(TEST_CONFIGURATION_BACKEND_FLASHDB_VFS)
    _products_orders_conf_handler.src_backend = configuration_backend_flashdb_get();
    _products_drinks_cocoa_conf_handler.src_backend = configuration_backend_flashdb_get();
    _products_drinks_tea_conf_handler.src_backend = configuration_backend_flashdb_get();
    _products_drinks_coffee_conf_handler.src_backend = configuration_backend_flashdb_get();
    _products_cake_donut_handler.src_backend = configuration_backend_flashdb_get();
    _products_cake_cheesecake_handler.src_backend = configuration_backend_flashdb_get();
    _products_bread_whole_grain_handler.src_backend = configuration_backend_flashdb_get();
    _products_bread_white_handler.src_backend = configuration_backend_flashdb_get();
#endif
}

static void _setup(void)
{
    configuration_register(configuration_get_root(), &_products_food_conf_handler);
    configuration_register(configuration_get_root(), &_products_drinks_conf_handler);
    configuration_register(configuration_get_root(), &_products_orders_conf_handler.handler.node);
    configuration_register(&_products_food_conf_handler, &_products_bread_handler);
    configuration_register(&_products_food_conf_handler, &_products_cake_handler);
    configuration_register(&_products_bread_handler, &_products_bread_white_handler.node);
    configuration_register(&_products_bread_handler, &_products_bread_whole_grain_handler.node);
    configuration_register(&_products_cake_handler, &_products_cake_cheesecake_handler.node);
    configuration_register(&_products_cake_handler, &_products_cake_donut_handler.node);
    configuration_register(&_products_drinks_conf_handler, &_products_drinks_coffee_conf_handler.node);
    configuration_register(&_products_drinks_conf_handler, &_products_drinks_tea_conf_handler.node);
    configuration_register(&_products_drinks_conf_handler, &_products_drinks_cocoa_conf_handler.node);
    configuration_register(&_products_orders_conf_handler.handler.node,
                           &_products_orders_items_conf_handler.handler.node);
}

static void test_configuration_get(void)
{
    struct configuration conf = { 0 };
    size_t conf_size;

    conf_size = sizeof(conf.food);
    strcpy(key_buf.buf, "invalid");
    TEST_ASSERT(configuration_get(&key_buf, &conf.food, &conf_size));

    conf_size = sizeof(conf.food);
    strcpy(key_buf.buf, "/food/bread/invalid");
    TEST_ASSERT(configuration_get(&key_buf, &conf.food, &conf_size));

    memset(&conf, 0, sizeof(conf));
    conf_size = sizeof(conf.food);
    strcpy(key_buf.buf, "/food");
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.food, &_conf.food, conf_size));

    memset(&conf, 0, sizeof(conf));
    conf_size = sizeof(conf.food.bread);
    strcpy(key_buf.buf, "/food/bread");
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food.bread, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.food.bread, &_conf.food.bread, conf_size));

    memset(&conf, 0, sizeof(conf));
    conf_size = sizeof(conf.food.bread.white);
    strcpy(key_buf.buf, "/food/bread/white");
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food.bread.white, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread/white"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.food.bread.white, &_conf.food.bread.white, conf_size));

    memset(&conf, 0, sizeof(conf));
    conf_size = sizeof(conf.food.bread.whole_grain);
    strcpy(key_buf.buf, "/food/bread/whole_grain");
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food.bread.whole_grain, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread/whole_grain"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.food.bread.whole_grain, &_conf.food.bread.whole_grain, conf_size));

    memset(&conf, 0, sizeof(conf));
    conf_size = sizeof(conf.food.cake);
    strcpy(key_buf.buf, "/food/cake");
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food.cake, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/cake"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.food.cake, &_conf.food.cake, conf_size));

    memset(&conf, 0, sizeof(conf));
    conf_size = sizeof(conf.food.cake.cheesecake);
    strcpy(key_buf.buf, "/food/cake/cheesecake");
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food.cake.cheesecake, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/cake/cheesecake"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.food.cake.cheesecake, &_conf.food.cake.cheesecake, conf_size));

    memset(&conf, 0, sizeof(conf));
    conf_size = sizeof(conf.food.cake.donut);
    strcpy(key_buf.buf, "/food/cake/donut");
    TEST_ASSERT(!configuration_get(&key_buf, &conf.food.cake.donut, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/cake/donut"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.food.cake.donut, &_conf.food.cake.donut, conf_size));

    conf_size = sizeof(conf.drinks);
    strcpy(key_buf.buf, "/drinks");
    TEST_ASSERT(!configuration_get(&key_buf, &conf.drinks, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks"));
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
    strcpy(key_buf.buf, "invalid");
    TEST_ASSERT(configuration_set(&key_buf, &new_conf.food, &conf_size));

    conf_size = sizeof(new_conf.food);
    strcpy(key_buf.buf, "/food/bread/invalid");
    TEST_ASSERT(configuration_set(&key_buf, &new_conf.food, &conf_size));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.food.bread.white);
    strcpy(key_buf.buf, "/food/bread/white");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food.bread.white, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread/white"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food.bread.white, &_conf.food.bread.white, sizeof(_conf.food.bread.white)));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.food.bread.whole_grain);
    strcpy(key_buf.buf, "/food/bread/whole_grain");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food.bread.whole_grain, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread/whole_grain"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food.bread.whole_grain, &_conf.food.bread.whole_grain, sizeof(_conf.food.bread.whole_grain)));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.food.bread);
    strcpy(key_buf.buf, "/food/bread");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food.bread, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food.bread, &_conf.food.bread, sizeof(_conf.food.bread)));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.food.cake.cheesecake);
    strcpy(key_buf.buf, "/food/cake/cheesecake");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food.cake.cheesecake, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/cake/cheesecake"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food.cake.cheesecake, &_conf.food.cake.cheesecake, sizeof(_conf.food.cake.cheesecake)));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.food.cake.donut);
    strcpy(key_buf.buf, "/food/cake/donut");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food.cake.donut, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/cake/donut"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food.cake.donut, &_conf.food.cake.donut, sizeof(_conf.food.cake.donut)));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.food.cake);
    strcpy(key_buf.buf, "/food/cake");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food.cake, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/cake"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food.cake, &_conf.food.cake, sizeof(_conf.food.cake)));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.food);
    strcpy(key_buf.buf, "/food");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food, &_conf.food, sizeof(_conf.food)));

    memset(&_conf, 0, sizeof(_conf));
    conf_size = sizeof(new_conf.drinks);
    strcpy(key_buf.buf, "/drinks");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.drinks, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.drinks, &_conf.drinks, sizeof(_conf.drinks)));

    _conf = conf_backup;
}

MAYBE_UNUSED
static void test_configuration_import(void)
{
    struct configuration conf_backup = _conf;

    strcpy(key_buf.buf, "invalid");
    TEST_ASSERT(configuration_import(&key_buf));

    strcpy(key_buf.buf, "/food/bread/invalid");
    TEST_ASSERT(configuration_import(&key_buf));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/food");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food"));
    TEST_ASSERT(!memcmp(&persist_conf.food, &_conf.food, sizeof(_conf.food)));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/food/bread");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread"));
    TEST_ASSERT(!memcmp(&persist_conf.food.bread, &_conf.food.bread, sizeof(_conf.food.bread)));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/food/bread/white");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread/white"));
    TEST_ASSERT(!memcmp(&persist_conf.food.bread.white, &_conf.food.bread.white, sizeof(_conf.food.bread.white)));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/food/bread/whole_grain");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread/whole_grain"));
    TEST_ASSERT(!memcmp(&persist_conf.food.bread.whole_grain, &_conf.food.bread.whole_grain, sizeof(_conf.food.bread.whole_grain)));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/food/cake");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/cake"));
    TEST_ASSERT(!memcmp(&persist_conf.food.cake, &_conf.food.cake, sizeof(_conf.food.cake)));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/food/cake/cheesecake");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/cake/cheesecake"));
    TEST_ASSERT(!memcmp(&persist_conf.food.cake.cheesecake, &_conf.food.cake.cheesecake, sizeof(_conf.food.cake.cheesecake)));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/food/cake/donut");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/cake/donut"));
    TEST_ASSERT(!memcmp(&persist_conf.food.cake.donut, &_conf.food.cake.donut, sizeof(_conf.food.cake.donut)));

    strcpy(key_buf.buf, "/drinks");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks"));
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

    strcpy(key_buf.buf, "invalid");
    TEST_ASSERT(configuration_export(&key_buf));
    strcpy(key_buf.buf, "/food/bread/invalid");
    TEST_ASSERT(configuration_export(&key_buf));
    memset(&persist_conf, 0, sizeof(persist_conf));

    strcpy(key_buf.buf, "/food/bread/white");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread/white"));
    TEST_ASSERT(!memcmp(&persist_conf.food.bread.white, &_conf.food.bread.white, sizeof(_conf.food.bread.white)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/food/bread/whole_grain");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread/whole_grain"));
    TEST_ASSERT(!memcmp(&new_conf.food.bread.whole_grain, &_conf.food.bread.whole_grain, sizeof(_conf.food.bread.whole_grain)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/food/bread");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread"));
    TEST_ASSERT(!memcmp(&new_conf.food.bread, &_conf.food.bread, sizeof(_conf.food.bread)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/food/cake/cheesecake");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/cake/cheesecake"));
    TEST_ASSERT(!memcmp(&new_conf.food.cake.cheesecake, &_conf.food.cake.cheesecake, sizeof(_conf.food.cake.cheesecake)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/food/cake/donut");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/cake/donut"));
    TEST_ASSERT(!memcmp(&new_conf.food.cake.donut, &_conf.food.cake.donut, sizeof(_conf.food.cake.donut)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/food/cake");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/cake"));
    TEST_ASSERT(!memcmp(&new_conf.food.cake, &_conf.food.cake, sizeof(_conf.food.cake)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/food");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food"));
    TEST_ASSERT(!memcmp(&new_conf.food, &_conf.food, sizeof(_conf.food)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/drinks");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks"));
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

    strcpy(key_buf.buf, "/food");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food"));

    strcpy(key_buf.buf, "/drinks");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks"));

    conf_size = sizeof(new_conf.food);
    strcpy(key_buf.buf, "/food");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food"));
    TEST_ASSERT(conf_size == 0);

    conf_size = sizeof(new_conf.drinks);
    strcpy(key_buf.buf, "/drinks");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.drinks, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks"));
    TEST_ASSERT(conf_size == 0);

    strcpy(key_buf.buf, "/food");
    configuration_delete(&key_buf);
    TEST_ASSERT(!strcmp(key_buf.buf, "/food"));

    strcpy(key_buf.buf, "/drinks");
    configuration_delete(&key_buf);
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks"));

    strcpy(key_buf.buf, "/food");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food"));

    strcpy(key_buf.buf, "/drinks");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks"));

    memset(&_conf, 0, sizeof(_conf));

    strcpy(key_buf.buf, "/food");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food"));
    TEST_ASSERT(!memcmp(&_conf.food, &new_conf.food, sizeof(_conf.food)));

    strcpy(key_buf.buf, "/drinks");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks"));
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

    strcpy(key_buf.buf, "invalid");
    TEST_ASSERT(configuration_delete(&key_buf));

    strcpy(key_buf.buf, "/food/bread/baguette");
    TEST_ASSERT(configuration_delete(&key_buf));

    strcpy(key_buf.buf, "/food/bread");
    TEST_ASSERT(!configuration_delete(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread"));

    conf_size = sizeof(new_conf.food);
    strcpy(key_buf.buf, "/food");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food"));
    TEST_ASSERT(conf_size == 0);

    conf_size = sizeof(new_conf.drinks);
    strcpy(key_buf.buf, "/drinks");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.drinks, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks"));
    TEST_ASSERT(conf_size == 0);

    strcpy(key_buf.buf, "/food");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food"));

    strcpy(key_buf.buf, "/drinks");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks"));

    strcpy(key_buf.buf, "/food");
    TEST_ASSERT(!configuration_delete(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food"));
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food"));

    strcpy(key_buf.buf, "/food/bread/white");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread/white"));

    strcpy(key_buf.buf, "/drinks");
    TEST_ASSERT(!configuration_delete(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks"));
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks"));

    strcpy(key_buf.buf, "/drinks/coffee");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks/coffee"));

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

    strcpy(key_buf.buf, "/food");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food"));

    conf_size = sizeof(new_conf.food.bread.white);
    strcpy(key_buf.buf, "/food/bread/white");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.food.bread.white, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread/white"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.food.bread.white, &_conf.food.bread.white, sizeof(_conf.food.bread.white)));

    strcpy(key_buf.buf, "/food/bread");
    TEST_ASSERT(configuration_verify(&key_buf, false));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread"));
    TEST_ASSERT(!configuration_verify(&key_buf, true));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread"));
    TEST_ASSERT(!configuration_apply(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/food/bread"));

    strcpy(key_buf.buf, "/drinks");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks"));

    conf_size = sizeof(new_conf.drinks.coffee);
    strcpy(key_buf.buf, "/drinks/coffee");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.drinks.coffee, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks/coffee"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&new_conf.drinks.coffee, &_conf.drinks.coffee, sizeof(_conf.drinks.coffee)));

    TEST_ASSERT(configuration_verify(&key_buf, false));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks/coffee"));
    TEST_ASSERT(!configuration_verify(&key_buf, true));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks/coffee"));
    TEST_ASSERT(!configuration_apply(&key_buf));
    TEST_ASSERT(!strcmp(key_buf.buf, "/drinks/coffee"));

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
    strcpy(key_buf.buf, "/orders");
    TEST_ASSERT(!configuration_set(&key_buf, orders, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/orders"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&_conf.orders, &orders, sizeof(orders)));

    conf_size = sizeof(orders[0]);
    strcpy(key_buf.buf, "/orders/3");
    TEST_ASSERT(configuration_set(&key_buf, orders, &conf_size));

    conf_size = sizeof(orders[0]);
    strcpy(key_buf.buf, "/orders/0");
    TEST_ASSERT(!configuration_set(&key_buf, NULL, NULL));
    TEST_ASSERT(!strcmp(key_buf.buf, "/orders/0"));
    TEST_ASSERT(!memcmp(&_conf.orders[0], &((order_t){0}), sizeof(orders[0])));

    conf_size = sizeof(orders[1]);
    strcpy(key_buf.buf, "/orders/1");
    TEST_ASSERT(!configuration_set(&key_buf, NULL, NULL));
    TEST_ASSERT(!strcmp(key_buf.buf, "/orders/1"));
    TEST_ASSERT(!memcmp(&_conf.orders[1], &((order_t){0}), sizeof(orders[1])));

    conf_size = sizeof(orders[2]);
    strcpy(key_buf.buf, "/orders/2");
    TEST_ASSERT(!configuration_set(&key_buf, NULL, NULL));
    TEST_ASSERT(!strcmp(key_buf.buf, "/orders/2"));
    TEST_ASSERT(!memcmp(&_conf.orders[0], &((order_t){0}), sizeof(orders[2])));


    order_t order_item_0 = {
        .items = { {"juice"}, {"tea"} },
    };

    conf_size = sizeof(order_item_0.items[0]);
    strcpy(key_buf.buf, "/orders/0/items/0");
    TEST_ASSERT(!configuration_set(&key_buf, &order_item_0.items[0], &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/orders/0/items/0"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&_conf.orders[0].items[0], &order_item_0.items[0], sizeof(order_item_0.items[0])));

    conf_size = sizeof(order_item_0.items[1]);
    strcpy(key_buf.buf, "/orders/0/items/1");
    TEST_ASSERT(!configuration_set(&key_buf, &order_item_0.items[1], &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/orders/0/items/1"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&_conf.orders[0].items[1], &order_item_0.items[1], sizeof(order_item_0.items[1])));

    order_t order_item_1 = {
        .items = { {"fish"}, {"chips"} },
    };

    conf_size = sizeof(order_item_1);
    strcpy(key_buf.buf, "/orders/2/items");
    TEST_ASSERT(!configuration_set(&key_buf, &order_item_1.items, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/orders/2/items"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&_conf.orders[2].items, &order_item_1.items, sizeof(order_item_1.items)));

    conf_size = sizeof(order_item_1);
    strcpy(key_buf.buf, "/orders/2/invalid");
    TEST_ASSERT(configuration_set(&key_buf, &order_item_1.items, &conf_size));

    conf_size = sizeof(order_item_1);
    strcpy(key_buf.buf, "/orders/0/2/invalid");
    TEST_ASSERT(configuration_set(&key_buf, &order_item_1.items, &conf_size));

    conf_size = sizeof(order_item_1);
    strcpy(key_buf.buf, "/orders/0/2");
    TEST_ASSERT(configuration_set(&key_buf, &order_item_1.items, &conf_size));

    _conf = conf_backup;
}

static void test_configuration_array_get(void)
{
    struct configuration conf = { 0 };
    size_t conf_size;

    conf_size = sizeof(conf.orders);
    strcpy(key_buf.buf, "/orders/invalid");
    TEST_ASSERT(configuration_get(&key_buf, &conf.orders, &conf_size));

    conf_size = sizeof(conf.orders[0]);
    strcpy(key_buf.buf, "/orders/3");
    TEST_ASSERT(configuration_get(&key_buf, &conf.orders[0], &conf_size));

    conf_size = sizeof(conf.orders);
    strcpy(key_buf.buf, "/orders/items");
    TEST_ASSERT(configuration_get(&key_buf, &conf.orders, &conf_size));

    conf_size = sizeof(conf.orders[0].items[0]);
    strcpy(key_buf.buf, "/orders/0/items/0");
    TEST_ASSERT(!configuration_get(&key_buf, &conf.orders[0].items[0], &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/orders/0/items/0"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.orders[0].items[0], &_conf.orders[0].items[0], conf_size));

    conf_size = sizeof(conf.orders[0].items[1]);
    strcpy(key_buf.buf, "/orders/0/items/1");
    TEST_ASSERT(!configuration_get(&key_buf, &conf.orders[0].items[1], &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/orders/0/items/1"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.orders[0].items[1], &_conf.orders[0].items[1], conf_size));

    conf_size = sizeof(conf.orders[1].items[0]);
    strcpy(key_buf.buf, "/orders/1/items/0");
    TEST_ASSERT(!configuration_get(&key_buf, &conf.orders[1].items[0], &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/orders/1/items/0"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.orders[1].items[0], &_conf.orders[1].items[0], conf_size));

    conf_size = sizeof(conf.orders[1].items[1]);
    strcpy(key_buf.buf, "/orders/1/items/1");
    TEST_ASSERT(!configuration_get(&key_buf, &conf.orders[1].items[1], &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/orders/1/items/1"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.orders[1].items[1], &_conf.orders[1].items[1], conf_size));

    conf_size = sizeof(conf.orders[2].items[0]);
    strcpy(key_buf.buf, "/orders/2/items/0");
    TEST_ASSERT(!configuration_get(&key_buf, &conf.orders[2].items[0], &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/orders/2/items/0"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.orders[2].items[0], &_conf.orders[2].items[0], conf_size));

    conf_size = sizeof(conf.orders[2].items[1]);
    strcpy(key_buf.buf, "/orders/2/items/1");
    TEST_ASSERT(!configuration_get(&key_buf, &conf.orders[2].items[1], &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/orders/2/items/1"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&conf.orders[2].items[1], &_conf.orders[2].items[1], conf_size));
}

static void test_configuration_array_import(void)
{
    struct configuration conf_backup = _conf;

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/orders");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!memcmp(&_conf.orders, &persist_conf.orders, sizeof(_conf.orders)));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/orders/items");
    TEST_ASSERT(configuration_import(&key_buf));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/orders/0/items/2");
    TEST_ASSERT(configuration_import(&key_buf));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/orders/0");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!memcmp(&_conf.orders[0], &persist_conf.orders[0],
                        sizeof(_conf.orders[0])));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/orders/1");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!memcmp(&_conf.orders[1], &persist_conf.orders[1],
                        sizeof(_conf.orders[1])));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/orders/2");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!memcmp(&_conf.orders[2], &persist_conf.orders[2],
                        sizeof(_conf.orders[2])));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/orders/0/items");
    TEST_ASSERT(!configuration_import(&key_buf));
    /* /orders/0/items has no import handler */
    TEST_ASSERT(memcmp(&_conf.orders[0].items, &persist_conf.orders[0].items,
                       sizeof(_conf.orders[0].items)));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/orders/1/items");
    TEST_ASSERT(!configuration_import(&key_buf));
    /* /orders/1/items has no import handler */
    TEST_ASSERT(memcmp(&_conf.orders[1].items, &persist_conf.orders[1].items,
                sizeof(_conf.orders[1].items)));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/orders/2/items");
    TEST_ASSERT(!configuration_import(&key_buf));
    /* /orders/2/items has no import handler */
    TEST_ASSERT(memcmp(&_conf.orders[2].items, &persist_conf.orders[2].items,
                       sizeof(_conf.orders[2].items)));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/orders/0/items/0");
    TEST_ASSERT(!configuration_import(&key_buf));
    /* /orders/0/items/0 has no import handler */
    TEST_ASSERT(memcmp(&_conf.orders[0].items[0], &persist_conf.orders[0].items[0],
                       sizeof(_conf.orders[0].items[0])));

    memset(&_conf, 0, sizeof(_conf));
    strcpy(key_buf.buf, "/orders/0/items/1");
    TEST_ASSERT(!configuration_import(&key_buf));
    /* /orders/0/items/1 has no import handler */
    TEST_ASSERT(memcmp(&_conf.orders[0].items[1], &persist_conf.orders[0].items[1],
                       sizeof(_conf.orders[0].items[1])));

    _conf = conf_backup;
}

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

    strcpy(key_buf.buf, "invalid");
    TEST_ASSERT(configuration_export(&key_buf));
    strcpy(key_buf.buf, "/orders/invalid");
    TEST_ASSERT(configuration_export(&key_buf));
    strcpy(key_buf.buf, "/orders/0/invalid");
    TEST_ASSERT(configuration_export(&key_buf));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/orders");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!memcmp(&persist_conf.orders, &_conf.orders, sizeof(_conf.orders)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/orders/0");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!memcmp(&persist_conf.orders[0], &_conf.orders[0], sizeof(_conf.orders[0])));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/orders/1");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!memcmp(&persist_conf.orders[1], &_conf.orders[1], sizeof(_conf.orders[1])));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/orders/2");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!memcmp(&persist_conf.orders[2], &_conf.orders[2], sizeof(_conf.orders[2])));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/orders/1/items");
    TEST_ASSERT(!configuration_export(&key_buf));
    /* /orders/1/items has no export handler */
    TEST_ASSERT(memcmp(&persist_conf.orders[1].items, &_conf.orders[1].items,
                       sizeof(_conf.orders[1].items)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/orders/0/items");
    TEST_ASSERT(!configuration_export(&key_buf));
    /* /orders/1/items has no export handler */
    TEST_ASSERT(memcmp(&persist_conf.orders[0].items, &_conf.orders[0].items,
                       sizeof(_conf.orders[0].items)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/orders/1/items");
    TEST_ASSERT(!configuration_export(&key_buf));
    /* /orders/1/items has no export handler */
    TEST_ASSERT(memcmp(&persist_conf.orders[1].items, &_conf.orders[1].items,
                       sizeof(_conf.orders[1].items)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/orders/2/items");
    TEST_ASSERT(!configuration_export(&key_buf));
    /* /orders/1/items has no export handler */
    TEST_ASSERT(memcmp(&persist_conf.orders[2].items, &_conf.orders[2].items,
                       sizeof(_conf.orders[2].items)));

    memset(&persist_conf, 0, sizeof(persist_conf));
    strcpy(key_buf.buf, "/orders/0/items/0");
    TEST_ASSERT(!configuration_export(&key_buf));
    /* /orders/1/items has no export handler */
    TEST_ASSERT(memcmp(&persist_conf.orders[0].items[0], &_conf.orders[0].items[0],
                       sizeof(_conf.orders[0].items[0])));

    memset(&persist_conf, 0, sizeof(persist_conf));

    strcpy(key_buf.buf, "/orders/0/items/1");
    TEST_ASSERT(!configuration_export(&key_buf));
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

    strcpy(key_buf.buf, "/orders/0/invalid");
    TEST_ASSERT(configuration_delete(&key_buf));

    strcpy(key_buf.buf, "/orders/0");
    TEST_ASSERT(!configuration_delete(&key_buf));
    strcpy(key_buf.buf, "/orders/1");
    TEST_ASSERT(!configuration_delete(&key_buf));
    strcpy(key_buf.buf, "/orders/2");
    TEST_ASSERT(!configuration_delete(&key_buf));

    strcpy(key_buf.buf, "/orders/0");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!memcmp(&_conf.orders, &new_conf.orders, sizeof(_conf.orders)));
    strcpy(key_buf.buf, "/orders/1");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!memcmp(&_conf.orders, &new_conf.orders, sizeof(_conf.orders)));
    strcpy(key_buf.buf, "/orders/2");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!memcmp(&_conf.orders, &new_conf.orders, sizeof(_conf.orders)));

    strcpy(key_buf.buf, "/orders");
    TEST_ASSERT(!configuration_export(&key_buf));

    strcpy(key_buf.buf, "/orders/0/items");
    TEST_ASSERT(!configuration_delete(&key_buf));
    strcpy(key_buf.buf, "/orders/1/items");
    TEST_ASSERT(!configuration_delete(&key_buf));
    strcpy(key_buf.buf, "/orders/2/items");
    TEST_ASSERT(!configuration_delete(&key_buf));

    strcpy(key_buf.buf, "/orders");
    TEST_ASSERT(!configuration_set(&key_buf, NULL, NULL));

    strcpy(key_buf.buf, "/orders/0");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!memcmp(&_conf.orders[0], &new_conf.orders[0], sizeof(_conf.orders[0])));
    strcpy(key_buf.buf, "/orders/1");
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!memcmp(&_conf.orders[1], &new_conf.orders[1], sizeof(_conf.orders[1])));
    strcpy(key_buf.buf, "/orders/2");
    TEST_ASSERT(!configuration_import(&key_buf));
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

    strcpy(key_buf.buf, "/orders");
    TEST_ASSERT(!configuration_import(&key_buf));

    conf_size = sizeof(new_conf.orders);
    strcpy(key_buf.buf, "/orders");
    TEST_ASSERT(!configuration_set(&key_buf, &new_conf.orders, &conf_size));
    TEST_ASSERT(!strcmp(key_buf.buf, "/orders"));
    TEST_ASSERT(conf_size == 0);
    TEST_ASSERT(!memcmp(&_conf.orders, &new_conf.orders, sizeof(_conf.orders)));

    strcpy(key_buf.buf, "/orders");
    TEST_ASSERT(!configuration_delete(&key_buf));

    strcpy(key_buf.buf, "/orders");
    TEST_ASSERT(!configuration_export(&key_buf));
    TEST_ASSERT(!configuration_import(&key_buf));
    TEST_ASSERT(!memcmp(&_conf.orders, &new_conf.orders, sizeof(_conf.orders)));

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
    };

    EMB_UNIT_TESTCALLER(tests_configuration, NULL, NULL, fixtures);
    return (Test *)&tests_configuration;
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
