/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mkrwan
 * @{
 *
 * @file
 * @brief       Device driver implementation for the MKRWAN LoRa radio.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "mkrwan.h"
#include "xtimer.h"
#include "fmt.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

static const char *deveui_cmd       = "DEVEUI";
static const char *appeui_cmd       = "APPEUI";
static const char *appkey_cmd       = "APPKEY";
static const char *devaddr_cmd      = "DEVADDR";
static const char *appskey_cmd      = "APPSKEY";
static const char *nwkskey_cmd      = "NWKSKEY";
static const char *class_cmd        = "CLASS";
static const char *dcycle_cmd       = "DUTYCYCLE";
static const char *nwk_cmd          = "NWK";
static const char *dr_cmd           = "DR";
static const char *adr_cmd          = "ADR";
static const char *port_cmd         = "PORT";

static char tmp_buf[MKRWAN_TMP_BUF_SIZE];

static void _set_array(mkrwan_t *dev, const char *cmd,
                       uint8_t *array, size_t len)
{
    char *pos = tmp_buf;
    pos += fmt_str(pos, "AT+");
    pos += fmt_str(pos, cmd);
    pos += fmt_str(pos, "=");
    pos += fmt_bytes_hex(pos, array, len);
    *pos = '\0';

    DEBUG("[mkrwan] set array cmd: %s\n", tmp_buf);

    at_send_cmd_wait_ok(&dev->at_dev, tmp_buf, MKRWAN_TIMEOUT);
}

static void _get_array(mkrwan_t *dev, const char *cmd,
                       uint8_t *array)
{
    char *pos = tmp_buf;
    pos += fmt_str(pos, "AT+");
    pos += fmt_str(pos, cmd);
    pos += fmt_str(pos, "?");
    *pos = '\0';

    DEBUG("[mkrwan] get array cmd: %s\n", tmp_buf);

    at_send_cmd(&dev->at_dev, tmp_buf, MKRWAN_TIMEOUT);

    at_readline(&dev->at_dev, tmp_buf, sizeof(tmp_buf),
                false, MKRWAN_TIMEOUT);

    DEBUG("[mkrwan] get array resp: %s\n", tmp_buf);

    fmt_hex_bytes(array, &tmp_buf[4]);
}

static void _set_integer(mkrwan_t *dev, const char *cmd, uint8_t integer)
{
    char *pos = tmp_buf;
    pos += fmt_str(pos, "AT+");
    pos += fmt_str(pos, cmd);
    pos += fmt_str(pos, "=");
    pos += fmt_u16_dec(pos, integer);
    *pos = '\0';

    DEBUG("[mkrwan] set integer cmd: %s\n", tmp_buf);

    at_send_cmd_wait_ok(&dev->at_dev, tmp_buf, MKRWAN_TIMEOUT);
}

static uint8_t _get_integer(mkrwan_t *dev, const char *cmd)
{
    char *pos = tmp_buf;
    pos += fmt_str(pos, "AT+");
    pos += fmt_str(pos, cmd);
    pos += fmt_str(pos, "?");
    *pos = '\0';

    DEBUG("[mkrwan] get integer cmd: %s\n", tmp_buf);

    at_send_cmd(&dev->at_dev, tmp_buf, MKRWAN_TIMEOUT);
    at_readline(&dev->at_dev, tmp_buf, sizeof(tmp_buf),
                false, MKRWAN_TIMEOUT);

    DEBUG("[mkrwan] get integer resp: %s\n", tmp_buf);

    return atoi(tmp_buf);
}

static void _set_bool(mkrwan_t *dev, const char *cmd, bool enable)
{
    _set_integer(dev, cmd, (uint8_t)enable);
}

static bool _get_bool(mkrwan_t *dev, const char *cmd)
{
    char *pos = tmp_buf;
    pos += fmt_str(pos, "AT+");
    pos += fmt_str(pos, cmd);
    pos += fmt_str(pos, "?");
    *pos = '\0';

    DEBUG("[mkrwan] get bool cmd: %s\n", tmp_buf);

    at_send_cmd_get_resp(&dev->at_dev, tmp_buf,
                         tmp_buf, sizeof(tmp_buf), MKRWAN_JOIN_TIMEOUT);

    DEBUG("[mkrwan] get bool resp: %s\n", tmp_buf);

    return (bool)atoi(&tmp_buf[4]);
}

void mkrwan_set_deveui(mkrwan_t *dev, uint8_t *eui)
{
    _set_array(dev, deveui_cmd, eui, LORAMAC_DEVEUI_LEN);
}

void mkrwan_get_deveui(mkrwan_t *dev, uint8_t *eui)
{
    _get_array(dev, deveui_cmd, eui);
}

void mkrwan_set_appeui(mkrwan_t *dev, uint8_t *eui)
{
    _set_array(dev, appeui_cmd, eui, LORAMAC_APPEUI_LEN);
}

void mkrwan_get_appeui(mkrwan_t *dev, uint8_t *eui)
{
    _get_array(dev, appeui_cmd, eui);
}

void mkrwan_set_appkey(mkrwan_t *dev, uint8_t *key)
{
    _set_array(dev, appkey_cmd, key, LORAMAC_APPKEY_LEN);
}

void mkrwan_get_appkey(mkrwan_t *dev, uint8_t *key)
{
    _get_array(dev, appkey_cmd, key);
}

void mkrwan_set_devaddr(mkrwan_t *dev, uint8_t *addr)
{
    _set_array(dev, devaddr_cmd, addr, LORAMAC_DEVADDR_LEN);
}

void mkrwan_get_devaddr(mkrwan_t *dev, uint8_t *addr)
{
    _get_array(dev, devaddr_cmd, addr);
}

void mkrwan_set_appskey(mkrwan_t *dev, uint8_t *skey)
{
    _set_array(dev, appskey_cmd, skey, LORAMAC_APPSKEY_LEN);
}

void mkrwan_get_appskey(mkrwan_t *dev, uint8_t *skey)
{
    _get_array(dev, appskey_cmd, skey);
}

void mkrwan_set_nwkskey(mkrwan_t *dev, uint8_t *skey)
{
    _set_array(dev, nwkskey_cmd, skey, LORAMAC_NWKSKEY_LEN);
}

void mkrwan_get_nwkskey(mkrwan_t *dev, uint8_t *skey)
{
    _get_array(dev, nwkskey_cmd, skey);
}

void mkrwan_set_class(mkrwan_t *dev, uint8_t class)
{
    char *pos = tmp_buf;
    pos += fmt_str(pos, "AT+");
    pos += fmt_str(pos, class_cmd);
    pos += fmt_str(pos, "=");
    switch (class) {
        case LORAMAC_CLASS_A:
            *pos++ = 'A';
            break;
        case LORAMAC_CLASS_B:
            *pos++ = 'B';
            break;
        case LORAMAC_CLASS_C:
            *pos++ = 'C';
            break;
    }
    *pos = '\0';

    DEBUG("[mkrwan] set sleep cmd: %s\n", tmp_buf);

    at_send_cmd_wait_ok(&dev->at_dev, tmp_buf, MKRWAN_TIMEOUT);
}

uint8_t mkrwan_get_class(mkrwan_t *dev)
{
    char *pos = tmp_buf;
    pos += fmt_str(pos, "AT+");
    pos += fmt_str(pos, class_cmd);
    pos += fmt_str(pos, "?");
    *pos = '\0';

    int res = at_send_cmd_get_resp(&dev->at_dev, tmp_buf,
                                   tmp_buf, sizeof(tmp_buf), MKRWAN_TIMEOUT);
    if (res > 0) {
        DEBUG("[mkrwan] get class resp: %s\n", tmp_buf);
        if (strcmp(&tmp_buf[4], "A") == 0) {
            return LORAMAC_CLASS_A;
        }
        else if (strcmp(&tmp_buf[4], "B") == 0) {
            return LORAMAC_CLASS_B;
        }
        else {
            return LORAMAC_CLASS_C;
        }
    }

    return LORAMAC_CLASS_A;
}

void mkrwan_set_duty_cycle(mkrwan_t *dev, bool enable)
{
    _set_bool(dev, dcycle_cmd, enable);
}

bool mkrwan_get_duty_cycle(mkrwan_t *dev)
{
    return _get_bool(dev, dcycle_cmd);
}

void mkrwan_set_public_network(mkrwan_t *dev, bool enable)
{
    _set_bool(dev, nwk_cmd, enable);
}

bool mkrwan_get_public_network(mkrwan_t *dev)
{
    return _get_bool(dev, nwk_cmd);
}

void mkrwan_set_datarate(mkrwan_t *dev, loramac_dr_idx_t dr)
{
    _set_integer(dev, dr_cmd, dr);
}

loramac_dr_idx_t mkrwan_get_datarate(mkrwan_t *dev)
{
    return _get_integer(dev, dr_cmd);
}

void mkrwan_set_adr(mkrwan_t *dev, bool enable)
{
    _set_bool(dev, adr_cmd, enable);
}

bool mkrwan_get_adr(mkrwan_t *dev)
{
    return _get_bool(dev, adr_cmd);
}

void mkrwan_set_tx_port(mkrwan_t *dev, uint8_t port)
{
    _set_integer(dev, port_cmd, port);
}

uint8_t mkrwan_get_tx_port(mkrwan_t *dev)
{
    return _get_integer(dev, port_cmd);
}
