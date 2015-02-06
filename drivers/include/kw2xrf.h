/*
 * Copyright (C) 2015 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_kw2xrf kw2x radio-driver
 * @ingroup     drivers
 * @brief       Device driver for the Freescale MKW2xD radio
 * @{
 *
 * @file
 * @brief       Interface definition for the MKW2xD device driver
 *
 * @author     Johann Fischer <j.fischer@phytec.de>
 * @author     Jonas Remmert <j.remmert@phytec.de>
 */
/*
 * @brief packet buffer
 * <pre>
     buffer for tx            buffer for rx
     -----------------         -----------------
   0 | Frame Length  |       0 | PSDU byte 0   |
     -----------------         -----------------
   1 | PSDU byte 0   |       1 | PSDU byte 1   |
     -----------------         -----------------
   2 | PSDU byte 1   |       2 | PSDU byte 2   |
     -----------------         -----------------
 ... |               |     ... |               |
     -----------------         -----------------
 125 | PSDU byte 125 |     125 | FCS byte 0    |
     -----------------         -----------------
 126 | res. (FCS)    |     126 | FCS byte 1    |
     -----------------         -----------------
 127 | res. (FCS)    |     127 | LQI           |
     -----------------         -----------------
  </pre>
*/

#ifndef MKW2XDRF_H_
#define MKW2XDRF_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "kernel_types.h"
#include "ieee802154_frame.h"

#include "transceiver.h"
#include "netdev/802154.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MKW2XDRF_RX_BUF_SIZE        3        /**< Max buffer size of kw2xrf rx buffer. */

#define MKW2XDRF_HEADER_LENGTH      3        /**< Physical-layer header lengh. */
#define MKW2XDRF_MAX_PKT_LENGTH     127      /**< Max packet length for kw2xrf device. */
#define MKW2XDRF_MAX_DATA_LENGTH    118      /**< Max data length in packet for kw2xrf device. */

#define MKW2XDRF_BROADCAST_ADDRESS  (0xFFFF) /**< Broadcast address. */

#define MKW2XDRF_DEFAULT_CHANNR     11       /**< Default radio channel. */
#define MKW2XDRF_DEFAULT_RADIO_PAN  0x0001   /**< Default radio pan ID */
/**
 *  Structure to represent a kw2xrf packet.
 */
typedef struct __attribute__((packed))
{
    uint8_t length;             /**< The length of the frame of the frame including fcs. */
    ieee802154_frame_t frame;   /**< The ieee802154 frame. */
    uint8_t lqi;                /**< The link quality indicator. */
    int8_t rssi;                /**< The rssi value. */
    bool crc;                   /**< 1 if crc was successfull, 0 otherwise. */
}
kw2xrf_packet_t;

extern kw2xrf_packet_t kw2xrf_rx_buffer[MKW2XDRF_RX_BUF_SIZE];

/**
 * @brief Initialize the KW2XRF transceiver.
 *
 * @param[in] dev The netdev network device.
 *
 * @return 0 if the radio was correctly initialized.
 */
int kw2xrf_initialize(netdev_t *dev);

#ifdef MODULE_TRANSCEIVER
/**
 * @brief Init the KW2XRF transceiver for use with RIOT's transceiver module.
 *
 * @param[in] tpid The PID of the transceiver thread.
 */
void kw2xrf_init(kernel_pid_t tpid);
#endif

/**
 * @brief Transmit the data loaded into the KW2XRF TX buffer.
 *
 * @param[in] dev       An IEEE 802.15.4 network device.
 *
 * @return @ref netdev_802154_tx_status_t
 */
netdev_802154_tx_status_t kw2xrf_transmit_tx_buf(netdev_t *dev);

/**
 * @brief Turn KW2XRF on.
 *
 * @return 0 if the radio was correctly turned on.
 */
int kw2xrf_on(void);

/**
 * @brief Turn KW2XRF off.
 */
void kw2xrf_off(void);

/**
 * @brief Indicate if the KW2XRF is on.
 *
 * @return 1 if the radio transceiver is on (active); 0 otherwise.
 */
int kw2xrf_is_on(void);

/**
 * @brief Switches the KW2XRF into receive mode.
 */
void kw2xrf_switch_to_rx(void);

/**
 * @brief Turns monitor (promiscuous) mode on or off.
 *
 * @param[in] mode The desired mode:
 *                 1 for monitor (promiscuous) mode;
 *                 0 for normal (auto address-decoding) mode.
 */
void kw2xrf_set_monitor(int mode);

/**
 * @brief Indicate if the KW2XRF is in monitor (promiscuous) mode.
 *
 * @return 1 if the transceiver is in monitor (promiscuous) mode;
 *         0 if it is in normal (auto address-decoding) mode.
 */
int kw2xrf_get_monitor(void);

/**
 * @brief Set the channel of the KW2XRF.
 *
 * @param[in] chan The desired channel, valid channels are from 11 to 26.
 *
 * @return The tuned channel after calling, or -1 on error.
 */
int kw2xrf_set_channel(unsigned int chan);

/**
 * @brief Get the channel of the KW2XRF.
 *
 * @return The tuned channel.
 */
unsigned int kw2xrf_get_channel(void);

/**
 * @brief Sets the short address of the KW2XRF.
 *
 * @param[in] addr The desired address.
 *
 * @return The set address after calling.
 */
uint16_t kw2xrf_set_address(uint16_t addr);

/**
 * @brief Gets the current short address of the KW2XRF.
 *
 * @return The current short address.
 */
uint16_t kw2xrf_get_address(void);

/**
 * @brief Sets the IEEE long address of the KW2XRF.
 *
 * @param[in] addr The desired address.
 *
 * @return The set address after calling.
 */
uint64_t kw2xrf_set_address_long(uint64_t addr);

/**
 * @brief Gets the current IEEE long address of the KW2XRF.
 *
 * @return The current IEEE long address.
 */
uint64_t kw2xrf_get_address_long(void);

/**
 * @brief Sets the pan ID of the KW2XRF.
 *
 * @param[in] pan The desired pan ID.
 *
 * @return The set pan ID after calling.
 */
uint16_t kw2xrf_set_pan(uint16_t pan);

/**
 * @brief Gets the current IEEE long address of the KW2XRF.
 *
 * @return The current pan ID.
 */
uint16_t kw2xrf_get_pan(void);

/**
 * @brief Sets the output (TX) power of the KW2XRF.
 *
 * @param[in] pow The desired TX (output) power in dBm,
 *                 valid values are -35 to 8; other values
 *                 will be "saturated" into this range.
 *
 * @return The set TX (output) power after calling.
 */
int kw2xrf_set_tx_power(int pow);

/**
 * @brief Gets the current output (TX) power of the KW2XRF.
 *
 * @return The current TX (output) power.
 */
int kw2xrf_get_tx_power(void);

/**
 * @brief Checks if the radio medium is available/clear to send
 *         ("Clear Channel Assessment" a.k.a. CCA).
 *
 * @param[in] dev   An IEEE 802.15.4 network device.
 *
 * @return a 0 value if radio medium is clear (available),
 *         a error-value otherwise.
 *
 */
int kw2xrf_channel_clear(netdev_t *dev);

/**
 * @brief Interrupt handler, gets fired when a RX overflow happens.
 *
 */
void kw2xrf_rxoverflow_irq(void);

/**
 * @brief Interrupt handler, gets fired when bytes in the RX FIFO are present.
 *
 */
void kw2xrf_rx_irq(void *args);

/**
 * @brief Sets the function called back when a packet is received.
 *        (Low-level mechanism, parallel to the `transceiver` module).
 *
 * @param[in] dev       An IEEE 802.15.4 network device.
 * @param[in] recv_cb   callback function for 802.15.4 packet arrival
 *
 * @return  0 on success
 * @return  -ENODEV if *dev* is not recognized
 * @return  -ENOBUFS, if maximum number of registable callbacks is exceeded
 */
int kw2xrf_add_raw_recv_callback(netdev_t *dev, netdev_802154_raw_packet_cb_t recv_cb);

/**
 * @brief Unsets the function called back when a packet is received.
 *        (Low-level mechanism, parallel to the `transceiver` module).
 *
 * @param[in] dev       An IEEE 802.15.4 network device.
 * @param[in] recv_cb   callback function to unset
 *
 * @return  0 on success
 * @return  -ENODEV if *dev* is not recognized
 */
int kw2xrf_rem_raw_recv_callback(netdev_t *dev, netdev_802154_raw_packet_cb_t recv_cb);

/**
 * @brief Sets a function called back when a data packet is received.
 *
 * @param[in] dev       An IEEE 802.15.4 network device.
 * @param[in] recv_cb   callback function for 802.15.4 data packet arrival
 *
 * @return  0 on success
 * @return  -ENODEV if *dev* is not recognized
 * @return  -ENOBUFS, if maximum number of registable callbacks is exceeded
 */
int kw2xrf_add_data_recv_callback(netdev_t *dev, netdev_rcv_data_cb_t recv_cb);

/**
 * @brief Unsets a function called back when a data packet is received.
 *
 * @param[in] dev       An IEEE 802.15.4 network device.
 * @param[in] recv_cb   callback function to unset
 *
 * @return  0 on success
 * @return  -ENODEV if *dev* is not recognized
 */
int kw2xrf_rem_data_recv_callback(netdev_t *dev, netdev_rcv_data_cb_t recv_cb);

/**
 * @brief RX handler, process data from the RX FIFO.
 *
 */
void kw2xrf_rx_handler(void);

/**
 * @brief Prepare the KW2XRF TX buffer to send with the given packet.
 *
 * @param[in] dev           An IEEE 802.15.4 network device.
 * @param[in] kind          Kind of packet to transmit.
 * @param[in] dest          Address of the node to which the packet is sent.
 * @param[in] use_long_addr 1 to use the 64-bit address mode
 *                          with *dest* param; 0 to use
 *                          "short" PAN-centric mode.
 * @param[in] wants_ack     1 to request an acknowledgement
 *                          from the receiving node for this packet;
 *                          0 otherwise.
 * @param[in] upper_layer_hdrs  header data from higher network layers from
 *                              highest to lowest layer. Must be prepended to
 *                              the data stream by the network device. May be
 *                              NULL if there are none.
 * @param[in] buf           Pointer to the buffer containing the payload
 *                          of the 802.15.4 packet to transmit.
 *                          The frame header (i.e.: FCS, sequence number,
 *                          src and dest PAN and addresses) is inserted
 *                          using values in accord with *kind* parameter
 *                          and transceiver configuration.
 * @param[in] len           Length (in bytes) of the outgoing packet payload.
 *
 * @return @ref netdev_802154_tx_status_t
 */
netdev_802154_tx_status_t kw2xrf_load_tx_buf(netdev_t *dev,
        netdev_802154_pkt_kind_t kind,
        netdev_802154_node_addr_t *dest,
        int use_long_addr,
        int wants_ack,
        netdev_hlist_t *upper_layer_hdrs,
        void *buf,
        unsigned int len);

/**
 * @brief Send function, sends a mkw2xrf_packet_t over the air.
 *
 * @param[in] *packet The Packet which will be send.
 *
 * @return The count of bytes which are send or -1 on error
 *
 */
int16_t kw2xrf_send(kw2xrf_packet_t *packet);

/**
 * Netdev representation of this driver.
 */
extern netdev_t kw2xrf_netdev;

/**
 * KW2XRF low-level radio driver definition.
 */
extern const netdev_802154_driver_t kw2xrf_driver;

#ifdef __cplusplus
}
#endif

#endif
/** @} */
