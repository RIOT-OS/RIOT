/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     nhdp
 * @{
 *
 * @file
 * @brief       Neighbor Information Base interface for NHDP
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef NIB_TABLE_H
#define NIB_TABLE_H

#include "timex.h"

#include "rfc5444/rfc5444_writer.h"

#include "nhdp_address.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Neighbor Set entry (neighbor tuple)
 */
typedef struct nib_entry {
    nhdp_addr_entry_t *address_list_head;   /**< Pointer to this tuple's addresses*/
    uint8_t symmetric;                      /**< Flag whether sym link to this nb exists */
    uint32_t metric_in;                     /**< Lowest metric value for incoming link */
    uint32_t metric_out;                    /**< Lowest metric value for outgoing link */
    struct nib_entry *next;                 /**< Pointer to next list entry */
} nib_entry_t;

/**
 * @brief   Lost Neighbor Set entry (lost neighbor tuple, lnt)
 */
typedef struct nib_lost_address_entry {
    nhdp_addr_t *address;                   /**< Pointer to addr represented by this lnt */
    timex_t expiration_time;                /**< Time at which entry expires */
    struct nib_lost_address_entry *next;    /**< Pointer to next list entry */
} nib_lost_address_entry_t;

/**
 * @brief                   Process a received HELLO message in the NIB
 *
 * @note
 * Must not be called from outside the NHDP reader's message processing.
 *
 * @return                  Pointer to the new Neighbor Tuple
 * @return                  NULL on error
 */
nib_entry_t *nib_process_hello(void);

/**
 * @brief                   Add addresses to the currently constructed HELLO message
 *
 * @note
 * Must not be called from outside the NHDP writer's message creation process.
 *
 * @param[in] wr            The NHDP writer used for message construction
 */
void nib_fill_wr_addresses(struct rfc5444_writer *wr);

/**
 * @brief                   Remove a Neighbor Tuple
 *
 * @param[in] nib_entry     Pointer to the Neighbor Tuple
 */
void nib_rem_nb_entry(nib_entry_t *nib_entry);

/**
 * @brief                   Set a Neighbor Tuple's symmetry flag
 *
 * Removes all Lost Neighbor Tuples representing addresses included in the
 * Neighbor Tuple's address list.
 *
 * @param[in] nib_entry     Pointer to the Neighbor Tuple
 */
void nib_set_nb_entry_sym(nib_entry_t *nib_entry);

/**
 * @brief                   Reset a Neighbor Tuple's symmetry flag
 *
 * Adds a Lost Neighbor Tuple for every address in the Neighbor Tuple's
 * address list.
 *
 * @param[in] nib_entry     Pointer to the Neighbor Tuple
 * @param[in] now           Pointer to current time timex representation
 */
void nib_reset_nb_entry_sym(nib_entry_t *nib_entry, timex_t *now);

#ifdef __cplusplus
}
#endif

#endif /* NIB_TABLE_H */
/** @} */
