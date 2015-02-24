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
 * @brief       Neighbor Information Base implementation for NHDP
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include "timex.h"
#include "mutex.h"
#include "vtimer.h"
#include "utlist.h"

#include "rfc5444/rfc5444_iana.h"
#include "rfc5444/rfc5444_writer.h"

#include "nib_table.h"
#include "iib_table.h"
#include "nhdp_address.h"
#include "nhdp_writer.h"

/* Internal variables */
static mutex_t mtx_nib_access = MUTEX_INIT;
static nib_entry_t *nib_entry_head = NULL;
static nib_lost_address_entry_t *nib_lost_address_entry_head = NULL;

/* Internal function prototypes */
static nib_entry_t *add_nib_entry(nhdp_addr_entry_t *nb_list);
static void rem_nib_entry(nib_entry_t *nib_entry, nhdp_addr_entry_t *nb_list,
                          nhdp_addr_entry_t **out_list, timex_t *now);
static void clear_nb_addresses(nib_entry_t *nib_entry, nhdp_addr_entry_t *nb_list,
                               nhdp_addr_entry_t **out_list, timex_t *now);
static int add_lost_neighbor_address(nhdp_addr_t *lost_addr, timex_t *now);
static void rem_ln_entry(nib_lost_address_entry_t *ln_entry);


/*---------------------------------------------------------------------------*
 *                       Neighbor Information Base API                       *
 *---------------------------------------------------------------------------*/

nib_entry_t *nib_process_hello(nhdp_addr_entry_t *nb_list, nhdp_addr_entry_t **out_list)
{
    nib_entry_t *nb_match = NULL;
    timex_t now;
    uint8_t matches = 0;

    mutex_lock(&mtx_nib_access);

    if (nb_list) {
        nib_entry_t *nib_elt, *nib_tmp;

        vtimer_now(&now);

        LL_FOREACH_SAFE(nib_entry_head, nib_elt, nib_tmp) {
            nhdp_addr_entry_t *list_elt;
            LL_FOREACH(nib_elt->address_list_head, list_elt) {
                nhdp_addr_entry_t *list_elt2;
                LL_FOREACH(nb_list, list_elt2) {
                    if (list_elt->address == list_elt2->address) {
                        /* Addresses are equal (same NHDP address db entry) */
                        matches++;

                        if (matches > 1) {
                            /* Multiple matching nb tuples, delete the previous one */
                            iib_propagate_nb_entry_change(nb_match, nib_elt);
                            rem_nib_entry(nb_match, nb_list, out_list, &now);
                        }

                        nb_match = nib_elt;
                        break;
                    }
                }

                if (nb_match == nib_elt) {
                    /* This nb tuple is already detected as matching */
                    break;
                }
            }
        }

        /* Add or update nb tuple */
        if (matches > 0) {
            /* We found matching nb tuples, reuse the last one */
            clear_nb_addresses(nb_match, nb_list, out_list, &now);

            if (matches > 1) {
                nb_match->symmetric = 0;
            }

            nb_match->address_list_head = nhdp_generate_new_addr_list(nb_list);

            if (!nb_match->address_list_head) {
                /* Insufficient memory */
                LL_DELETE(nib_entry_head, nb_match);
                free(nb_match);
                nb_match = NULL;
            }
        }
        else {
            nb_match = add_nib_entry(nb_list);
        }
    }

    mutex_unlock(&mtx_nib_access);
    return nb_match;
}

void nib_fill_wr_addresses(struct rfc5444_writer *wr)
{
    nib_entry_t *nib_elt;
    nhdp_addr_entry_t *addr_elt;
    nib_lost_address_entry_t *lost_elt, *lost_tmp;
    timex_t now;

    mutex_lock(&mtx_nib_access);

    vtimer_now(&now);

    /* Add addresses of symmetric neighbors to HELLO msg */
    LL_FOREACH(nib_entry_head, nib_elt) {
        if (nib_elt->symmetric) {
            LL_FOREACH(nib_elt->address_list_head, addr_elt) {
                /* Check whether address is not already included with link status symmetric */
                if (!NHDP_ADDR_TMP_IN_SYM(addr_elt->address)) {
                    nhdp_writer_add_addr(wr, addr_elt->address, RFC5444_ADDRTLV_OTHER_NEIGHB,
                                         RFC5444_OTHERNEIGHB_SYMMETRIC);
                    addr_elt->address->in_tmp_table = NHDP_ADDR_TMP_SYM;
                }
            }
        }
    }

    /* Add lost addresses of neighbors to HELLO msg */
    LL_FOREACH_SAFE(nib_lost_address_entry_head, lost_elt, lost_tmp) {
        if (timex_cmp(lost_elt->expiration_time, now) != 1) {
            /* Entry expired, remove it */
            rem_ln_entry(lost_elt);
        }
        else {
            /* Check if address is not already present in one of the temporary lists */
            if (!NHDP_ADDR_TMP_IN_ANY(lost_elt->address)) {
                /* Address is not present in one of the lists, add it */
                nhdp_writer_add_addr(wr, lost_elt->address, RFC5444_ADDRTLV_OTHER_NEIGHB,
                                     RFC5444_OTHERNEIGHB_LOST);
            }
        }
    }

    mutex_unlock(&mtx_nib_access);
}

void nib_rem_nb_entry(nib_entry_t *nib_entry)
{
    nhdp_free_addr_list(nib_entry->address_list_head);
    LL_DELETE(nib_entry_head, nib_entry);
    free(nib_entry);
}

void nib_set_nb_entry_sym(nib_entry_t *nib_entry)
{
    nib_lost_address_entry_t *ln_elt, *ln_tmp;
    nhdp_addr_entry_t *nb_elt;

    nib_entry->symmetric = 1;
    LL_FOREACH(nib_entry->address_list_head, nb_elt) {
        LL_FOREACH_SAFE(nib_lost_address_entry_head, ln_elt, ln_tmp) {
            /* Remove all Lost Neighbor Tuples matching an address of the newly sym nb */
            if (ln_elt->address == nb_elt->address) {
                rem_ln_entry(ln_elt);
                break;
            }
        }
    }
}

void nib_reset_nb_entry_sym(nib_entry_t *nib_entry, timex_t *now)
{
    nhdp_addr_entry_t *nb_elt;

    nib_entry->symmetric = 0;
    LL_FOREACH(nib_entry->address_list_head, nb_elt) {
        /* Add a Lost Neighbor Tuple for each address of the neighbor */
        if (add_lost_neighbor_address(nb_elt->address, now) == -1) {
            /* Insufficient memory */
            return;
        }
    }
}


/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/

/**
 * Add a Neighbor Tuple with the given address list
 */
static nib_entry_t *add_nib_entry(nhdp_addr_entry_t *nb_list)
{
    nib_entry_t *new_elem;

    new_elem = malloc(sizeof(nib_entry_t));

    if (!new_elem) {
        /* Insufficient memory */
        return NULL;
    }

    /* Copy address list to new neighbor tuple */
    new_elem->address_list_head = nhdp_generate_new_addr_list(nb_list);

    if (!new_elem->address_list_head) {
        /* Insufficient memory */
        free(new_elem);
        return NULL;
    }

    new_elem->symmetric = 0;
    LL_PREPEND(nib_entry_head, new_elem);

    return new_elem;
}

/**
 * Remove a given Neighbor Tuple
 */
static void rem_nib_entry(nib_entry_t *nib_entry, nhdp_addr_entry_t *nb_list,
                          nhdp_addr_entry_t **out_list, timex_t *now)
{
    clear_nb_addresses(nib_entry, nb_list, out_list, now);
    LL_DELETE(nib_entry_head, nib_entry);
    free(nib_entry);
}

/**
 * Clear address list of a Neighbor Tuple and add Lost Neighbor Tuple for addresses
 * no longer used by this neighbor
 */
static void clear_nb_addresses(nib_entry_t *nib_entry, nhdp_addr_entry_t *nb_list,
                               nhdp_addr_entry_t **out_list, timex_t *now)
{
    nhdp_addr_entry_t *elt, *nib_elt, *nib_tmp;
    uint8_t found;

    LL_FOREACH_SAFE(nib_entry->address_list_head, nib_elt, nib_tmp) {
        found = 0;
        LL_FOREACH(nb_list, elt) {
            /* Check whether address is still present in the new neighbor address list */
            if (nib_elt->address == elt->address) {
                /* Simply free the address entry */
                nhdp_free_addr_entry(nib_elt);
                found = 1;
                break;
            }
        }

        if (!found) {
            /* Address is not in the newly received address list of the neighbor */
            /* Add it to the Removed Address List (out_list) */
            LL_PREPEND(*out_list, nib_elt);

            if (nib_entry->symmetric) {
                /* Additionally create a Lost Neighbor Tuple for symmetric neighbors */
                add_lost_neighbor_address(nib_elt->address, now);
            }
        }
    }
    nib_entry->address_list_head = NULL;
}

/**
 * Add or update a Lost Neighbor Tuple
 */
static int add_lost_neighbor_address(nhdp_addr_t *lost_addr, timex_t *now)
{
    nib_lost_address_entry_t *elt;
    timex_t n_hold = timex_from_uint64(((uint64_t)NHDP_N_HOLD_TIME_MS) * MS_IN_USEC);

    LL_FOREACH(nib_lost_address_entry_head, elt) {
        if (elt->address == lost_addr) {
            /* Existing entry for this address, no need to add a new one */
            if (timex_cmp(elt->expiration_time, *now) == -1) {
                /* Entry expired, so just update expiration time */
                elt->expiration_time = timex_add(*now, n_hold);
            }

            return 0;
        }
    }

    /* No existing entry, create a new one */
    elt = malloc(sizeof(nib_lost_address_entry_t));

    if (!elt) {
        /* Insufficient memory */
        return -1;
    }

    /* Increment usage counter of address in central NHDP address storage */
    lost_addr->usg_count++;
    elt->address = lost_addr;
    elt->expiration_time = timex_add(*now, n_hold);
    LL_PREPEND(nib_lost_address_entry_head, elt);

    return 0;
}

/**
 * Remove a given Lost Neighbor Tuple
 */
static void rem_ln_entry(nib_lost_address_entry_t *ln_entry)
{
    nhdp_decrement_addr_usage(ln_entry->address);
    LL_DELETE(nib_lost_address_entry_head, ln_entry);
    free(ln_entry);
}
