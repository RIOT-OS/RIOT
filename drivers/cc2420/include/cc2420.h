/**
  * cc2420.h - Definitions for CC2420 functions.
  * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
  *
  * This source code is licensed under the GNU Lesser General Public License,
  * Version 2.  See the file LICENSE for more details.
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
		0-2	| Frame Type
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
*/

#ifndef CC2420_H
#define CC2420_H

#include <ieee802154_frame.h>
#include <cc2420_settings.h>

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
    uint8_t length;  			/** < the length of the frame of the frame including fcs*/
    ieee802154_frame_t frame;   /** < the ieee802154 frame */
    int8_t rssi;                /** < the rssi value */
    uint8_t crc;                /** < 1 if crc was successfull, 0 otherwise */
    uint8_t lqi;                /** < the link quality indicator */
    /* @} */
} cc2420_packet_t;

/**
 * @brief Init the cc2420.
 *
 * @param[in] tpid The PID of the transceiver thread.
 */

void cc2420_init(int tpid);

/**
 * @brief Turns monitor mode on off.
 *
 * @param[in] mode The desired mode, 1 for on; 0 for off.
 */
void cc2420_set_monitor(uint8_t mode);

/**
 * @brief Switchs the cc2420 into receive mode.
 *
 */
void cc2420_switch_to_rx(void);

/**
 * @brief Set the channel of the cc2420.
 *
 * @param[in] chan The desired channel, valid channels are from 11 to 26.
 *
 * @return The tuned channel after calling, or -1 on error.
 */
int16_t cc2420_set_channel(uint16_t chan);

/**
 * @brief Get the channel of the cc2420.
 *
 * @return The tuned channel.
 */
uint16_t cc2420_get_channel(void);

/**
 * @brief Sets the short address of the cc2420.
 *
 * @param[in] addr The desired address.
 *
 * @return The set address after calling.
 */
uint16_t cc2420_set_address(uint16_t addr);

/**
 * @brief Gets the current short address of the cc2420.
 *
 * @return The current short address.
 *
 */
uint16_t cc2420_get_address(void);

/**
 * @brief Sets the IEEE long address of the cc2420.
 *
 * @param[in] addr The desired address.
 *
 * @return The set address after calling.
 */
uint64_t cc2420_set_address_long(uint64_t addr);

/**
 * @brief Gets the current IEEE long address of the cc2420.
 *
 * @return The current IEEE long address.
 *
 */
uint64_t cc2420_get_address_long(void);

/**
 * @brief Sets the pan ID of the cc2420.
 *
 * @param[in] pan The desired pan ID.
 *
 * @return The set pan ID after calling.
 */
uint16_t cc2420_set_pan(uint16_t pan);

/**
 * @brief Gets the current IEEE long address of the cc2420.
 *
 * @return The current IEEE long address.
 *
 */
uint16_t cc2420_get_pan(void);


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
 * @brief RX handler, process data from the RX FIFO.
 *
 */
void cc2420_rx_handler(void);

/**
 * @brief Send function, sends a cc2420_packet_t over the air.
 *
 * @param[in] *packet The Packet which will be send.
 *
 * @return The count of bytes which are send or -1 on error
 *
 */
int16_t cc2420_send(cc2420_packet_t *packet);

/**
 * @brief Changes the byte order of the two fcf bytes in a buffer.
 *
 * @param[in] *buf The Packet to swap.
 *
 */
void cc2420_swap_fcf_bytes(uint8_t *buf);

/**
 * The PID of the transceiver thread.
 */
extern int transceiver_pid;

/*
 * RX Packet Buffer, read from the transceiver, filled by the cc2420_rx_handler.
 */
extern cc2420_packet_t cc2420_rx_buffer[CC2420_RX_BUF_SIZE];


#endif

