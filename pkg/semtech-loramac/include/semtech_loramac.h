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

#include "mutex.h"

#include "net/netdev.h"
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
#define MSG_TYPE_LORAMAC_JOIN          (0x3461)  /**< MAC join event */
#define MSG_TYPE_LORAMAC_TX_DONE       (0x3462)  /**< MAC TX completes */
#define MSG_TYPE_LORAMAC_RX            (0x3463)  /**< Some data received */
#define MSG_TYPE_LORAMAC_LINK_CHECK    (0x3464)  /**< Link check info received */
#define MSG_TYPE_LORAMAC_TX_CNF_FAILED (0x3465)  /**< MAC TX confirmed failed */
/** @} */

/**
 * @brief   Maximum payload size of a LoRaWAN application data
 */
#define LORAWAN_APP_DATA_MAX_SIZE      (242U)

/**
 * @brief   LoRaMAC return status
 */
enum {
    SEMTECH_LORAMAC_JOIN_SUCCEEDED,              /**< Join procedure succeeded */
    SEMTECH_LORAMAC_JOIN_FAILED,                 /**< Join procedure failed */
    SEMTECH_LORAMAC_NOT_JOINED,                  /**< MAC is not joined */
    SEMTECH_LORAMAC_TX_SCHEDULED,                /**< TX data scheduled */
    SEMTECH_LORAMAC_TX_DONE,                     /**< Transmission completed */
    SEMTECH_LORAMAC_TX_CNF_FAILED,               /**< Confirmable transmission failed */
    SEMTECH_LORAMAC_DATA_RECEIVED,               /**< Data received */
    SEMTECH_LORAMAC_BUSY                         /**< Internal MAC is busy */
};

/**
 * @brief   LoRaMAC internal state
 */
enum {
    SEMTECH_LORAMAC_STATE_IDLE = 0,
    SEMTECH_LORAMAC_STATE_BUSY
};

/**
 * @brief   LoRaMAC channel radio parameters
 */
typedef struct {
    uint32_t frequency;                          /**< channel center frequency */
    uint8_t datarate;                            /**< channel datarate */
} semtech_loramac_channel_params_t;

/**
 * @brief   Structure containing LoRaWAN RX data
 */
typedef struct {
    uint8_t payload[LORAWAN_APP_DATA_MAX_SIZE];  /**< RX payload buffer */
    uint8_t payload_len;                         /**< Length of the RX payload */
    uint8_t port;                                /**< RX port */
} semtech_loramac_rx_data_t;

/**
 * @brief   LoRaMAC link check information
 */
typedef struct {
    uint8_t demod_margin;                        /**< Demodulation margin */
    uint8_t nb_gateways;                         /**< number of LoRa gateways found */
    bool available;                              /**< new link check information avalable */
} semtech_loramac_link_check_info_t;

/**
 * @brief   Semtech LoRaMAC descriptor
 */
typedef struct {
    mutex_t lock;                                /**< loramac access lock */
    uint8_t state;                               /**< internal loramac state */
    uint8_t caller_pid;                          /**< pid of caller thread */
    uint8_t port;                                /**< application TX port */
    uint8_t cnf;                                 /**< enable/disable confirmable messages */
    uint8_t deveui[LORAMAC_DEVEUI_LEN];          /**< device EUI */
    uint8_t appeui[LORAMAC_APPEUI_LEN];          /**< application EUI */
    uint8_t appkey[LORAMAC_APPKEY_LEN];          /**< application key */
    uint8_t appskey[LORAMAC_APPSKEY_LEN];        /**< application session key */
    uint8_t nwkskey[LORAMAC_NWKSKEY_LEN];        /**< network session key */
    uint8_t devaddr[LORAMAC_DEVADDR_LEN];        /**< device address */
    semtech_loramac_rx_data_t rx_data;           /**< struct handling the RX data */
    semtech_loramac_link_check_info_t link_chk;  /**< link check information */
} semtech_loramac_t;

/**
 * @brief   Initializes the semtech loramac mac
 *
 * @param[in] mac          Pointer to loramac descriptor
 *
 * @return  0 on success
 * @return -1 on failure
 */
int semtech_loramac_init(semtech_loramac_t *mac);

/**
 * @brief   Starts a LoRaWAN network join procedure
 *
 * This function blocks until the join procedure succeeds or fails.
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] type         The type of join procedure (otaa or abp)
 *
 * @return SEMTECH_LORAMAC_JOIN_SUCCEEDED on success
 * @return SEMTECH_LORAMAC_JOIN_FAILED on failure
 * @return SEMTECH_LORAMAC_BUSY when the mac is already active (join or tx in progress)
 */
uint8_t semtech_loramac_join(semtech_loramac_t *mac, uint8_t type);

/**
 * @brief   Sends data to the LoRaWAN network
 *
 * This function returns immediately and leave the mac in busy state until a
 * message is received from the network (with RX1 and RX2 receive windows).
 * @see semtech_loramac_recv
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] data         The TX data
 * @param[in] len          The length of the TX data
 *
 * @return SEMTECH_LORAMAC_NOT_JOINED when the network is not joined
 * @return SEMTECH_LORAMAC_BUSY when the mac is already active (join or tx in progress)
 * @return SEMTECH_LORAMAC_TX_SCHEDULED when the TX is scheduled in the mac
 */
uint8_t semtech_loramac_send(semtech_loramac_t *mac, uint8_t *data, uint8_t len);

/**
 * @brief   Wait for a message sent by the LoRaWAN network
 *
 * This function blocks until a single message is received by the mac (RX1 and
 * RX2 windows).
 * With a class A device, a message can only be received after a send. With a
 * class C device, a message can be received at any time. In this case, this
 * function can be used in a dedicated listener thread.
 *
 * @see semtech_loramac_send
 *
 * @param[in] mac          Pointer to the mac
 *
 * @return SEMTECH_LORAMAC_TX_DONE when TX has completed, no data received
 * @return SEMTECH_LORAMAC_DATA_RECEIVED when TX has completed and data is received
 */
uint8_t semtech_loramac_recv(semtech_loramac_t *mac);

/**
 * @brief   Requests a LoRaWAN link check
 *
 * @param[in] mac          Pointer to the mac
 */
void semtech_loramac_request_link_check(semtech_loramac_t *mac);

/**
 * @brief   Sets the device EUI
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] eui          The device EUI
 */
void semtech_loramac_set_deveui(semtech_loramac_t *mac, const uint8_t *eui);

/**
 * @brief   Gets the device EUI
 *
 * @param[in] mac          Pointer to the mac
 * @param[out] eui         The device EUI
 */
void semtech_loramac_get_deveui(const semtech_loramac_t *mac, uint8_t *eui);

/**
 * @brief   Sets the application EUI
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] eui          The application EUI
 */
void semtech_loramac_set_appeui(semtech_loramac_t *mac, const uint8_t *eui);

/**
 * @brief   Gets the application EUI
 *
 * @param[in] mac          Pointer to the mac
 * @param[out] eui         The application EUI
 */
void semtech_loramac_get_appeui(const semtech_loramac_t *mac, uint8_t *eui);

/**
 * @brief   Sets the application key
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] key          The application key
 */
void semtech_loramac_set_appkey(semtech_loramac_t *mac, const uint8_t *key);

/**
 * @brief   Gets the application key
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] key          The application key
 */
void semtech_loramac_get_appkey(const semtech_loramac_t *mac, uint8_t *key);

/**
 * @brief   Sets the application session key
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] skey         The application session key
 */
void semtech_loramac_set_appskey(semtech_loramac_t *mac, const uint8_t *skey);

/**
 * @brief   Gets the application session key
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] skey         The application session key
 */
void semtech_loramac_get_appskey(const semtech_loramac_t *mac, uint8_t *skey);

/**
 * @brief   Sets the network session key
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] skey         The network session key
 */
void semtech_loramac_set_nwkskey(semtech_loramac_t *mac, const uint8_t *skey);

/**
 * @brief   Gets the network session key
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] skey         The network session key
 */
void semtech_loramac_get_nwkskey(const semtech_loramac_t *mac, uint8_t *skey);

/**
 * @brief   Sets the device address
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] addr         The device address
 */
void semtech_loramac_set_devaddr(semtech_loramac_t *mac, const uint8_t *addr);

/**
 * @brief   Gets the device address
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] addr         The device address
 */
void semtech_loramac_get_devaddr(const semtech_loramac_t *mac, uint8_t *addr);

/**
 * @brief   Sets the device class
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] cls          The device class
 */
void semtech_loramac_set_class(semtech_loramac_t *mac, loramac_class_t cls);

/**
 * @brief   Gets the device class
 *
 * @param[in] mac          Pointer to the mac
 * @return                 The device class
 */
loramac_class_t semtech_loramac_get_class(semtech_loramac_t *mac);

/**
 * @brief   Sets the channels datarate
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] dr           The datarate (from 1 to 16)
 */
void semtech_loramac_set_dr(semtech_loramac_t *mac, uint8_t dr);

/**
 * @brief   Gets the channels datarate
 *
 * @param[in] mac          Pointer to the mac
 * @return                 The datarate (from 1 to 16)
 */
uint8_t semtech_loramac_get_dr(semtech_loramac_t *mac);

/**
 * @brief   Enables/disable adaptive datarate
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] adr          Adaptive datarate mode
 */
void semtech_loramac_set_adr(semtech_loramac_t *mac, bool adr);

/**
 * @brief   Checks if adaptive datarate is set
 *
 * @param[in] mac          Pointer to the mac
 * @return                 true if adr is on, false otherwise
 */
bool semtech_loramac_get_adr(semtech_loramac_t *mac);

/**
 * @brief   Enable/disable the public network mode
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] public       The public network mode
 */
void semtech_loramac_set_public_network(semtech_loramac_t *mac, bool public);

/**
 * @brief   Checks if public network is set
 *
 * @param[in] mac          Pointer to the mac
 * @return                 true if public network is on, false otherwise
 */
bool semtech_loramac_get_public_network(semtech_loramac_t *mac);

/**
 * @brief   Sets the NetID (only useful with ABP join procedure)
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] netid        The NetID
 */
void semtech_loramac_set_netid(semtech_loramac_t *mac, uint32_t netid);

/**
 * @brief   Gets the NetID
 *
 * @param[in] mac          Pointer to the mac
 * @return                 The NetID
 */
uint32_t semtech_loramac_get_netid(semtech_loramac_t *mac);

/**
 * @brief   Sets the channels TX power index
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] power        The TX power index (from 1 to 16)
 */
void semtech_loramac_set_tx_power(semtech_loramac_t *mac, uint8_t power);

/**
 * @brief   Gets the channels TX power index
 *
 * @param[in] mac          Pointer to the mac
 * @return                 The TX power index (from 1 to 16)
 */
uint8_t semtech_loramac_get_tx_power(semtech_loramac_t *mac);

/**
 * @brief   Sets the TX application port
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] port         The TX application port
 */
void semtech_loramac_set_tx_port(semtech_loramac_t *mac, uint8_t port);

/**
 * @brief   Gets the TX application port
 *
 * @param[in] mac          Pointer to the mac
 * @return                 The TX application port
 */
uint8_t semtech_loramac_get_tx_port(semtech_loramac_t *mac);

/**
 * @brief   Sets the TX confirmable mode
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] mode          The TX mode (confirmable or not confirmable)
 */
void semtech_loramac_set_tx_mode(semtech_loramac_t *mac, uint8_t mode);

/**
 * @brief   Gets the TX confirmable mode
 *
 * @param[in] mac          Pointer to the mac
 * @return                 The TX mode (confirmable or not confirmable)
 */
uint8_t semtech_loramac_get_tx_mode(semtech_loramac_t *mac);

/**
 * @brief   Sets the RX2 frequency
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] freq         The RX2 frequency
 */
void semtech_loramac_set_rx2_freq(semtech_loramac_t *mac, uint32_t freq);

/**
 * @brief   Gets the RX2 frequency
 *
 * @param[in] mac          Pointer to the mac
 * @return                 The RX2 frequency
 */
uint32_t semtech_loramac_get_rx2_freq(semtech_loramac_t *mac);

/**
 * @brief   Sets the RX2 datarate
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] dr           The RX2 datarate
 */
void semtech_loramac_set_rx2_dr(semtech_loramac_t *mac, uint8_t dr);

/**
 * @brief   Gets the RX2 datarate
 *
 * @param[in] mac          Pointer to the mac
 * @return                 The RX2 datarate
 */
uint8_t semtech_loramac_get_rx2_dr(semtech_loramac_t *mac);

#ifdef __cplusplus
}
#endif

#endif /* SEMTECH_LORAMAC_H */
/** @} */
