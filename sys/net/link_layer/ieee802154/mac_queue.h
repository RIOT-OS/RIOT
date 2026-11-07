/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @{
 *
 * @file
 * @author Moritz Holzer <moritz.holzer@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "mutex.h"

#include "net/ieee802154/mac.h"

/**
 * @brief Whether the TX queue is full.
 */
bool ieee802154_mac_tx_full(const ieee802154_mac_txq_t *txq);

/**
 * @brief Whether the TX queue is empty.
 */
bool ieee802154_mac_tx_empty(const ieee802154_mac_txq_t *txq);

/**
 * @brief Returns a descriptor of TX queue head. Caller fills it.
 */
ieee802154_mac_tx_desc_t *ieee802154_mac_tx_reserve(ieee802154_mac_txq_t *txq);

/**
 * @brief Advances TX queue tail to make it visible to the sender.
 */
void ieee802154_mac_tx_commit(ieee802154_mac_txq_t *txq);

/**
 * @brief Returns descriptor of TX queue head (next to send), does not remove.
 */
ieee802154_mac_tx_desc_t *ieee802154_mac_tx_peek(ieee802154_mac_txq_t *txq);

/**
 * @brief Removes TX queue head entry.
 */
void ieee802154_mac_tx_pop(ieee802154_mac_txq_t *txq);

/**
 * @brief Allocate a slot in the indirect queue.
 */
int ieee802154_indirectq_alloc_slot(ieee802154_mac_indirect_q_t *indirect_q);
/**
 * @brief Free a slot in the indirect queue.
 */
void ieee802154_indirectq_free_slot(ieee802154_mac_indirect_q_t *indirect_q, uint8_t slot);

/**
 * @brief Whether if indirect queue is empty
 */
bool ieee802154_indirectq_empty(const ieee802154_mac_indirect_q_t *indirect_q);

bool ieee802154_indirectq_has_addr(ieee802154_mac_indirect_q_t *indirect_q,
                                   ieee802154_addr_mode_t dst_mode,
                                   const void *dst_addr);

/**
 * @brief Get the deadline tick for indirect transmission.
 */
uint16_t ieee802154_indirect_get_deadline(ieee802154_mac_t *mac);
/**
 * @brief Check whether a frame is expired.
 */
bool ieee802154_mac_frame_is_expired(uint16_t now_tick, uint16_t deadline_tick);

/**
 * @brief Record an associated device's extended-to-short address alias.
 */
void ieee802154_mac_assoc_update(ieee802154_mac_t *mac,
                                 const ieee802154_ext_addr_t *ext_addr,
                                 ieee802154_short_addr_t short_addr);
/**
 * @brief Update the frame pending bit for a destination address.
 */
void ieee802154_mac_indirect_fp_update(ieee802154_mac_t *mac,
                                       ieee802154_addr_mode_t dst_mode,
                                       const void *dst_addr,
                                       bool pending);
/**
 * @brief Auto-free an indirect queue slot when it expires.
 */
void ieee802154_mac_handle_indirectq_auto_free(ieee802154_mac_t *mac,
                                               ieee802154_mac_indirect_q_t *indirect_q,
                                               uint8_t slot);
/**
 * @brief Find an indirect queue slot by destination address.
 */
int ieee802154_mac_indirectq_search_slot(ieee802154_mac_t *mac,
                                         ieee802154_addr_mode_t dst_mode,
                                         const void *dst_addr);
/**
 * @brief Find an indirect queue slot by destination address.
 *
 * The caller must hold @ref ieee802154_mac_indirect_q_t::lock.
 */
int ieee802154_mac_indirectq_search_slot_locked(ieee802154_mac_t *mac,
                                                ieee802154_addr_mode_t dst_mode,
                                                const void *dst_addr);
/**
 * @brief Get a slot index for the given destination address.
 */
int ieee802154_mac_indirectq_get_slot(ieee802154_mac_t *mac,
                                      ieee802154_addr_mode_t dst_mode,
                                      const void *dst_addr);
/**
 * @brief Enqueue a data frame into the MAC TX queue.
 */
int ieee802154_mac_map_push(ieee802154_mac_t *mac,
                            uint8_t frame_type,
                            ieee802154_addr_mode_t src_mode,
                            ieee802154_addr_mode_t dst_mode,
                            const uint16_t *dst_panid,
                            const void *dst_addr,
                            iolist_t *msdu,
                            const uint8_t *msdu_handle,
                            bool ack_req,
                            bool indirect);
/**
 * @brief Enqueue a data request command.
 */
int ieee802154_mac_enqueue_data_request(ieee802154_mac_t *mac,
                                        ieee802154_addr_mode_t dst_mode,
                                        uint16_t *dst_panid,
                                        const void *dst_addr);
/**
 * @brief Enqueue an association request command.
 */
/**
 * @brief Enequeu a beacon request command.
 */
int ieee802154_mac_enqueue_beacon_request(ieee802154_mac_t *mac);

#ifdef __cplusplus
}
#endif

/** @} */
