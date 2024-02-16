/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common_esp_now
 * @{
 *
 * @file
 * @brief       Netdev interface for the ESP-NOW WiFi P2P protocol
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Timo Rothenpieler <timo.rothenpieler@uni-bremen.de>
 */

#include "log.h"
#include "tools.h"

#include <string.h>
#include <errno.h>

#include "net/gnrc.h"
#include "ztimer.h"

#include "esp_common.h"
#include "esp_attr.h"
#ifdef CPU_ESP8266
#include "esp_event_loop.h"
#else
#include "esp_event.h"
#endif
#include "esp_now.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "irq_arch.h"
#include "od.h"

#include "nvs_flash.h"

#include "esp_now_params.h"
#include "esp_now_netdev.h"

#define ENABLE_DEBUG             0
#include "debug.h"

#define ESP_NOW_UNICAST          (1)

#define ESP_NOW_WIFI_STA         (1)
#define ESP_NOW_WIFI_SOFTAP      (2)
#define ESP_NOW_WIFI_STA_SOFTAP  (ESP_NOW_WIFI_STA + ESP_NOW_WIFI_SOFTAP)

#define ESP_NOW_AP_PREFIX        "RIOT_ESP_"
#define ESP_NOW_AP_PREFIX_LEN    (strlen(ESP_NOW_AP_PREFIX))

#if MODULE_ESP_WIFI && !ESP_NOW_UNICAST
#error If module esp_wifi is used, module esp_now has to be used in unicast mode
#endif

/**
 * There is only one ESP-NOW device. We define it as static device variable
 * to have access to the device inside ESP-NOW interrupt routines which do
 * not provide an argument that could be used as pointer to the ESP-NOW
 * device which triggers the interrupt.
 */
static esp_now_netdev_t _esp_now_dev = { 0 };
static const netdev_driver_t _esp_now_driver;

static bool _esp_now_add_peer(const uint8_t* bssid, uint8_t channel, const uint8_t* key)
{
    if (esp_now_is_peer_exist(bssid)) {
        return false;
    }

    esp_now_peer_info_t peer = {};

    memcpy(peer.peer_addr, bssid, ESP_NOW_ETH_ALEN);
    peer.channel = channel;
    peer.ifidx = WIFI_IF_AP;

    if (esp_now_params.key) {
        peer.encrypt = true;
        memcpy(peer.lmk, esp_now_params.key, ESP_NOW_KEY_LEN);
    }

    esp_err_t ret = esp_now_add_peer(&peer);
    DEBUG("esp_now_add_peer node %02x:%02x:%02x:%02x:%02x:%02x "
          "added with return value %d\n",
          bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], ret);
    return (ret == ESP_OK);
}

#if ESP_NOW_UNICAST

static ztimer_t _esp_now_scan_peers_timer;
static bool _esp_now_scan_peers_done = false;

#define ESP_NOW_APS_BLOCK_SIZE 8 /* has to be power of two */

static wifi_ap_record_t* aps = NULL;
static uint32_t aps_size = 0;

static wifi_scan_config_t scan_cfg = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = ESP_NOW_CHANNEL,
        .show_hidden = true,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time.active.min = 0,
        .scan_time.active.max = 120 /* TODO tune value */
};

static void IRAM_ATTR esp_now_scan_peers_done(void)
{
    mutex_lock(&_esp_now_dev.dev_lock);

    esp_err_t ret;
    uint16_t ap_num;

    ret = esp_wifi_scan_get_ap_num(&ap_num);
    DEBUG("wifi_scan_get_ap_num ret=%d num=%d\n", ret, ap_num);

    if (ret == ESP_OK && ap_num) {
        uint32_t state;
        /* reallocation of memory must not be disturbed */
        critical_enter_var(state);
        /* allocate memory for APs record list blockwise and fetch them the list */
        if (ap_num > aps_size) {
            if (aps) {
                /* free allocated AP record list memory */
                aps_size = 0;
                free (aps);
            }
            /* allocate new memory */
            aps_size = (ap_num & ~(ESP_NOW_APS_BLOCK_SIZE - 1)) + ESP_NOW_APS_BLOCK_SIZE;
            aps = malloc(sizeof(wifi_ap_record_t) * aps_size);
            ap_num = aps_size;
        }
        critical_exit_var(state);

        ret = esp_wifi_scan_get_ap_records(&ap_num, aps);

        DEBUG("wifi_scan_get_aps ret=%d num=%d\n", ret, ap_num);

        critical_enter_var(state);
        /* iterate over APs records */
        for (uint16_t i = 0; i < ap_num; i++) {

            /* check whether the AP is an ESP_NOW node */
            if (strncmp((char*)aps[i].ssid, ESP_NOW_AP_PREFIX, ESP_NOW_AP_PREFIX_LEN) == 0) {
                /* add the AP as peer */
                _esp_now_add_peer(aps[i].bssid, aps[i].primary, esp_now_params.key);
            }
        }
        critical_exit_var(state);
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        esp_now_peer_num_t peer_num;
        esp_now_get_peer_num(&peer_num);
        DEBUG("associated peers total=%d, encrypted=%d\n",
               peer_num.total_num, peer_num.encrypt_num);
    }

    _esp_now_scan_peers_done = true;

    mutex_unlock(&_esp_now_dev.dev_lock);
}

static void esp_now_scan_peers_start(void)
{
    DEBUG("%s\n", __func__);

    /* start the scan */
    esp_wifi_scan_start(&scan_cfg, false);
    /* set the time for next scan */
    ztimer_set(ZTIMER_MSEC, &_esp_now_scan_peers_timer, esp_now_params.scan_period);
}

static void IRAM_ATTR esp_now_scan_peers_timer_cb(void* arg)
{
    DEBUG("%s\n", __func__);

    netdev_t *netdev = arg;
    esp_now_netdev_t *dev = container_of(netdev, esp_now_netdev_t, netdev);

    if (dev->netdev.event_callback) {
        dev->scan_event++;
        netdev_trigger_event_isr(&dev->netdev);
    }
}

#else /* ESP_NOW_UNICAST */

static const uint8_t _esp_now_mac[6] = { 0x82, 0x73, 0x79, 0x84, 0x79, 0x83 }; /* RIOTOS */

#endif /* ESP_NOW_UNICAST */

static bool _in_recv_cb = false;

static IRAM_ATTR void esp_now_recv_cb(const uint8_t *mac, const uint8_t *data, int len)
{
#if ESP_NOW_UNICAST
    if (!_esp_now_scan_peers_done) {
        /* if peers are not scanned, we cannot receive anything */
        return;
    }
#endif /* ESP_NOW_UNICAST */

    /*
     * The function `esp_now_recv_cb` is executed in the context of the `wifi`
     * thread. The ISRs handling the hardware interrupts from the WiFi
     * interface pass events to a message queue of the `wifi` thread which is
     * sequentially processed by the `wifi` thread to asynchronously execute
     * callback functions such as `esp_now_recv_cb`.
     *
     * It should be therefore not possible to reenter function
     * `esp_now_recv_cb`. To avoid inconsistencies this is checked by an
     * additional boolean variable . This can not be realized by a mutex
     * because `esp_now_recv_cb` would be reentered from same thread context.
     */
    if (_in_recv_cb) {
        return;
    }
    _in_recv_cb = true;

    /*
     * Since it is not possible to reenter the function `esp_now_recv_cb`, and
     * the functions netif::_ recv and esp_now_netdev::_ recv are called
     * directly in the same thread context, neither a mutual exclusion has to
     * be realized nor have the interrupts to be deactivated.
     * Therefore we can read directly from the `buf` and don't need a receive
     * buffer.
     */
    if (_esp_now_dev.rx_len) {
        DEBUG("%s: buffer full, dropping incoming packet of %d bytes\n", __func__, len);
        _in_recv_cb = false;
        return;
    }

    _esp_now_dev.rx_mac = (uint8_t*)mac;
    _esp_now_dev.rx_data = (uint8_t*)data;
    _esp_now_dev.rx_len = len;

    /*
     * Since we are not in the interrupt context, we do not have to pass
     * `NETDEV_EVENT_ISR` first. We can call the receive function directly.
     */
    if (_esp_now_dev.netdev.event_callback) {
        _esp_now_dev.netdev.event_callback(&_esp_now_dev.netdev,
                                           NETDEV_EVENT_RX_COMPLETE);
    }

    _in_recv_cb = false;
}

static volatile int _esp_now_sending = 0;

static void IRAM_ATTR esp_now_send_cb(const uint8_t *mac, esp_now_send_status_t status)
{
    DEBUG("%s: sent to %02x:%02x:%02x:%02x:%02x:%02x with status %d\n",
          __func__,
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], status);

    if (_esp_now_sending) {
        _esp_now_sending--;
    }
}

/*
 * Event handler for esp system events.
 */
static esp_err_t IRAM_ATTR _esp_system_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            DEBUG("%s WiFi started\n", __func__);
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            DEBUG("%s WiFi scan done\n", __func__);
#if ESP_NOW_UNICAST
            esp_now_scan_peers_done();
#endif /* ESP_NOW_UNICAST */
            break;
        default:
            break;
    }
    return ESP_OK;
}

extern esp_err_t esp_system_event_add_handler(system_event_cb_t handler,
                                              void *arg);

/* ESP-NOW SoftAP configuration */
static wifi_config_t wifi_config_ap = {};

esp_now_netdev_t *netdev_esp_now_setup(void)
{
    esp_now_netdev_t* dev = &_esp_now_dev;

    DEBUG("%s: %p\n", __func__, dev);

    if (dev->netdev.driver) {
        DEBUG("%s: early returning previously initialized device\n", __func__);
        return dev;
    }

    /* initialize buffer */
    dev->rx_len = 0;

    /* set the event handler */
    esp_system_event_add_handler(_esp_system_event_handler, NULL);

#ifdef CPU_ESP32
    /* init the WiFi driver */
    extern portMUX_TYPE g_intr_lock_mux;
    mutex_init(&g_intr_lock_mux);
#endif /* CPU_ESP32 */

    esp_err_t result;

#if CONFIG_ESP32_WIFI_NVS_ENABLED
    result = nvs_flash_init();
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_now",
                      "nfs_flash_init failed with return value %d\n", result);
        return NULL;
    }
#endif /* CONFIG_ESP32_WIFI_NVS_ENABLED */

    /* initialize the WiFi driver with default configuration */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    result = esp_wifi_init(&cfg);
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_now",
                      "esp_wifi_init failed with return value %d\n", result);
        return NULL;
    }

    /* set configuration storage type */
    result = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_now",
                      "esp_wifi_set_storage failed with return value %d\n",
                      result);
        return NULL;
    }

#ifdef CONFIG_WIFI_COUNTRY
    /* TODO */
#endif /* CONFIG_WIFI_COUNTRY */

    /* we use predefined station configuration since it has not to be changed */
    wifi_config_t wifi_config_sta = {
        .sta = {
            .channel = 0,
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
            .threshold.rssi = -127,
            .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK
        }
    };

    /* get SoftAP interface mac address and store it as device address */
    esp_read_mac(dev->addr, ESP_MAC_WIFI_SOFTAP);

    /* prepare initial ESP_NOW configuration for SoftAP */
    strcpy ((char*)wifi_config_ap.ap.password, esp_now_params.softap_pass);
    sprintf((char*)wifi_config_ap.ap.ssid, "%s%02x%02x%02x%02x%02x%02x",
            ESP_NOW_AP_PREFIX,
            dev->addr[0], dev->addr[1], dev->addr[2],
            dev->addr[3], dev->addr[4], dev->addr[5]);
    wifi_config_ap.ap.ssid_len = strlen((char*)wifi_config_ap.ap.ssid);

    wifi_config_ap.ap.channel = esp_now_params.channel;
    wifi_config_ap.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    wifi_config_ap.ap.ssid_hidden = false;
    wifi_config_ap.ap.max_connection = 4;
    wifi_config_ap.ap.beacon_interval = 100;

    /* set the WiFi interface to Station + SoftAP */
    result = esp_wifi_set_mode(WIFI_MODE_STA | WIFI_MODE_AP);
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_now",
                      "esp_wifi_set_mode failed with return value %d\n",
                      result);
        return NULL;
    }

    /* set the Station and SoftAP configuration */
    result = esp_wifi_set_config(WIFI_IF_STA, &wifi_config_sta);
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_now", "esp_wifi_set_config station failed with "
                      "return value %d\n", result);
        return NULL;
    }
    result = esp_wifi_set_config(WIFI_IF_AP, &wifi_config_ap);
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_now",
                      "esp_wifi_set_config softap failed with return value %d\n",
                      result);
        return NULL;
    }

#ifndef MODULE_ESP_WIFI
    /* start WiFi if esp_wifi is not used, otherwise it is done by esp_wifi */
    result = esp_wifi_start();
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_now",
                      "esp_wifi_start failed with return value %d\n", result);
        return NULL;
    }

#if !ESP_NOW_UNICAST
    /* all ESP-NOW nodes get the shared mac address on their station interface */
    esp_wifi_set_mac(WIFI_IF_STA, (uint8_t*)_esp_now_mac);
#endif

#endif /* MODULE_ESP_WIFI */

    /* set the netdev driver */
    dev->netdev.driver = &_esp_now_driver;

    /* initialize netdev data structure */
    dev->scan_event = 0;

    mutex_init(&dev->dev_lock);

    /* initialize ESP-NOW and register callback functions */
    result = esp_now_init();
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_now", "esp_now_init failed with return value %d\n",
                      result);
        return NULL;
    }
    esp_now_register_send_cb(esp_now_send_cb);
    esp_now_register_recv_cb(esp_now_recv_cb);

#if ESP_NOW_UNICAST
    /* timer for peer scan initialization */
    _esp_now_scan_peers_done = false;
    _esp_now_scan_peers_timer.callback = &esp_now_scan_peers_timer_cb;
    _esp_now_scan_peers_timer.arg = dev;

    /* execute the first scan */
    esp_now_scan_peers_start();

#else /* ESP_NOW_UNICAST */
    bool res = _esp_now_add_peer((uint8_t*)_esp_now_mac, esp_now_params.channel,
                                                         esp_now_params.key);
    DEBUG("%s: multicast node add %s\n", __func__, res ? "success" : "error");
#endif /* ESP_NOW_UNICAST */

    netdev_register(&dev->netdev, NETDEV_ESP_NOW, 0);
    return dev;
}

int esp_now_set_channel(uint8_t channel)
{
#ifdef ESP_NOW_UNICAST
    scan_cfg.channel = channel;
#endif
#ifdef MODULE_ESP_WIFI
    /* channel is controlled by `esp_wifi`, only update SoftAP info */
    wifi_config_ap.ap.channel = channel;
    return ESP_ERR_NOT_SUPPORTED;
#else
    /* channel is controlled by `esp_now`, try to reconfigure SoftAP */
    uint8_t old_channel = wifi_config_ap.ap.channel;
    wifi_config_ap.ap.channel = channel;
    esp_err_t result = esp_wifi_set_config(WIFI_IF_AP, &wifi_config_ap);
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_now",
                      "esp_wifi_set_config softap failed with return value %d\n",
                      result);
        wifi_config_ap.ap.channel = old_channel;
        return result;
    }
    return ESP_OK;
#endif
}

static int _init(netdev_t *netdev)
{
    DEBUG("%s: %p\n", __func__, netdev);

    /* signal link UP */
    netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
#if ESP_NOW_UNICAST
    if (!_esp_now_scan_peers_done) {
        return -ENODEV;
    }
#endif

    DEBUG("%s: %p %p\n", __func__, netdev, iolist);

    CHECK_PARAM_RET(netdev != NULL, -ENODEV);
    CHECK_PARAM_RET(iolist != NULL && iolist->iol_len == ESP_NOW_ADDR_LEN, -EINVAL);
    CHECK_PARAM_RET(iolist->iol_next != NULL, -EINVAL);

    esp_now_netdev_t *dev = container_of(netdev, esp_now_netdev_t, netdev);

    mutex_lock(&dev->dev_lock);

#if ESP_NOW_UNICAST
    uint8_t* _esp_now_dst = NULL;

    for (uint8_t i = 0; i < ESP_NOW_ADDR_LEN; i++) {
        if ((iolist->iol_base != NULL) && (((uint8_t*)iolist->iol_base)[i] != 0xff)) {
            _esp_now_dst = iolist->iol_base;
            break;
        }
    }
#else
    const uint8_t* _esp_now_dst = _esp_now_mac;
#endif
    iolist = iolist->iol_next;

    uint8_t *data_pos = dev->tx_mem;
    uint8_t data_len = 0;

    while (iolist) {
        if (((int)data_len + iolist->iol_len) > ESP_NOW_MAX_SIZE_RAW) {
            DEBUG("%s: payload length exceeds maximum(%u>%u)\n", __func__,
                  data_len + iolist->iol_len, ESP_NOW_MAX_SIZE_RAW);
            return -EBADMSG;
        }
        if (iolist->iol_len) {
            memcpy(data_pos, iolist->iol_base, iolist->iol_len);
            data_pos += iolist->iol_len;
            data_len += iolist->iol_len;
        }
        iolist = iolist->iol_next;
    }

    DEBUG("%s: send %u byte\n", __func__, (unsigned)data_len);
#if defined(MODULE_OD) && ENABLE_DEBUG
    od_hex_dump(dev->tx_mem, data_len, OD_WIDTH_DEFAULT);
#endif

    if (_esp_now_dst) {
        DEBUG("%s: send to esp_now addr %02x:%02x:%02x:%02x:%02x:%02x\n", __func__,
              _esp_now_dst[0], _esp_now_dst[1], _esp_now_dst[2],
              _esp_now_dst[3], _esp_now_dst[4], _esp_now_dst[5]);
    } else {
        DEBUG("%s: send esp_now broadcast\n", __func__);
    }

    _esp_now_sending = 1;

    /* send the packet to the peer(s) mac address */
    if (esp_now_send((uint8_t*)_esp_now_dst, dev->tx_mem, data_len) == ESP_OK) {
        while (_esp_now_sending > 0) {
            thread_yield_higher();
        }
#ifdef MODULE_NETSTATS_L2
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
#endif

        mutex_unlock(&dev->dev_lock);
        return data_len;
    } else {
        _esp_now_sending = 0;
    }

    mutex_unlock(&dev->dev_lock);
    return -EIO;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    DEBUG("%s: %p %p %u %p\n", __func__, netdev, buf, len, info);

    CHECK_PARAM_RET(netdev != NULL, -ENODEV);

    esp_now_netdev_t *dev = container_of(netdev, esp_now_netdev_t, netdev);

    /* we store source mac address and received data in `buf` */
    uint16_t size = dev->rx_len ? ESP_NOW_ADDR_LEN + dev->rx_len : 0;

    if (!buf) {
        /* get the size of the frame */
        if (len > 0 && size) {
            /* if len > 0, drop the frame */
            dev->rx_len = 0;
        }
        return size;
    }

    if (len < size) {
        /* buffer is smaller than the number of received bytes */
        DEBUG("[esp_now] No space in receive buffers\n");
        /* newest API requires to drop the frame in that case */
        dev->rx_len = 0;
        return -ENOBUFS;
    }

    /* copy the buffer */
    memcpy(buf, dev->rx_mac, ESP_NOW_ADDR_LEN);
    memcpy((char *)buf + ESP_NOW_ADDR_LEN, dev->rx_data, dev->rx_len);
    dev->rx_len = 0;

    uint8_t *mac = buf;
    DEBUG("%s: received %d byte from %02x:%02x:%02x:%02x:%02x:%02x\n",
          __func__, size - ESP_NOW_ADDR_LEN,
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
#if defined(MODULE_OD) && ENABLE_DEBUG
    od_hex_dump(buf + ESP_NOW_ADDR_LEN, size - ESP_NOW_ADDR_LEN, OD_WIDTH_DEFAULT);
#endif

#if ESP_NOW_UNICAST
    if (esp_now_is_peer_exist(mac) <= 0) {
        _esp_now_add_peer(mac, esp_now_params.channel, esp_now_params.key);
    }
#endif
    return size;
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    DEBUG("%s: %s %p %p %u\n", __func__, netopt2str(opt), netdev, val, max_len);

    CHECK_PARAM_RET(netdev != NULL, -ENODEV);
    CHECK_PARAM_RET(val != NULL, -EINVAL);

    esp_now_netdev_t *dev = container_of(netdev, esp_now_netdev_t, netdev);
    int res = -ENOTSUP;

    switch (opt) {

        case NETOPT_DEVICE_TYPE:
            CHECK_PARAM_RET(max_len >= sizeof(uint16_t), -EOVERFLOW);
            *((uint16_t *)val) = NETDEV_TYPE_ESP_NOW;
            res = sizeof(uint16_t);
            break;

#ifdef MODULE_GNRC
        case NETOPT_PROTO:
            CHECK_PARAM_RET(max_len == sizeof(gnrc_nettype_t), -EOVERFLOW);
            *((gnrc_nettype_t *)val) = dev->proto;
            res = sizeof(gnrc_nettype_t);
            break;
#endif

        case NETOPT_MAX_PDU_SIZE:
            CHECK_PARAM_RET(max_len >= sizeof(uint16_t), -EOVERFLOW);
            *((uint16_t *)val) = ESP_NOW_MAX_SIZE;
            res = sizeof(uint16_t);
            break;

        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            CHECK_PARAM_RET(max_len >= sizeof(uint16_t), -EOVERFLOW);
            *((uint16_t *)val) = sizeof(dev->addr);
            res = sizeof(uint16_t);
            break;

        case NETOPT_ADDRESS:
            CHECK_PARAM_RET(max_len >= sizeof(dev->addr), -EOVERFLOW);
            memcpy(val, dev->addr, sizeof(dev->addr));
            res = sizeof(dev->addr);
            break;

        case NETOPT_CHANNEL:
            CHECK_PARAM_RET(max_len >= sizeof(uint16_t), -EOVERFLOW);
            *((uint16_t *)val) = wifi_config_ap.ap.channel;
            res = sizeof(uint16_t);
            break;

        default:
            DEBUG("%s: %s not supported\n", __func__, netopt2str(opt));
            break;
    }
    return res;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t max_len)
{
    DEBUG("%s: %s %p %p %u\n", __func__, netopt2str(opt), netdev, val, max_len);

    CHECK_PARAM_RET(netdev != NULL, -ENODEV);
    CHECK_PARAM_RET(val != NULL, -EINVAL);

    esp_now_netdev_t *dev = container_of(netdev, esp_now_netdev_t, netdev);
    int res = -ENOTSUP;

    switch (opt) {

#ifdef MODULE_GNRC
        case NETOPT_PROTO:
            CHECK_PARAM_RET(max_len == sizeof(gnrc_nettype_t), -EOVERFLOW);
            dev->proto = *((gnrc_nettype_t *)val);
            res = sizeof(gnrc_nettype_t);
            break;
#endif

        case NETOPT_ADDRESS:
            CHECK_PARAM_RET(max_len >= sizeof(dev->addr), -EOVERFLOW);
            memcpy(dev->addr, val, sizeof(dev->addr));
            res = sizeof(dev->addr);
            break;

        case NETOPT_CHANNEL:
            CHECK_PARAM_RET(max_len >= sizeof(uint16_t), -EOVERFLOW);
            uint16_t channel = *((uint16_t *)val);
            if (esp_now_set_channel(channel) != ESP_OK) {
                return -ENOTSUP;
            }
            res = sizeof(uint16_t);
            break;

        default:
            DEBUG("%s: %s not supported\n", __func__, netopt2str(opt));
            break;
    }
    return res;
}

static void _isr(netdev_t *netdev)
{
    DEBUG("%s: %p\n", __func__, netdev);

    CHECK_PARAM(netdev != NULL);

    esp_now_netdev_t *dev = container_of(netdev, esp_now_netdev_t, netdev);

    critical_enter();

    if (dev->scan_event) {
        dev->scan_event--;
        critical_exit();
#if ESP_NOW_UNICAST
        esp_now_scan_peers_start();
#endif
    }
    return;
}

static const netdev_driver_t _esp_now_driver =
{
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

/** @} */
