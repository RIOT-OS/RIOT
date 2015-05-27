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
 * @brief       Local Information Base implementation for NHDP
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include "mutex.h"
#include "utlist.h"
#include "kernel_types.h"

#include "rfc5444/rfc5444_iana.h"
#include "rfc5444/rfc5444_writer.h"

#include "lib_table.h"
#include "nhdp_address.h"
#include "nhdp_writer.h"

/* Internal variables */
static mutex_t mtx_lib_access = MUTEX_INIT;
static lib_entry_t *lib_entry_head = NULL;

/* Internal function prototypes */
static int create_if_entry(kernel_pid_t if_pid, nhdp_addr_t *addr);
static int add_address_to_if(lib_entry_t *if_entry, nhdp_addr_t *addr);


/*---------------------------------------------------------------------------*
 *                       Local Information Base API                          *
 *---------------------------------------------------------------------------*/

int lib_add_if_addr(kernel_pid_t if_pid, nhdp_addr_t *addr)
{
    lib_entry_t *lib_elt;
    nhdp_addr_entry_t *addr_elt;
    int result = -1;

    mutex_lock(&mtx_lib_access);

    /* Check whether the given interface is already registered */
    LL_FOREACH(lib_entry_head, lib_elt) {
        if (lib_elt->if_pid == if_pid) {
            LL_FOREACH(lib_entry_head->if_addr_list_head, addr_elt) {
                if (addr_elt->address == addr) {
                    /* Address already known for the interface */
                    result = 0;
                    break;
                }
            }

            if (result) {
                /* Existing interface entry, but new address */
                result = add_address_to_if(lib_elt, addr);
                break;
            }
        }
    }

    if (result) {
        /* New interface, create a lib entry */
        result = create_if_entry(if_pid, addr);
    }

    mutex_unlock(&mtx_lib_access);

    return result;
}

void lib_rem_if(kernel_pid_t if_pid)
{
    lib_entry_t *lib_elt, *lib_tmp;

    mutex_lock(&mtx_lib_access);

    LL_FOREACH_SAFE(lib_entry_head, lib_elt, lib_tmp) {
        if (lib_elt->if_pid == if_pid) {
            nhdp_free_addr_list(lib_elt->if_addr_list_head);
            LL_DELETE(lib_entry_head, lib_elt);
            free(lib_elt);
            break;
        }
    }

    mutex_unlock(&mtx_lib_access);
}

void lib_fill_wr_addresses(kernel_pid_t if_pid, struct rfc5444_writer *wr)
{
    lib_entry_t *lib_elt;
    nhdp_addr_entry_t *add_tmp;

    mutex_lock(&mtx_lib_access);

    /* First fill the list for LOCAL_IF = THIS_IF */
    LL_FOREACH(lib_entry_head, lib_elt) {
        if (lib_elt->if_pid == if_pid) {
            LL_FOREACH(lib_elt->if_addr_list_head, add_tmp) {
                nhdp_writer_add_addr(wr, add_tmp->address,
                                     RFC5444_ADDRTLV_LOCAL_IF, RFC5444_LOCALIF_THIS_IF,
                                     NHDP_METRIC_UNKNOWN, NHDP_METRIC_UNKNOWN);
                add_tmp->address->in_tmp_table = NHDP_ADDR_TMP_ANY;
            }
            break;
        }
    }

    /* Second fill the list for LOCAL_IF = OTHER_IF */
    LL_FOREACH(lib_entry_head, lib_elt) {
        if (lib_elt->if_pid != if_pid) {
            LL_FOREACH(lib_elt->if_addr_list_head, add_tmp) {
                /* Check if this address is not already included in a list */
                if (!NHDP_ADDR_TMP_IN_ANY(add_tmp->address)) {
                    /* Address can be added */
                    nhdp_writer_add_addr(wr, add_tmp->address,
                                         RFC5444_ADDRTLV_LOCAL_IF, RFC5444_LOCALIF_OTHER_IF,
                                         NHDP_METRIC_UNKNOWN, NHDP_METRIC_UNKNOWN);
                    add_tmp->address->in_tmp_table = NHDP_ADDR_TMP_ANY;
                }
            }
        }
    }

    mutex_unlock(&mtx_lib_access);
}

uint8_t lib_is_reg_addr(kernel_pid_t if_pid, nhdp_addr_t *addr)
{
    lib_entry_t *lib_elt;
    nhdp_addr_entry_t *addr_elt;

    LL_FOREACH(lib_entry_head, lib_elt) {
        LL_FOREACH(lib_elt->if_addr_list_head, addr_elt) {
            if (addr_elt->address == addr) {
                if (lib_elt->if_pid == if_pid) {
                    /* Given address is assigned to the given IF */
                    return 1;
                }

                /* Given address is assigned to any other IF */
                return 2;
            }
        }
    }
    return 0;
}


/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/

/**
 * Create an entry for a newly registered interface
 */
static int create_if_entry(kernel_pid_t if_pid, nhdp_addr_t *addr)
{
    lib_entry_t *new_entry;

    new_entry = (lib_entry_t *) malloc(sizeof(lib_entry_t));

    if (!new_entry) {
        /* Insufficient memory */
        return -1;
    }

    new_entry->if_addr_list_head = NULL;
    new_entry->if_pid = if_pid;

    if (add_address_to_if(new_entry, addr)) {
        /* Insufficient memory */
        free(new_entry);
        return -1;
    }

    LL_PREPEND(lib_entry_head, new_entry);

    return 0;
}

/**
 * Add another address to an interface entry
 */
static int add_address_to_if(lib_entry_t *if_entry, nhdp_addr_t *addr)
{
    nhdp_addr_entry_t *new_entry = (nhdp_addr_entry_t *) malloc(sizeof(nhdp_addr_entry_t));

    if (!new_entry) {
        /* Insufficient memory */
        return -1;
    }

    /* Increment usage counter of address in central NHDP address storage */
    addr->usg_count++;
    new_entry->address = addr;
    LL_PREPEND(if_entry->if_addr_list_head, new_entry);

    return 0;
}
