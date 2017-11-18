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

#ifndef NET_NETOPT_H
#define NET_NETOPT_H

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
     * @brief   get/set hop limit as uint8_t
     */
    NETOPT_HOP_LIMIT,

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

    /**
     * @brief   get IPv6 addresses of an interface as array of @ref ipv6_addr_t
     *          or add an IPv6 address as @ref ipv6_addr_t to an  interface
     *
     * When adding an IPv6 address to a GNRC interface using
     * @ref GNRC_NETAPI_MSG_TYPE_SET, the gnrc_netapi_opt_t::context field can
     * be used to pass the prefix length (8 MSB) and some flags (8 LSB)
     * according to @ref net_gnrc_netif2_ipv6_addrs_flags. The address is however always
     * considered to be manually added.
     * When getting the option you can pass an array of @ref ipv6_addr_t of any
     * length greater than 0 to the getter. The array will be filled up to to
     * its maximum and the remaining addresses on the interface will be ignored
     */
    NETOPT_IPV6_ADDR,
    /**
     * @brief   Removes an IPv6 address as @ref ipv6_addr_t from an interface
     */
    NETOPT_IPV6_ADDR_REMOVE,
    /**
     * @brief   get the flags to the addresses returned by @ref NETOPT_IPV6_ADDR
     *          as array of uint8_t
     *
     * The information contained in the array is very specific to the
     * interface's API. For GNRC e.g. the values are according to
     * @ref net_gnrc_netif2_ipv6_addrs_flags.
     */
    NETOPT_IPV6_ADDR_FLAGS,
    /**
     * @brief   get IPv6 multicast groups of an interface as array of
     *          @ref ipv6_addr_t or join an IPv6 multicast group as
     *          @ref ipv6_addr_t on an interface
     *
     * When adding an IPv6 address to a GNRC interface using
     * @ref GNRC_NETAPI_MSG_TYPE_SET, the gnrc_netapi_opt_t::context field can
     * be used to pass the prefix length (8 MSB) and some flags (8 LSB)
     * according to @ref net_gnrc_netif2_ipv6_addrs_flags. The address is however always
     * considered to be manually added.
     * When getting the option you can pass an array of @ref ipv6_addr_t of any
     * length greater than 0 to the getter. The array will be filled up to to
     * its maximum and the remaining addresses on the interface will be ignored
     */
    NETOPT_IPV6_GROUP,
    /**
     * @brief   Leaves an IPv6 multicast group as @ref ipv6_addr_t on an
     *          interface
     */
    NETOPT_IPV6_GROUP_LEAVE,
    NETOPT_IPV6_FORWARDING,     /**< en/disable IPv6 forwarding or read the
                                 *   current state */
    NETOPT_IPV6_SND_RTR_ADV,    /**< en/disable sending of IPv6 router
                                 *   advertisements or read the current state */
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
    NETOPT_ACK_REQ,             /**< en/disable acknowledgement requests or
                                 *   read the current state */
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
     * @note Be sure not to set NETOPT_CSMA simultaneously.
     *
     * TODO: How to get feedback?
     */
    NETOPT_AUTOCCA,

    /**
     * @brief en/disable CSMA/CA support
     *
     * If the device supports CSMA in hardware, this option enables it with
     * default parameters. For further configuration refer to the other
     * NETOPT_CSMA_* options.
     */
    NETOPT_CSMA,

    /**
     * @brief get/set the maximum number of CSMA retries
     *
     * (uint8_t)
     * The maximum number of backoffs the CSMA-CA algorithm will attempt before
     * declaring a channel access failure. Named macMaxCsmaBackoffs in
     * IEEE Std 802.15.4-2015.
     *
     * 802.15.4 default: 4
     */
    NETOPT_CSMA_RETRIES,

    /**
     * @brief get/set the maximum backoff exponent for the CSMA-CA algorithm
     *
     * (uint8_t) Named macMaxBE in IEEE Std 802.15.4-2015.
     *
     * 802.15.4 default: 5
     */
    NETOPT_CSMA_MAXBE,

    /**
     * @brief get/set the minimum backoff exponent for the CSMA-CA algorithm
     *
     * (uint8_t) Named macMinBE in IEEE Std 802.15.4-2015.
     *
     * 802.15.4 default: 3
     */
    NETOPT_CSMA_MINBE,

    /**
     * @brief en/disable blocking of radio sleep when running a duty cycling MAC layer
     *
     * (netopt_enable_t) Enabling this option tells the MAC layer to never put
     * the radio to sleep. Useful in gateways and routers not running on
     * batteries to improve responsiveness and allow battery powered nodes on
     * the same network to sleep more often.
     */
    NETOPT_MAC_NO_SLEEP,

    /**
     * @brief read-only check for a wired interface.
     *
     * If the interface is wireless this function will return -ENOTSUP, a
     * positive value otherwise.
     *
     * @note Setting this option will always return -ENOTSUP.
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

    /**
     * @brief CCA mode for the radio transceiver
     *
     * Get/set the CCA mode as uint8_t
     * corresponding to the respective PHY standard.
     * - IEEE 802.15.4: @ref netdev_ieee802154_cca_mode_t
     */
    NETOPT_CCA_MODE,

    /**
     * @brief get statistics about sent and received packets and data of the device or protocol
     *
     * Expects a pointer to a @ref netstats_t struct that will be pointed to
     * the corresponding @ref netstats_t of the module.
     */
    NETOPT_STATS,

    /**
     * @brief en/disable encryption.
     */
    NETOPT_ENCRYPTION,        /**< en/disable encryption */
    NETOPT_ENCRYPTION_KEY,    /**< set encryption key */

    /**
     * @brief Test mode for the radio, e.g. for CE or FCC certification
     *
     * Get/set the test mode as type @ref netopt_rf_testmode_t or as uint8_t
     * if the radio supports other vendor specific test modes.
     *
     * @note Setting this option should always return -ENOTSUP,
     * unless it was explicitly allowed at build time,
     * therefore it should be secured with an additional macro in the device driver.
     * For development and certification purposes only, this test modes can disturb
     * normal radio communications and exceed the limits, established by
     * the regulatory authority.
     *
     */
    NETOPT_RF_TESTMODE,

    /**
     * @brief   add an address to a link layer filter list
     *
     * 'Getting' this option from a device will return a pointer of type
     * @ref l2filter_t to the first entry of a filter list.
     * When 'Setting' this option a pointer to an link layer address as well as
     * the length of the address are expected as parameters.
     */
    NETOPT_L2FILTER,

    /**
     * @brief   remove an address from a link layer filter list
     *
     * 'Getting' this value always returns -ENOTSUP.
     * When 'Setting' this option a pointer to an link layer address as well as
     * the length of the address are expected as parameters. 'Setting' this
     * option will lead to the given address being removed from the filer list.
     */
    NETOPT_L2FILTER_RM,

    /**
     * @brief   Energy level during the last performed CCA or RX frame
     *
     * Get the last ED level available as an int8_t. The source of the
     * measurement is unspecified and may come from the latest CCA
     * measurement (CCA mode 1), or from the last received frame.
     */
    NETOPT_LAST_ED_LEVEL,

    /**
     * @brief   Get/Set preamble length as uint16_t in host byte order.
     */
    NETOPT_PREAMBLE_LENGTH,

    /**
     * @brief   Enable/disable integrity check (e.g CRC).
     */
    NETOPT_INTEGRITY_CHECK,

    /**
     * @brief   Enable/disable channel hopping.
     */
    NETOPT_CHANNEL_HOP,

    /**
     * @brief   Get/Set channel hopping period as uint8_t.
     */
    NETOPT_CHANNEL_HOP_PERIOD,

    /**
      * @brief   Enable/disable single packet reception.
      *
      * If enabled, RX is turned off upon reception of a packet
      */
    NETOPT_SINGLE_RECEIVE,

    /**
     * @brief   Get/Set the reception timeout of a packet.
     *
     * Values are retrieved/passed as uint32_t in host byte order.
     */
    NETOPT_RX_TIMEOUT,

    /**
     * @brief   Get/Set the transmission timeout of a packet.
     *
     * Values are retrieved/passed as uint32_t in host byte order.
     */
    NETOPT_TX_TIMEOUT,

    /**
     * @brief   Get/Set the radio modem type as uint8_t.
     */
    NETOPT_DEVICE_MODE,

    /**
     * @brief   Get/Set the radio modulation bandwidth as uint8_t.
     */
    NETOPT_BANDWIDTH,

    /**
     * @brief   Get/Set the radio spreading factor as uint8_t.
     */
    NETOPT_SPREADING_FACTOR,

    /**
     * @brief   Get/Set the radio coding rate as uint8_t.
     */
    NETOPT_CODING_RATE,

    /**
     * @brief   Enable/disable fixed header mode.
     */
    NETOPT_FIXED_HEADER,

    /**
     * @brief   Enable/disable IQ inverted.
     */
    NETOPT_IQ_INVERT,

    NETOPT_6LO_IPHC,            /**< en/disable header compression according to
                                 *   [RFC 6282](https://tools.ietf.org/html/rfc6282)
                                 *   or read the current state */

    /**
     * @brief   Get retry amount from missing ACKs of the last transmission
     *
     * This retrieves the number of retries needed for the last transmissions.
     * Only retransmissions due to missing ACK packets are considered.
     * Retries due to CCA failures are not counted.
     */
    NETOPT_TX_RETRIES_NEEDED,

    /* add more options if needed */

    /**
     * @brief   maximum number of options defined here.
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
    NETOPT_STATE_STANDBY,       /**< standby mode. The devices is awake but
                                 *   not listening to packets. */
    /* add other states if needed */
} netopt_state_t;

/**
 * @brief   Option parameter to be used with @ref NETOPT_RF_TESTMODE
 */
typedef enum {
    NETOPT_RF_TESTMODE_IDLE = 0,    /**< idle mode, radio off */
    NETOPT_RF_TESTMODE_CRX,         /**< continuous rx mode */
    NETOPT_RF_TESTMODE_CTX_CW,      /**< carrier wave continuous tx mode */
    NETOPT_RF_TESTMODE_CTX_PRBS9,   /**< PRBS9 continuous tx mode */
} netopt_rf_testmode_t;

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

#endif /* NET_NETOPT_H */
/** @} */
