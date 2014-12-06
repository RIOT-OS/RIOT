/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup radio_driver IEEE 802.15.4 radio driver interface
 * @brief API definitions for 802.15.4 radio transceivers' drivers
 * @ingroup drivers
 *
 * @{
 * @file
 * @author Kévin Roussel <Kevin.Roussel@inria.fr>
 *
 */

#ifndef IEEE802154_RADIO_DRIVER_API_
#define IEEE802154_RADIO_DRIVER_API_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback function type for receiving incoming packets
 *        from 802.15.4 radio transceiver.
 *
 * @param[in] buf  Pointer to the buffer containing the incoming
 *                 802.15.4 packet's raw data.
 * @param[in] len  Length (in bytes) of the incoming packet's raw data.
 * @param[in] rssi Value of the Receive Signal Strength Indicator (RSSI)
 *                 for the incoming packet.
 * @param[in] lqi  Value of the Link Quality Indicator (LQI)
 *                 for the incoming packet.
 * @param[in] crc_ok @c true if incoming packet's checksum (CRC) is valid;
 *                   @c false otherwise (corrupted packet).
 */
typedef void (* receive_802154_packet_callback_t)(void *buf,
                                                  unsigned int len,
                                                  int8_t rssi,
                                                  uint8_t lqi,
                                                  bool crc_ok);

/**
 * @brief Kind of packet to prepare/configure for transmission.
 *
 */
typedef enum {
    /** Beacon packet */
    PACKET_KIND_BEACON,

    /** Standard data packet */
    PACKET_KIND_DATA,

    /** Acknowledgement packet */
    PACKET_KIND_ACK,

    /** MAC command */
    PACKET_KIND_MAC_CMD,

    /** invalid packet kind */
    PACKET_KIND_INVALID = -1

} ieee802154_packet_kind_t;

/**
 * @brief Return values for packet emission function of 802.15.4 radio driver.
 *
 */
typedef enum {
    /** Transmission completed successfully */
    RADIO_TX_OK,

    /** Transmission buffer underflow (forgot to call @c load_tx()
        before @c transmit() ) */
    RADIO_TX_UNDERFLOW,

    /** Transmission cannot start because radio medium is already busy */
    RADIO_TX_MEDIUM_BUSY,

    /** Transmission failed because of collision on radio medium */
    RADIO_TX_COLLISION,

    /** Wrong parameter given to TX-related functions */
    RADIO_TX_INVALID_PARAM,

    /** Too much given data to be included in a single packet */
    RADIO_TX_PACKET_TOO_LONG,

    /** Transmission supposedly failed since no ACK packet
        has been received as response */
    RADIO_TX_NOACK,

    /** Transmission failed because of an unexpected (fatal?) error */
    RADIO_TX_ERROR,

} radio_tx_status_t;


/**
 * @brief Definition of an IEEE 802.15.4 node address.
 *
 * @details The @c union allows to choose between PAN-centric addressing
 *           ("volatile" 16-bit address and 16-bit PAN ID), or canonical
 *           IEEE 64-bit ("long") addressing.
 *
 */
typedef union {
    /** @brief PAN-centric ("short") addressing mode */
    struct {
        /** @brief Address assigned to the node within its current PAN */
        uint16_t addr;
        /** @brief ID of the PAN to which the node is currently associated */
        uint16_t id;
    } pan;
    /** @brief 64-bit ("long") addressing mode */
    uint64_t long_addr;
} ieee802154_node_addr_t;


/**
 * @brief IEEE 802.15.4 radio driver API definition.
 *
 * @details This is the set of functions that must be implemented
 *          by any driver for a 802.15.4 radio transceiver.
 *
 */
typedef struct {
    /**
     * @brief Initialize the radio transceiver (call before first use).
     */
    void (* init)(void);

    /**
     * @brief Turn radio transceiver on.
     *
     * @return @c true if radio transceiver was actually started;
     *         @c false if an error prevented transceiver to start.
     */
    bool (* on)(void);

    /**
     * @brief Turn radio transceiver off.
     */
    void (* off)(void);

    /**
     * @brief Indicate whether the radio transceiver is up and running.
     *
     * @return @c true if radio transceiver is on;
     *         @c false if it is off.
     */
    bool (* is_on)(void);

    /**
     * @brief Load the tranceiver TX buffer with the given
     *        IEEE 802.15.4 packet.
     *
     * @param[in] kind Kind of packet to transmit.
     * @param[in] dest Address of the node to which the packet is sent.
     * @param[in] use_long_addr @c true to use the 64-bit address mode
     *                          with @c dest param; @c false to use
     *                          "short" PAN-centric mode.
     * @param[in] wants_ack @c true to request an acknowledgement
     *                      from the receiving node for this packet;
     *                      @c false otherwise.
     * @param[in] buf Pointer to the buffer containing the payload
     *                of the 802.15.4 packet to transmit.
     *                The frame header (i.e.: FCS, sequence number,
     *                src and dest PAN and addresses) is inserted
     *                using values in accord with @c kind parameter
     *                and transceiver configuration.
     * @param[in] len Length (in bytes) of the outgoing packet payload.
     *
     * @return The outcome of this packet's transmission.
     *         @see radio_tx_status_t
     */
    radio_tx_status_t (* load_tx)(ieee802154_packet_kind_t kind,
                                  ieee802154_node_addr_t dest,
                                  bool use_long_addr,
                                  bool wants_ack,
                                  void *buf,
                                  unsigned int len);

    /**
     * @brief Transmit the data loaded into the transceiver TX buffer.
     *
     * @return The outcome of this packet's transmission.
     *         @see radio_tx_status_t
     */
    radio_tx_status_t (* transmit)(void);

    /**
     * @brief Transmit the given IEEE 802.15.4 packet,
     *        by calling successively functions @c load_tx()
     *        and @c transmit().
     *
     * @param[in] kind Kind of packet to transmit.
     * @param[in] dest Address of the node to which the packet is sent.
     * @param[in] use_long_addr @c true to use the 64-bit address mode
     *                          with @c dest param; @c false to use
     *                          "short" PAN-centric mode.
     * @param[in] wants_ack @c true to request an acknowledgement
     *                      from the receiving node for this packet;
     *                      @c false otherwise.
     * @param[in] buf Pointer to the buffer containing the payload
     *                of the 802.15.4 packet to transmit.
     *                The frame header (i.e.: FCS, sequence number,
     *                src and dest PAN and addresses) is inserted
     *                using values in accord with @c kind parameter
     *                and transceiver configuration.
     * @param[in] len Length (in bytes) of the outgoing packet payload.
     *
     * @return The outcome of this packet's transmission.
     *         @see radio_tx_status_t
     */
    radio_tx_status_t (* send)(ieee802154_packet_kind_t kind,
                               ieee802154_node_addr_t dest,
                               bool use_long_addr,
                               bool wants_ack,
                               void *buf,
                               unsigned int len);

    /**
     * @brief Define the function to be called back the radio transceiver
     *        has received a incoming packet.
     *
     * @param[in] recv_func the callback function to invoke for each
     *                      packet received by the radio transceiver.
     * @see receive_802154_packet_callback_t
     */
    void (* set_receive_callback)(receive_802154_packet_callback_t recv_func);

    /**
     * @brief Switch the radio transceiver to receive mode.
     */
    void (* switch_to_rx)(void);

    /**
     * @brief Set the 802.15.4 channel to be used
     *        by the radio transceiver as medium.
     *
     * @param[in] chan Channel to switch to.
     *                 Usual values are 0 (868 MHz band),
     *                  1 to 10 (915 MHz band), and
     *                  11 to 26 (2.4 GHz band).
     */
    void (* set_channel)(unsigned int chan);

    /**
     * @brief Get the 802.15.4 channel currently used
     *        by the radio transceiver as medium.
     */
    unsigned int (* get_channel)(void);

    /**
     * @brief Set the 16-bit short address to be used by
     *        the radio transceiver within the current PAN.
     *
     * @param[in] addr Address to use.
     */
    void (* set_address)(uint16_t addr);

    /**
     * @brief Get the 16-bit short address currently used by
     *        the radio transceiver within the current PAN.
     */
    uint16_t (* get_address)(void);

    /**
     * @brief Set the 64-bit long ("extended") address
     *        to be used by the radio transceiver.
     *
     * @param[in] addr Address to use.
     */
    void (* set_long_address)(uint64_t addr);

    /**
     * @brief Get the 64-bit long ("extended") address
     *        currently used by the radio transceiver.
     */
    uint64_t (* get_long_address)(void);

    /**
     * @brief Set the 16-bit PAN ID within which the radio
     *        transceiver shall operate.
     *
     * @param[in] pan PAN ID to use.
     */
    void (* set_pan_id)(uint16_t pan);

    /**
     * @brief Get the 16-bit PAN ID within which the radio
     *        transceiver currently operates.
     */
    uint16_t (* get_pan_id)(void);

    /**
     * @brief Set the transmission power of the radio transceiver.
     *
     * @param[in] pow Output power to use (in dB).
     */
    void (* set_tx_power)(int pow);

    /**
     * @brief Get the transmission (output) power currently used
     *        by the radio transceiver (in dB).
     */
    int (* get_tx_power)(void);

    /**
     * @brief Indicates if the radio medium is available for transmission
     *        ("Clear Channel Assessment").
     *
     * @return @c true if radio medium is "clear" (available);
     *         @c false if another transmission is already running.
     */
    bool (* channel_is_clear)(void);

    /**
     * @brief Set the radio transceiver in or out of "promiscuous mode"
     *        (i.e. reception of all packets without regard for their
     *        intended destination).
     *
     * @param[in] monitor Set to @c true to put the transceiver in
     *                    "promiscuous mode" (a.k.a. monitor mode);
     *                    @c false to only receive packets actually
     *                    destined to the currently used address (i.e.:
     *                    to activate hardware address-decoding).
     */
    void (* set_promiscuous_mode)(bool monitor);

    /**
     * @brief Indicates if the radio medium is currently in "promiscuous
     *        mode" (i.e. receiving all packets without regard for their
     *        intended destination).
     *
     * @return @c true if the transceiver is in "promiscuous mode";
     *         @c false if only packets actually destined to the
     *         current transceiver are received (i.e. hardware
     *         address-decoding is active).
     */
    bool (* in_promiscuous_mode)(void);

} ieee802154_radio_driver_t;

#ifdef __cplusplus
}
#endif

#endif /* IEEE802154_RADIO_DRIVER_API_ */

/**
 * @}
 */
