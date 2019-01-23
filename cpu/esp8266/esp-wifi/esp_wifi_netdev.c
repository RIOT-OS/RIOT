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

#define ESP_WIFI_STATION_IF         (STATION_IF)
#define ESP_WIFI_SOFTAP_IF          (SOFTAP_IF)

#define MAC_STR                     "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_STR_ARG(m)              m[0], m[1], m[2], m[3], m[4], m[5]
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

extern struct netif * eagle_lwip_getif(uint8 index);

/** guard variable to avoid reentrance to _send */
static bool _in_send = false;

/** guard variable to avoid reentrance to _esp_wifi_recv_cb */
static bool _in_esp_wifi_recv_cb = false;

/**
 * @brief   Callback when ethernet frame is received. Has to run in IRAM.
 */
void _esp_wifi_recv_cb(struct pbuf *pb, struct netif *netif)
{
    assert(pb != NULL);
    assert(netif != NULL);

    /*
     * The function `esp_wifi_recv_cb` is executed in the context of the `ets`
     * thread. The ISRs handling the hardware interrupts from the WiFi
     * interface pass events to a message queue of the `ets` thread which is
     * sequentially processed by the `ets` thread to asynchronously execute
     * callback functions such as `esp_wifi_recv_cb`.
     *
     * It should be therefore not possible to reenter function
     * `esp_wifi_recv_cb`. If it does occur inspite of that, we use a
     * guard variable to avoid inconsistencies. This can not be realized
     * by a mutex because `esp_wifi_recv_cb` would be reentered from same
     * thread context.
     */
    if (_in_esp_wifi_recv_cb || _in_send) {
        pbuf_free(pb);
        return;
    }
    _in_esp_wifi_recv_cb = true;

    /* avoid concurrent access to the receive buffer */
    mutex_lock(&_esp_wifi_dev.dev_lock);

    critical_enter();

    /* first, check packet buffer for the minimum packet size */
    if (pb->len < sizeof(ethernet_hdr_t)) {
        ESP_WIFI_DEBUG("frame length is less than the size of an Ethernet"
                       "header (%u < %u)", pb->len, sizeof(ethernet_hdr_t));
        pbuf_free(pb);
        _in_esp_wifi_recv_cb = false;
        critical_exit();
        mutex_unlock(&_esp_wifi_dev.dev_lock);
        return;
    }

    /* check whether the receive buffer is already holding a frame */
    if (_esp_wifi_dev.rx_len) {
        ESP_WIFI_DEBUG("buffer used, dropping incoming frame of %d bytes",
                       pb->tot_len);
        pbuf_free(pb);
        _in_esp_wifi_recv_cb = false;
        critical_exit();
        mutex_unlock(&_esp_wifi_dev.dev_lock);
        return;
    }

    /* check whether packet buffer fits into receive buffer */
    if (pb->len > ETHERNET_MAX_LEN) {
        ESP_WIFI_DEBUG("frame length is greater than the maximum size of an "
                       "Ethernet frame (%u > %u)", pb->len, ETHERNET_MAX_LEN);
        pbuf_free(pb);
        _in_esp_wifi_recv_cb = false;
        critical_exit();
        mutex_unlock(&_esp_wifi_dev.dev_lock);
        return;
    }

    /* store the frame in the buffer and free lwIP pbuf */
    _esp_wifi_dev.rx_len = pb->tot_len;
    pbuf_copy_partial(pb, _esp_wifi_dev.rx_buf, _esp_wifi_dev.rx_len, 0);
    pbuf_free(pb);

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
    if (_esp_wifi_dev.netdev.event_callback) {
        _esp_wifi_dev.netdev.event_callback(&_esp_wifi_dev.netdev,
                                            NETDEV_EVENT_ISR);
    }

    /* reset IRQ nesting counter */
    irq_interrupt_nesting--;

    _in_esp_wifi_recv_cb = false;
    critical_exit();
    mutex_unlock(&_esp_wifi_dev.dev_lock);
}

/**
 * @brief   Event handler for esp system events.
 */
static void _esp_wifi_handle_event_cb(System_Event_t *evt)
{
    ESP_WIFI_DEBUG("event %d", evt->event);

    switch (evt->event) {
        case EVENT_STAMODE_CONNECTED:
            ESP_WIFI_LOG_INFO("connected to ssid %s, channel %d",
                              evt->event_info.connected.ssid,
                              evt->event_info.connected.channel);
            _esp_wifi_dev.state = ESP_WIFI_CONNECTED;
            break;

        case EVENT_STAMODE_DISCONNECTED:
            ESP_WIFI_LOG_INFO("disconnected from ssid %s, reason %d",
                              evt->event_info.disconnected.ssid,
                              evt->event_info.disconnected.reason);
            _esp_wifi_dev.state = ESP_WIFI_DISCONNECTED;

            /* call disconnect to reset internal state */
            if (evt->event_info.disconnected.reason != REASON_ASSOC_LEAVE) {
                wifi_station_disconnect();
            }

            /* try to reconnect */
            wifi_station_connect();
            _esp_wifi_dev.state = ESP_WIFI_CONNECTING;

            break;

        default:
            break;
    }
}

static int _init(netdev_t *netdev)
{
    ESP_WIFI_DEBUG("%p", netdev);

#ifdef MODULE_NETSTATS_L2
    memset(&netdev->stats, 0x00, sizeof(netstats_t));
#endif

    return 0;
}

#if ENABLE_DEBUG
/** buffer for sent packet dump */
uint8_t _send_pkt_buf[ETHERNET_MAX_LEN];
#endif

/** function used to send an ethernet frame over WiFi */
extern err_t ieee80211_output_pbuf(struct netif *netif, struct pbuf *p);

/** guard variable to avoid reentrance to _send */
static int _send(netdev_t *netdev, const iolist_t *iolist)
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

    if (wifi_get_opmode() != ESP_WIFI_STATION_MODE) {
        ESP_WIFI_DEBUG("WiFi is not in station mode, cannot send");
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

    struct pbuf *pb = pbuf_alloc(PBUF_LINK, iol_len, PBUF_RAM);
    if (pb == NULL || pb->tot_len < iol_len) {
        ESP_WIFI_DEBUG("could not allocate buffer to send %d bytes ", iol_len);
        /*
         * The memory of EPS8266 is quite small. Therefore, it may happen on
         * heavy network load that we run into out of memory and we have
         * to wait until lwIP pbuf has been flushed. For that purpose, we
         * have to disconnect from AP and slow down sending. The node will
         * then reconnect to AP automatically.
         */
        critical_exit();
        /* disconnect from AP */
        wifi_station_disconnect();
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

    int res = ieee80211_output_pbuf(sta_netif, pb);
    pbuf_free(pb);

    if (res == ERR_OK) {
#ifdef MODULE_NETSTATS_L2
        netdev->stats.tx_bytes += iol_len;
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
#endif
        _in_send = false;
        critical_exit();
        return iol_len;
    }
    else {
#ifdef MODULE_NETSTATS_L2
        netdev->stats.tx_failed++;
#endif
        _in_send = false;
        critical_exit();
        /*
         * This error usually happens because we run into out of memory. We have
         * to wait until lwIP pbuf has been flushed. For that purpose, we
         * have to disconnect from AP and wait for a short time. The node will
         * then reconnect to AP automatically.
         */
        wifi_station_disconnect();
        return -EIO;
    }
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    ESP_WIFI_DEBUG("%p %p %u %p", netdev, buf, len, info);

    assert(netdev != NULL);

    esp_wifi_netdev_t* dev = (esp_wifi_netdev_t*)netdev;

    /* avoid concurrent access to the receive buffer */
    mutex_lock(&dev->dev_lock);

    uint16_t size = dev->rx_len ? dev->rx_len : 0;

    if (!buf) {
        /* get the size of the frame */
        if (len > 0 && size) {
            /* if len > 0, drop the frame */
            dev->rx_len = 0;
        }
        mutex_unlock(&dev->dev_lock);
        return size;
    }

    if (len < size) {
        /* buffer is smaller than the number of received bytes */
        ESP_WIFI_DEBUG("not enough space in receive buffer");
        /* newest API requires to drop the frame in that case */
        dev->rx_len = 0;
        mutex_unlock(&dev->dev_lock);
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

#if MODULE_NETSTATS_L2
    netdev->stats.rx_count++;
    netdev->stats.rx_bytes += size;
#endif

    mutex_unlock(&dev->dev_lock);
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

    esp_wifi_netdev_t *dev = (esp_wifi_netdev_t *)netdev;

    if (dev->rx_len) {
        dev->netdev.event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
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

    mutex_init(&dev->dev_lock);

    /* set the netdev driver */
    dev->netdev.driver = &_esp_wifi_driver;

    /* set the WiFi interface to Station mode without DHCP */
    if (!wifi_set_opmode_current(ESP_WIFI_STATION_MODE)) {
        ESP_WIFI_LOG_ERROR("could not set WiFi working mode");
        return;
    }

    /* set the WiFi configuration */
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

    /* register callbacks */
    wifi_set_event_handler_cb(_esp_wifi_handle_event_cb);

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
