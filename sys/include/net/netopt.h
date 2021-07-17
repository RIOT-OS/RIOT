/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_netopt   Netopt - Configuration options for network APIs
 * @ingroup     net
 * @brief       List of available configuration options for the
 *              @ref drivers_netdev_api and the @ref net_gnrc_netapi
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
 *
 * The data type specified in parentheses for each individual option is the
 * data type to use for the argument when getting/setting the value of the option.
 *
 * All arguments longer than 1 byte (e.g. uint16_t) are given in host byte order
 * unless anything else is specified below.
 */
typedef enum {
    /**
     * @brief   (uint16_t) channel number
     */
    NETOPT_CHANNEL,
    /**
     * @brief   (@ref netopt_enable_t) check whether the network medium is clear
     *
     * Getting this option can be used to trigger a manual clear channel
     * assessment (CCA) on some wireless devices.
     */
    NETOPT_IS_CHANNEL_CLR,
    /**
     * @brief   (byte array, see below) link layer address in network byte order
     *
     * Device type   | Length | Meaning
     * ------------- | ------ | -----
     * IEEE 802.15.4 | 2      | device short address
     * Ethernet      | 6      | device MAC address
     * nrfmin        | 2      | device short address
     * CC110x        | 1      | device address
     * NRF24L01+     | 5      | device address
     * LoRaWAN       | 4      | device address
     */
    NETOPT_ADDRESS,

    /**
     * @brief   (byte array, see below) long link layer address in network byte order
     *
     * Device type   | Length   | Meaning
     * ------------- | -------- | -----
     * IEEE 802.15.4 | 8        | device long address (EUI-64), @ref eui64_t
     * nrfmin        | 8        | device long address (based on short address)
     * BLE           | 8        | device long address (EUI-64), @ref eui64_t
     * LoRaWAN       | 8        | Device EUI
     */
    NETOPT_ADDRESS_LONG,
    /**
     * @brief   (uint16_t) get the default address length a network device expects
     */
    NETOPT_ADDR_LEN,
    /**
     * @brief   (uint16_t) address length to use for the link layer source address
     */
    NETOPT_SRC_LEN,
    /**
     * @brief   (uint16_t) network ID
     *
     * Examples for this include the PAN ID in IEEE 802.15.4 and netid in
     * LoRaWAN (uint32_t in this case)
     */
    NETOPT_NID,

    /**
     * @brief   (uint8_t) hop limit
     */
    NETOPT_HOP_LIMIT,

    /**
     * @brief   (@ref eui64_t) get the IPv6 interface identifier of a network interface
     *
     * @see <a href="https://tools.ietf.org/html/rfc4291#section-2.5.1">
     *          RFC 4291, section 2.5.1
     *      </a>
     *
     * @note    Do not implement this in a network device driver. Other APIs
     *          utilizing [netopt](@ref net_netopt) such as @ref net_gnrc_netif
     *          or @ref net_netif may still implement it.
     *
     * The generation of the interface identifier is dependent on the link-layer.
     * Please refer to the appropriate IPv6 over `<link>` specification for
     * further implementation details (such as
     * <a href="https://tools.ietf.org/html/rfc2464">RFC 2464</a> or
     * <a href="https://tools.ietf.org/html/rfc4944">RFC 4944</a>).
     */
    NETOPT_IPV6_IID,

    /**
     * @brief   (@ref ipv6_addr_t[]) get IPv6 addresses of an interface as array
     *          of @ref ipv6_addr_t or add an IPv6 address as @ref ipv6_addr_t
     *          to an interface
     *
     * When adding an IPv6 address to a GNRC interface using
     * @ref GNRC_NETAPI_MSG_TYPE_SET, the gnrc_netapi_opt_t::context field can
     * be used to pass the prefix length (8 MSB) and some flags (8 LSB)
     * according to @ref net_gnrc_netif_ipv6_addrs_flags. The address is however
     * always considered to be manually added.
     * When getting the option you can pass an array of @ref ipv6_addr_t of any
     * length greater than 0 to the getter. The array will be filled up to to
     * its maximum and the remaining addresses on the interface will be ignored
     */
    NETOPT_IPV6_ADDR,
    /**
     * @brief   (@ref ipv6_addr_t) Removes an IPv6 address from an interface
     */
    NETOPT_IPV6_ADDR_REMOVE,
    /**
     * @brief   (array of uint8_t) get the flags to the addresses returned by
     *          @ref NETOPT_IPV6_ADDR as array
     *
     * The information contained in the array is very specific to the
     * interface's API. For GNRC e.g. the values are according to
     * @ref net_gnrc_netif_ipv6_addrs_flags.
     */
    NETOPT_IPV6_ADDR_FLAGS,
    /**
     * @brief   (@ref ipv6_addr_t) get IPv6 multicast groups of an interface as
     *          array of @ref ipv6_addr_t or join an IPv6 multicast group as
     *          @ref ipv6_addr_t on an interface
     *
     * When adding an IPv6 address to a GNRC interface using
     * @ref GNRC_NETAPI_MSG_TYPE_SET, the gnrc_netapi_opt_t::context field can
     * be used to pass the prefix length (8 MSB) and some flags (8 LSB)
     * according to @ref net_gnrc_netif_ipv6_addrs_flags. The address is however always
     * considered to be manually added.
     * When getting the option you can pass an array of @ref ipv6_addr_t of any
     * length greater than 0 to the getter. The array will be filled up to to
     * its maximum and the remaining addresses on the interface will be ignored
     */
    NETOPT_IPV6_GROUP,
    /**
     * @brief   (@ref ipv6_addr_t) Leave an IPv6 multicast group on an interface
     */
    NETOPT_IPV6_GROUP_LEAVE,
    /**
     * @brief   (@ref netopt_enable_t) IPv6 forwarding state
     */
    NETOPT_IPV6_FORWARDING,
    /**
     * @brief   (@ref netopt_enable_t) sending of IPv6 router advertisements
     */
    NETOPT_IPV6_SND_RTR_ADV,
    /**
     * @brief   (int16_t) transmit power for radio devices in dBm
     */
    NETOPT_TX_POWER,
    /**
     * @brief   (uint16_t) maximum protocol data unit
     */
    NETOPT_MAX_PDU_SIZE,
    /**
     * @brief   (uint16_t) protocol data unit size
     *
     * When set, fixes the number of bytes to be received. This is required for
     * MAC layers with implicit header mode (no packet length information in
     * PDDU) and predictable packet length (e.g LoRaWAN beacons). The device
     * driver implementation should attempt to read exactly the expected number
     * of bytes (possibly filling it up with garbage data if the payload is
     * smaller).
     *
     * When get, returns the number of expected bytes for the next reception.
     *
     * In some MAC layers it will only be effective if used in conjunction with
     * @ref NETOPT_FIXED_HEADER
     */
    NETOPT_PDU_SIZE,
    /**
     * @brief   (@ref netopt_enable_t) frame preloading
     *
     * Preload frame data using gnrc_netdev_driver_t::send_data() or gnrc_netapi_send(),
     * trigger sending by setting state to @ref NETOPT_STATE_TX
     */
    NETOPT_PRELOADING,
    /**
     * @brief   (@ref netopt_enable_t) promiscuous mode
     */
    NETOPT_PROMISCUOUSMODE,
    /**
     * @brief   (@ref netopt_enable_t) automatic link layer ACKs
     */
    NETOPT_AUTOACK,
    /**
     * @brief   (@ref netopt_enable_t) frame pending bit of ACKs
     *
     * For IEEE 802.15.4, this bit is copied into the frame pending subfield of
     * the ACK if it is the response to a data request MAC command frame.
     */
    NETOPT_ACK_PENDING,
    /**
     * @brief   (@ref netopt_enable_t) acknowledgement request on outgoing frames
     *
     * For IEEE 802.15.4, this bit is copied into the ACK req subfield of the
     * frame control field.
     */
    NETOPT_ACK_REQ,
    /**
     * @brief   (uint8_t) maximum number of retransmissions
     */
    NETOPT_RETRANS,
    /**
     * @brief   (@ref gnrc_nettype_t) the protocol for the layer
     */
    NETOPT_PROTO,
    /**
     * @brief   (@ref netopt_state_t) state of network device
     */
    NETOPT_STATE,
    /**
     * @brief   (@ref netopt_enable_t) when enabled, bypass protocol processing of incoming frames
     */
    NETOPT_RAWMODE,
    /**
     * @brief   (@ref netopt_enable_t) Used to check if the driver generates NETDEV_EVENT_RX_STARTED
     *          events
     *
     * It is mostly triggered after the preamble is correctly received
     *
     * @warning This value is read-only and cannot be configured at run-time
     */
    NETOPT_RX_START_IRQ,

    /**
     * @brief   (@ref netopt_enable_t) Used to check if the driver generates
     *          NETDEV_EVENT_RX_COMPLETE events
     *
     * This interrupt is triggered after a complete frame is received.
     *
     * @note    In case a transceiver does not support this interrupt, the event
     *          may be triggered by the driver
     * @warning This value is read-only and cannot be configured at run-time
     */
    NETOPT_RX_END_IRQ,

    /**
     * @brief   (@ref netopt_enable_t) Used to check if the driver generates NETDEV_EVENT_TX_STARTED
     *          events
     *
     * This interrupt is triggered when the transceiver starts to send out the
     * frame.
     *
     * @note    In case a transceiver does not support this interrupt, the event
     *          may be triggered by the driver
     * @warning This value is read-only and cannot be configured at run-time
     */
    NETOPT_TX_START_IRQ,

    /**
     * @brief   (@ref netopt_enable_t) Used to check if the driver generates
     *          NETDEV_EVENT_TX_COMPLETE events
     *
     * This interrupt is triggered when the full frame has been transmitted.
     *
     * @note not all transceivers may support this interrupt
     */
    NETOPT_TX_END_IRQ,

    /**
     * @brief   (@ref netopt_enable_t) perform channel clear assessment before transmitting
     *
     * This may be a hardware feature of the given transceiver, or might be
     * otherwise implemented in software. If the device supports CSMA this
     * option will enable CSMA with a certain set of parameters to emulate the
     * desired behaviour.
     *
     * @note Be sure not to set NETOPT_CSMA simultaneously.
     *
     * @todo How to get feedback?
     */
    NETOPT_AUTOCCA,

    /**
     * @brief (@ref netopt_enable_t) network interface link status.
     *
     * This option is used to set or check the link status (up or down).
     *
     * @note On error this option should return a negative number.
     */
    NETOPT_LINK,

    /**
     * @brief   (@ref netopt_enable_t) CSMA/CA support
     *
     * If the device supports CSMA in hardware, this option enables it with
     * default parameters. For further configuration refer to the other
     * NETOPT_CSMA_* options.
     */
    NETOPT_CSMA,

    /**
     * @brief   (uint8_t) maximum number of CSMA retries
     *
     * The maximum number of backoffs the CSMA-CA algorithm will attempt before
     * declaring a channel access failure. Named macMaxCsmaBackoffs in
     * IEEE Std 802.15.4-2015.
     *
     * IEEE 802.15.4 default: 4
     */
    NETOPT_CSMA_RETRIES,

    /**
     * @brief   (uint8_t) maximum backoff exponent for the CSMA-CA algorithm
     *
     * Named macMaxBE in IEEE Std 802.15.4-2015.
     *
     * IEEE 802.15.4 default: 5
     */
    NETOPT_CSMA_MAXBE,

    /**
     * @brief   (uint8_t) minimum backoff exponent for the CSMA-CA algorithm
     *
     * Named macMinBE in IEEE Std 802.15.4-2015.
     *
     * IEEE 802.15.4 default: 3
     */
    NETOPT_CSMA_MINBE,

    /**
     * @brief   (@ref netopt_enable_t) block transceiver sleep
     *
     * Enabling this option tells the MAC layer to never put the radio to sleep.
     * Useful in gateways and routers not running on batteries to improve
     * responsiveness and allow battery powered nodes on the same network to
     * sleep more often.
     */
    NETOPT_MAC_NO_SLEEP,

    /**
     * @brief   (@ref netopt_enable_t) read-only check for a wired interface.
     *
     * This option will return -ENOTSUP for wireless interfaces.
     *
     * @note Setting this option will always return -ENOTSUP.
     */
    NETOPT_IS_WIRED,

    /**
     * @brief   (uint16_t) device type
     *
     * e.g. NETDEV_TYPE_ETHERNET, NETDEV_TYPE_IEEE802154, etc.
     */
    NETOPT_DEVICE_TYPE,

    /**
     * @brief   (uint16_t) channel page as defined by IEEE 802.15.4
     */
    NETOPT_CHANNEL_PAGE,

    /**
     * @brief   (int8_t) CCA threshold for the radio transceiver
     *
     * This is the value, in dBm, that the radio transceiver uses to decide
     * whether the channel is clear or not (CCA). If the current signal strength
     * (RSSI/ED) is stronger than this CCA threshold value, the transceiver
     * usually considers that the radio medium is busy. Otherwise, i.e. if
     * RSSI/ED value is less than the CCA threshold value, the radio medium is
     * supposed to be free (the possibly received weak signal is considered to
     * be background, meaningless noise).
     *
     * Most transceivers allow to set this CCA threshold value. Some research
     * work has proven that dynamically adapting it to network environment can
     * improve QoS, especially in WSN.
     */
    NETOPT_CCA_THRESHOLD,

    /**
     * @brief   (uint8_t) CCA mode for the radio transceiver
     *
     * Get/set the CCA mode corresponding to the respective PHY standard.
     * - IEEE 802.15.4: @ref netdev_ieee802154_cca_mode_t
     */
    NETOPT_CCA_MODE,

    /**
     * @brief   (@ref netstats_t*) get statistics about sent and received packets and data of the
     *          device or protocol
     *
     * Expects a pointer to a @ref netstats_t struct that will be pointed to
     * the corresponding @ref netstats_t of the module.
     */
    NETOPT_STATS,

    /**
     * @brief   (@ref netopt_enable_t) link layer encryption.
     */
    NETOPT_ENCRYPTION,

    /**
     * @brief   (byte array) set encryption key
     *
     * The required byte array size is dependent on encryption algorithm and device.
     */
    NETOPT_ENCRYPTION_KEY,

    /**
     * @brief   (@ref netopt_rf_testmode_t) Test mode for the radio, e.g. for CE or FCC
     *          certification
     *
     * Get/set the test mode as type @ref netopt_rf_testmode_t or as uint8_t if
     * the radio supports other vendor specific test modes.
     *
     * @note Setting this option should always return -ENOTSUP, unless it was
     * explicitly allowed at build time, therefore it should be secured with an
     * additional macro in the device driver.
     *
     * @attention For development and certification purposes only! These test
     * modes can disturb normal radio communications and exceed the limits
     * established by the regulatory authority.
     *
     */
    NETOPT_RF_TESTMODE,

    /**
     * @brief   (@ref l2filter_t) add an address to a link layer filter list
     *
     * Getting this option from a device will return a pointer of type
     * @ref l2filter_t to the first entry of a filter list.
     * When setting this option a pointer to an link layer address as well as
     * the length of the address are expected as parameters.
     */
    NETOPT_L2FILTER,

    /**
     * @brief   (@ref l2filter_t) remove an address from a link layer filter list
     *
     * Getting this value always returns -ENOTSUP.
     * When setting this option a pointer to an link layer address as well as
     * the length of the address are expected as parameters. Setting this
     * option will lead to the given address being removed from the filer list.
     */
    NETOPT_L2FILTER_RM,

    /**
     * @brief   (int8_t) Energy level during the last performed CCA or RX frame
     *
     * Get the last ED level available as an int8_t. The source of the
     * measurement is unspecified and may come from the latest CCA
     * measurement (CCA mode 1), or from the last received frame.
     */
    NETOPT_LAST_ED_LEVEL,

    /**
     * @brief   (uint16_t) preamble length
     */
    NETOPT_PREAMBLE_LENGTH,

    /**
     * @brief   (@ref netopt_enable_t) frame integrity check (e.g CRC)
     */
    NETOPT_INTEGRITY_CHECK,

    /**
     * @brief   (uint32_t) channel center frequency
     *
     * For example, with LoRa, this corresponds to the center frequency of
     * each channel (867300000, etc) for a given frequency band
     * (868, 915, etc).
     */
    NETOPT_CHANNEL_FREQUENCY,

    /**
     * @brief   (@ref netopt_enable_t) channel hopping
     */
    NETOPT_CHANNEL_HOP,

    /**
     * @brief   (uint8_t) channel hopping period
     */
    NETOPT_CHANNEL_HOP_PERIOD,

    /**
      * @brief   (@ref netopt_enable_t) single frame reception
      *
      * If enabled, RX is turned off upon reception of a frame
      */
    NETOPT_SINGLE_RECEIVE,

    /**
     * @brief   (uint32_t) reception timeout of a frame
     *
     * @todo in what time unit?
     */
    NETOPT_RX_TIMEOUT,

    /**
     * @brief   (uint32_t) transmission timeout of a frame
     *
     * @todo in what time unit?
     */
    NETOPT_TX_TIMEOUT,

    /**
     * @brief   (uint8_t) radio modulation bandwidth
     */
    NETOPT_BANDWIDTH,

    /**
     * @brief   (uint8_t) radio spreading factor
     */
    NETOPT_SPREADING_FACTOR,

    /**
     * @brief   (uint8_t) radio coding rate
     */
    NETOPT_CODING_RATE,

    /**
     * @brief   (@ref netopt_enable_t) fixed header mode
     */
    NETOPT_FIXED_HEADER,

    /**
     * @brief   (@ref netopt_enable_t) IQ inverted
     */
    NETOPT_IQ_INVERT,

    /**
     * @brief   (@ref netopt_enable_t) 6Lo support
     *
     * @see [RFC 4944](https://tools.ietf.org/html/rfc4944)
     */
    NETOPT_6LO,

    /**
     * @brief   (@ref netopt_enable_t) header compression
     *
     * @see [RFC 6282](https://tools.ietf.org/html/rfc6282)
     */
    NETOPT_6LO_IPHC,

    /**
     * @brief   (uint8_t) retry amount from missing ACKs of the last transmission
     *
     * This retrieves the number of retries needed for the last transmission.
     * Only retransmissions due to missing ACK frames are considered, retries
     * due to CCA failures are not counted.
     */
    NETOPT_TX_RETRIES_NEEDED,

    /**
     * @brief   (netdev_ble_ctx_t) set BLE radio context (channel, CRC, AA)
     *
     * @warning As @ref drivers_netdev_ble is still experimental, use with care!
     */
    NETOPT_BLE_CTX,

    /**
     * @brief   (@ref netopt_enable_t) enable hardware checksumming
     *
     * If enabled, enable hardware checksumming of incoming frames.
     */
    NETOPT_CHECKSUM,

    /**
     * @brief   (@ref netopt_enable_t) enable busy mode
     *
     * When set, the PHY will enter busy mode, in which it will not accept
     * incoming frames until unset.
     */
    NETOPT_PHY_BUSY,

    /**
     * @brief   (uint8_t*) LoRaWAN application EUI (8 bytes length)
     */
    NETOPT_LORAWAN_APPEUI,

    /**
     * @brief   (uint8_t*) LoRaWAN application key (16 bytes length)
     */
    NETOPT_LORAWAN_APPKEY,

    /**
     * @brief   (uint8_t*) LoRaWAN network session key (16 bytes length)
     */
    NETOPT_LORAWAN_NWKSKEY,
    /**
     * @brief   (uint8_t*) LoRaWAN application session key (16 bytes length)
     */
    NETOPT_LORAWAN_APPSKEY,

     /**
     * @brief   (uint8_t) LoRaWAN device class (A, B, C)
     * - LoRaWAN: @ref loramac_class_t
     */
    NETOPT_LORAWAN_DEVICE_CLASS,

    /**
     * @brief   (uint8_t) LoRaWAN datarate
     * - LoRaWAN: @ref loramac_dr_idx_t
     */
    NETOPT_LORAWAN_DR,

    /**
     * @brief   (@ref netopt_enable_t) LoRaWAN adaptive datarate
     */
    NETOPT_LORAWAN_ADR,

    /**
     * @brief   (@ref netopt_enable_t) LoRaWAN public network
     */
    NETOPT_LORAWAN_PUBLIC_NETWORK,

    /**
     * @brief   (uint8_t) LoRaWAN TX application port
     * - LoRaWAN: between 1 and 223 (included)
     *
     * @deprecated  This option is deprecated and will be removed in the
     *              2022.01 Release.
     *              The port is encoded now as a one byte
     *              destination address in a @ref net_gnrc_netif_hdr snip
     *              prepended in the packet.
     *              The user must take care of prepending the required snip
     *              during transmission. On reception, the
     *              snip is prepended automatically by the stack and shall be
     *              consumed by the user.
     *              During the deprecation period it is required to
     *              compile with @ref
     *              CONFIG_GNRC_NETIF_LORAWAN_NETIF_HDR
     *
     */
    NETOPT_LORAWAN_TX_PORT,

    /**
     * @brief   (loramac_dr_idx_t) LoRaWAN datarate for second RX window
     * - LoRaWAN: @ref loramac_dr_idx_t
     */
    NETOPT_LORAWAN_RX2_DR,

    /**
     * @brief   (uint32_t) LoRaWAN frequency used for second RX window
     */
    NETOPT_LORAWAN_RX2_FREQ,

    /**
     * @brief   (uint32_t) LoRaWAN maximum system overall timing error (ms)
     */
    NETOPT_LORAWAN_MAX_RX_ERROR,

    /**
     * @brief   (uint8_t) LoRaWAN maximum system overall timing error (symbols)
     */
    NETOPT_LORAWAN_MIN_RX_SYMBOL,

    /**
     * @brief   (uint8_t) 802.15.4 PHY mode
     */
    NETOPT_IEEE802154_PHY,

    /**
     * @brief   (uint8_t) legacy O-QPSK proprietary mode
     *          Allows to select higher data rates than standard 250 kbit/s
     *          Not compatible across vendors, only use with radios of the same type.
     */
    NETOPT_OQPSK_RATE,

    /**
     * @brief   (uint8_t) MR-O-QPSK Chip Rate (kchip/s)
     */
    NETOPT_MR_OQPSK_CHIPS,

    /**
     * @brief   (uint8_t) MR-O-QPSK Rate Mode
     */
    NETOPT_MR_OQPSK_RATE,

    /**
     * @brief   (uint8_t) MR-OFDM PHY Option (Values: 1-4)
     */
    NETOPT_MR_OFDM_OPTION,

    /**
     * @brief   (uint8_t) MR-OFDM PHY Modulation and Coding Scheme (Values: 0-6)
     */
    NETOPT_MR_OFDM_MCS,

    /**
     * @brief   (uint8_t) MR-FSK PHY Modulation Index (x 64)
     */
    NETOPT_MR_FSK_MODULATION_INDEX,

    /**
     * @brief   (uint8_t) MR-FSK Modulation Order
     */
    NETOPT_MR_FSK_MODULATION_ORDER,

    /**
     * @brief   (uint8_t) MR-FSK PHY Symbol Rate (kHz)
     */
    NETOPT_MR_FSK_SRATE,

    /**
     * @brief   (uint8_t) MR-FSK PHY Forward Error Correction
     */
    NETOPT_MR_FSK_FEC,

    /**
     * @brief   (uint8_t) PHY Channel Spacing (kHz)
     */
    NETOPT_CHANNEL_SPACING,

    /**
     * @brief   (uint8_t*) phy layer syncword
     */
    NETOPT_SYNCWORD,

    /**
     * @brief  (uint32_t) Get a random value from the device
     *
     * Nothing happens when set
     */
    NETOPT_RANDOM,

    /**
     * @brief (uint16_t) Get or set the number of PHY symbols before assuming there's no data
     */
    NETOPT_RX_SYMBOL_TIMEOUT,

    /* add more options if needed */

    /**
     * @brief (@ref netopt_enable_t) Enable or disable OTAA activation (LoRaWAN)
     */
    NETOPT_OTAA,

     /**
     * @brief (uint8_t) Get the demodulation margin of the last Link Check request.
     */
    NETOPT_DEMOD_MARGIN,

    /**
     * @brief (uint8_t) Get the number of gateways of the last Link Check request.
     */
    NETOPT_NUM_GATEWAYS,

    /**
     * @brief (@ref netopt_enable_t) Perform a Link Check request (LoRaWAN)
     *
     * When set, the next transmission will request a Link Check and will
     * be received on the next downlink
     */
    NETOPT_LINK_CHECK,

    /**
     * @brief (int8_t) Received Signal Strength Indicator (RSSI)
     *
     * The RSSI is an indicator for the received field strength in wireless
     * channels. It is often represented as the ratio of received power to
     * a given unit, for example milliwatts. With a device-dependent scaling
     * factor, the RSSI value can be expressed as power level in the unit
     * dBm or ASU (Arbitrary Strength Unit).
     */
    NETOPT_RSSI,

    /**
     * @brief (uint16_t) Set the battery monitor voltage (in mV).
     *
     * When set, a @ref SYS_BUS_POWER_EVENT_LOW_VOLTAGE event is generated
     * on the SYS_BUS_POWER bus if the supply voltage falls below the set value.
     *
     * Set to 0 to disable battery monitoring.
     */
    NETOPT_BATMON,

    /**
     * @brief   (array of byte array) get link layer multicast groups as array
     *          of byte arrays (length of each byte array corresponds to the
     *          length of @ref NETOPT_ADDRESS) or join a link layer multicast
     *          group as byte array on an interface
     *
     * When getting the option you can pass an array of byte arrays of any
     * length greater than 0 to the getter. The array will be filled up to to
     * its maximum and the remaining addresses on the interface will be ignored
     */
    NETOPT_L2_GROUP,
    /**
     * @brief   (array of byte arrays) Leave an link layer multicast group
     */
    NETOPT_L2_GROUP_LEAVE,
    /**
     * @brief   maximum number of options defined here.
     *
     * @note    Interfaces are not meant to respond to this option
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
