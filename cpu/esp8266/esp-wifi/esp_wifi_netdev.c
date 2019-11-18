/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266_esp_wifi
 * @{
 *
 * @file
 * @brief       Network device driver for the ESP8266 WiFi interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifdef MODULE_ESP_WIFI

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
#include "tools.h"

#include "nvs_flash/include/nvs_flash.h"

#include "esp_wifi_params.h"
#include "esp_wifi_netdev.h"

#define ENABLE_DEBUG_HEXDUMP    (0)
#define ENABLE_DEBUG            (0)
#include "debug.h"
#include "log.h"

#define SYSTEM_EVENT_WIFI_RX_DONE    (SYSTEM_EVENT_MAX + 3)
#define SYSTEM_EVENT_WIFI_TX_DONE    (SYSTEM_EVENT_MAX + 4)

#define ESP_WIFI_DEBUG(f, ...) \
        DEBUG("[esp_wifi] %s: " f "\n", __func__, ## __VA_ARGS__)

#define ESP_WIFI_LOG_INFO(f, ...) \
        LOG_TAG_INFO("esp_wifi", f "\n", ## __VA_ARGS__)

#define ESP_WIFI_LOG_ERROR(f, ...) \
        LOG_TAG_ERROR("esp_wifi", f "\n", ## __VA_ARGS__)

#define MAC_STR                     "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_STR_ARG(m)              m[0], m[1], m[2], m[3], m[4], m[5]

#ifdef MCU_ESP8266

#include "esp_socket.h"
#include "net/sockio.h"
#include "xtensa/xtensa_context.h"

#define CONFIG_TCP_OVERSIZE_MSS 1
#define LL_ALIGN(s)             (((uint32_t)s + 3) & 0xfffffffcU)

#include "lwip/pbuf.h"

#endif /* MCU_ESP8266 */

/**
 * There is only one ESP WiFi device. We define it as static device variable
 * to have access to the device inside ESP WiFi interrupt routines which do
 * not provide an argument that could be used as pointer to the ESP WiFi
 * device which triggers the interrupt.
 */
esp_wifi_netdev_t _esp_wifi_dev;
static const netdev_driver_t _esp_wifi_driver;

/* device thread stack */
static char _esp_wifi_stack[ESP_WIFI_STACKSIZE];

/** guard variable to avoid reentrance to _esp_wifi_send function */
static bool _esp_wifi_send_is_in = false;

/** guard variable to to decive when receive buffer can be overwritten */
static bool _esp_wifi_rx_in_progress = false;

extern esp_err_t esp_system_event_add_handler (system_event_cb_t handler,
                                               void *arg);

#ifdef MCU_ESP8266

/**
 * The low level WiFi driver function expects a lwIP pbuf data structure as
 * input. To avoid the integration of the whole lwIP package from ESP8266 RTOS
 * SDK, only the pbuf allocation function is realized with a very restricted
 * functionality. It uses malloc to allocate a packet buffer of type PBUF_RAM
 * for layer PBUF_RAW_TX.
 */
static struct pbuf *_esp_wifi_pbuf_alloc(size_t size)
{
    /* Low level WiFi driver can only use 32-bit aligned DRAM memory */
    size_t mem_size = LL_ALIGN(sizeof(struct pbuf)) + LL_ALIGN(size + PBUF_LINK_ENCAPSULATION_HLEN);
    struct pbuf *pb = heap_caps_malloc(mem_size, MALLOC_CAP_8BIT);
    if (pb == NULL) {
        ESP_WIFI_LOG_ERROR("no space left for packet buffer allocation");
        return NULL;
    }
    memset(pb, 0, mem_size);

    /* initialize pbuf data structure */
    pb->next = NULL;
    pb->payload = (void *)LL_ALIGN((uint8_t *)pb + sizeof(struct pbuf) + PBUF_LINK_ENCAPSULATION_HLEN);
    pb->tot_len = size;
    pb->len = size;
    pb->type = PBUF_RAM;
    pb->flags = 0;
    pb->ref = 1;

    ESP_WIFI_DEBUG("pb=%p size=%d", pb, size);

    return (struct pbuf*)pb;
}

/**
 * Free function for pbuf allocation
 */
static int _esp_wifi_pbuf_free(struct pbuf *pb)
{
    assert(pb != NULL);

    ESP_WIFI_DEBUG("pb=%p ref=%d", pb, pb->ref);

    if (pb->ref > 1) {
        pb->ref--;
    }
    else if (pb->ref == 1) {
        pb->ref = 0;
        heap_caps_free(pb);
        return 1;
    }
    return 0;
}

/**
 * Socket used for interaction with low level WiFi driver, -1 if not opened.
 * Since we have only one WiFi interface, it has not to be a member of the
 * netdev data structures. We can use a static variable instead.
 */
static int _esp_wifi_socket = -1;

/**
 * Function called when transmission of a packet has been finished.
 */
static int _esp_wifi_tx_cb(esp_aio_t* aio)
{
    assert(aio != NULL);

    ESP_WIFI_DEBUG("aio=%p buf=%p", aio, aio->pbuf);

    struct pbuf* pbuf = aio->arg;
    _esp_wifi_pbuf_free(pbuf);
    _esp_wifi_send_is_in = false;
    _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev, NETDEV_EVENT_TX_COMPLETE);

    return 0;
}

/**
 * Function for source code compatibility with ESP-IDF for ESP32
 */
int esp_wifi_internal_tx(wifi_interface_t wifi_if, void *buf, uint16_t len)
{
    ESP_WIFI_DEBUG("buf=%p len=%u", buf, len);

    struct pbuf *pb = _esp_wifi_pbuf_alloc(len);
    if (pb == NULL) {
        return ERR_MEM;
    }
    if (len) {
        memcpy(pb->payload, buf, len);
    }

    esp_aio_t aio;
    aio.fd = wifi_if;
    aio.pbuf = pb->payload;
    aio.len = pb->len;
    aio.cb = _esp_wifi_tx_cb;
    aio.arg = pb;
    aio.ret = 0;

    if (esp_aio_sendto(&aio, NULL, 0) != 0) {
        return ERR_IF;
    }

    ESP_WIFI_DEBUG("done");

    return ERR_OK;
}

/**
 * Function for source code compatibility with ESP-IDF for ESP32
 */
void esp_wifi_internal_free_rx_buffer(const char* buf)
{
    assert(buf != NULL);
    assert(_esp_wifi_socket != -1);

    ESP_WIFI_DEBUG("buf=%p sock=%d", buf, _esp_wifi_socket);

    esp_free_pbuf(_esp_wifi_socket, (void *)buf);
}

/**
 * Type definition for source code compatibility with ESP-IDF for ESP32
 */
typedef int (*wifi_rxcb_t)(struct esp_aio *aio);

/**
 * Function for source code compatibility with ESP-IDF for ESP32
 */
esp_err_t esp_wifi_internal_reg_rxcb(wifi_interface_t ifx, wifi_rxcb_t fn)
{
    assert(ifx == ESP_IF_WIFI_STA);

    ESP_WIFI_DEBUG("%d %p", ifx, fn);

    extern int8_t wifi_get_netif(uint8_t fd);

    /* if function is NULL, it is deregistered */
    if (fn == NULL) {
        /* if socket is allocated, it has to be closed */
        if (_esp_wifi_socket != -1 && esp_close(_esp_wifi_socket) < 0) {
            return ESP_FAIL;
        }
        _esp_wifi_socket = -1;
        return ESP_OK;
    }

    /* if socket is already allocated we have to close it to register a function */
    if (_esp_wifi_socket != -1 && esp_close(_esp_wifi_socket) < 0) {
        return ESP_FAIL;
    }

    /* now, we have to allocate a new socket and register the function */
    _esp_wifi_socket = esp_socket(AF_PACKET, SOCK_RAW, ETH_P_ALL);

    if (_esp_wifi_socket < 0) {
        ESP_WIFI_LOG_ERROR("create socket of (AF_PACKET, SOCK_RAW, ETH_P_ALL) error");
        return ESP_FAIL;
    }
    if (esp_ioctl(_esp_wifi_socket, SIOCGIFINDEX, "sta0") < 0) {
        ESP_WIFI_LOG_ERROR("bind socket %d to netcard %s error", _esp_wifi_socket, "sta0");
        esp_close(_esp_wifi_socket);
        return ESP_FAIL;
    }
    if (esp_aio_event(_esp_wifi_socket, ESP_SOCKET_RECV_EVENT, fn, &_esp_wifi_dev) < 0) {
        ESP_WIFI_LOG_ERROR("socket %d register receive callback function %p error",
                           _esp_wifi_socket, fn);
        esp_close(_esp_wifi_socket);
        return ESP_FAIL;
    }
    return ESP_OK;
}

#endif /* MCU_ESP8266 */

#ifdef MCU_ESP8266

/* Prolog for source code compatibility with ESP-IDF for ESP32 */
static int _esp_wifi_rx_cb(struct esp_aio *aio)
{
    assert(aio != NULL);

    const char *eb = aio->pbuf;
    const char *buffer = aio->pbuf;
    uint16_t len = aio->len;

#else /* MCU_ESP8266 */

esp_err_t _esp_wifi_rx_cb(void *buffer, uint16_t len, void *eb)
{
#endif /* MCU_ESP8266 */

    /*
     * This callback function is not executed in interrupt context but in the
     * context of the low level WiFi driver thread. That is, mutex_lock or
     * msg_send functions could block.
     */

    assert(buffer != NULL);
    assert(len <= ETHERNET_MAX_LEN);

    critical_enter();

    ESP_WIFI_DEBUG("buf=%p len=%d eb=%p", buffer, len, eb);

    /*
     * The ring buffer uses a single byte for the pkt length, followed by the mac address,
     * followed by the actual packet data. The MTU for ESP-NOW is 250 bytes, so len will never
     * exceed the limits of a byte as the mac address length is not included.
     */
    if (ringbuffer_get_free(&_esp_wifi_dev.rx_buf) < len + sizeof(uint16_t)) {
        ESP_WIFI_DEBUG("buffer full, dropping incoming packet of %d bytes", len);
        /* free the receive buffer */
        if (eb) {
            esp_wifi_internal_free_rx_buffer(eb);
        }
        /*
         * we must not return a failure code in this case, otherwise,
         * the WiFi driver hangs up
         */
        critical_exit();
        return ESP_OK;
    }

    /* store length information as first two bytes */
    ringbuffer_add(&_esp_wifi_dev.rx_buf, (char *)&len, sizeof(uint16_t));

    /* copy the buffer and free WiFi driver buffer */
    ringbuffer_add(&_esp_wifi_dev.rx_buf, (char *)buffer, len);
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
    _esp_wifi_dev.event_recv++;
    _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev, NETDEV_EVENT_ISR);

    /* reset IRQ nesting counter */
    irq_interrupt_nesting--;

    critical_exit();
    return ESP_OK;
}

#define REASON_BEACON_TIMEOUT      (200)
#define REASON_HANDSHAKE_TIMEOUT   (204)
#define INDEX_BEACON_TIMEOUT       (REASON_BEACON_TIMEOUT - 24)

static const char *_esp_wifi_disc_reasons [] = {
    "INVALID",                     /* 0 */
    "UNSPECIFIED",                 /* 1 */
    "AUTH_EXPIRE",                 /* 2 */
    "AUTH_LEAVE",                  /* 3 */
    "ASSOC_EXPIRE",                /* 4 */
    "ASSOC_TOOMANY",               /* 5 */
    "NOT_AUTHED",                  /* 6 */
    "NOT_ASSOCED",                 /* 7 */
    "ASSOC_LEAVE",                 /* 8 */
    "ASSOC_NOT_AUTHED",            /* 9 */
    "DISASSOC_PWRCAP_BAD",         /* 10 (11h) */
    "DISASSOC_SUPCHAN_BAD",        /* 11 (11h) */
    "IE_INVALID",                  /* 13 (11i) */
    "MIC_FAILURE",                 /* 14 (11i) */
    "4WAY_HANDSHAKE_TIMEOUT",      /* 15 (11i) */
    "GROUP_KEY_UPDATE_TIMEOUT",    /* 16 (11i) */
    "IE_IN_4WAY_DIFFERS",          /* 17 (11i) */
    "GROUP_CIPHER_INVALID",        /* 18 (11i) */
    "PAIRWISE_CIPHER_INVALID",     /* 19 (11i) */
    "AKMP_INVALID",                /* 20 (11i) */
    "UNSUPP_RSN_IE_VERSION",       /* 21 (11i) */
    "INVALID_RSN_IE_CAP",          /* 22 (11i) */
    "802_1X_AUTH_FAILED",          /* 23 (11i) */
    "CIPHER_SUITE_REJECTED",       /* 24 (11i) */
    "BEACON_TIMEOUT",              /* 200 */
    "NO_AP_FOUND",                 /* 201 */
    "AUTH_FAIL",                   /* 202 */
    "ASSOC_FAIL",                  /* 203 */
    "HANDSHAKE_TIMEOUT"            /* 204 */
};

/*
 * Event handler for esp system events.
 */
static esp_err_t IRAM_ATTR _esp_system_event_handler(void *ctx, system_event_t *event)
{
    assert(event != NULL);

    esp_err_t result;

    uint8_t reason;
    const char* reason_str = "UNKNOWN";

    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            ESP_WIFI_DEBUG("WiFi started");
            result = esp_wifi_connect();
            if (result != ESP_OK) {
                ESP_WIFI_LOG_ERROR("esp_wifi_connect failed with return "
                                   "value %d", result);
            }
            break;

        case SYSTEM_EVENT_SCAN_DONE:
            ESP_WIFI_DEBUG("WiFi scan done");
            break;

        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_WIFI_LOG_INFO("connected to ssid %s, channel %d",
                              event->event_info.connected.ssid,
                              event->event_info.connected.channel);

            /* register RX callback function */
            esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, _esp_wifi_rx_cb);

            _esp_wifi_dev.connected = true;
            _esp_wifi_dev.event_conn++;
            _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev, NETDEV_EVENT_ISR);

            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            reason = event->event_info.disconnected.reason;
            if (reason < REASON_BEACON_TIMEOUT) {
                reason_str = _esp_wifi_disc_reasons[reason];
            }
            else if (reason <= REASON_HANDSHAKE_TIMEOUT) {
                reason_str = _esp_wifi_disc_reasons[reason - INDEX_BEACON_TIMEOUT];
            }
            ESP_WIFI_LOG_INFO("disconnected from ssid %s, reason %d (%s)",
                              event->event_info.disconnected.ssid,
                              event->event_info.disconnected.reason, reason_str);

            /* unregister RX callback function */
            esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, NULL);

            _esp_wifi_dev.connected = false;
            _esp_wifi_dev.event_disc++;
            _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev, NETDEV_EVENT_ISR);

            /* call disconnect to reset internal state */
            result = esp_wifi_disconnect();
            if (result != ESP_OK) {
                ESP_WIFI_LOG_ERROR("esp_wifi_disconnect failed with "
                                   "return value %d", result);
                return result;
            }

            /* try to reconnect */
            result = esp_wifi_connect();
            if (result != ESP_OK) {
               ESP_WIFI_LOG_ERROR("esp_wifi_connect failed with "
                                  "return value %d", result);
            }

            break;

        default:
            ESP_WIFI_DEBUG("event %d", event->event_id);
            break;
    }
    return ESP_OK;
}

static int _esp_wifi_send(netdev_t *netdev, const iolist_t *iolist)
{
    ESP_WIFI_DEBUG("netdev=%p iolist=%p", netdev, iolist);

    assert(netdev != NULL);
    assert(iolist != NULL);

    if (_esp_wifi_send_is_in) {
        return 0;
    }
    _esp_wifi_send_is_in = true;

    esp_wifi_netdev_t* dev = (esp_wifi_netdev_t*)netdev;

    if (!_esp_wifi_dev.connected) {
        ESP_WIFI_DEBUG("WiFi is still not connected to AP, cannot send");
        _esp_wifi_send_is_in = false;
        return -ENODEV;
    }

    critical_enter();
    dev->tx_len = 0;

    /* load packet data into TX buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (dev->tx_len + iol->iol_len > ETHERNET_MAX_LEN) {
            _esp_wifi_send_is_in = false;
            critical_exit();
            return -EOVERFLOW;
        }
        if (iol->iol_len) {
            memcpy (dev->tx_buf + dev->tx_len, iol->iol_base, iol->iol_len);
            dev->tx_len += iol->iol_len;
        }
    }

#if ENABLE_DEBUG
    ESP_WIFI_DEBUG("send %d byte", dev->tx_len);
#if MODULE_OD && ENABLE_DEBUG_HEXDUMP
    od_hex_dump(dev->tx_buf, dev->tx_le, OD_WIDTH_DEFAULT);
#endif /* MODULE_OD && ENABLE_DEBUG_HEXDUMP */
#endif
    critical_exit();

    int ret = 0;

    /* send the the packet to the peer(s) mac address */
    if (esp_wifi_internal_tx(ESP_IF_WIFI_STA, dev->tx_buf, dev->tx_len) == ESP_OK) {
#ifdef MCU_ESP32
        /* for ESP8266 it is done in _esp_wifi_tx_cb */
        _esp_wifi_send_is_in = false;
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
#endif
    }
    else {
        _esp_wifi_send_is_in = false;
        ESP_WIFI_DEBUG("sending WiFi packet failed");
        ret = -EIO;
    }

    return ret;
}

static int _esp_wifi_recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    ESP_WIFI_DEBUG("%p %p %u %p", netdev, buf, len, info);

    assert(netdev != NULL);

    esp_wifi_netdev_t* dev = (esp_wifi_netdev_t*)netdev;
    uint16_t size;

    critical_enter();

    _esp_wifi_rx_in_progress = true;

    if (ringbuffer_peek(&dev->rx_buf, (char *)&size, sizeof(uint16_t)) < sizeof(uint16_t)) {
        critical_exit();
        return 0;
    }

    if (!buf) {
        /* get the size of the frame */
        if (len > 0 && size) {
            /* if len > 0, drop the frame */
            ringbuffer_remove(&dev->rx_buf, sizeof(uint16_t) + size);
            _esp_wifi_rx_in_progress = false;
        }
        critical_exit();
        return size;
    }

    if (len < size) {
        /* buffer is smaller than the number of received bytes */
        ESP_WIFI_DEBUG("not enough space in receive buffer");
        /* newest API requires to drop the frame in that case */
        ringbuffer_remove(&dev->rx_buf, sizeof(uint16_t) + size);
        _esp_wifi_rx_in_progress = false;
        critical_exit();
        return -ENOBUFS;
    }

    /* remove length bytes, copy the buffer to the ringbuffer and free it */
    ringbuffer_remove(&dev->rx_buf, sizeof(uint16_t));
    ringbuffer_get(&dev->rx_buf, buf, size);

#if ENABLE_DEBUG
    ethernet_hdr_t *hdr = (ethernet_hdr_t *)buf;

    ESP_WIFI_DEBUG("received %u byte from addr " MAC_STR,
                   size, MAC_STR_ARG(hdr->src));
#if MODULE_OD && ENABLE_DEBUG_HEXDUMP
    od_hex_dump(buf, size, OD_WIDTH_DEFAULT);
#endif /* MODULE_OD && ENABLE_DEBUG_HEXDUMP */
#endif /* ENABLE_DEBUG */

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
        case NETOPT_IS_WIRED:
            return -ENOTSUP;
        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
            esp_wifi_get_mac(ESP_MAC_WIFI_STA,(uint8_t *)val);
            return ETHERNET_ADDR_LEN;
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

    while (dev->event_recv) {
        dev->event_recv--;
        dev->netdev.event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
    if (dev->event_conn) {
        dev->event_conn--;
        dev->netdev.event_callback(netdev, NETDEV_EVENT_LINK_UP);
    }
    else if (dev->event_disc) {
        dev->event_disc--;
        dev->netdev.event_callback(netdev, NETDEV_EVENT_LINK_DOWN);
    }

    return;
}

static int _esp_wifi_init(netdev_t *netdev)
{
    ESP_WIFI_DEBUG("%p", netdev);

    return 0;
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

/*
 * Static configuration for the Station interface
 */
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

#ifndef MODULE_ESP_NOW
/**
 * Static configuration for the SoftAP interface if ESP-NOW is not enabled.
 *
 * Although only the Station interface is needed, the SoftAP interface must
 * also be enabled for stability reasons to prevent the Station interface
 * from being shut down by power management in the event of silence.
 * Otherwise, the WLAN module and the WLAN task will hang sporadically.
 *
 * Since the SoftAP interface is not required, we make it invisible and
 * unusable. This configuration
 *
 * - uses the same hidden SSID that the Station interface uses to
 *   connect to the AP,
 * - uses the same channel that the Station interface uses to connect to the AP,
 * - defines a very long beacon interval
 * - doesn't allow any connection.
 */
static wifi_config_t wifi_config_ap = {
    .ap = {
        .ssid = ESP_WIFI_SSID,
        .ssid_len = ARRAY_SIZE(ESP_WIFI_SSID),
        .ssid_hidden = 1,               /* don't make the AP visible */
        .password = ESP_WIFI_PASS,
        .authmode = WIFI_AUTH_WPA2_PSK,
        .max_connection = 0,            /* don't allow connections */
        .beacon_interval = 60000,       /* send beacon only every 60 s */
    }
};
#endif

void esp_wifi_setup (esp_wifi_netdev_t* dev)
{
    ESP_WIFI_DEBUG("dev=%p", dev);

    /* initialize buffer */
    ringbuffer_init(&dev->rx_buf, (char*)dev->rx_mem, sizeof(dev->rx_mem));

    /* set the event handler */
    esp_system_event_add_handler(_esp_system_event_handler, NULL);

    /*
     * Init the WiFi driver. TODO It is not only required before ESP_WIFI is
     * initialized but also before other WiFi functions are used. Once other
     * WiFi functions are realized it has to be moved to a more common place.
     */
    esp_err_t result;

#ifndef MODULE_ESP_NOW
    /* if module esp_now is used, the following part is already done */
#if MCU_ESP32
    extern portMUX_TYPE g_intr_lock_mux;
    mutex_init(&g_intr_lock_mux);
#endif

#if CONFIG_ESP32_WIFI_NVS_ENABLED
    result = nvs_flash_init();
    if (result != ESP_OK) {
        ESP_WIFI_LOG_ERROR("nfs_flash_init failed with return value %d", result);
        return;
    }
#endif /* CONFIG_ESP32_WIFI_NVS_ENABLED */

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    result = esp_wifi_init(&cfg);
    if (result != ESP_OK) {
        ESP_WIFI_LOG_ERROR("esp_wifi_init failed with return value %d", result);
        return;
    }

#ifdef CONFIG_WIFI_COUNTRY
    /* TODO */
#endif

    /* activate the Station and the SoftAP interface */
    result = esp_wifi_set_mode(WIFI_MODE_APSTA);
    if (result != ESP_OK) {
        ESP_WIFI_LOG_ERROR("esp_wifi_set_mode failed with return value %d", result);
        return;
    }

    /* set the SoftAP configuration */
    result = esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config_ap);
    if (result != ESP_OK) {
        ESP_WIFI_LOG_ERROR("esp_wifi_set_config softap failed with return value %d", result);
        return;
    }

#endif /* MODULE_ESP_NOW */

    /* set the Station configuration */
    result = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta);
    if (result != ESP_OK) {
        ESP_WIFI_LOG_ERROR("esp_wifi_set_config station failed with return value %d", result);
        return;
    }

    /* start the WiFi driver */
    result = esp_wifi_start();
    if (result != ESP_OK) {
        ESP_WIFI_LOG_ERROR("esp_wifi_start failed with return value %d", result);
        return;
    }

    /* register RX callback function */
    esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA, _esp_wifi_rx_cb);

    /* set the netdev driver */
    dev->netdev.driver = &_esp_wifi_driver;

    /* initialize netdev data structure */
    dev->connected = false;
    dev->event_recv = 0;
    dev->event_conn = 0;
    dev->event_disc = 0;
}

void auto_init_esp_wifi (void)
{
    ESP_WIFI_DEBUG("initializing ESP WiFi device");

    esp_wifi_setup(&_esp_wifi_dev);
    _esp_wifi_dev.netif = gnrc_netif_ethernet_create(_esp_wifi_stack,
                                                    ESP_WIFI_STACKSIZE,
#ifdef MODULE_ESP_NOW
                                                    ESP_WIFI_PRIO - 1,
#else
                                                    ESP_WIFI_PRIO,
#endif
                                                    "esp_wifi",
                                                    (netdev_t *)&_esp_wifi_dev);
}

#endif /* MODULE_ESP_WIFI */
/**@}*/
