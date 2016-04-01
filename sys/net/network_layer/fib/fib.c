/**
 * Copyright (C) 2014 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_fib
 * @{
 *
 * @file
 * @brief       Functions to manage FIB entries
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include "thread.h"
#include "mutex.h"
#include "msg.h"
#include "xtimer.h"
#include "utlist.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "net/fib.h"
#include "net/fib/table.h"

#ifdef MODULE_IPV6_ADDR
#include "net/ipv6/addr.h"
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

#ifdef MODULE_IPV6_ADDR
    #define FIB_ADDR_PRINT_LEN      39
#else
    #define FIB_ADDR_PRINT_LEN      32
    #if FIB_ADDR_PRINT_LEN != (UNIVERSAL_ADDRESS_SIZE * 2)
        #error "FIB_ADDR_PRINT_LEN MUST BE (UNIVERSAL_ADDRESS_SIZE * 2)"
    #endif
#endif

#define FIB_ADDR_PRINT_LENS1(X)     #X
#define FIB_ADDR_PRINT_LENS2(X)     FIB_ADDR_PRINT_LENS1(X)
#define FIB_ADDR_PRINT_LENS         FIB_ADDR_PRINT_LENS2(FIB_ADDR_PRINT_LEN)

/**
 * @brief convert an offset given in ms to abolute time in time in us
 * @param[in]  ms       the milliseconds to be converted
 * @param[out] target   the converted point in time
 */
static void fib_lifetime_to_absolute(uint32_t ms, uint64_t *target)
{
    *target = xtimer_now64() + (ms * 1000);
}

/**
 * @brief returns pointer to the entry for the given destination address
 *
 * @param[in] table                the FIB table to search in
 * @param[in] dst                  the destination address
 * @param[in] dst_size             the destination address size
 * @param[out] entry_arr           the array to scribe the found match
 * @param[in, out] entry_arr_size  the number of entries provided by entry_arr (should be always 1)
 *                                 this value is overwritten with the actual found number
 *
 * @return 0 if we found a next-hop prefix
 *         1 if we found the exact address next-hop
 *         -EHOSTUNREACH if no fitting next-hop is available
 */
static int fib_find_entry(fib_table_t *table, uint8_t *dst, size_t dst_size,
                          fib_entry_t **entry_arr, size_t *entry_arr_size) {
    uint64_t now = xtimer_now64();

    size_t count = 0;
    size_t prefix_size = 0;
    size_t match_size = dst_size << 3;
    int ret = -EHOSTUNREACH;
    bool is_all_zeros_addr = true;

#if ENABLE_DEBUG
    DEBUG("[fib_find_entry] dst =");
    for (size_t i = 0; i < dst_size; i++) {
        DEBUG(" %02x", dst[i]);
    }
    DEBUG("\n");
#endif

    for (size_t i = 0; i < dst_size; ++i) {
        if (dst[i] != 0) {
            is_all_zeros_addr = false;
            break;
        }
    }

    for (size_t i = 0; i < table->size; ++i) {

        /* autoinvalidate if the entry lifetime is not set to not expire */
        if (table->data.entries[i].lifetime != FIB_LIFETIME_NO_EXPIRE) {

            /* check if the lifetime expired */
            if (table->data.entries[i].lifetime < now) {
                /* remove this entry if its lifetime expired */
                table->data.entries[i].lifetime = 0;
                table->data.entries[i].global_flags = 0;
                table->data.entries[i].next_hop_flags = 0;
                table->data.entries[i].iface_id = KERNEL_PID_UNDEF;

                if (table->data.entries[i].global != NULL) {
                    universal_address_rem(table->data.entries[i].global);
                    table->data.entries[i].global = NULL;
                }

                if (table->data.entries[i].next_hop != NULL) {
                    universal_address_rem(table->data.entries[i].next_hop);
                    table->data.entries[i].next_hop = NULL;
                }
            }
        }

        if ((prefix_size < (dst_size<<3)) && (table->data.entries[i].global != NULL)) {

            int ret_comp = universal_address_compare(table->data.entries[i].global, dst, &match_size);
            /* If we found an exact match */
            if ((ret_comp == UNIVERSAL_ADDRESS_EQUAL)
                || (is_all_zeros_addr && (ret_comp == UNIVERSAL_ADDRESS_IS_ALL_ZERO_ADDRESS))) {
                entry_arr[0] = &(table->data.entries[i]);
                *entry_arr_size = 1;
                /* we will not find a better one so we return */
                return 1;
            }
            else {
                /* we try to find the most fitting prefix */
                if (ret_comp == UNIVERSAL_ADDRESS_MATCHING_PREFIX) {
                    if (table->data.entries[i].global_flags & FIB_FLAG_NET_PREFIX_MASK) {
                        /* we shift the most upper flag byte back to get the number of prefix bits */
                        size_t global_prefix_len = (table->data.entries[i].global_flags
                                                    & FIB_FLAG_NET_PREFIX_MASK) >> FIB_FLAG_NET_PREFIX_SHIFT;

                        if ((match_size >= global_prefix_len) &&
                            ((prefix_size == 0) || (match_size > prefix_size))) {
                            entry_arr[0] = &(table->data.entries[i]);
                            /* we could find a better one so we move on */
                            ret = 0;

                            prefix_size = match_size;
                            count = 1;
                        }
                    }
                 }
                 else if (ret_comp == UNIVERSAL_ADDRESS_IS_ALL_ZERO_ADDRESS) {
                    /* we found the default gateway entry, e.g. ::/0 for IPv6
                     * and we keep it only if there is no better one
                     */
                    if (prefix_size == 0) {
                        entry_arr[0] = &(table->data.entries[i]);
                        /* we could find a better one so we move on */
                        ret = 0;
                        count = 1;
                    }
                }
                match_size = dst_size<<3;
            }
        }
    }

#if ENABLE_DEBUG
    if (count > 0) {
        DEBUG("[fib_find_entry] found prefix on interface %d:", entry_arr[0]->iface_id);
        for (size_t i = 0; i < entry_arr[0]->global->address_size; i++) {
            DEBUG(" %02x", entry_arr[0]->global->address[i]);
        }
        DEBUG("\n");
    }
#endif

    *entry_arr_size = count;
    return ret;
}

/**
 * @brief updates the next hop the lifetime and the interface id for a given entry
 *
 * @param[in] entry          the entry to be updated
 * @param[in] next_hop       the next hop address to be updated
 * @param[in] next_hop_size  the next hop address size
 * @param[in] next_hop_flags the next-hop address flags
 * @param[in] lifetime       the lifetime in ms
 *
 * @return 0 if the entry has been updated
 *         -ENOMEM if the entry cannot be updated due to insufficient RAM
 */
static int fib_upd_entry(fib_entry_t *entry, uint8_t *next_hop,
                         size_t next_hop_size, uint32_t next_hop_flags,
                         uint32_t lifetime)
{
    universal_address_container_t *container = universal_address_add(next_hop, next_hop_size);

    if (container == NULL) {
        return -ENOMEM;
    }

    universal_address_rem(entry->next_hop);
    entry->next_hop = container;
    entry->next_hop_flags = next_hop_flags;

    if (lifetime != (uint32_t)FIB_LIFETIME_NO_EXPIRE) {
        fib_lifetime_to_absolute(lifetime, &entry->lifetime);
    }
    else {
        entry->lifetime = FIB_LIFETIME_NO_EXPIRE;
    }

    return 0;
}

/**
 * @brief creates a new FIB entry with the provided parameters
 *
 * @param[in] table          the FIB table to create the entry in
 * @param[in] iface_id       the interface ID
 * @param[in] dst            the destination address
 * @param[in] dst_size       the destination address size
 * @param[in] dst_flags      the destination address flags
 * @param[in] next_hop       the next hop address
 * @param[in] next_hop_size  the next hop address size
 * @param[in] next_hop_flags the next-hop address flags
 * @param[in] lifetime       the lifetime in ms
 *
 * @return 0 on success
 *         -ENOMEM if no new entry can be created
 */
static int fib_create_entry(fib_table_t *table, kernel_pid_t iface_id,
                            uint8_t *dst, size_t dst_size, uint32_t dst_flags,
                            uint8_t *next_hop, size_t next_hop_size, uint32_t
                            next_hop_flags, uint32_t lifetime)
{
    for (size_t i = 0; i < table->size; ++i) {
        if (table->data.entries[i].lifetime == 0) {

            table->data.entries[i].global = universal_address_add(dst, dst_size);

            if (table->data.entries[i].global != NULL) {
                table->data.entries[i].global_flags = dst_flags;
                table->data.entries[i].next_hop = universal_address_add(next_hop, next_hop_size);
                table->data.entries[i].next_hop_flags = next_hop_flags;
            }

            if (table->data.entries[i].next_hop != NULL) {
                /* everything worked fine */
                table->data.entries[i].iface_id = iface_id;

                if (lifetime != (uint32_t) FIB_LIFETIME_NO_EXPIRE) {
                    fib_lifetime_to_absolute(lifetime, &table->data.entries[i].lifetime);
                }
                else {
                    table->data.entries[i].lifetime = FIB_LIFETIME_NO_EXPIRE;
                }

                return 0;
            }
        }
    }

    return -ENOMEM;
}

/**
 * @brief removes the given entry
 *
 * @param[in] entry the entry to be removed
 *
 * @return 0 on success
 */
static int fib_remove(fib_entry_t *entry)
{
    if (entry->global != NULL) {
        universal_address_rem(entry->global);
    }

    if (entry->next_hop) {
        universal_address_rem(entry->next_hop);
    }

    entry->global = NULL;
    entry->global_flags = 0;
    entry->next_hop = NULL;
    entry->next_hop_flags = 0;

    entry->iface_id = KERNEL_PID_UNDEF;
    entry->lifetime = 0;

    return 0;
}

/**
 * @brief signals (sends a message to) all registered routing protocols
 *        registered with a matching prefix (usually this should be only one).
 *        The receiver MUST copy the content, i.e. the address before reply.
 *
 * @param[in] table     the fib instance to use
 * @param[in] type      the kind of signal
 * @param[in] dat       the data to send
 * @param[in] dat_size  the data size in bytes
 * @param[in] dat_flags the data flags
 *
 * @return 0 on a new available entry,
 *         -ENOENT if no suiting entry is provided.
 */
static int fib_signal_rp(fib_table_t *table, uint16_t type, uint8_t *dat,
                         size_t dat_size, uint32_t dat_flags)
{
    msg_t msg, reply;
    rp_address_msg_t rp_addr_msg;
    int ret = -ENOENT;
    void *content = NULL;

    if (type != FIB_MSG_RP_SIGNAL_SOURCE_ROUTE_CREATED) {
        /* the passed data is an address */
        rp_addr_msg.address = dat;
        rp_addr_msg.address_size = dat_size;
        rp_addr_msg.address_flags = dat_flags;
        content = (void *)&rp_addr_msg;
    }
    else {
        /* the passed data is a sr head
         * dat_size and dat_flags are not used in this case
         */
        content = (void *)dat;
    }

    msg.type = type;
    msg.content.ptr = content;

    for (size_t i = 0; i < FIB_MAX_REGISTERED_RP; ++i) {
        if (table->notify_rp[i] != KERNEL_PID_UNDEF) {
            DEBUG("[fib_signal_rp] send msg@: %p to pid[%d]: %d\n", \
                  (void *)msg.content.ptr, (int)i, (int)(table->notify_rp[i]));

            /* do only signal a RP if its registered prefix matches */
            if (type != FIB_MSG_RP_SIGNAL_SOURCE_ROUTE_CREATED) {
                size_t dat_size_in_bits = dat_size<<3;
                if (universal_address_compare(table->prefix_rp[i], dat,
                                              &dat_size_in_bits) != -ENOENT) {
                    /* the receiver, i.e. the RP, MUST copy the content value.
                     * using the provided pointer after replying this message
                     * will lead to errors
                     */
                    msg_send_receive(&msg, &reply, table->notify_rp[i]);
                    DEBUG("[fib_signal_rp] got reply.\n");
                    ret = 0;
                }
            }
            else {
                fib_sr_t *temp_sr = (fib_sr_t *)dat;
                size_t dat_size_in_bits = temp_sr->sr_dest->address->address_size << 3;
                if (universal_address_compare(table->prefix_rp[i],
                                              temp_sr->sr_dest->address->address,
                                              &dat_size_in_bits) != -ENOENT) {
                    /* the receiver, i.e. the RP, MUST copy the content value.
                     * using the provided pointer after replying this message
                     * will lead to errors
                     */
                    msg_send_receive(&msg, &reply, table->notify_rp[i]);
                    DEBUG("[fib_signal_rp] got reply.\n");
                    ret = 0;
                }
            }
        }
    }

    return ret;
}

int fib_add_entry(fib_table_t *table,
                  kernel_pid_t iface_id, uint8_t *dst, size_t dst_size,
                  uint32_t dst_flags, uint8_t *next_hop, size_t next_hop_size,
                  uint32_t next_hop_flags, uint32_t lifetime)
{
    mutex_lock(&(table->mtx_access));
    DEBUG("[fib_add_entry]\n");
    size_t count = 1;
    fib_entry_t *entry[count];

    /* check if dst and next_hop are valid pointers */
    if ((dst == NULL) || (next_hop == NULL)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    int ret = fib_find_entry(table, dst, dst_size, &(entry[0]), &count);

    if (ret == 1) {
        /* we must take the according entry and update the values */
        ret = fib_upd_entry(entry[0], next_hop, next_hop_size, next_hop_flags, lifetime);
    }
    else {
        ret = fib_create_entry(table, iface_id, dst, dst_size, dst_flags,
                               next_hop, next_hop_size, next_hop_flags, lifetime);
    }

    mutex_unlock(&(table->mtx_access));
    return ret;
}

int fib_update_entry(fib_table_t *table, uint8_t *dst, size_t dst_size,
                     uint8_t *next_hop, size_t next_hop_size,
                     uint32_t next_hop_flags, uint32_t lifetime)
{
    mutex_lock(&(table->mtx_access));
    DEBUG("[fib_update_entry]\n");
    size_t count = 1;
    fib_entry_t *entry[count];
    int ret = -ENOMEM;

    /* check if dst and next_hop are valid pointers */
    if ((dst == NULL) || (next_hop == NULL)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_find_entry(table, dst, dst_size, &(entry[0]), &count) == 1) {
        DEBUG("[fib_update_entry] found entry: %p\n", (void *)(entry[0]));
        /* we must take the according entry and update the values */
        ret = fib_upd_entry(entry[0], next_hop, next_hop_size, next_hop_flags, lifetime);
    }
    else {
        /* we have ambiguous entries, i.e. count > 1
         * this should never happen
         */
        DEBUG("[fib_update_entry] ambigious entries detected!!!\n");
    }

    mutex_unlock(&(table->mtx_access));
    return ret;
}

void fib_remove_entry(fib_table_t *table, uint8_t *dst, size_t dst_size)
{
    mutex_lock(&(table->mtx_access));
    DEBUG("[fib_remove_entry]\n");
    size_t count = 1;
    fib_entry_t *entry[count];

    int ret = fib_find_entry(table, dst, dst_size, &(entry[0]), &count);

    if (ret == 1) {
        /* we must take the according entry and update the values */
        fib_remove(entry[0]);
    }
    else {
        /* we have ambiguous entries, i.e. count > 1
         * this should never happen
         */
        DEBUG("[fib_update_entry] ambigious entries detected!!!\n");
    }

    mutex_unlock(&(table->mtx_access));
}

void fib_flush(fib_table_t *table, kernel_pid_t interface)
{
    mutex_lock(&(table->mtx_access));
    DEBUG("[fib_flush]\n");

    for (size_t i = 0; i < table->size; ++i) {
        if ((interface == KERNEL_PID_UNDEF) ||
            (interface == table->data.entries[i].iface_id)) {
            fib_remove(&table->data.entries[i]);
        }
    }

    mutex_unlock(&(table->mtx_access));
}

int fib_get_next_hop(fib_table_t *table, kernel_pid_t *iface_id,
                     uint8_t *next_hop, size_t *next_hop_size,
                     uint32_t *next_hop_flags, uint8_t *dst, size_t dst_size,
                     uint32_t dst_flags)
{
    mutex_lock(&(table->mtx_access));
    DEBUG("[fib_get_next_hop]\n");
    size_t count = 1;
    fib_entry_t *entry[count];

    if ((iface_id == NULL)
        || (next_hop_size == NULL)
        || (next_hop_flags == NULL)) {
            mutex_unlock(&(table->mtx_access));
            return -EINVAL;
        }

    if ((dst == NULL) || (next_hop == NULL)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    int ret = fib_find_entry(table, dst, dst_size, &(entry[0]), &count);
    if (!(ret == 0 || ret == 1)) {
        /* notify all responsible RPs for unknown  next-hop for the destination address */
        if (fib_signal_rp(table, FIB_MSG_RP_SIGNAL_UNREACHABLE_DESTINATION,
                          dst, dst_size, dst_flags) == 0) {
            count = 1;
            /* now lets see if the RRPs have found a valid next-hop */
            ret = fib_find_entry(table, dst, dst_size, &(entry[0]), &count);
        }
    }

    if (ret == 0 || ret == 1) {

        uint8_t *address_ret = universal_address_get_address(entry[0]->next_hop,
                               next_hop, next_hop_size);

        if (address_ret == NULL) {
            mutex_unlock(&(table->mtx_access));
            return -ENOBUFS;
        }
    }
    else {
        mutex_unlock(&(table->mtx_access));
        return -EHOSTUNREACH;
    }

    *iface_id = entry[0]->iface_id;
    *next_hop_flags = entry[0]->next_hop_flags;
    mutex_unlock(&(table->mtx_access));
    return 0;
}

int fib_get_destination_set(fib_table_t *table, uint8_t *prefix,
                            size_t prefix_size,
                            fib_destination_set_entry_t *dst_set,
                            size_t* dst_set_size)
{
    mutex_lock(&(table->mtx_access));
    int ret = -EHOSTUNREACH;
    size_t found_entries = 0;

    for (size_t i = 0; i < table->size; ++i) {
        if ((table->data.entries[i].global != NULL) &&
            (universal_address_compare_prefix(table->data.entries[i].global, prefix, prefix_size<<3) >= UNIVERSAL_ADDRESS_EQUAL)) {
            if( (dst_set != NULL) && (found_entries < *dst_set_size) ) {
            /* set the size to full byte usage */
            dst_set[found_entries].dest_size = sizeof(dst_set[found_entries].dest);
            universal_address_get_address(table->data.entries[i].global,
                                          dst_set[found_entries].dest,
                                          &dst_set[found_entries].dest_size);
            }
            found_entries++;
        }
    }

    if (found_entries > *dst_set_size) {
        ret = -ENOBUFS;
    }
    else if (found_entries > 0) {
        ret = 0;
    }

    *dst_set_size = found_entries;

    mutex_unlock(&(table->mtx_access));

    return ret;
}

void fib_init(fib_table_t *table)
{
    DEBUG("[fib_init] hello. Initializing some stuff.\n");
    mutex_init(&(table->mtx_access));
    mutex_lock(&(table->mtx_access));

    for (size_t i = 0; i < FIB_MAX_REGISTERED_RP; ++i) {
        table->notify_rp[i] = KERNEL_PID_UNDEF;
        table->prefix_rp[i] = NULL;
    }

    table->notify_rp_pos = 0;

    if (table->table_type == FIB_TABLE_TYPE_SR) {
        memset(table->data.source_routes->headers, 0,
               sizeof(fib_sr_t) * table->size);
        memset(table->data.source_routes->entry_pool, 0,
               sizeof(fib_sr_entry_t) * table->data.source_routes->entry_pool_size);
    }
    else {
        memset(table->data.entries, 0, (table->size * sizeof(fib_entry_t)));
    }
    universal_address_init();
    mutex_unlock(&(table->mtx_access));
}

void fib_deinit(fib_table_t *table)
{
    DEBUG("[fib_deinit] hello. De-Initializing stuff.\n");
    mutex_lock(&(table->mtx_access));

    for (size_t i = 0; i < FIB_MAX_REGISTERED_RP; ++i) {
        table->notify_rp[i] = KERNEL_PID_UNDEF;
        table->prefix_rp[i] = NULL;
    }

    table->notify_rp_pos = 0;

    if (table->table_type == FIB_TABLE_TYPE_SR) {
        memset(table->data.source_routes->headers, 0,
               sizeof(fib_sr_t) * table->size);
        memset(table->data.source_routes->entry_pool, 0,
               sizeof(fib_sr_entry_t) * table->data.source_routes->entry_pool_size);
    }
    else {
        memset(table->data.entries, 0, (table->size * sizeof(fib_entry_t)));
    }
    universal_address_reset();
    mutex_unlock(&(table->mtx_access));
}

int fib_register_rp(fib_table_t *table, uint8_t *prefix, size_t prefix_addr_type_size)
{
    mutex_lock(&(table->mtx_access));

    if (table->notify_rp_pos >= FIB_MAX_REGISTERED_RP) {
        mutex_unlock(&(table->mtx_access));
        return -ENOMEM;
    }

    if ((prefix == NULL) || (prefix_addr_type_size == 0)) {
        mutex_unlock(&(table->mtx_access));
        return -EINVAL;
    }

    if (table->notify_rp_pos < FIB_MAX_REGISTERED_RP) {
        table->notify_rp[table->notify_rp_pos] = sched_active_pid;
        universal_address_container_t *container = universal_address_add(prefix,
                                                                         prefix_addr_type_size);
        table->prefix_rp[table->notify_rp_pos] = container;
        table->notify_rp_pos++;
    }

    mutex_unlock(&(table->mtx_access));
    return 0;
}

int fib_get_num_used_entries(fib_table_t *table)
{
    mutex_lock(&(table->mtx_access));
    size_t used_entries = 0;

    for (size_t i = 0; i < table->size; ++i) {
        used_entries += (size_t)(table->data.entries[i].global != NULL);
    }

    mutex_unlock(&(table->mtx_access));
    return used_entries;
}

/* source route handling */
int fib_sr_create(fib_table_t *table, fib_sr_t **fib_sr, kernel_pid_t sr_iface_id,
                  uint32_t sr_flags, uint32_t sr_lifetime)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (sr_lifetime == 0)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    for (size_t i = 0; i < table->size; ++i) {
        if (table->data.source_routes->headers[i].sr_lifetime == 0) {
            table->data.source_routes->headers[i].sr_iface_id = sr_iface_id;
            table->data.source_routes->headers[i].sr_flags = sr_flags;
            table->data.source_routes->headers[i].sr_path = NULL;
            table->data.source_routes->headers[i].sr_dest = NULL;
            if (sr_lifetime < (uint32_t)FIB_LIFETIME_NO_EXPIRE) {
                fib_lifetime_to_absolute(sr_lifetime,
                                         &table->data.source_routes->headers[i].sr_lifetime);
            }
            else {
                table->data.source_routes->headers[i].sr_lifetime = FIB_LIFETIME_NO_EXPIRE;
            }
            *fib_sr = &table->data.source_routes->headers[i];
            mutex_unlock(&(table->mtx_access));
            return 0;
        }
    }

    mutex_unlock(&(table->mtx_access));
    return -ENOBUFS;
}

/**
* @brief Internal function:
*        checks the lifetime and removes the entry in case it expired
*/
static int fib_sr_check_lifetime(fib_sr_t *fib_sr)
{
    uint64_t tm = fib_sr->sr_lifetime - xtimer_now64();
    /* check if the lifetime expired */
    if ((int64_t)tm < 0) {
        /* remove this sr if its lifetime expired */
        fib_sr->sr_lifetime = 0;

        if (fib_sr->sr_path != NULL) {
            fib_sr_entry_t *elt;
            LL_FOREACH(fib_sr->sr_path, elt) {
                universal_address_rem(elt->address);
            }
            fib_sr->sr_path = NULL;
        }

        /* and return an errorcode */
        return -ENOENT;
    }
    return 0;
}

/**
* @brief Internal function:
*        creates a new entry in the table entry pool for a hop in a source route
*/
static int fib_sr_new_entry(fib_table_t *table, uint8_t *addr, size_t addr_size,
                            fib_sr_entry_t **new_entry)
{
    for (size_t i = 0; i < table->data.source_routes->entry_pool_size; ++i) {
        if (table->data.source_routes->entry_pool[i].address == NULL) {
            table->data.source_routes->entry_pool[i].address = universal_address_add(addr, addr_size);
            if (table->data.source_routes->entry_pool[i].address == NULL) {
                return -ENOMEM;
            }
            else {
                (void)new_entry;
                *new_entry = &table->data.source_routes->entry_pool[i];
                return 0;
            }
        }
    }
    return -ENOMEM;
}

/**
* @brief Internal function:
*        checks if the source route belongs to the given table
*/
static int fib_is_sr_in_table(fib_table_t *table, fib_sr_t *fib_sr)
{
    for (size_t i = 0; i < table->size; ++i) {
        if (&(table->data.source_routes->headers[i]) == fib_sr) {
            return 0;
        }
    }
    return -ENOENT;
}

int fib_sr_read_head(fib_table_t *table, fib_sr_t *fib_sr, kernel_pid_t *iface_id,
                     uint32_t *sr_flags, uint32_t *sr_lifetime)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (iface_id == NULL) || (sr_flags == NULL)
        || (sr_lifetime == NULL) || (fib_is_sr_in_table(table, fib_sr) == -ENOENT) ) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    *iface_id = fib_sr->sr_iface_id;
    *sr_flags = fib_sr->sr_flags;
    *sr_lifetime = fib_sr->sr_lifetime - xtimer_now64();

    mutex_unlock(&(table->mtx_access));
    return 0;
}

int fib_sr_read_destination(fib_table_t *table, fib_sr_t *fib_sr,
                            uint8_t *dst, size_t *dst_size)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (dst == NULL) || (dst_size == NULL)
        || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    if (fib_sr->sr_dest == NULL) {
        mutex_unlock(&(table->mtx_access));
        return -EHOSTUNREACH;
    }

    if (universal_address_get_address(fib_sr->sr_dest->address, dst, dst_size) == NULL) {
        mutex_unlock(&(table->mtx_access));
        return -ENOBUFS;
    }

    mutex_unlock(&(table->mtx_access));
    return 0;
}

int fib_sr_set(fib_table_t *table, fib_sr_t *fib_sr, kernel_pid_t *sr_iface_id,
               uint32_t *sr_flags, uint32_t *sr_lifetime)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    if (sr_iface_id != NULL) {
        fib_sr->sr_iface_id = *sr_iface_id;
    }

    if (sr_flags != NULL) {
        fib_sr->sr_flags = *sr_flags;
    }

    if (sr_lifetime != NULL) {
        fib_lifetime_to_absolute(*sr_lifetime, &(fib_sr->sr_lifetime));
    }

    mutex_unlock(&(table->mtx_access));
    return 0;
}

int fib_sr_delete(fib_table_t *table, fib_sr_t *fib_sr)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    fib_sr->sr_lifetime = 0;

    if (fib_sr->sr_path != NULL) {
        fib_sr_entry_t *elt, *tmp;
        LL_FOREACH_SAFE(fib_sr->sr_path, elt, tmp) {
            universal_address_rem(elt->address);
            elt->address = NULL;
            LL_DELETE(fib_sr->sr_path, elt);
        }
        fib_sr->sr_path = NULL;
    }

    mutex_unlock(&(table->mtx_access));
    return 0;
}

int fib_sr_next(fib_table_t *table, fib_sr_t *fib_sr, fib_sr_entry_t **sr_path_entry)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (sr_path_entry == NULL)
        || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr->sr_path == NULL) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    /* if we reach the destination entry, i.e. the last entry we just return 1 */
    if (*sr_path_entry == fib_sr->sr_dest) {
        mutex_unlock(&(table->mtx_access));
        return 1;
    }

    /* when we start, we pass the first entry */
    if (*sr_path_entry == NULL) {
        *sr_path_entry = fib_sr->sr_path;
    }
    else {
        /* in any other case we just return the next entry */
        *sr_path_entry = (*sr_path_entry)->next;
    }

    mutex_unlock(&(table->mtx_access));
    return 0;
}

int fib_sr_search(fib_table_t *table, fib_sr_t *fib_sr, uint8_t *addr, size_t addr_size,
                  fib_sr_entry_t **sr_path_entry)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (addr == NULL) || (sr_path_entry == NULL)
        || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    fib_sr_entry_t *elt;
    LL_FOREACH(fib_sr->sr_path, elt) {
        size_t addr_size_match = addr_size << 3;
        if (universal_address_compare(elt->address, addr, &addr_size_match) == UNIVERSAL_ADDRESS_EQUAL) {

            /* temporary workaround to calm compiler */
            (void)sr_path_entry;

            *sr_path_entry = elt;
            mutex_unlock(&(table->mtx_access));
            return 0;
        }
    }

    mutex_unlock(&(table->mtx_access));
    return -EHOSTUNREACH;
}

int fib_sr_entry_append(fib_table_t *table, fib_sr_t *fib_sr,
                        uint8_t *addr, size_t addr_size)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (addr == NULL)
        || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    fib_sr_entry_t *elt;
    LL_FOREACH(fib_sr->sr_path, elt) {
        size_t addr_size_match = addr_size << 3;
        if (universal_address_compare(elt->address, addr, &addr_size_match) == UNIVERSAL_ADDRESS_EQUAL) {
            mutex_unlock(&(table->mtx_access));
            return -EINVAL;
        }
    }

    fib_sr_entry_t *new_entry[1];
    int ret = fib_sr_new_entry(table, addr, addr_size, &new_entry[0]);

    if (ret == 0) {
        fib_sr_entry_t *tmp = fib_sr->sr_dest;
        if (tmp != NULL) {
            /* we append the new entry behind the former destination */
            tmp->next = new_entry[0];
        }
        else {
            /* this is also our first entry */
            fib_sr->sr_path = new_entry[0];
        }
        fib_sr->sr_dest = new_entry[0];
    }

    mutex_unlock(&(table->mtx_access));
    return ret;
}

int fib_sr_entry_add(fib_table_t *table, fib_sr_t *fib_sr,
                     fib_sr_entry_t *sr_path_entry, uint8_t *addr, size_t addr_size,
                     bool keep_remaining_route)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (sr_path_entry == NULL) || (addr == NULL)
        || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    bool found = false;
    fib_sr_entry_t *elt;
    LL_FOREACH(fib_sr->sr_path, elt) {
        size_t addr_size_match = addr_size << 3;
        if (universal_address_compare(elt->address, addr, &addr_size_match) == UNIVERSAL_ADDRESS_EQUAL) {
            mutex_unlock(&(table->mtx_access));
            return -EINVAL;
        }
        if (sr_path_entry == elt) {
            found = true;
            break;
        }
    }

    int ret = -ENOENT;
    if (found) {
        fib_sr_entry_t *new_entry[1];
        ret = fib_sr_new_entry(table, addr, addr_size, &new_entry[0]);
        if (ret == 0) {
            fib_sr_entry_t *remaining = sr_path_entry->next;
            sr_path_entry->next = new_entry[0];
            if (keep_remaining_route) {
                new_entry[0]->next = remaining;
            }
            else {
                fib_sr_entry_t *elt, *tmp;
                LL_FOREACH_SAFE(remaining, elt, tmp) {
                    universal_address_rem(elt->address);
                    elt->address = NULL;
                    LL_DELETE(remaining, elt);
                }
                new_entry[0]->next = NULL;
                fib_sr->sr_dest = new_entry[0];
            }
        }
    }

    mutex_unlock(&(table->mtx_access));
    return ret;
}

int fib_sr_entry_delete(fib_table_t *table, fib_sr_t *fib_sr, uint8_t *addr, size_t addr_size,
                        bool keep_remaining_route)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    fib_sr_entry_t *elt, *tmp;
    tmp = fib_sr->sr_path;
    LL_FOREACH(fib_sr->sr_path, elt) {
        size_t addr_size_match = addr_size << 3;

        if (universal_address_compare(elt->address, addr, &addr_size_match) == UNIVERSAL_ADDRESS_EQUAL) {
            universal_address_rem(elt->address);
            if (keep_remaining_route) {
                tmp->next = elt->next;
            }
            else {
                fib_sr_entry_t *elt_del, *tmp_del;
                LL_FOREACH_SAFE(tmp, elt_del, tmp_del) {
                    universal_address_rem(elt_del->address);
                    elt_del->address = NULL;
                    LL_DELETE(tmp, elt_del);
                }
            }
            if (elt == fib_sr->sr_path) {
                /* if we remove the first entry we must adjust the path start */
                fib_sr->sr_path = elt->next;
            }
            if (elt == fib_sr->sr_dest) {
                /* if we remove the last entry we must adjust the destination */
                fib_sr->sr_dest = tmp;
            }
            mutex_unlock(&(table->mtx_access));
            return 0;
        }
        tmp = elt;
    }

    return -ENOENT;
}

int fib_sr_entry_overwrite(fib_table_t *table, fib_sr_t *fib_sr,
                           uint8_t *addr_old, size_t addr_old_size,
                           uint8_t *addr_new, size_t addr_new_size)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (addr_old == NULL) || (addr_new == NULL)
        || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    fib_sr_entry_t *elt, *elt_repl;
    elt_repl = NULL;
    LL_FOREACH(fib_sr->sr_path, elt) {
        size_t addr_old_size_match = addr_old_size << 3;
        size_t addr_new_size_match = addr_old_size << 3;
        if (universal_address_compare(elt->address, addr_old, &addr_old_size_match) == UNIVERSAL_ADDRESS_EQUAL) {
            elt_repl = elt;
        }

        if (universal_address_compare(elt->address, addr_new, &addr_new_size_match) == UNIVERSAL_ADDRESS_EQUAL) {
            mutex_unlock(&(table->mtx_access));
            return -EINVAL;
        }
    }

    if (elt_repl != NULL) {
        universal_address_rem(elt_repl->address);
        universal_address_container_t *add = universal_address_add(addr_new, addr_new_size);

        if (add == NULL) {
            /* if this happened we deleted one entry, i.e. decreased the usecount
             * adding a new one was not possible since lack of memory
             * so we add back the old entry, i.e. increasing the usecount
             */
            universal_address_add(addr_old, addr_old_size);
            mutex_unlock(&(table->mtx_access));
            return -ENOMEM;
        }
        elt_repl->address = add;
    }

    mutex_unlock(&(table->mtx_access));
    return 0;
}

int fib_sr_entry_get_address(fib_table_t *table, fib_sr_t *fib_sr, fib_sr_entry_t *sr_entry,
                             uint8_t *addr, size_t *addr_size)
{
    mutex_lock(&(table->mtx_access));
    if ((fib_sr == NULL) || (fib_is_sr_in_table(table, fib_sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    if (fib_sr_check_lifetime(fib_sr) == -ENOENT) {
        mutex_unlock(&(table->mtx_access));
        return -ENOENT;
    }

    fib_sr_entry_t *elt;
    LL_FOREACH(fib_sr->sr_path, elt) {
        if (elt == sr_entry) {
            if (universal_address_get_address(elt->address, addr, addr_size) != NULL) {
                mutex_unlock(&(table->mtx_access));
                return 0;
            }
            else {
                mutex_unlock(&(table->mtx_access));
                return -ENOMEM;
            }
        }
    }
    mutex_unlock(&(table->mtx_access));
    return -ENOENT;
}

/**
 * @brief helper function to search a partial path to a given destination,
 *         and iff successful to create a new source route
 *
 * @param[in] table the fib table the entry should be added to
 * @param[in] dst pointer to the destination address bytes
 * @param[in] dst_size the size in bytes of the destination address type
 * @param[in] check_free_entry position to start the search for a free entry
 * @param[out] error the state of of this operation when finished
 *
 * @return pointer to the new source route on success
 *         NULL otherwise
*/
static fib_sr_t* _fib_create_sr_from_partial(fib_table_t *table, uint8_t *dst, size_t dst_size,
                                             int check_free_entry, int *error) {
fib_sr_t* hit = NULL;

    for (size_t i = 0; i < table->size; ++i) {
        if (table->data.source_routes->headers[i].sr_lifetime != 0) {

            fib_sr_entry_t *elt;
            LL_FOREACH(table->data.source_routes->headers[i].sr_path, elt) {
                size_t addr_size_match = dst_size << 3;
                if (universal_address_compare(elt->address, dst, &addr_size_match) == UNIVERSAL_ADDRESS_EQUAL) {
                    /* we create a new sr */
                    if (check_free_entry == -1) {
                        /* we have no room to create a new sr
                         * so we just retrun and NOT tell the RPs to find a route
                         * since we cannot save it
                         */
                        *error = -ENOBUFS;
                        return NULL;
                    }
                    else {
                        /* we check if there is a free place for the new sr */
                        fib_sr_t *new_sr = NULL;
                        for (size_t j = check_free_entry; j < table->size; ++j) {
                            if (table->data.source_routes->headers[j].sr_lifetime != 0) {
                                /* not this one, maybe the next one */
                                continue;
                            }
                            else {

                                /* there it is, so we copy the header */
                                new_sr = &table->data.source_routes->headers[j];
                                new_sr->sr_iface_id = table->data.source_routes->headers[i].sr_iface_id;
                                new_sr->sr_flags = table->data.source_routes->headers[i].sr_flags;
                                new_sr->sr_lifetime = table->data.source_routes->headers[i].sr_lifetime;
                                new_sr->sr_path = NULL;

                                /* and the path until the searched destination */
                                fib_sr_entry_t *elt_iter, *elt_add = NULL;

                                LL_FOREACH(table->data.source_routes->headers[i].sr_path, elt_iter) {
                                    fib_sr_entry_t *new_entry;

                                    if (fib_sr_new_entry(table, elt_iter->address->address,
                                                         elt_iter->address->address_size,
                                                         &new_entry) != 0) {
                                        /* we could not create a new entry
                                         * so we return to clean up the partial route
                                         */
                                        *error = -ENOBUFS;
                                        return new_sr;
                                    }

                                    if (new_sr->sr_path == NULL) {
                                        new_sr->sr_path = new_entry;
                                        elt_add = new_sr->sr_path;
                                    }
                                    else {
                                        elt_add->next = new_entry;
                                        elt_add = elt_add->next;
                                    }

                                    if (elt_iter == elt) {
                                        /* we copied until the destination */
                                        new_sr->sr_dest = new_entry;
                                        hit = new_sr;

                                        /* tell the RPs that a new sr has been created
                                         * the size and the flags parameters are ignored
                                         */
                                        if (fib_signal_rp(table, FIB_MSG_RP_SIGNAL_SOURCE_ROUTE_CREATED,
                                                      (uint8_t *)new_sr, 0, 0) != 0) {
                                            /* if no RP can handle the source route
                                             * then the host is not directly reachable
                                             */
                                            *error = -EHOSTUNREACH;
                                        }

                                        /* break from iterating for copy */
                                        break;
                                    }
                                }
                            }
                        }

                        /* break from iterating the found path */
                        break;
                    }
                }
            }
            if (hit != NULL) {
                /* break iterating all sr since we have a path now */
                break;
            }
        }
    }
    return hit;
}

int fib_sr_get_route(fib_table_t *table, uint8_t *dst, size_t dst_size, kernel_pid_t *sr_iface_id,
                     uint32_t *sr_flags,
                     uint8_t *addr_list, size_t *addr_list_elements, size_t *element_size,
                     bool reverse, fib_sr_t **fib_sr)
{
    mutex_lock(&(table->mtx_access));

    if ((dst == NULL) || (sr_iface_id == NULL) || (sr_flags == NULL)
        || (addr_list == NULL) || (addr_list_elements == NULL) || (element_size == NULL)) {
        mutex_unlock(&(table->mtx_access));
        return -EFAULT;
    }

    fib_sr_t *hit = NULL;
    fib_sr_t *tmp_hit = NULL;
    int check_free_entry = -1;

    bool skip = (fib_sr != NULL) && (*fib_sr != NULL)?true:false;
    /* Case 1 - check if we know a direct route */
    for (size_t i = 0; i < table->size; ++i) {

        if (fib_sr_check_lifetime(&table->data.source_routes->headers[i]) == -ENOENT) {
            /* expired, so skip this sr and remember its position */
            if (check_free_entry == -1) {
                /* we want to fill up the source routes from the beginning */
                check_free_entry = i;
            }
            continue;
        }

        if( skip ) {
            if(*fib_sr == &table->data.source_routes->headers[i]) {
                skip = false;
            }
            /* we skip all entries upon the consecutive one to start search */
            continue;
        }

        size_t addr_size_match = dst_size << 3;
        if (universal_address_compare(table->data.source_routes->headers[i].sr_dest->address,
                                      dst, &addr_size_match) == UNIVERSAL_ADDRESS_EQUAL) {
            if (*sr_flags == table->data.source_routes->headers[i].sr_flags) {
                /* found a perfect matching sr, no need to search further */
                hit = &table->data.source_routes->headers[i];
                tmp_hit = NULL;
                if (check_free_entry == -1) {
                    check_free_entry = i;
                }
                break;
            }
            else {
                /* found a sr to the destination but with different flags,
                 * maybe we find a better one.
                 */
                tmp_hit = &table->data.source_routes->headers[i];
            }
        }
    }

    if (hit == NULL) {
        /* we didn't find a perfect sr, but one with distinct flags */
        hit = tmp_hit;
    }

    /* Case 2 - if no hit is found check if there is a matching entry in one sr_path
     * @note the first match wins, if we find one we will NOT continue searching,
     * since this search is very expensive in terms of compare operations
    */
    if (hit == NULL) {
        int error = 0;
        hit = _fib_create_sr_from_partial(table, dst, dst_size, check_free_entry, &error);
        if ((error != 0) && (error != -EHOSTUNREACH)) {
            /* something went wrong, so we clean up our mess
             *
             * @note we could handle -EHOSTUNREACH differently here,
             * since it says that we have a partial source route but no RP
             * to manage it.
             * Thats why I let it pass for now.
             */
            if (hit != NULL) {
                hit->sr_lifetime = 0;

                if (hit->sr_path != NULL) {
                    fib_sr_entry_t *elt, *tmp;
                    LL_FOREACH_SAFE(hit->sr_path, elt, tmp) {
                        universal_address_rem(elt->address);
                        elt->address = NULL;
                        LL_DELETE(hit->sr_path, elt);
                    }
                    hit->sr_path = NULL;
                }
            }
            mutex_unlock(&(table->mtx_access));
            return error;
        }
    }

    /* Final step - copy the list in the desired order */
    if (hit != NULL) {

        /* store the current hit to enable consecutive searches */
        if( fib_sr != NULL ) {
            *fib_sr = hit;
        }

        /* check the list size and if the sr entries will fit */
        int count;
        fib_sr_entry_t *elt = NULL;
        LL_COUNT(hit->sr_path, elt, count);

        if (((size_t)count > *addr_list_elements)
            || (sizeof(hit->sr_path->address->address) > *element_size)) {
            *addr_list_elements = count;
            *element_size = sizeof(hit->sr_path->address->address);
            mutex_unlock(&(table->mtx_access));
            return -ENOBUFS;
        }

        /* start copy the individual entries in the desired order */
        uint8_t *next_entry = addr_list;
        int one_address_size = *element_size;

        if (reverse) {
            /* we move to the last list element */
            next_entry += (count - 1) * sizeof(hit->sr_path->address->address);
            /* and set the storing direction during the iteration */
            one_address_size *= -1;
        }

        elt = NULL;
        LL_FOREACH(hit->sr_path, elt) {
            size_t tmp_size = sizeof(hit->sr_path->address->address);
            universal_address_get_address(elt->address, next_entry, &tmp_size);
            next_entry += one_address_size;
        }
        *sr_iface_id = hit->sr_iface_id;
        *sr_flags = hit->sr_flags;
        *addr_list_elements = count;
        *element_size = sizeof(hit->sr_path->address->address);
    }
    else {

        /* trigger RPs for route discovery */
        fib_signal_rp(table, FIB_MSG_RP_SIGNAL_UNREACHABLE_DESTINATION, dst, dst_size, *sr_flags);

        mutex_unlock(&(table->mtx_access));
        return -EHOSTUNREACH;
    }

    mutex_unlock(&(table->mtx_access));

    if (tmp_hit == NULL) {
        return 0;
    }
    else {
        return 1;
    }
}

/* print functions */

void fib_print_notify_rp(fib_table_t *table)
{
    mutex_lock(&(table->mtx_access));

    for (size_t i = 0; i < FIB_MAX_REGISTERED_RP; ++i) {
        printf("[fib_print_notify_rp] pid[%d]: %d\n", (int)i, (int)(table->notify_rp[i]));
    }

    mutex_unlock(&(table->mtx_access));
}

void fib_print_fib_table(fib_table_t *table)
{
    mutex_lock(&(table->mtx_access));

    for (size_t i = 0; i < table->size; ++i) {
        printf("[fib_print_table] %d) iface_id: %d, global: %p, next hop: %p, lifetime: %"PRIu32"\n",
               (int)i, (int)table->data.entries[i].iface_id,
               (void *)table->data.entries[i].global,
               (void *)table->data.entries[i].next_hop,
               (uint32_t)(table->data.entries[i].lifetime / 1000));
    }

    mutex_unlock(&(table->mtx_access));
}

void fib_print_sr(fib_table_t *table, fib_sr_t *sr)
{
    /* does not adjust the lifetime */
    mutex_lock(&(table->mtx_access));
    if ((sr == NULL) || (fib_is_sr_in_table(table, sr) == -ENOENT)) {
        mutex_unlock(&(table->mtx_access));
        return;
    }

    printf("\n-= Source route (%p) =-\nIface: %d\nflags: %x\npath: %p\ndest: ",
           (void *)sr, sr->sr_iface_id, (unsigned int)sr->sr_flags, (void *)sr->sr_path);

    if (sr->sr_dest != NULL) {
        universal_address_print_entry(sr->sr_dest->address);
    } else {
        puts("Not set.");
    }

    fib_sr_entry_t *nxt = sr->sr_path;
    while (nxt) {
        universal_address_print_entry(nxt->address);
        nxt = nxt->next;
    }
    printf("-= END (%p) =-\n", (void *)sr);

    mutex_unlock(&(table->mtx_access));
}

static void fib_print_address(universal_address_container_t *entry)
{
    uint8_t address[UNIVERSAL_ADDRESS_SIZE];
    size_t addr_size = UNIVERSAL_ADDRESS_SIZE;
    uint8_t *ret = universal_address_get_address(entry, address, &addr_size);

    if (ret == address) {
#ifdef MODULE_IPV6_ADDR
        if (addr_size == sizeof(ipv6_addr_t)) {
            printf("%-" FIB_ADDR_PRINT_LENS "s",
                    ipv6_addr_to_str(addr_str, (ipv6_addr_t *) address, sizeof(addr_str)));
            return;
        }
#endif
        for (size_t i = 0; i < UNIVERSAL_ADDRESS_SIZE; ++i) {
            if (i <= addr_size) {
                printf("%02x", address[i]);
            }
            else {
                printf("  ");
            }
        }
#ifdef MODULE_IPV6_ADDR
        /* print trailing whitespaces */
        for (size_t i = 0; i < FIB_ADDR_PRINT_LEN - (UNIVERSAL_ADDRESS_SIZE * 2); ++i) {
            printf(" ");
        }
#endif
    }
}

void fib_print_routes(fib_table_t *table)
{
    mutex_lock(&(table->mtx_access));
    uint64_t now = xtimer_now64();

    if (table->table_type == FIB_TABLE_TYPE_SH) {
        printf("%-" FIB_ADDR_PRINT_LENS "s %-17s %-" FIB_ADDR_PRINT_LENS "s %-10s %-16s"
                " Interface\n" , "Destination", "Flags", "Next Hop", "Flags", "Expires");

        for (size_t i = 0; i < table->size; ++i) {
            if (table->data.entries[i].lifetime != 0) {
                fib_print_address(table->data.entries[i].global);
                printf(" 0x%08"PRIx32" ", table->data.entries[i].global_flags);
                if(table->data.entries[i].global_flags & FIB_FLAG_NET_PREFIX_MASK) {
                    uint32_t prefix = (table->data.entries[i].global_flags
                                       & FIB_FLAG_NET_PREFIX_MASK);
                    printf("N /%-3d ", (int)(prefix >> FIB_FLAG_NET_PREFIX_SHIFT));
                } else {
                    printf("H      ");
                }

                fib_print_address(table->data.entries[i].next_hop);
                printf(" 0x%08"PRIx32" ", table->data.entries[i].next_hop_flags);
                if (table->data.entries[i].lifetime != FIB_LIFETIME_NO_EXPIRE) {

                    uint64_t tm = table->data.entries[i].lifetime - now;

                    /* we must interpret the values as signed */
                    if ((int64_t)tm < 0 ) {
                        printf("%-16s ", "EXPIRED");
                    }
                    else {
                        printf("%"PRIu32".%05"PRIu32, (uint32_t)(tm / 1000000),
                               (uint32_t)(tm % 1000000));
                    }
                }
                else {
                    printf("%-16s ", "NEVER");
                }

                printf("%d\n", (int)table->data.entries[i].iface_id);
            }
        }
    }
    else if (table->table_type == FIB_TABLE_TYPE_SR) {
        printf("%-" FIB_ADDR_PRINT_LENS "s %-" FIB_ADDR_PRINT_LENS "s %-6s %-16s Interface\n"
               , "SR Destination", "SR First Hop", "SR Flags", "Expires");
        for (size_t i = 0; i < table->size; ++i) {
            if (table->data.source_routes->headers[i].sr_lifetime != 0) {
                fib_print_address(table->data.source_routes->headers[i].sr_dest->address);
                fib_print_address(table->data.source_routes->headers[i].sr_path->address);
                printf(" 0x%04"PRIx32" ", table->data.source_routes->headers[i].sr_flags);

                if (table->data.source_routes->headers[i].sr_lifetime != FIB_LIFETIME_NO_EXPIRE) {

                    uint64_t tm = table->data.source_routes->headers[i].sr_lifetime - now;

                    /* we must interpret the values as signed */
                    if ((int64_t)tm < 0 ) {
                        printf("%-16s ", "EXPIRED");
                    }
                    else {
                        printf("%"PRIu32".%05"PRIu32, (uint32_t)(tm / 1000000),
                               (uint32_t)(tm % 1000000));
                    }
                }
                else {
                    printf("%-16s ", "NEVER");
                }

                printf("%d\n", (int)table->data.source_routes->headers[i].sr_iface_id);
            }
        }
    }
    mutex_unlock(&(table->mtx_access));
}

#if FIB_DEVEL_HELPER
int fib_devel_get_lifetime(fib_table_t *table, uint64_t *lifetime, uint8_t *dst,
                           size_t dst_size)
{
    if (table->table_type == FIB_TABLE_TYPE_SH) {
        size_t count = 1;
        fib_entry_t *entry[count];

        int ret = fib_find_entry(table, dst, dst_size, &(entry[0]), &count);
        if (ret == 1 ) {
            /* only return lifetime of exact matches */
            *lifetime = entry[0]->lifetime;
            return 0;
        }
        return -EHOSTUNREACH;
    }
    else if (table->table_type == FIB_TABLE_TYPE_SR) {
        size_t addr_size_match = dst_size << 3;
        /* first hit wins here */
        for (size_t i = 0; i < table->size; ++i) {
            if (universal_address_compare(table->data.source_routes->headers[i].sr_dest->address,
                                        dst, &addr_size_match) == UNIVERSAL_ADDRESS_EQUAL) {
                *lifetime = table->data.source_routes->headers[i].sr_lifetime;
                return 0;
            }
        }
        return -EHOSTUNREACH;
    }
    return -EFAULT;
}
#endif
