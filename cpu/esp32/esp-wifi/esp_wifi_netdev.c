/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_wifi
 * @{
 *
 * @file
 * @brief       Network device driver for the ESP32 WiFi interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifdef MODULE_ESP_WIFI

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "log.h"
#include "tools.h"

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "net/gnrc/netif/ethernet.h"
#include "net/gnrc/netif/raw.h"
#include "net/gnrc.h"
#include "net/ethernet.h"
#include "net/netdev/eth.h"

#include "xtimer.h"

#include "esp_common.h"
#include "esp_attr.h"
#include "esp_event_loop.h"
#include "esp_now.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wifi_internal.h"
#include "irq_arch.h"

#include "nvs_flash/include/nvs_flash.h"

#include "esp_wifi_params.h"
#include "esp_wifi_netdev.h"

#include "net/ipv6/hdr.h"
#include "net/gnrc/ipv6/nib.h"

#define SYSTEM_EVENT_WIFI_RX_DONE    (SYSTEM_EVENT_MAX + 3)
#define SYSTEM_EVENT_WIFI_TX_DONE    (SYSTEM_EVENT_MAX + 4)

/**
 * There is only one ESP WiFi device. We define it as static device variable
 * to have accesss to the device inside ESP WiFi interrupt routines which do
 * not provide an argument that could be used as pointer to the ESP WiFi
 * device which triggers the interrupt.
 */
static esp_wifi_netdev_t _esp_wifi_dev;
static const netdev_driver_t _esp_wifi_driver;

/* device thread stack */
static char _esp_wifi_stack[ESP_WIFI_STACKSIZE];

extern esp_err_t esp_system_event_add_handler (system_event_cb_t handler,
                                               void *arg);

esp_err_t _esp_wifi_rx_cb(void *buffer, uint16_t len, void *eb)
{
    /*
     * This callback function is executed in interrupt context but in the
     * context of the wifi thread. That is, mutex_lock or msg_send can block.
     */

    DEBUG("%s: buf=%p len=%d eb=%p\n", __func__, buffer, len, eb);

    if ((buffer == NULL) || (len >= ETHERNET_MAX_LEN)) {
        if (eb != NULL) {
            esp_wifi_internal_free_rx_buffer(eb);
        }
        return ESP_ERR_INVALID_ARG;
    }

    mutex_lock(&_esp_wifi_dev.dev_lock);
    critical_enter();

    /* copy the buffer and free WiFi driver buffer */
    memcpy(_esp_wifi_dev.rx_buf, buffer, len);
    if (eb) {
        esp_wifi_internal_free_rx_buffer(eb);
    }

    /*
     * Because this function is not executed in interrupt context but in thread
     * context, following msg_send could block on heavy network load, if frames
     * are coming in faster than the ISR events can be handled. To avoid
     * blocking during msg_send, we pretend we are in an ISR by incrementing
     * the IRQ nesting counter. If IRQ nesting counter is greater 0, function
     * irq_is_in returns true and the non-blocking version of msg_send is used.
     */
    irq_interrupt_nesting++;

    /* trigger netdev event to read the data */
    _esp_wifi_dev.rx_len = len;
    _esp_wifi_dev.event = SYSTEM_EVENT_WIFI_RX_DONE;
    _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev, NETDEV_EVENT_ISR);

    /* reset IRQ nesting counter */
    irq_interrupt_nesting--;

    critical_exit();
    mutex_unlock(&_esp_wifi_dev.dev_lock);

    return ESP_OK;
}

/*
 * Event handler for esp system events.
 */
static esp_err_t IRAM_ATTR _esp_system_event_handler(void *ctx, system_event_t *event)
{
    esp_err_t result;

    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            DEBUG("%s WiFi started\n", __func__);
            result = esp_wifi_connect();
            if (result != ESP_OK) {
                LOG_TAG_ERROR("esp_wifi", "esp_wifi_connect failed with return "
                              "value %d\n", result);
            }
            break;

        case SYSTEM_EVENT_SCAN_DONE:
            DEBUG("%s WiFi scan done\n", __func__);
            break;

        case SYSTEM_EVENT_STA_CONNECTED:
            DEBUG("%s WiFi connected\n", __func__);

            /* register RX callback function */
            esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, _esp_wifi_rx_cb);

            _esp_wifi_dev.connected = true;
            _esp_wifi_dev.event = SYSTEM_EVENT_STA_CONNECTED;
            _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev, NETDEV_EVENT_ISR);

            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            DEBUG("%s WiFi disconnected from ssid %s, reason %d\n", __func__,
                  event->event_info.disconnected.ssid,
                  event->event_info.disconnected.reason);

            /* unregister RX callback function */
            esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, NULL);

            _esp_wifi_dev.connected = false;
            _esp_wifi_dev.event = SYSTEM_EVENT_STA_DISCONNECTED;
            _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev, NETDEV_EVENT_ISR);

            /* call disconnect to reset internal state */
            result = esp_wifi_disconnect();
            if (result != ESP_OK) {
                LOG_TAG_ERROR("esp_wifi", "esp_wifi_disconnect failed with "
                             "return value %d\n", result);
                return result;
            }

            /* try to reconnect */
            result = esp_wifi_connect();
            if (result != ESP_OK) {
               LOG_TAG_ERROR("esp_wifi", "esp_wifi_connect failed with "
                             "return value %d\n", result);
            }

            break;

        default:
            DEBUG("%s event %d\n", __func__, event->event_id);
            break;
    }
    return ESP_OK;
}

/* we use predefined station configuration */
static wifi_config_t wifi_config_sta = {
    .sta = {
        .ssid = ESP_WIFI_SSID,
        .password = ESP_WIFI_PASS,
        .channel = 0,
        .scan_method = WIFI_ALL_CHANNEL_SCAN,
        .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
        .threshold.rssi = -127,
        .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK
    }
};

static void esp_wifi_setup (esp_wifi_netdev_t* dev)
{
    DEBUG("%s: %p\n", __func__, dev);

    /*
     * Init the WiFi driver. TODO It is not only required before ESP_WIFI is
     * initialized but also before other WiFi functions are used. Once other
     * WiFi functions are realized it has to be moved to a more common place.
     */
    extern portMUX_TYPE g_intr_lock_mux;
    mutex_init(&g_intr_lock_mux);

    esp_system_event_add_handler (_esp_system_event_handler, NULL);

    esp_err_t result;
    #if CONFIG_ESP32_WIFI_NVS_ENABLED
    result = nvs_flash_init();
    if (result != ESP_OK) {
       LOG_TAG_ERROR("esp_wifi", "nfs_flash_init failed with return value %d\n", result);
        return;
    }
    #endif

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    result = esp_wifi_init(&cfg);
    if (result != ESP_OK) {
       LOG_TAG_ERROR("esp_wifi", "esp_wifi_init failed with return value %d\n", result);
        return;
    }

    #ifdef CONFIG_WIFI_COUNTRY
    /* TODO */
    #endif

    result = esp_wifi_set_mode(WIFI_MODE_STA);
    if (result != ESP_OK) {
       LOG_TAG_ERROR("esp_wifi", "esp_wifi_set_mode failed with return value %d\n", result);
        return;
    }

    /* set the Station and SoftAP configuration */
    result = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta);
    if (result != ESP_OK) {
       LOG_TAG_ERROR("esp_wifi", "esp_wifi_set_config station failed with return value %d\n", result);
        return;
    }

    /* start the WiFi driver */
    result = esp_wifi_start();
    if (result != ESP_OK) {
       LOG_TAG_ERROR("esp_wifi", "esp_wifi_start failed with return value %d\n", result);
        return;
    }

    /* register RX callback function */
    esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, _esp_wifi_rx_cb);

    /* set the netdev driver */
    dev->netdev.driver = &_esp_wifi_driver;

    /* initialize netdev data structure */
    dev->connected = false;

    mutex_init(&dev->dev_lock);
}

static int _esp_wifi_init(netdev_t *netdev)
{
    DEBUG("%s: %p\n", __func__, netdev);

    return 0;
}

static int _esp_wifi_send(netdev_t *netdev, const iolist_t *iolist)
{
    DEBUG("%s: netdev=%p iolist=%p\n", __func__, netdev, iolist);

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);
    CHECK_PARAM_RET (iolist != NULL, -EINVAL);

    esp_wifi_netdev_t* dev = (esp_wifi_netdev_t*)netdev;

    if (!_esp_wifi_dev.connected) {
        DEBUG("%s: WiFi is not connected\n", __func__);
        return -ENODEV;
    }

    mutex_lock(&dev->dev_lock);

    dev->tx_len = 0;

    /* load packet data into TX buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (dev->tx_len + iol->iol_len > ETHERNET_MAX_LEN) {
            mutex_unlock(&dev->dev_lock);
            return -EOVERFLOW;
        }
        memcpy (dev->tx_buf + dev->tx_len, iol->iol_base, iol->iol_len);
        dev->tx_len += iol->iol_len;
    }

    #if ENABLE_DEBUG
    printf ("%s: send %d byte\n", __func__, dev->tx_len);
    /* esp_hexdump (dev->tx_buf, dev->tx_len, 'b', 16); */
    #endif

    int ret = 0;

    /* send the the packet to the peer(s) mac address */
    if (esp_wifi_internal_tx(ESP_IF_WIFI_STA, dev->tx_buf, dev->tx_len) == ESP_OK) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
    else {
        DEBUG("%s: sending WiFi packet failed\n", __func__);
        ret = -EIO;
    }

    mutex_unlock(&dev->dev_lock);

    return ret;
}

static int _esp_wifi_recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    DEBUG("%s: %p %p %u %p\n", __func__, netdev, buf, len, info);

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);

    esp_wifi_netdev_t* dev = (esp_wifi_netdev_t*)netdev;

    mutex_lock(&dev->dev_lock);

    uint16_t size = dev->rx_len;

    if (!buf && !len) {
        /* return the size without dropping received data */
        mutex_unlock(&dev->dev_lock);
        return size;
    }

    if (!buf && len) {
        /* return the size and drop received data */
        mutex_unlock(&dev->dev_lock);
        dev->rx_len = 0;
        return size;
    }

    if (buf && len && dev->rx_len) {
        if (dev->rx_len > len) {
            DEBUG("[esp_wifi] No space in receive buffers\n");
            mutex_unlock(&dev->dev_lock);
            return -ENOBUFS;
        }

        #if ENABLE_DEBUG
        /* esp_hexdump (dev->rx_buf, dev->rx_len, 'b', 16); */
        #endif

        /* copy received date and reset the receive length */
        memcpy(buf, dev->rx_buf, dev->rx_len);
        dev->rx_len = 0;

        mutex_unlock(&dev->dev_lock);
        return size;
    }

    mutex_unlock(&dev->dev_lock);
    return -EINVAL;
}

static int _esp_wifi_get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    DEBUG("%s: %s %p %p %u\n", __func__, netopt2str(opt), netdev, val, max_len);

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);
    CHECK_PARAM_RET (val != NULL, -EINVAL);

    esp_wifi_netdev_t* dev = (esp_wifi_netdev_t*)netdev;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len == ETHERNET_ADDR_LEN);
            esp_wifi_get_mac(ESP_MAC_WIFI_STA,(uint8_t *)val);
            return ETHERNET_ADDR_LEN;
        case NETOPT_IS_WIRED:
            return false;
        case NETOPT_LINK_CONNECTED:
            assert(max_len == 1);
            *((netopt_enable_t *)val) = (dev->connected) ? NETOPT_ENABLE
                                                         : NETOPT_DISABLE;
            return 1;
        default:
            return netdev_eth_get(netdev, opt, val, max_len);
    }
}

static int _esp_wifi_set(netdev_t *netdev, netopt_t opt, const void *val, size_t max_len)
{
    DEBUG("%s: %s %p %p %u\n", __func__, netopt2str(opt), netdev, val, max_len);

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);
    CHECK_PARAM_RET (val != NULL, -EINVAL);

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len == ETHERNET_ADDR_LEN);
            esp_wifi_set_mac(ESP_MAC_WIFI_STA, (uint8_t *)val);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_set(netdev, opt, val, max_len);
    }
}

static void _esp_wifi_isr(netdev_t *netdev)
{
    DEBUG("%s: %p\n", __func__, netdev);

    CHECK_PARAM (netdev != NULL);

    esp_wifi_netdev_t *dev = (esp_wifi_netdev_t *) netdev;

    switch (dev->event) {
        case SYSTEM_EVENT_WIFI_RX_DONE:
            if (dev->rx_len) {
                dev->netdev.event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
            }
        case SYSTEM_EVENT_STA_CONNECTED:
            dev->netdev.event_callback(netdev, NETDEV_EVENT_LINK_UP);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            dev->netdev.event_callback(netdev, NETDEV_EVENT_LINK_DOWN);
            break;
        default:
            break;
    }
    _esp_wifi_dev.event = SYSTEM_EVENT_MAX; /* no event */

    return;
}

static const netdev_driver_t _esp_wifi_driver =
{
    .send = _esp_wifi_send,
    .recv = _esp_wifi_recv,
    .init = _esp_wifi_init,
    .isr = _esp_wifi_isr,
    .get = _esp_wifi_get,
    .set = _esp_wifi_set,
};

void auto_init_esp_wifi (void)
{
    LOG_TAG_DEBUG("esp_wifi", "initializing ESP WiFi device\n");

    esp_wifi_setup(&_esp_wifi_dev);
    _esp_wifi_dev.event = SYSTEM_EVENT_MAX; /* no event */
    _esp_wifi_dev.netif = gnrc_netif_ethernet_create(_esp_wifi_stack,
                                                    ESP_WIFI_STACKSIZE, ESP_WIFI_PRIO,
                                                    "netdev-esp-wifi",
                                                    (netdev_t *)&_esp_wifi_dev);
}

#endif /* MODULE_ESP_WIFI */
/**@}*/
