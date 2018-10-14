/*
 * Copyright (C) 2018 Bumsik Kim <k.bumsik@gmail.com>
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
 * @brief       Type definitions of internal interfaces for WINC1500 drivers
 *
 * @author      Bumsik Kim <k.bumsik@gmail.com>
 */

#ifndef WINC1500_INTERNAL_TYPES_H
#define WINC1500_INTERNAL_TYPES_H

/* From pkg/winc1500 */
#include "bsp/include/nm_bsp_internal.h"
#include "driver/include/m2m_types.h"

#include "bsp/include/nm_bsp.h"
#include "driver/source/m2m_hif.h"
#include "driver/include/m2m_ota.h"
#include "driver/include/m2m_wifi.h"

#if	!defined(MODULE_NETDEV_ETH)
#include "socket/include/socket.h"
#include "driver/include/m2m_ssl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

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

/**
 * @brief Forward declarations of structs used in the Atmel driver.
 * @{
 */
/** Originally in m2m_hif.c */
struct tstrHifContext {
	uint8 u8ChipMode;
	uint8 u8ChipSleep;
	uint8 u8HifRXDone;
	uint8 u8Interrupt;
	uint32 u32RxAddr;
	uint32 u32RxSize;
	tpfHifCallBack pfWifiCb;
	tpfHifCallBack pfIpCb;
	tpfHifCallBack pfOtaCb;
	tpfHifCallBack pfSigmaCb;
	tpfHifCallBack pfHifCb;
	tpfHifCallBack pfCryptoCb;
	tpfHifCallBack pfSslCb;
};
/** Originally in socket.c in Atmel driver */
#if	!defined(MODULE_NETDEV_ETH)
struct tstrSocket{
	uint8				*pu8UserBuffer;
	uint16				u16UserBufferSize;
	uint16				u16SessionID;
	uint16				u16DataOffset;
	uint8				bIsUsed;
	uint8				u8SSLFlags;
	uint8				bIsRecvPending;
};
#endif
/** Originally in m2m_ssl.c. in Atmel driver */
#ifdef CONF_CRYPTO_SOFT
/** Used in m2m_cypto.c */
struct tstrCryptoCtxt {
	tpfAppCryproCb pfAppCryptoCb;
	uint8 * pu8Digest;
	uint8 * pu8Rsa;
	uint8 u8CryptoBusy;
};
#endif
/** @} */

/**
 * @brief   Data structure containing variables used in Atmel driver, not RIOT port.
 */
typedef struct winc1500_internal {
    /**< Used in nm_bsp_riot.c */
    tpfNmBspIsr gpfIsr;
    /**< Used in m2m_hif.c */
    struct tstrHifContext gstrHifCxt;
    /**< Used in m2m_ota.c. */
    tpfOtaUpdateCb gpfOtaUpdateCb;      /**< should be initialized as NULL */
    tpfOtaNotifCb  gpfOtaNotifCb;       /**< should be initialized as NULL */
    /**< Used in m2m_wifi.c */
    volatile uint8 gu8ChNum;
    /* The next two are also used in winc1500_callback.c */
    uint8_t gu8scanInProgress;          /**< Should be initialized as 0 */
    tpfAppWifiCb gpfAppWifiCb;          /**< Should be initialized as NULL */
#ifdef ETH_MODE
    tpfAppEthCb gpfAppEthCb;            /**< Should be initialized as NULL */
    uint8* gau8ethRcvBuf;               /**< Should be initialized as NULL */
    uint16 gu16ethRcvBufSize ;
#endif
#ifdef CONF_MGMT
    tpfAppMonCb  gpfAppMonCb;           /**< Should be initialized as NULL */
    static struct _tstrMgmtCtrl
    {
    	uint8* pu8Buf;
    	uint16 u16Offset;
    	uint16 u16Sz;
    } gstrMgmtCtrl;               /**< Should be initialized {NULL, 0 , 0} */
#endif
    /**< Used in nmspi.c */
    uint8 gu8Crc_off;                   /**< Should be initialized as 0 */
#if	!defined(MODULE_NETDEV_ETH)
    /**< Used in socket.c in Atmel driver */
    volatile sint8 gsockerrno;
    volatile tstrSocket gastrSockets[MAX_SOCKET];
    volatile uint8 gu8OpCode;
    volatile uint16 gu16BufferSize;
    volatile uint16 gu16SessionID       /**< Should be initialized as 0 */

    volatile tpfAppSocketCb gpfAppSocketCb;
    volatile tpfAppResolveCb gpfAppResolveCb;
    volatile uint8 gbSocketInit;        /**< Should be initialized as 0 */
    volatile tpfPingCb gfpPingCb;
    /**< Used in m2m_ssl.c. */
    tpfAppSSLCb gpfAppSSLCb;            /**< Should be initialized as NULL */
    uint32 gu32HIFAddr;                 /**< Should be initialized as 0 */
#endif
#ifdef CONF_CRYPTO_SOFT
    /** Used in m2m_ssl.c */
    struct tstrCryptoCtxt gstrCryptoCtxt;
#endif
} winc1500_internal_t;

#ifdef __cplusplus
}
#endif

#endif /* WINC1500_INTERNAL_TYPES_H */
/** @} */
