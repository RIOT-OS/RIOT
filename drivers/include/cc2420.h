/**
  * cc2420.h - Definitions for CC2420 functions.
  * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
  * Copyright (C) 2014 Kévin Roussel <Kevin.Roussel@inria.fr>
  *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
  */


/**
  * @ingroup CC2420
  * @{
  * @file
  * @brief Definitions for CC2420 functions
  * @author Milan Babel <babel@inf.fu-berlin.de>
  *
  */

/**
  * @brief Definition of the cc2420 layer 0 protocol
  * <pre>
---------------------------------------------------------------------------
|        |         |          |         |         |            |          |
| Length |   FCF   |  Seq No. |Address  | PhySrc  |    Data    |    FCS   |
|        |         |          |         |         |            |          |
---------------------------------------------------------------------------
 1 byte    2 bytes     1 byte  2/8 bytes  2/8 bytes  <=118 bytes  2 bytes

A 5 byte SHR will be generated and added in hardware.
SHR contains a preable sequence and a start of delimiter,

Length does not contain SHR and Length,
first bit of length has to be 0 (length has only 7bit)

Address fields can be in total between 4 and 20 bytes
FCS contain a hardware generated CRC sum with the polynom x^16+x^12+x^5+1
When receiving a package FCS will be checked by hardware, the first FCS byte will be replaced by RSSI,
followed by a CRC OK bit and the unsigned 7 bit correlation value.
FCF:
        Bit | Meaning
        --------------------
        0-2 | Frame Type
          3 | Security Enabled
          4 | Frame Pending
          5 | Acknowledge request
          6 | PAN ID Compression Field
        7-9 | Reserved
      10-11 | Destination addressing mode
      12-13 | Reserved
      14-15 | Source addressing mode

For the cc2420 bit 0 is the most right bit and bit 15 is the most left bit.
But the 2 FCF bytes have to be transmitted littel endian (byte 15 to 8 first than 7 to 0)

Addressing mode value:

       Bit | Meaning
       ---------------------
        00 | PAN identifier and address field are not present.
        01 | Reserved.
        10 | Address field contains a 16 bit short address.
        11 | Address field contains a 64 bit extended address.

Frame type value:

       Bit | Meaning
       ---------------------
       000 | Beacon
       001 | Data
       010 | Acknowledgment
       011 | MAC command
       1xx | Reserved
  </pre>

  *  This corresponds to IEEE 802.15.4 frame format.
  */

#ifndef CC2420_H
#define CC2420_H

#include "kernel_types.h"
#include "ieee802154_frame.h"
#include "cc2420_settings.h"

#include "netdev/802154.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CC2420_MAX_PKT_LENGTH 127
#define CC2420_MAX_DATA_LENGTH (118)

#define CC2420_BROADCAST_ADDRESS (0xFFFF)

#define CC2420_MAX_UID (0xFFFE)
#define CC2420_MIN_UID (0x0000)

/**
 *  Structure to represent a cc2420 packet.
 */
typedef struct __attribute__ ((packed)) {
    /* @{ */
    uint8_t length;             /** < the length of the frame of the frame including fcs*/
    ieee802154_frame_t frame;   /** < the ieee802154 frame */
    int8_t rssi;                /** < the rssi value */
    uint8_t lqi;                /** < the link quality indicator */
    int crc;                    /** < 1 if crc was successfull, 0 otherwise */
    /* @} */
} cc2420_packet_t;

extern netdev_t cc2420_netdev;  /**< netdev representation of this driver */

/**
 * @brief Initialize the CC2420 transceiver.
 */
int cc2420_initialize(netdev_t *dev);

#ifdef MODULE_TRANSCEIVER
/**
 * @brief Init the CC2420 for use with RIOT's transceiver module.
 *
 * @param[in] tpid The PID of the transceiver thread.
 */

void cc2420_init(kernel_pid_t tpid);
#endif

/**
 * @brief Turn CC2420 on.
 *
 * @return 1 if the radio was correctly turned on; 0 otherwise.
 */
int cc2420_on(void);

/**
 * @brief Turn CC2420 off.
 */
void cc2420_off(void);

/**
 * @brief Indicate if the CC2420 is on.
 *
 * @return 1 if the radio transceiver is on (active); 0 otherwise.
 */
int cc2420_is_on(void);

/**
 * @brief Switches the CC2420 into receive mode.
 */
void cc2420_switch_to_rx(void);

/**
 * @brief Turns monitor (promiscuous) mode on or off.
 *
 * @param[in] mode The desired mode:
 *                 1 for monitor (promiscuous) mode;
 *                 0 for normal (auto address-decoding) mode.
 */
void cc2420_set_monitor(int mode);

/**
 * @brief Indicate if the CC2420 is in monitor (promiscuous) mode.
 *
 * @return 1 if the transceiver is in monitor (promiscuous) mode;
 *         0 if it is in normal (auto address-decoding) mode.
 */
int cc2420_get_monitor(void);

/**
 * @brief Set the channel of the CC2420.
 *
 * @param[in] chan The desired channel, valid channels are from 11 to 26.
 *
 * @return The tuned channel after calling, or -1 on error.
 */
int cc2420_set_channel(unsigned int chan);

/**
 * @brief Get the channel of the CC2420.
 *
 * @return The tuned channel.
 */
unsigned int cc2420_get_channel(void);

/**
 * @brief Sets the short address of the CC2420.
 *
 * @param[in] addr The desired address.
 *
 * @return The set address after calling.
 */
uint16_t cc2420_set_address(uint16_t addr);

/**
 * @brief Gets the current short address of the CC2420.
 *
 * @return The current short address.
 */
uint16_t cc2420_get_address(void);

/**
 * @brief Sets the IEEE long address of the CC2420.
 *
 * @param[in] addr The desired address.
 *
 * @return The set address after calling.
 */
uint64_t cc2420_set_address_long(uint64_t addr);

/**
 * @brief Gets the current IEEE long address of the CC2420.
 *
 * @return The current IEEE long address.
 */
uint64_t cc2420_get_address_long(void);

/**
 * @brief Sets the pan ID of the CC2420.
 *
 * @param[in] pan The desired pan ID.
 *
 * @return The set pan ID after calling.
 */
uint16_t cc2420_set_pan(uint16_t pan);

/**
 * @brief Gets the current IEEE long address of the CC2420.
 *
 * @return The current IEEE long address.
 */
uint16_t cc2420_get_pan(void);

/**
 * @brief Sets the output (TX) power of the CC2420.
 *
 * @param[in] pow The desired TX (output) power in dBm,
 *                 valid values are -25 to 0; other values
 *                 will be "saturated" into this range.
 *
 * @return The set TX (output) power after calling.
 */
int cc2420_set_tx_power(int pow);

/**
 * @brief Gets the current output (TX) power of the CC2420.
 *
 * @return The current TX (output) power.
 */
int cc2420_get_tx_power(void);

/**
 * @brief Checks if the radio medium is available/clear to send
 *         ("Clear Channel Assessment" a.k.a. CCA).
 *
 * @return a 1 value if radio medium is clear (available),
 *         a 0 value otherwise.
 *
 */
int cc2420_channel_clear(netdev_t *dev);

/**
 * @brief Interrupt handler, gets fired when a RX overflow happens.
 *
 */
void cc2420_rxoverflow_irq(void);

/**
 * @brief Interrupt handler, gets fired when bytes in the RX FIFO are present.
 *
 */
void cc2420_rx_irq(void);

/**
 * @brief Sets the function called back when a packet is received.
 *        (Low-level mechanism, parallel to the `transceiver` module).
 *
 * @param[in] recv_cb callback function for 802.15.4 packet arrival
 *
 * @return  0 on success
 * @return  -ENODEV if *dev* is not recognized
 * @return  -ENOBUFS, if maximum number of registable callbacks is exceeded
 */
int cc2420_add_raw_recv_callback(netdev_t *dev,
                                 netdev_802154_raw_packet_cb_t recv_cb);

/**
 * @brief Unsets the function called back when a packet is received.
 *        (Low-level mechanism, parallel to the `transceiver` module).
 *
 * @param[in] recv_cb callback function to unset
 *
 * @return  0 on success
 * @return  -ENODEV if *dev* is not recognized
 */
int cc2420_rem_raw_recv_callback(netdev_t *dev,
                                 netdev_802154_raw_packet_cb_t recv_cb);

/**
 * @brief Sets a function called back when a data packet is received.
 *
 * @param[in] recv_cb callback function for 802.15.4 data packet arrival
 *
 * @return  0 on success
 * @return  -ENODEV if *dev* is not recognized
 * @return  -ENOBUFS, if maximum number of registable callbacks is exceeded
 */
int cc2420_add_data_recv_callback(netdev_t *dev,
                                  netdev_rcv_data_cb_t recv_cb);

/**
 * @brief Unsets a function called back when a data packet is received.
 *
 * @param[in] recv_cb callback function to unset
 *
 * @return  0 on success
 * @return  -ENODEV if *dev* is not recognized
 */
int cc2420_rem_data_recv_callback(netdev_t *dev,
                                  netdev_rcv_data_cb_t recv_cb);

/**
 * @brief RX handler, process data from the RX FIFO.
 *
 */
void cc2420_rx_handler(void);

/**
 * @brief Prepare the CC2420 TX buffer to send with the given packet.
 *
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
netdev_802154_tx_status_t cc2420_load_tx_buf(netdev_t *dev,
                                             netdev_802154_pkt_kind_t kind,
                                             netdev_802154_node_addr_t *dest,
                                             int use_long_addr,
                                             int wants_ack,
                                             netdev_hlist_t *upper_layer_hdrs,
                                             void *buf,
                                             unsigned int len);

/**
 * @brief Transmit the data loaded into the CC2420 TX buffer.
 *
 * @return @ref netdev_802154_tx_status_t
 */
netdev_802154_tx_status_t cc2420_transmit_tx_buf(netdev_t *dev);

/**
 * @brief Send function, sends a cc2420_packet_t over the air.
 *
 * @param[in] *packet The Packet which will be send.
 *
 * @return The count of bytes which are send or -1 on error
 *
 */
int16_t cc2420_send(cc2420_packet_t *packet);

/*
 * RX Packet Buffer, read from the transceiver, filled by the cc2420_rx_handler.
 */
extern cc2420_packet_t cc2420_rx_buffer[CC2420_RX_BUF_SIZE];


/** Utility macro: get CC2420's status byte */
#define cc2420_status_byte() cc2420_strobe(NOBYTE)

/**
 * CC2420 low-level radio driver definition.
 */
extern const netdev_802154_driver_t cc2420_driver;

#ifdef __cplusplus
}
#endif

#endif
