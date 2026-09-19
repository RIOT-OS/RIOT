/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp_common_esp_wifi
 * @{
 *
 * @file
 * @brief       Network device driver for the ESP SoCs WiFi interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifdef MODULE_ESP_WIFI

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "net/ethernet.h"
#include "net/netdev/eth.h"
#include "od.h"

#include "esp_common.h"
#include "esp_attr.h"
#ifdef CPU_ESP8266
#include "esp_event_loop.h"
#else
#include "esp_event.h"
#include "esp_event_legacy.h"
#endif
#ifndef MODULE_ESP_WIFI_AP
#include "esp_now.h"
#endif
#include "esp_system.h"
#include "esp_wifi.h"
#ifndef CPU_ESP8266
#include "esp_private/wifi.h"
#endif
#include "irq_arch.h"
#include "tools.h"

#include "nvs_flash.h"

#ifdef MODULE_ESP_WIFI_ENTERPRISE
#include "esp_eap_client.h"
#endif

#include "esp_wifi_params.h"
#include "esp_wifi_netdev.h"

#include "log.h"
#include "board.h"

#define ESP_WIFI_DEBUG(f, ...) \
        DEBUG("[esp_wifi] %s: " f "\n", __func__, ## __VA_ARGS__)

#define ESP_WIFI_LOG_INFO(f, ...) \
        LOG_TAG_INFO("esp_wifi", f "\n", ## __VA_ARGS__)

#define ESP_WIFI_LOG_ERROR(f, ...) \
        LOG_TAG_ERROR("esp_wifi", f "\n", ## __VA_ARGS__)

#define ESP_WIFI_LOG_DEBUG(f, ...) \
        LOG_TAG_DEBUG("esp_wifi", f "\n", ## __VA_ARGS__)

#define MAC_STR                     "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_STR_ARG(m)              m[0], m[1], m[2], m[3], m[4], m[5]

#ifdef CPU_ESP8266

#include "esp_socket.h"
#include "net/sockio.h"
#include "xtensa/xtensa_context.h"

#define CONFIG_TCP_OVERSIZE_MSS 1
#define LL_ALIGN(s)             (((uint32_t)s + 3) & 0xfffffffcU)

/**
 * The SDK interface of the WiFi module uses the lwIP `pbuf` structure for
 * packets sent to and received from the WiFi interface. For compatibility
 * reasons with the binary SDK libraries we need to include the SDK lwIP
 * `pbuf` header here.
 *
 * To avoid compilation errors, we need to undefine all our pkg/lwIP settings
 * that are also defined by SDK lwIP header files. These definitions do not
 * affect the implementation of this module.
 */
#undef ETHARP_SUPPORT_STATIC_ENTRIES
#undef LWIP_HAVE_LOOPIF
#undef LWIP_NETIF_LOOPBACK
#undef SO_REUSE
#undef TCPIP_THREAD_PRIO
#undef TCPIP_THREAD_STACKSIZE

#include "lwip/pbuf.h"

#endif /* CPU_ESP8266 */

#define ENABLE_DEBUG_HEXDUMP    0
#define ENABLE_DEBUG            0
#include "debug.h"

/**
 * There is only one ESP WiFi device. We define it as static device variable
 * to have access to the device inside ESP WiFi interrupt routines which do
 * not provide an argument that could be used as pointer to the ESP WiFi
 * device which triggers the interrupt.
 */
esp_wifi_netdev_t _esp_wifi_dev;
static const netdev_driver_t _esp_wifi_driver;

/** guard variable to avoid reentrance to _esp_wifi_send function */
static bool _esp_wifi_send_is_in = false;

/** guard variable to to decive when receive buffer can be overwritten */
static bool _esp_wifi_rx_in_progress = false;

extern esp_err_t esp_system_event_add_handler (system_event_cb_t handler,
                                               void *arg);

#ifdef CPU_ESP8266

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
    (void)wifi_if;
    ESP_WIFI_DEBUG("buf=%p len=%u", buf, len);

    struct pbuf *pb = _esp_wifi_pbuf_alloc(len);
    if (pb == NULL) {
        return ERR_MEM;
    }
    if (len) {
        memcpy(pb->payload, buf, len);
    }

    esp_aio_t aio;
    aio.fd = _esp_wifi_socket;
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
    assert(ifx == ESP_IF_WIFI_STA || ifx == ESP_IF_WIFI_AP);

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
    const char *ifx_name = "sta0";
    if (ifx == ESP_IF_WIFI_AP) {
        ifx_name = "ap0";
    }
    if (_esp_wifi_socket < 0) {
        ESP_WIFI_LOG_ERROR("create socket of (AF_PACKET, SOCK_RAW, ETH_P_ALL) error");
        return ESP_FAIL;
    }
    if (esp_ioctl(_esp_wifi_socket, SIOCGIFINDEX, ifx_name) < 0) {
        ESP_WIFI_LOG_ERROR("bind socket %d to netcard %s error", _esp_wifi_socket, ifx_name);
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

#endif /* CPU_ESP8266 */

#ifdef CPU_ESP8266

/* Prolog for source code compatibility with ESP-IDF for ESP32 */
static int _esp_wifi_rx_cb(struct esp_aio *aio)
{
    assert(aio != NULL);

    const char *eb = aio->pbuf;
    const char *buffer = aio->pbuf;
    uint16_t len = aio->len;

#else /* CPU_ESP8266 */

esp_err_t _esp_wifi_rx_cb(void *buffer, uint16_t len, void *eb)
{
#endif /* CPU_ESP8266 */

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
     * The ring buffer uses two bytes for the pkt length, followed by the
     * actual packet data.
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
    netdev_trigger_event_isr(&_esp_wifi_dev.netdev);

    /* reset IRQ nesting counter */
    irq_interrupt_nesting--;

    critical_exit();
    return ESP_OK;
}

#ifndef MODULE_ESP_WIFI_AP
static const char *_esp_wifi_disc_reasons[] = {
    "INVALID",                                  /* 0 */
    "UNSPECIFIED",                              /* 1 */
    "AUTH_EXPIRE",                              /* 2 */
    "AUTH_LEAVE",                               /* 3 */
    "ASSOC_EXPIRE",                             /* 4 */
    "ASSOC_TOOMANY",                            /* 5 */
    "NOT_AUTHED",                               /* 6 */
    "NOT_ASSOCED",                              /* 7 */
    "ASSOC_LEAVE",                              /* 8 */
    "ASSOC_NOT_AUTHED",                         /* 9 */
    "DISASSOC_PWRCAP_BAD",                      /* 10 */
    "DISASSOC_SUPCHAN_BAD",                     /* 11 */
    "BSS_TRANSITION_DISASSOC",                  /* 12 */
    "IE_INVALID",                               /* 13 */
    "MIC_FAILURE",                              /* 14 */
    "4WAY_HANDSHAKE_TIMEOUT",                   /* 15 */
    "GROUP_KEY_UPDATE_TIMEOUT",                 /* 16 */
    "IE_IN_4WAY_DIFFERS",                       /* 17 */
    "GROUP_CIPHER_INVALID",                     /* 18 */
    "PAIRWISE_CIPHER_INVALID",                  /* 19 */
    "AKMP_INVALID",                             /* 20 */
    "UNSUPP_RSN_IE_VERSION",                    /* 21 */
    "INVALID_RSN_IE_CAP",                       /* 22 */
    "802_1X_AUTH_FAILED",                       /* 23 */
    "CIPHER_SUITE_REJECTED",                    /* 24 */
    "TDLS_PEER_UNREACHABLE",                    /* 25 */
    "TDLS_UNSPECIFIED",                         /* 26 */
    "SSP_REQUESTED_DISASSOC",                   /* 27 */
    "NO_SSP_ROAMING_AGREEMENT",                 /* 28 */
    "BAD_CIPHER_OR_AKM",                        /* 29 */
    "NOT_AUTHORIZED_THIS_LOCATION",             /* 30 */
    "SERVICE_CHANGE_PERCLUDES_TS",              /* 31 */
    "UNSPECIFIED_QOS",                          /* 32 */
    "NOT_ENOUGH_BANDWIDTH",                     /* 33 */
    "MISSING_ACKS",                             /* 34 */
    "EXCEEDED_TXOP",                            /* 35 */
    "STA_LEAVING",                              /* 36 */
    "END_BA",                                   /* 37 */
    "UNKNOWN_BA",                               /* 38 */
    "TIMEOUT",                                  /* 39 */

    "PEER_INITIATED",                           /* 46 */
    "AP_INITIATED",                             /* 47 */
    "INVALID_FT_ACTION_FRAME_COUNT",            /* 48 */
    "INVALID_PMKID",                            /* 49 */
    "INVALID_MDE",                              /* 50 */
    "INVALID_FTE",                              /* 51 */

    "TRANSMISSION_LINK_ESTABLISH_FAILED",       /* 67 */
    "ALTERATIVE_CHANNEL_OCCUPIED",              /* 68 */

    "BEACON_TIMEOUT",                           /* 200 */
    "NO_AP_FOUND",                              /* 201 */
    "AUTH_FAIL",                                /* 202 */
    "ASSOC_FAIL",                               /* 203 */
    "HANDSHAKE_TIMEOUT",                        /* 204 */
    "CONNECTION_FAIL",                          /* 205 */
    "AP_TSF_RESET",                             /* 206 */
    "ROAMING",                                  /* 207 */
    "WIFI_REASON_ASSOC_COMEBACK_TIME_TOO_LONG"  /* 208 */
};

typedef struct _esp_wifi_valid_disc_reason_codes {
    uint8_t from;
    uint8_t to;
} _esp_wifi_valid_disc_reason_codes_t;

static const _esp_wifi_valid_disc_reason_codes_t _esp_wifi_valid_disc_reasons[] = {
    /* From, To */
    { 0, 39 },
    { 46, 51 },
    { 67, 68 },
    { 200, 208 },
};

static const char *_esp_wifi_get_disc_reason(uint8_t code)
{
    uint8_t offset = 0;
    uint8_t valid_reasons_len = ARRAY_SIZE(_esp_wifi_valid_disc_reasons);
    for (uint8_t i = 0; i < valid_reasons_len; i++) {
        if ((_esp_wifi_valid_disc_reasons[i].from <= code) &&
            (_esp_wifi_valid_disc_reasons[i].to >= code))
        {
            return _esp_wifi_disc_reasons[code - offset];
        }
        else if (i < (valid_reasons_len - 1)) {
            offset += (_esp_wifi_valid_disc_reasons[i + 1].from - 1) -
                      _esp_wifi_valid_disc_reasons[i].to;
        }
    }
    return "UNKNOWN";
}
#endif /* MODULE_ESP_WIFI_AP */

/* indicator whether the WiFi interface is started */
static unsigned _esp_wifi_started = 0;

/* current channel used by the WiFi interface */
static unsigned _esp_wifi_channel = 0;

/*
 * Event handler for esp system events.
 */
static esp_err_t IRAM_ATTR _esp_system_event_handler(void *ctx, system_event_t *event)
{
    assert(event != NULL);

#ifndef MODULE_ESP_WIFI_AP
    esp_err_t result;

    uint8_t reason;
#endif /* MODULE_ESP_WIFI_AP */

    switch (event->event_id) {
#ifdef MODULE_ESP_WIFI_AP
        case SYSTEM_EVENT_AP_START:
            _esp_wifi_started = 1;
            esp_wifi_internal_reg_rxcb(WIFI_IF_AP, _esp_wifi_rx_cb);
            ESP_WIFI_DEBUG("WiFi started");
            break;

        case SYSTEM_EVENT_AP_STOP:
            _esp_wifi_started = 0;
            esp_wifi_internal_reg_rxcb(WIFI_IF_AP, NULL);
            ESP_WIFI_DEBUG("WiFi stopped");
            break;

        case SYSTEM_EVENT_AP_STACONNECTED:
            _esp_wifi_dev.sta_connected += 1;
            ESP_WIFI_LOG_INFO("Station "MAC_STR" join (AID=%d)",
                              MAC_STR_ARG(event->event_info.sta_connected.mac),
                              event->event_info.sta_connected.aid);
            break;

        case SYSTEM_EVENT_AP_STADISCONNECTED:
            _esp_wifi_dev.sta_connected -= 1;
            ESP_WIFI_LOG_INFO("Station "MAC_STR" leave (AID=%d)",
                              MAC_STR_ARG(event->event_info.sta_disconnected.mac),
                              event->event_info.sta_disconnected.aid);
            break;

        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            ESP_WIFI_LOG_DEBUG("Station "MAC_STR" probed (rssi=%d)",
                               MAC_STR_ARG(event->event_info.ap_probereqrecved.mac),
                               event->event_info.ap_probereqrecved.rssi);
            break;
#else /* MODULE_ESP_WIFI_AP */
        case SYSTEM_EVENT_STA_START:
            _esp_wifi_started = 1;
            ESP_WIFI_DEBUG("WiFi started");
            result = esp_wifi_connect();
            if (result != ESP_OK) {
                ESP_WIFI_LOG_ERROR("esp_wifi_connect failed with return "
                                   "value %d", result);
            }
            break;

        case SYSTEM_EVENT_STA_STOP:
            _esp_wifi_started = 0;
            ESP_WIFI_DEBUG("WiFi stopped");
            break;

        case SYSTEM_EVENT_SCAN_DONE:
            ESP_WIFI_DEBUG("WiFi scan done");
            break;

        case SYSTEM_EVENT_STA_CONNECTED:
            ESP_WIFI_LOG_INFO("WiFi connected to ssid %s, channel %d",
                              event->event_info.connected.ssid,
                              event->event_info.connected.channel);
            _esp_wifi_channel = event->event_info.connected.channel;
#ifdef MODULE_ESP_NOW
            extern void esp_now_set_channel(uint8_t channel);
            esp_now_set_channel(_esp_wifi_channel);
#endif
            /* register RX callback function */
            esp_wifi_internal_reg_rxcb(WIFI_IF_STA, _esp_wifi_rx_cb);

            _esp_wifi_dev.connected = true;
            _esp_wifi_dev.event_conn++;
            netdev_trigger_event_isr(&_esp_wifi_dev.netdev);

            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            reason = event->event_info.disconnected.reason;
            ESP_WIFI_LOG_INFO("WiFi disconnected from ssid %s, reason %d (%s)",
                              event->event_info.disconnected.ssid,
                              reason, _esp_wifi_get_disc_reason(reason));

            /* unregister RX callback function */
            esp_wifi_internal_reg_rxcb(WIFI_IF_STA, NULL);

            _esp_wifi_dev.connected = false;
            _esp_wifi_dev.event_disc++;
            netdev_trigger_event_isr(&_esp_wifi_dev.netdev);

            if (reason != WIFI_REASON_ASSOC_LEAVE) {
                /* call disconnect to reset internal state */
                result = esp_wifi_disconnect();
                if (result != ESP_OK) {
                    ESP_WIFI_LOG_ERROR("esp_wifi_disconnect failed with "
                                       "return value %d", result);
                    return result;
                }

                /* try to reconnect */
                if (_esp_wifi_started && ((result = esp_wifi_connect()) != ESP_OK)) {
                   ESP_WIFI_LOG_ERROR("esp_wifi_connect failed with "
                                      "return value %d", result);
                }
            }

            break;
#endif /* MODULE_ESP_WIFI_AP */

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

    esp_wifi_netdev_t* dev = container_of(netdev, esp_wifi_netdev_t, netdev);

#ifdef MODULE_ESP_WIFI_AP
    if (_esp_wifi_dev.sta_connected == 0) {
        ESP_WIFI_DEBUG("No STAs are connected to SoftAP, cannot send");
        _esp_wifi_send_is_in = false;
        return -ENODEV;
    }
#else /* MODULE_ESP_WIFI_AP */
    if (!_esp_wifi_dev.connected) {
        ESP_WIFI_DEBUG("WiFi is still not connected to AP, cannot send");
        _esp_wifi_send_is_in = false;
        return -ENODEV;
    }
#endif /* MODULE_ESP_WIFI_AP */

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

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        ESP_WIFI_DEBUG("send %d byte", dev->tx_len);
        if (IS_ACTIVE(ENABLE_DEBUG_HEXDUMP) && IS_USED(MODULE_OD)) {
            od_hex_dump(dev->tx_buf, dev->tx_len, OD_WIDTH_DEFAULT);
        }
    }

    critical_exit();

#ifdef MODULE_ESP_WIFI_AP
    if (esp_wifi_internal_tx(WIFI_IF_AP, dev->tx_buf, dev->tx_len) == ESP_OK) {
#else /* MODULE_ESP_WIFI_AP */
    /* send the packet to the peer(s) mac address */
    if (esp_wifi_internal_tx(WIFI_IF_STA, dev->tx_buf, dev->tx_len) == ESP_OK) {
#endif
#ifndef CPU_ESP8266
        /* for ESP8266 it is done in _esp_wifi_tx_cb */
        _esp_wifi_send_is_in = false;
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
#endif
        return dev->tx_len;
    }
    else {
        _esp_wifi_send_is_in = false;
        ESP_WIFI_DEBUG("sending WiFi packet failed");
        return -EIO;
    }
}

static int _esp_wifi_recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    ESP_WIFI_DEBUG("%p %p %u %p", netdev, buf, len, info);

    assert(netdev != NULL);

    esp_wifi_netdev_t* dev = container_of(netdev, esp_wifi_netdev_t, netdev);
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

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        ethernet_hdr_t *hdr = (ethernet_hdr_t *)buf;
        ESP_WIFI_DEBUG("received %u byte from addr " MAC_STR,
                    size, MAC_STR_ARG(hdr->src));

        if (IS_ACTIVE(ENABLE_DEBUG_HEXDUMP) && IS_USED(MODULE_OD)) {
            od_hex_dump(buf, size, OD_WIDTH_DEFAULT);
        }
    }

    critical_exit();
    return size;
}

static int _esp_wifi_get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    ESP_WIFI_DEBUG("%s %p %p %u", netopt2str(opt), netdev, val, max_len);

    assert(netdev != NULL);

#ifndef MODULE_ESP_WIFI_AP
    esp_wifi_netdev_t* dev = container_of(netdev, esp_wifi_netdev_t, netdev);
#endif /* MODULE_ESP_WIFI_AP */

    switch (opt) {
        case NETOPT_IS_WIRED:
            return -ENOTSUP;
        case NETOPT_CHANNEL:
            CHECK_PARAM_RET(max_len >= sizeof(uint16_t), -EOVERFLOW);
            *((uint16_t *)val) = _esp_wifi_channel;
            return sizeof(uint16_t);
        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
#ifdef MODULE_ESP_WIFI_AP
            esp_wifi_get_mac(WIFI_IF_AP, (uint8_t *)val);
#else /* MODULE_ESP_WIFI_AP */
            esp_wifi_get_mac(WIFI_IF_STA, (uint8_t *)val);
#endif /* MODULE_ESP_WIFI_AP */
            return ETHERNET_ADDR_LEN;
        case NETOPT_LINK:
            assert(max_len == sizeof(netopt_enable_t));
#ifdef MODULE_ESP_WIFI_AP
            *((netopt_enable_t *)val) = (_esp_wifi_started) ? NETOPT_ENABLE
                                                            : NETOPT_DISABLE;
#else /* MOUDLE_ESP_WIFI_AP */
            *((netopt_enable_t *)val) = (dev->connected) ? NETOPT_ENABLE
                                                         : NETOPT_DISABLE;
#endif /* MODULE_ESP_WIFI_AP */
            return sizeof(netopt_enable_t);
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
#ifdef MODULE_ESP_WIFI_AP
            esp_wifi_set_mac(WIFI_IF_AP, (uint8_t *)val);
#else /* MODULE_ESP_WIFI_AP */
            esp_wifi_set_mac(WIFI_IF_STA, (uint8_t *)val);
#endif /* MODULE_ESP_WIFI_AP */
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_set(netdev, opt, val, max_len);
    }
}

static void _esp_wifi_isr(netdev_t *netdev)
{
    ESP_WIFI_DEBUG("%p", netdev);

    assert(netdev != NULL);

    esp_wifi_netdev_t* dev = container_of(netdev, esp_wifi_netdev_t, netdev);

    while (dev->event_recv) {
        dev->event_recv--;
        dev->netdev.event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
#ifndef MODULE_ESP_WIFI_AP
    if (dev->event_conn) {
        dev->event_conn--;
        dev->netdev.event_callback(netdev, NETDEV_EVENT_LINK_UP);
    }
    else if (dev->event_disc) {
        dev->event_disc--;
        dev->netdev.event_callback(netdev, NETDEV_EVENT_LINK_DOWN);
    }
#endif

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

#ifndef MODULE_ESP_WIFI_AP
/*
 * Static configuration for the Station interface
 */
static wifi_config_t wifi_config_sta = {
    .sta = {
        .ssid = WIFI_SSID,
#if !defined(MODULE_ESP_WIFI_ENTERPRISE) && defined(WIFI_PASS)
        .password = WIFI_PASS,
#endif
        .channel = 0,
        .scan_method = WIFI_ALL_CHANNEL_SCAN,
        .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
        .threshold.rssi = -127,
        .threshold.authmode = WIFI_AUTH_OPEN
    }
};
#endif /* MODULE_ESP_WIFI_AP */

#if (defined(CPU_ESP8266) && !defined(MODULE_ESP_NOW)) || defined(MODULE_ESP_WIFI_AP)
/**
 * Static configuration for the SoftAP interface if ESP-NOW is not enabled.
 *
 * Although only the Station interface is needed, the SoftAP interface must
 * also be enabled for stability reasons to prevent the Station interface
 * from being shut down by power management in the event of silence.
 * Otherwise, the WiFi module and the WiFi task will hang sporadically.
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
#ifdef WIFI_SSID
        .ssid = WIFI_SSID,
        .ssid_len = sizeof(WIFI_SSID) - 1,
#endif
#ifdef WIFI_PASS
        .password = WIFI_PASS,
        .authmode = WIFI_AUTH_WPA2_PSK,
#else
        .authmode = WIFI_AUTH_OPEN,
#endif
#ifdef MODULE_ESP_WIFI_AP
        .ssid_hidden = ESP_WIFI_SSID_HIDDEN, /* don't make the AP visible */
        .max_connection = ESP_WIFI_MAX_CONN, /* maximum number of connections */
        .beacon_interval = ESP_WIFI_BEACON_INTERVAL,
#else
        .ssid_hidden = 1,
        .max_connection = 0,                 /* don't allow connections */
        .beacon_interval = 60000,            /* send beacon only every 60 s */
#endif
    }
};
#endif /* (defined(CPU_ESP8266) && !defined(MODULE_ESP_NOW)) || defined(MODULE_ESP_WIFI_AP) */


#if (defined(MCU_ESP32) && defined(MODULE_ESP_WIFI_CSI))
void __attribute__((weak)) esp_wifi_csi_rx_cb(void *ctx, wifi_csi_info_t *info)
{
    if (!info || !info->buf) {
        ESP_WIFI_LOG_ERROR("ERROR: esp_wifi_csi_rx_cb missing info or buf");
        return;
    }
    printf("{\n");
    printf("  \"mac\": \"" MAC_STR "\",\n", MAC_STR_ARG(info->mac));
#ifdef MODULE_ESP_WIFI_CSI_METADATA
    printf("  \"rssi\": %d,\n", info->rx_ctrl.rssi);
    printf("  \"rate\": %d,\n", info->rx_ctrl.rate);
    printf("  \"sig_mode\": %d,\n", info->rx_ctrl.sig_mode);
    printf("  \"mcs\": %d,\n", info->rx_ctrl.mcs);
    printf("  \"cwb\": %d,\n", info->rx_ctrl.cwb);
    printf("  \"smoothing\": %d,\n", info->rx_ctrl.smoothing);
    printf("  \"not_sounding\": %d,\n", info->rx_ctrl.not_sounding);
    printf("  \"aggregation\": %d,\n", info->rx_ctrl.aggregation);
    printf("  \"stbc\": %d,\n", info->rx_ctrl.stbc);
    printf("  \"fec_coding\": %d,\n", info->rx_ctrl.fec_coding);
    printf("  \"sgi\": %d,\n", info->rx_ctrl.sgi);
    printf("  \"noise_floor\": %d,\n", info->rx_ctrl.noise_floor);
    printf("  \"ampdu_cnt\": %d,\n", info->rx_ctrl.ampdu_cnt);
    printf("  \"channel\": %d,\n", info->rx_ctrl.channel);
    printf("  \"secondary_channel\": %d,\n", info->rx_ctrl.secondary_channel);
    printf("  \"timestamp\": %d,\n", info->rx_ctrl.timestamp);
    printf("  \"ant\": %d,\n", info->rx_ctrl.ant);
    printf("  \"sig_len\": %d,\n", info->rx_ctrl.sig_len);
    printf("  \"rx_state\": %d,\n", info->rx_ctrl.rx_state);
#endif
    printf("  \"first_word_invalid\": %d,\n", info->first_word_invalid);

    printf("  \"buf\": [");
    for (int i = 0; i < info->len; i++) {
        printf("%u", (uint8_t)info->buf[i]);
        if (i < info->len - 1) {
            printf(",");
        }
    }
    printf("]\n");
    puts("}");

}

static wifi_csi_config_t wifi_config_csi = {
    .lltf_en = true,
    .htltf_en = true,
    .stbc_htltf2_en = true,
    .ltf_merge_en = true,
    .channel_filter_en = true,
    .manu_scale = false,
    .shift = 0,
};
#endif /* (defined(MCU_ESP32) && defined(MODULE_ESP_WIFI_CSI)) */

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
#ifndef CPU_ESP8266
    extern portMUX_TYPE g_intr_lock_mux;
    mutex_init(&g_intr_lock_mux);
#endif

#if CONFIG_ESP_WIFI_NVS_ENABLED
    result = nvs_flash_init();
    if (result != ESP_OK) {
        ESP_WIFI_LOG_ERROR("nfs_flash_init failed with return value %d", result);
        return;
    }
#endif /* CONFIG_ESP_WIFI_NVS_ENABLED */

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    result = esp_wifi_init(&cfg);
    if (result != ESP_OK) {
        ESP_WIFI_LOG_ERROR("esp_wifi_init failed with return value %d", result);
        return;
    }

#ifdef CONFIG_WIFI_COUNTRY
    /* TODO */
#endif

#ifdef MODULE_ESP_WIFI_AP
    /* Activate the SoftAP interface */
    result = esp_wifi_set_mode(WIFI_MODE_AP);
#elif defined(CPU_ESP8266)
    /*
     * Although only the Station interface is needed, the SoftAP interface must
     * also be enabled on ESP8266 for stability reasons to prevent the Station
     * interface from being shut down by power management in the event of
     * silence. Otherwise, the WiFi module and the WiFi task will hang
     * sporadically.
     */
    /* activate the Station and the SoftAP interface */
    result = esp_wifi_set_mode(WIFI_MODE_APSTA);
#else /* defined(CPU_ESP8266) */
    /* activate only the Station interface */
    result = esp_wifi_set_mode(WIFI_MODE_STA);
#endif /* defined(CPU_ESP8266) */
    if (result != ESP_OK) {
        ESP_WIFI_LOG_ERROR("esp_wifi_set_mode failed with return value %d", result);
        return;
    }

#if defined(CPU_ESP8266) || defined(MODULE_ESP_WIFI_AP)
#if IS_ACTIVE(ESP_WIFI_SSID_DYNAMIC)
    uint8_t mac[ETHERNET_ADDR_LEN];
    esp_wifi_get_mac(WIFI_IF_AP, mac);
    sprintf((char*)wifi_config_ap.ap.ssid, "%s_%02x%02x%02x%02x%02x%02x",
            WIFI_SSID, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    wifi_config_ap.ap.ssid_len = strlen((char*)wifi_config_ap.ap.ssid);
#endif /* IS_ACTIVE(ESP_WIFI_SSID_DYNAMIC) */
    /* set the SoftAP configuration */
    result = esp_wifi_set_config(WIFI_IF_AP, &wifi_config_ap);
    if (result != ESP_OK) {
        ESP_WIFI_LOG_ERROR("esp_wifi_set_config softap failed with return value %d", result);
        return;
    }
#endif /* defined(CPU_ESP8266) || defined(MODULE_ESP_WIFI_AP) */

#endif /* MODULE_ESP_NOW */

#ifndef MODULE_ESP_WIFI_AP
    /* set the Station configuration */
    result = esp_wifi_set_config(WIFI_IF_STA, &wifi_config_sta);
    if (result != ESP_OK) {
        ESP_WIFI_LOG_ERROR("esp_wifi_set_config station failed with return value %d", result);
        return;
    }
#endif /* MODULE_ESP_WIFI_AP */

#if defined(MODULE_ESP_WIFI_ENTERPRISE) && !defined(MODULE_ESP_WIFI_AP)

#if !defined(WIFI_EAP_ID) && defined(ESP_WIFI_EAP_ID)
#define WIFI_EAP_ID     ESP_WIFI_EAP_ID
#endif

#if !defined(WIFI_EAP_USER) && defined(ESP_WIFI_EAP_USER)
#define WIFI_EAP_USER   ESP_WIFI_EAP_USER
#endif

#if !defined(WIFI_EAP_PASS) && defined(ESP_WIFI_EAP_PASS)
#define WIFI_EAP_PASS   ESP_WIFI_EAP_PASS
#endif

#ifdef WIFI_EAP_ID
    esp_eap_client_set_identity((const unsigned char *)WIFI_EAP_ID,
                                strlen(WIFI_EAP_ID));
#endif /* WIFI_EAP_ID */
#if defined(WIFI_EAP_USER) && defined(WIFI_EAP_PASS)
    ESP_WIFI_DEBUG("eap_user=%s eap_pass=%s\n",
                   WIFI_EAP_USER, WIFI_EAP_PASS);
    esp_eap_client_set_username((const unsigned char *)WIFI_EAP_USER,
                                strlen(WIFI_EAP_USER));
    esp_eap_client_set_password((const unsigned char *)WIFI_EAP_PASS,
                                strlen(WIFI_EAP_PASS));
#else /* defined(WIFI_EAP_USER) && defined(WIFI_EAP_PASS) */
#error "WIFI_EAP_USER and WIFI_EAP_PASS have to be defined for EAP phase 2 authentication"
#endif /* defined(WIFI_EAP_USER) && defined(WIFI_EAP_PASS) */
    esp_wifi_sta_enterprise_enable();
#endif /* defined(MODULE_ESP_WIFI_ENTERPRISE) && !defined(MODULE_ESP_WIFI_AP) */

    /* start the WiFi driver */
    result = esp_wifi_start();
    if (result != ESP_OK) {
        ESP_WIFI_LOG_ERROR("esp_wifi_start failed with return value %d", result);
        return;
    }

    /* set the netdev driver */
    dev->netdev.driver = &_esp_wifi_driver;

    /* initialize netdev data structure */
    dev->event_recv = 0;
#ifdef MODULE_ESP_WIFI_AP
    dev->sta_connected = 0;
#else /* MODULE_ESP_WIFI_AP */
    dev->event_conn = 0;
    dev->event_disc = 0;
    dev->connected = false;
#endif /* MODULE_ESP_WIFI_AP */

    netdev_register(&dev->netdev, NETDEV_ESP_WIFI, 0);

#if (defined(MCU_ESP32) && defined(MODULE_ESP_WIFI_CSI))
    esp_wifi_set_csi_config(&wifi_config_csi);
    esp_wifi_set_csi_rx_cb(esp_wifi_csi_rx_cb, NULL);
    esp_wifi_set_csi(true);
    esp_wifi_set_promiscuous(true);
#endif /* (defined(MCU_ESP32) && defined(MODULE_ESP_WIFI_CSI)) */
}

#endif /* MODULE_ESP_WIFI */
/**@}*/
