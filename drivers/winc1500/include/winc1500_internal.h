/*
 * Copyright (C) 2017 Bumsik Kim <k.bumsik@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_winc1500
 * @{
 *
 * @file
 * @brief       Internal interfaces for WINC1500 drivers
 *
 * @author      Bumsik Kim <k.bumsik@gmail.com>
 */

#ifndef WINC1500_INTERNAL_H
#define WINC1500_INTERNAL_H

#include "winc1500_params.h"
/* From pkg/winc1500 */
#include "driver/include/m2m_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name WINC1500 Socket API definitions
 * @{
 */
#define WINC1500_MAX_TCP_SOCKET         (7)
#define WINC1500_MAX_UDP_SOCKET         (4)
/** @} */

/**
 * @name WINC1500 driver configuration
 * @{
 */
#define WINC1500_STACKSIZE       (256 + 512)
#define WINC1500_EVENT_QUEUE_LEN       (1)
#define WINC1500_EVENT_MBOX_LEN        (1)
/** @} */

/**
 * @name   Flags for device internal states (see datasheet)
 * @{
 */
#define WINC1500_STATE_INIT          (1 << 0) /**< Driver is initialized */
#define WINC1500_STATE_IDLE          (1 << 1) /**< idle state after init */
#define WINC1500_STATE_STA           (1 << 2) /**< connected to an AP */
#define WINC1500_STATE_CONNECTED     (1 << 3) /**< connected to an AP */
#define WINC1500_STATE_IP_OBTAINED   (1 << 4) /**< IP is obtained from DHCP server */
/** Others are not used yet */
#define WINC1500_STATE_AP            (1 << 5) /**< Reserved. Wi-Fi AP mode */
#define WINC1500_STATE_SNIFFER       (1 << 6) /**< Reserved. WINC1500 sniffer mode */
#define WINC1500_STATE_P2P           (1 << 7) /**< Reserved. P2P mode (Wi-Fi Direct) */
/** @} */

/**
 * @brief   Flags for device internal states (see datasheet)
 */
typedef enum {
    WINC1500_EVENT_NOTHING =                (0), /**< Reserved, nothing happend */
    WINC1500_EVENT_OTHERS =                 (1<<0), /**< Unexpected event */
    /* From M2M_WIFI_RESP_CON_STATE_CHANGED */
    WINC1500_EVENT_CON_STATE_CONNECTED =    (1<<1), /**< WiFi connected */
    WINC1500_EVENT_CON_STATE_DISCONNECTED = (1<<2), /**< WiFi disconnected */
    /* From M2M_WIFI_RESP_SCAN_DONE */
    WINC1500_EVENT_SCAN_DONE =              (1<<3), /**< Scanning for APs done */
    /* From M2M_WIFI_RESP_SCAN_RESULT */
    WINC1500_EVENT_SCAN_RESULT =            (1<<4), /**< Scan result request done */
    /* From M2M_WIFI_RESP_CURRENT_RSSI */
    WINC1500_EVENT_CURRENT_RSSI =           (1<<5), /**< Request for Current 
                                                        connected RSSI done */
    /* From M2M_WIFI_RESP_CONN_INFO */
    WINC1500_EVENT_CONN_INFO =              (1<<6) /**< Request for the Connected
                                                        WiFi information done */
} winc1500_cb_msg_t;

/**
 * @brief   Flags for device internal states (see WINC1500 programming guide)
 *          Each types has a corresponding event.
 */
typedef union {
    tstrM2mWifiStateChanged state_change;   /**< M2M_WIFI_RESP_CON_STATE_CHANGED */
    tstrSystemTime          sys_time;       /**< M2M_WIFI_RESP_GET_SYS_TIME */
    tstrM2MConnInfo         conn_info;      /**< M2M_WIFI_RESP_CONN_INFO */
    tstrM2MIPConfig         ip_config;      /**< M2M_WIFI_REQ_DHCP_CONF */
    tstrM2MWPSInfo          wps_info;       /**< M2M_WIFI_REQ_WPS */
    uint32_t                ip_conflicted;  /**< M2M_WIFI_RESP_IP_CONFLICT */
    tstrM2mScanDone         scan_done;      /**< M2M_WIFI_RESP_SCAN_DONE */
    tstrM2mWifiscanResult   scan_result;    /**< M2M_WIFI_RESP_SCAN_RESULT */
    tstrM2MProvisionInfo    prov_info;      /**< M2M_WIFI_RESP_PROVISION_INFO */
    tstrM2MDefaultConnResp  default_conn_resp;  /**< M2M_WIFI_RESP_DEFAULT_CONNECT */
    tstrPrng                prng_result;        /**< M2M_WIFI_RESP_GET_PRNG */
    uint8_t                 rx_buf[8];      /**< M2M_WIFI_RESP_CURRENT_RSSI and
                                                 M2M_WIFI_RESP_CLIENT_INFO */
#ifdef MODULE_NETDEV_ETH
    tstrM2mIpRsvdPkt        recv_pkt;   /**< M2M_WIFI_RESP_ETHERNET_RX_PACKET */
#endif
} winc1500_event_info_t;

extern winc1500_t winc1500; /**< winc1500 object */

/**
 * @name   Message queue and data to be passed to the caller
 *          from the event handler
 * @{
 */
extern msg_t _queue[WINC1500_EVENT_QUEUE_LEN];
extern msg_t _mbox_msgs[WINC1500_EVENT_MBOX_LEN];

extern char _ssid[WINC1500_MAX_SSID_LEN + 1];
extern winc1500_ap_t _ap;
extern uint8_t *_mac_addr;
/** @} */

/**
 * @brief   A callback function when a Wi-Fi event received.
 */
void _wifi_cb(uint8_t opcode, uint16_t size, uint32_t addr);

/**
 * @brief   Prevent other threads from entering the function concurrently.
 */
static inline void _lock(winc1500_t *dev)
{
#ifdef MODULE_NETDEV_ETH
    mutex_lock(&dev->mutex);
#else
    spi_acquire(dev->params.spi, SPI_CS_UNDEF,
        WINC1500_SPI_MODE, WINC1500_SPI_CLOCK);
#endif
}

/**
 * @brief   Unlock concurrency lock
 */
static inline void _unlock(winc1500_t *dev)
{
#ifdef MODULE_NETDEV_ETH
    mutex_unlock(&dev->mutex);
#else
    spi_release(dev->params.spi);
#endif
}

#ifdef MODULE_NETDEV_ETH
/**
 * @brief   Lock the SPI bus
 */
static inline void _lock_bus(winc1500_t *dev)
{
    spi_acquire(dev->params.spi, SPI_CS_UNDEF,
        WINC1500_SPI_MODE, WINC1500_SPI_CLOCK);
}

/**
 * @brief   Unlock the SPI bus
 */
static inline void _unlock_bus(winc1500_t *dev)
{
    spi_release(dev->params.spi);
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* WINC1500_INTERNAL_H */
/** @} */
