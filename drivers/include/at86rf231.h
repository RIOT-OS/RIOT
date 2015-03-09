/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_at86rf231 at86rf231
 * @ingroup     drivers
 * @brief       Device driver for the Atmel AT86RF231 radio
 * @{
 *
 * @file
 * @brief       Interface definition for the AT86RF231 device driver
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#ifndef AT86RF231_H_
#define AT86RF231_H_

#include <stdio.h>
#include <stdint.h>

#include "kernel_types.h"
#include "board.h"
#include "radio/types.h"
#include "ieee802154_frame.h"
#include "at86rf231/at86rf231_settings.h"
#include "periph/gpio.h"
#include "netdev/802154.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum length of a frame on at86rf231
 */
#define AT86RF231_MAX_PKT_LENGTH    (127)

/**
 * @brief Maximum payload length
 * @details Assuming intra PAN short address mode
 *          results in 2 bytes FCF
 *          + 1 bytes SEQNr
 *          + 2 bytes PAN Id
 *          + 2 bytes destination address
 *          + 2 bytes source address
 */
#define AT86RF231_MAX_DATA_LENGTH   (118)

/**
 * @brief Broadcast address
 */
#define AT86RF231_BROADCAST_ADDRESS (0xFFFF)

/**
 * @brief at86rf231's minimum RSSI detection level in dBm
 */
#define AT86RF231_RSSI_BASE_VAL -94

/**
 * @brief at86rf231's minimum TX power in dBm
 */
#define AT86RF231_TX_POWER_MIN  -17

/**
 * @brief at86rf231's maximum TX power in dBm
 */
#define AT86RF231_TX_POWER_MAX  4

/**
 * @brief at86rf231's lowest supported channel
 */
#define AT86RF231_MIN_CHANNEL       (11)

/**
 * @brief at86rf231's highest supported channel
 */
#define AT86RF231_MAX_CHANNEL       (26)

/**
 *  @brief Structure to represent a at86rf231 packet.
 */
typedef struct __attribute__((packed))
{
    /** @{ */
    uint8_t length;             /**< the length of the frame of the frame including fcs*/
    ieee802154_frame_t frame;   /**< the ieee802154 frame */
    uint8_t rssi;               /**< the rssi value */
    uint8_t crc;                /**< 1 if crc was successfull, 0 otherwise */
    uint8_t lqi;                /**< the link quality indicator */
    /** @} */
} at86rf231_packet_t;

extern netdev_t at86rf231_netdev;   /**< netdev representation of this driver */

/**
 * @brief States to be assigned to `driver_state`
 * @{
 */
#define AT_DRIVER_STATE_DEFAULT     (0)
#define AT_DRIVER_STATE_SENDING     (1)
/** @} */

/**
 * @brief To keep state inside of at86rf231 driver
 * @details This variable is used to determine if a TRX_END IRQ from
 *          the radio transceiver has to be interpreted as end of
 *          sending or reception.
 */
extern uint8_t driver_state;

/**
 * @brief Initialize the at86rf231 transceiver
 */
int at86rf231_initialize(netdev_t *dev);

#ifdef MODULE_TRANSCEIVER
/**
 * @brief Init the at86rf231 for use with RIOT's transceiver module.
 *
 * @param[in] tpid The PID of the transceiver thread.
 */

void at86rf231_init(kernel_pid_t tpid);
#endif

/**
 * @brief Turn at86rf231 on.
 *
 * @return 1 if the radio was correctly turned on; 0 otherwise.
 */
int at86rf231_on(void);

/**
 * @brief Turn at86rf231 off.
 */
void at86rf231_off(void);

/**
 * @brief Indicate if the at86rf231 is on.
 *
 * @return 1 if the radio transceiver is on (active); 0 otherwise.
 */
int at86rf231_is_on(void);

/**
 * @brief Switches the at86rf231 into receive mode.
 */
void at86rf231_switch_to_rx(void);

/**
 * @brief Turns monitor (promiscuous) mode on or off.
 *
 * @param[in] mode The desired mode:
 *                 1 for monitor (promiscuous) mode;
 *                 0 for normal (auto address-decoding) mode.
 */
void at86rf231_set_monitor(int mode);

/**
 * @brief Indicate if the at86rf231 is in monitor (promiscuous) mode.
 *
 * @return 1 if the transceiver is in monitor (promiscuous) mode;
 *         0 if it is in normal (auto address-decoding) mode.
 */
int at86rf231_get_monitor(void);

/**
 * @brief Set the channel of the at86rf231.
 *
 * @param[in] chan The desired channel, valid channels are from 11 to 26.
 *
 * @return The tuned channel after calling, or -1 on error.
 */
int at86rf231_set_channel(unsigned int chan);

/**
 * @brief Get the channel of the at86rf231.
 *
 * @return The tuned channel.
 */
unsigned int at86rf231_get_channel(void);

/**
 * @brief Sets the short address of the at86rf231.
 *
 * @param[in] addr The desired address.
 *
 * @return The set address after calling.
 */
uint16_t at86rf231_set_address(uint16_t addr);

/**
 * @brief Gets the current short address of the at86rf231.
 *
 * @return The current short address.
 */
uint16_t at86rf231_get_address(void);

/**
 * @brief Sets the IEEE long address of the at86rf231.
 *
 * @param[in] addr The desired address.
 *
 * @return The set address after calling.
 */
uint64_t at86rf231_set_address_long(uint64_t addr);

/**
 * @brief Gets the current IEEE long address of the at86rf231.
 *
 * @return The current IEEE long address.
 */
uint64_t at86rf231_get_address_long(void);

/**
 * @brief Sets the pan ID of the at86rf231.
 *
 * @param[in] pan The desired pan ID.
 *
 * @return The set pan ID after calling.
 */
uint16_t at86rf231_set_pan(uint16_t pan);

/**
 * @brief Gets the current IEEE long address of the at86rf231.
 *
 * @return The current IEEE long address.
 */
uint16_t at86rf231_get_pan(void);

/**
 * @brief Sets the output (TX) power of the at86rf231.
 *
 * @param[in] pow The desired TX (output) power in dBm,
 *                 valid values are -17 to 4; other values
 *                 will be "saturated" into this range.
 *
 * @return The set TX (output) power after calling.
 */
int at86rf231_set_tx_power(int pow);

/**
 * @brief Gets the current output (TX) power of the at86rf231.
 *
 * @return The current TX (output) power.
 */
int at86rf231_get_tx_power(void);

/**
 * @brief Resets the counter of detected SFDs.
 */
void at86rf231_reset_sfd_count(void);

/**
 * @brief Returns the number of SFDs detected by the at86rf231.
 *
 * @return The current SFD count.
 */
unsigned long at86rf231_get_sfd_count(void);

/**
 * @brief Sets the threshold value for CCA detection.
 *
 * @param[in] pow The desired threshold value (in dBm)
 *                 for determining if radio channel is
 *                 busy or not. Valid values are -91 to
 *                 -61; other values will be "saturated"
 *                 into this range.
 *
 * @return The set threshold value after calling.
 */
int at86rf231_set_cca_threshold(int pow);

/**
 * @brief Returns the threshold value for CCA detection.
 *
 * @return The current threshold value.
 */
int at86rf231_get_cca_threshold(void);

/**
 * @brief Checks if the radio medium is available/clear to send
 *         ("Clear Channel Assessment" a.k.a. CCA).
 *
 * @return a 1 value if radio medium is clear (available),
 *         a 0 value otherwise.
 *
 */
int at86rf231_channel_is_clear(netdev_t *dev);

/**
 * @brief Interrupt handler, gets fired when a RX overflow happens.
 *
 */
void at86rf231_rxoverflow_irq(void);

/**
 * @brief Interrupt handler, gets fired when bytes in the RX FIFO are present.
 *
 */
void at86rf231_rx_irq(void);

/**
 * @brief Sets the function called back when a packet is received.
 *        (Low-level mechanism, parallel to the `transceiver` module).
 *
 * @param[in] dev     The network device to operate on. (Currently not used)
 * @param[in] recv_cb callback function for 802.15.4 packet arrival
 *
 * @return  0 on success
 * @return  -ENODEV if *dev* is not recognized
 * @return  -ENOBUFS, if maximum number of registable callbacks is exceeded
 */
int at86rf231_add_raw_recv_callback(netdev_t *dev,
                                 netdev_802154_raw_packet_cb_t recv_cb);

/**
 * @brief Unsets the function called back when a packet is received.
 *        (Low-level mechanism, parallel to the `transceiver` module).
 *
 * @param[in] dev     The network device to operate on. (Currently not used)
 * @param[in] recv_cb callback function to unset
 *
 * @return  0 on success
 * @return  -ENODEV if *dev* is not recognized
 * @return  -ENOBUFS, if maximum number of registable callbacks is exceeded
 */
int at86rf231_rem_raw_recv_callback(netdev_t *dev,
                                 netdev_802154_raw_packet_cb_t recv_cb);

/**
 * @brief Sets a function called back when a data packet is received.
 *
 * @param[in] dev     The network device to operate on. (Currently not used)
 * @param[in] recv_cb callback function for 802.15.4 data packet arrival
 *
 * @return  0 on success
 * @return  -ENODEV if *dev* is not recognized
 * @return  -ENOBUFS, if maximum number of registable callbacks is exceeded
 */
int at86rf231_add_data_recv_callback(netdev_t *dev,
                                  netdev_rcv_data_cb_t recv_cb);

/**
 * @brief Unsets a function called back when a data packet is received.
 *
 * @param[in] dev     The network device to operate on. (Currently not used)
 * @param[in] recv_cb callback function to unset
 *
 * @return  0 on success
 * @return  -ENODEV if *dev* is not recognized
 * @return  -ENOBUFS, if maximum number of registable callbacks is exceeded
 */
int at86rf231_rem_data_recv_callback(netdev_t *dev,
                                  netdev_rcv_data_cb_t recv_cb);

/**
 * @brief RX handler, process data from the RX FIFO.
 *
 */
void at86rf231_rx_handler(void);

/**
 * @brief Prepare the at86rf231 TX buffer to send with the given packet.
 *
 * @param[in] dev  The network device to operate on. (Currently not used)
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
netdev_802154_tx_status_t at86rf231_load_tx_buf(netdev_t *dev,
                                             netdev_802154_pkt_kind_t kind,
                                             netdev_802154_node_addr_t *dest,
                                             int use_long_addr,
                                             int wants_ack,
                                             netdev_hlist_t *upper_layer_hdrs,
                                             void *buf,
                                             unsigned int len);

/**
 * @brief Transmit the data loaded into the at86rf231 TX buffer.
 *
 * @param[in] dev The network device to operate on. (Currently not used)
 *
 * @return @ref netdev_802154_tx_status_t
 */
netdev_802154_tx_status_t at86rf231_transmit_tx_buf(netdev_t *dev);

/**
 * @brief Send function, sends a at86rf231_packet_t over the air.
 *
 * @param[in] *packet The Packet which will be send.
 *
 * @return The count of bytes which are send or -1 on error
 *
 */
int16_t at86rf231_send(at86rf231_packet_t *packet);

/**
 * RX Packet Buffer, read from the transceiver, filled by the at86rf231_rx_handler.
 */
extern at86rf231_packet_t at86rf231_rx_buffer[AT86RF231_RX_BUF_SIZE];

/**
 * Get at86rf231's status byte
 */
uint8_t at86rf231_get_status(void);

/**
 * Get at86rf231's TRAC status byte
 */
uint8_t at86rf231_get_trac_status(void);

/**
 * at86rf231 low-level radio driver definition.
 */
extern const netdev_802154_driver_t at86rf231_driver;

#ifdef __cplusplus
}
#endif

#endif /* AT86RF231_H_ */
/** @} */
