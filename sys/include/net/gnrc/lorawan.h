/*
 * Copyright (C) 2017 Fundación Inria Chile
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_lorawan GNRC LoRaWAN
 * @ingroup     net_gnrc
 * @brief       GNRC LoRaWAN stack implementation
 *
 * @{
 *
 * @file
 * @brief   GNRC LoRaWAN API definition
 *
 * @author  José Ignacio Alamos <jose.alamos@haw-hamburg.de>
 * @author  Francisco Molina <femolina@uc.cl>
 */
#ifndef NET_GNRC_LORAWAN_H
#define NET_GNRC_LORAWAN_H

#include "gnrc_lorawan_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_gnrc_lorawan_conf GNRC LoRaWAN compile configurations
 * @ingroup net_gnrc_conf
 * @{
 */
/**
 * @brief maximum timer drift in per mille
 *
 * @note this is only a workaround to compensate inaccurate timers.
 *
 * E.g a value of 1 means there's a positive drift of 0.1% (set timeout to
 * 1000 ms => triggers after 1001 ms)
 */
#ifndef CONFIG_GNRC_LORAWAN_TIMER_DRIFT
#define CONFIG_GNRC_LORAWAN_TIMER_DRIFT 10
#endif

/**
 * @brief the minimum symbols to detect a LoRa preamble
 */
#ifndef CONFIG_GNRC_LORAWAN_MIN_SYMBOLS_TIMEOUT
#define CONFIG_GNRC_LORAWAN_MIN_SYMBOLS_TIMEOUT 30
#endif
/** @} */

#define GNRC_LORAWAN_REQ_STATUS_SUCCESS (0)     /**< MLME or MCPS request successful status */
#define GNRC_LORAWAN_REQ_STATUS_DEFERRED (1)    /**< the MLME or MCPS confirm message is asynchronous */

/**
 * @brief MCPS events
 */
typedef enum {
    MCPS_EVENT_RX,              /**< MCPS RX event */
    MCPS_EVENT_NO_RX,           /**< MCPS no RX event */
} mcps_event_t;

/**
 * @brief LoRaWAN activation mechanism
 */
typedef enum {
    MLME_ACTIVATION_NONE,       /**< MAC layer is not activated */
    MLME_ACTIVATION_ABP,        /**< MAC layer activated by ABP */
    MLME_ACTIVATION_OTAA        /**< MAC layer activated by OTAA */
} mlme_activation_t;

/**
 * @brief MAC Information Base attributes
 */
typedef enum {
    MIB_ACTIVATION_METHOD,      /**< type is activation method */
    MIB_DEV_ADDR,               /**< type is dev addr */
    MIB_RX2_DR,                 /**< type is rx2 DR */
} mlme_mib_type_t;

/**
 * @brief MLME primitive types
 */
typedef enum {
    MLME_JOIN,                  /**< join a LoRaWAN network */
    MLME_LINK_CHECK,            /**< perform a Link Check */
    MLME_RESET,                 /**< reset the MAC layer */
    MLME_SET,                   /**< set the MIB */
    MLME_GET,                   /**< get the MIB */
    MLME_SCHEDULE_UPLINK        /**< schedule uplink indication */
} mlme_type_t;

/**
 * @brief MCPS primitive types
 */
typedef enum {
    MCPS_CONFIRMED,             /**< confirmed data */
    MCPS_UNCONFIRMED            /**< unconfirmed data */
} mcps_type_t;

/**
 * @brief MAC Information Base descriptor for MLME Request-Confirm
 */
typedef struct {
    mlme_mib_type_t type;               /**< MIB attribute identifier */
    union {
        mlme_activation_t activation;   /**< holds activation mechanism */
        void *dev_addr;                 /**< pointer to the dev_addr */
        uint8_t rx2_dr;                 /** datarate of second rx window */
    };
} mlme_mib_t;

/**
 * @brief MAC (sub) Layer Management Entity (MLME) request representation
 */
typedef struct {
    union {
        mlme_lorawan_join_t join;   /**< Join Data holder */
        mlme_mib_t mib;             /**< MIB holder */
    };
    mlme_type_t type;               /**< type of the MLME request */
} mlme_request_t;

/**
 * @brief Mac Common Part Sublayer (MCPS) request representation
 */
typedef struct {
    union {
        mcps_data_t data;   /**< MCPS data holder */
    };
    mcps_type_t type;       /**< type of the MCPS request */
} mcps_request_t;

/**
 * @brief MAC (sub) Layer Management Entity (MLME) confirm representation
 */
typedef struct {
    int16_t status;                         /**< status of the MLME confirm */
    mlme_type_t type;                       /**< type of the MLME confirm */
    union {
        mlme_link_req_confirm_t link_req;   /**< Link Check confirmation data */
        mlme_mib_t mib;                     /**< MIB confirmation data */
    };
} mlme_confirm_t;

/**
 * @brief Mac Common Part Sublayer (MCPS) confirm representation
 */
typedef struct {
    int16_t status;     /**< status of the MCPS confirm */
    mcps_type_t type;   /**< type of the MCPS confirm */
    iolist_t *msdu;     /**< pointer to the msdu */
} mcps_confirm_t;

/**
 * @brief Mac Common Part Sublayer (MCPS) indication representation
 */
typedef struct {
    mcps_type_t type;       /**< type of the MCPS indication */
    union {
        mcps_data_t data;   /**< MCPS Data holder */
    };
} mcps_indication_t;

/**
 * @brief MAC (sub) Layer Management Entity (MLME) indication representation
 */
typedef struct {
    mlme_type_t type; /**< type of the MLME indication */
} mlme_indication_t;

/**
 * @brief Indicate the MAC layer there was a timeout event
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void gnrc_lorawan_radio_rx_timeout_cb(gnrc_lorawan_t *mac);

/**
 * @brief Indicate the MAC layer when the transmission finished
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void gnrc_lorawan_radio_tx_done_cb(gnrc_lorawan_t *mac);

/**
 * @brief Indicate the MAC layer that the timer was fired
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void gnrc_lorawan_timeout_cb(gnrc_lorawan_t *mac);

/**
 * @brief Init GNRC LoRaWAN
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] nwkskey buffer to store the NwkSKey. Should be at least 16 bytes long
 * @param[in] appskey buffer to store the AppsKey. Should be at least 16 bytes long
 */
void gnrc_lorawan_init(gnrc_lorawan_t *mac, uint8_t *nwkskey, uint8_t *appskey);

/**
 * @brief Perform a MLME request
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] mlme_request the MLME request
 * @param[out] mlme_confirm the MLME confirm. `mlme_confirm->status` could either
 *             be GNRC_LORAWAN_REQ_STATUS_SUCCESS if the request was OK,
 *             GNRC_LORAWAN_REQ_STATUS_DEFERRED if the confirmation is deferred
 *             or an standard error number
 */
void gnrc_lorawan_mlme_request(gnrc_lorawan_t *mac,
                               const mlme_request_t *mlme_request,
                               mlme_confirm_t *mlme_confirm);

/**
 * @brief Perform a MCPS request
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] mcps_request the MCPS request
 * @param[out] mcps_confirm the MCPS confirm. `mlme_confirm->status` could either
 *             be GNRC_LORAWAN_REQ_STATUS_SUCCESS if the request was OK,
 *             GNRC_LORAWAN_REQ_STATUS_DEFERRED if the confirmation is deferred
 *             or an standard error number
 */
void gnrc_lorawan_mcps_request(gnrc_lorawan_t *mac,
                               const mcps_request_t *mcps_request,
                               mcps_confirm_t *mcps_confirm);

/**
 * @brief Fetch a LoRaWAN packet from the radio.
 *
 *        To be called on radio RX done event.
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] data pointer to the psdu. Pass NULL if the packet was wrong (or
 * allocation failed)
 * @param[in] size size of the PSDU
 */
void gnrc_lorawan_radio_rx_done_cb(gnrc_lorawan_t *mac, uint8_t *data,
                                   size_t size);

/**
 * @brief MCPS indication callback
 * @note Supposed to be implemented by the user of GNRC LoRaWAN
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] ind pointer of the indication (see @ref mcps_indication_t)
 */
void gnrc_lorawan_mcps_indication(gnrc_lorawan_t *mac, mcps_indication_t *ind);

/**
 * @brief MLME indication callback
 * @note Supposed to be implemented by the user of GNRC LoRaWAN
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] ind pointer of the indication (see @ref mlme_indication_t)
 */
void gnrc_lorawan_mlme_indication(gnrc_lorawan_t *mac, mlme_indication_t *ind);

/**
 * @brief MCPS Confirm callback
 * @note Supposed to be implemented by the user of GNRC LoRaWAN
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] confirm pointer to the confirm (see @ref mcps_confirm_t)
 */
void gnrc_lorawan_mcps_confirm(gnrc_lorawan_t *mac, mcps_confirm_t *confirm);

/**
 * @brief MLME confirm callback
 * @note Supposed to be implemented by the user of GNRC LoRaWAN
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] confirm pointer to the confirm (see @ref mlme_confirm_t)
 */
void gnrc_lorawan_mlme_confirm(gnrc_lorawan_t *mac, mlme_confirm_t *confirm);

/**
 * @brief Get netdev pointer from mac descriptor
 * @note Supposed to be implemented by the user of GNRC LoRaWAN
 *
 * @param[in] mac pointer to the MAC descriptor
 *
 * @return pointer to the @ref netdev_t structure
 */
netdev_t *gnrc_lorawan_get_netdev(gnrc_lorawan_t *mac);

/**
 * @brief Set the channel mask in order to enable or disable LoRaWAN channels
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] channel_mask the channel mask. LSB maps to channel 0
 *
 * @return 0 on success
 * @return -EINVAL if @p channel_mask is zero or if the channel mask tries to
 *         enable an undefined channel
 */
int gnrc_lorawan_phy_set_channel_mask(gnrc_lorawan_t *mac, uint16_t channel_mask);

/**
 * @brief Set a timer with the given time
 * @note Supposed to be implemented by the user of GNRC LoRaWAN
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param us timeout microseconds
 */
void gnrc_lorawan_set_timer(gnrc_lorawan_t *mac, uint32_t us);

/**
 * @brief Remove the current timer
 * @note Supposed to be implemented by the user of GNRC LoRaWAN
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void gnrc_lorawan_remove_timer(gnrc_lorawan_t *mac);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_LORAWAN_H */
/** @} */
