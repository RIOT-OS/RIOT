/*
 * Copyright (C) 2016-2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_netdev_ieee802154 802.15.4 radio drivers
 * @ingroup     drivers_netdev_api
 * @brief
 * @{
 *
 * @file
 * @brief   Definitions for netdev common IEEE 802.15.4 code
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NET_NETDEV_IEEE802154_H
#define NET_NETDEV_IEEE802154_H

#include "net/eui_provider.h"
#include "net/ieee802154.h"
#if IS_USED(MODULE_IEEE802154_SECURITY)
#include "net/ieee802154_security.h"
#endif
#include "net/gnrc/nettype.h"
#include "net/netopt.h"
#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    IEEE 802.15.4 netdev flags
 * @brief   Flags for netdev_ieee802154_t::flags
 *
 * The flag-space `0xff00` is available for device-specific flags.
 * The flag-space `0x00ff` was chosen for global flags to be in accordance to
 * the IEEE 802.15.4 MAC header flags.
 * @{
 */

#define NETDEV_IEEE802154_SEND_MASK         (0x0028)    /**< flags to take for send packets */
#define NETDEV_IEEE802154_RAW               (0x0002)    /**< pass raw frame to upper layer */
/**
 * @brief   use long source address (set) or short source address (unset)
 */
#define NETDEV_IEEE802154_SRC_MODE_LONG     (0x0004)
/**
 * @brief enable security
 */
#define NETDEV_IEEE802154_SECURITY_EN       (IEEE802154_FCF_SECURITY_EN)

/**
 * @brief   request ACK from receiver
 */
#define NETDEV_IEEE802154_ACK_REQ           (IEEE802154_FCF_ACK_REQ)

/**
 * @brief   set frame pending bit
 */
#define NETDEV_IEEE802154_FRAME_PEND        (IEEE802154_FCF_FRAME_PEND)
/**
 * @}
 */

 /**
 * @name    Flags for use in @ref netdev_ieee802154_rx_info::flags
 * @{
 */
#define NETDEV_RX_IEEE802154_INFO_FLAG_TIMESTAMP       (0x01)  /**< Timestamp valid */
/** @} */

/**
 * @brief   Option parameter to be used with @ref NETOPT_CCA_MODE to set
 *          the mode of the clear channel assessment (CCA) defined
 *          in Std 802.15.4.
 */
typedef enum {
    NETDEV_IEEE802154_CCA_MODE_1 = 1,   /**< Energy above threshold */
    NETDEV_IEEE802154_CCA_MODE_2,       /**< Carrier sense only */
    NETDEV_IEEE802154_CCA_MODE_3,       /**< Carrier sense with energy above threshold */
    NETDEV_IEEE802154_CCA_MODE_4,       /**< ALOHA */
    NETDEV_IEEE802154_CCA_MODE_5,       /**< UWB preamble sense based on the SHR of a frame */
    NETDEV_IEEE802154_CCA_MODE_6,       /**< UWB preamble sense based on the packet
                                         *   with the multiplexed preamble */
} netdev_ieee802154_cca_mode_t;

/**
 * @brief Extended structure to hold IEEE 802.15.4 driver state
 *
 * @extends netdev_t
 *
 * Supposed to be extended by driver implementations.
 * The extended structure should contain all variable driver state.
 */
typedef struct {
    netdev_t netdev;                        /**< @ref netdev_t base class */
    /**
     * @brief IEEE 802.15.4 specific fields
     * @{
     */
#ifdef MODULE_GNRC
    gnrc_nettype_t proto;                   /**< Protocol for upper layer */
#endif

    /**
     * @brief   PAN ID in network byte order
     */
    uint16_t pan;

    /**
     * @brief   Short address in network byte order
     */
    uint8_t short_addr[IEEE802154_SHORT_ADDRESS_LEN];

    /**
     * @brief   Long address in network byte order
     */
    uint8_t long_addr[IEEE802154_LONG_ADDRESS_LEN];
    uint8_t seq;                            /**< sequence number */
    uint8_t chan;                           /**< channel */
    uint8_t page;                           /**< channel page */
    uint16_t flags;                         /**< flags as defined above */
    int16_t txpower;                        /**< tx power in dBm */
#if IS_USED(MODULE_IEEE802154_SECURITY) || defined(DOXYGEN)
    ieee802154_sec_context_t sec_ctx;       /**< security context */
#endif
    /** @} */
} netdev_ieee802154_t;

/**
 * @brief   Received packet status information for IEEE 802.15.4 radios
 */
typedef struct netdev_ieee802154_rx_info {
    uint64_t timestamp;     /**< Timestamp value of a received frame in ns */
    int16_t rssi;           /**< RSSI of a received frame in dBm */
    uint8_t lqi;            /**< LQI of a received frame */
    uint8_t flags;          /**< Flags e.g. used to mark other fields as valid */
#if IS_USED(MODULE_SOCK_AUX_TIMESTAP)
    uint64_t timestamp;     /**< Timestamp value of a received frame in ns */
#endif
} netdev_ieee802154_rx_info_t;

/**
 * @brief   Reset function for ieee802154 common fields
 *
 * Supposed to be used by netdev drivers to reset the ieee802154 fields when
 * resetting the device
 *
 * @param[in]   dev     network device descriptor
 */
void netdev_ieee802154_reset(netdev_ieee802154_t *dev);


/**
 * @brief   Fallback function for netdev IEEE 802.15.4 devices' _get function
 *
 * Supposed to be used by netdev drivers as default case.
 *
 * @param[in]   dev     network device descriptor
 * @param[in]   opt     option type
 * @param[out]  value   pointer to store the option's value in
 * @param[in]   max_len maximal amount of byte that fit into @p value
 *
 * @return              number of bytes written to @p value
 * @return              <0 on error
 */
int netdev_ieee802154_get(netdev_ieee802154_t *dev, netopt_t opt, void *value,
                          size_t max_len);

/**
 * @brief   Fallback function for netdev IEEE 802.15.4 devices' _set function
 *
 * Sets netdev_ieee802154_t::pan, netdev_ieee802154_t::short_addr, and
 * netdev_ieee802154_t::long_addr in device struct.
 * Additionally @ref NETDEV_IEEE802154_SRC_MODE_LONG,
 * @ref NETDEV_IEEE802154_RAW and, @ref NETDEV_IEEE802154_ACK_REQ in
 * netdev_ieee802154_t::flags can be set or unset.
 *
 * The setting of netdev_ieee802154_t::chan is omitted since the legality of
 * its value can be very device specific and can't be checked in this function.
 * Please set it in the netdev_driver_t::set function of your driver.
 *
 * Be aware that this only manipulates the netdev_ieee802154_t struct.
 * Configuration to the device needs to be done in the netdev_driver_t::set
 * function of the device driver (which should call this function as a fallback
 * afterwards).
 *
 * @param[in] dev       network device descriptor
 * @param[in] opt       option type
 * @param[in] value     value to set
 * @param[in] value_len the length of @p value
 *
 * @return              number of bytes used from @p value
 * @return              <0 on error
 */
int netdev_ieee802154_set(netdev_ieee802154_t *dev, netopt_t opt, const void *value,
                          size_t value_len);

/**
 * @brief  This function compares destination address and pan id with addresses
 * and pan id of the device
 *
 * this function is meant top be used by drivers that do not support address
 * filtering in hw
 *
 * @deprecated  This function is currently deprecated and will be removed
 * after Release 2022.01. Use @ref ieee802154_dst_filter instead.
 *
 * @param[in] dev       network device descriptor
 * @param[in] mhr       mac header
 *
 * @return 0            successful if packet is for the device
 * @return 1            fails if packet is not for the device or pan
 */
int netdev_ieee802154_dst_filter(netdev_ieee802154_t *dev, const uint8_t *mhr);

/**
 * @brief   Configure the hardware address of a IEEE 802.15.4 devices
 *
 * This will obtain a long and short address based on the netdev ID.
 * The addresses is stored in the netdev's `long_addr` & `short_addr`.
 * The caller must take care of writing them to the hardware.
 *
 * @pre the netdev registered itself with @see netdev_register
 *
 * @param[out]  dev     Netdev to configure
 */
static inline void netdev_ieee802154_setup(netdev_ieee802154_t *dev)
{
    /* generate EUI-64 and short address */
    netdev_eui64_get(&dev->netdev, (eui64_t *)&dev->long_addr);
    eui_short_from_eui64((eui64_t *)&dev->long_addr,
                         (network_uint16_t *)&dev->short_addr);
}

#ifdef __cplusplus
}
#endif

#endif /* NET_NETDEV_IEEE802154_H */
/** @} */
