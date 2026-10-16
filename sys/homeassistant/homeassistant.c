/*
 * Copyright (C) 2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     module_homeassistant
 * @{
 *
 * @file
 * @brief       homeassistant implementation
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>
#include "fmt.h"
#include "luid.h"
#include "homeassistant.h"
#include "mjson.h"


#ifndef HASS_RIOT_DEV_NAME
#ifndef HASS_RIOT_UID_LEN
#define HASS_RIOT_UID_LEN               6
#endif

#ifndef HASS_RIOT_DEV_PREFIX
#define HASS_RIOT_DEV_PREFIX            "RIOT-"
#endif

static char hass_id[sizeof(HASS_RIOT_DEV_PREFIX) + HASS_RIOT_UID_LEN] = HASS_RIOT_DEV_PREFIX;
#endif /* HASS_RIOT_DEV_NAME */

static const char *_entity_names[] = {
    [HOMEASSISTANT_ENTITY_TYPE_SENSOR] = "sensor",
};

static void _init_hass_dev_name(void)
{
#ifndef HASS_RIOT_DEV_NAME
    size_t buf_len = (HASS_RIOT_UID_LEN + 1) / 2;
    uint8_t luid_buf[buf_len];

    luid_custom(luid_buf, buf_len, 0);

    fmt_bytes_hex(hass_id + sizeof(HASS_RIOT_DEV_PREFIX) - 1, luid_buf, buf_len);
#endif
}

static const char *_get_hass_dev_name(void)
{
#ifndef HASS_RIOT_DEV_NAME
    return hass_id;
#else
    return HASS_RIOT_DEV_NAME;
#endif
}

static const char *_get_entity_type_as_string(const homeassistant_resource_t *resource)
{
    return _entity_names[resource->type];
}

static void _unspace_hass_topic(char *topic)
{
    char *pos = strchr(topic, ' ');
    while (pos) {
        *pos = '_';
        pos = strchr(pos, ' ');
    }
}

static int _format_hass_discovery_topic(const homeassistant_resource_t *resource, char *buf, size_t buf_len)
{
    int res = snprintf(buf, buf_len, "%s/%s/%s/%s/config",
            HASS_DISCOVERY_PREFIX,
            _get_entity_type_as_string(resource),
            _get_hass_dev_name(),
            resource->name);
    _unspace_hass_topic(buf);
    return res;
}

static int _format_hass_topic_prefix(const saul_reg_t *reg, char *buf, size_t buf_len)
{
    int res = snprintf(buf, buf_len, "%s/%s", _get_hass_dev_name(), reg->name);
    _unspace_hass_topic(buf);
    return res;
}

static int _gen_hass_discovery_payload(const saul_reg_t *reg, char *buf, size_t buf_len)
{
    static char prefix[32];
    _format_hass_topic_prefix(reg, prefix, sizeof(prefix));
    int res = mjson_snprintf(buf, buf_len,
            "{\"~\":%Q,\"name\":%Q,\"stat_t\":\"~/state\"", prefix, reg->name);

    if (res < 0) {
        return res;
    }

    if ((reg->driver->type & SAUL_CAT_MASK) == SAUL_CAT_ACT) {
        res += mjson_snprintf(buf + res, buf_len - res, ",\"cmd_t\":\"~/set\"");
    }
    res += mjson_snprintf(buf + res, buf_len - res, "}");
    return res;
}

int homeassistant_add_sensor(homeassistant_t *hass)
{
}

int homeassistant_add_actuator(homeassistant_t *hass)
{
}

int homeassistant_remove_device(homeassistant_t *hass)
{
}

void homeassistant_init(homeassistant_t *hass, MQTTClient *client)
{
    hass->mqttclient = client;
    hass->resources = NULL;
}

/* Implementation of the module */
