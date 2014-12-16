/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    netdev_802154  IEEE 802.15.4 interface
 * @addtogroup  netdev
 * @{
 * @file  netdev/802154.h
 * @brief API definitions for 802.15.4 radio transceivers' drivers
 * @author Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 */

#ifndef __NETDEV_802154_H_
#define __NETDEV_802154_H_

#include <stdint.h>

#include "kernel_types.h"
#include "netdev/base.h"

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
 * @param[in] crc_ok 1 if incoming packet's checksum (CRC) is valid;
 *                   0 otherwise (corrupted packet).
 */
typedef void (* netdev_802154_raw_packet_cb_t)(netdev_t *dev,
        void *buf,
        size_t len,
        int8_t rssi,
        uint8_t lqi,
        int crc_ok);

/**
 * @brief Kind of packet to prepare/configure for transmission.
 *
 */
typedef enum {
    /** Beacon packet */
    NETDEV_802154_PKT_KIND_BEACON,

    /** Standard data packet */
    NETDEV_802154_PKT_KIND_DATA,

    /** Acknowledgement packet */
    NETDEV_802154_PKT_KIND_ACK,

    /** MAC command */
    NETDEV_802154_PKT_KIND_MAC_CMD,

    /** invalid packet kind */
    NETDEV_802154_PKT_KIND_INVALID = -1

} netdev_802154_pkt_kind_t;

/**
 * @brief Return values for packet emission function of 802.15.4 radio driver.
 *
 */
typedef enum {
    /** Transmission completed successfully */
    NETDEV_802154_TX_STATUS_OK,

    /** Device not found or not an IEEE 802.15.4 device */
    NETDEV_802154_TX_STATUS_NO_DEV,

    /** Transmission buffer underflow (forgot to call netdev_802154_driver_t::load_tx()
        before netdev_802154_driver_t::transmit() ) */
    NETDEV_802154_TX_STATUS_UNDERFLOW,

    /** Transmission cannot start because radio medium is already busy */
    NETDEV_802154_TX_STATUS_MEDIUM_BUSY,

    /** Transmission failed because of collision on radio medium */
    NETDEV_802154_TX_STATUS_COLLISION,

    /** Wrong parameter given to TX-related functions */
    NETDEV_802154_TX_STATUS_INVALID_PARAM,

    /** Too much given data to be included in a single packet */
    NETDEV_802154_TX_STATUS_PACKET_TOO_LONG,

    /** Transmission supposedly failed since no ACK packet
        has been received as response */
    NETDEV_802154_TX_STATUS_NOACK,

    /** Transmission failed because of an unexpected (fatal?) error */
    NETDEV_802154_TX_STATUS_ERROR,

} netdev_802154_tx_status_t;

/**
 * @brief Definition of an IEEE 802.15.4 node address.
 *
 * @details The `union` allows to choose between PAN-centric addressing
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
} netdev_802154_node_addr_t;

/**
 * @brief IEEE 802.15.4 radio driver API definition.
 *
 * @details This is the set of functions that must be implemented
 *          by any driver for a 802.15.4 radio transceiver.
 *
 * @extends netdev_driver_t
 */
typedef struct {
    /**
     * @see netdev_driver_t::init
     */
    int (*init)(netdev_t *dev);

    /**
     * @details  wraps netdev_802154_driver_t::send with
     *
     * @see netdev_driver_t::send_data
     */
    int (*send_data)(netdev_t *dev, void *dest, size_t dest_len,
                     netdev_hlist_t *upper_layer_hdrs, void *data,
                     size_t data_len);

    /**
     * @see netdev_driver_t::add_receive_data_callback
     */
    int (*add_receive_data_callback)(netdev_t *dev, netdev_rcv_data_cb_t cb);

    /**
     * @see netdev_driver_t::rem_receive_data_callback
     */
    int (*rem_receive_data_callback)(netdev_t *dev, netdev_rcv_data_cb_t cb);

    /**
     * @see netdev_driver_t::get_option
     *
     * @details The options are constrained as follows:
     *
     *         *opt*                       | type        | *value_len*
     *         --------------------------- | ----------- | -----------
     *         NETDEV_OPT_CHANNEL          | uint8_t     | >= 1
     *         NETDEV_OPT_ADDRESS          | uint16_t    | >= 2
     *         NETDEV_OPT_NID              | uint16_t    | >= 2
     *         NETDEV_OPT_ADDRESS_LONG     | uint64_t    | >= 8
     *         NETDEV_OPT_TX_POWER         | int         | >= 4
     *         NETDEV_OPT_MAX_PACKET_SIZE  | uint8_t     | >= 1
     */
    int (*get_option)(netdev_t *dev, netdev_opt_t opt, void *value,
                      size_t *value_len);

    /**
     * @see netdev_driver_t::set_option
     *
     * @details The options are constrained as follows:
     *
     *         *opt*                       | type        | *value_len* | *value*
     *         --------------------------- | ----------- | ----------- | --------
     *         NETDEV_OPT_CHANNEL          | uint8_t     | >= 1        | <= 26
     *         NETDEV_OPT_ADDRESS          | uint16_t    | >= 2        |
     *         NETDEV_OPT_NID              | uint16_t    | >= 2        |
     *         NETDEV_OPT_ADDRESS_LONG     | uint64_t    | >= 8        |
     *         NETDEV_OPT_TX_POWER         | int         | >= 4        |
     *
     *         NETDEV_OPT_MAX_PACKET_SIZE can not be set.
     */
    int (*set_option)(netdev_t *dev, netdev_opt_t opt, void *value,
                      size_t value_len);

    /**
     * @see netdev_driver_t::get_state
     */
    int (*get_state)(netdev_t *dev, netdev_state_t *state);

    /**
     * @see netdev_driver_t::set_state
     */
    int (*set_state)(netdev_t *dev, netdev_state_t state);

    /**
     * @see netdev_driver_t::event
     */
    void (*event)(netdev_t *dev, uint32_t event_type);

    /**
     * @brief Load the transceiver TX buffer with the given
     *        IEEE 802.15.4 packet.
     *
     * @param[in] dev  the network device
     * @param[in] kind Kind of packet to transmit.
     * @param[in] dest Address of the node to which the packet is sent.
     * @param[in] use_long_addr 1 to use the 64-bit address mode
     *                          with *dest* param; 0 to use
     *                          "short" PAN-centric mode.
     * @param[in] wants_ack 1 to request an acknowledgement
     *                      from the receiving node for this packet;
     *                      0 otherwise.
     * @param[in] upper_layer_hdrs  header data from higher network layers from
     *                              highest to lowest layer. Must be prepended to
     *                              the data stream by the network device. May be
     *                              NULL if there are none.
     * @param[in] buf Pointer to the buffer containing the payload
     *                of the 802.15.4 packet to transmit.
     *                The frame header (i.e.: FCS, sequence number,
     *                src and dest PAN and addresses) is inserted
     *                using values in accord with *kind* parameter
     *                and transceiver configuration.
     * @param[in] len Length (in bytes) of the outgoing packet payload.
     *
     * @return The outcome of this packet's transmission.
     *         @see netdev_802154_tx_status_t
     */
    netdev_802154_tx_status_t (* load_tx)(netdev_t *dev,
                                          netdev_802154_pkt_kind_t kind,
                                          netdev_802154_node_addr_t *dest,
                                          int use_long_addr,
                                          int wants_ack,
                                          netdev_hlist_t *upper_layer_hdrs,
                                          void *buf,
                                          unsigned int len);

    /**
     * @brief Transmit the data loaded into the transceiver TX buffer.
     *
     * @param[in] dev the network device
     *
     * @return The outcome of this packet's transmission.
     *         @see netdev_802154_tx_status_t
     */
    netdev_802154_tx_status_t (* transmit)(netdev_t *dev);

    /**
     * @brief Transmit the given IEEE 802.15.4 packet,
     *        by calling successively functions netdev_802154_driver_t::load_tx()
     *        and netdev_802154_driver_t::transmit().
     *
     * @param[in] dev  the network device
     * @param[in] kind Kind of packet to transmit.
     * @param[in] dest Address of the node to which the packet is sent.
     * @param[in] use_long_addr 1 to use the 64-bit address mode
     *                          with *dest* param; 0 to use
     *                          "short" PAN-centric mode.
     * @param[in] wants_ack 1 to request an acknowledgement
     *                      from the receiving node for this packet;
     *                      0 otherwise.
     * @param[in] upper_layer_hdrs  header data from higher network layers from
     *                              highest to lowest layer. Must be prepended to
     *                              the data stream by the network device. May be
     *                              NULL if there are none.
     * @param[in] buf Pointer to the buffer containing the payload
     *                of the 802.15.4 packet to transmit.
     *                The frame header (i.e.: FCS, sequence number,
     *                src and dest PAN and addresses) is inserted
     *                using values in accord with *kind* parameter
     *                and transceiver configuration.
     * @param[in] len Length (in bytes) of the outgoing packet payload.
     *
     * @return The outcome of this packet's transmission.
     *         @see netdev_802154_tx_status_t
     */
    netdev_802154_tx_status_t (* send)(netdev_t *dev,
                                       netdev_802154_pkt_kind_t kind,
                                       netdev_802154_node_addr_t *dest,
                                       int use_long_addr,
                                       int wants_ack,
                                       netdev_hlist_t *upper_layer_hdrs,
                                       void *buf,
                                       unsigned int len);

    /**
     * @brief Add a function to be called back when the radio transceiver
     *        has received a incoming packet.
     *
     * @details  This function differentiates from
     *          netdev_driver_t::add_receive_data_callback() as it expects
     *          a callback that excepts the raw frame format of IEEE 802.15.4,
     *          rather than just the source and destination addresses and the
     *          payload.
     *
     * @param[in] dev       the network device
     * @param[in] recv_func the callback function to invoke for each
     *                      packet received by the radio transceiver.
     * @see netdev_802153_raw_packet_cb_t
     *
     * @return  0, on success
     * @return  -ENOBUFS, if maximum number of registrable callbacks is exceeded
     * @return  -ENODEV, if *dev* is not recognized
     */
    int (* add_receive_raw_callback)(netdev_t *dev, netdev_802154_raw_packet_cb_t recv_func);

    /**
     * @brief Remove a callback set by netdev_802154_driver_t::add_receive_raw_callback()
     *
     * @param[in] dev       the network device
     * @param[in] recv_func the callback function to invoke for each
     *                      packet received by the radio transceiver.
     * @see netdev_802153_raw_packet_cb_t
     *
     * @return  0, on success
     * @return  -ENODEV, if *dev* is not recognized
     */
    int (* rem_receive_raw_callback)(netdev_t *dev, netdev_802154_raw_packet_cb_t recv_func);

    /**
     * @brief Indicates if the radio medium is available for transmission
     *        ("Clear Channel Assessment").
     *
     * @param[in] dev the network device
     *
     * @return 1 if radio medium is "clear" (available);
     * @return 0 if another transmission is already running.
     * @return  -ENODEV, if *dev* is not recognized
     */
    int (* channel_is_clear)(netdev_t *dev);
} netdev_802154_driver_t;

/* define to implement yourself and omit compilation of this function */
#ifndef NETDEV_802154_SEND_DATA_OVERLOAD
/**
 * @brief  wraps netdev_802154_driver_t::send(), default value for
 *         netdev_802154_driver_t::send_data().
 *
 * @param[in] dev               the network device
 * @param[in] dest              the (hardware) destination address for the data
 *                              in host byte order.
 * @param[in] dest_len          the length of *dest* in byte
 * @param[in] upper_layer_hdrs  header data from higher network layers from
 *                              highest to lowest layer. Must be prepended to
 *                              the data stream by the network device. May be
 *                              NULL if there are none.
 * @param[in] data              the data to send
 * @param[in] data_len          the length of *data* in byte
 *
 * @return  the number of byte actually (data_len + total length of upper layer
 *          headers) send on success
 * @return  -EAFNOSUPPORT if address of length dest_len is not supported
 *          by the device *dev*
 * @return  -EBUSY if transmission cannot start because radio medium is already
 *          busy or collision on radio medium occured.
 * @return  -EINVAL if wrong parameter was given
 * @return  -ENODEV if *dev* is not recognized as IEEE 802.15.4 device
 * @return  -EMSGSIZE if the total frame size is too long to fit in a frame
 *          of the device *dev*
 * @return  -EIO if any other occured (netdev_802154_driver_t::send() returned
 *          NETDEV_802154_TX_STATUS_ERROR)
 */
int netdev_802154_send_data(netdev_t *dev, void *dest, size_t dest_len,
                            netdev_hlist_t *upper_layer_hdrs, void *data,
                            size_t data_len);
#endif /* NETDEV_802154_SEND_DATA_OVERLOAD */

/* define to implement yourself and omit compilation of this function */
#ifndef NETDEV_802154_SEND_OVERLOAD
/**
 * @brief Transmit the given IEEE 802.15.4 packet, by calling
 *        functions netdev_802154_driver_t::load_tx() and
 *        netdev_802154_driver_t::transmit() successfully. Default value for
 *        netdev_802154_driver_t::send()
 *
 * @param[in] dev  the network device
 * @param[in] kind Kind of packet to transmit.
 * @param[in] dest Address of the node to which the packet is sent.
 * @param[in] use_long_addr 1 to use the 64-bit address mode
 *                          with *dest* param; 0 to use
 *                          "short" PAN-centric mode.
 * @param[in] wants_ack 1 to request an acknowledgement
 *                      from the receiving node for this packet;
 *                      0 otherwise.
 * @param[in] upper_layer_hdrs  header data from higher network layers from
 *                              highest to lowest layer. Must be prepended to
 *                              the data stream by the network device. May be
 *                              NULL if there are none.
 * @param[in] buf Pointer to the buffer containing the payload
 *                of the 802.15.4 packet to transmit.
 *                The frame header (i.e.: FCS, sequence number,
 *                src and dest PAN and addresses) is inserted
 *                using values in accord with *kind* parameter
 *                and transceiver configuration.
 * @param[in] len Length (in bytes) of the outgoing packet payload.
 *
 * @return @ref netdev_802154_tx_status_t
 */
netdev_802154_tx_status_t netdev_802154_send(netdev_t *dev,
        netdev_802154_pkt_kind_t kind,
        netdev_802154_node_addr_t *dest,
        int use_long_addr,
        int wants_ack,
        netdev_hlist_t *upper_layer_hdrs,
        void *buf,
        unsigned int len);
#endif /* NETDEV_802154_SEND_OVERLOAD */

#ifdef __cplusplus
}
#endif

#endif /* __NETDEV_802154_H_ */

/**
 * @}
 */
