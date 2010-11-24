#ifndef CC1100_H
#define CC1100_H

#include <radio/radio.h>
#include <radio/types.h>
#include <stdint.h>
#include <cc1100-config.h>

#define CC1100_MAX_DATA_LENGTH (58)

#define	CC1100_BROADCAST_ADDRESS (0x00)	///< CC1100 broadcast address

#define MAX_UID					 (0xFF) ///< Maximum UID of a node is 255
#define MIN_UID					 (0x01) ///< Minimum UID of a node is 1

#define MIN_CHANNR					(0)	///< Minimum channel number
#define MAX_CHANNR				   (24)	///< Maximum channel number

#define MIN_OUTPUT_POWER			(0)	///< Minimum output power value
#define MAX_OUTPUT_POWER		   (11)	///< Maximum output power value

/**
 * @name	Defines used as state values for state machine
 * @{
 */
#define RADIO_UNKNOWN			(0)
#define RADIO_AIR_FREE_WAITING	(1)
#define RADIO_WOR				(2)
#define RADIO_IDLE				(3)
#define RADIO_SEND_BURST		(4)
#define RADIO_RX				(5)
#define RADIO_SEND_ACK			(6)
#define RADIO_PWD				(7)

/** @} */


extern volatile cc1100_flags rflags;	///< Radio flags
extern char cc1100_conf[];

/**
 * @brief	CC1100 layer 0 protocol
 *
 * <pre>
---------------------------------------------------
|        |         |         |       |            |
| Length | Address | PhySrc  | Flags |    Data    |
|        |         |         |       |            |
---------------------------------------------------
  1 byte   1 byte    1 byte   1 byte   <= 58 bytes

Flags:
		Bit | Meaning
		--------------------
		7:4	| -
		3:1 | Protocol
		  0 | Identification
</pre>
Notes:
\li length & address are given by CC1100
\li Identification is increased is used to scan duplicates. It must be increased
	for each new packet and kept for packet retransmissions.
 */
typedef struct __attribute__ ((packed)) {
	uint8_t length;					///< Length of the packet (without length byte)
	uint8_t address;				///< Destination address
	uint8_t phy_src;				///< Source address (physical source)
	uint8_t flags;					///< Flags
	uint8_t data[CC1100_MAX_DATA_LENGTH];	///< Data (high layer protocol)
} cc1100_packet_t;

typedef struct {
    uint8_t rssi;
    uint8_t lqi;
    cc1100_packet_t packet;
} rx_buffer_t;

enum radio_mode {
	RADIO_MODE_GET	= -1,		///< leave mode unchanged
	RADIO_MODE_OFF	= 0,		///< turn radio off
	RADIO_MODE_ON	= 1			///< turn radio on
};

extern rx_buffer_t cc1100_rx_buffer[]; 

void cc1100_init(int transceiver_pid);

void cc1100_rx_handler(void);

uint8_t cc1100_get_buffer_pos(void);

void cc1100_setup_rx_mode(void);
void cc1100_switch_to_rx(void);
void cc1100_wakeup_from_rx(void);
void cc1100_switch_to_pwd(void);

void cc1100_disable_interrupts(void);
uint8_t cc1100_set_channel(uint8_t channr);
#endif
