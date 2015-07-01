/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_netconf  Configuration options for network APIs
 * @ingroup     net
 * @brief       List of available configuration options for the
 *              @ref net_ng_netdev and the @ref net_ng_netapi
 * @{
 *
 * @file
 * @brief       Definition of global configuration options
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef NG_NET_CONF_H_
#define NG_NET_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Global list of configuration options available throughout the
 *          network stack, e.g. by netdev and netapi
 */
typedef enum {
    NETCONF_OPT_CHANNEL,            /**< get/set channel as uint16_t in host
                                     *   byte order */
    NETCONF_OPT_IS_CHANNEL_CLR,     /**< check if channel is clear */
    NETCONF_OPT_ADDRESS,            /**< get/set address in host byte order */

    /**
     * @brief    get/set long address in host byte order
     *
     * Examples for this include the EUI-64 in IEEE 802.15.4
     */
    NETCONF_OPT_ADDRESS_LONG,
    NETCONF_OPT_ADDR_LEN,           /**< get the default address length a
                                     *   network device expects as uint16_t in
                                     *   host byte order */
    NETCONF_OPT_SRC_LEN,            /**< get/set the address length to choose
                                     *   for the network device's source address
                                     *   as uint16_t in host byte order */
    /**
     * @brief    get/set the network ID as uint16_t in host byte order
     *
     * Examples for this include the PAN ID in IEEE 802.15.4
     */
    NETCONF_OPT_NID,

    /**
     * @brief   get the IPv6 interface identifier of a network interface as
     *          eui64_t.
     *
     * @see <a href="https://tools.ietf.org/html/rfc4291#section-2.5.1">
     *          RFC 4291, section 2.5.1
     *      </a>
     *
     * The generation of the interface identifier is dependent on the link-layer.
     * Please refer to the appropriate IPv6 over `<link>` specification for
     * further implementation details (such as
     * <a href="https://tools.ietf.org/html/rfc2464">RFC 2464</a> or
     * <a href="https://tools.ietf.org/html/rfc4944">RFC 4944</a>).
     */
    NETCONF_OPT_IPV6_IID,
    NETCONF_OPT_TX_POWER,           /**< get/set the output power for radio
                                     *   devices in dBm as int16_t in host byte
                                     *   order */
    NETCONF_OPT_MAX_PACKET_SIZE,    /**< get/set the maximum packet size a
                                     *   network module can handle as uint16_t
                                     *   in host byte order */
    /**
     * @brief en/disable preloading or read the current state.
     *
     * Preload using ng_netdev_driver_t::send_data() or ng_netapi_send()
     * respectively, send setting state to @ref NETCONF_STATE_TX
     */
    NETCONF_OPT_PRELOADING,
    NETCONF_OPT_PROMISCUOUSMODE,    /**< en/disable promiscuous mode or read
                                     *   the current state */
    NETCONF_OPT_AUTOACK,            /**< en/disable link layer auto ACKs or read
                                     *   the current state */
    NETCONF_OPT_RETRANS,            /**< get/set the maximum number of
                                         retransmissions. */
    NETCONF_OPT_PROTO,              /**< get/set the protocol for the layer
                                     *   as type ng_nettype_t. */
    NETCONF_OPT_STATE,              /**< get/set the state of network devices as
                                     *   type ng_netconf_state_t */
    NETCONF_OPT_RAWMODE,            /**< en/disable the pre-processing of data
                                     *   in a network device driver as type
                                     *   ng_nettype_t */
    /**
     * @brief en/disable the interrupt at reception start.
     *
     * It is mostly triggered after the preamble is correctly received
     *
     * @note not all transceivers may support this interrupt
     */
    NETCONF_OPT_RX_START_IRQ,

    /**
     * @brief en/disable the interrupt after packet reception.
     *
     * This interrupt is triggered after a complete packet is received.
     *
     * @note in case a transceiver does not support this interrupt, the event
     *       may be triggered by the driver
     */
    NETCONF_OPT_RX_END_IRQ,

    /**
     * @brief en/disable the interrupt right in the beginning of transmission.
     *
     * This interrupt is triggered when the transceiver starts to send out the
     * packet.
     *
     * @note in case a transceiver does not support this interrupt, the event
     *       may be triggered by the driver
     */
    NETCONF_OPT_TX_START_IRQ,

    /**
     * @brief en/disable the interrupt after packet transmission.
     *
     * This interrupt is triggered when the full packet is transmitted.
     *
     * @note not all transceivers may support this interrupt
     */
    NETCONF_OPT_TX_END_IRQ,
    NETCONF_OPT_AUTOCCA,            /**< en/disable to check automatically
                                         before sending the channel is clear. */
    /* add more options if needed */

    /**
     * @brief   maximum number of options defined here
     *
     * @note    Interfaces are not meant to respond to that.
     */
    NETCONF_OPT_NUMOF,
} ng_netconf_opt_t;

/**
 * @brief   Binary parameter for enabling and disabling options
 */
typedef enum {
    NETCONF_DISABLE = 0,            /**< disable a given option */
    NETCONF_ENABLE = 1,             /**< enable a given option */
} ng_netconf_enable_t;

/**
 * @brief   Option parameter to be used with @ref NETCONF_OPT_STATE to set or get
 *          the state of a network device or protocol implementation
 */
typedef enum {
    NETCONF_STATE_OFF = 0,          /**< powered off */
    NETCONF_STATE_SLEEP,            /**< sleep mode */
    NETCONF_STATE_IDLE,             /**< idle mode,
                                     *   the device listens to receive packets */
    NETCONF_STATE_RX,               /**< receive mode,
                                     *   the device currently receives a packet */
    NETCONF_STATE_TX,               /**< transmit mode,
                                     *   set: triggers transmission of a preloaded packet
                                     *   (see *NETCONF_OPT_PRELOADING*). The resulting
                                     *   state of the network device is *NETCONF_STATE_IDLE*
                                     *   get: the network device is in the process of
                                     *   transmitting a packet */
    NETCONF_STATE_RESET,            /**< triggers a hardware reset. The resulting
                                     *   state of the network device is *NETCONF_STATE_IDLE* */
    /* add other states if needed */
} ng_netconf_state_t;

#ifdef __cplusplus
}
#endif

#endif /* NG_NET_CONF_H_ */
/** @} */
