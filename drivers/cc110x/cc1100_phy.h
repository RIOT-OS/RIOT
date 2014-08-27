/*
 * Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     dev_cc110x
 * @{
 */

/**
 * @file
 * @internal
 * @brief       TI Chipcon CC110x physical radio driver
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @version     $Revision: 1285 $
 *
 * @note        $Id: cc1100_phy.h 1285 2009-08-27 13:22:42Z hillebra $
 */
#ifndef CC1100_PHY_H_
#define CC1100_PHY_H_

#include <stdbool.h>
#include <stdint.h>
//#include "cc1100.h"
#include "cc1100-internal.h"
#include "cc110x.h"

#define MAX_DATA_LENGTH     (0x3A)  ///< Maximum data length of layer 0 = 58 Bytes.

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
typedef struct __attribute__((packed)) cc1100_packet_layer0_t {
    radio_packet_length_t length;   ///< Length of the packet (without length byte)
    uint8_t length;                 ///< Length of the packet (without length byte)
    uint8_t address;                ///< Destination address
    uint8_t phy_src;                ///< Source address (physical source)
    uint8_t flags;                  ///< Flags
    uint8_t data[MAX_DATA_LENGTH];  ///< Data (high layer protocol)
} cc1100_packet_layer0_t;

typedef struct cc1100_wor_config_t {
    uint16_t rx_interval;   ///< RX polling interval in milliseconds
    float    rx_time_ms;    ///< WOR_RX_TIME in milliseconds
    uint8_t  rx_time_reg;   ///< WOR_RX_TIME (CC1100 "MCSM2.RX_TIME" register value)
    uint8_t  wor_evt_0;     ///< CC1100 WOREVT0 register value
    uint8_t  wor_evt_1;     ///< CC1100 WOREVT1 register value
    uint8_t  wor_ctrl;      ///< CC1100 WORCTRL register value
} cc1100_wor_config_t;

/*---------------------------------------------------------------------------*/
//                  CC1100 Wake-On-Radio configuration
/*---------------------------------------------------------------------------*/

extern uint16_t cc1100_burst_count;
extern uint8_t cc1100_retransmission_count_uc;
extern uint8_t cc1100_retransmission_count_bc;
extern cc1100_wor_config_t cc1100_wor_config;

/*---------------------------------------------------------------------------*/
//                  CC1100 physical radio driver API
/*---------------------------------------------------------------------------*/

/**
 * @brief   Initialize the physical radio layer.
 */
void cc1100_phy_init(void);

/**
 * Lock radio for exclusive access. If no lock could be gained,
 * the current process is blocked and waits for release of lock.
 *
 * This function can be used by MAC for locking the radio, the
 * ::cc1100_send function will automatically lock the radio.
 */
void cc1100_phy_mutex_lock(void);

/**
 * Unlock radio. Must always be called after ::cc1100_phy_mutex_lock.
 * If ::cc1100_send is called, it will automatically unlock the radio
 * at the end of transmission.
 */
void cc1100_phy_mutex_unlock(void);

/**
 * @brief   Calculate and store Wake-On-Radio settings.
 *
 * Calculates WOR settings for a given RX interval in
 * milliseconds and stores the values in global configuration.
 * <p>
 * Does not change settings if not applicable.
 *
 * @param   millis Desired RX interval in milliseconds [50..60000].
 *
 * @return  The burst count (number of packets in a burst transfer)
 *          or -1 if an error occurred (e.g. RX interval invalid).
 */
int cc1100_phy_calc_wor_settings(uint16_t millis);

/**
 * @brief   Handler function for incoming packets.
 *
 * This handler function must be called in the receive
 * interrupt service routine.
 */
void cc1100_phy_rx_handler(void);

/** @} */

#endif /* CC1100_PHY_H_ */
