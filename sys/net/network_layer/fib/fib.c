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
 * @brief access mutex to control exclusive operations on calls
 */
static mutex_t mtx_access = MUTEX_INIT;

/**
 * @brief maximum number of handled routing protocols (RP)
 *        used to notify the saved kernel_pid_t on ureachable destination
 */
#define FIB_MAX_REGISTERED_RP (5)

/**
 * @brief registered RPs for notifications about unreachable destinations
 */
static size_t notify_rp_pos = 0;

/**
 * @brief the kernel_pid_t for notifying the RPs
 */
static kernel_pid_t notify_rp[FIB_MAX_REGISTERED_RP];

/**
 * @brief the prefix handled by the RP
 */
static universal_address_container_t* prefix_rp[FIB_MAX_REGISTERED_RP];

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
    size_t match_size = dst_size<<3;
    int ret = -EHOSTUNREACH;
    bool is_all_zeros_addr = true;

    for(size_t i = 0; i < dst_size; ++i) {
        if (dst[i] != 0) {
            is_all_zeros_addr = false;
            break;
        }
    }

    for (size_t i = 0; i < table->size; ++i) {

        /* autoinvalidate if the entry lifetime is not set to not expire */
        if (table->entries[i].lifetime != FIB_LIFETIME_NO_EXPIRE) {

            /* check if the lifetime expired */
            if (table->entries[i].lifetime < now) {
                /* remove this entry if its lifetime expired */
                table->entries[i].lifetime = 0;
                table->entries[i].global_flags = 0;
                table->entries[i].next_hop_flags = 0;
                table->entries[i].iface_id = KERNEL_PID_UNDEF;

                if (table->entries[i].global != NULL) {
                    universal_address_rem(table->entries[i].global);
                    table->entries[i].global = NULL;
                }

                if (table->entries[i].next_hop != NULL) {
                    universal_address_rem(table->entries[i].next_hop);
                    table->entries[i].next_hop = NULL;
                }
            }
        }

        if ((prefix_size < (dst_size<<3)) && (table->entries[i].global != NULL)) {

            int ret_comp = universal_address_compare(table->entries[i].global, dst, &match_size);
            /* If we found an exact match */
            if (ret_comp == 0 || (is_all_zeros_addr && match_size == 0)) {
                entry_arr[0] = &(table->entries[i]);
                *entry_arr_size = 1;
                /* we will not find a better one so we return */
                return 1;
            }
            else {
                /* we try to find the most fitting prefix */
                if (ret_comp == 1) {
                    entry_arr[0] = &(table->entries[i]);
                    /* we could find a better one so we move on */
                    ret = 0;

                    prefix_size = match_size;
                    match_size = dst_size<<3;
                    count = 1;
                }
            }
        }
    }

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
        if (table->entries[i].lifetime == 0) {

            table->entries[i].global = universal_address_add(dst, dst_size);

            if (table->entries[i].global != NULL) {
                table->entries[i].global_flags = dst_flags;
                table->entries[i].next_hop = universal_address_add(next_hop, next_hop_size);
                table->entries[i].next_hop_flags = next_hop_flags;
            }

            if (table->entries[i].next_hop != NULL) {
                /* everything worked fine */
                table->entries[i].iface_id = iface_id;

                if (lifetime != (uint32_t) FIB_LIFETIME_NO_EXPIRE) {
                    fib_lifetime_to_absolute(lifetime, &table->entries[i].lifetime);
                }
                else {
                    table->entries[i].lifetime = FIB_LIFETIME_NO_EXPIRE;
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
 *        The message informs the recipient that no next-hop is available for the
 *        requested destination address.
 *        The receiver MUST copy the content, i.e. the address before reply.
 *
 * @param[in] dst       the destination address
 * @param[in] dst_size  the destination address size
 *
 * @return 0 on a new available entry,
 *         -ENOENT if no suiting entry is provided.
 */
static int fib_signal_rp(uint8_t *dst, size_t dst_size, uint32_t dst_flags)
{
    msg_t msg, reply;
    rp_address_msg_t content;
    content.address = dst;
    content.address_size = dst_size;
    content.address_flags = dst_flags;
    int ret = -ENOENT;

    msg.type = FIB_MSG_RP_SIGNAL;
    msg.content.ptr = (void *)&content;

    for (size_t i = 0; i < FIB_MAX_REGISTERED_RP; ++i) {
        if (notify_rp[i] != KERNEL_PID_UNDEF) {
            DEBUG("[fib_signal_rp] send msg@: %p to pid[%d]: %d\n", \
                  msg.content.ptr, (int)i, (int)notify_rp[i]);

            /* do only signal a RP if its registered prefix matches */
            size_t dst_size_in_bits = dst_size<<3;
            if (universal_address_compare(prefix_rp[i], dst, &dst_size_in_bits) == 1) {
                /* the receiver, i.e. the RP, MUST copy the content value.
                 * using the provided pointer after replying this message
                 * will lead to errors
                 */
                msg_send_receive(&msg, &reply, notify_rp[i]);
                DEBUG("[fib_signal_rp] got reply.\n");
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
    mutex_lock(&mtx_access);
    DEBUG("[fib_add_entry]\n");
    size_t count = 1;
    fib_entry_t *entry[count];

    /* check if dst and next_hop are valid pointers */
    if ((dst == NULL) || (next_hop == NULL)) {
        mutex_unlock(&mtx_access);
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

    mutex_unlock(&mtx_access);
    return ret;
}

int fib_update_entry(fib_table_t *table, uint8_t *dst, size_t dst_size,
                     uint8_t *next_hop, size_t next_hop_size,
                     uint32_t next_hop_flags, uint32_t lifetime)
{
    mutex_lock(&mtx_access);
    DEBUG("[fib_update_entry]\n");
    size_t count = 1;
    fib_entry_t *entry[count];
    int ret = -ENOMEM;

    /* check if dst and next_hop are valid pointers */
    if ((dst == NULL) || (next_hop == NULL)) {
        mutex_unlock(&mtx_access);
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

    mutex_unlock(&mtx_access);
    return ret;
}

void fib_remove_entry(fib_table_t *table, uint8_t *dst, size_t dst_size)
{
    mutex_lock(&mtx_access);
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

    mutex_unlock(&mtx_access);
}

int fib_get_next_hop(fib_table_t *table, kernel_pid_t *iface_id,
                     uint8_t *next_hop, size_t *next_hop_size,
                     uint32_t *next_hop_flags, uint8_t *dst, size_t dst_size,
                     uint32_t dst_flags)
{
    mutex_lock(&mtx_access);
    DEBUG("[fib_get_next_hop]\n");
    size_t count = 1;
    fib_entry_t *entry[count];

    if ((iface_id == NULL)
        || (next_hop_size == NULL)
        || (next_hop_flags == NULL)) {
            mutex_unlock(&mtx_access);
            return -EINVAL;
        }

    if ((dst == NULL) || (next_hop == NULL)) {
        mutex_unlock(&mtx_access);
        return -EFAULT;
    }

    int ret = fib_find_entry(table, dst, dst_size, &(entry[0]), &count);
    if (!(ret == 0 || ret == 1)) {
        /* notify all responsible RPs for unknown  next-hop for the destination address */
        if (fib_signal_rp(dst, dst_size, dst_flags) == 0) {
            count = 1;
            /* now lets see if the RRPs have found a valid next-hop */
            ret = fib_find_entry(table, dst, dst_size, &(entry[0]), &count);
        }
    }

    if (ret == 0 || ret == 1) {

        uint8_t *address_ret = universal_address_get_address(entry[0]->next_hop,
                               next_hop, next_hop_size);

        if (address_ret == NULL) {
            mutex_unlock(&mtx_access);
            return -ENOBUFS;
        }
    }
    else {
        mutex_unlock(&mtx_access);
        return -EHOSTUNREACH;
    }

    *iface_id = entry[0]->iface_id;
    *next_hop_flags = entry[0]->next_hop_flags;
    mutex_unlock(&mtx_access);
    return 0;
}

int fib_get_destination_set(fib_table_t *table, uint8_t *prefix,
                            size_t prefix_size,
                            fib_destination_set_entry_t *dst_set,
                            size_t* dst_set_size)
{
    mutex_lock(&mtx_access);
    int ret = -EHOSTUNREACH;
    size_t found_entries = 0;

    for (size_t i = 0; i < table->size; ++i) {
        if ((table->entries[i].global != NULL) &&
            (universal_address_compare_prefix(table->entries[i].global, prefix, prefix_size<<3) >= 0)) {
            if( (dst_set != NULL) && (found_entries < *dst_set_size) ) {
            /* set the size to full byte usage */
            dst_set[found_entries].dest_size = sizeof(dst_set[found_entries].dest);
            universal_address_get_address(table->entries[i].global,
                                          dst_set[found_entries].dest,
                                          &dst_set[found_entries].dest_size);
            }
            found_entries++;
        }
    }

    if( found_entries > *dst_set_size ) {
        ret = -ENOBUFS;
    } else if( found_entries > 0 ) {
        ret = 0;
    }

    *dst_set_size = found_entries;

    mutex_unlock(&mtx_access);

    return ret;
}

void fib_init(fib_table_t *table)
{
    DEBUG("[fib_init] hello. Initializing some stuff.\n");
    mutex_lock(&mtx_access);

    for (size_t i = 0; i < FIB_MAX_REGISTERED_RP; ++i) {
        notify_rp[i] = KERNEL_PID_UNDEF;
        prefix_rp[i] = NULL;
    }

    memset(table->entries, 0, (table->size * sizeof(fib_entry_t)));

    universal_address_init();
    mutex_unlock(&mtx_access);
}

void fib_deinit(fib_table_t *table)
{
    DEBUG("[fib_deinit] hello. De-Initializing stuff.\n");
    mutex_lock(&mtx_access);

    for (size_t i = 0; i < FIB_MAX_REGISTERED_RP; ++i) {
        notify_rp[i] = KERNEL_PID_UNDEF;
        prefix_rp[i] = NULL;
    }

    notify_rp_pos = 0;

    memset(table->entries, 0, (table->size * sizeof(fib_entry_t)));

    universal_address_reset();
    mutex_unlock(&mtx_access);
}

int fib_register_rp(uint8_t *prefix, size_t prefix_addr_type_size)
{
    mutex_lock(&mtx_access);

    if (notify_rp_pos >= FIB_MAX_REGISTERED_RP) {
        mutex_unlock(&mtx_access);
        return -ENOMEM;
    }

    if ((prefix == NULL) || (prefix_addr_type_size == 0)) {
        mutex_unlock(&mtx_access);
        return -EINVAL;
    }

    if (notify_rp_pos < FIB_MAX_REGISTERED_RP) {
        notify_rp[notify_rp_pos] = sched_active_pid;
        universal_address_container_t *container = universal_address_add(prefix, prefix_addr_type_size);
        prefix_rp[notify_rp_pos] = container;
        notify_rp_pos++;
    }

    mutex_unlock(&mtx_access);
    return 0;
}

int fib_get_num_used_entries(fib_table_t *table)
{
    mutex_lock(&mtx_access);
    size_t used_entries = 0;

    for (size_t i = 0; i < table->size; ++i) {
        used_entries += (size_t)(table->entries[i].global != NULL);
    }

    mutex_unlock(&mtx_access);
    return used_entries;
}

/* print functions */

void fib_print_notify_rp(void)
{
    mutex_lock(&mtx_access);

    for (size_t i = 0; i < FIB_MAX_REGISTERED_RP; ++i) {
        printf("[fib_print_notify_rp] pid[%d]: %d\n", (int)i, (int)notify_rp[i]);
    }

    mutex_unlock(&mtx_access);
}

void fib_print_fib_table(fib_table_t *table)
{
    mutex_lock(&mtx_access);

    for (size_t i = 0; i < table->size; ++i) {
        printf("[fib_print_table] %d) iface_id: %d, global: %p, next hop: %p, lifetime: %"PRIu32"\n",
               (int)i, (int)table->entries[i].iface_id,
               (void *)table->entries[i].global,
               (void *)table->entries[i].next_hop,
               (uint32_t)(table->entries[i].lifetime / 1000));
    }

    mutex_unlock(&mtx_access);
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
    mutex_lock(&mtx_access);
    printf("%-" FIB_ADDR_PRINT_LENS "s %-6s %-" FIB_ADDR_PRINT_LENS "s %-6s %-16s Interface\n"
           , "Destination", "Flags", "Next Hop", "Flags", "Expires");

    uint64_t now = xtimer_now64();

    for (size_t i = 0; i < table->size; ++i) {
        if (table->entries[i].lifetime != 0) {
            fib_print_address(table->entries[i].global);
            printf(" 0x%04"PRIx32" ", table->entries[i].global_flags);
            fib_print_address(table->entries[i].next_hop);
            printf(" 0x%04"PRIx32" ", table->entries[i].next_hop_flags);

            if (table->entries[i].lifetime != FIB_LIFETIME_NO_EXPIRE) {

                uint64_t tm = table->entries[i].lifetime - now;

                /* we must interpret the values as signed */
                if ((int64_t)tm < 0 ) {
                    printf("%-16s ", "EXPIRED");
                }
                else {
                    printf("%"PRIu32".%05"PRIu32, (uint32_t)(tm / 1000000), (uint32_t)(tm % 1000000));
                }
            }
            else {
                printf("%-16s ", "NEVER");
            }

            printf("%d\n", (int)table->entries[i].iface_id);
        }
    }

    mutex_unlock(&mtx_access);
}

#if FIB_DEVEL_HELPER
int fib_devel_get_lifetime(fib_table_t *table, uint64_t *lifetime, uint8_t *dst,
                           size_t dst_size)
{
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
#endif
