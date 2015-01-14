/*
 * Copyright (C) 2009 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x_legacy
 * @{
 *
 * @file
 * @brief       Configuration parameters for the cc110x radio chip
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */
#ifndef CC1100_CONFIG_H
#define CC1100_CONFIG_H

#include <stdint.h>
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/** CC1100 register configuration */
typedef struct {
    uint8_t _IOCFG2;
    uint8_t _IOCFG1;
    uint8_t _IOCFG0;
    uint8_t _FIFOTHR;
    uint8_t _SYNC1;
    uint8_t _SYNC0;
    uint8_t _PKTLEN;
    uint8_t _PKTCTRL1;
    uint8_t _PKTCTRL0;
    uint8_t _ADDR;
    uint8_t _CHANNR;
    uint8_t _FSCTRL1;
    uint8_t _FSCTRL0;
    uint8_t _FREQ2;
    uint8_t _FREQ1;
    uint8_t _FREQ0;
    uint8_t _MDMCFG4;
    uint8_t _MDMCFG3;
    uint8_t _MDMCFG2;
    uint8_t _MDMCFG1;
    uint8_t _MDMCFG0;
    uint8_t _DEVIATN;
    uint8_t _MCSM2;
    uint8_t _MCSM1;
    uint8_t _MCSM0;
    uint8_t _FOCCFG;
    uint8_t _BSCFG;
    uint8_t _AGCCTRL2;
    uint8_t _AGCCTRL1;
    uint8_t _AGCCTRL0;
    uint8_t _WOREVT1;
    uint8_t _WOREVT0;
    uint8_t _WORCTRL;
    uint8_t _FREND1;
    uint8_t _FREND0;
    uint8_t _FSCAL3;
    uint8_t _FSCAL2;
    uint8_t _FSCAL1;
    uint8_t _FSCAL0;
} cc110x_reg_t;

/** CC1100 radio configuration */
typedef struct {
    cc110x_reg_t reg_cfg;       /**< CC1100 register configuration */
    uint8_t pa_power;           /**< Output power setting */
} cc110x_cfg_t;

/**
 * @brief   Radio Control Flags
 */
typedef struct {
    uint32_t TOF;               /**< Time of flight of the last packet and last ACK */
    timex_t  TOA;               /**< Time of packet arrival */
    uint32_t TCP;               /**< Time to compute packet */
    unsigned RPS        : 16;   /**< Raw packets sent to transmit last packet */
    unsigned RETC       : 8;    /**< Retransmission count of last sent packet */
    unsigned _RSSI      : 8;    /**< The RSSI value of last received packet */
    unsigned RSSI_SEND  : 8;    /**< The RSSI value of the last sent unicast packet of this node */
    unsigned _LQI       : 8;    /**< The LQI value of the last received packet */
    unsigned LL_ACK     : 1;    /**< Is set if Link-Level ACK is received, otherwise 0 (reset on new burst) */
    unsigned CAA        : 1;    /**< The status of the air (1 = air free, 0 = air not free) */
    unsigned CRC_STATE  : 1;    /**< The CRC status of last received packet (1 = OK, 0 = not OK) */
    unsigned SEQ        : 1;    /**< Sequence number (toggles between 0 and 1) */
    unsigned MAN_WOR    : 1;    /**< Manual WOR set (for randomized WOR times => no synch) */
    unsigned KT_RES_ERR : 1;    /**< A hwtimer resource error has occurred (no free timers available) */
    unsigned TX         : 1;    /**< State machine TX lock, only ACKs will be received */
    unsigned WOR_RST    : 1;    /**< Reset CC1100 real time clock (WOR) on next WOR strobe */
} cc110x_flags;

/**
 * @brief   Statistic interface for debugging
 */
typedef struct cc110x_statistic {
    uint32_t    packets_in;
    uint32_t    packets_in_crc_fail;
    uint32_t    packets_in_while_tx;
    uint32_t    packets_in_dups;
    uint32_t    packets_in_up;
    uint32_t    packets_out;
    uint32_t    packets_out_broadcast;
    uint32_t    raw_packets_out;
    uint32_t    acks_send;
    uint32_t    rx_buffer_max;
    uint32_t    watch_dog_resets;
} cc110x_statistic_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __CC110X_CONFIG_H */
