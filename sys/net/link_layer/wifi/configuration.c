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
#include <nanocbor/nanocbor.h>

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

/* TODO: refine */
#define WIFI_CONF_AP_ENC_MAX        256

enum {
    _WIFI_CONF_AP_SSID_ID,
    _WIFI_CONF_AP_SEC_ID,
};

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

static wifi_conf_ap_t _wifi_conf_ap_table[WIFI_CONF_STORED_AP_NUMOF];

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
static int _wifi_ap_conf_encode(const conf_handler_t *handler,
                                conf_key_buf_t *key,
                                const void **enc_data, size_t *enc_size);
static int _wifi_ap_conf_decode(const conf_handler_t *handler,
                                conf_key_buf_t *key,
                                void **dec_data, size_t *dec_size);

static const conf_handler_ops_t _wifi_conf_ap_handler_ops = {
    .set = _wifi_ap_conf_set,
    .get = _wifi_ap_conf_get,
    .import = _wifi_ap_conf_import,
    .export = _wifi_ap_conf_export,
    .delete = _wifi_ap_conf_delete,
};

static const conf_handler_data_ops_t _wifi_conf_ap_handler_data_ops = {
    .encode = _wifi_ap_conf_encode,
    .decode = _wifi_ap_conf_decode,
};

/* WiFI access point encoding/decoding buffer */
static uint8_t _wifi_ap_buf[WIFI_CONF_AP_ENC_MAX];
/* do not copy to real configuration value destination to consider encoding failure */
static wifi_conf_ap_t _ap_table[WIFI_CONF_STORED_AP_NUMOF];

/* /wifi/ap */
static conf_array_handler_t _wifi_conf_ap_handler
    = CONF_ARRAY_HANDLER_INITIALIZER("ap",
                                     &_wifi_conf_ap_handler_ops,
                                     &_wifi_conf_ap_handler_data_ops,
                                     sizeof(_wifi_conf_ap_table[0]),
                                     _wifi_conf_ap_table,
                                     ARRAY_SIZE(_wifi_conf_ap_table),
                                     0);

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
static int _wifi_ap_conf_encode_ssid(const conf_handler_t *handler,
                                     conf_key_buf_t *key,
                                     const void **enc_data, size_t *enc_size);
static int _wifi_ap_conf_decode_ssid(const conf_handler_t *handler,
                                     conf_key_buf_t *key,
                                     void **dec_data, size_t *dec_size);

static const conf_handler_ops_t _wifi_conf_ap_ssid_handler_ops = {
    .set = _wifi_ap_conf_set_ssid,
    .get = _wifi_ap_conf_get_ssid,
    .import = _wifi_ap_conf_import_ssid,
    .export = _wifi_ap_conf_export_ssid,
    .delete = _wifi_ap_conf_delete_ssid,
};

static const conf_handler_data_ops_t _wifi_conf_ap_ssid_handler_data_ops = {
    .encode = _wifi_ap_conf_encode_ssid,
    .decode = _wifi_ap_conf_decode_ssid,
};

/* /wifi/ap/ssid */
static conf_handler_t _wifi_conf_ap_ssid_handler
    = CONF_HANDLER_INITIALIZER("ssid",
                               &_wifi_conf_ap_ssid_handler_ops,
                               &_wifi_conf_ap_ssid_handler_data_ops,
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
static int _wifi_ap_conf_encode_sec(const conf_handler_t *handler,
                                    conf_key_buf_t *key,
                                    const void **enc_data, size_t *enc_size);
static int _wifi_ap_conf_decode_sec(const conf_handler_t *handler,
                                    conf_key_buf_t *key,
                                    void **dec_data, size_t *dec_size);

static const conf_handler_ops_t _wifi_conf_ap_sec_handler_ops = {
    .set = _wifi_ap_conf_set_sec,
    .get = _wifi_ap_conf_get_sec,
    .import = _wifi_ap_conf_import_sec,
    .export = _wifi_ap_conf_export_sec,
    .delete = _wifi_ap_conf_delete_sec,
};

static const conf_handler_data_ops_t _wifi_conf_ap_sec_handler_data_ops = {
    .encode = _wifi_ap_conf_encode_sec,
    .decode = _wifi_ap_conf_decode_sec,
};

/* /wifi/ap/sec */
static conf_handler_t _wifi_conf_ap_sec_handler
    = CONF_HANDLER_INITIALIZER("sec",
                               &_wifi_conf_ap_sec_handler_ops,
                               &_wifi_conf_ap_sec_handler_data_ops,
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

static int _encode_ssid(const wifi_conf_ap_t *ap,
                        nanocbor_encoder_t *enc_parent)
{
    if (nanocbor_put_tstr(enc_parent, ap->ssid.ssid) < 0) {
        return -ENOMEM;
    }
    return 0;
}

static int _encode_sec(const wifi_conf_ap_t *ap,
                       nanocbor_encoder_t *enc_parent)
{
    if (nanocbor_fmt_map(enc_parent, 1) < 0) {
        return -ENOMEM;
    }
    if (nanocbor_fmt_uint(enc_parent, ap->sec.sec) < 0) {
        return -ENOMEM;
    }
    if (ap->sec.sec == WIFI_SECURITY_MODE_WEP_PSK) {
        if (nanocbor_put_tstr(enc_parent, ap->sec.wep.psk) < 0) {
            return -ENOMEM;
        }
    }
    else if (ap->sec.sec == WIFI_SECURITY_MODE_WPA2_PERSONAL) {
        if (nanocbor_put_tstr(enc_parent, ap->sec.wpa_psk.psk) < 0) {
            return -ENOMEM;
        }
    }
    else if (ap->sec.sec == WIFI_SECURITY_MODE_WPA2_ENTERPRISE) {
        if (nanocbor_put_tstr(enc_parent, ap->sec.wpa_enterprise.user) < 0) {
            return -ENOMEM;
        }
        if (nanocbor_put_tstr(enc_parent, ap->sec.wpa_enterprise.pwd) < 0) {
            return -ENOMEM;
        }
    }
    return 0;
}

static int _wifi_ap_conf_encode_one(const wifi_conf_ap_t *ap,
                                    nanocbor_encoder_t *enc_parent)
{
/*
    A2                                          # map(2)
        00                                      # unsigned(0)
        67                                      # text(7)
            <SSID>                              # "<SSID>"
        01                                      # unsigned(1)
        A1                                      # map(1)
            02                                  # unsigned(2)
            70                                  # text(16)
                <PASSWORD>                      # "<PASSWORD>"

    {0: "<SSID>", 1: {2: "<PASSWORD>"}}
*/
    if (nanocbor_fmt_map(enc_parent, 2) < 0) {
        return -ENOMEM;
    }
    if (nanocbor_fmt_uint(enc_parent, _WIFI_CONF_AP_SSID_ID) < 0) {
        return -ENOMEM;
    }
    if (_encode_ssid(ap, enc_parent) < 0) {
        return -ENOMEM;
    }
    if (nanocbor_fmt_uint(enc_parent, _WIFI_CONF_AP_SEC_ID) < 0) {
        return -ENOMEM;
    }
    if (_encode_sec(ap, enc_parent) < 0) {
        return -ENOMEM;
    }
    return 0;
}

static int _wifi_ap_conf_encode(const conf_handler_t *handler,
                                conf_key_buf_t *key,
                                const void **enc_data, size_t *enc_size)
{
    assert(handler); (void)handler;
    assert(key); (void)key;
    assert(enc_data);
    assert(enc_size);

    nanocbor_encoder_t enc;
    nanocbor_encoder_init(&enc, _wifi_ap_buf, sizeof(_wifi_ap_buf));
    memset(_wifi_ap_buf, 0, sizeof(_wifi_ap_buf));

    if (key->offset >= 0) {
        if ((unsigned)key->offset >= ARRAY_SIZE(_wifi_conf_ap_table)) {
            return -EINVAL;
        }
        /* encode one */
        if (_wifi_ap_conf_encode_one(&_wifi_conf_ap_table[key->offset], &enc) < 0) {
            return -ENOMEM;
        }
    }
    else {
        /* encode all */
        if (nanocbor_fmt_array_indefinite(&enc) < 0) {
            return -ENOMEM;
        }
        for (unsigned i = 0; i < ARRAY_SIZE(_wifi_conf_ap_table); i++) {
            if (_wifi_ap_conf_encode_one(&_wifi_conf_ap_table[i], &enc)) {
                return -ENOMEM;
            }
        }
        if (nanocbor_fmt_end_indefinite(&enc) < 0) {
            return -ENOMEM;
        }
    }

    *enc_data = _wifi_ap_buf;
    *enc_size = nanocbor_encoded_len(&enc);
    return 0;
}

static int _wifi_ap_conf_encode_ssid(const conf_handler_t *handler,
                                     conf_key_buf_t *key,
                                     const void **enc_data, size_t *enc_size)
{
    assert(handler); (void)handler;
    assert(key); (void)key;
    assert(enc_data);
    assert(enc_size);
    assert(key->offset >= 0);

    if ((unsigned)key->offset >= ARRAY_SIZE(_wifi_conf_ap_table)) {
        return -EINVAL;
    }
    const wifi_conf_ap_t *ap = &_wifi_conf_ap_table[key->offset];
    nanocbor_encoder_t enc;
    nanocbor_encoder_init(&enc, _wifi_ap_buf, sizeof(_wifi_ap_buf));
    memset(_wifi_ap_buf, 0, sizeof(_wifi_ap_buf));
    if (_encode_ssid(ap, &enc) < 0) {
        return -ENOMEM;
    }

    *enc_data = _wifi_ap_buf;
    *enc_size = nanocbor_encoded_len(&enc);
    return 0;
}

static int _wifi_ap_conf_encode_sec(const conf_handler_t *handler,
                                    conf_key_buf_t *key,
                                    const void **enc_data, size_t *enc_size)
{
    assert(handler); (void)handler;
    assert(key); (void)key;
    assert(enc_data);
    assert(enc_size);
    assert(key->offset >= 0);

    if ((unsigned)key->offset >= ARRAY_SIZE(_wifi_conf_ap_table)) {
        return -EINVAL;
    }
    const wifi_conf_ap_t *ap = &_wifi_conf_ap_table[key->offset];
    nanocbor_encoder_t enc;
    nanocbor_encoder_init(&enc, _wifi_ap_buf, sizeof(_wifi_ap_buf));
    memset(_wifi_ap_buf, 0, sizeof(_wifi_ap_buf));
    if (_encode_sec(ap, &enc) < 0) {
        return -ENOMEM;
    }

    *enc_data = _wifi_ap_buf;
    *enc_size = nanocbor_encoded_len(&enc);
    return 0;
}

static int _decode_ssid(nanocbor_value_t *dec_parent,
                        wifi_conf_ap_t *ap)
{
    const uint8_t *dec_str;
    size_t len;
    if (nanocbor_get_tstr(dec_parent, &dec_str, &len) != NANOCBOR_OK) {
        return -EINVAL;
    }
    if (len >= sizeof(ap->ssid.ssid)) {
        return -EINVAL;
    }
    memcpy(ap->ssid.ssid, dec_str, len);
    ap->ssid.ssid[len] = '\0';
    return 0;
}

static int _decode_sec(nanocbor_value_t *dec_parent,
                       wifi_conf_ap_t *ap)
{
    nanocbor_value_t dec;
    if (nanocbor_enter_map(dec_parent, &dec) != NANOCBOR_OK) {
        return -EINVAL;
    }
    uint32_t idx;
    if (nanocbor_get_uint32(&dec, &idx) < 0) {
        return -EINVAL;
    }
    size_t len;
    const uint8_t *str;
    if (idx == WIFI_SECURITY_MODE_WEP_PSK) {
        if (nanocbor_get_tstr(&dec, &str, &len) != NANOCBOR_OK) {
            return -EINVAL;
        }
        if (len >= sizeof(ap->sec.wep.psk)) {
            return -EINVAL;
        }
        memcpy(&ap->sec.wep.psk, str, len);
        ap->sec.wep.psk[len] = '\0';
        ap->sec.sec = WIFI_SECURITY_MODE_WEP_PSK;
    }
    else if (idx == WIFI_SECURITY_MODE_WPA2_PERSONAL) {
        if (nanocbor_get_tstr(&dec, &str, &len) != NANOCBOR_OK) {
            return -EINVAL;
        }
        if (len >= sizeof(ap->sec.wpa_psk.psk)) {
            return -EINVAL;
        }
        memcpy(&ap->sec.wpa_psk.psk, str, len);
        ap->sec.wpa_psk.psk[len] = '\0';
        ap->sec.sec = WIFI_SECURITY_MODE_WPA2_PERSONAL;
    }
    else if (idx == WIFI_SECURITY_MODE_WPA2_ENTERPRISE) {
        if (nanocbor_get_tstr(&dec, &str, &len) != NANOCBOR_OK) {
            return -EINVAL;
        }
        if (len >= sizeof(ap->sec.wpa_enterprise.user)) {
            return -EINVAL;
        }
        memcpy(&ap->sec.wpa_enterprise.user, str, len);
        ap->sec.wpa_enterprise.user[len] = '\0';
        if (nanocbor_get_tstr(&dec, &str, &len) != NANOCBOR_OK) {
            return -EINVAL;
        }
        if (len >= sizeof(ap->sec.wpa_enterprise.pwd)) {
            return -EINVAL;
        }
        memcpy(&ap->sec.wpa_enterprise.pwd, str, len);
        ap->sec.wpa_enterprise.pwd[len] = '\0';
        ap->sec.sec = WIFI_SECURITY_MODE_WPA2_ENTERPRISE;
    }
    else if (idx != WIFI_SECURITY_MODE_OPEN) {
        return -EINVAL;
    }
    nanocbor_leave_container(dec_parent, &dec);
    return 0;
}

static int _wifi_ap_conf_decode_one(wifi_conf_ap_t *ap,
                                    nanocbor_value_t *dec_parent)
{
    nanocbor_value_t dec_map;
    if (nanocbor_enter_map(dec_parent, &dec_map) != NANOCBOR_OK) {
        return -EINVAL;
    }
    uint32_t idx;
    while (!nanocbor_at_end(&dec_map)) {
        if (nanocbor_get_uint32(&dec_map, &idx) < 0) {
            return -EINVAL;
        }
        if (idx == _WIFI_CONF_AP_SSID_ID) {
            if (_decode_ssid(&dec_map, ap) < 0) {
                return -EINVAL;
            }
        }
        else if (idx == _WIFI_CONF_AP_SEC_ID) {
            if (_decode_sec(&dec_map, ap) < 0) {
                return -EINVAL;
            }
        }
        else {
            nanocbor_skip(&dec_map);
        }
    }
    return 0;
}

static int _wifi_ap_conf_decode(const conf_handler_t *handler,
                                conf_key_buf_t *key,
                                void **dec_data, size_t *dec_size)
{
    assert(handler); (void)handler;
    assert(key); (void)key;
    assert(dec_data);
    assert(dec_size);

    if (*dec_data == NULL) {
        *dec_data = _wifi_ap_buf;
        *dec_size = sizeof(_wifi_ap_buf);
        return 0;
    }

    nanocbor_value_t dec;
    nanocbor_decoder_init(&dec, *dec_data, *dec_size);
    if (key->offset >= 0) {
        /* decode one */
        if (_wifi_ap_conf_decode_one(&_ap_table[0], &dec) < 0) {
            return -EINVAL;
        }

        *dec_data = _ap_table;
        *dec_size = sizeof(_ap_table[0]);
    }
    else {
        nanocbor_value_t dec_array;
        if (nanocbor_enter_array(&dec, &dec_array) != NANOCBOR_OK) {
            return -EINVAL;
        }
        unsigned num = 0;
        while (!nanocbor_at_end(&dec_array)) {
            wifi_conf_ap_t ap = { 0 };
            if (_wifi_ap_conf_decode_one(&ap, &dec_array)) {
                return -EINVAL;
            }
            if (num < ARRAY_SIZE(_ap_table)) {
                return -ENOMEM;
            }
            memcpy(&_ap_table[num], &ap, sizeof(ap));
        }
        nanocbor_leave_container(&dec, &dec_array);

        *dec_data = _ap_table;
        *dec_size = num * sizeof(_ap_table[0]);
    }

    return 0;
}


static int _wifi_ap_conf_decode_ssid(const conf_handler_t *handler,
                                     conf_key_buf_t *key,
                                     void **dec_data, size_t *dec_size)
{
    assert(handler); (void)handler;
    assert(key); (void)key;
    assert(dec_data);
    assert(dec_size);
    assert(key->offset >= 0);

    if (*dec_data == NULL) {
        *dec_data = _wifi_ap_buf;
        *dec_size = sizeof(_wifi_ap_buf);
        return 0;
    }

    nanocbor_value_t dec;
    nanocbor_decoder_init(&dec, *dec_data, *dec_size);
    wifi_conf_ap_t *ap = &_ap_table[key->offset];
    if (_decode_ssid(&dec, ap) < 0) {
        return -EINVAL;
    }

    *dec_data = &_ap_table[key->offset].ssid;
    *dec_size = sizeof(_ap_table[key->offset].ssid);
    return 0;
}


static int _wifi_ap_conf_decode_sec(const conf_handler_t *handler,
                                    conf_key_buf_t *key,
                                    void **dec_data, size_t *dec_size)
{
    assert(handler); (void)handler;
    assert(key); (void)key;
    assert(dec_data);
    assert(dec_size);

    if (*dec_data == NULL) {
        *dec_data = _wifi_ap_buf;
        *dec_size = sizeof(_wifi_ap_buf);
        return 0;
    }

    nanocbor_value_t dec;
    nanocbor_decoder_init(&dec, *dec_data, *dec_size);
    wifi_conf_ap_t *ap = &_ap_table[key->offset];
    if (_decode_sec(&dec, ap)) {
        return -ENOMEM;
    }

    *dec_data = &_ap_table[key->offset].sec;
    *dec_size = sizeof(_ap_table[key->offset].sec);
    return 0;
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
