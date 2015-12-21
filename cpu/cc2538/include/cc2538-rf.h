/*
 * Copyright (C) 2015 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file        cc2538-rf.h
 * @brief       Low-level radio driver for the CC2538
 *
 * @author      Ian Martin <ian@locicontrols.com>
 */

#ifndef CC2538_RF_H
#define CC2538_RF_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "debug.h"
#include "kernel_types.h"
#include "ieee802154_frame.h"
#include "radio_driver.h"
#include "netdev/802154.h"

#define CC2538_RF_FIFO_SIZE         128
#define CC2538_PACKET_LENGTH_SIZE     1 /**< Packet length is represented by one byte in the FIFO */
#define CC2538_FCS_LEN                2
#define CC2538_AUTOCRC_LEN            2
#define CC2538_RSSI_OFFSET           73 /**< 73 dB RSSI offset */

#define CC2538_RF_MAX_PACKET_LEN    (CC2538_RF_FIFO_SIZE - 1)

#define CC2538_MAX_DATA_LENGTH      CC2538_RF_MAX_PACKET_LEN

#define RFCORE_ASSERT(expr) (void)( (expr) || RFCORE_ASSERT_failure(#expr, __FUNCTION__, __LINE__) )

#if DEVELHELP
#define RFCORE_WAIT_UNTIL(expr) while (!(expr)) {                                        \
    DEBUG("RFCORE_WAIT_UNTIL(%s) at line %u in %s()\n", #expr, __LINE__, __FUNCTION__);  \
    thread_yield();                                                                      \
}
#else
#define RFCORE_WAIT_UNTIL(expr) while (!(expr)) thread_yield()
#endif

#ifndef BIT
#define BIT(n) ( 1 << (n) )
#endif

enum {
    FSM_STATE_IDLE           =  0,
    FSM_STATE_RX_CALIBRATION =  2,
    FSM_STATE_TX_CALIBRATION = 32,
};

/* Values for use with CCTEST_OBSSELx registers: */
#define OBSSEL_EN BIT(7)
enum {
    rfc_obs_sig0 = 0,
    rfc_obs_sig1 = 1,
    rfc_obs_sig2 = 2,
};

/* Values for RFCORE_XREG_RFC_OBS_CTRLx registers: */
#define RFC_OBS_POL BIT(6)
enum {
    constant_value_0 = 0x00, /**< Constant value */
    constant_value_1 = 0x01, /**< Constant value */
    rfc_sniff_data   = 0x08, /**< Data from packet sniffer. Sample data on rising edges of sniff_clk. */
    rfc_sniff_clk    = 0x09, /**< 250kHz clock for packet sniffer data. */
    rssi_valid       = 0x0c, /**< Pin is high when the RSSI value has been updated at least once since RX was started. Cleared when leaving RX. */
    demod_cca        = 0x0d, /**< Clear channel assessment. See FSMSTAT1 register for details on how to configure the behavior of this signal. */
    sampled_cca      = 0x0e, /**< A sampled version of the CCA bit from demodulator. The value is updated whenever a SSAMPLECCA or STXONCCA strobe is issued. */
    sfd_sync         = 0x0f, /**< Pin is high when a SFD has been received or transmitted. Cleared when leaving RX/TX respectively. Not to be confused with the SFD exception. */
    tx_active        = 0x10, /**< Indicates that FFCTRL is in one of the TX states. Active-high. */
    rx_active        = 0x11, /**< Indicates that FFCTRL is in one of the RX states. Active-high.  */
    ffctrl_fifo      = 0x12, /**< Pin is high when one or more bytes are in the RXFIFO. Low during RXFIFO overflow. */
    ffctrl_fifop     = 0x13, /**< Pin is high when the number of bytes in the RXFIFO exceeds the programmable threshold or at least one complete frame is in the RXFIFO. */
    packet_done      = 0x14, /**< A complete frame has been received. I.e., the number of bytes set by the frame-length field has been received. */
    rfc_xor_rand_i_q = 0x16, /**< XOR between I and Q random outputs. Updated at 8 MHz. */
    rfc_rand_q       = 0x17, /**< Random data output from the Q channel of the receiver. Updated at 8 MHz. */
    rfc_rand_i       = 0x18, /**< Random data output from the I channel of the receiver. Updated at 8 MHz */
    lock_status      = 0x19, /**< 1 when PLL is in lock, otherwise 0 */
    pa_pd            = 0x20, /**< Power amplifier power-down signal */
    lna_pd           = 0x2a, /**< LNA power-down signal */
};

/**
 *  Structure to represent a cc2538 packet.
 */
typedef struct {
    /* @{ */
    uint_fast8_t length;        /** < the length of the frame of the frame including fcs*/
    ieee802154_frame_t frame;   /** < the ieee802154 frame */
    int_fast8_t rssi;           /** < the rssi value */
    uint_fast8_t lqi;           /** < the link quality indicator */
    bool crc;                   /** < 1 if crc was successfull, 0 otherwise */
    /* @} */
} cc2538_packet_t;

/**
 * @brief 802.15.4 long (IEEE) destination and source address pair
 */
typedef struct __attribute__((packed)) {
    uint64_t dest_addr;         /**< Destination long (IEEE) address            */
    uint64_t src_addr;          /**< Source long (IEEE) address                 */
}
long_addrs_t;

/**
 * @brief 802.15.4 short destination and source address pair
 */
typedef struct __attribute__((packed)) {
    uint16_t dest_pan;          /**< Destination Personal Area Network (PAN) ID */
    uint16_t dest_addr;         /**< Destination short address                  */
    uint16_t src_pan;           /**< Source Personal Area Network (PAN) ID      */
    uint16_t src_addr;          /**< Source short address                       */
}
short_addrs_t;

/**
 * @brief 802.15.4 compressed short destination and source address pair
 */
typedef struct __attribute__((packed)) {
    uint16_t dest_pan;          /**< Destination Personal Area Network (PAN) ID */
    uint16_t dest_addr;         /**< Destination short address                  */
    uint16_t src_addr;          /**< Source short address                       */
}
short_addrs_compr_t;

/**
 * @brief 802.15.4 MAC header format
 */
typedef struct __attribute__((packed)) {
    uint8_t type;               /**< IEEE 802.15.4 packet type                  */
    uint8_t addr_type;          /**< IEEE 802.15.4 address type (short/long)    */
    uint8_t sequence_nr;        /**< IEEE 802.15.4 sequence number              */

    union {
        long_addrs_t long_addrs;
        short_addrs_t short_addrs;
        short_addrs_compr_t short_addrs_compr;
    };
}
mac_header_t;

/**
 * @brief CC2538 received Frame Check Sequence (FCS) format
 */
typedef struct __attribute__((packed)) {
    signed   rssi : 8;          /**< Relative Signal Strength Indicator         */
    unsigned lqi  : 7;          /**< Link Quality Indicator                     */
    unsigned crc  : 1;          /**< Cyclic Redundancy Check pass flag          */
}
cc2538_fcs_t;

/* Bits within mac_header_t::type */
#define INTRA_PAN_COMPRESSION BIT(6)
#define WANTS_ACK             BIT(5)

/* Values for mac_header_t::addr_type */
enum {
    ADDR_TYPE_LONG  = 0xcc,
    ADDR_TYPE_SHORT = 0x88,
};

extern cc2538_packet_t cc2538_packet;

extern netdev_rcv_data_cb_t netdev_rcv_data_cb;

bool RFCORE_ASSERT_failure(const char *expr, const char *func, int line);
void rfcore_strobe(uint_fast8_t instr);

uint_fast8_t rfcore_read_byte(void);
void rfcore_write_byte(uint_fast8_t byte);

void rfcore_read_fifo(void *buf, uint_fast8_t len);
void rfcore_write_fifo(const void *buf, uint_fast8_t len);

void cc2538_initialize(void);
void cc2538_init(kernel_pid_t tpid);
bool cc2538_on(void);
void cc2538_off(void);
bool cc2538_is_on(void);
void cc2538_switch_to_rx(void);
void cc2538_set_monitor(bool mode);
bool cc2538_get_monitor(void);
void cc2538_set_channel(unsigned int chan);
unsigned int cc2538_get_channel(void);
void cc2538_set_address(uint16_t addr);
void cc2538_set_address_long(uint64_t addr);
uint16_t cc2538_get_address(void);
uint64_t cc2538_get_address_long(void);
void cc2538_set_pan(uint16_t pan);
uint16_t cc2538_get_pan(void);
void cc2538_set_tx_power_raw(uint_fast8_t pow);
void cc2538_set_tx_power(int pow);
int cc2538_get_tx_power(void);
bool cc2538_channel_clear(void);
radio_tx_status_t cc2538_load_tx_buf(ieee802154_packet_kind_t kind,
                                     ieee802154_node_addr_t dest,
                                     bool use_long_addr,
                                     bool wants_ack,
                                     void *buf,
                                     unsigned int len);
radio_tx_status_t cc2538_transmit_tx_buf(void);
radio_tx_status_t cc2538_send(ieee802154_packet_kind_t kind,
                              ieee802154_node_addr_t dest,
                              bool use_long_addr,
                              bool wants_ack,
                              void *buf,
                              unsigned int len);
void cc2538_set_recv_callback(receive_802154_packet_callback_t recv_cb);
int16_t cc2538_send_lowlevel(const void *buf, int len);
int16_t cc2538_send_ieee802154_packet(ieee802154_packet_t *packet);
void rfcore_rx_frame_hook(const void *buf, uint_fast8_t len, const cc2538_fcs_t *fcs);
void rfcore_tx_frame_hook(const void *buf, uint_fast8_t len);

/* 802.15.4 netdev driver */
extern const netdev_802154_driver_t cc2538_driver;
extern const netdev_t cc2538_netdev;

#ifdef __cplusplus
}
#endif

#endif /* CC2538_RF_H */

/*@}*/
