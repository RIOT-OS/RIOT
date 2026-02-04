/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         pkg_semtech-loramac
 * @brief           Public API and definitions of the Semtech LoRaMAC
 * @{
 *
 * @file
 *
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#include "mutex.h"

#include "net/netdev.h"
#include "net/loramac.h"

/**
 * @name    Definitions for messages exchanged between the MAC and call threads
 * @{
 */
#define MSG_TYPE_ISR                        (0x3456) /**< radio device ISR */
#define MSG_TYPE_RX_TIMEOUT                 (0x3457) /**< radio driver RX timeout */
#define MSG_TYPE_TX_TIMEOUT                 (0x3458) /**< radio driver TX timeout */
#define MSG_TYPE_MAC_TIMEOUT                (0x3459) /**< MAC timers timeout */
#define MSG_TYPE_LORAMAC_CMD                (0x3460) /**< Command sent to the MAC */
#define MSG_TYPE_LORAMAC_JOIN_STATUS        (0x3461) /**< Join status */
#define MSG_TYPE_LORAMAC_TX_STATUS          (0x3462) /**< Uplink status */
#define MSG_TYPE_LORAMAC_MLME_CONFIRM       (0x3463) /**< MAC MLME confirm event */
#define MSG_TYPE_LORAMAC_MLME_INDICATION    (0x3464) /**< MAC MLME indication event */
#define MSG_TYPE_LORAMAC_MCPS_CONFIRM       (0x3465) /**< MAC MCPS confirm event */
#define MSG_TYPE_LORAMAC_MCPS_INDICATION    (0x3466) /**< MAC MCPS indication event */
/** @} */

/**
 * @brief   Maximum payload size of a LoRaWAN application data
 */
#define LORAWAN_APP_DATA_MAX_SIZE      (242U)

/**
 * @brief   LoRaMAC return status
 */
enum {
    SEMTECH_LORAMAC_JOIN_SUCCEEDED,             /**< Join procedure succeeded */
    SEMTECH_LORAMAC_JOIN_FAILED,                /**< Join procedure failed */
    SEMTECH_LORAMAC_NOT_JOINED,                 /**< MAC is not joined */
    SEMTECH_LORAMAC_ALREADY_JOINED,             /**< MAC is already joined */
    SEMTECH_LORAMAC_TX_OK,                      /**< Transmission is in progress */
    SEMTECH_LORAMAC_TX_SCHEDULE,                /**< TX needs reschedule */
    SEMTECH_LORAMAC_TX_DONE,                    /**< Transmission completed */
    SEMTECH_LORAMAC_TX_CNF_FAILED,              /**< Confirmable transmission failed */
    SEMTECH_LORAMAC_TX_ERROR,                   /**< Error in TX (invalid param, unknown service) */
    SEMTECH_LORAMAC_RX_DATA,                    /**< Data received */
    SEMTECH_LORAMAC_RX_LINK_CHECK,              /**< Link check info received */
    SEMTECH_LORAMAC_RX_CONFIRMED,               /**< Confirmed ACK received */
    SEMTECH_LORAMAC_BUSY,                       /**< Internal MAC is busy */
    SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED        /**< Restricted access to channels */
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

#if defined(MODULE_SEMTECH_LORAMAC_RX) || DOXYGEN
/**
 * @brief   LoRaMAC link check information
 */
typedef struct {
    uint8_t demod_margin;                        /**< Demodulation margin */
    uint8_t nb_gateways;                         /**< number of LoRa gateways found */
} semtech_loramac_link_check_info_t;
#endif

/**
 * @brief   Semtech LoRaMAC descriptor
 */
typedef struct {
    netdev_t *netdev;                            /**< pointer to internal radio device */
    mutex_t lock;                                /**< loramac access lock */
    uint8_t tx_pid;                              /**< pid of sender thread */
#if defined(MODULE_SEMTECH_LORAMAC_RX) || DOXYGEN
    uint8_t rx_pid;                              /**< pid of receiver thread */
#endif
    uint8_t port;                                /**< application TX port */
    uint8_t cnf;                                 /**< enable/disable confirmable messages */
    uint8_t deveui[LORAMAC_DEVEUI_LEN];          /**< device EUI */
    uint8_t appeui[LORAMAC_APPEUI_LEN];          /**< application EUI */
    uint8_t appkey[LORAMAC_APPKEY_LEN];          /**< application key */
#if defined(MODULE_SEMTECH_LORAMAC_RX) || DOXYGEN
    semtech_loramac_rx_data_t rx_data;           /**< struct handling the RX data */
    semtech_loramac_link_check_info_t link_chk;  /**< link check information */
#endif
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
 * @return SEMTECH_LORAMAC_ALREADY_JOINED if network was already joined
 */
uint8_t semtech_loramac_join(semtech_loramac_t *mac, uint8_t type);

/**
 * @brief   Sends data to the LoRaWAN network
 *
 * This function returns after TX status is replied from the MAC. To receive
 * potential messages sent from the network an explicit call to
 * @ref semtech_loramac_recv must be done after this function if it returned
 * @ref SEMTECH_LORAMAC_TX_DONE and within the RX windows delays.
 *
 * @see semtech_loramac_recv
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] data         The TX data
 * @param[in] len          The length of the TX data
 *
 * @return SEMTECH_LORAMAC_TX_DONE when the message was transmitted
 * @return SEMTECH_LORAMAC_NOT_JOINED when the network is not joined
 * @return SEMTECH_LORAMAC_BUSY when the mac is already active (join or tx in progress)
 * @return SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED when the send is rejected because of dutycycle restriction
 * @return SEMTECH_LORAMAC_TX_ERROR when an invalid parameter is given
 * @return SEMTECH_LORAMAC_TX_CNF_FAILED when message was transmitted but no ACK was received
 */
uint8_t semtech_loramac_send(semtech_loramac_t *mac, uint8_t *data, uint8_t len);

#if defined(MODULE_SEMTECH_LORAMAC_RX) || DOXYGEN
/**
 * @brief   Wait for a message sent by the LoRaWAN network
 *
 * This function blocks until a single message is received by the mac (RX1 and
 * RX2 windows).
 * With a class A device, a message can only be received after a send. With a
 * class C device, a message can be received at any time. In this case, this
 * function can be used in a dedicated listener thread.
 *
 * Be sure to call this function before the end of the RX windows otherwise it
 * may block the calling thread.
 *
 * By default this feature is not available to the user application, enable it
 * by adding `USEMODULE += semtech_loramac_rx` to the application Makefile.
 *
 * @see semtech_loramac_send
 *
 * @param[in] mac          Pointer to the mac
 *
 * @return SEMTECH_LORAMAC_RX_DATA when data is received
 * @return SEMTECH_LORAMAC_RX_LINK_CHECK when link check information is received
 * @return SEMTECH_LORAMAC_RX_CONFIRMED when an ACK is received from the network
 */
uint8_t semtech_loramac_recv(semtech_loramac_t *mac);
#endif

/**
 * @brief   Check if network is already joined
 *
 * @param[in] mac          Pointer to the mac
 *
 * @return true when network is joined, false otherwise
 */
bool semtech_loramac_is_mac_joined(semtech_loramac_t *mac);

#if defined(MODULE_SEMTECH_LORAMAC_RX) || DOXYGEN
/**
 * @brief   Requests a LoRaWAN link check
 *
 * By default this feature is not available to the user application, enable it
 * by adding `USEMODULE += semtech_loramac_rx` to the application Makefile.
 *
 * @param[in] mac          Pointer to the mac
 */
void semtech_loramac_request_link_check(semtech_loramac_t *mac);
#endif

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
void semtech_loramac_get_appskey(semtech_loramac_t *mac, uint8_t *skey);

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
void semtech_loramac_get_nwkskey(semtech_loramac_t *mac, uint8_t *skey);

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
void semtech_loramac_get_devaddr(semtech_loramac_t *mac, uint8_t *addr);

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
 * @brief   Sets the maximum system overall timing error for RX (in ms)
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] error        The maximum rx timing error
 */
void semtech_loramac_set_system_max_rx_error(semtech_loramac_t *mac, uint32_t error);

/**
 * @brief   Sets the minimum required number of symbols to detect a frame
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] min_rx       The minimum rx symbols
 */
void semtech_loramac_set_min_rx_symbols(semtech_loramac_t *mac, uint8_t min_rx);

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

/**
 * @brief   Sets the Uplink Frame Counter
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] counter      Frame counter to set
 */
void semtech_loramac_set_uplink_counter(semtech_loramac_t *mac, uint32_t counter);

/**
 * @brief   Gets the Uplink Frame Counter
 *
 * @param[in] mac          Pointer to the mac
 * @return                 Uplink frame counter
 */
uint32_t semtech_loramac_get_uplink_counter(semtech_loramac_t *mac);

/**
 * @brief   Sets the Channels Mask
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] mask         Mask array, e.g., {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000}
 */
void semtech_loramac_set_channels_mask(semtech_loramac_t *mac, uint16_t *mask);

/**
 * @brief   Gets the Channels Mask
 *
 * @param[in] mac          Pointer to the mac
 * @param[in] mask         Mask array pointer
 */
void semtech_loramac_get_channels_mask(semtech_loramac_t *mac, uint16_t *mask);

#ifdef MODULE_PERIPH_EEPROM
/**
 * @brief   The magic number used to identify the LoRaWAN configuration
 */
#ifndef SEMTECH_LORAMAC_EEPROM_MAGIC
#define SEMTECH_LORAMAC_EEPROM_MAGIC        {0x52, 0x49, 0x4F, 0x54} /* RIOT */
#endif

/**
 * @brief   The magic number length used to identify the LoRaWAN configuration
 */
#ifndef SEMTECH_LORAMAC_EEPROM_MAGIC_LEN
#define SEMTECH_LORAMAC_EEPROM_MAGIC_LEN    4
#endif

/**
 * @brief   Start position of LoRaWAN configuration stored in eeprom
 */
#ifndef SEMTECH_LORAMAC_EEPROM_START
#define SEMTECH_LORAMAC_EEPROM_START        (0)
#endif

/**
 * @brief   Saves the current LoRaWAN configuration to the internal EEPROM
 *
 * @param[in] mac           Pointer to the mac
 */
void semtech_loramac_save_config(semtech_loramac_t *mac);

/**
 * @brief   Erases any stored LoRaWAN configuration from the internal EEPROM
 */
void semtech_loramac_erase_config(void);
#endif

#ifdef __cplusplus
}
#endif

/** @} */
