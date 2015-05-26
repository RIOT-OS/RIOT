/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_nrf51822_blemin BLE Minimal Radio Driver
 * @ingroup         cpu_nrf51822
 * @brief           Minimal driver for the NRF51822 radio
 * @{
 *
 * @file
 * @brief           Interface definition for the blemin NRF51822 radio driver
 *
 * This driver enables the use of the NRF51822 radio BLE protocol.
 * - development in progress / experimental
 *
 * @author          Jan Wagner <mail@jwagner.eu>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BLEMIN_H_
#define BLEMIN_H_

#include "net/ng_netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default channel set after initialization
 */
#define BLEMIN_DEFAULT_CHANNEL	37	/* 2402MHz */

/**
 * @brief   Default transmission power used
 */
#define BLEMIN_DEFAULT_TXPOWER	(0)	/* 0dBm */

/**
 * @brief   BLE Advertising channel PDU types
 * @{
 */
enum pdu_types {
    ADV_IND_TYPE,
    ADV_DIRECT_IND_TYPE,
    ADV_NONCONN_IND_TYPE,
    SCAN_REQ_TYPE,
    SCAN_RSP_TYPE,
    CONNECT_REQ_TYPE,
    ADV_SCAN_IND_TYPE
};
/** @} */

/**
 * @brief   BLE constant lengths and limits
 * @{
 */
#define BLE_PDU_HDR_LEN			2
#define BLE_ACCESS_ADDR_LEN		4
#define BLE_ADDR_LEN			6
#define BLE_PAYLOAD_LEN_MIN     	(BLE_ADDR_LEN)
#define BLE_PAYLOAD_LEN_MAX     	37
#define BLE_MAX_DATA_LEN		(BLE_PAYLOAD_LEN_MAX - BLE_ADDR_LEN)
#define BLE_DEFAULT_ACCESS_ADDR		0x8E89BED6
/** @} */

/**
 * @brief   Arrays for BLE frequency / channel convertion
 * @{
 */
static const uint8_t chan_to_freq[] = {
    4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 28, 30, 32, 34, 36, 38, 40, 42, 44,
    46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 2, 26, 80
};

static const uint8_t freq_to_chan[] = {
    0, 0, 37, 0, 0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0,
    8, 0, 9, 0, 10, 0, 38, 0, 11, 0, 12, 0, 13, 0, 14, 0, 15, 0, 16, 0,
    17, 0, 18, 0, 19, 0, 20, 0, 21, 0, 22, 0, 23, 0, 24, 0, 25, 0, 26, 0,
    27, 0, 28, 0, 29, 0, 30, 0, 31, 0, 32, 0, 33, 0, 34, 0, 35, 0, 36, 0, 39
};
/** @} */

/**
 * @brief   Structure of a BLE Advertising channel PDU header
 */
typedef struct __attribute__((packed))
{
    uint8_t pdu_type: 4;                   /**< PDU type */
    uint8_t RFU1:     2;                   /**< Reserved for future use (1) */
    uint8_t tx_add:   1;                   /**< TXAdd field */
    uint8_t rx_add:   1;                   /**< TXAdd field */
    uint8_t length:   6;                   /**< Payload length */
    uint8_t RFU2:     2;                   /**< Reserved for future use (2) */
} PDU_header;

/**
 * @brief   Structure of a BLE connection request Link Layer field
 */
typedef struct __attribute__((packed))
{
    uint8_t AA[3];                          /**< Access Address */
    uint8_t CRCInit[2];                     /**< CRC initialization value */
    uint8_t WinSize;                        /**< Transmit window size */
    uint8_t WinOffset[1];                   /**< Transmit window offset */
    uint8_t Interval[1];                    /**< Connection interval */
    uint8_t Latency[1];                     /**< Connection slave latency */
    uint8_t Timeout[1];                     /**< Connection supervision timeout */
    uint8_t ChM[4];                         /**< Channel map */
    uint8_t Hop: 5;                         /**< Hop increment */
    uint8_t SCA: 3;                         /**< Sleep clock accuracy */
} LL_data;

/**
 * @brief   Reference to the netdev driver interface
 */
extern const ng_netdev_driver_t blemin_driver;

/**
 * @brief   Initialize the NRF51822 radio
 *
 * The initialization uses static configuration values.
 *
 * @param dev[out]      pointer to the netdev device descriptor
 *
 * @return              0 on success
 * @return              -ENODEV if @p dev is invalid
 */
int blemin_init(ng_netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* BLEMIN_H_ */
/** @} */
