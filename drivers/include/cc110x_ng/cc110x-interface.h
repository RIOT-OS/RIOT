/*
 * Copyright (C) 2009 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_cc110x_ng CC110x_ng
 * @brief       Driver for Texas Instruments CC110x (without MAC protocol)
 * @ingroup     drivers
 * @{
 *
 * @file        cc110x_ng.h
 * @brief       Data structures and variables for the cc110x driver interface
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef CC110X_NG_INTERFACE_H
#define CC110X_NG_INTERFACE_H

#include <stdint.h>
#include "radio/radio.h"
#include "radio/types.h"
#include "cc110x-config.h"

#define CC1100_MAX_DATA_LENGTH (58)

#define CC1100_HEADER_LENGTH   (3)             ///< Header covers SRC, DST and FLAGS

#define CC1100_BROADCAST_ADDRESS (0x00) ///< CC1100 broadcast address

#define MAX_UID                  (0xFF) ///< Maximum UID of a node is 255
#define MIN_UID                  (0x01) ///< Minimum UID of a node is 1

#define MIN_CHANNR                  (0) ///< Minimum channel number
#define MAX_CHANNR                 (24) ///< Maximum channel number

#define MIN_OUTPUT_POWER            (0) ///< Minimum output power value
#define MAX_OUTPUT_POWER           (11) ///< Maximum output power value

#define PACKET_LENGTH               (0x3E)      ///< Packet length = 62 Bytes.
#define CC1100_SYNC_WORD_TX_TIME   (90000)      // loop count (max. timeout ~ 15 ms) to wait for
                                                // sync word to be transmitted (GDO2 from low to high)
/**
 * @name    Defines used as state values for state machine
 * @{
 */
#define RADIO_UNKNOWN           (0)
#define RADIO_AIR_FREE_WAITING  (1)
#define RADIO_WOR               (2)
#define RADIO_IDLE              (3)
#define RADIO_SEND_BURST        (4)
#define RADIO_RX                (5)
#define RADIO_SEND_ACK          (6)
#define RADIO_PWD               (7)

/** @} */

extern volatile cc110x_flags rflags;    ///< Radio flags
extern char cc110x_conf[];

/**
 * @brief   CC1100 layer 0 protocol
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
        7:4 | -
        3:1 | Protocol
          0 | Identification
</pre>
Notes:
\li length & address are given by CC1100
\li Identification is increased is used to scan duplicates. It must be increased
    for each new packet and kept for packet retransmissions.
 */
typedef struct __attribute__((packed))
{
    uint8_t length;                 ///< Length of the packet (without length byte)
    uint8_t address;                ///< Destination address
    uint8_t phy_src;                ///< Source address (physical source)
    uint8_t flags;                  ///< Flags
    uint8_t data[CC1100_MAX_DATA_LENGTH];   ///< Data (high layer protocol)
}
cc110x_packet_t;

typedef struct {
    uint8_t rssi;
    uint8_t lqi;
#ifdef MODULE_GTIMER
    gtimer_timeval_t toa;
#endif
    cc110x_packet_t packet;
} rx_buffer_t;

enum radio_mode {
    RADIO_MODE_GET  = -1,       ///< leave mode unchanged
    RADIO_MODE_OFF  = 0,        ///< turn radio off
    RADIO_MODE_ON   = 1         ///< turn radio on
};

extern rx_buffer_t cc110x_rx_buffer[];

extern volatile uint8_t rx_buffer_next;     ///< Next packet in RX queue

extern volatile uint8_t radio_state;        ///< Radio state
extern cc110x_statistic_t cc110x_statistic;

extern int transceiver_pid;                 ///< the transceiver thread pid

void cc110x_init(int transceiver_pid);

void cc110x_rx_handler(void);

int8_t cc110x_send(cc110x_packet_t *pkt);

uint8_t cc110x_get_buffer_pos(void);

void cc110x_setup_rx_mode(void);
void cc110x_switch_to_rx(void);
void cc110x_wakeup_from_rx(void);
void cc110x_switch_to_pwd(void);

void cc110x_disable_interrupts(void);
int16_t cc110x_set_config_channel(uint8_t channr);
int16_t cc110x_set_channel(uint8_t channr);
int16_t cc110x_get_channel(void);

radio_address_t cc110x_set_address(radio_address_t addr);
radio_address_t cc110x_set_config_address(radio_address_t addr);
radio_address_t cc110x_get_address(void);
void cc110x_set_monitor(uint8_t mode);

void cc110x_print_config(void);

/**
 * @brief   GDO0 interrupt handler.
 */
void cc110x_gdo0_irq(void);

/**
 * @brief   GDO2 interrupt handler.
 *
 * @note    Wakes up MCU on packet reception.
 */
void cc110x_gdo2_irq(void);

#ifdef DBG_IGNORE
/**
 * @brief   Initialize ignore function
 */
void cc110x_init_ignore(void);

/**
 * @brief       Adds a address to the ignore list
 *
 * @param addr  The physical address to be ignored
 *
 * @return      0 if list is full, 1 otherwise
 *
 */
uint8_t cc110x_add_ignored(radio_address_t addr);
#endif

/** @} */
#endif /* CC110X_NG_INTERFACE_H */
