/**
 * FIB implementation
 *
 * Copyright (C) 2014 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup fib
 * @{
 * @file
 * @brief   Functions to manage FIB entries
 * @author  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "thread.h"
#include "msg.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "ng_fib.h"
#include "ng_fib/ng_fib_table.h"

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
 * @brief array of the FIB tables
 */
static fib_entry_t fib_table[FIB_MAX_FIB_TABLE_ENTRIES];

void fib_ms_to_timex(uint32_t ms, timex_t *timex)
{
    vtimer_now(timex);
    timex->seconds += ms / 1000;
    timex->microseconds += (ms - timex->seconds * 1000) * 1000;
}

int fib_find_entry(uint8_t *dst, size_t dst_size,
                   fib_entry_t **entry_arr, size_t *entry_arr_size)
{
    timex_t now;
    vtimer_now(&now);

    size_t count = 0;
    size_t prefix_size = 0;
    size_t match_size = dst_size;
    int ret = -EHOSTUNREACH;

    for (size_t i = 0; i < FIB_MAX_FIB_TABLE_ENTRIES; ++i) {

        /* autoinvalidate if the entry lifetime is not set to not expire */
        if ((fib_table[i].lifetime.seconds != FIB_LIFETIME_NO_EXPIRE)
            || (fib_table[i].lifetime.microseconds != FIB_LIFETIME_NO_EXPIRE)) {

            /* check if the lifetime expired */
            if (timex_cmp(now, fib_table[i].lifetime) > -1) {
                /* remove this entry if its lifetime expired */
                fib_table[i].lifetime.seconds = 0;
                fib_table[i].lifetime.microseconds = 0;
                fib_table[i].global_flags = 0;
                fib_table[i].next_hop_flags = 0;
                fib_table[i].iface_id = KERNEL_PID_UNDEF;

                if (fib_table[i].global != NULL) {
                    universal_address_rem(fib_table[i].global);
                    fib_table[i].global = NULL;
                }

                if (fib_table[i].next_hop != NULL) {
                    universal_address_rem(fib_table[i].next_hop);
                    fib_table[i].next_hop = NULL;
                }
            }
        }

        if ((prefix_size < dst_size) &&
            (fib_table[i].global != NULL) &&
            (universal_address_compare(fib_table[i].global, dst, &match_size) == 0)) {

            /* If we found an exact match */
            if (match_size == dst_size) {
                entry_arr[0] = &(fib_table[i]);
                *entry_arr_size = 1;
                /* we will not find a better one so we return */
                return 1;
            }
            else {
                /* we try to find the most fitting prefix */
                if (match_size > prefix_size) {
                    entry_arr[0] = &(fib_table[i]);
                    /* we could find a better one so we move on */
                    ret = 0;
                }
            }

            prefix_size = match_size;
            match_size = dst_size;
            count = 1;
        }
    }

    *entry_arr_size = count;
    return ret;
}

int fib_upd_entry(fib_entry_t *entry,
                  uint8_t *next_hop, size_t next_hop_size, uint32_t next_hop_flags,
                  uint32_t lifetime)
{
    universal_address_container_t *container = universal_address_add(next_hop, next_hop_size);

    if (container == NULL) {
        return -ENOMEM;
    }

    universal_address_rem(entry->next_hop);
    entry->next_hop = container;
    entry->next_hop_flags = next_hop_flags;

    if (lifetime < FIB_LIFETIME_NO_EXPIRE) {
        fib_ms_to_timex(lifetime, &entry->lifetime);
    }
    else {
        entry->lifetime.seconds = FIB_LIFETIME_NO_EXPIRE;
        entry->lifetime.microseconds = FIB_LIFETIME_NO_EXPIRE;
    }

    return 0;
}

int fib_create_entry(kernel_pid_t iface_id,
                     uint8_t *dst, size_t dst_size, uint32_t dst_flags,
                     uint8_t *next_hop, size_t next_hop_size, uint32_t next_hop_flags,
                     uint32_t lifetime)
{
    for (size_t i = 0; i < FIB_MAX_FIB_TABLE_ENTRIES; ++i) {
        if (fib_table[i].lifetime.seconds == 0 && fib_table[i].lifetime.microseconds == 0) {

            fib_table[i].global = universal_address_add(dst, dst_size);

            if (fib_table[i].global != NULL) {
                fib_table[i].global_flags = dst_flags;
                fib_table[i].next_hop = universal_address_add(next_hop, next_hop_size);
                fib_table[i].next_hop_flags = next_hop_flags;
            }

            if (fib_table[i].next_hop != NULL) {
                /* everything worked fine */
                fib_table[i].iface_id = iface_id;

                if (lifetime < FIB_LIFETIME_NO_EXPIRE) {
                    fib_ms_to_timex(lifetime, &fib_table[i].lifetime);
                }
                else {
                    fib_table[i].lifetime.seconds = FIB_LIFETIME_NO_EXPIRE;
                    fib_table[i].lifetime.microseconds = FIB_LIFETIME_NO_EXPIRE;
                }

                return 0;
            }
        }
    }

    return -ENOMEM;
}

int fib_remove(fib_entry_t *entry)
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
    entry->lifetime.seconds = 0;
    entry->lifetime.microseconds = 0;

    return 0;
}

int fib_signal_rp(uint8_t *dst, size_t dst_size, uint32_t dst_flags)
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
            if (universal_address_compare(prefix_rp[i], dst, &dst_size) == 0) {
                /* the receiver, i.e. the RP, MUST copy the content value.
                 * using the provided pointer after replying this message
                 * will lead to errors
                 */
                msg_send_receive(&msg, &reply, notify_rp[i]);
                DEBUG("[fib_signal_rp] got reply.");
            }
        }
    }

    return ret;
}

void fib_init(void)
{
    DEBUG("[fib_init] hello. Initializing some stuff.");
    mutex_lock(&mtx_access);

    for (size_t i = 0; i < FIB_MAX_REGISTERED_RP; ++i) {
        notify_rp[i] = KERNEL_PID_UNDEF;
        prefix_rp[i] = NULL;
    }

    for (size_t i = 0; i < FIB_MAX_FIB_TABLE_ENTRIES; ++i) {
        fib_table[i].iface_id = 0;
        fib_table[i].lifetime.seconds = 0;
        fib_table[i].lifetime.microseconds = 0;
        fib_table[i].global_flags = 0;
        fib_table[i].global = NULL;
        fib_table[i].next_hop_flags = 0;
        fib_table[i].next_hop = NULL;
    }

    universal_address_init();
    mutex_unlock(&mtx_access);
}

void fib_deinit(void)
{
    DEBUG("[fib_deinit] hello. De-Initializing stuff.");
    mutex_lock(&mtx_access);

    for (size_t i = 0; i < FIB_MAX_REGISTERED_RP; ++i) {
        notify_rp[i] = KERNEL_PID_UNDEF;
        prefix_rp[i] = NULL;
    }

    notify_rp_pos = 0;

    for (size_t i = 0; i < FIB_MAX_FIB_TABLE_ENTRIES; ++i) {
        fib_table[i].iface_id = 0;
        fib_table[i].lifetime.seconds = 0;
        fib_table[i].lifetime.microseconds = 0;
        fib_table[i].global_flags = 0;
        fib_table[i].global = NULL;
        fib_table[i].next_hop_flags = 0;
        fib_table[i].next_hop = NULL;
    }

    universal_address_reset();
    mutex_unlock(&mtx_access);
}

int fib_register_rp(uint8_t *prefix, size_t prefix_size)
{
    mutex_lock(&mtx_access);

    if (notify_rp_pos >= FIB_MAX_REGISTERED_RP) {
        mutex_unlock(&mtx_access);
        return -ENOMEM;
    }

    if ((prefix == NULL) || (prefix_size == 0)) {
        mutex_unlock(&mtx_access);
        return -EINVAL;
    }

    if (notify_rp_pos < FIB_MAX_REGISTERED_RP) {
        notify_rp[notify_rp_pos] = sched_active_pid;
        universal_address_container_t *container = universal_address_add(prefix, prefix_size);
        prefix_rp[notify_rp_pos] = container;
        notify_rp_pos++;
    }

    mutex_unlock(&mtx_access);
    return 0;
}

int fib_get_num_used_entries(void)
{
    mutex_lock(&mtx_access);
    size_t used_entries = 0;

    for (size_t i = 0; i < FIB_MAX_FIB_TABLE_ENTRIES; ++i) {
        used_entries += (size_t)(fib_table[i].global != NULL);
    }

    mutex_unlock(&mtx_access);
    return used_entries;
}

mutex_t *fib_get_mutex(void)
{
    return &mtx_access;
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

void fib_print_fib_table(void)
{
    mutex_lock(&mtx_access);

    for (size_t i = 0; i < FIB_MAX_FIB_TABLE_ENTRIES; ++i) {
        printf("[fib_print_fib_table] %d) iface_id: %d, global: %p, next hop: %p, lifetime: %d.%d\n", \
               (int)i, \
               (int)fib_table[i].iface_id, \
               (void *)fib_table[i].global, \
               (void *)fib_table[i].next_hop, \
               (int)fib_table[i].lifetime.seconds, \
               (int)fib_table[i].lifetime.microseconds);
    }

    mutex_unlock(&mtx_access);
}

static void fib_print_adress(universal_address_container_t *entry)
{
    uint8_t address[UNIVERSAL_ADDRESS_SIZE];
    size_t addr_size = UNIVERSAL_ADDRESS_SIZE;
    uint8_t *ret = universal_address_get_address(entry, address, &addr_size);

    if (ret == address) {
        for (size_t i = 0; i < UNIVERSAL_ADDRESS_SIZE; ++i) {
            if (i <= addr_size) {
                printf("%02x", address[i]);
            }
            else {
                printf("  ");
            }
        }
    }
}

void fib_print_routes(void)
{
    mutex_lock(&mtx_access);
    printf("%-32s %-6s %-32s %-6s %-16sInterface\n"
           , "Destination", "Flags", "Next Hop", "Flags", "Expires");

    timex_t now;
    vtimer_now(&now);

    for (size_t i = 0; i < FIB_MAX_FIB_TABLE_ENTRIES; ++i) {
        if (fib_table[i].lifetime.seconds != 0 || fib_table[i].lifetime.microseconds != 0) {
            fib_print_adress(fib_table[i].global);
            printf(" 0x%04"PRIx32" ", fib_table[i].global_flags);
            fib_print_adress(fib_table[i].next_hop);
            printf(" 0x%04"PRIx32" ", fib_table[i].next_hop_flags);

            if ((fib_table[i].lifetime.seconds != FIB_LIFETIME_NO_EXPIRE)
                || (fib_table[i].lifetime.microseconds != FIB_LIFETIME_NO_EXPIRE)) {

                timex_t tm = timex_sub(fib_table[i].lifetime, now);

                /* we must interpret the values as signed */
                if ((int32_t)tm.seconds < 0
                    || (tm.seconds == 0 && (int32_t)tm.microseconds < 0)) {
                    printf("%-16s ", "EXPIRED");
                }
                else {
                    printf("%"PRIu32".%05"PRIu32"\t", tm.seconds, tm.microseconds);
                }
            }
            else {
                printf("%-16s ", "NEVER");
            }

            printf("%d\n", (int)fib_table[i].iface_id);
        }
    }

    mutex_unlock(&mtx_access);
}
