/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_netdev_netdev2
 * @brief
 * @{
 *
 * @file
 * @brief   Definitions for netdev2 common IEEE 802.15.4 code
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NETDEV2_IEEE802154_H
#define NETDEV2_IEEE802154_H

#include "net/ieee802154.h"
#include "net/gnrc/nettype.h"
#include "net/netopt.h"
#include "net/netdev2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    IEEE 802.15.4 netdev2 flags
 * @brief   Flags for netdev_ieee802154_t::flags
 *
 * The flag-space `0xff00` is available for device-specific flags.
 * The flag-space `0x00ff` was chosen for global flags to be in accordance to
 * the IEEE 802.15.4 MAC header flags.
 * @{
 */

#define NETDEV2_IEEE802154_SEND_MASK        (0x0028)    /**< flags to take for send packets */
#define NETDEV2_IEEE802154_RAW              (0x0002)    /**< pass raw frame to upper layer */
/**
 * @brief   use long source addres (set) or short source address (unset)
 */
#define NETDEV2_IEEE802154_SRC_MODE_LONG    (0x0004)
/**
 * @brief enable security
 */
#define NETDEV2_IEEE802154_SECURITY_EN      (IEEE802154_FCF_SECURITY_EN)

/**
 * @brief   request ACK from receiver
 */
#define NETDEV2_IEEE802154_ACK_REQ          (IEEE802154_FCF_ACK_REQ)
/**
 * @}
 */

/**
 * @brief   Option parameter to be used with @ref NETOPT_CCA_MODE to set
 *          the mode of the clear channel assessment (CCA) defined
 *          in Std 802.15.4.
 */
typedef enum {
    NETDEV2_IEEE802154_CCA_MODE_1 = 1,  /**< Energy above threshold */
    NETDEV2_IEEE802154_CCA_MODE_2,      /**< Carrier sense only */
    NETDEV2_IEEE802154_CCA_MODE_3,      /**< Carrier sense with energy above threshold */
    NETDEV2_IEEE802154_CCA_MODE_4,      /**< ALOHA */
    NETDEV2_IEEE802154_CCA_MODE_5,      /**< UWB preamble sense based on the SHR of a frame */
    NETDEV2_IEEE802154_CCA_MODE_6,      /**< UWB preamble sense based on the packet
                                         *   with the multiplexed preamble */
} netdev2_ieee802154_cca_mode_t;

/**
 * @brief Extended structure to hold IEEE 802.15.4 driver state
 *
 * @extends netdev2_t
 *
 * Supposed to be extended by driver implementations.
 * The extended structure should contain all variable driver state.
 */
typedef struct {
    netdev2_t netdev;                       /**< @ref netdev2_t base class */
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
    uint16_t flags;                         /**< flags as defined above */
    /** @} */
} netdev2_ieee802154_t;

/**
 * @brief   Received packet status information for IEEE 802.15.4 radios
 */
typedef struct netdev2_radio_rx_info netdev2_ieee802154_rx_info_t;

/**
 * @brief   Fallback function for netdev2 IEEE 802.15.4 devices' _get function
 *
 * Supposed to be used by netdev2 drivers as default case.
 *
 * @param[in]   dev     network device descriptor
 * @param[in]   opt     option type
 * @param[out]  value   pointer to store the option's value in
 * @param[in]   max_len maximal amount of byte that fit into @p value
 *
 * @return              number of bytes written to @p value
 * @return              <0 on error
 */
int netdev2_ieee802154_get(netdev2_ieee802154_t *dev, netopt_t opt, void *value,
                           size_t max_len);

/**
 * @brief   Fallback function for netdev2 IEEE 802.15.4 devices' _set function
 *
 * Sets netdev2_ieee802154_t::pan, netdev2_ieee802154_t::short_addr, and
 * netdev2_ieee802154_t::long_addr in device struct.
 * Additionally @ref NETDEV2_IEEE802154_SRC_MODE_LONG,
 * @ref NETDEV2_IEEE802154_RAW and, @ref NETDEV2_IEEE802154_ACK_REQ in
 * netdev2_ieee802154_t::flags can be set or unset.
 *
 * The setting of netdev2_ieee802154_t::chan is omitted since the legality of
 * its value can be very device specific and can't be checked in this function.
 * Please set it in the netdev2_driver_t::set function of your driver.
 *
 * Be aware that this only manipulates the netdev2_ieee802154_t struct.
 * Configuration to the device needs to be done in the netdev2_driver_t::set
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
int netdev2_ieee802154_set(netdev2_ieee802154_t *dev, netopt_t opt, void *value,
                           size_t value_len);

#ifdef __cplusplus
}
#endif

#endif /* NETDEV2_IEEE802154_H */
/** @} */
