/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "kernel_defines.h"

#if IS_USED(MODULE_ESP_WIFI_ANY) || IS_USED(MODULE_ESP_ETH)

#include <string.h>

#include "buildinfo/cpu.h"
#include "esp_common.h"
#include "log.h"

#include "esp_attr.h"
#ifdef CPU_ESP8266
#include "esp_event_loop.h"
#else
#include "esp_eth_com.h"
#include "esp_event.h"
#include "esp_event_legacy.h"
#endif
#include "irq_arch.h"

#define ENABLE_DEBUG 0
#include "debug.h"

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

#ifdef CPU_ESP8266
static esp_err_t esp_system_event_handler(void *ctx, system_event_t *event)
{
    for (int i = 0; i < MAX_HANDLER_NUM; i++) {
        if (_handler[i] != NULL) {
            _handler[i](_handler_arg[i], event);
        }
    }
    return ESP_OK;
}
#else /* CPU_ESP8266 */
/* The event loop API of newer ESP-IDF versions split system events into
 * different event bases types. For code compatibility, we have to map
 * new event base types to the former system events */
static void esp_system_event_handler(void* arg,
                                     esp_event_base_t event_base,
                                     int32_t event_id,
                                     void* event_data)
{
    system_event_t sys_event = { .event_id = SYSTEM_EVENT_MAX };

#if IS_USED(MODULE_ESP_WIFI_ANY)
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
        case WIFI_EVENT_WIFI_READY:
            sys_event.event_id = SYSTEM_EVENT_WIFI_READY;
            break;
        case WIFI_EVENT_SCAN_DONE:
            sys_event.event_id = SYSTEM_EVENT_SCAN_DONE;
            sys_event.event_info.scan_done
                        = *((system_event_sta_scan_done_t *)event_data);
            break;
        case WIFI_EVENT_STA_START:
            sys_event.event_id = SYSTEM_EVENT_STA_START;
            break;
        case WIFI_EVENT_STA_STOP:
            sys_event.event_id = SYSTEM_EVENT_STA_STOP;
            break;
        case WIFI_EVENT_STA_CONNECTED:
            sys_event.event_id = SYSTEM_EVENT_STA_CONNECTED;
            sys_event.event_info.connected
                        = *((system_event_sta_connected_t *)event_data);
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            sys_event.event_id = SYSTEM_EVENT_STA_DISCONNECTED;
            sys_event.event_info.disconnected
                        = *((system_event_sta_disconnected_t *)event_data);
            break;
        case WIFI_EVENT_STA_AUTHMODE_CHANGE:
            sys_event.event_id = SYSTEM_EVENT_STA_AUTHMODE_CHANGE;
            sys_event.event_info.auth_change
                        = *((system_event_sta_authmode_change_t *)event_data);
            break;
        case WIFI_EVENT_STA_WPS_ER_SUCCESS:
            sys_event.event_id = SYSTEM_EVENT_STA_WPS_ER_SUCCESS;
             sys_event.event_info.sta_er_success
                        = *((system_event_sta_wps_er_success_t *)event_data);
           break;
        case WIFI_EVENT_STA_WPS_ER_FAILED:
            sys_event.event_id = SYSTEM_EVENT_STA_WPS_ER_FAILED;
            sys_event.event_info.sta_er_fail_reason
                        = *((system_event_sta_wps_fail_reason_t *)event_data);
            break;
        case WIFI_EVENT_STA_WPS_ER_TIMEOUT:
            sys_event.event_id = SYSTEM_EVENT_STA_WPS_ER_TIMEOUT;
            break;
        case WIFI_EVENT_STA_WPS_ER_PIN:
            sys_event.event_id = SYSTEM_EVENT_STA_WPS_ER_PIN;
            sys_event.event_info.sta_er_pin
                        = *((system_event_sta_wps_er_pin_t *)event_data);
            break;
        case WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP:
            sys_event.event_id = SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP;
            break;
        case WIFI_EVENT_AP_START:
            sys_event.event_id = SYSTEM_EVENT_AP_START;
            break;
        case WIFI_EVENT_AP_STOP:
            sys_event.event_id = SYSTEM_EVENT_AP_STOP;
            break;
        case WIFI_EVENT_AP_STACONNECTED:
            sys_event.event_id = SYSTEM_EVENT_AP_STACONNECTED;
            sys_event.event_info.sta_connected
                        = *((system_event_ap_staconnected_t *)event_data);
            break;
        case WIFI_EVENT_AP_STADISCONNECTED:
            sys_event.event_id = SYSTEM_EVENT_AP_STADISCONNECTED;
            sys_event.event_info.sta_disconnected
                        = *((system_event_ap_stadisconnected_t *)event_data);
            break;
        case WIFI_EVENT_AP_PROBEREQRECVED:
            sys_event.event_id = SYSTEM_EVENT_AP_PROBEREQRECVED;
            sys_event.event_info.ap_probereqrecved
                        = *((system_event_ap_probe_req_rx_t *)event_data);
            break;
        case WIFI_EVENT_FTM_REPORT:
            sys_event.event_id = SYSTEM_EVENT_FTM_REPORT;
            sys_event.event_info.ftm_report
                        = *((system_event_ftm_report_t *)event_data);
            break;
        case WIFI_EVENT_STA_BSS_RSSI_LOW:
            sys_event.event_id = SYSTEM_EVENT_STA_BSS_RSSI_LOW;
            break;
        case WIFI_EVENT_ACTION_TX_STATUS:
            sys_event.event_id = SYSTEM_EVENT_ACTION_TX_STATUS;
            break;
        case WIFI_EVENT_ROC_DONE:
            sys_event.event_id = SYSTEM_EVENT_ROC_DONE;
            break;
        case WIFI_EVENT_STA_BEACON_TIMEOUT:
            sys_event.event_id = SYSTEM_EVENT_STA_BEACON_TIMEOUT;
            break;
        }
    }
#endif

#if IS_USED(MODULE_ESP_ETH)
    if (event_base == ETH_EVENT) {
        switch (event_id) {
        case ETHERNET_EVENT_START:
            sys_event.event_id = SYSTEM_EVENT_ETH_START;
            break;
        case ETHERNET_EVENT_STOP:
            sys_event.event_id = SYSTEM_EVENT_ETH_STOP;
            break;
        case ETHERNET_EVENT_CONNECTED:
            sys_event.event_id = SYSTEM_EVENT_ETH_CONNECTED;
            break;
        case ETHERNET_EVENT_DISCONNECTED:
            sys_event.event_id = SYSTEM_EVENT_ETH_DISCONNECTED;
            break;
        }
    }
#endif

    if (sys_event.event_id == SYSTEM_EVENT_MAX) {
        return;
    }

    for (int i = 0; i < MAX_HANDLER_NUM; i++) {
        if (_handler[i] != NULL) {
            _handler[i](_handler_arg[i], &sys_event);
        }
    }
}
#endif /* CPU_ESP8266 */
#endif /* IS_USED(MODULE_ESP_WIFI_ANY) || IS_USED(MODULE_ESP_ETH) */

void esp_event_handler_init(void)
{
#if IS_USED(MODULE_ESP_WIFI_ANY) || IS_USED(MODULE_ESP_ETH)
#ifdef CPU_ESP8266
    esp_event_loop_init(esp_system_event_handler, NULL);
#else
    /* newer ESP-IDF versions use another event loop API that have to be used */
    esp_event_loop_create_default();
    if (IS_USED(MODULE_ESP_WIFI_ANY)) {
        esp_event_handler_instance_register(WIFI_EVENT,
                                            ESP_EVENT_ANY_ID,
                                            esp_system_event_handler,
                                            NULL, NULL);
    }
    if (IS_USED(MODULE_ESP_ETH)) {
        esp_event_handler_instance_register(ETH_EVENT,
                                            ESP_EVENT_ANY_ID,
                                            esp_system_event_handler,
                                            NULL, NULL);
    }
#endif
#endif
}
