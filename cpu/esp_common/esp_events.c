/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       ESP system event handler
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#if defined(MODULE_ESP_WIFI_ANY) || defined(MODULE_ESP_ETH)

#define ENABLE_DEBUG 0
#include "debug.h"

#include <string.h>

#include "esp_common.h"
#include "log.h"

#include "esp_attr.h"
#include "esp_event_loop.h"
#include "irq_arch.h"

#define MAX_HANDLER_NUM     5

static system_event_cb_t _handler[MAX_HANDLER_NUM] = {};
static void* _handler_arg[MAX_HANDLER_NUM] = {};

esp_err_t esp_system_event_add_handler (system_event_cb_t handler, void *arg)
{
    int i;

    /* determine next free handler entry */
    for (i = 0; i < MAX_HANDLER_NUM; i++) {
        if (_handler[i] == NULL) {
            break;
        }
    }

    /* return if there is no free entry */
    if (i == MAX_HANDLER_NUM) {
        return ESP_FAIL;
    }

    /* set the handler and argument entry */
    _handler[i] = handler;
    _handler_arg[i] = arg;

    return ESP_OK;
}

esp_err_t esp_system_event_del_handler (system_event_cb_t handler)
{
    int i;

    /* determine the handler entry */
    for (i = 0; i < MAX_HANDLER_NUM; i++) {
        if (_handler[i] == handler) {
            break;
        }
    }

    /* return if entry was not found */
    if (i == MAX_HANDLER_NUM) {
        return ESP_FAIL;
    }

    /* clean handler and arg entry */
    _handler[i] = NULL;
    _handler_arg[i] = NULL;

    return ESP_OK;
}

static esp_err_t esp_system_event_handler(void *ctx, system_event_t *event)
{
    for (int i = 0; i < MAX_HANDLER_NUM; i++) {
        if (_handler[i] != NULL) {
            _handler[i](_handler_arg[i], event);
        }
    }
    return ESP_OK;
}

#endif

void esp_event_handler_init(void)
{
    #if defined(MODULE_ESP_WIFI_ANY) || defined(MODULE_ESP_ETH)
    esp_event_loop_init(esp_system_event_handler, NULL);
    #endif
}
