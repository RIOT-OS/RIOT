/*
 * Copyright (C) 2018 Gunar Schorcht
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

#include "log.h"
#include "tools.h"

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>

#include "net/ethernet.h"
#include "net/ipv4/addr.h"
#include "net/gnrc/netif/ethernet.h"
#include "net/netdev/eth.h"
#include "od.h"
#include "xtimer.h"

#include "common.h"
#include "espressif/c_types.h"
#include "espnow.h"
#include "esp/common_macros.h"
#include "irq_arch.h"
#include "sdk/sdk.h"

#include "lwip/igmp.h"
#include "lwip/udp.h"

#include "esp_wifi_params.h"
#include "esp_wifi_netdev.h"

#define ENABLE_DEBUG                (0)
#include "debug.h"

#define ESP_WIFI_DEBUG(f, ...) \
        DEBUG("[esp_wifi] %s: " f "\n", __func__, ## __VA_ARGS__)

#define ESP_WIFI_LOG_INFO(f, ...) \
        LOG_INFO("[esp_wifi] " f "\n", ## __VA_ARGS__)

#define ESP_WIFI_LOG_ERROR(f, ...) \
        LOG_ERROR("[esp_wifi] " f "\n", ## __VA_ARGS__)

#define ESP_WIFI_STATION_MODE       (STATION_MODE)
#define ESP_WIFI_AP_MODE            (SOFTAP_MODE)
#define ESP_WIFI_STATION_AP_MODE    (STATIONAP_MODE)
#define ESP_WIFI_MODE               (STATIONAP_MODE)

#define ESP_WIFI_STATION_IF         (STATION_IF)
#define ESP_WIFI_SOFTAP_IF          (SOFTAP_IF)

#define ESP_WIFI_RECONNECT_TIME     (20 * US_PER_SEC)
#define ESP_WIFI_HEAP_MARGIN        (2 * ETHERNET_MAX_LEN)

#define MAC_STR                     "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_STR_ARG(m)              m[0], m[1], m[2], m[3], m[4], m[5]

#define PBUF_IEEE80211_HLEN         (36)

/** Timer used to reconnect automatically after 20 seconds if not connected */
static xtimer_t _esp_wifi_reconnect_timer;

/**
 * There is only one ESP WIFI device. We define it as static device variable
 * to have accesss to the device inside ESP WIFI interrupt routines which do
 * not provide an argument that could be used as pointer to the ESP WIFI
 * device which triggers the interrupt.
 */
static esp_wifi_netdev_t _esp_wifi_dev;

/** forward declaration of the driver functions structure */
static const netdev_driver_t _esp_wifi_driver;

/** Stack for the netif thread */
static char _esp_wifi_stack[ESP_WIFI_STACKSIZE];

/** Static station configuration used for the WiFi interface */
static const struct station_config station_cfg = {
        .bssid_set = 0,             /* no check of MAC address of AP */
        .ssid = ESP_WIFI_SSID,
        .password = ESP_WIFI_PASS,
};

#ifndef MODULE_ESP_NOW
/**
 * Static const configuration for the SoftAP which is used to configure the
 * SoftAP interface if ESP-NOW is not enabled.
 *
 * Since we need to use the WiFi interface in SoftAP + Station mode for
 * stability reasons, although in fact only the station interface is required,
 * we make the SoftAP interface invisible and unusable. This configuration
 *
 * - uses the same hidden SSID that the station interface uses to
 *   connect to the AP,
 * - uses the same channel that the station interface uses to connect to the AP,
 * - defines a very long beacon interval
 * - doesn't allow any connection.
 */
static const struct softap_config softap_cfg = {
        .ssid = ESP_WIFI_SSID,
        .ssid_len = sizeof(ESP_WIFI_SSID) / sizeof(ESP_WIFI_SSID[0]),
        .ssid_hidden = 1,               /* don't make the AP visible */
        .password = ESP_WIFI_PASS,
        .authmode = AUTH_WPA2_PSK,
        .max_connection = 0,            /* don't allow connections */
        .beacon_interval = 60000,       /* send beacon only every 60 s */
};
#endif

extern struct netif * eagle_lwip_getif(uint8 index);

/** guard variable to avoid reentrance to _send */
static bool _in_send = false;

/** guard variable to avoid reentrance to _esp_wifi_recv_cb */
static bool _in_esp_wifi_recv_cb = false;

/**
 * @brief   Reconnect function called back by the reconnect timer
 */
static void IRAM _esp_wifi_reconnect_timer_cb(void* arg)
{
    DEBUG("%s\n", __func__);

    esp_wifi_netdev_t* dev = (esp_wifi_netdev_t*)arg;

    if (dev->state == ESP_WIFI_DISCONNECTED ||
        dev->state == ESP_WIFI_CONNECTING) {
        ESP_WIFI_LOG_INFO("trying to reconnect to ssid " ESP_WIFI_SSID);

        wifi_station_disconnect();
        wifi_station_connect();
        dev->state = ESP_WIFI_CONNECTING;
    }

    /* set the time for next connection check */
    xtimer_set(&_esp_wifi_reconnect_timer, ESP_WIFI_RECONNECT_TIME);
}

/**
 * @brief   Callback when ethernet frame is received. Has to run in IRAM.
 */
void IRAM _esp_wifi_recv_cb(struct pbuf *pb, struct netif *netif)
{
    assert(pb != NULL);
    assert(netif != NULL);

    /*
     * Function `esp_wifi_recv_cb` is executed in the context of the `ets`
     * thread. ISRs which handle hardware interrupts from the WiFi interface
     * simply pass events to a message queue of the `ets` thread which are then
     * sequentially processed by the `ets` thread to asynchronously execute
     * callback functions such as `esp_wifi_recv_cb`.
     *
     * It should be therefore not possible to reenter function
     * `esp_wifi_recv_cb`. If it does occur inspite of that, we use a
     * guard variable to avoid inconsistencies. This can not be realized
     * by a mutex because `esp_wifi_recv_cb` would be reentered from same
     * thread context.
     */
    if (_in_esp_wifi_recv_cb) {
        pbuf_free(pb);
        return;
    }
    _in_esp_wifi_recv_cb = true;

    critical_enter();

    /* first, check packet buffer for the minimum packet size */
    if (pb->tot_len < sizeof(ethernet_hdr_t)) {
        ESP_WIFI_DEBUG("frame length is less than the size of an Ethernet"
                       "header (%u < %u)", pb->tot_len, sizeof(ethernet_hdr_t));
        pbuf_free(pb);
        _in_esp_wifi_recv_cb = false;
        critical_exit();
        return;
    }

    /* check whether the receive buffer is already holding a frame */
    if (_esp_wifi_dev.rx_len) {
        ESP_WIFI_DEBUG("buffer used, dropping incoming frame of %d bytes",
                       pb->tot_len);
        pbuf_free(pb);
        _in_esp_wifi_recv_cb = false;
        critical_exit();
        return;
    }

    /* check whether packet buffer fits into receive buffer */
    if (pb->tot_len > ETHERNET_MAX_LEN) {
        ESP_WIFI_DEBUG("frame length is greater than the maximum size of an "
                       "Ethernet frame (%u > %u)", pb->tot_len, ETHERNET_MAX_LEN);
        pbuf_free(pb);
        _in_esp_wifi_recv_cb = false;
        critical_exit();
        return;
    }

    /* we have to store the frame in the buffer and free lwIP pbuf immediatly */
    _esp_wifi_dev.rx_len = pb->tot_len;
    pbuf_copy_partial(pb, _esp_wifi_dev.rx_buf, _esp_wifi_dev.rx_len, 0);
    pbuf_free(pb);

    /*
     * Since _esp_wifi_recv_cb is not executed in interrupt context but in
     * the context of the `ets` thread, it is not necessary to pass the
     * `NETDEV_EVENT_ISR` event first. Instead, the receive function can be
     * called directly which result in much faster handling, a less frame lost
     * rate and more robustness. There is no need for a mutex anymore to
     * synchronize the access to the receive buffer between _esp_wifi_recv_cb
     * and _recv function.
     */
    if (_esp_wifi_dev.netdev.event_callback) {
        _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev,
                                            NETDEV_EVENT_RX_COMPLETE);
    }

    _in_esp_wifi_recv_cb = false;
    critical_exit();
}

#define BEACON_TIMEOUT      (200)
#define HANDSHAKE_TIMEOUT   (204)

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

/**
 * @brief   Event handler for esp system events.
 */
static void _esp_wifi_handle_event_cb(System_Event_t *evt)
{
    ESP_WIFI_DEBUG("event %d", evt->event);

    uint8_t reason;
    const char* reason_str = "UNKNOWN";

    switch (evt->event) {
        case EVENT_STAMODE_CONNECTED:
            ESP_WIFI_LOG_INFO("connected to ssid %s, channel %d",
                              evt->event_info.connected.ssid,
                              evt->event_info.connected.channel);
            _esp_wifi_dev.state = ESP_WIFI_CONNECTED;
            _esp_wifi_dev.event = EVENT_STAMODE_CONNECTED;
            _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev, NETDEV_EVENT_ISR);
            break;

        case EVENT_STAMODE_DISCONNECTED:
            reason = evt->event_info.disconnected.reason;
            if (reason < REASON_BEACON_TIMEOUT) {
                reason_str = _esp_wifi_disc_reasons[reason];
            }
            else if (reason <= REASON_HANDSHAKE_TIMEOUT) {
                reason_str = _esp_wifi_disc_reasons[reason - REASON_BEACON_TIMEOUT];
            }
            ESP_WIFI_LOG_INFO("disconnected from ssid %s, reason %d (%s)",
                              evt->event_info.disconnected.ssid,
                              evt->event_info.disconnected.reason, reason_str);
            _esp_wifi_dev.state = ESP_WIFI_DISCONNECTED;
            _esp_wifi_dev.event = EVENT_STAMODE_DISCONNECTED;
            _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev, NETDEV_EVENT_ISR);
            break;

        case EVENT_SOFTAPMODE_STACONNECTED:
            ESP_WIFI_LOG_INFO("station " MACSTR " join, aid %d",
                              MAC2STR(evt->event_info.sta_connected.mac),
                              evt->event_info.sta_connected.aid);
            break;

        default:
            break;
    }
}

static int _init(netdev_t *netdev)
{
    ESP_WIFI_DEBUG("%p", netdev);

    return 0;
}

#if ENABLE_DEBUG
/** buffer for sent packet dump */
uint8_t _send_pkt_buf[ETHERNET_MAX_LEN];
#endif

/** function used to send an ethernet frame over WiFi */
extern err_t ieee80211_output_pbuf(struct netif *netif, struct pbuf *p);

/** function to get free heap */
unsigned int IRAM get_free_heap_size (void);

static int IRAM _send(netdev_t *netdev, const iolist_t *iolist)
{
    ESP_WIFI_DEBUG("%p %p", netdev, iolist);

    assert(netdev != NULL);
    assert(iolist != NULL);

    if (_in_send) {
        return 0;
    }
    _in_send = true;

    esp_wifi_netdev_t *dev = (esp_wifi_netdev_t*)netdev;

    critical_enter();
    if (dev->state != ESP_WIFI_CONNECTED) {
        ESP_WIFI_DEBUG("WiFi is still not connected to AP, cannot send");
        _in_send = false;
        critical_exit();
        return -EIO;
    }

    if (wifi_get_opmode() != ESP_WIFI_MODE) {
        ESP_WIFI_DEBUG("WiFi is not in correct mode, cannot send");
        _in_send = false;
        critical_exit();
        return -EIO;
    }

    const iolist_t *iol = iolist;
    size_t iol_len = 0;

    /* determine the frame size */
    while (iol) {
        iol_len += iol->iol_len;
        iol = iol->iol_next;
    }

    /* limit checks */
    if (iol_len > ETHERNET_MAX_LEN) {
        ESP_WIFI_DEBUG("frame length exceeds the maximum (%u > %u)",
                       iol_len, ETHERNET_MAX_LEN);
        _in_send = false;
        critical_exit();
        return -EBADMSG;
    }

    if (iol_len < sizeof(ethernet_hdr_t)) {
        ESP_WIFI_DEBUG("frame length is less than the size of an Ethernet"
                       "header (%u < %u)", iol_len, sizeof(ethernet_hdr_t));
        _in_send = false;
        critical_exit();
        return -EBADMSG;
    }

    struct netif *sta_netif = (struct netif *)eagle_lwip_getif(ESP_WIFI_STATION_IF);
    netif_set_default(sta_netif);

    struct pbuf *pb;

    if (get_free_heap_size() < ESP_WIFI_HEAP_MARGIN ||
        (pb = pbuf_alloc(PBUF_LINK, iol_len, PBUF_RAM)) == NULL ||
        (pb->tot_len < iol_len)) {
        ESP_WIFI_LOG_ERROR("could not allocate buffer to send %d bytes ", iol_len);
        /*
         * The memory of EPS8266 is quite small. Therefore, it may happen on
         * heavy network load that we run into out of memory and we have
         * to wait until lwIP pbuf has been flushed. We slow down sending a bit.
         */
        critical_exit();
        /* wait 20 ms */
        xtimer_usleep(20 * US_PER_MS);

        _in_send = false;
        return -EIO;
    }

    struct pbuf *pbi = pb;
    uint8_t *pbi_payload = pb->payload;
    size_t pbi_pos = 0;

    /* prepare lwIP packet buffer direct from iolist without any buffer */
    for (const iolist_t *iol = iolist; iol && pbi; iol = iol->iol_next) {
        uint8_t *iol_base = iol->iol_base;
        for (unsigned i = 0; i < iol->iol_len && pbi; i++) {
            pbi_payload[pbi_pos++] = iol_base[i];
            if (pbi_pos >= pbi->len) {
                pbi = pbi->next;
            }
        }
    }

#if ENABLE_DEBUG
    pbi = pb;
    pbi_pos = 0;

    for (; pbi; pbi = pbi->next) {
        memcpy(_send_pkt_buf + pbi_pos, pbi->payload, pbi->len);
        pbi_pos += pbi->len;
    }

    const ethernet_hdr_t* hdr = (const ethernet_hdr_t *)_send_pkt_buf;

    ESP_WIFI_DEBUG("send %u byte to " MAC_STR,
                   (unsigned)iol_len, MAC_STR_ARG(hdr->dst));
#if MODULE_OD
    od_hex_dump(_send_pkt_buf, iol_len, OD_WIDTH_DEFAULT);
#endif /* MODULE_OD */
#endif /* ENABLE_DEBUG */

    critical_exit();
    /* sta_netif->linkoutput = ieee80211_output_pbuf */
    err_t res = sta_netif->linkoutput(sta_netif, pb);
    pbuf_free(pb);

    if (res == ERR_OK) {
        /* There was no ieee80211_output_pbuf error and no send timeout. */
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
        _in_send = false;
        return iol_len;
    }
    else {
        /* There was either a ieee80211_output_pbuf error or send timed out. */
        _in_send = false;
        return -EIO;
    }
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    ESP_WIFI_DEBUG("%p %p %u %p", netdev, buf, len, info);

    assert(netdev != NULL);

    esp_wifi_netdev_t* dev = (esp_wifi_netdev_t*)netdev;

    uint16_t size = dev->rx_len ? dev->rx_len : 0;

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
        ESP_WIFI_DEBUG("not enough space in receive buffer");
        /* newest API requires to drop the frame in that case */
        dev->rx_len = 0;
        return -ENOBUFS;
    }

    /* copy the buffer and free */
    memcpy(buf, dev->rx_buf, dev->rx_len);
    dev->rx_len = 0;

#if ENABLE_DEBUG
    ethernet_hdr_t *hdr = (ethernet_hdr_t *)buf;

    ESP_WIFI_DEBUG("received %u byte from addr " MAC_STR,
                   size, MAC_STR_ARG(hdr->src));
#if MODULE_OD
    od_hex_dump(buf, size, OD_WIDTH_DEFAULT);
#endif /* MODULE_OD */
#endif /* ENABLE_DEBUG */

    return size;
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    ESP_WIFI_DEBUG("%s %p %p %u", netopt2str(opt), netdev, val, max_len);

    assert(netdev != NULL);
    assert(val != NULL);

    esp_wifi_netdev_t *dev = (esp_wifi_netdev_t*)netdev;

    switch (opt) {

        case NETOPT_IS_WIRED:
            return -ENOTSUP;

        case NETOPT_LINK_CONNECTED:
            assert(max_len == 1);
            if (dev->state == ESP_WIFI_CONNECTED) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            return 1;

        case NETOPT_ADDRESS:
            assert(max_len >= sizeof(dev->mac));
            memcpy(val, dev->mac, sizeof(dev->mac));
            return sizeof(dev->mac);

        default:
            return netdev_eth_get(netdev, opt, val, max_len);

    }
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t max_len)
{
    ESP_WIFI_DEBUG("%s %p %p %u", netopt2str(opt), netdev, val, max_len);

    assert(netdev != NULL);
    assert(val != NULL);

    esp_wifi_netdev_t *dev = (esp_wifi_netdev_t *) netdev;

    switch (opt) {

        case NETOPT_ADDRESS:
            assert(max_len >= sizeof(dev->mac));
            memcpy(dev->mac, val, sizeof(dev->mac));
            return sizeof(dev->mac);

        default:
            return netdev_eth_set(netdev, opt, val, max_len);
    }
}

static void _isr(netdev_t *netdev)
{
    ESP_WIFI_DEBUG("%p", netdev);

    assert(netdev != NULL);

    esp_wifi_netdev_t *dev = (esp_wifi_netdev_t *) netdev;

    switch (dev->event) {
        case EVENT_STAMODE_CONNECTED:
            dev->netdev.event_callback(netdev, NETDEV_EVENT_LINK_UP);
            break;
        case EVENT_STAMODE_DISCONNECTED:
            dev->netdev.event_callback(netdev, NETDEV_EVENT_LINK_DOWN);
            break;
        default:
            break;
    }
    _esp_wifi_dev.event = EVENT_MAX; /* no event */

    return;
}

/** override lwIP ethernet_intput to get ethernet frames */
extern err_t __real_ethernet_input(struct pbuf *pb, struct netif* netif);

err_t __wrap_ethernet_input(struct pbuf *pb, struct netif* netif)
{
    ESP_WIFI_DEBUG("%p %p", pb, netif);
    if (_esp_wifi_dev.state == ESP_WIFI_CONNECTED) {
        _esp_wifi_recv_cb(pb, netif);
    }
    else {
        __real_ethernet_input(pb, netif);
    }
    return ERR_OK;
}

static const netdev_driver_t _esp_wifi_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

static void _esp_wifi_setup(void)
{
    esp_wifi_netdev_t* dev = &_esp_wifi_dev;

    ESP_WIFI_DEBUG("%p", dev);

    if (dev->netdev.driver) {
        ESP_WIFI_DEBUG("early returning previously initialized device");
        return;
    }

    /* initialize netdev data structure */
    dev->rx_len = 0;
    dev->state = ESP_WIFI_DISCONNECTED;
    dev->event = EVENT_MAX;

    /* set the netdev driver */
    dev->netdev.driver = &_esp_wifi_driver;

#ifndef MODULE_ESP_NOW
    /* set the WiFi interface mode */
    if (!wifi_set_opmode_current(ESP_WIFI_MODE)) {
        ESP_WIFI_LOG_ERROR("could not set WiFi working mode");
        return;
    }

    /* set the WiFi SoftAP configuration */
    if (!wifi_softap_set_config_current((struct softap_config *)&softap_cfg)) {
        ESP_WIFI_LOG_ERROR("could not set WiFi configuration");
        return;
    }
#endif

    /* set the WiFi station configuration */
    if (!wifi_station_set_config_current((struct station_config *)&station_cfg)) {
        ESP_WIFI_LOG_ERROR("could not set WiFi configuration");
        return;
    }

    /* get station mac address and store it in device address */
    if (!wifi_get_macaddr(ESP_WIFI_STATION_IF, dev->mac)) {
        ESP_WIFI_LOG_ERROR("could not get MAC address of WiFi interface");
        return;
    }
    ESP_WIFI_DEBUG("own MAC addr is " MAC_STR, MAC_STR_ARG(dev->mac));

    /* set auto reconnect policy */
    wifi_station_set_reconnect_policy(true);
    wifi_station_set_auto_connect(true);

    /* register callbacks */
    wifi_set_event_handler_cb(_esp_wifi_handle_event_cb);

    /* reconnect timer initialization */
    _esp_wifi_reconnect_timer.callback = &_esp_wifi_reconnect_timer_cb;
    _esp_wifi_reconnect_timer.arg = dev;

    /* set the the reconnect timer */
    xtimer_set(&_esp_wifi_reconnect_timer, ESP_WIFI_RECONNECT_TIME);

    /* avoid the WiFi modem going into sleep mode */
    wifi_set_sleep_type(NONE_SLEEP_T);

    /* connect */
    wifi_station_connect();
    _esp_wifi_dev.state = ESP_WIFI_CONNECTING;

    return;
}

void auto_init_esp_wifi(void)
{
    ESP_WIFI_DEBUG("auto initializing netdev\n");

    /* setup netdev device */
    _esp_wifi_setup();

    /* create netif */
    gnrc_netif_ethernet_create(_esp_wifi_stack, ESP_WIFI_STACKSIZE,
                               ESP_WIFI_PRIO, "esp_wifi",
                               (netdev_t *)&_esp_wifi_dev);
}

/** @} */
