// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef RIOT_VERSION
#define ENABLE_DEBUG 0
#include "debug.h"
#include "esp_common.h"
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "rom/ets_sys.h"

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#ifndef RIOT_VERSION
#include "tcpip_adapter.h"
#endif
#include "esp_log.h"

static const char* TAG = "event";

#define WIFI_API_CALL_CHECK(info, api_call, ret) \
do{\
    esp_err_t __err = (api_call);\
    if ((ret) != __err) {\
        ESP_LOGE(TAG, "%s %d %s ret=0x%X", __FUNCTION__, __LINE__, (info), __err);\
        return __err;\
    }\
} while(0)

typedef esp_err_t (*system_event_handler_t)(system_event_t *e);

static esp_err_t system_event_ap_start_handle_default(system_event_t *event);
static esp_err_t system_event_ap_stop_handle_default(system_event_t *event);
static esp_err_t system_event_sta_start_handle_default(system_event_t *event);
static esp_err_t system_event_sta_stop_handle_default(system_event_t *event);
static esp_err_t system_event_sta_connected_handle_default(system_event_t *event);
static esp_err_t system_event_sta_disconnected_handle_default(system_event_t *event);
static esp_err_t system_event_sta_got_ip_default(system_event_t *event);
static esp_err_t system_event_sta_lost_ip_default(system_event_t *event);

/* Default event handler functions

   Any entry in this table which is disabled by config will have a NULL handler.
*/
static system_event_handler_t default_event_handlers[SYSTEM_EVENT_MAX] = { 0 };

static esp_err_t system_event_sta_got_ip_default(system_event_t *event)
{
    #ifndef RIOT_VERSION /* TODO implement */
    ESP_LOGI(TAG, "sta ip: " IPSTR ", mask: " IPSTR ", gw: " IPSTR,
           IP2STR(&event->event_info.got_ip.ip_info.ip),
           IP2STR(&event->event_info.got_ip.ip_info.netmask),
           IP2STR(&event->event_info.got_ip.ip_info.gw));
    #else
    ESP_LOGI(TAG, "%s", __func__);
    #endif

    return ESP_OK;
}

static esp_err_t system_event_sta_lost_ip_default(system_event_t *event)
{
    ESP_LOGI(TAG, "station ip lost");
    return ESP_OK;
}

esp_err_t system_event_ap_start_handle_default(system_event_t *event)
{
    #ifndef RIOT_VERSION /* TODO implement */
    tcpip_adapter_ip_info_t ap_ip;
    uint8_t ap_mac[6];

    WIFI_API_CALL_CHECK("esp_wifi_mac_get",  esp_wifi_get_mac(ESP_IF_WIFI_AP, ap_mac), ESP_OK);

    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ap_ip);
    tcpip_adapter_start(TCPIP_ADAPTER_IF_AP, ap_mac,  &ap_ip);
    #else
    ESP_LOGI(TAG, "%s", __func__);
    #endif

    return ESP_OK;
}

esp_err_t system_event_ap_stop_handle_default(system_event_t *event)
{
    #ifndef RIOT_VERSION /* TODO implement */
    tcpip_adapter_stop(TCPIP_ADAPTER_IF_AP);
    #else
    ESP_LOGI(TAG, "%s", __func__);
    #endif

    return ESP_OK;
}

esp_err_t system_event_sta_start_handle_default(system_event_t *event)
{
    #ifndef RIOT_VERSION /* TODO implement */
    tcpip_adapter_ip_info_t sta_ip;
    uint8_t sta_mac[6];

    WIFI_API_CALL_CHECK("esp_wifi_mac_get",  esp_wifi_get_mac(ESP_IF_WIFI_STA, sta_mac), ESP_OK);
    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &sta_ip);
    tcpip_adapter_start(TCPIP_ADAPTER_IF_STA, sta_mac, &sta_ip);
    #else
    ESP_LOGI(TAG, "%s", __func__);
    #endif

    return ESP_OK;
}

esp_err_t system_event_sta_stop_handle_default(system_event_t *event)
{
    #ifndef RIOT_VERSION /* TODO implement */
    tcpip_adapter_stop(TCPIP_ADAPTER_IF_STA);
    #else
    ESP_LOGI(TAG, "%s", __func__);
    #endif

    return ESP_OK;
}

esp_err_t system_event_sta_connected_handle_default(system_event_t *event)
{
    #ifndef RIOT_VERSION /* TODO implement */
    tcpip_adapter_dhcp_status_t status;

    tcpip_adapter_up(TCPIP_ADAPTER_IF_STA);
    tcpip_adapter_dhcpc_get_status(TCPIP_ADAPTER_IF_STA, &status);

    if (status == TCPIP_ADAPTER_DHCP_INIT) {
        tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_STA);
    } else if (status == TCPIP_ADAPTER_DHCP_STOPPED) {
        tcpip_adapter_ip_info_t sta_ip;
        tcpip_adapter_ip_info_t sta_old_ip;

        tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &sta_ip);
        tcpip_adapter_get_old_ip_info(TCPIP_ADAPTER_IF_STA, &sta_old_ip);

        if (!(ip4_addr_isany_val(sta_ip.ip) || ip4_addr_isany_val(sta_ip.netmask) || ip4_addr_isany_val(sta_ip.gw))) {
            system_event_t evt;

            evt.event_id = SYSTEM_EVENT_STA_GOT_IP;
            evt.event_info.got_ip.ip_changed = false;

            if (memcmp(&sta_ip, &sta_old_ip, sizeof(sta_ip))) {
                evt.event_info.got_ip.ip_changed = true;
            }

            memcpy(&evt.event_info.got_ip.ip_info, &sta_ip, sizeof(tcpip_adapter_ip_info_t));
            tcpip_adapter_set_old_ip_info(TCPIP_ADAPTER_IF_STA, &sta_ip);

            esp_event_send(&evt);
            ESP_LOGD(TAG, "static ip: ip changed=%d", evt.event_info.got_ip.ip_changed);
        } else {
            ESP_LOGE(TAG, "invalid static ip");
        }
    }
    #else
    ESP_LOGI(TAG, "%s", __func__);
    #endif

    return ESP_OK;
}

esp_err_t system_event_sta_disconnected_handle_default(system_event_t *event)
{
    #ifndef RIOT_VERSION /* TODO implement */
    tcpip_adapter_down(TCPIP_ADAPTER_IF_STA);
    #else
    ESP_LOGI(TAG, "%s", __func__);
    #endif
    return ESP_OK;
}

static esp_err_t esp_system_event_debug(system_event_t *event)
{
    if (event == NULL) {
        ESP_LOGE(TAG, "event is null!");
        printf("event is null!");
        return ESP_FAIL;
    }

    switch (event->event_id) {
    case SYSTEM_EVENT_WIFI_READY: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_WIFI_READY");
        break;
    }
    case SYSTEM_EVENT_SCAN_DONE: {
        system_event_sta_scan_done_t *scan_done = &event->event_info.scan_done;
        (void)scan_done; /* to avoid compile error: unused variable */
        ESP_LOGD(TAG, "SYSTEM_EVENT_SCAN_DONE, status:%d, number:%d",  scan_done->status, scan_done->number);
        break;
    }
    case SYSTEM_EVENT_STA_START: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_START");
        break;
    }
    case SYSTEM_EVENT_STA_STOP: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_STOP");
        break;
    }
    case SYSTEM_EVENT_STA_CONNECTED: {
        system_event_sta_connected_t *connected = &event->event_info.connected;
        (void)connected; /* to avoid compile error: unused variable */
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_CONNECTED, ssid:%s, ssid_len:%d, bssid:" MACSTR ", channel:%d, authmode:%d", \
                   connected->ssid, connected->ssid_len, MAC2STR(connected->bssid), connected->channel, connected->authmode);
        break;
    }
    case SYSTEM_EVENT_STA_DISCONNECTED: {
        system_event_sta_disconnected_t *disconnected = &event->event_info.disconnected;
        (void)disconnected; /* to avoid compile error: unused variable */
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_DISCONNECTED, ssid:%s, ssid_len:%d, bssid:" MACSTR ", reason:%d", \
                   disconnected->ssid, disconnected->ssid_len, MAC2STR(disconnected->bssid), disconnected->reason);
        break;
    }
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE: {
        system_event_sta_authmode_change_t *auth_change = &event->event_info.auth_change;
        (void)auth_change; /* to avoid compile error: unused variable */
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_AUTHMODE_CHNAGE, old_mode:%d, new_mode:%d", auth_change->old_mode, auth_change->new_mode);
        break;
    }
    case SYSTEM_EVENT_STA_GOT_IP: {
        #ifndef RIOT_VERSION /* TODO implement */
        system_event_sta_got_ip_t *got_ip = &event->event_info.got_ip;
        (void)got_ip; /* to avoid compile error: unused variable */
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_GOT_IP, ip:" IPSTR ", mask:" IPSTR ", gw:" IPSTR,
            IP2STR(&got_ip->ip_info.ip),
            IP2STR(&got_ip->ip_info.netmask),
            IP2STR(&got_ip->ip_info.gw));
        #else
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        #endif
        break;
    }
    case SYSTEM_EVENT_STA_LOST_IP: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_LOST_IP");
        break;
    }
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
        break;
    }
    case SYSTEM_EVENT_STA_WPS_ER_FAILED: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_WPS_ER_FAILED");
        break;
    }
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
        break;
    }
    case SYSTEM_EVENT_STA_WPS_ER_PIN: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_STA_WPS_ER_PIN");
        break;
    }
    case SYSTEM_EVENT_AP_START: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_AP_START");
        break;
    }
    case SYSTEM_EVENT_AP_STOP: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_AP_STOP");
        break;
    }
    case SYSTEM_EVENT_AP_STACONNECTED: {
        system_event_ap_staconnected_t *staconnected = &event->event_info.sta_connected;
        (void)staconnected; /* to avoid compile error: unused variable */
        ESP_LOGD(TAG, "SYSTEM_EVENT_AP_STACONNECTED, mac:" MACSTR ", aid:%d", \
                   MAC2STR(staconnected->mac), staconnected->aid);
        break;
    }
    case SYSTEM_EVENT_AP_STADISCONNECTED: {
        system_event_ap_stadisconnected_t *stadisconnected = &event->event_info.sta_disconnected;
        (void)stadisconnected; /* to avoid compile error: unused variable */
        ESP_LOGD(TAG, "SYSTEM_EVENT_AP_STADISCONNECTED, mac:" MACSTR ", aid:%d", \
                   MAC2STR(stadisconnected->mac), stadisconnected->aid);
        break;
    }
    case SYSTEM_EVENT_AP_STAIPASSIGNED: {
        ESP_LOGD(TAG, "SYSTEM_EVENT_AP_STAIPASSIGNED");
        break;
    }
    case SYSTEM_EVENT_AP_PROBEREQRECVED: {
        system_event_ap_probe_req_rx_t *ap_probereqrecved = &event->event_info.ap_probereqrecved;
        (void)ap_probereqrecved; /* to avoid compile error: unused variable */
        ESP_LOGD(TAG, "SYSTEM_EVENT_AP_PROBEREQRECVED, rssi:%d, mac:" MACSTR, \
                   ap_probereqrecved->rssi, \
                   MAC2STR(ap_probereqrecved->mac));
        break;
    }
#if ESP_EVENT_IPV6
    case SYSTEM_EVENT_GOT_IP6: {
        #ifndef RIOT_VERSION /* TODO implement */
        ip6_addr_t *addr = &event->event_info.got_ip6.ip6_info.ip;
        ESP_LOGD(TAG, "SYSTEM_EVENT_AP_STA_GOT_IP6 address %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
                 IP6_ADDR_BLOCK1(addr),
                 IP6_ADDR_BLOCK2(addr),
                 IP6_ADDR_BLOCK3(addr),
                 IP6_ADDR_BLOCK4(addr),
                 IP6_ADDR_BLOCK5(addr),
                 IP6_ADDR_BLOCK6(addr),
                 IP6_ADDR_BLOCK7(addr),
                 IP6_ADDR_BLOCK8(addr));
        #else
        ESP_LOGD(TAG, "SYSTEM_EVENT_AP_STA_GOT_IP6 address ");
        #endif
        break;
    }
#endif

    
    default: {
        ESP_LOGW(TAG, "unexpected system event %d!", event->event_id);
        break;
    }
    }

    return ESP_OK;
}

esp_err_t esp_event_process_default(system_event_t *event)
{
    if (event == NULL) {
        ESP_LOGE(TAG, "Error: event is null!");
        return ESP_FAIL;
    }

    esp_system_event_debug(event);
    if ((event->event_id < SYSTEM_EVENT_MAX)) {
        if (default_event_handlers[event->event_id] != NULL) {
            ESP_LOGV(TAG, "enter default callback");
            default_event_handlers[event->event_id](event);
            ESP_LOGV(TAG, "exit default callback");
        }
    } else {
        ESP_LOGE(TAG, "mismatch or invalid event, id=%d", event->event_id);
        return ESP_FAIL;
    }
    return ESP_OK;
}

void esp_event_set_default_wifi_handlers(void)
{
     default_event_handlers[SYSTEM_EVENT_STA_START]        = system_event_sta_start_handle_default;
     default_event_handlers[SYSTEM_EVENT_STA_STOP]         = system_event_sta_stop_handle_default;
     default_event_handlers[SYSTEM_EVENT_STA_CONNECTED]    = system_event_sta_connected_handle_default;
     default_event_handlers[SYSTEM_EVENT_STA_DISCONNECTED] = system_event_sta_disconnected_handle_default;
     default_event_handlers[SYSTEM_EVENT_STA_GOT_IP]       = system_event_sta_got_ip_default;
     default_event_handlers[SYSTEM_EVENT_STA_LOST_IP]      = system_event_sta_lost_ip_default;
     default_event_handlers[SYSTEM_EVENT_AP_START]         = system_event_ap_start_handle_default;
     default_event_handlers[SYSTEM_EVENT_AP_STOP]          = system_event_ap_stop_handle_default;

     //esp_register_shutdown_handler((shutdown_handler_t)esp_wifi_stop);
}
