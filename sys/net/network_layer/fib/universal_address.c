/**
 * universal address container implementation
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
 * @brief   Functions to manage universal address container
 * @author  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "mutex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "ng_fib/ng_universal_address.h"

/**
 * @brief counter indicating the number of entries allocated
 */
static size_t universal_address_table_filled = 0;

/**
 * @brief The array of universal_address containers
 */
static universal_address_container_t universal_address_table[UNIVERSAL_ADDRESS_MAX_ENTRIES];

/**
 * @brief access mutex to control exclusive operations on calls
 */
static mutex_t mtx_access = MUTEX_INIT;

/**
 * @brief finds the universal address container for the given address
 *
 * @param[in] addr       pointer to the address
 * @param[in] addr_size  the number of bytes required for the address entry
 *
 * @return pointer to the universal_address_container_t containing the address on success
 *         NULL if the address could not be inserted
 */
static universal_address_container_t *universal_address_find_entry(uint8_t *addr, size_t addr_size)
{
    for (size_t i = 0; i < UNIVERSAL_ADDRESS_MAX_ENTRIES; ++i) {
        if ((universal_address_table[i].address_size == addr_size) && (universal_address_table[i].address != NULL)) {
            if (memcmp((universal_address_table[i].address), addr, addr_size) == 0) {
                return &(universal_address_table[i]);
            }
        }
    }

    return NULL;
}

/**
 * @brief finds the next empty or unused universal address containers
 *
 * @return pointer to the next free/unused universal_address_container_t
 *         or NULL if no memory is left in universal_address_table
 */
static universal_address_container_t *universal_address_get_next_unused_entry(void)
{
    if (universal_address_table_filled < UNIVERSAL_ADDRESS_MAX_ENTRIES) {
        for (size_t i = 0; i < UNIVERSAL_ADDRESS_MAX_ENTRIES; ++i) {
            if (universal_address_table[i].use_count == 0) {
                return &(universal_address_table[i]);
            }
        }
    }

    return NULL;
}

universal_address_container_t *universal_address_add(uint8_t *addr, size_t addr_size)
{
    mutex_lock(&mtx_access);
    universal_address_container_t *pEntry = universal_address_find_entry(addr, addr_size);

    if (pEntry == NULL) {
        /* look for a free entry */
        pEntry = universal_address_get_next_unused_entry();

        if (pEntry == NULL) {
            mutex_unlock(&mtx_access);
            /* no free room */
            return NULL;
        }

        /* look if the former memory has distinct size */
        if (pEntry->address_size != addr_size) {
            /* clean the address */
            memset(pEntry->address, 0, UNIVERSAL_ADDRESS_SIZE);

            /* set the used bytes */
            pEntry->address_size = addr_size;
            pEntry->use_count = 0;
        }

        /* copy the address */
        memcpy((pEntry->address), addr, addr_size);
    }

    pEntry->use_count++;

    if (pEntry->use_count == 1) {
        DEBUG("[universal_address_add] universal_address_table_filled: %d\n", \
              (int)universal_address_table_filled);
        universal_address_table_filled++;
    }

    mutex_unlock(&mtx_access);
    return pEntry;
}

void universal_address_rem(universal_address_container_t *entry)
{
    mutex_lock(&mtx_access);
    DEBUG("[universal_address_rem] entry: %p\n", (void *)entry);

    /* we do not delete anything on remove */
    if (entry != NULL) {
        if (entry->use_count != 0) {
            entry->use_count--;

            if (entry->use_count == 0) {
                universal_address_table_filled--;
            }
        }
        else {
            DEBUG("[universal_address_rem] universal_address_table_filled: %d\n", \
                  (int)universal_address_table_filled);
        }
    }

    mutex_unlock(&mtx_access);
}

uint8_t *universal_address_get_address(universal_address_container_t *entry,
                                       uint8_t *addr, size_t *addr_size)
{
    mutex_lock(&mtx_access);

    if (*addr_size >= entry->address_size) {
        memcpy(addr, entry->address, entry->address_size);
        *addr_size = entry->address_size;
        mutex_unlock(&mtx_access);
        return addr;
    }

    *addr_size = entry->address_size;
    mutex_unlock(&mtx_access);
    return NULL;
}

int universal_address_compare(universal_address_container_t *entry,
                              uint8_t *addr, size_t *addr_size)
{
    mutex_lock(&mtx_access);

    int ret = -ENOENT;

    /* If we have distinct sizes, the addresses are probably not comperable */
    if (entry->address_size != *addr_size) {
        mutex_unlock(&mtx_access);
        return ret;
    }

    /* Get the index of the first trailing `0` (indicates a network prefix) */
    int i = 0;

    for (i = entry->address_size - 1; i >= 0; --i) {
        if (entry->address[i] != 0) {
            break;
        }
    }

    if (memcmp(entry->address, addr, i + 1) == 0) {
        ret = 0;
        *addr_size = i + 1;
    }

    mutex_unlock(&mtx_access);
    return ret;
}

void universal_address_init(void)
{
    mutex_lock(&mtx_access);

    for (size_t i = 0; i < UNIVERSAL_ADDRESS_MAX_ENTRIES; ++i) {
        universal_address_table[i].use_count = 0;
        universal_address_table[i].address_size = 0;
        memset(universal_address_table[i].address, 0, UNIVERSAL_ADDRESS_SIZE);
    }

    mutex_unlock(&mtx_access);
}

void universal_address_reset(void)
{
    mutex_lock(&mtx_access);

    for (size_t i = 0; i < UNIVERSAL_ADDRESS_MAX_ENTRIES; ++i) {
        universal_address_table[i].use_count = 0;
    }

    universal_address_table_filled = 0;
    mutex_unlock(&mtx_access);
}

void universal_address_print_entry(universal_address_container_t *entry)
{
    mutex_lock(&mtx_access);

    if (entry != NULL) {
        printf("[universal_address_print_entry] entry@: %p, use_count: %d, \
address_size: %d, content: ", \
               (void *)entry, (int)entry->use_count, (int)entry->address_size);

        for (size_t i = 0; i < entry->address_size; ++i) {
            /* printf("%02x ", (char)entry->address[i]); */
            printf("%c", (char)entry->address[i]);
        }

        puts("");
    }

    mutex_unlock(&mtx_access);
}

int universal_address_get_num_used_entries(void)
{
    mutex_lock(&mtx_access);
    size_t ret = universal_address_table_filled;
    mutex_unlock(&mtx_access);
    return ret;
}

void universal_address_print_table(void)
{
    printf("[universal_address_print_table] universal_address_table_filled: %d\n", \
           (int)universal_address_table_filled);

    for (size_t i = 0; i < UNIVERSAL_ADDRESS_MAX_ENTRIES; ++i) {
        universal_address_print_entry(&universal_address_table[i]);
    }
}
