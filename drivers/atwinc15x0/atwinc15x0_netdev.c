/*
 * Copyright (C) 2020 Gunar Schorcht
 *               2023 ML!PA Consulting GmbH
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
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
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

#include "thread.h"
#include "compiler_hints.h"
#include "assert.h"
#include "log.h"
#include "net/netopt.h"
#include "net/wifi.h"
#include "net/netdev/eth.h"
#include "net/netdev/wifi.h"
#include "net/wifi_scan_list.h"
#include "od.h"
#include "ztimer.h"
#include "string_utils.h"
#include "net/netif.h"

#define ENABLE_DEBUG        0
#define ENABLE_DEBUG_DUMP   0
#include "debug.h"

#define ATWINC15X0_MAC_STR          "%02x:%02x:%02x:%02x:%02x:%02x"
#define ATWINC15X0_MAC_STR_ARG(m)   m[0], m[1], m[2], m[3], m[4], m[5]

#define ATWINC15X0_WAIT_TIME_MS         (1)
#define ATWINC15X0_WAIT_TIMEOUT         (20)
#define ATWINC15X0_WAIT_RECONNECT_MS    (5000)

/**
 *  @brief   Maximum number of scan list entries to deliver
 */
#define ATWINC15X0_SCAN_LIST_NUMOF  CONFIG_ATWINC15X0_SCAN_LIST_NUMOF

/* Forward function declarations */
static void _atwinc15x0_wifi_cb(uint8_t event, void *msg);
static void _atwinc15x0_eth_cb(uint8_t type, void *msg, void *ctrl);
static int _atwinc15x0_connect(const wifi_connect_request_t *req);
static int _atwinc15x0_disconnect(const wifi_disconnect_request_t *req);
static int _atwinc15x0_scan(const wifi_scan_request_t *req);
static int _atwinc15x0_init(netdev_t *netdev);
static int _set_state(atwinc15x0_t *dev, netopt_state_t state);
static netopt_state_t _get_state(const atwinc15x0_t *dev);
static void _atwinc15x0_isr(netdev_t *netdev);

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

MAYBE_UNUSED
static struct {
    wifi_scan_list_t head;
    wifi_scan_list_node_t array[ATWINC15X0_SCAN_LIST_NUMOF];
} _atwinc15x0_scan_list;

static inline void _wifi_scan_list_empty(void)
{
#if IS_USED(MODULE_WIFI_SCAN_LIST)
    wifi_scan_list_empty(&_atwinc15x0_scan_list.head,
                         _atwinc15x0_scan_list.array,
                         ARRAY_SIZE(_atwinc15x0_scan_list.array));
#endif
}

static inline void _wifi_scan_list_insert(const wifi_scan_result_t *result)
{
    (void)result;
#if IS_USED(MODULE_WIFI_SCAN_LIST)
    wifi_scan_list_insert(&_atwinc15x0_scan_list.head,
                          _atwinc15x0_scan_list.array,
                          ARRAY_SIZE(_atwinc15x0_scan_list.array),
                          result);
#endif
}

MAYBE_UNUSED
static wifi_scan_request_t _atwinc15x0_scan_req;

static inline void _wifi_scan_result_callback(const wifi_scan_list_t *scan_list)
{
    if (_atwinc15x0_scan_req.base.scan_cb) {
        void *netif = netif_get_by_id(thread_getpid());
        ((wifi_on_scan_result_t)_atwinc15x0_scan_req.base.scan_cb)(netif,
                                                                   scan_list);
    }
    _atwinc15x0_scan_req.base.scan_cb = NULL;
}

/**
 * @brief   Internal next timeout type
 */
typedef enum {
    /**
     * @brief   No / clear timeout
     */
    ATWINC15X0_WIFI_STA_TIMEOUT_NONE = 0,
    /**
     * @brief   Timeout to reconnect to
     */
    ATWINC15X0_WIFI_STA_TIMEOUT_RECONNECT,
} atwinc15x0_wifi_sta_timeout_t;

MAYBE_UNUSED
static union {
    wifi_disconnect_request_t disconn_req;
    wifi_connect_request_t conn_req;
} _atwinc15x0_connect_req;

static inline void _wifi_connect_result_callback(const wifi_connect_result_t *result)
{
    if (_atwinc15x0_connect_req.conn_req.base.conn_cb) {
        void *netif = netif_get_by_id(thread_getpid());
        ((wifi_on_connect_result_t)_atwinc15x0_connect_req.conn_req.base.conn_cb)(netif,
                                                                                  result);
    }
    _atwinc15x0_connect_req.conn_req.base.conn_cb = NULL;
    /* _atwinc15x0_connect_req.conn_req.base.disconn_cb is called when connection is lost */
}

static inline void _wifi_disconnect_result_callback(const wifi_disconnect_result_t *result)
{
    if (_atwinc15x0_connect_req.conn_req.base.disconn_cb) {
        void *netif = netif_get_by_id(thread_getpid());
        ((wifi_on_disconnect_result_t)_atwinc15x0_connect_req.conn_req.base.disconn_cb)(netif,
                                                                                        result);
    }
    _atwinc15x0_connect_req.conn_req.base.conn_cb = NULL;
    _atwinc15x0_connect_req.conn_req.base.disconn_cb = NULL;
}

MAYBE_UNUSED
static struct {
    atwinc15x0_wifi_sta_timeout_t timeout;
    ztimer_t timer;
} _atwinc15x0_timer;

static void _atwinc15x0_reconnect_timer(void *arg)
{
    (void)arg;
    _atwinc15x0_timer.timeout = ATWINC15X0_WIFI_STA_TIMEOUT_RECONNECT;
    _atwinc15x0_timer.timer.callback = NULL;
    atwinc15x0_irq();
}

static void _atwinc15x0_set_timer(void *arg, ztimer_callback_t cb, uint32_t timeout)
{
    ztimer_remove(ZTIMER_MSEC, &_atwinc15x0_timer.timer);
    _atwinc15x0_timer.timer.arg = arg;
    _atwinc15x0_timer.timer.callback = cb;
    ztimer_set(ZTIMER_MSEC, &_atwinc15x0_timer.timer, timeout);
}

static inline void _atwinc15x0_set_reconnect_timer(void)
{
    _atwinc15x0_set_timer(NULL, _atwinc15x0_reconnect_timer, ATWINC15X0_WAIT_RECONNECT_MS);
}

static int _atwinc15x0_static_connect(void)
{
    if (!IS_USED(MODULE_ATWINC15X0_STATIC_CONNECT)) {
        return 0;
    }
    tuniM2MWifiAuth auth_info;
    tenuM2mSecType auth_type = M2M_WIFI_SEC_OPEN;
    if (_atwinc15x0_is_busy(atwinc15x0)) {
        return -EBUSY;
    }
    if (_atwinc15x0_is_connected(atwinc15x0)) {
        return -EALREADY;
    }
    if (_atwinc15x0_is_sleeping(atwinc15x0)) {
        return -ECANCELED;
    }
#if !defined(MODULE_WIFI_ENTERPRISE) && defined(WIFI_PASS)

    strncpy((char *)auth_info.au8PSK, WIFI_PASS, M2M_MAX_PSK_LEN);
    auth_type = M2M_WIFI_SEC_WPA_PSK;

#elif defined(MODULE_WIFI_ENTERPRISE)

#if defined(WIFI_USER) && defined(WIFI_PASS)
    strncpy((char *)&auth_info.strCred1x.au8UserName, WIFI_USER, M2M_1X_USR_NAME_MAX);
    strncpy((char *)&auth_info.strCred1x.au8Passwd, WIFI_PASS, M2M_1X_PWD_MAX);
    auth_type = M2M_WIFI_SEC_802_1X;
#else /* defined(WIFI_USER) && defined(WIFI_PASS) */
#error WIFI_EAP_USER and WIFI_EAP_PASS have to define the user name \
       and the password for EAP phase 2 authentication in wifi_enterprise
#endif /* defined(WIFI_USER) && defined(WIFI_PASS) */

#endif /* defined(MODULE_WIFI_ENTERPRISE) */
    /* connect */
    int8_t res;
    if ((res = m2m_wifi_connect(WIFI_SSID, sizeof(WIFI_SSID),
                                auth_type, &auth_info,
                                M2M_WIFI_CH_ALL)) != M2M_SUCCESS) {
        LOG_ERROR("[atwinc15x0] WiFi connect failed with %d\n", res);
        return -EIO;
    }
    _atwinc15x0_set_connecting(atwinc15x0);
    return 0;
}

static inline int _atwinc15x0_get_sec_mode(tenuM2mSecType mode)
{
    switch (mode) {
        case M2M_WIFI_SEC_OPEN:
            return WIFI_SECURITY_MODE_OPEN;
        case M2M_WIFI_SEC_WPA_PSK:
            return WIFI_SECURITY_MODE_WPA2_PERSONAL;
        case M2M_WIFI_SEC_WEP:
            return WIFI_SECURITY_MODE_WEP_PSK;
        case M2M_WIFI_SEC_802_1X:
            return WIFI_SECURITY_MODE_WPA2_ENTERPRISE;
        default:
            return -1;
    }
}

static void _atwinc15x0_eth_cb(uint8_t type, void *msg, void *ctrl_buf)
{
    assert(atwinc15x0);
    assert(msg != NULL);
    assert(ctrl_buf != NULL);

    tstrM2mIpCtrlBuf *ctrl = (tstrM2mIpCtrlBuf *)ctrl_buf;

    DEBUG("%s type=%u msg=%p len=%d remaining=%d\n", __func__,
          type, msg, ctrl->u16DataSize, ctrl->u16RemainigDataSize);

    if (IS_ACTIVE(ENABLE_DEBUG_DUMP) && IS_USED(MODULE_OD)) {
        od_hex_dump(msg, ctrl->u16DataSize, 16);
    }

    /* the buffer shouldn't be used here */
    assert(atwinc15x0->rx_buf == NULL);

    atwinc15x0->rx_buf = msg;
    atwinc15x0->rx_len = ctrl->u16DataSize;

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

static void _atwinc15x0_handle_resp_scan_done(const tstrM2mScanDone* scan_done)
{
    DEBUG("%s scan done, %d APs found\n", __func__, scan_done->u8NumofCh);
    if (scan_done->u8NumofCh > 0) {
        /* read the first scan result record */
        m2m_wifi_req_scan_result(0);
    }
    else {
        /* no results */
        _atwinc15x0_set_idle(atwinc15x0);
        if (IS_USED(MODULE_ATWINC15X0_DYNAMIC_SCAN)) {
            _wifi_scan_result_callback(&_atwinc15x0_scan_list.head);
        }
    }
}

static void _atwinc15x0_handle_resp_scan_result(const tstrM2mWifiscanResult* scan_result)
{
    LOG_DEBUG("[atwinc15x0] %s: rssi %d, auth %d, ch %d, bssid "
              ATWINC15X0_MAC_STR "\n",
              scan_result->au8SSID,
              scan_result->s8rssi,
              scan_result->u8AuthType,
              scan_result->u8ch,
              ATWINC15X0_MAC_STR_ARG(scan_result->au8BSSID));
    if (_atwinc15x0_is_connected(atwinc15x0)) {
        if (!memcmp(scan_result->au8BSSID, &atwinc15x0->ap, ETHERNET_ADDR_LEN)) {
            /* use the results for current AP to set the current channel */
            atwinc15x0->channel = scan_result->u8ch;
        }
    }
    if (IS_USED(MODULE_ATWINC15X0_DYNAMIC_SCAN)) {
        int sec;
        if ((sec = _atwinc15x0_get_sec_mode(scan_result->u8AuthType)) != -1) {
            wifi_scan_result_t result = WIFI_SCAN_RESULT_INITIALIZER(scan_result->u8ch,
                                                                     scan_result->s8rssi, sec);
            memcpy(result.bssid, scan_result->au8BSSID, sizeof(result.bssid));
            strncpy(result.ssid, (const char *)scan_result->au8SSID, sizeof(result.ssid) - 1);
            _wifi_scan_list_insert(&result);
        }
    }
    if (scan_result->u8index < m2m_wifi_get_num_ap_found() - 1) {
        /* read the next scan result record */
        m2m_wifi_req_scan_result(scan_result->u8index + 1);
    }
    else {
        _atwinc15x0_set_idle(atwinc15x0);
        if (IS_USED(MODULE_ATWINC15X0_DYNAMIC_SCAN)) {
            _wifi_scan_result_callback(&_atwinc15x0_scan_list.head);
        }
    }
}

static void _atwinc15x0_handle_resp_con_state_changed(const tstrM2mWifiStateChanged *state_changed)
{
    /**
     * The logic here can be tested with the following test cases:
     * 1. connect when disconnected
     * 2. connect to another AP when connected
     * 3. disconnect when connected
     * 4. go to sleep when connected
     */
    switch (state_changed->u8CurrState) {
        case M2M_WIFI_DISCONNECTED:
            LOG_INFO("[atwinc15x0] WiFi disconnected\n");
            /* We disconnect before we connect, so we will first get a disconnect event when we
               were connected. After that when connection to the new AP fails we are already in a
               disconnected state. */
            bool was_connected = _atwinc15x0_is_connected(atwinc15x0);
            bool is_connecting = _atwinc15x0_is_connecting(atwinc15x0);
            bool is_disconnecting = _atwinc15x0_is_disconnecting(atwinc15x0);
            bool is_sleeping;
            if (!(is_sleeping = _atwinc15x0_is_sleeping(atwinc15x0))) {
                /* We requested to disconnect before sleep.
                   Don´t override the sleep state when the disconnect event is received. */
                _atwinc15x0_set_disconnected(atwinc15x0);
            }
            if (was_connected || is_disconnecting || is_sleeping) {
                /* notify when connection state changed or when we disconnected due to sleep */
                DEBUG("atwinc15x0: notify upper layer about disconnect\n");
                atwinc15x0->netdev.event_callback(&atwinc15x0->netdev, NETDEV_EVENT_LINK_DOWN);
            }
            if (IS_USED(MODULE_ATWINC15X0_DYNAMIC_CONNECT)) {
                if (!was_connected && !is_disconnecting && !is_sleeping) {
                    /* connection failed */
                    DEBUG("atwinc15x0: notify about connection failure\n");
                    wifi_disconnect_result_t disconn
                        = WIFI_DISCONNECT_RESULT_INITIALIZER(
                            _atwinc15x0_connect_req.conn_req.base.channel,
                            _atwinc15x0_connect_req.conn_req.ssid);
                    _wifi_disconnect_result_callback(&disconn);
                }
                else if ((was_connected || is_disconnecting || is_sleeping) && !is_connecting) {
                    /* disconnect from previous connection */
                    DEBUG("atwinc15x0: notify about disconnect\n");
                    wifi_disconnect_result_t disconn
                        = WIFI_DISCONNECT_RESULT_INITIALIZER(
                            atwinc15x0->channel,
                            _atwinc15x0_sta_get_current_ssid(atwinc15x0));
                    _wifi_disconnect_result_callback(&disconn);
                }
            }
            if (IS_USED(MODULE_ATWINC15X0_STATIC_CONNECT)) {
                /* do not reconnect on sleep */
                if (!_atwinc15x0_is_sleeping(atwinc15x0)) {
                    /* schedule reconnect timer:
                    Not trying to reconnect immediately allows
                    other connect requests to get through. */
                    _atwinc15x0_set_reconnect_timer();
                }
            }
            break;
        case M2M_WIFI_CONNECTED:
            LOG_INFO("[atwinc15x0] WiFi connected\n");
            _atwinc15x0_set_connected(atwinc15x0);
            atwinc15x0->netdev.event_callback(&atwinc15x0->netdev, NETDEV_EVENT_LINK_UP);
            /* get information about the current AP */
            m2m_wifi_get_connection_info();
            if (IS_USED(MODULE_ATWINC15X0_DYNAMIC_CONNECT)) {
                _atwinc15x0_sta_set_current_ssid(atwinc15x0,
                                                 _atwinc15x0_connect_req.conn_req.ssid);
                atwinc15x0->channel = _atwinc15x0_connect_req.conn_req.base.channel;
                wifi_connect_result_t conn
                    = WIFI_CONNECT_RESULT_INITIALIZER(
                        atwinc15x0->channel,
                        _atwinc15x0_sta_get_current_ssid(atwinc15x0));
                if (_atwinc15x0_connect_req.conn_req.cred) {
                    if (*_atwinc15x0_connect_req.conn_req.cred == WIFI_SECURITY_MODE_WEP_PSK) {
                        conn.credentials.wep = *((const wifi_security_wep_psk_t *)
                                                _atwinc15x0_connect_req.conn_req.cred);
                    }
                    else if (*_atwinc15x0_connect_req.conn_req.cred == WIFI_SECURITY_MODE_WPA2_PERSONAL) {
                        conn.credentials.wpa_psk = *((const wifi_security_wpa_psk_t *)
                                                _atwinc15x0_connect_req.conn_req.cred);
                    }
                    else if (*_atwinc15x0_connect_req.conn_req.cred == WIFI_SECURITY_MODE_WPA2_ENTERPRISE) {
                        conn.credentials.wpa_enterprise = *((const wifi_security_wpa_enterprise_t *)
                                                           _atwinc15x0_connect_req.conn_req.cred);
                    }
                }
                _wifi_connect_result_callback(&conn);
            }
            if (IS_USED(MODULE_ATWINC15X0_STATIC_CONNECT)) {
                /* start a scan for additional info, e.g. used channel */
                m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
            }
            break;
    }
}

static void _atwinc15x0_handle_resp_conn_info(const tstrM2MConnInfo *conn_info)
{
    DEBUG("%s conn info %s, rssi %d, sec %u, bssid "
          ATWINC15X0_MAC_STR "\n", __func__,
          conn_info->acSSID,
          conn_info->s8RSSI,
          conn_info->u8SecType,
          ATWINC15X0_MAC_STR_ARG(conn_info->au8MACAddress));

    /* set the RSSI and BSSID of the current AP */
    atwinc15x0->rssi = conn_info->s8RSSI;
    memcpy(atwinc15x0->ap, conn_info->au8MACAddress, ETHERNET_ADDR_LEN);
    if (IS_USED(MODULE_ATWINC15X0_DYNAMIC_CONNECT)) {
        _atwinc15x0_sta_set_current_ssid(atwinc15x0, conn_info->acSSID);
    }
}

static void _atwinc15x0_handle_resp_current_rssi(int8_t rssi)
{
    DEBUG("%s current rssi %d\n", __func__, rssi);
    /* set the RSSI */
    atwinc15x0->rssi = rssi;
    _rssi_info_ready = true;
}

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
            _atwinc15x0_handle_resp_scan_done(&event->scan_done);
            break;
        case M2M_WIFI_RESP_SCAN_RESULT:
            _atwinc15x0_handle_resp_scan_result(&event->scan_result);
            break;
        case M2M_WIFI_RESP_CON_STATE_CHANGED:
            _atwinc15x0_handle_resp_con_state_changed(&event->state_changed);
            break;
        case M2M_WIFI_RESP_CONN_INFO:
            _atwinc15x0_handle_resp_conn_info(&event->conn_info);
            break;
        case M2M_WIFI_RESP_CURRENT_RSSI:
            _atwinc15x0_handle_resp_current_rssi(event->rssi);
            break;
    }
}

static int _atwinc15x0_send(netdev_t *netdev, const iolist_t *iolist)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    assert(dev);
    assert(dev == atwinc15x0);
    assert(iolist);

    /* send wakes from standby but not from sleep */
    if (_atwinc15x0_is_sleeping(dev)) {
        DEBUG("%s WiFi is in SLEEP state, cannot send\n", __func__);
        return -ENETDOWN;
    }
    if (!_atwinc15x0_is_connected(dev)) {
        DEBUG("%s WiFi is still not connected to AP, cannot send\n", __func__);
        return -ENETDOWN;
    }
    /* atwinc15x0_eth_buf should not be used for incoming packets here */
    assert(dev->rx_buf == NULL);

    uint16_t tx_len = 0;

    /* load packet data into the buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (tx_len + iol->iol_len > ETHERNET_MAX_LEN) {
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

    /* send the packet */
    if (m2m_wifi_send_ethernet_pkt(atwinc15x0_eth_buf, tx_len) == M2M_SUCCESS) {
        return tx_len;
    }
    else {
        DEBUG("%s sending WiFi packet failed", __func__);
        return -EIO;
    }
}

static int _confirm_send(netdev_t *netdev, void *info)
{
    (void)netdev;
    (void)info;

    return -EOPNOTSUPP;
}

static int _atwinc15x0_recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    (void)info;
    assert(dev);
    assert(dev == atwinc15x0);

    uint16_t rx_size = dev->rx_len;

    if (!rx_size) {
        /* there is nothing in receive buffer */
        return 0;
    }

    if (!buf) {
        /* get the size of the frame */
        if (len > 0) {
            /* if len > 0, drop the frame */
            dev->rx_len = 0;
            dev->rx_buf = NULL;
        }
        return rx_size;
    }

    if (len < rx_size) {
        /* buffer is smaller than the number of received bytes */
        DEBUG("%s not enough space in receive buffer", __func__);
        /* newest API requires to drop the frame in that case */
        dev->rx_len = 0;
        dev->rx_buf = NULL;
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

    return rx_size;
}

static netopt_enable_t _get_link_state(atwinc15x0_t *dev)
{
    return _atwinc15x0_is_connected(dev) ? NETOPT_ENABLE : NETOPT_DISABLE;
}

static int _atwinc15x0_get(netdev_t *netdev, netopt_t opt, void *val,
                           size_t max_len)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    (void)max_len;
    assert(dev);
    assert(dev == atwinc15x0);

    DEBUG("%s dev=%p opt=%u val=%p max_len=%" PRIuSIZE "\n", __func__,
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
            *((netopt_enable_t *)val) = _get_link_state(dev);
            return sizeof(netopt_enable_t);

        case NETOPT_CHANNEL:
            assert(max_len == sizeof(uint16_t));
            *((uint16_t *)val) = dev->channel;
            return sizeof(uint16_t);

        case NETOPT_STATE:
            assert(max_len >= sizeof(netopt_state_t));
            *((netopt_state_t *)val) = _get_state(dev);
            return sizeof(netopt_state_t);

        case NETOPT_RSSI:
            assert(max_len == sizeof(int16_t));
            _rssi_info_ready = false;
            if (!_atwinc15x0_is_connected(dev)) {
                return -ECANCELED;
            }
            /* trigger the request current RSSI (asynchronous function) */
            if (m2m_wifi_req_curr_rssi() != M2M_SUCCESS) {
                return 0;
            }
            /* wait for the response with a given timeout */
            unsigned int _rssi_info_time_out = ATWINC15X0_WAIT_TIMEOUT;
            while (!_rssi_info_ready && _rssi_info_time_out--) {
                ztimer_sleep(ZTIMER_MSEC, ATWINC15X0_WAIT_TIME_MS);
            }
            /* return the RSSI */
            *((int16_t *)val) = dev->rssi;
            return sizeof(int16_t);

        default:
            return netdev_eth_get(netdev, opt, val, max_len);
    }
}

static int _set_state(atwinc15x0_t *dev, netopt_state_t state)
{
    if (_atwinc15x0_is_busy(dev)) {
        return -EBUSY;
    }
    switch (state) {
    case NETOPT_STATE_SLEEP:
        _atwinc15x0_set_sleeping(dev);
        m2m_wifi_disconnect();
        m2m_wifi_set_sleep_mode(M2M_PS_MANUAL, CONFIG_ATWINC15X0_RECV_BCAST);
        m2m_wifi_request_sleep(UINT32_MAX);
        if (gpio_is_valid(atwinc15x0->params.wake_pin)) {
            gpio_clear(atwinc15x0->params.wake_pin);
        }
        return sizeof(netopt_state_t);
    case NETOPT_STATE_IDLE:
        if (_atwinc15x0_is_sleeping(dev)) {
            if (gpio_is_valid(atwinc15x0->params.wake_pin)) {
                gpio_set(atwinc15x0->params.wake_pin);
            }
            m2m_wifi_set_sleep_mode(M2M_PS_DEEP_AUTOMATIC, CONFIG_ATWINC15X0_RECV_BCAST);
            _atwinc15x0_set_disconnected(dev);
            if (IS_USED(MODULE_ATWINC15X0_STATIC_CONNECT)) {
                _atwinc15x0_set_reconnect_timer();
            }
        }
        return sizeof(netopt_state_t);
    case NETOPT_STATE_RESET:
        if (_atwinc15x0_is_sleeping(dev)) {
            _set_state(dev, NETOPT_STATE_IDLE);
        }
        else if (_atwinc15x0_is_connected(dev)) {
            m2m_wifi_disconnect();
        }
        _atwinc15x0_init(&dev->netdev);
        return sizeof(netopt_state_t);
    default:
        break;
    }

    return -ENOTSUP;
}

static netopt_state_t _get_state(const atwinc15x0_t *dev)
{
    if (dev->state == ATWINC15X0_STATE_SLEEP) {
        return NETOPT_STATE_SLEEP;
    }
    else {
        return NETOPT_STATE_IDLE;
    }
}

static int _atwinc15x0_set(netdev_t *netdev, netopt_t opt, const void *val,
                           size_t max_len)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    DEBUG("%s dev=%p opt=%u val=%p max_len=%" PRIuSIZE "\n", __func__,
          (void *)netdev, opt, val, max_len);

    int ret;
    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len == ETHERNET_ADDR_LEN);
            m2m_wifi_set_mac_address((uint8_t *)val);
            return ETHERNET_ADDR_LEN;
        case NETOPT_STATE:
            assert(max_len <= sizeof(netopt_state_t));
            return _set_state(dev, *((const netopt_state_t *)val));
        case NETOPT_L2_GROUP:
            /* sometimes m2m_wifi_enable_mac_mcast() fails with M2M_ERR_MEM_ALLOC */
            m2m_wifi_enable_mac_mcast((void *)val, 0);
            /* sometimes it fails with M2M_ERR_BUS_FAIL */
            int tries = 5;
            do {
                ret = m2m_wifi_enable_mac_mcast((void *)val, 1);
                DEBUG_PUTS("busy loop setting L2 multicast address on atwinc15x0");
            } while (--tries && ret == M2M_ERR_BUS_FAIL);
            if (ret) {
                return -EINVAL;
            } else {
                return max_len;
            }
        case NETOPT_L2_GROUP_LEAVE:
            if (m2m_wifi_enable_mac_mcast((void *)val, 0)) {
                return -EINVAL;
            } else {
                return max_len;
            }
        case NETOPT_SCAN:
            if (!IS_USED(MODULE_ATWINC15X0_DYNAMIC_SCAN)) {
                break;
            }
            if (max_len < sizeof(wifi_scan_request_t)) {
                return -EINVAL;
            }
            if ((ret = _atwinc15x0_scan((const wifi_scan_request_t *)val)) != 0) {
                return ret;
            }
            return sizeof(wifi_scan_request_t);
        case NETOPT_CONNECT:
            if (!IS_USED(MODULE_ATWINC15X0_DYNAMIC_CONNECT)) {
                break;
            }
            if (max_len < sizeof(wifi_connect_request_t)) {
                return -EINVAL;
            }
            if ((ret = _atwinc15x0_connect((const wifi_connect_request_t *)val)) != 0) {
                return ret;
            }
            return sizeof(wifi_connect_request_t);
        case NETOPT_DISCONNECT:
            if (!IS_USED(MODULE_ATWINC15X0_DYNAMIC_CONNECT)) {
                break;
            }
            if (max_len < sizeof(wifi_disconnect_request_t)) {
                return -EINVAL;
            }
            if ((ret = _atwinc15x0_disconnect((const wifi_disconnect_request_t *)val)) != 0) {
                return ret;
            }
            return sizeof(wifi_disconnect_request_t);
        default:
            break;
    }
    return netdev_eth_set(netdev, opt, val, max_len);
}

static void _print_firmware_version(const tstrM2mRev *info)
{
    LOG_DEBUG("[atwinc15x0] CHIP ID: %lu\n",
              info->u32Chipid);
    LOG_DEBUG("[atwinc15x0] FIRMWARE: %u.%u.%u\n",
              info->u8FirmwareMajor, info->u8FirmwareMinor, info->u8FirmwarePatch);
    LOG_DEBUG("[atwinc15x0] DRIVER: %u.%u.%u\n",
              info->u8DriverMajor, info->u8DriverMinor, info->u8DriverPatch);
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
    atwinc15x0->state = ATWINC15X0_STATE_DISCONNECTED;
    atwinc15x0->rx_len = 0;
    atwinc15x0->rx_buf = NULL;

    nm_bsp_init();

    int res;
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

    /* get firmware version */
    if (IS_ACTIVE(ENABLE_DEBUG)) {
        tstrM2mRev fw_ver;
        if ((res = m2m_wifi_get_firmware_version(&fw_ver)) != M2M_SUCCESS) {
            LOG_ERROR("[atwinc15x0] Could not read firmware version\n");
        }
        else {
            _print_firmware_version(&fw_ver);
        }
    }
    /* set Wi-Fi region */
    if (WIFI_REGION == WIFI_REGION_EUROPE) {
        res = m2m_wifi_set_scan_region(EUROPE);
    }
    else if (WIFI_REGION == WIFI_REGION_NORTH_AMERICA) {
        res = m2m_wifi_set_scan_region(NORTH_AMERICA);
    }
    else if (WIFI_REGION == WIFI_REGION_ASIA ) {
        res = m2m_wifi_set_scan_region(ASIA);
    }
    if (res != M2M_SUCCESS) {
        return -ENOTSUP;
    }
    /* disable the built-in DHCP client */
    if ((res = m2m_wifi_enable_dhcp(false)) != M2M_SUCCESS) {
        LOG_ERROR("[atwinc15x0] m2m_wifi_enable_dhcp failed with %d\n", res);
        return res;
    }

    /* enable automatic power saving */
    m2m_wifi_set_sleep_mode(M2M_PS_DEEP_AUTOMATIC, CONFIG_ATWINC15X0_RECV_BCAST);

    res = 0;
    if (IS_USED(MODULE_ATWINC15X0_STATIC_CONNECT)) {
        /* try to connect and return */
        res = _atwinc15x0_static_connect();
    }
    return res;
}

static int _atwinc15x0_scan(const wifi_scan_request_t *req)
{
    assert(req);
    (void)req;
    if (!IS_USED(MODULE_ATWINC15X0_DYNAMIC_SCAN)) {
        return 0;
    }
    if (_atwinc15x0_is_busy(atwinc15x0)) {
        return -EBUSY;
    }
    if (req->base.channel != NETOPT_SCAN_REQ_ALL_CH) {
        if (req->base.channel < WIFI_2_4_CH_MIN ||
            req->base.channel > WIFI_2_4_CH_MAX) {
            return -EINVAL;
        }
    }
    if (_atwinc15x0_is_sleeping(atwinc15x0)) {
        _set_state(atwinc15x0, NETOPT_STATE_IDLE);
    }
    tstrM2MScanOption opt = {
        .u8NumOfSlot = ATWINC1510_SCAN_SLOTS_DEF,
        .u8ProbesPerSlot = ATWINC1510_SCAN_PROBES_NUMOF_DEF,
        .u8SlotTime = ATWINC1510_SCAN_SLOT_TIME_MS_DEF,
        .s8RssiThresh = ATWINC1510_SCAN_THRESHOLD_DBM_DEF,
    };
    if (req->timeout_ms_per_ch) {
        uint16_t ch_time_max = ATWINC1510_SCAN_SLOT_TIME_MS_MAX * opt.u8NumOfSlot;
        if (req->timeout_ms_per_ch > ch_time_max) {
            opt.u8SlotTime = ATWINC1510_SCAN_SLOT_TIME_MS_MAX;
        }
        else {
            opt.u8SlotTime = req->timeout_ms_per_ch / opt.u8NumOfSlot;
        }
    }
    int ret;
    if ((ret = m2m_wifi_set_scan_options(&opt)) != M2M_SUCCESS) {
        LOG_ERROR("[atwinc15x0] WiFi setting scan options failed with %d\n", ret);
        return -EIO;
    }
    if ((ret = m2m_wifi_request_scan(req->base.channel == NETOPT_SCAN_REQ_ALL_CH
                                     ? M2M_WIFI_CH_ALL
                                     : req->base.channel)) != M2M_SUCCESS) {
        LOG_ERROR("[atwinc15x0] WiFi scan failed with %d\n", ret);
        return -EIO;
    }
    _atwinc15x0_set_scanning(atwinc15x0);
    _atwinc15x0_scan_req = *req;
    _wifi_scan_list_empty();
    return 0;
}

static int _atwinc15x0_disconnect(const wifi_disconnect_request_t *req)
{
    assert(req);
    if (_atwinc15x0_is_busy(atwinc15x0)) {
        return -EBUSY;
    }
    if (!_atwinc15x0_is_connected(atwinc15x0)) {
        /* also when sleeping */
        return -EALREADY;
    }
    int ret;
    if ((ret = m2m_wifi_disconnect()) != M2M_SUCCESS) {
        LOG_ERROR("[atwinc15x0] WiFi disconnect failed with %d\n", ret);
        return -EIO;
    }
    _atwinc15x0_set_disconnecting(atwinc15x0);
    _atwinc15x0_connect_req.disconn_req = *req;
    return 0;
}

static int _atwinc15x0_connect(const wifi_connect_request_t *req)
{
    assert(req);
    if (!IS_USED(MODULE_ATWINC15X0_DYNAMIC_CONNECT)) {
        return 0;
    }
    tuniM2MWifiAuth auth_info;
    tenuM2mSecType auth_type = M2M_WIFI_SEC_OPEN;
    if (_atwinc15x0_is_busy(atwinc15x0)) {
        return -EBUSY;
    }
    if (_atwinc15x0_is_connected(atwinc15x0)) {
        if (!strcmp(req->ssid, _atwinc15x0_sta_get_current_ssid(atwinc15x0))) {
            return -EALREADY;
        }
    }
    else if (_atwinc15x0_is_sleeping(atwinc15x0)) {
        _set_state(atwinc15x0, NETOPT_STATE_IDLE);
    }
    if (req->cred && *(req->cred) != WIFI_SECURITY_MODE_OPEN) {
        if (*(req->cred) == WIFI_SECURITY_MODE_WEP_PSK) {
            auth_type = M2M_WIFI_SEC_WEP;
            const wifi_security_wep_psk_t *cred = (const wifi_security_wep_psk_t *)req->cred;
            strscpy((char *)auth_info.au8PSK, cred->psk, sizeof(auth_info.au8PSK));
        }
        else if (*(req->cred) == WIFI_SECURITY_MODE_WPA2_PERSONAL) {
            auth_type = M2M_WIFI_SEC_WPA_PSK;
            const wifi_security_wpa_psk_t *cred = (const wifi_security_wpa_psk_t *)req->cred;
            strscpy((char *)auth_info.au8PSK, cred->psk, sizeof(auth_info.au8PSK));
        }
        else if (*(req->cred) == WIFI_SECURITY_MODE_WPA2_ENTERPRISE) {
            auth_type = M2M_WIFI_SEC_802_1X;
            const wifi_security_wpa_enterprise_t *cred
                = (const wifi_security_wpa_enterprise_t *)req->cred;
            strscpy((char *)auth_info.strCred1x.au8UserName, cred->user,
                    sizeof(auth_info.strCred1x.au8UserName));
            strscpy((char *)auth_info.strCred1x.au8Passwd, cred->pwd,
                    sizeof(auth_info.strCred1x.au8Passwd));
        }
        else {
            return -ENOTSUP;
        }
    }
    int8_t res;
    if (_atwinc15x0_is_connected(atwinc15x0)) {
        /* late disconnect to not interrupt connection on errors before */
        if ((res = m2m_wifi_disconnect()) != M2M_SUCCESS) {
            LOG_ERROR("[atwinc15x0] WiFi disconnect failed with %d\n", res);
            return -EIO;
        }
    }
    /* connect */
    if ((res = m2m_wifi_connect((char *)req->ssid, strlen(req->ssid),
                                auth_type, &auth_info,
                                M2M_WIFI_CH_ALL)) != M2M_SUCCESS) {
        LOG_ERROR("[atwinc15x0] WiFi connect failed with %d\n", res);
        return res;
    }
    _atwinc15x0_set_connecting(atwinc15x0);
    _atwinc15x0_connect_req.conn_req = *req;
    return 0;
}

static void _atwinc15x0_isr(netdev_t *netdev)
{
    atwinc15x0_t *dev = (atwinc15x0_t *)netdev;

    assert(dev);
    assert(dev == atwinc15x0);

    DEBUG("%s dev=%p\n", __func__, (void *)dev);

    /* handle pending ATWINC15x0 module events */
    if (m2m_wifi_handle_events(NULL) != M2M_SUCCESS) {
        DEBUG("%s handle events failed, reset device\n", __func__);
        _atwinc15x0_init(netdev);
    }
    int err;
    if (IS_USED(MODULE_ATWINC15X0_STATIC_CONNECT)) {
        if (_atwinc15x0_timer.timeout == ATWINC15X0_WIFI_STA_TIMEOUT_RECONNECT) {
            if (!_atwinc15x0_is_connected(atwinc15x0)) {
                /* try again if device is busy or the Atmel firmware throws an error */
                if ((err = _atwinc15x0_static_connect()) == -EBUSY || err == -EIO) {
                    _atwinc15x0_set_reconnect_timer();
                }
            }
            _atwinc15x0_timer.timeout = ATWINC15X0_WIFI_STA_TIMEOUT_NONE;
        }
    }
}

const netdev_driver_t atwinc15x0_netdev_driver = {
    .send = _atwinc15x0_send,
    .recv = _atwinc15x0_recv,
    .init = _atwinc15x0_init,
    .isr = _atwinc15x0_isr,
    .get = _atwinc15x0_get,
    .set = _atwinc15x0_set,
    .confirm_send = _confirm_send,
};

void atwinc15x0_setup(atwinc15x0_t *dev, const atwinc15x0_params_t *params, uint8_t idx)
{
    assert(dev);

    atwinc15x0 = dev;
    atwinc15x0->netdev.driver = &atwinc15x0_netdev_driver;
    atwinc15x0->params = *params;

    netdev_register(&dev->netdev, NETDEV_ATWINC15X0, idx);
}

void atwinc15x0_irq(void)
{
    if (atwinc15x0) {
        netdev_trigger_event_isr(&atwinc15x0->netdev);
    }
}
