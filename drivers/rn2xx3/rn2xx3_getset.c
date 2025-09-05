/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_rn2xx3
 * @{
 * @file
 * @brief       Implementation of get and set functions for RN2XX3
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <string.h>
#include <errno.h>

#include "fmt.h"

#include "net/loramac.h"

#include "rn2xx3.h"
#include "rn2xx3_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static const char *mac = "mac";
static const char *get = "get";
static const char *set = "set";

/* internal helpers */
static uint8_t _get_uint8_value(rn2xx3_t *dev, const char *command)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1,
                        "%s %s %s", mac, get, command);
    dev->cmd_buf[p] = 0;

    rn2xx3_write_cmd(dev);

    return atoi(dev->resp_buf);
}

static void _set_uint8_value(rn2xx3_t *dev,
                             const char *command, uint8_t value)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1, "%s %s %s %d",
                        mac, set, command, value);
    dev->cmd_buf[p] = 0;

    rn2xx3_write_cmd(dev);
}

static uint16_t _get_uint16_value(rn2xx3_t *dev, const char *command)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1,
                        "%s %s %s", mac, get, command);
    dev->cmd_buf[p] = 0;

    rn2xx3_write_cmd(dev);

    return atoi(dev->resp_buf);
}

static void _set_uint16_value(rn2xx3_t *dev,
                              const char *command, uint16_t value)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1, "%s %s %s %i",
                        mac, set, command, value);
    dev->cmd_buf[p] = 0;

    rn2xx3_write_cmd(dev);
}

static bool _get_bool_value(rn2xx3_t *dev, const char *command)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1,
                        "%s %s %s", mac, get, command);
    dev->cmd_buf[p] = 0;

    rn2xx3_write_cmd(dev);

    return strcmp(dev->resp_buf, "on") == 0;
}

static void _set_bool_value(rn2xx3_t *dev,
                            const char *command, bool value)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1, "%s %s %s %s",
                        mac, set, command, value ? "on": "off");
    dev->cmd_buf[p] = 0;

    rn2xx3_write_cmd(dev);
}

static void _get_array_value(rn2xx3_t *dev, const char *command,
                             uint8_t *value)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1,
                        "%s %s %s", mac, get, command);
    dev->cmd_buf[p] = 0;

    rn2xx3_write_cmd(dev);

    fmt_hex_bytes(value, dev->resp_buf);

}

static void _set_array_value(rn2xx3_t *dev, const char *command,
                             const uint8_t *value, uint8_t value_len)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1, "%s %s %s ",
                        mac, set, command);
    dev->cmd_buf[p] = 0;

    rn2xx3_cmd_start(dev);
    rn2xx3_cmd_append(dev, value, value_len);
    rn2xx3_cmd_finalize(dev);
}

void rn2xx3_mac_get_deveui(rn2xx3_t *dev, uint8_t *eui)
{
    _get_array_value(dev, "deveui", eui);
}

void rn2xx3_mac_set_deveui(rn2xx3_t *dev, const uint8_t *eui)
{
    _set_array_value(dev, "deveui", eui, LORAMAC_DEVEUI_LEN);
}

void rn2xx3_mac_get_appeui(rn2xx3_t *dev, uint8_t *eui)
{
    _get_array_value(dev, "appeui", eui);
}

void rn2xx3_mac_set_appeui(rn2xx3_t *dev, const uint8_t *eui)
{
    _set_array_value(dev, "appeui", eui, LORAMAC_APPEUI_LEN);
}

void rn2xx3_mac_set_appkey(rn2xx3_t *dev, const uint8_t *key)
{
    _set_array_value(dev, "appkey", key, LORAMAC_APPKEY_LEN);
}

void rn2xx3_mac_set_appskey(rn2xx3_t *dev, const uint8_t *key)
{
    _set_array_value(dev, "appskey", key, LORAMAC_APPSKEY_LEN);
}

void rn2xx3_mac_set_nwkskey(rn2xx3_t *dev, const uint8_t *key)
{
    _set_array_value(dev, "nwkskey", key, LORAMAC_NWKSKEY_LEN);
}

void rn2xx3_mac_get_devaddr(rn2xx3_t *dev, uint8_t *addr)
{
    _get_array_value(dev, "devaddr", addr);
}

void rn2xx3_mac_set_devaddr(rn2xx3_t *dev, const uint8_t *addr)
{
    _set_array_value(dev, "devaddr", addr, 4);
}

loramac_tx_pwr_idx_t rn2xx3_mac_get_tx_power(rn2xx3_t *dev)
{
    return _get_uint8_value(dev, "pwridx");
}

void rn2xx3_mac_set_tx_power(rn2xx3_t *dev, loramac_tx_pwr_idx_t power)
{
    _set_uint8_value(dev, "pwridx", power);
}

loramac_dr_idx_t rn2xx3_mac_get_dr(rn2xx3_t *dev)
{
    return _get_uint8_value(dev, "dr");
}

void rn2xx3_mac_set_dr(rn2xx3_t *dev, loramac_dr_idx_t datarate)
{
    _set_uint8_value(dev, "dr", datarate);
}

uint16_t rn2xx3_mac_get_band(rn2xx3_t *dev)
{
    return _get_uint16_value(dev, "band");
}

bool rn2xx3_mac_get_adr(rn2xx3_t *dev)
{
    return _get_bool_value(dev, "adr");
}

void rn2xx3_mac_set_adr(rn2xx3_t *dev, bool adr)
{
    _set_bool_value(dev, "adr", adr);
}

void rn2xx3_mac_set_battery(rn2xx3_t *dev, uint8_t battery)
{
    _set_uint8_value(dev, "bat", battery);
}

uint8_t rn2xx3_mac_get_retx(rn2xx3_t *dev)
{
    return _get_uint8_value(dev, "retx");
}

void rn2xx3_mac_set_retx(rn2xx3_t *dev, uint8_t retx)
{
    _set_uint8_value(dev, "retx", retx);
}

void rn2xx3_mac_set_linkchk_interval(rn2xx3_t *dev, uint16_t linkchk)
{
    _set_uint16_value(dev, "linkchk", linkchk);
}

uint16_t rn2xx3_mac_get_rx1_delay(rn2xx3_t *dev)
{
    return _get_uint16_value(dev, "rxdelay1");
}

void rn2xx3_mac_set_rx1_delay(rn2xx3_t *dev, uint16_t rx1)
{
    _set_uint16_value(dev, "rxdelay1", rx1);
}

uint16_t rn2xx3_mac_get_rx2_delay(rn2xx3_t *dev)
{
    return _get_uint16_value(dev, "rxdelay2");
}

bool rn2xx3_mac_get_ar(rn2xx3_t *dev)
{
    return _get_bool_value(dev, "ar");
}

void rn2xx3_mac_set_ar(rn2xx3_t *dev, bool ar)
{
    _set_bool_value(dev, "ar", ar);
}

loramac_dr_idx_t rn2xx3_mac_get_rx2_dr(rn2xx3_t *dev)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1, "%s %s %s %d",
                        mac, get, "rx2", RN2XX3_FREQ_BAND);
    dev->cmd_buf[p] = 0;

    rn2xx3_write_cmd(dev);

    loramac_dr_idx_t dr = strtol(dev->resp_buf, NULL, 10);

    return dr;
}

void rn2xx3_mac_set_rx2_dr(rn2xx3_t *dev, loramac_dr_idx_t dr)
{
    dev->loramac.rx2_dr = dr;
    dev->loramac.rx2_freq = rn2xx3_mac_get_rx2_freq(dev);

    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1,
                        "%s %s %s %d %lu",
                        mac, set, "rx2", dr,
                        (unsigned long)dev->loramac.rx2_freq);
    dev->cmd_buf[p] = 0;

    rn2xx3_write_cmd(dev);
}

uint32_t rn2xx3_mac_get_rx2_freq(rn2xx3_t *dev)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1,
                        "%s %s %s %d", mac, get, "rx2", RN2XX3_FREQ_BAND);
    dev->cmd_buf[p] = 0;

    rn2xx3_write_cmd(dev);

    uint32_t freq = strtoul(dev->resp_buf + 2, NULL, 10);

    return freq;
}

void rn2xx3_mac_set_rx2_freq(rn2xx3_t *dev, uint32_t freq)
{
    dev->loramac.rx2_freq = freq;
    dev->loramac.rx2_dr = rn2xx3_mac_get_rx2_dr(dev);

    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1,
                        "%s %s %s %d %lu",
                        mac, set, "rx2", dev->loramac.rx2_dr,
                        (unsigned long)freq);
    dev->cmd_buf[p] = 0;

    rn2xx3_write_cmd(dev);
}

uint8_t rn2xx3_mac_get_tx_port(rn2xx3_t *dev)
{
    return dev->loramac.tx_port;
}

void rn2xx3_mac_set_tx_port(rn2xx3_t *dev, uint8_t port)
{
    dev->loramac.tx_port = port;
}

loramac_tx_mode_t rn2xx3_mac_get_tx_mode(rn2xx3_t *dev)
{
    return dev->loramac.tx_mode;
}

void rn2xx3_mac_set_tx_mode(rn2xx3_t *dev, loramac_tx_mode_t mode)
{
    dev->loramac.tx_mode = mode;
}

uint8_t rn2xx3_mac_get_rx_port(rn2xx3_t *dev)
{
    return strtoul(&dev->resp_buf[6], NULL, 10);
}

void rn2xx3_sys_set_sleep_duration(rn2xx3_t *dev, uint32_t sleep)
{
    if (sleep < RN2XX3_SLEEP_MIN) {
        DEBUG("[rn2xx3] sleep: duration should be greater than 100 (ms)\n");
        return;
    }

    dev->sleep = sleep;
}
