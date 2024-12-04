/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_nimble_scanlist Scan Result Helper
 * @ingroup     pkg_nimble
 * @brief       List for storing and printing BLE scan results
 *
 * @note        This scanlist implementation is not thread safe. So calling
 *              nimble_scanlist_update() in between nimble_scanlist_get*()
 *              calls is not a good idea.
 * @{
 *
 * @file
 * @brief       List for keeping scanned BLE devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NIMBLE_SCANLIST_H
#define NIMBLE_SCANLIST_H

#include "clist.h"
#include "net/ble.h"
#include "nimble/ble.h"
#include "nimble_scanner.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default number of list entries that are allocated in RAM
 */
#ifndef NIMBLE_SCANLIST_SIZE
#define NIMBLE_SCANLIST_SIZE               (20U)
#endif

/**
 * @brief   Data structure for holding a single scanlist entry
 */
typedef struct {
    clist_node_t node;              /**< list node */
    ble_addr_t addr;                /**< a node's BLE address */
    uint8_t ad[BLE_ADV_PDU_LEN];    /**< the received raw advertising data */
    uint8_t ad_len;                 /**< length of the advertising data */
    int8_t last_rssi;               /**< last RSSI of a scanned node */
    uint32_t adv_msg_cnt;           /**< number of adv packets by a node */
    uint32_t first_update;          /**< first packet timestamp */
    uint32_t last_update;           /**< last packet timestamp */
    uint8_t type;                   /**< advertising packet type */
    uint8_t phy_pri;                /**< primary PHY used */
    uint8_t phy_sec;                /**< secondary PHY advertised */
} nimble_scanlist_entry_t;

/**
 * @brief   Initialize the scanlist
 */
void nimble_scanlist_init(void);

/**
 * @brief   Add/update the entry for the a scanned node
 *
 * If the list is already full, the scanned node is simply ignored.
 *
 * @param[in]  type     type of the advertising packet received
 * @param[in]  addr     BLE address of the scanned node
 * @param[in]  info     Additional information on received advertising event
 * @param[in]  ad       the payload of the advertising packet
 * @param[in]  len      length of @p ad
 */
void nimble_scanlist_update(uint8_t type, const ble_addr_t *addr,
                            const nimble_scanner_info_t *info,
                            const uint8_t *ad, size_t len);

/**
 * @brief   Get an entry of the scanlist by its position
 *
 * @param[in] pos       position in the list
 *
 * @return  the scanlist entry at position @p pos
 * @return  NULL if there is no entry a the given position
 */
nimble_scanlist_entry_t *nimble_scanlist_get_by_pos(unsigned pos);

/**
 * @brief   Get the next entry from the list, pass NULL for getting the first
 *          entry
 *
 * @param[in] e         any entry in the list
 *
 * @return  the entry following @p e
 * @return  NULL if @p e was the last entry
 */
nimble_scanlist_entry_t *nimble_scanlist_get_next(nimble_scanlist_entry_t *e);

/**
 * @brief   Clear all entries in the list
 */
void nimble_scanlist_clear(void);

/**
 * @brief   Dump the entire scanlist to STDIO using nimble_scanlist_print_entry()
 */
void nimble_scanlist_print(void);

/**
 * @brief   Dump a single scanlist entry to STDIO
 *
 * @param[in] e         entry to dump
 */
void nimble_scanlist_print_entry(nimble_scanlist_entry_t *e);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NIMBLE_SCANLIST_H */
