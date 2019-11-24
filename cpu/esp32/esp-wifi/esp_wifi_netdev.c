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
#include "od.h"
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

#define ENABLE_DEBUG (0)
#include "debug.h"

#define ESP_WIFI_EVENT_RX_DONE          BIT(0)
#define ESP_WIFI_EVENT_TX_DONE          BIT(1)
#define ESP_WIFI_EVENT_STA_CONNECTED    BIT(2)
#define ESP_WIFI_EVENT_STA_DISCONNECTED BIT(3)

#define ESP_WIFI_DEBUG(f, ...) \
        DEBUG("[esp_wifi] %s: " f "\n", __func__, ## __VA_ARGS__)

#define ESP_WIFI_LOG_INFO(f, ...) \
        LOG_INFO("[esp_wifi] " f "\n", ## __VA_ARGS__)

#define ESP_WIFI_LOG_ERROR(f, ...) \
        LOG_ERROR("[esp_wifi] " f "\n", ## __VA_ARGS__)

#define MAC_STR                         "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_STR_ARG(m)                  m[0], m[1], m[2], m[3], m[4], m[5]

/*
 * There is only one ESP WiFi device. We define it as static device variable
 * to have access to the device inside ESP WiFi interrupt routines which do
 * not provide an argument that could be used as pointer to the ESP WiFi
 * device which triggers the interrupt.
 */
esp_wifi_netdev_t _esp_wifi_dev;
static const netdev_driver_t _esp_wifi_driver;

/*
 * Ring buffer for rx_buf elements which hold a pointer to the WiFi frame
 * buffer, a pointer to the ethernet frame and the frame length for each
 * received frame. Since we have only one device, it the ring buffer can be
 * static and has not to be exposed as part of the network device.
 */
#ifndef ESP_WIFI_MAX_RX_BUF
/** The maximum of pending incoming WiFi frames */
#define ESP_WIFI_MAX_RX_BUF 20
#endif

typedef struct {
    void* buffer;
    void* eb;
    uint16_t len;
} rx_buf_t;

static rx_buf_t rx_buf[ESP_WIFI_MAX_RX_BUF] = { 0 };

static unsigned int rx_buf_write = 0;
static unsigned int rx_buf_read = 0;

/* device thread stack */
static char _esp_wifi_stack[ESP_WIFI_STACKSIZE];

extern esp_err_t esp_system_event_add_handler (system_event_cb_t handler,
                                               void *arg);

/**
 * @brief   Callback when ethernet frame is received. Has to run in IRAM.
 * @param   buffer  pointer to the begin of the ethernet frame in *eb
 * @param   eb      allocated buffer in WiFi interface
 */
esp_err_t _esp_wifi_rx_cb(void *buffer, uint16_t len, void *eb)
{
    assert(buffer);
    assert(eb);

    /*
     * When interrupts of the WiFi hardware interface occur, the ISRs only
     * send events to the message queue of the `wifi-event-loop` thread.
     * The `wifi-event-loop` thread then processes these events sequentially
     * and invokes callback functions like the function `esp_wifi_recv_cb`
     * asynchronously.
     *
     * This means that the function `_esp_wifi_rx_cb` is never executed in
     * the interrupt context, but always in the context of the thread
     * `wifi-event-loop`. Furthermore, function `_esp_wifi_rx_cb` can't
     * never be reentered.
     */
    ESP_WIFI_DEBUG("buf=%p len=%d eb=%p", buffer, len, eb);

    /* check packet buffer for the minimum packet size */
    if (len < sizeof(ethernet_hdr_t)) {
        ESP_WIFI_DEBUG("frame length is less than the size of an Ethernet"
                       "header (%u < %u)", len, sizeof(ethernet_hdr_t));
        esp_wifi_internal_free_rx_buffer(eb);
        return ESP_ERR_INVALID_SIZE;
    }

    /* check whether packet buffer fits into receive buffer */
    if (len > ETHERNET_MAX_LEN) {
        ESP_WIFI_DEBUG("frame length is greater than the maximum size of an "
                       "Ethernet frame (%u > %u)", len, ETHERNET_MAX_LEN);
        esp_wifi_internal_free_rx_buffer(eb);
        return ESP_ERR_INVALID_SIZE;
    }

    /* check whether rx buffer is full, that is, rx_buf_write points to an
       element that is already in use */
    if (rx_buf[rx_buf_write].buffer) {
        ESP_WIFI_LOG_ERROR("no space left in receive buffer");
        esp_wifi_internal_free_rx_buffer(eb);
        return ESP_ERR_NO_MEM;
    }

    critical_enter();

    /* fill the rx_buf element */
    rx_buf[rx_buf_write].buffer = buffer;
    rx_buf[rx_buf_write].eb = eb;
    rx_buf[rx_buf_write].len = len;

    /* point to the next element */
    rx_buf_write = (rx_buf_write + 1) % ESP_WIFI_MAX_RX_BUF;

    critical_exit();

    /* trigger netdev event to read the data */
    _esp_wifi_dev.event |= ESP_WIFI_EVENT_RX_DONE;
    _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev, NETDEV_EVENT_ISR);

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
            ESP_WIFI_DEBUG("WiFi started");
            result = esp_wifi_connect();
            if (result != ESP_OK) {
                LOG_TAG_ERROR("esp_wifi", "esp_wifi_connect failed with return "
                              "value %d\n", result);
            }
            break;

        case SYSTEM_EVENT_SCAN_DONE:
            ESP_WIFI_DEBUG("WiFi scan done");
            break;

        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_WIFI_DEBUG("WiFi connected to ssid %s",
                           event->event_info.connected.ssid);

            /* register RX callback function */
            esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, _esp_wifi_rx_cb);

            _esp_wifi_dev.connected = true;
            _esp_wifi_dev.event |= ESP_WIFI_EVENT_STA_CONNECTED;
            _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev, NETDEV_EVENT_ISR);

            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_WIFI_DEBUG("WiFi disconnected from ssid %s, reason %d",
                           event->event_info.disconnected.ssid,
                           event->event_info.disconnected.reason);

            /* unregister RX callback function */
            esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, NULL);

            _esp_wifi_dev.connected = false;
            _esp_wifi_dev.event |= ESP_WIFI_EVENT_STA_DISCONNECTED;
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
            ESP_WIFI_DEBUG("event %d", event->event_id);
            break;
    }
    return ESP_OK;
}

/* we use predefined station configuration */
static wifi_config_t wifi_config_sta = {
    .sta = {
        .ssid = ESP_WIFI_SSID,
        .password = ESP_WIFI_PASS,
        .bssid_set = 0,
        .channel = 0,
        .scan_method = WIFI_ALL_CHANNEL_SCAN,
        .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
        .threshold.rssi = -127,
        .threshold.authmode = WIFI_AUTH_WPA_WPA2_PSK
    }
};

void esp_wifi_setup (esp_wifi_netdev_t* dev)
{
    ESP_WIFI_DEBUG("%p", dev);

    /* set the event handler */
    esp_system_event_add_handler(_esp_system_event_handler, NULL);

    /*
     * Init the WiFi driver. TODO It is not only required before ESP_WIFI is
     * initialized but also before other WiFi functions are used. Once other
     * WiFi functions are realized it has to be moved to a more common place.
     */
    esp_err_t result;

#ifndef MODULE_ESP_NOW
    /* if esp_now is used, the following part is already done */
    extern portMUX_TYPE g_intr_lock_mux;
    mutex_init(&g_intr_lock_mux);

#if CONFIG_ESP32_WIFI_NVS_ENABLED
    result = nvs_flash_init();
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_wifi", "nfs_flash_init failed "
                      "with return value %d\n", result);
        return;
    }
#endif /* CONFIG_ESP32_WIFI_NVS_ENABLED */

    /* initialize the WiFi driver with default configuration */
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    result = esp_wifi_init(&cfg);
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_wifi", "esp_wifi_init failed "
                      "with return value %d\n", result);
        return;
    }

    /* set configuration storage type */
    result = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_now", "esp_wifi_set_storage failed "
                      "with return value %d\n", result);
        return NULL;
    }

#ifdef CONFIG_WIFI_COUNTRY
    /* TODO */
#endif /* CONFIG_WIFI_COUNTRY */

    result = esp_wifi_set_mode(WIFI_MODE_STA);
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_wifi", "esp_wifi_set_mode failed "
                      "with return value %d\n", result);
        return;
    }
#endif /* MODULE_ESP_NOW */

    /* set the Station configuration */
    result = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta);
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_wifi", "esp_wifi_set_config station failed "
                      "with return value %d\n", result);
        return;
    }

    /* start the WiFi driver */
    result = esp_wifi_start();
    if (result != ESP_OK) {
        LOG_TAG_ERROR("esp_wifi", "esp_wifi_start failed "
                      "with return value %d\n", result);
        return;
    }

    /* register RX callback function */
    esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, _esp_wifi_rx_cb);

    /* set the netdev driver */
    dev->netdev.driver = &_esp_wifi_driver;

    /* initialize netdev data structure */
    dev->connected = false;
}

static int _esp_wifi_init(netdev_t *netdev)
{
    ESP_WIFI_DEBUG("%p", netdev);

    _esp_wifi_dev.event = 0; /* no event */

    return 0;
}

static int _esp_wifi_send(netdev_t *netdev, const iolist_t *iolist)
{
    ESP_WIFI_DEBUG("%p %p", netdev, iolist);

    assert(netdev != NULL);
    assert(iolist != NULL);

    if (!_esp_wifi_dev.connected) {
        ESP_WIFI_DEBUG("WiFi is still not connected to AP, cannot send");
        return -EIO;
    }

    uint16_t tx_len = 0;              /**< number of bytes in transmit buffer */
    uint8_t tx_buf[ETHERNET_MAX_LEN]; /**< transmit buffer */

    /* load packet data into TX buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (tx_len + iol->iol_len > ETHERNET_MAX_LEN) {
            return -EOVERFLOW;
        }
        if (iol->iol_len) {
            memcpy (tx_buf + tx_len, iol->iol_base, iol->iol_len);
            tx_len += iol->iol_len;
        }
    }

#if ENABLE_DEBUG
    const ethernet_hdr_t* hdr = (const ethernet_hdr_t *)tx_buf;

    ESP_WIFI_DEBUG("send %u byte to " MAC_STR,
                   (unsigned)tx_len, MAC_STR_ARG(hdr->dst));
#if MODULE_OD
    od_hex_dump(tx_buf, tx_len, OD_WIDTH_DEFAULT);
#endif /* MODULE_OD */
#endif /* ENABLE_DEBUG */

    /* send the the packet to the peer(s) mac address */
    if (esp_wifi_internal_tx(ESP_IF_WIFI_STA, tx_buf, tx_len) == ESP_OK) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
        return tx_len;
    }
    else {
        ESP_WIFI_DEBUG("sending WiFi packet failed");
        return -EIO;
    }
}

static int _esp_wifi_recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    ESP_WIFI_DEBUG("%p %p %u %p", netdev, buf, len, info);

    assert(netdev != NULL);

    if (!rx_buf[rx_buf_read].buffer) {
        /* there is nothing in rx_buf */
        return 0;
    }

    uint16_t size = rx_buf[rx_buf_read].len;

    if (buf == NULL && len == 0) {
        /* just return the size */
        return size;
    }

    if (buf && len >= size) {
        /* copy the buffer */
        memcpy(buf, rx_buf[rx_buf_read].buffer, size);
#if ENABLE_DEBUG
        ethernet_hdr_t *hdr = (ethernet_hdr_t *)buf;

        ESP_WIFI_DEBUG("received %u byte from addr " MAC_STR,
                       size, MAC_STR_ARG(hdr->src));
#if MODULE_OD
        od_hex_dump(buf, size, OD_WIDTH_DEFAULT);
#endif /* MODULE_OD */
#endif /* ENABLE_DEBUG */
    }

    /*
     * free the packet buffer and clean the rx_buf element at the read pointer,
     * it covers the also the cases where the packet is simply dropped when
     * (buf == NULL && len != 0) or (buf != NULL && len < size)
     */
    esp_wifi_internal_free_rx_buffer(rx_buf[rx_buf_read].eb);

    critical_enter();
    rx_buf[rx_buf_read].buffer = NULL;
    rx_buf[rx_buf_write].len = len;
    rx_buf_read = (rx_buf_read + 1) % ESP_WIFI_MAX_RX_BUF;
    critical_exit();

    return size;
}

static int _esp_wifi_get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    ESP_WIFI_DEBUG("%s %p %p %u", netopt2str(opt), netdev, val, max_len);

    assert(netdev != NULL);
    assert(val != NULL);

    esp_wifi_netdev_t* dev = (esp_wifi_netdev_t*)netdev;

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
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
    ESP_WIFI_DEBUG("%s %p %p %u", netopt2str(opt), netdev, val, max_len);

    assert(netdev != NULL);
    assert(val != NULL);

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
    ESP_WIFI_DEBUG("%p", netdev);

    assert(netdev != NULL);

    esp_wifi_netdev_t *dev = (esp_wifi_netdev_t *) netdev;

    dev->event &= ~ESP_WIFI_EVENT_RX_DONE;
    while (rx_buf[rx_buf_read].buffer) {
        dev->netdev.event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
    if (dev->event & ESP_WIFI_EVENT_TX_DONE) {
        dev->event &= ~ESP_WIFI_EVENT_TX_DONE;
        dev->netdev.event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
    if (dev->event & ESP_WIFI_EVENT_STA_CONNECTED) {
        dev->event &= ~ESP_WIFI_EVENT_STA_CONNECTED;
        dev->netdev.event_callback(netdev, NETDEV_EVENT_LINK_UP);
    }
    if (dev->event & ESP_WIFI_EVENT_STA_DISCONNECTED) {
        dev->event &= ~ESP_WIFI_EVENT_STA_DISCONNECTED;
        dev->netdev.event_callback(netdev, NETDEV_EVENT_LINK_DOWN);
    }

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
    _esp_wifi_dev.netif = gnrc_netif_ethernet_create(_esp_wifi_stack,
                                                    ESP_WIFI_STACKSIZE,
#ifdef MODULE_ESP_NOW
                                                    ESP_WIFI_PRIO - 1,
#else
                                                    ESP_WIFI_PRIO,
#endif
                                                    "esp-wifi",
                                                    (netdev_t *)&_esp_wifi_dev);
}

#endif /* MODULE_ESP_WIFI */
/**@}*/
