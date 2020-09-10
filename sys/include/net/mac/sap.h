/*
 * Copyright (C) 2019 HAW Hamburg
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_MAC_SAP_H
#define NET_MAC_SAP_H

#include "sap_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAC_REQ_STATUS_SUCCESS (0)     /**< MLME or MCPS request successful status */
#define MAC_REQ_STATUS_DEFERRED (1)    /**< the MLME or MCPS confirm message is asynchronous */

/**
 * @brief MAC (sub) Layer Management Entity (MLME) request representation
 */
typedef struct {
    union {
        struct _mac_mlme_data_t;    /**< mlme request data types*/
        mlme_mib_t mib;             /**< MIB holder */
    };
    mlme_type_t type;               /**< type of the MLME request */
} mlme_request_t;

/**
 * @brief Mac Common Part Sublayer (MCPS) request representation
 */
typedef struct {
    union {
        mcps_data_t data;           /**< MCPS data holder */
    };
    mcps_type_t type;                /**< type of the MCPS request */
} mcps_request_t;

/**
 * @brief MAC (sub) Layer Management Entity (MLME) confirm representation
 */
typedef struct {
    int16_t status;                  /**< status of the MLME confirm */
    mlme_type_t type;                /**< type of the MLME confirm */
    union {
        mlme_mib_t mib;              /**< MIB confirmation data */
    };
} mlme_confirm_t;

/**
 * @brief Mac Common Part Sublayer (MCPS) confirm representation
 */
typedef struct {
    void *data;                     /**< data of the MCPS confirm */
    int16_t status;                 /**< status of the MCPS confirm */
    mcps_type_t type;               /**< type of the MCPS confirm */
} mcps_confirm_t;

/**
 * @brief Mac Common Part Sublayer (MCPS) indication representation
 */
typedef struct {
    mcps_type_t type;               /**< type of the MCPS indication */
    union {
        mcps_data_t data;           /**< MCPS Data holder */
    };
} mcps_indication_t;

/**
 * @brief MAC (sub) Layer Management Entity (MLME) indication representation
 */
typedef struct {
    mlme_type_t type;               /**< type of the MLME indication */
} mlme_indication_t;

/**
 * @brief Perform a MLME request
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] mlme_request the MLME request
 * @param[out] mlme_confirm the MLME confirm. `mlme_confirm->status` could either
 *             be MAC_REQ_STATUS_SUCCESS if the request was OK,
 *             MAC_REQ_STATUS_DEFERRED if the confirmation is deferred
 *             or a standard error number
 */
void mac_mlme_request(mac_t *mac, const mlme_request_t *mlme_request,
                               mlme_confirm_t *mlme_confirm);

/**
 * @brief Perform a MCPS request
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] mcps_request the MCPS request
 * @param[out] mcps_confirm the MCPS confirm. `mlme_confirm->status` could either
 *             be MAC_REQ_STATUS_SUCCESS if the request was OK,
 *             MAC_REQ_STATUS_DEFERRED if the confirmation is deferred
 *             or a standard error number
 */
void mac_mcps_request(mac_t *mac, const mcps_request_t *mcps_request,
                               mcps_confirm_t *mcps_confirm);

/**
 * @brief MCPS indication callback
 * @note Supposed to be implemented by the user of GNRC LoRaWAN
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] ind pointer of the indication (see @ref mcps_indication_t)
 */
void mac_mcps_indication(mac_t *mac, mcps_indication_t *ind);

/**
 * @brief MLME indication callback
 * @note Supposed to be implemented by the user of GNRC LoRaWAN
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] ind pointer of the indication (see @ref mlme_indication_t)
 */
void mac_mlme_indication(mac_t *mac, mlme_indication_t *ind);

/**
 * @brief MCPS Confirm callback
 * @note Supposed to be implemented by the user of GNRC LoRaWAN
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] confirm pointer to the confirm (see @ref mcps_confirm_t)
 */
void mac_mcps_confirm(mac_t *mac, mcps_confirm_t *confirm);

/**
 * @brief MLME confirm callback
 * @note Supposed to be implemented by the user of GNRC LoRaWAN
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] confirm pointer to the confirm (see @ref mlme_confirm_t)
 */
void mac_mlme_confirm(mac_t *mac, mlme_confirm_t *confirm);

#ifdef __cplusplus
}
#endif

#endif /* NET_MAC_SAP_H */
/** @} */
