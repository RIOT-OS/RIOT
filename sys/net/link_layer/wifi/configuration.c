/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_configuration_wifi
 * @{
 *
 * @file
 * @brief   Wi-Fi configuration implementation
 *
 * @author  Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "fmt.h"
#include "macros/utils.h"
#include "container.h"
#include "list.h"
#include "net/l2scan_list.h"
#include "net/wifi.h"
#include "net/netdev/wifi.h"
#include "configuration.h"
#include "configuration/wifi.h"

#define ENABLE_DEBUG    0
#include "debug.h"

__attribute__((weak))
const conf_backend_t *wifi_conf_ap_backend_init(void)
{
    return NULL;
}

__attribute__((weak))
const conf_backend_t *wifi_conf_ap_ssid_backend_init(void)
{
    return NULL;
}

__attribute__((weak))
const conf_backend_t *wifi_conf_ap_sec_backend_init(void)
{
    return NULL;
}

/* /wifi */
static conf_handler_node_t _wifi_conf_handler
    = CONF_HANDLER_NODE_INITIALIZER("wifi");

static wifi_conf_ap_t _wifi_conf_ap_table[CONFIG_WIFI_CONF_STORED_AP_NUMOF];

static int _wifi_ap_conf_set(const conf_handler_t *handler,
                             conf_key_buf_t *key, const void *val,
                             size_t *size);
static int _wifi_ap_conf_get(const conf_handler_t *handler,
                             conf_key_buf_t *key, void *val,
                             size_t *size);
static int _wifi_ap_conf_import(const conf_handler_t *handler,
                                conf_key_buf_t *key);
static int _wifi_ap_conf_export(const conf_handler_t *handler,
                                conf_key_buf_t *key);
static int _wifi_ap_conf_delete(const conf_handler_t *handler,
                                conf_key_buf_t *key);

static const conf_handler_ops_t _wifi_conf_ap_handler_ops = {
    .set = _wifi_ap_conf_set,
    .get = _wifi_ap_conf_get,
    .import = _wifi_ap_conf_import,
    .export = _wifi_ap_conf_export,
    .delete = _wifi_ap_conf_delete,
};

/* /wifi/ap */
static conf_array_handler_t _wifi_conf_ap_handler
    = CONF_ARRAY_HANDLER_INITIALIZER("ap", &_wifi_conf_ap_handler_ops, NULL,
                                     sizeof(_wifi_conf_ap_table[0]),
                                     ARRAY_SIZE(_wifi_conf_ap_table),
                                     _wifi_conf_ap_table);

static int _wifi_ap_conf_set_ssid(const conf_handler_t *handler,
                                  conf_key_buf_t *key, const void *val,
                                  size_t *size);
static int _wifi_ap_conf_get_ssid(const conf_handler_t *handler,
                                  conf_key_buf_t *key, void *val,
                                  size_t *size);
static int _wifi_ap_conf_import_ssid(const conf_handler_t *handler,
                                     conf_key_buf_t *key);
static int _wifi_ap_conf_export_ssid(const conf_handler_t *handler,
                                     conf_key_buf_t *key);
static int _wifi_ap_conf_delete_ssid(const conf_handler_t *handler,
                                     conf_key_buf_t *key);

static const conf_handler_ops_t _wifi_conf_ap_ssid_handler_ops = {
    .set = _wifi_ap_conf_set_ssid,
    .get = _wifi_ap_conf_get_ssid,
    .import = _wifi_ap_conf_import_ssid,
    .export = _wifi_ap_conf_export_ssid,
    .delete = _wifi_ap_conf_delete_ssid,
};

/* /wifi/ap/ssid */
static conf_handler_t _wifi_conf_ap_ssid_handler
    = CONF_HANDLER_INITIALIZER("ssid", &_wifi_conf_ap_ssid_handler_ops, NULL,
                               sizeof(_wifi_conf_ap_table[0].ssid),
                               &_wifi_conf_ap_table[0].ssid);

static int _wifi_ap_conf_set_sec(const conf_handler_t *handler,
                                 conf_key_buf_t *key, const void *val,
                                 size_t *size);
static int _wifi_ap_conf_get_sec(const conf_handler_t *handler,
                                 conf_key_buf_t *key, void *val,
                                 size_t *size);
static int _wifi_ap_conf_import_sec(const conf_handler_t *handler,
                                    conf_key_buf_t *key);
static int _wifi_ap_conf_export_sec(const conf_handler_t *handler,
                                    conf_key_buf_t *key);
static int _wifi_ap_conf_delete_sec(const conf_handler_t *handler,
                                    conf_key_buf_t *key);

static const conf_handler_ops_t _wifi_conf_ap_sec_handler_ops = {
    .set = _wifi_ap_conf_set_sec,
    .get = _wifi_ap_conf_get_sec,
    .import = _wifi_ap_conf_import_sec,
    .export = _wifi_ap_conf_export_sec,
    .delete = _wifi_ap_conf_delete_sec,
};

/* /wifi/ap/sec */
static conf_handler_t _wifi_conf_ap_sec_handler
    = CONF_HANDLER_INITIALIZER("sec", &_wifi_conf_ap_sec_handler_ops, NULL,
                               sizeof(_wifi_conf_ap_table[0].sec),
                               &_wifi_conf_ap_table[0].sec);

/* When locked successfully, a buffer of a WiFi configuration object is returned,
   which can be used until the key is unlocked. The reasoning behind this is to reduce
   stack size, as usually a temporay object would need to be allocated by the caller */
static inline wifi_conf_ap_t *_wifi_configuration_ap_lock(conf_key_t *key)
{
    static wifi_conf_ap_t _tmp;
    memset(&_tmp, 0, sizeof(_tmp));
    if (configuration_lock(key)) {
        return NULL;
    }
    return &_tmp;
}

static inline void _wifi_configuration_ap_unlock(conf_key_t *key)
{
    configuration_unlock(key);
}

static inline bool _wifi_configuration_ap_valid(const char *ssid,
                                                const wifi_security_mode_t *sec)
{
    assert(ssid);
    return *ssid && (!sec ||
                     *sec == WIFI_SECURITY_MODE_OPEN ||
                     *sec == WIFI_SECURITY_MODE_WEP_PSK ||
                     *sec == WIFI_SECURITY_MODE_WPA2_PERSONAL ||
                     *sec == WIFI_SECURITY_MODE_WPA2_ENTERPRISE);
}


static int _wifi_ap_conf_set_ssid(const conf_handler_t *handler,
                                  conf_key_buf_t *key, const void *val,
                                  size_t *size)
{
    assert(handler == &_wifi_conf_ap_ssid_handler);
    return configuration_set_handler_default(handler, key, val, size);
}

static int _wifi_ap_conf_set_sec(const conf_handler_t *handler,
                                 conf_key_buf_t *key, const void *val,
                                 size_t *size)
{
    assert(handler == &_wifi_conf_ap_sec_handler);
    return configuration_set_handler_default(handler, key, val, size);
}

static int _wifi_ap_conf_set(const conf_handler_t *handler,
                             conf_key_buf_t *key, const void *val,
                             size_t *size)
{
    assert(handler == &_wifi_conf_ap_handler.handler);
    return configuration_set_handler_default(handler, key, val, size);
}

static int _wifi_ap_conf_get_ssid(const conf_handler_t *handler,
                                  conf_key_buf_t *key, void *val,
                                  size_t *size)
{
    assert(handler == &_wifi_conf_ap_ssid_handler);
    return configuration_get_handler_default(handler, key, val, size);
}

static int _wifi_ap_conf_get_sec(const conf_handler_t *handler,
                                 conf_key_buf_t *key, void *val,
                                 size_t *size)
{
    assert(handler == &_wifi_conf_ap_sec_handler);
    return configuration_get_handler_default(handler, key, val, size);
}

static int _wifi_ap_conf_get(const conf_handler_t *handler,
                             conf_key_buf_t *key, void *val,
                             size_t *size)
{
    assert(handler == &_wifi_conf_ap_handler.handler);
    return configuration_get_handler_default(handler, key, val, size);
}

static int _wifi_ap_conf_import(const conf_handler_t *handler,
                                conf_key_buf_t *key)
{
    assert(handler == &_wifi_conf_ap_handler.handler);
    return configuration_import_handler_default(handler, key);
}

static int _wifi_ap_conf_import_ssid(const conf_handler_t *handler,
                                     conf_key_buf_t *key)
{
    assert(handler == &_wifi_conf_ap_ssid_handler);
    return configuration_import_handler_default(handler, key);
}

static int _wifi_ap_conf_import_sec(const conf_handler_t *handler,
                                    conf_key_buf_t *key)
{
    assert(handler == &_wifi_conf_ap_sec_handler);
    return configuration_import_handler_default(handler, key);
}

static int _wifi_ap_conf_export(const conf_handler_t *handler,
                                conf_key_buf_t *key)
{
    assert(handler == &_wifi_conf_ap_handler.handler);
    return configuration_export_handler_default(handler, key);
}

static int _wifi_ap_conf_export_ssid(const conf_handler_t *handler,
                                     conf_key_buf_t *key)
{
    assert(handler == &_wifi_conf_ap_ssid_handler);
    return configuration_export_handler_default(handler, key);
}

static int _wifi_ap_conf_export_sec(const conf_handler_t *handler,
                                    conf_key_buf_t *key)
{
    assert(handler == &_wifi_conf_ap_sec_handler);
    return configuration_export_handler_default(handler, key);
}

static int _wifi_ap_conf_delete(const conf_handler_t *handler,
                                conf_key_buf_t *key)
{
    assert(handler == &_wifi_conf_ap_handler.handler);
    return configuration_delete_handler_default(handler, key);
}

static int _wifi_ap_conf_delete_ssid(const conf_handler_t *handler,
                                     conf_key_buf_t *key)
{
    assert(handler == &_wifi_conf_ap_ssid_handler);
    return configuration_delete_handler_default(handler, key);
}

static int _wifi_ap_conf_delete_sec(const conf_handler_t *handler,
                                    conf_key_buf_t *key)
{
    assert(handler == &_wifi_conf_ap_sec_handler);
    return configuration_delete_handler_default(handler, key);
}

void wifi_configuration_register(void)
{
    configuration_register(configuration_get_root(), &_wifi_conf_handler);
    configuration_register(&_wifi_conf_handler, &_wifi_conf_ap_handler.handler.node);
    configuration_register(&_wifi_conf_ap_handler.handler.node, &_wifi_conf_ap_ssid_handler.node);
    configuration_register(&_wifi_conf_ap_handler.handler.node, &_wifi_conf_ap_sec_handler.node);
}

int wifi_configuration_ap_get(wifi_conf_ap_t *ap,
                              const char *ssid,
                              const wifi_security_mode_t *sec)
{
    assert(ap);
    assert(ssid);
    CONF_KEY_T(20) key_buf = CONF_KEY_INITIALIZER(20, "/wifi/ap");
    if (!_wifi_configuration_ap_valid(ssid, sec)) {
        return -EINVAL;
    }
    if (!_wifi_configuration_ap_lock(&key_buf)) {
        return -ENOENT;
    }
    int ret = -ENOENT;
    for (unsigned i = 0; i < ARRAY_SIZE(_wifi_conf_ap_table); i++) {
        key_buf.buf[8] = '/';
        int len = fmt_u32_dec(&key_buf.buf[9], i);
        key_buf.buf[9 + len] = '\0';
        size_t size = sizeof(*ap);
        if (configuration_get(&key_buf, ap, &size)) {
            continue;
        }
        if (strcmp(ssid, ap->ssid.ssid) || (sec && ap->sec.sec != *sec)) {
            continue;
        }
        ret = 0;
        break;
    }
    key_buf.buf[8] = '\0';
    _wifi_configuration_ap_unlock(&key_buf);
    return ret;
}

int wifi_configuration_ap_set(const wifi_conf_ap_t *ap,
                              const char *ssid,
                              const wifi_security_mode_t *sec,
                              bool be_sync)
{
    CONF_KEY_T(32) key_buf = CONF_KEY_INITIALIZER(32, "/wifi/ap");
    wifi_conf_ap_t *tmp;
    if (!_wifi_configuration_ap_valid(ap->ssid.ssid, &ap->sec.sec) ||
        !_wifi_configuration_ap_valid(ssid, sec)) {
        return -EINVAL;
    }
    if (!(tmp = _wifi_configuration_ap_lock(&key_buf))) {
        return -ENOENT;
    }
    int ret = -ENOENT;
    for (unsigned i = 0; i < ARRAY_SIZE(_wifi_conf_ap_table); i++) {
        key_buf.buf[8] = '/';
        int len = fmt_u32_dec(&key_buf.buf[9], i);
        key_buf.buf[9 + len] = '\0';
        size_t size = sizeof(*ap);
        if (configuration_get(&key_buf, tmp, &size)) {
            continue;
        }
        if (strcmp(tmp->ssid.ssid, ap->ssid.ssid) || (sec && *sec != tmp->sec.sec)) {
            continue;
        }
        ret = i;
        break;
    }
    if (ret >= 0) {
        size_t size = sizeof(*ap);
        key_buf.buf[8] = '/';
        int len = fmt_u32_dec(&key_buf.buf[9], ret);
        key_buf.buf[9 + len] = '\0';
        if (!(ret = configuration_set(&key_buf, ap, &size))) {
            if (be_sync) {
                ret = configuration_export(&key_buf);
            }
        }
    }
    key_buf.buf[8] = '\0';
    _wifi_configuration_ap_unlock(&key_buf);
    return ret;
}

int wifi_configuration_ap_add(const wifi_conf_ap_t *ap, bool be_sync)
{
    CONF_KEY_T(32) key_buf = CONF_KEY_INITIALIZER(32, "/wifi/ap");
    wifi_conf_ap_t *tmp;
    if (!_wifi_configuration_ap_valid(ap->ssid.ssid, &ap->sec.sec)) {
        return -EINVAL;
    }
    if (!(tmp = _wifi_configuration_ap_lock(&key_buf))) {
        return -ENOENT;
    }
    int ret = -ENOENT;
    int new = -1;
    for (unsigned i = 0; i < ARRAY_SIZE(_wifi_conf_ap_table); i++) {
        key_buf.buf[8] = '/';
        int len = fmt_u32_dec(&key_buf.buf[9], i);
        key_buf.buf[9 + len] = '\0';
        size_t size = sizeof(*ap);
        if (configuration_get(&key_buf, tmp, &size)) {
            continue;
        }
        if (!_wifi_configuration_ap_valid(tmp->ssid.ssid, &tmp->sec.sec)) {
            if (new < 0) {
                new = i;
            }
        }
        else if (!strcmp(tmp->ssid.ssid, ap->ssid.ssid)) {
            size = sizeof(*ap);
            key_buf.buf[8] = '/';
            ret = configuration_set(&key_buf, ap, &size);
            break;
        }
    }
    if (ret == -ENOENT && new >= 0) {
        key_buf.buf[8] = '/';
        int len = fmt_u32_dec(&key_buf.buf[9], new);
        key_buf.buf[9 + len] = '\0';
        size_t size = sizeof(*ap);
        if (!(ret = configuration_set(&key_buf, ap, &size))) {
            if (be_sync) {
                ret = configuration_export(&key_buf);
            }
        }
    }
    key_buf.buf[8] = '\0';
    _wifi_configuration_ap_unlock(&key_buf);
    return ret;
}

int wifi_configuration_ap_remove(const wifi_conf_ap_t *ap, bool be_sync)
{
    CONF_KEY_T(32) key_buf = CONF_KEY_INITIALIZER(32, "/wifi/ap");
    wifi_conf_ap_t *tmp;
    if (!_wifi_configuration_ap_valid(ap->ssid.ssid, &ap->sec.sec)) {
        return -EINVAL;
    }
    if (!(tmp = _wifi_configuration_ap_lock(&key_buf))) {
        return -ENOENT;
    }
    int ret = -ENOENT;
    int remove = -1;
    for (unsigned i = 0; i < ARRAY_SIZE(_wifi_conf_ap_table); i++) {
        key_buf.buf[8] = '/';
        int len = fmt_u32_dec(&key_buf.buf[9], i);
        key_buf.buf[9 + len] = '\0';
        size_t size = sizeof(*ap);
        if (configuration_get(key_buf.buf, tmp, &size)) {
            continue;
        }
        if (strcmp(tmp->ssid.ssid, ap->ssid.ssid)) {
            continue;
        }
        remove = i;
        break;
    }
    if (remove >= 0) {
        key_buf.buf[8] = '/';
        int len = fmt_u32_dec(&key_buf.buf[9], remove);
        key_buf.buf[9 + len] = '\0';
        if (!(ret = configuration_set(&key_buf, NULL, NULL))) {
            if (be_sync) {
                ret = configuration_delete(&key_buf);
            }
        }
    }
    key_buf.buf[8] = '\0';
    _wifi_configuration_ap_unlock(&key_buf);
    return ret;
}

int wifi_configuration_ap_load_all(void)
{
    CONF_KEY_T(32) key_buf = CONF_KEY_INITIALIZER(32, "/wifi/ap");
    if (!_wifi_configuration_ap_lock(&key_buf)) {
        return -ENOENT;
    }
    memset(_wifi_conf_ap_table, 0, sizeof(_wifi_conf_ap_table));
    for (unsigned i = 0; i < ARRAY_SIZE(_wifi_conf_ap_table); i++) {
        key_buf.buf[8] = '/';
        int len = fmt_u32_dec(&key_buf.buf[9], i);
        key_buf.buf[9 + len] = '\0';
        configuration_import(&key_buf);
    }
    key_buf.buf[8] = '\0';
    _wifi_configuration_ap_unlock(&key_buf);
    return 0;
}

int wifi_configuration_ap_save_all(void)
{
    CONF_KEY_T(32) key_buf = CONF_KEY_INITIALIZER(32, "/wifi/ap");
    if (_wifi_configuration_ap_lock(&key_buf)) {
        return -ENOENT;
    }
    for (unsigned i = 0; i < ARRAY_SIZE(_wifi_conf_ap_table); i++) {
        key_buf.buf[8] = '/';
        int len = fmt_u32_dec(&key_buf.buf[9], i);
        key_buf.buf[9 + len] = '\0';
        if (_wifi_conf_ap_table[i].ssid.ssid[0]) {
            configuration_export(&key_buf);
        }
        else {
            configuration_delete(&key_buf);
        }
    }
    key_buf.buf[8] = '\0';
    _wifi_configuration_ap_unlock(&key_buf);
    return 0;
}

void auto_init_configuration_wifi(void)
{
    _wifi_conf_ap_handler.handler.src_backend = wifi_conf_ap_backend_init();
    _wifi_conf_ap_ssid_handler.src_backend = wifi_conf_ap_ssid_backend_init();
    _wifi_conf_ap_sec_handler.src_backend = wifi_conf_ap_sec_backend_init();
    wifi_configuration_register();
    wifi_configuration_ap_load_all();
}

#ifndef AUTO_INIT_PRIO_MOD_CONFIGURATION_WIFI
#define AUTO_INIT_PRIO_MOD_CONFIGURATION_WIFI       1200
#endif

AUTO_INIT_CONFIGURATION(auto_init_configuration_wifi,
                        AUTO_INIT_PRIO_MOD_CONFIGURATION_WIFI);
