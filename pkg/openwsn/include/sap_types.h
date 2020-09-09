/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup pkg_openwsn
 * @{
 *
 * @file
 * @brief   OpenWSN SAP MLME MCPS types
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef SAP_TYPES_H
#define SAP_TYPES_H

#include "net/mac.h"

#include "sixtop.h"
#include "schedule.h"
#include "idmanager.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IEEE802154E PAN roles
 */
typedef enum {
    IEEE802154E_ROLE_PAN_COORDINATOR = ROLE_PAN_COORDINATOR,
    IEEE802154E_ROLE_COORDINATOR = ROLE_COORDINATOR,
    IEEE802154E_ROLE_LEAF = ROLE_LEAF,
} iee802154e_pan_role_t;

/**
 * @brief MAC Information Base attributes
 */
typedef enum {
    MIB_ROLE,
    MIB_SLOTFRAME_LENGTH,
    MIB_SLOTFRAME_HANDLE,
} mlme_mib_type_t;

/**
 * @brief MLME primitive types
 */
typedef enum {
    MLME_RESET,                /**< reset the MAC layer */
    MLME_START,                /**< start a new configuration */
    MLME_SET,                  /**< set the MIB */
    MLME_GET,                  /**< get the MIB */
    MLME_SYNC,                 /**< indicate synchronization */
    MLME_SYNC_LOSS,            /**< schedule uplink indication */
    MLME_SET_LINK,             /**< schedule uplink indication */
    MLME_SIXTOP_REQ,           /**< schedule uplink indication */
} mlme_type_t;

/**
 * @brief MCPS primitive types
 */
typedef enum {
    MCPS_CONFIRMED,            /**< confirmed data */
    MCPS_UNCONFIRMED           /**< unconfirmed data */
} mcps_type_t;

/**
 * @brief MAC Information Base descriptor for MLME Request-Confirm
 */
typedef struct {
    mlme_mib_type_t type; /**< MIB attribute identifier */
    uint32_t content;         /**< Value content field. */
} mlme_mib_t;

/**
 * @brief MCPS data
 */
typedef struct {
    uint8_t* addr;    /**< packet of the request */
    uint8_t* data; /**< port of the request */
    size_t len;    /**< datarate of the request */
} mcps_data_t;

/**
 * @brief GNRC LoRaWAN mac descriptor */
typedef struct {
    mac_t mac;
} openwsn_mac_t;

/**
 * @brief MLME Link Request operation types
 */
typedef enum {
    ADD_LINK,          /**< add new link to schedule */
    DELETE_LINK,       /**< remove link from schedule */
    MODIFY_LINK,       /**< modify link in schedule */
} tsch_link_operation_t;

/**
 * @brief MLME Link Request options
 */
typedef enum {
    TX_LINK,            /**< transmit link */
    RX_LINK,            /**< receive link */
    TXRX_LINK,          /**< transmit receive link*/
    ADVERTISING_LINK,   /**< advertising link */
} tsch_link_type_t;

/**
 * @brief MLME Link Request
 */
typedef struct {
    tsch_link_operation_t operation;    /**< type of operation */
    tsch_link_type_t type;              /**< cell options */
    bool shared;                        /**< the link is shared */
    int slot_offset;                    /**< timeslot of the link */
    int channel_offset;                 /**< channel offset of the link */
    uint8_t* addr;                      /**< address of the neighbor connected to the link */
} mlme_tsch_link_request_t;

/**
 * @brief IANA 6top CMD
 */
typedef enum {
    NONE = IANA_6TOP_CMD_NONE,                /**< none */
    SIXTOP_ADD = IANA_6TOP_CMD_ADD,           /**< add one or more cells */
    SIXTOP_REMOVE = IANA_6TOP_CMD_DELETE,     /**< delete one or more cells */
    SIXTOP_RELOCATE = IANA_6TOP_CMD_RELOCATE, /**< relocate one or more cells */
    SIXTOP_COUNT = IANA_6TOP_CMD_COUNT,       /**< count scheduled cells */
    SIXTOP_LIST = IANA_6TOP_CMD_LIST,         /**< list the scheduled cells */
    SIXTOP_SIGNAL = IANA_6TOP_CMD_SIGNAL,     /**< signal command */
    SIXTOP_CLEAR = IANA_6TOP_CMD_CLEAR,       /**< clear all cells */
} sixtop_cmd_type_t;

/**
 * @brief Sixtop cell options
 */
typedef enum {
    TX_CELL = CELLOPTIONS_TX,            /**< transmit cell */
    RX_CELL = CELLOPTIONS_RX,            /**< receive cell */
    SHARED_CELL = CELLOPTIONS_SHARED,    /**< shared cell */
} sixtop_cell_opts_t;

/**
 * @brief SixTop request
 */
typedef struct {
    sixtop_cmd_type_t cmd;      /**< IANA sixtop command */
    uint8_t num;                /**< number of cells to add */
    sixtop_cell_opts_t opts;    /**< type of the Operation */
    uint8_t *addr;              /**< address of the neighbor connected to the link */
} mlme_sixtop_request_t;

/**
 * @brief MAC MLME request data
 */
typedef struct _mac_mlme_data_t {
    union {
        mlme_tsch_link_request_t link; /**< tsch data holder */
        mlme_sixtop_request_t sixtop;  /**< sixtop Data holder */
    };
} mac_mlme_data_t;

#ifdef __cplusplus
}
#endif

#endif /* SAP_TYPES_H */
/** @} */
