/*
 * Copyright (C) 2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_homeassistant homeassistant
 * @ingroup     sys
 * @brief       HomeAssistant MQTT integration for RIOT
 *
 * This module provides integration with Home-Assistant via an MQTT broker. This module supports the
 * MQTT discovery mechanism used by Home-Assistant to discover sensors and actuators on the RIOT
 * node and automatically add and integrate them into Home-Assistant.
 *
 * @see https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery
 *
 * @{
 *
 * @file
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef HOMEASSISTANT_H
#define HOMEASSISTANT_H

#include "clist.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_HASS_DISCOVERY_PREFIX
#define CONFIG_HASS_DISCOVERY_PREFIX           "homeassistant"
#endif

typedef enum {
    HOMEASSISTANT_ENTITY_TYPE_SENSOR,
} homeassistant_entity_type_t;

typedef struct homeassistant_resource homeassistant_resource_t;

struct homeassistant_resource {
    clist_t next;
    int (*write)(void);
    const char *name;
    homeassistant_entity_type_t type;
};

typedef struct {
    MQTTClient *mqttclient;
    homeassistant_resource_t *resources;
} homeassistant_t;



#ifdef __cplusplus
}
#endif

#endif /* HOMEASSISTANT_H */
/** @} */
