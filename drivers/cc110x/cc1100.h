/*
 * Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * These sources were developed at the Freie Universitaet Berlin, Computer Systems
 * and Telematics group (http://cst.mi.fu-berlin.de).
 * ----------------------------------------------------------------------------
 * This file is part of RIOT.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
*******************************************************************************/

/**
 * @defgroup        drivers_cc110x CC110x
 * @ingroup         drivers
 * @brief           Driver for Texas Instruments CC110x (including CMSA/CA MAC)
 *
 * <h3>Quick links</h3>
 * \li \ref cc1100_packet_layer0_t      MAC packet format
 *
 * @{
 *
 * @file
 * @brief       TI Chipcon CC110x radio driver
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 */

#ifndef CC1100_H
#define CC1100_H

#include <stdbool.h>
#include "cc110x.h"

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

/** CC1100 register configuration */
typedef struct cc1100_reg {
    uint8_t IOCFG2;
    uint8_t IOCFG1;
    uint8_t IOCFG0;
    uint8_t FIFOTHR;
    uint8_t SYNC1;
    uint8_t SYNC0;
    uint8_t PKTLEN;
    uint8_t PKTCTRL1;
    uint8_t PKTCTRL0;
    uint8_t ADDR;
    uint8_t CHANNR;
    uint8_t FSCTRL1;
    uint8_t FSCTRL0;
    uint8_t FREQ2;
    uint8_t FREQ1;
    uint8_t FREQ0;
    uint8_t MDMCFG4;
    uint8_t MDMCFG3;
    uint8_t MDMCFG2;
    uint8_t MDMCFG1;
    uint8_t MDMCFG0;
    uint8_t DEVIATN;
    uint8_t MCSM2;
    uint8_t MCSM1;
    uint8_t MCSM0;
    uint8_t FOCCFG;
    uint8_t BSCFG;
    uint8_t AGCCTRL2;
    uint8_t AGCCTRL1;
    uint8_t AGCCTRL0;
    uint8_t WOREVT1;
    uint8_t WOREVT0;
    uint8_t WORCTRL;
    uint8_t FREND1;
    uint8_t FREND0;
    uint8_t FSCAL3;
    uint8_t FSCAL2;
    uint8_t FSCAL1;
    uint8_t FSCAL0;
} cc1100_reg_t;

/** CC1100 radio configuration */
typedef struct cc1100_cfg_t {
    cc1100_reg_t reg_cfg;       ///< CC1100 register configuration
    uint8_t pa_power;           ///< Output power setting
} cc1100_cfg_t;

/**
 * @brief   Radio Control Flags
 */
typedef struct cc1100_flags {
    uint32_t TOF;               ///< Time of flight of the last packet and last ACK
    uint32_t TCP;               ///< Time to compute packet
    unsigned RPS        : 16;   ///< Raw packets sent to transmit last packet
    unsigned RETC       : 8;    ///< Retransmission count of last send packet
    unsigned RSSI       : 8;    ///< The RSSI value of last received packet
    unsigned RSSI_SEND  : 8;    ///< The RSSI value of the last send unicast packet of this node
    unsigned LQI        : 8;    ///< The LQI value of the last received packet
    unsigned LL_ACK     : 1;    ///< Is set if Link-Level ACK is received, otherwise 0 (reset on new burst)
    unsigned CAA        : 1;    ///< The status of the air (1 = air free, 0 = air not free)
    unsigned CRC_STATE  : 1;    ///< The CRC status of last received packet (1 = OK, 0 = not OK)
    unsigned SEQ        : 1;    ///< Sequence number (toggles between 0 and 1)
    unsigned MAN_WOR    : 1;    ///< Manual WOR set (for randomized WOR times => no synch)
    unsigned KT_RES_ERR : 1;    ///< A hwtimer resource error has occurred (no free timers available)
    unsigned TX         : 1;    ///< State machine TX lock, only ACKs will be received
    unsigned WOR_RST    : 1;    ///< Reset CC1100 real time clock (WOR) on next WOR strobe
} cc1100_flags;

/**
 * @brief   Statistic interface for debugging
 */
typedef struct cc1100_statistic {
    uint32_t    packets_in;
    uint32_t    packets_in_crc_fail;
    uint32_t    packets_in_while_tx;
    uint32_t    packets_in_dups;
    uint32_t    packets_in_up;
    uint32_t    packets_out;
    uint32_t    packets_out_acked;
    uint32_t    packets_out_broadcast;
    uint32_t    raw_packets_out;
    uint32_t    raw_packets_out_acked;
    uint32_t    acks_send;
    uint32_t    rx_buffer_max;
    uint32_t    watch_dog_resets;
} cc1100_statistic_t;

enum radio_mode {
    RADIO_MODE_GET  = -1,       ///< leave mode unchanged
    RADIO_MODE_OFF  = 0,        ///< turn radio off
    RADIO_MODE_ON   = 1         ///< turn radio on
};

enum radio_result {
    RADIO_PAYLOAD_TOO_LONG      = -1,   ///< payload too long
    RADIO_WRONG_MODE            = -2,   ///< operation not supported in current mode
    RADIO_ADDR_OUT_OF_RANGE     = -3,   ///< address out of range
    RADIO_OP_FAILED             = -4,   ///< operation failed
    RADIO_CS_TIMEOUT            = -5,   ///< Carrier Sense timeout: air was never free
    RADIO_INVALID_PARAM         = -6    ///< Invalid parameters passed to radio
};

/* ------------------------------------------------------------------------- */
//              Variable declarations (also used in other files)
/* ------------------------------------------------------------------------- */

extern volatile cc1100_flags rflags;    ///< Radio flags

extern volatile uint8_t radio_mode;     ///< Radio mode
extern volatile uint8_t radio_state;    ///< Radio state

/** Mode callback functions */
typedef void (*cc1100_mode_callback_t)(void);
extern volatile cc1100_mode_callback_t cc1100_go_idle;
extern volatile cc1100_mode_callback_t cc1100_go_receive;
extern volatile cc1100_mode_callback_t cc1100_go_after_tx;
extern volatile cc1100_mode_callback_t cc1100_setup_mode;

extern volatile int wor_hwtimer_id;     ///< WOR hwtimer identifier

/* ------------------------------------------------------------------------- */
//              CC1100 radio driver API
/* ------------------------------------------------------------------------- */

/**
 * @brief   Set chip and state machine to IDLE mode.
 */
void cc1100_set_idle(void);

/**
 * @brief   Convert radio mode to textual representation.
 *
 * @param   mode The radio mode to convert.
 *
 * @return  Textual representation of radio mode.
 */
char *cc1100_mode_to_text(uint8_t mode);

/**
 * @brief   Convert radio state to textual representation.
 *
 * @param   mode The radio state to convert.
 *
 * @return  Textual representation of radio state.
 */
char *cc1100_state_to_text(uint8_t state);

/**
 * @brief   Convert current output power to textual representation.
 *
 * @return  Textual representation of current output power in dBm.
 */
char *cc1100_get_output_power(char *buf);

/**
 * @brief   Read out main radio control FSM state.
 *
 * @return  Textual representation of current main radio control FSM state.
 */
char *cc1100_get_marc_state(void);

/**
 * @brief   hwtimer wrapper function.
 *
 * This wrapper function puts the radio to receive mode.
 *
 * @param   ptr Optional data (only to match hwtimer interface).
 */
void cc1100_hwtimer_go_receive_wrapper(void *ptr);

/**
 * @brief   GDO0 interrupt handler.
 */
void cc1100_gdo0_irq(void);

/**
 * @brief   GDO2 interrupt handler.
 *
 * @note    Wakes up MCU on packet reception.
 */
void cc1100_gdo2_irq(void);

/**
 * @brief   Transfer packet from CC1100 RX FIFO.
 *
 * Transfers a packet from CC1100 RX FIFO into a buffer.
 *
 * @param   rxBuffer The buffer in which to transfer the packet.
 * @param   length The size of the buffer in which to transfer the packet.
 *
 * @return  true if operation succeeded; false otherwise (e.g. no data
 *          available, buffer to small or CRC check failed).
 */
bool cc1100_spi_receive_packet(uint8_t *rxBuffer, radio_packet_length_t length);

/**
 * @brief   Sends raw data.
 *
 * Sends the data of the given buffer. The first two bytes of the
 * buffer must match the CC1100 packet format (so address interpretation
 * succeeds).
 * <p>
 * This function is not mode safe!The radio must be woke up before
 * sending and has to be put back manually to receive mode.
 *
 * @param   tx_buffer Data source buffer.
 * @param   size The size of the data source buffer (maximum: 62 bytes).
 */
void cc1100_send_raw(uint8_t *tx_buffer, uint8_t size);

/**
 * @name Carrier Sense interface
 * @{
 */

/**
 * @brief   Initialize radio for carrier sense detection.
 */
void cc1100_cs_init(void);

/**
 * @brief   Enable or disable carrier sense detections.
 *
 * Turns interrupts for carrier sense on or off.
 *
 * @param   enabled true if carrier sense detection should
 *                  be enabled; false otherwise.
 */
void cc1100_cs_set_enabled(bool enabled);

/**
 * @brief   Read carrier sense signal.
 *
 * @return  High (1) if air is not free; low (0) if air is
 *          currently free.
 */
int cc1100_cs_read(void);

/**
 * @brief   Reads the CCA (Clear Channel Assessment) flag.
 *
 * The CCA flag is set by an interrupt service routine, after
 * carrier sense detection is enabled. So the status of the
 * carrier sense signal can be evaluated in a non blocking
 * manner.
 *
 * @return  The current value of the CCA flag (High: air is free,
 *          low: air is not free).
 */
int cc1100_cs_read_cca(void);

/**
 * @brief   Sets the CCA flag.
 *
 * @param   cca The new value for the CCA flag.
 */
void cc1100_cs_write_cca(const int cca);

/** @} */

/**
 * @name    Statistic interface
 * @{
 */

/**
 * @brief   Reset radio statistics.
 */
void cc1100_reset_statistic(void);

/** @} */

/** @} */
#endif
