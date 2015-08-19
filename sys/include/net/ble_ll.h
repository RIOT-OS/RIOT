/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        ble_ll BLE Link Layer
 * @ingroup         ble_ll
 * @brief           Ble link layer implementation
 * @{
 *
 * @file
 * @brief           Interface definition for the ble link layer
 *
 * @author          Jan Wagner <mail@jwagner.eu>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BLE_LL_H_
#define BLE_LL_H_

#include "net/gnrc/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default channel set after initialization
 */
#define BLE_DEFAULT_CHANNEL         37  /* 2402MHz */

/**
 * @brief   Default transmission power used
 */
#define BLE_DEFAULT_TXPOWER         0 /* 0dBm */

/**
 * @brief   BLE Advertising channel PDU types
 * @{
 */
enum ble_adv_pdu_types {
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
#define BLE_MAX_CHANNELS            39

#define BLE_ACCESS_ADDR_LEN         4
#define BLE_ADV_HDR_LEN             2
#define BLE_ADV_ADDR_LEN            6
#define BLE_ADV_PAYLOAD_LEN_MIN     (BLE_ADV_ADDR_LEN)
#define BLE_ADV_PAYLOAD_LEN_MAX     37
#define BLE_ADV_DATA_LEN_MAX        (BLE_ADV_PAYLOAD_LEN_MAX - BLE_ADV_ADDR_LEN)
#define BLE_PDU_LEN_MAX             257

#define BLE_DEFAULT_ACCESS_ADDR     0x8E89BED6

#define BLE_DEFAULT_ADV_ADDRESS     "\x28\x7c\x97\x38\x5e\xc2"
#define BLE_DEFAULT_ADV_PDU_TYPE    ADV_IND_TYPE
#define BLE_DEFAULT_ADV_TXADD       1
#define BLE_DEFAULT_ADV_RXADD       0
#define BLE_DEFAULT_RFU             0
#define BLE_DEFAULT_ADV_DATA_LEN    27
#define BLE_DEFAULT_ADV_DATA        "\x08\x09\x52\x49\x4f\x54\x2d\x4f" \
                                    "\x53\x02\x01\x06\x02\x0a\x04\x03" \
                                    "\x03\x20\x22\x07\xff\x00\x00\x64" \
                                    "\x61\x74\x61"

#define BLE_DEFAULT_SCAN_RSP_TXADD  1
#define BLE_DEFAULT_SCAN_RSP_RXADD  0

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
} ble_adv_hdr_t;

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
} ble_conn_req_data_t;

/**
 * @brief   In-memory structure of a ble radio packet
 */
typedef struct __attribute__((packed))
{
    ble_adv_hdr_t header;                       /**< BLE pdu header */
    uint8_t payload[BLE_ADV_PAYLOAD_LEN_MAX];   /**< actual payload */
} ble_adv_pkt_t;

/**
 * @brief   Reference to the netdev driver interface
 */
extern const gnrc_netdev_driver_t ble_driver;

/**
 * @brief   Initialize an instance of the ble link layer
 *
 * The initialization starts a new thread that connects to the given netdev
 * device and starts a link layer event loop.
 *
 * @param[in] stack         stack for the control thread
 * @param[in] stacksize     size of *stack*
 * @param[in] priority      priority for the thread housing the NOMAC instance
 * @param[in] name          name of the thread housing the NOMAC instance
 * @param[in] dev           netdev device, needs to be already initialized
 *
 * @return                  PID of NOMAC thread on success
 * @return                  -EINVAL if creation of thread fails
 * @return                  -ENODEV if *dev* is invalid
 */
kernel_pid_t ble_ll_init(char *stack, int stacksize, char priority,
                           const char *name, gnrc_netdev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* BLE_LL_H_ */
/** @} */
