/*
 * Copyright (C) 2018 Baptiste Cartier
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc430
 * @{
 *
 * @file
 * @brief       Data structures and variables for the cc430 driver interface, based on the cc110x driver interface
 *
 * @author      Baptiste Cartier
 */

#ifndef CC430_RF_INTERNAL_H
#define CC430_RF_INTERNAL_H

#include <stdint.h>
#include <msp430.h> // generic file for register name, I/O, ...
#include "net/gnrc/nettype.h"

#ifdef __cplusplus
extern "C" {
#endif
#define CC110X_RSSI_OFFSET          (74)

#define CC430_PKT_SIZE_LENGTH       (1)

#define CC430_FIFO_SIZE             (64)

#define CC430_HEADER_LENGTH         (3)                                                             /**< Header covers SRC, DST and
                                                                                                         FLAGS */
#define CC430_MAX_DATA_LENGTH       (CC430_FIFO_SIZE - CC430_HEADER_LENGTH - CC430_PKT_SIZE_LENGTH) /*Max data length = size of FIFO - length of header - 1 byte for packet size
                                                                                                     * Max size is based on FIFO size for now, some work should be done to allow up to 255 data length*/

#define CC430_MAX_PACKET_SIZE       (CC430_FIFO_SIZE - CC430_PKT_SIZE_LENGTH)

#define CC430_BROADCAST_ADDRESS     (0x00)  /**< CC110X broadcast address */

#define MIN_UID                     (0x01)  /**< Minimum UID of a node is
                                                 1 */
#define MAX_UID                     (0xFF)  /**< Maximum UID of a node is
                                                 255 */

#define MIN_CHANNR                  (0)     /**< Minimum channel number */
#define MAX_CHANNR                  (24)    /**< Maximum channel number */

#define CC110X_PACKET_LENGTH        (64)    /**< max packet length = 64 bytes for the moment */
#define CC110X_SYNC_WORD_TX_TIME    (90000) /**< loop count (max. timeout ~15ms)
                                                 to wait for sync word to be
                                                 transmitted (GDO2 from low to
                                                 high) */

#ifndef CC430_DEFAULT_CHANNEL
#define CC430_DEFAULT_CHANNEL      (0)  /**< The default channel number */
#endif
#define CC430_MIN_CHANNR           (0)  /**< lowest possible channel number */
#define CC430_MAX_CHANNR           (0)  /**< highest possible channel number */

#define CONF_REG_SIZE   (47)            /* There are 47 8-bit configuration registers */
#define cc430_radio_POWER_OUTPUT_MINUS_30DBM       (0x03)
#define cc430_radio_POWER_OUTPUT_MINUS_12DBM       (0x25)
#define cc430_radio_POWER_OUTPUT_MINUS_6DBM        (0x2D)
#define cc430_radio_POWER_OUTPUT_0DBM              (0x8D)
#define cc430_radio_POWER_OUTPUT_10DBM             (0xC3)
#define cc430_radio_POWER_OUTPUT_MAX               (0xC0)
#define cc430_radio_POWER_OUTPUT_DEFAULT_8_8DBM    (0xC6)

#define RADIOINSTRW_MINUS_30DBM              (0x7E03)
#define RADIOINSTRW_MINUS_12DBM              (0x7E25)
#define RADIOINSTRW_MINUS_6DBM               (0x7E2D)
#define RADIOINSTRW_0DBM                     (0x7E8D)
#define RADIOINSTRW_10DBM                    (0x7EC3)
#define RADIOINSTRW_MAX                      (0x7EC0)
#define RADIOINSTRW_DEFAULT_8_8DBM           (0x7EC6)

#define TX_TO_IDLE_TIME     (10)        // TX to IDLE, no calibration: ~1us   => 0.3us *10 = 3us
#define RX_TO_IDLE_TIME     (2)         // RX to IDLE, no calibration: ~0.1us => 0.3*2 = 0.6us
#define IDLE_TO_RX_TIME     (300)       // IDLE to RX, no calibration: 75.1us => 0.3*300 = 90us

/**
 * @name    State values for state machine
 * @{
 */
enum {
    RADIO_UNKNOWN,
    RADIO_IDLE,
    RADIO_TX_BUSY,
    RADIO_RX,
    RADIO_RX_BUSY,
    RADIO_PWD,
};
/** @} */

/**
 * @brief array holding cc110x register values
 */
extern char cc110x_conf[];

/**
 * @brief   CC110X layer 0 protocol
 *
 * <pre>
   ---------------------------------------------------
 |        |         |         |       |            |
 | Length | Address | PhySrc  | Flags |    Data    |
 |        |         |         |       |            |
   ---------------------------------------------------
   1 byte   1 byte    1 byte   1 byte   <= 60 bytes

   Flags:
        Bit | Meaning
        --------------------
        7:4 | -
        3:1 | Protocol
          0 | Identification
   </pre>
   Notes:
   \li length & address are given by CC110X
   \li Identification is increased is used to scan duplicates. It must be increased
    for each new packet and kept for packet retransmissions.
 */
typedef struct __attribute__((packed)){
    uint8_t length;                         /**< Length of the packet (without length byte) */
    uint8_t address;                        /**< Destination address */
    uint8_t phy_src;                        /**< Source address (physical source) */
    uint8_t flags;                          /**< Flags */
    uint8_t data[CC430_MAX_DATA_LENGTH];    /**< Data (high layer protocol) */
} cc110x_pkt_t;

/**
 * @brief struct holding cc110x packet + metadata
 */
typedef struct {
    uint8_t rssi;                           /**< RSSI value */
    uint8_t lqi;                            /**< link quality indicator */
    uint8_t pos;                            /**< I have no clue. */
    cc110x_pkt_t packet;                    /**< whole packet */
} cc110x_pkt_buf_t;

/**
 * @brief enum for holding cc110x radio on/off state */
enum cc110x_radio_mode {
    RADIO_MODE_GET  = -1,                   /**< leave mode unchanged */
    RADIO_MODE_OFF  = 0,                    /**< turn radio off */
    RADIO_MODE_ON   = 1                     /**< turn radio on */
};

/**
 * @brief enum for radio module FSM state */
enum cc110x_radio_state {
    SLEEP,
    IDLE,
    XOFF,
    MANCAL,
    FS_WAKEUP,
    CALIBRATE,
    SETTLING,
    RX,
    TXRX_SETTLING,
    RX_OVERFLOW,
    FSTXON,
    TX,
    RXTX_SETTLING,
    TX_UNDERFLOW,
    STATE_READ_ERROR
};

/**
 * @brief   CC110x register configuration
 */
typedef struct {
    uint8_t _IOCFG2;        /**< GDO2 output pin configuration */
    uint8_t _IOCFG1;        /**< GDO1 output pin configuration */
    uint8_t _IOCFG0;        /**< GDO0 output pin configuration */
    uint8_t _FIFOTHR;       /**< RX FIFO and TX FIFO thresholds */
    uint8_t _SYNC1;         /**< Sync word, high byte */
    uint8_t _SYNC0;         /**< Sync word, low byte */
    uint8_t _PKTLEN;        /**< Packet length */
    uint8_t _PKTCTRL1;      /**< Packet automation control */
    uint8_t _PKTCTRL0;      /**< Packet automation control */
    uint8_t _ADDR;          /**< Device address */
    uint8_t _CHANNR;        /**< Channel number */
    uint8_t _FSCTRL1;       /**< Frequency synthesizer control */
    uint8_t _FSCTRL0;       /**< Frequency synthesizer control */
    uint8_t _FREQ2;         /**< Frequency control word, high byte */
    uint8_t _FREQ1;         /**< Frequency control word, middle byte */
    uint8_t _FREQ0;         /**< Frequency control word, low byte */
    uint8_t _MDMCFG4;       /**< Modem configuration */
    uint8_t _MDMCFG3;       /**< Modem configuration */
    uint8_t _MDMCFG2;       /**< Modem configuration */
    uint8_t _MDMCFG1;       /**< Modem configuration */
    uint8_t _MDMCFG0;       /**< Modem configuration */
    uint8_t _DEVIATN;       /**< Modem deviation setting */
    uint8_t _MCSM2;         /**< Main Radio Control State Machine configuration */
    uint8_t _MCSM1;         /**< Main Radio Control State Machine configuration */
    uint8_t _MCSM0;         /**< Main Radio Control State Machine configuration */
    uint8_t _FOCCFG;        /**< Frequency Offset Compensation configuration */
    uint8_t _BSCFG;         /**< Bit Synchronization configuration */
    uint8_t _AGCCTRL2;      /**< AGC control */
    uint8_t _AGCCTRL1;      /**< AGC control */
    uint8_t _AGCCTRL0;      /**< AGC control */
    uint8_t _WOREVT1;       /**< High byte Event 0 timeout */
    uint8_t _WOREVT0;       /**< Low byte Event 0 timeout */
    uint8_t _WORCTRL;       /**< Wake On Radio control */
    uint8_t _FREND1;        /**< Front end RX configuration */
    uint8_t _FREND0;        /**< Front end TX configuration */
    uint8_t _FSCAL3;        /**< Frequency synthesizer calibration */
    uint8_t _FSCAL2;        /**< Frequency synthesizer calibration */
    uint8_t _FSCAL1;        /**< Frequency synthesizer calibration */
    uint8_t _FSCAL0;        /**< Frequency synthesizer calibration */
    uint8_t _RES1;          /**< Reserved, read as 0x00 */
    uint8_t _RES0;          /**< Reserved, read as 0x00 */
    uint8_t _FSTEST;        /**< Frequency synthesizer calibration control */
    uint8_t _PTEST;         /**< Production test */
    uint8_t _AGCTEST;       /**< AGC test */
    uint8_t _TEST2;         /**< Test 2 */
    uint8_t _TEST1;         /**< Test 1 */
    uint8_t _TEST0;         /**< Test 0 */
} cc110x_reg_t;

/**
 * @brief   CC110x radio configuration
 */
typedef struct {
    cc110x_reg_t reg_cfg;       /**< CC110X register configuration */
    uint8_t pa_power;           /**< Output power setting */
} cc110x_cfg_t;

/**
 * @brief   Radio Control Flags
 */
typedef struct {
    uint8_t _RSSI;              /**< The RSSI value of last received packet */
    uint8_t _LQI;               /**< The LQI value of the last received packet */
} cc110x_flags_t;

/**
 * @brief   Statistic interface for debugging
 */
typedef struct cc110x_statistic {
    uint32_t packets_in;                /**< total nr of packets received */
    uint32_t packets_in_crc_fail;       /**< dropped because of invalid crc */
    uint32_t packets_in_while_tx;       /**< receive while tx */
    uint32_t raw_packets_out;           /**< packets sent */
} cc110x_statistic_t;

/**
 * @brief   Forward declaration
 */
typedef struct cc110x cc110x_t;

/**
 * @brief   Struct for holding cc110x device state
 */
struct cc110x {

    cc110x_statistic_t cc110x_statistic;        /**< Statistic values for
                                                     debugging */

    uint8_t radio_state;                        /**< Radio state */
    uint8_t radio_channel;                      /**< current Radio channel */
    uint8_t radio_address;                      /**< current Radio address */

    cc110x_pkt_buf_t pkt_buf;                   /**< RX/TX buffer */
    void (*isr_cb)(cc110x_t *dev, void *arg);   /**< isr callback */
    void *isr_cb_arg;                           /**< isr callback argument */
#ifdef MODULE_GNRC_NETIF
    gnrc_nettype_t proto;                       /**< protocol the radio expects */
#endif
};

void cc430_radio_delay_rf(volatile uint32_t p);

uint8_t cc430_radio_strobe(uint8_t strobe);

uint8_t cc430_radio_read_single_reg(uint8_t addr);

void cc430_radio_write_single_reg(uint8_t addr, uint8_t value);

void cc430_radio_read_burst_reg(uint8_t addr, void *buffer, uint8_t count);

void cc430_radio_write_burst_reg(uint8_t addr, void *buffer, uint8_t count);

void cc430_radio_write_rf_reg(const uint8_t rf_setting[][2], uint8_t size);

void cc430_radio_reset_radio_core(void);

int8_t cc430_radio_write_pa_table(uint8_t value);

void cc430_radio_transmit(void *buffer, uint8_t length);

void cc430_radio_receive(void *buffer, uint8_t *length);

void cc430_radio_receive_off(void);

void cc430_radio_receive_on(void);

void cc430_radio_transmit_bigger_than_fifotx(void *buffer, uint8_t length);


#ifdef __cplusplus
}
#endif

#endif /* CC430_RF_INTERNAL_H */
/** @} */
