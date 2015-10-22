/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_netopt   Configuration options for network APIs
 * @ingroup     net
 * @brief       List of available configuration options for the
 *              @ref net_gnrc_netdev and the @ref net_gnrc_netapi
 * @{
 *
 * @file
 * @brief       Definition of global configuration options
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef NETOPT_H_
#define NETOPT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Global list of configuration options available throughout the
 *          network stack, e.g. by netdev and netapi
 */
typedef enum {
    NETOPT_CHANNEL,             /**< get/set channel as uint16_t in host
                                 *   byte order */
    NETOPT_IS_CHANNEL_CLR,      /**< check if channel is clear */
    NETOPT_ADDRESS,             /**< get/set address in host byte order */

    /**
     * @brief    get/set long address in host byte order
     *
     * Examples for this include the EUI-64 in IEEE 802.15.4
     */
    NETOPT_ADDRESS_LONG,
    NETOPT_ADDR_LEN,            /**< get the default address length a
                                 *   network device expects as uint16_t in
                                 *   host byte order */
    NETOPT_SRC_LEN,             /**< get/set the address length to choose
                                 *   for the network device's source address
                                 *   as uint16_t in host byte order */
    /**
     * @brief    get/set the network ID as uint16_t in host byte order
     *
     * Examples for this include the PAN ID in IEEE 802.15.4
     */
    NETOPT_NID,

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
    NETOPT_IPV6_IID,
    NETOPT_TX_POWER,            /**< get/set the output power for radio
                                 *   devices in dBm as int16_t in host byte
                                 *   order */
    NETOPT_MAX_PACKET_SIZE,     /**< get/set the maximum packet size a
                                 *   network module can handle as uint16_t
                                 *   in host byte order */
    /**
     * @brief en/disable preloading or read the current state.
     *
     * Preload using gnrc_netdev_driver_t::send_data() or gnrc_netapi_send()
     * respectively, send setting state to @ref NETOPT_STATE_TX
     */
    NETOPT_PRELOADING,
    NETOPT_PROMISCUOUSMODE,     /**< en/disable promiscuous mode or read
                                 *   the current state */
    NETOPT_AUTOACK,             /**< en/disable link layer auto ACKs or read
                                 *   the current state */
    NETOPT_RETRANS,             /**< get/set the maximum number of
                                 *   retransmissions. */
    NETOPT_PROTO,               /**< get/set the protocol for the layer
                                 *   as type gnrc_nettype_t. */
    NETOPT_STATE,               /**< get/set the state of network devices as
                                 *   type netopt_state_t */
    NETOPT_RAWMODE,             /**< en/disable the pre-processing of data
                                 *   in a network device driver as type
                                 *   gnrc_nettype_t */
    /**
     * @brief en/disable the interrupt at reception start.
     *
     * It is mostly triggered after the preamble is correctly received
     *
     * @note not all transceivers may support this interrupt
     */
    NETOPT_RX_START_IRQ,

    /**
     * @brief en/disable the interrupt after packet reception.
     *
     * This interrupt is triggered after a complete packet is received.
     *
     * @note in case a transceiver does not support this interrupt, the event
     *       may be triggered by the driver
     */
    NETOPT_RX_END_IRQ,

    /**
     * @brief en/disable the interrupt right in the beginning of transmission.
     *
     * This interrupt is triggered when the transceiver starts to send out the
     * packet.
     *
     * @note in case a transceiver does not support this interrupt, the event
     *       may be triggered by the driver
     */
    NETOPT_TX_START_IRQ,

    /**
     * @brief en/disable the interrupt after packet transmission.
     *
     * This interrupt is triggered when the full packet is transmitted.
     *
     * @note not all transceivers may support this interrupt
     */
    NETOPT_TX_END_IRQ,

    /**
     * @brief Check automatically before sending if the channel is clear.
     *
     * This may be a hardware feature of the given transceiver, or might be
     * otherwise implemented in software. If the device supports CSMA this
     * option will enable CSMA with a certain set of parameters to emulate the
     * desired behaviour.
     *
     * @note Be sure not to set NETCONF_OPT_CSMA simultaneously.
     *
     * TODO: How to get feedback?
     */
    NETOPT_AUTOCCA,

    /**
     * @brief en/disable CSMA/CA support
     *
     * If the device supports CSMA in hardware, this option enables it with
     * default parameters. For further configuration refer to the other
     * NETCONF_OPT_CSMA_* options.
     */
    NETOPT_CSMA,
    NETOPT_CSMA_RETRIES,            /**< get/set the number of retries when
                                         when channel is busy */
    /**
     * @brief read-only check for a wired interface.
     *
     * If the interface is wireless this function will return -ENOTSUP, a
     * positive value otherwise.
     *
     * @note Setting this option will always return -EONOTSUP.
     */
    NETOPT_IS_WIRED,

    /**
     * @brief get a device's "type", e.g., ethernet, 802.15.4, ...
     */
    NETOPT_DEVICE_TYPE,

    /**
     * @brief get/set the channel page as defined by IEEE 802.15.4
     */
    NETOPT_CHANNEL_PAGE,

    /**
     * @brief get/set the CCA threshold for the radio transceiver
     *
     * This is the value, in dBm, that the radio transceiver uses to decide
     * if the channel is clear or not (CCA). If the current signal strength
     * (RSSI/ED) is stronger than this CCA threshold value, the transceiver
     * usually considers that the radio medium is busy. Otherwise, i.e.
     * if RSSI/ED value is less than the CCA threshold value, the radio
     * medium is supposed to be free (the possibly received weak signal
     * is considered to be background, meaningless noise).
     *
     * Most transceivers allow to set this CCA threshold value.
     * Some research work has proven that dynamically adapting it
     * to network environment can improve QoS, especially in WSN.
     */
    NETOPT_CCA_THRESHOLD,

    /* add more options if needed */

    /**
     * @brief   maximum number of options defined here
     *
     * @note    Interfaces are not meant to respond to that.
     */
    NETOPT_NUMOF,
} netopt_t;

/**
 * @brief   Binary parameter for enabling and disabling options
 */
typedef enum {
    NETOPT_DISABLE = 0,         /**< disable a given option */
    NETOPT_ENABLE = 1,          /**< enable a given option */
} netopt_enable_t;

/**
 * @brief   Option parameter to be used with @ref NETOPT_STATE to set or get
 *          the state of a network device or protocol implementation
 */
typedef enum {
    NETOPT_STATE_OFF = 0,       /**< powered off */
    NETOPT_STATE_SLEEP,         /**< sleep mode */
    NETOPT_STATE_IDLE,          /**< idle mode,
                                 *   the device listens to receive packets */
    NETOPT_STATE_RX,            /**< receive mode,
                                 *   the device currently receives a packet */
    NETOPT_STATE_TX,            /**< transmit mode,
                                 *   set: triggers transmission of a preloaded packet
                                 *   (see @ref NETOPT_PRELOADING*). The resulting
                                 *   state of the network device is @ref NETOPT_STATE_IDLE
                                 *   get: the network device is in the process of
                                 *   transmitting a packet */
    NETOPT_STATE_RESET,         /**< triggers a hardware reset. The resulting
                                 *   state of the network device is @ref NETOPT_STATE_IDLE */
    /* add other states if needed */
} netopt_state_t;

/**
 * @brief   Get a string ptr corresponding to opt, for debugging
 *
 * @param[in] opt   The option to get a string representation for
 *
 * @return          ptr to string representation for given option or "unknown"
 */
const char *netopt2str(netopt_t opt);

#ifdef __cplusplus
}
#endif

#endif /* NETOPT_H_ */
/** @} */
