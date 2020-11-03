/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_atwinc15x0
 * @{
 *
 * @file
 * @brief       Netdev driver for the ATWINC15x0 WiFi module
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <string.h>

#define ETH_MODE            (1)

#include "atwinc15x0_internal.h"
#include "atwinc15x0_params.h"

#include "bus_wrapper/include/nm_bus_wrapper.h"
#include "driver/source/m2m_hif.h"
#include "driver/include/m2m_wifi.h"

#include "assert.h"
#include "log.h"
#include "net/netdev/eth.h"
#include "od.h"
#include "xtimer.h"

#define ENABLE_DEBUG        0
#define ENABLE_DEBUG_DUMP   0
#include "debug.h"

#define ATWINC15X0_MAC_STR          "%02x:%02x:%02x:%02x:%02x:%02x"
#define ATWINC15X0_MAC_STR_ARG(m)   m[0], m[1], m[2], m[3], m[4], m[5]

#define ATWINC15X0_WAIT_TIME        (1 * US_PER_MS)
#define ATWINC15X0_WAIT_TIMEOUT     (20)
#define ATWINC15X0_WAIT_RECONNECT   (5 * US_PER_SEC)

/* Forward function declarations */
static void _atwinc15x0_wifi_cb(uint8_t event, void *msg);
static void _atwinc15x0_eth_cb(uint8_t type, void *msg, void *ctrl);
static int _atwinc15x0_connect(void);

/**
 * The following buffer is required by the ATWINC15x0 vendor driver to store
 * packets received from the ATWINC15x0 WiFI module in it. Its size has to be
 * at least one Ethernet frame of maximum length.
 *
 * The event-driven handling of incoming packets is strictly sequential in
 * the context of the `netif` thread. This means that an incoming packet
 * is first received by the `netif` thread and copied to its packet buffer
 * before the next event of an incoming packet is handled by the ATWINC15x0
 * vendor driver. It can therefore be assumed that only one received packet
 * can be in the buffer at a time. No further separate intermediate buffer
 * is required.
 *
 * Furthermore, this buffer can be used for preparing a packet to be sent,
 * since it can be assumed that receiving and sending packets are implicitly
 * mutually exclusive due to their strictly sequential processing.
 */
static uint8_t atwinc15x0_eth_buf[ETHERNET_MAX_LEN];

/* ATWINC15x0 vendor driver initialization structure (can't be const) */
static tstrWifiInitParam atwinc15x0_wifi_params = {
    .pfAppWifiCb = _atwinc15x0_wifi_cb,
    .strEthInitParam = {
        .pfAppWifiCb = _atwinc15x0_wifi_cb,
        .pfAppEthCb = _atwinc15x0_eth_cb,
        .au8ethRcvBuf = atwinc15x0_eth_buf,
        .u16ethRcvBufSize = ARRAY_SIZE(atwinc15x0_eth_buf),
        .u8EthernetEnable = M2M_WIFI_MODE_ETHERNET,
    },
};

/**
 * Reference to the single ATWINC15x0 device instance
 *
 * Since the vendor ATWINC15x0 host driver uses many global variables, only
 * a single ATWINC15x0 device can be used. Therefore, the RIOT driver only
 * supports a single instance of an ATWINC15x0 device. The reference is
 * needed in callback functions where a reference to the device is not
 * available.
 */
atwinc15x0_t *atwinc15x0 = NULL;

static void _atwinc15x0_eth_cb(uint8_t type, void *msg, void *ctrl_buf)
{
    assert(atwinc15x0);
    assert(msg != NULL);
    assert(ctrl_buf != NULL);

    tstrM2mIpCtrlBuf *ctrl = (tstrM2mIpCtrlBuf *)ctrl_buf;

    DEBUG("%s type=%u msg=%p len=%d remaining=%d\n", __func__,
          type, msg, ctrl->u16DataSize, ctrl->u16RemainigDataSize);

    if (IS_ACTIVE(ENABLE_DEBUG) && IS_USED(MODULE_OD)) {
        od_hex_dump(msg, ctrl->u16DataSize, 16);
    }

    /* the buffer shouldn't be used here */
    assert(atwinc15x0->rx_buf == NULL);

    uint32_t state = irq_disable();

    atwinc15x0->rx_buf = msg;
    atwinc15x0->rx_len = ctrl->u16DataSize;

    irq_restore(state);

    /**
     * This function is executed in the thread context. Therefore
     * netdev.event_callback can be called directly, which avoids an
     * additional intermediate buffer.
     */
    atwinc15x0->netdev.event_callback(&atwinc15x0->netdev,
                                      NETDEV_EVENT_RX_COMPLETE);
}

typedef union {
    tstrM2mWifiStateChanged state_changed;
    tstrM2MConnInfo conn_info;
    tstrM2mScanDone scan_done;
    tstrM2mWifiscanResult scan_result;
    int8_t rssi;
} atwinc15x0_event_t;

static bool _rssi_info_ready = false;

static void _atwinc15x0_wifi_cb(uint8_t type, void *msg)
{
    /**
     * This function is executed in thread context. There is no need to call
     * netdev_trigger_event_isr and to handle the events in _atwinc15x0_isr
     */

    DEBUG("%s %u %p\n", __func__, type, msg);

    atwinc15x0_event_t* event = (atwinc15x0_event_t *)msg;

    switch (type) {
        case M2M_WIFI_RESP_SCAN_DONE:
            DEBUG("%s scan done, %d APs found\n", __func__,
                  event->scan_done.u8NumofCh);
            /* read the first scan result record */
            m2m_wifi_req_scan_result(0);
            break;

        case M2M_WIFI_RESP_SCAN_RESULT:
            LOG_DEBUG("[atwinc15x0] %s: rssi %d, auth %d, ch %d, bssid "
                      ATWINC15X0_MAC_STR "\n",
                      event->scan_result.au8SSID,
                      event->scan_result.s8rssi,
                      event->scan_result.u8AuthType,
                      event->scan_result.u8ch,
                      ATWINC15X0_MAC_STR_ARG(event->scan_result.au8BSSID));

            if (memcmp(&event->scan_result.au8BSSID,
                       &atwinc15x0->ap, ETHERNET_ADDR_LEN) == 0) {
                /* use the results for current AP to set the current channel */
                atwinc15x0->channel = event->scan_result.u8ch;
            }
            if (event->scan_result.u8index < m2m_wifi_get_num_ap_found()) {
                /* read the next scan result record */
                m2m_wifi_req_scan_result(event->scan_result.u8index + 1);
            }
            break;

        case M2M_WIFI_RESP_CON_STATE_CHANGED:
            switch (event->state_changed.u8CurrState) {
                case M2M_WIFI_DISCONNECTED:
                    LOG_INFO("[atwinc15x0] WiFi disconnected\n");
                    atwinc15x0->connected = false;
                    atwinc15x0->netdev.event_callback(&atwinc15x0->netdev,
                                                      NETDEV_EVENT_LINK_DOWN);
                    /* wait and try to reconnect */
                    xtimer_usleep(ATWINC15X0_WAIT_RECONNECT);
                    _atwinc15x0_connect();
                    break;
                case M2M_WIFI_CONNECTED:
                    LOG_INFO("[atwinc15x0] WiFi connected\n");
                    atwinc15x0->connected = true;
                    atwinc15x0->netdev.event_callback(&atwinc15x0->netdev,
                                                      NETDEV_EVENT_LINK_UP);
                    /* get information about the current AP */
                    m2m_wifi_get_connection_info();
                    /* start a scan for additional info, e.g. used channel */
                    m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
                    break;
                default:
                    break;
            }
            break;

        case M2M_WIFI_RESP_CONN_INFO:
            DEBUG("%s conn info %s, rssi %d, sec %u, bssid "
                  ATWINC15X0_MAC_STR "\n", __func__,
                  event->conn_info.acSSID,
                  event->conn_info.s8RSSI,
                  event->conn_info.u8SecType,
                  ATWINC15X0_MAC_STR_ARG(event->conn_info.au8MACAddress));

            /* set the RSSI and BSSID of the current AP */
            atwinc15x0->rssi = event->conn_info.s8RSSI;
            memcpy(atwinc15x0->ap,
                   event->conn_info.au8MACAddress, ETHERNET_ADDR_LEN);
            break;

        case M2M_WIFI_RESP_CURRENT_RSSI:
            DEBUG("%s current rssi %d\n", __func__, event->rssi);
            /* set the RSSI */
            atwinc15x0->rssi = event->rssi;
            _rssi_info_ready = true;
            break;

        default:
            break;
    }
}

static int _atwinc15x0_send(netdev_t *netdev, const iolist_t *iolist)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    assert(dev);
    assert(dev == atwinc15x0);
    assert(iolist);

    if (!dev->connected) {
        DEBUG("%s WiFi is still not connected to AP, cannot send", __func__);
        return -ENODEV;
    }

    /* atwinc15x0_eth_buf should not be used for incoming packets here */
    assert(dev->rx_buf == NULL);

    uint32_t state = irq_disable();
    uint16_t tx_len = 0;

    /* load packet data into the buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (tx_len + iol->iol_len > ETHERNET_MAX_LEN) {
            irq_restore(state);
            return -EOVERFLOW;
        }
        if (iol->iol_len) {
            memcpy (atwinc15x0_eth_buf + tx_len, iol->iol_base, iol->iol_len);
            tx_len += iol->iol_len;
        }
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        DEBUG("%s send %d byte", __func__, tx_len);
        if (IS_ACTIVE(ENABLE_DEBUG_DUMP) && IS_USED(MODULE_OD)) {
            od_hex_dump(atwinc15x0_eth_buf, tx_len, OD_WIDTH_DEFAULT);
        }
    }

    irq_restore(state);

    /* send the the packet */
    if (m2m_wifi_send_ethernet_pkt(atwinc15x0_eth_buf, tx_len) == M2M_SUCCESS) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
        return tx_len;
    }
    else {
        DEBUG("%s sending WiFi packet failed", __func__);
        return -EIO;
    }
}

static int _atwinc15x0_recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    (void)info;
    assert(dev);
    assert(dev == atwinc15x0);

    uint32_t state = irq_disable();
    uint16_t rx_size = dev->rx_len;

    if (!rx_size) {
        /* there is nothing in receive buffer */
        irq_restore(state);
        return 0;
    }

    if (!buf) {
        /* get the size of the frame */
        if (len > 0) {
            /* if len > 0, drop the frame */
            dev->rx_len = 0;
            dev->rx_buf = NULL;
        }
        irq_restore(state);
        return rx_size;
    }

    if (len < rx_size) {
        /* buffer is smaller than the number of received bytes */
        DEBUG("%s not enough space in receive buffer", __func__);
        /* newest API requires to drop the frame in that case */
        dev->rx_len = 0;
        dev->rx_buf = NULL;
        irq_restore(state);
        return -ENOBUFS;
    }

    /* remove length bytes, copy received packet to buffer */
    memcpy(buf, dev->rx_buf, dev->rx_len);
    dev->rx_len = 0;
    dev->rx_buf = NULL;

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        ethernet_hdr_t *hdr = (ethernet_hdr_t *)buf;
        DEBUG("%s received %u byte from addr " ATWINC15X0_MAC_STR "\n",
            __func__, rx_size, ATWINC15X0_MAC_STR_ARG(hdr->src));

        if (IS_ACTIVE(ENABLE_DEBUG_DUMP) && IS_USED(MODULE_OD)) {
            od_hex_dump(buf, rx_size, OD_WIDTH_DEFAULT);
        }
    }

    irq_restore(state);

    return rx_size;
}

static int _atwinc15x0_get(netdev_t *netdev, netopt_t opt, void *val,
                           size_t max_len)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    (void)max_len;
    assert(val);
    assert(dev);
    assert(dev == atwinc15x0);

    DEBUG("%s dev=%p opt=%u val=%p max_len=%u\n", __func__,
          (void *)netdev, opt, val, max_len);

    switch (opt) {
        case NETOPT_IS_WIRED:
            return -ENOTSUP;

        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
            uint8_t valid;
            m2m_wifi_get_otp_mac_address((uint8_t *)val, &valid);
            return (valid) ? ETHERNET_ADDR_LEN : 0;

        case NETOPT_LINK:
            assert(max_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)val) = (dev->connected) ? NETOPT_ENABLE
                                                         : NETOPT_DISABLE;
            return sizeof(netopt_enable_t);

        case NETOPT_CHANNEL:
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)val) = dev->channel;
            return sizeof(uint16_t);

        case NETOPT_RSSI:
            assert(max_len == sizeof(int8_t));
            _rssi_info_ready = false;
            /* trigger the request current RSSI (asynchronous function) */
            if (m2m_wifi_req_curr_rssi() != M2M_SUCCESS) {
                return 0;
            }
            /* wait for the response with a given timeout */
            unsigned int _rssi_info_time_out = ATWINC15X0_WAIT_TIMEOUT;
            while (!_rssi_info_ready && _rssi_info_time_out--) {
                xtimer_usleep(ATWINC15X0_WAIT_TIME);
            }
            /* return the RSSI */
            *((int8_t *)val) = dev->rssi;
            return sizeof(int8_t);

        default:
            return netdev_eth_get(netdev, opt, val, max_len);
    }
}

static int _atwinc15x0_set(netdev_t *netdev, netopt_t opt, const void *val,
                           size_t max_len)
{
    assert(val);

    DEBUG("%s dev=%p opt=%u val=%p max_len=%u\n", __func__,
          (void *)netdev, opt, val, max_len);

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len == ETHERNET_ADDR_LEN);
            m2m_wifi_set_mac_address((uint8_t *)val);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_set(netdev, opt, val, max_len);
    }
}

static int _atwinc15x0_init(netdev_t *netdev)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    (void)netdev;
    assert(dev);
    assert(dev == atwinc15x0);

    DEBUG("%s dev=%p\n", __func__, (void *)dev);

    atwinc15x0->bsp_isr = NULL;
    atwinc15x0->bsp_irq_enabled = true;
    atwinc15x0->connected = false;
    atwinc15x0->rx_len = 0;
    atwinc15x0->rx_buf = NULL;

    nm_bsp_init();

    int8_t res;

    /* initialize the WINC Driver*/
    if ((res = m2m_wifi_init(&atwinc15x0_wifi_params)) != M2M_SUCCESS) {
        DEBUG("m2m_wifi_init failed with code %d\n", res);
        if (res == M2M_ERR_FW_VER_MISMATCH) {
            LOG_WARNING("[atwinc15x0] Firmware version mismatch, "
                        "this may lead to problems.\n");
        }
        else {
            LOG_ERROR("[atwinc15x0] Driver initialization error %d\n", res);
            return res;
        }
    }

    /* disable the built-in DHCP client */
    if ((res = m2m_wifi_enable_dhcp(false)) != M2M_SUCCESS) {
        LOG_ERROR("[atwinc15x0] m2m_wifi_enable_dhcp failed with %d\n", res);
        return res;
    }

    /* try to connect and return */
    return _atwinc15x0_connect();
}

static int _atwinc15x0_connect(void)
{
    tuniM2MWifiAuth auth_info;
    tenuM2mSecType auth_type = M2M_WIFI_SEC_OPEN;

#if !defined(MODULE_WIFI_ENTERPRISE) && defined(WIFI_PASS)

    strncpy((char *)auth_info.au8PSK, WIFI_PASS, M2M_MAX_PSK_LEN);
    auth_type = M2M_WIFI_SEC_WPA_PSK;

#elif defined(MODULE_WIFI_ENTERPRISE)

#if defined(WIFI_USER) && defined(WIFI_PASS)
    strncpy((char *)&auth_info.strCred1x.au8UserName, WIFI_USER, M2M_1X_USR_NAME_MAX);
    strncpy((char *)&auth_info.strCred1x.au8Passwd, WIFI_PASS, M2M_1X_PWD_MAX);
    auth_type = M2M_WIFI_SEC_802_1X;
#else /* defined(WIFI_EAP_USER) && defined(WIFI_EAP_PASS) */
#error WIFI_EAP_USER and WIFI_EAP_PASS have to define the user name \
       and the password for EAP phase 2 authentication in wifi_enterprise
#endif /* defined(WIFI_EAP_USER) && defined(WIFI_EAP_PASS) */

#endif /* defined(MODULE_ESP_WIFI_ENTERPRISE) */

    /* connect */
    int8_t res;
    if ((res = m2m_wifi_connect(WIFI_SSID, sizeof(WIFI_SSID),
                                auth_type, &auth_info,
                                M2M_WIFI_CH_ALL)) != M2M_SUCCESS) {
        LOG_ERROR("[atwinc15x0] WiFi connect failed with %d\n", res);
        return res;
    }

    return 0;
}

static void _atwinc15x0_isr(netdev_t *netdev)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    assert(dev);
    assert(dev == atwinc15x0);

    DEBUG("%s dev=%p\n", __func__, (void *)dev);

    /* handle pending ATWINC15x0 module events */
    while (m2m_wifi_handle_events(NULL) != M2M_SUCCESS) { }
}

const netdev_driver_t atwinc15x0_netdev_driver = {
    .send = _atwinc15x0_send,
    .recv = _atwinc15x0_recv,
    .init = _atwinc15x0_init,
    .isr = _atwinc15x0_isr,
    .get = _atwinc15x0_get,
    .set = _atwinc15x0_set,
};

void atwinc15x0_setup(atwinc15x0_t *dev, const atwinc15x0_params_t *params)
{
    assert(dev);

    atwinc15x0 = dev;
    atwinc15x0->netdev.driver = &atwinc15x0_netdev_driver;
    atwinc15x0->params = *params;
}

void atwinc15x0_irq(void)
{
    if (atwinc15x0) {
        netdev_trigger_event_isr(&atwinc15x0->netdev);
    }
}
