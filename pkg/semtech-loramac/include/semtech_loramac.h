/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         pkg_semtech-loramac
 * @brief           Public API and definitions of the Semtech LoRaMAC
 * @{
 *
 * @file
 *
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef SEMTECH_LORAMAC_H
#define SEMTECH_LORAMAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "net/loramac.h"

#include "sx127x.h"

/**
 * @name    Definitions for messages exchanged between the MAC and call threads
 * @{
 */
#define MSG_TYPE_ISR                   (0x3456)  /**< radio device ISR */
#define MSG_TYPE_RX_TIMEOUT            (0x3457)  /**< radio driver RX timeout */
#define MSG_TYPE_TX_TIMEOUT            (0x3458)  /**< radio driver TX timeout */
#define MSG_TYPE_MAC_TIMEOUT           (0x3459)  /**< MAC timers timeout */
#define MSG_TYPE_LORAMAC_CMD           (0x3460)  /**< Command sent to the MAC */
#define MSG_TYPE_LORAMAC_NOTIFY        (0x3461)  /**< MAC notifications */
/** @} */

/**
 * @brief   Maximum payload size of a LoRaWAN application data
 */
#define LORAWAN_APP_DATA_MAX_SIZE      (242U)

/**
 * @brief   LoRaMAC status
 */
enum {
    SEMTECH_LORAMAC_JOIN_SUCCEEDED,              /**< Join procedure succeeded */
    SEMTECH_LORAMAC_JOIN_FAILED,                 /**< Join procedure failed */
    SEMTECH_LORAMAC_NOT_JOINED,                  /**< MAC is not joined */
    SEMTECH_LORAMAC_TX_DONE,                     /**< Transmission completed */
    SEMTECH_LORAMAC_RX_DATA,                     /**< Data received */
};

typedef struct {
    uint8_t payload[LORAWAN_APP_DATA_MAX_SIZE];  /**< RX payload buffer */
    uint8_t payload_len;                         /**< Length of the RX payload */
    uint8_t port;                                /**< RX port */
} semtech_loramac_rx_data_t;

/**
 * @brief   Initializes semtech loramac
 *
 * @param[in] dev          pointer to the radio device
 *
 * @return  0 on success
 * @return -1 on failure
 */
int semtech_loramac_init(sx127x_t *dev);

/**
 * @brief   Starts a LoRaWAN network join procedure
 *
 * @param[in] type         The type of join procedure (otaa or abp)
 *
 * @return SEMTECH_LORAMAC_JOIN_SUCCEEDED on success
 * @return SEMTECH_LORAMAC_JOIN_FAILED on failure
 */
uint8_t semtech_loramac_join(uint8_t type);

/**
 * @brief   Sends data to LoRaWAN
 *
 * @param[in] cnf          Use confirmable/unconfirmable send type
 * @param[in] port         The send port to use (between 1 and 223)
 * @param[in] tx_buf       The TX buffer
 * @param[in] tx_len       The length of the TX buffer
 * @param[out] rx_data     The RX data descriptor
 *
 * @return SEMTECH_LORAMAC_NOT_JOINED when the network is not joined
 * @return SEMTECH_LORAMAC_TX_DONE when TX has completed, no data received
 * @return SEMTECH_LORAMAC_RX_DATA when TX has completed and data is received
 */
uint8_t semtech_loramac_send(uint8_t cnf, uint8_t port,
                             uint8_t *tx_buf, uint8_t tx_len,
                             semtech_loramac_rx_data_t *rx_data);

/**
 * @brief   Sets the device EUI
 *
 * @param[in] eui           The device EUI
 */
void semtech_loramac_set_deveui(const uint8_t *eui);

/**
 * @brief   Gets the device EUI
 *
 * @param[out] eui           The device EUI
 */
void semtech_loramac_get_deveui(uint8_t *eui);

/**
 * @brief   Sets the application EUI
 *
 * @param[in] eui           The application EUI
 */
void semtech_loramac_set_appeui(const uint8_t *eui);

/**
 * @brief   Gets the application EUI
 *
 * @param[out] eui           The application EUI
 */
void semtech_loramac_get_appeui(uint8_t *eui);

/**
 * @brief   Sets the application key
 *
 * @param[in] key           The application key
 */
void semtech_loramac_set_appkey(const uint8_t *key);

/**
 * @brief   Gets the application key
 *
 * @param[in] key           The application key
 */
void semtech_loramac_get_appkey(uint8_t *key);

/**
 * @brief   Sets the application session key
 *
 * @param[in] key           The application session key
 */
void semtech_loramac_set_appskey(const uint8_t *skey);

/**
 * @brief   Gets the application session key
 *
 * @param[in] key           The application session key
 */
void semtech_loramac_get_appskey(uint8_t *skey);

/**
 * @brief   Sets the network session key
 *
 * @param[in] key           The network session key
 */
void semtech_loramac_set_nwkskey(const uint8_t *skey);

/**
 * @brief   Gets the network session key
 *
 * @param[in] key           The network session key
 */
void semtech_loramac_get_nwkskey(uint8_t *skey);

/**
 * @brief   Sets the device address
 *
 * @param[in] addr          The device address
 */
void semtech_loramac_set_devaddr(const uint8_t *addr);

/**
 * @brief   Gets the device address
 *
 * @param[in] addr          The device address
 */
void semtech_loramac_get_devaddr(uint8_t *addr);

/**
 * @brief   Sets the device class
 *
 * @param[in] cls           The device class
 */
void semtech_loramac_set_class(loramac_class_t cls);

/**
 * @brief   Gets the device class
 *
 * @return                 The device class
 */
loramac_class_t semtech_loramac_get_class(void);

/**
 * @brief   Sets the channels datarate
 *
 * @param[in] dr           The datarate (from 1 to 16)
 */
void semtech_loramac_set_dr(uint8_t dr);

/**
 * @brief   Gets the channels datarate
 *
 * @return                 The datarate (from 1 to 16)
 */
uint8_t semtech_loramac_get_dr(void);

/**
 * @brief   Enables/disable adaptive datarate
 *
 * @param[in] adr           Adaptive datarate mode
 */
void semtech_loramac_set_adr(bool adr);

/**
 * @brief   Checks if adaptive datarate is set
 *
 * @return                  true if adr is on, false otherwise
 */
bool semtech_loramac_get_adr(void);

/**
 * @brief   Enable/disable the public network mode
 *
 * @param[in] public        The public network mode
 */
void semtech_loramac_set_public_network(bool public);

/**
 * @brief   Checks if public network is set
 *
 * @return                 true if public network is on, false otherwise
 */
bool semtech_loramac_get_public_network(void);

/**
 * @brief   Sets the NetID (only useful with ABP join procedure)
 *
 * @param[in] network_id    The NetID
 */
void semtech_loramac_set_netid(uint32_t netid);

/**
 * @brief   Gets the NetID
 *
 * @return                 The NetID
 */
uint32_t semtech_loramac_get_netid(void);

/**
 * @brief   Sets the channels TX power index
 *
 * @param[in] dr           The TX power index (from 1 to 16)
 */
void semtech_loramac_set_tx_power(uint8_t power);

/**
 * @brief   Gets the channels TX power index
 *
 * @return                 The TX power index (from 1 to 16)
 */
uint8_t semtech_loramac_get_tx_power(void);

/**
 * @brief   Sets the RX2 frequency
 *
 * @param[in] freq         The RX2 frequency
 */
void semtech_loramac_set_rx2_freq(uint8_t freq);

/**
 * @brief   Gets the RX2 frequency
 *
 * @return                 The RX2 frequency
 */
uint32_t semtech_loramac_get_rx2_freq(void);

/**
 * @brief   Sets the RX2 datarate
 *
 * @param[in] freq         The RX2 datarate
 */
void semtech_loramac_set_rx2_dr(uint8_t dr);

/**
 * @brief   Gets the RX2 datarate
 *
 * @return                 The RX2 datarate
 */
uint8_t semtech_loramac_get_rx2_dr(void);

#ifdef __cplusplus
}
#endif

#endif /* SEMTECH_LORAMAC_H */
/** @} */
