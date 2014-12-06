/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file        cc110x-config.h
 * @brief       Configuration parameters for the cc110x radio chip
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef CC110X_CONFIG_H
#define CC110X_CONFIG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   CC110x register configuration
 */
typedef struct {
    uint8_t _IOCFG2;      /**< GDO2 output pin configuration */
    uint8_t _IOCFG1;      /**< GDO1 output pin configuration */
    uint8_t _IOCFG0;      /**< GDO0 output pin configuration */
    uint8_t _FIFOTHR;     /**< RX FIFO and TX FIFO thresholds */
    uint8_t _SYNC1;       /**< Sync word, high byte */
    uint8_t _SYNC0;       /**< Sync word, low byte */
    uint8_t _PKTLEN;      /**< Packet length */
    uint8_t _PKTCTRL1;    /**< Packet automation control */
    uint8_t _PKTCTRL0;    /**< Packet automation control */
    uint8_t _ADDR;        /**< Device address */
    uint8_t _CHANNR;      /**< Channel number */
    uint8_t _FSCTRL1;     /**< Frequency synthesizer control */
    uint8_t _FSCTRL0;     /**< Frequency synthesizer control */
    uint8_t _FREQ2;       /**< Frequency control word, high byte */
    uint8_t _FREQ1;       /**< Frequency control word, middle byte */
    uint8_t _FREQ0;       /**< Frequency control word, low byte */
    uint8_t _MDMCFG4;     /**< Modem configuration */
    uint8_t _MDMCFG3;     /**< Modem configuration */
    uint8_t _MDMCFG2;     /**< Modem configuration */
    uint8_t _MDMCFG1;     /**< Modem configuration */
    uint8_t _MDMCFG0;     /**< Modem configuration */
    uint8_t _DEVIATN;     /**< Modem deviation setting */
    uint8_t _MCSM2;       /**< Main Radio Control State Machine configuration */
    uint8_t _MCSM1;       /**< Main Radio Control State Machine configuration */
    uint8_t _MCSM0;       /**< Main Radio Control State Machine configuration */
    uint8_t _FOCCFG;      /**< Frequency Offset Compensation configuration */
    uint8_t _BSCFG;       /**< Bit Synchronization configuration */
    uint8_t _AGCCTRL2;    /**< AGC control */
    uint8_t _AGCCTRL1;    /**< AGC control */
    uint8_t _AGCCTRL0;    /**< AGC control */
    uint8_t _WOREVT1;     /**< High byte Event 0 timeout */
    uint8_t _WOREVT0;     /**< Low byte Event 0 timeout */
    uint8_t _WORCTRL;     /**< Wake On Radio control */
    uint8_t _FREND1;      /**< Front end RX configuration */
    uint8_t _FREND0;      /**< Front end TX configuration */
    uint8_t _FSCAL3;      /**< Frequency synthesizer calibration */
    uint8_t _FSCAL2;      /**< Frequency synthesizer calibration */
    uint8_t _FSCAL1;      /**< Frequency synthesizer calibration */
    uint8_t _FSCAL0;      /**< Frequency synthesizer calibration */
} cc110x_reg_t;

/**
 * @brief   CC110x radio configuration
 */
typedef struct {
    cc110x_reg_t reg_cfg;       ///< CC1100 register configuration
    uint8_t pa_power;           ///< Output power setting
} cc110x_cfg_t;

/**
 * @brief   Radio Control Flags
 */
typedef struct {
    uint8_t  _RSSI;             ///< The RSSI value of last received packet
    uint8_t  _LQI;              ///< The LQI value of the last received packet
} cc110x_flags;

/**
 * @brief   Statistic interface for debugging
 */
typedef struct cc110x_statistic {
    uint32_t    packets_in;
    uint32_t    packets_in_crc_fail;
    uint32_t    packets_in_while_tx;
    uint32_t    raw_packets_out;
} cc110x_statistic_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CC110X_CONFIG_H */
