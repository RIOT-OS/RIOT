/**
 * universal address container implementation
 *
 * Copyright (C) 2014 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sys_universal_address
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
#ifdef MODULE_FIB
#include "net/fib.h"
#ifdef MODULE_GNRC_IPV6
#include "net/gnrc/ipv6.h"
#endif
#endif
#include "mutex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "universal_address.h"

/**
 * @brief Maximum number of entries handled
 */
/* determine the maximum number of entries */
#ifndef UNIVERSAL_ADDRESS_MAX_ENTRIES
/* all potential users of universal addresses have to add their requirements here */
#   if defined(MODULE_FIB) && defined(MODULE_GNRC_IPV6)
#       define UA_ADD0 (2 * GNRC_IPV6_FIB_TABLE_SIZE)
#   else
#       define UA_ADD0  (0)
#   endif

#   define UNIVERSAL_ADDRESS_MAX_ENTRIES    (UA_ADD0)
#endif

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
    /* cppcheck-suppress unsignedLessThanZero
     * (reason: UNIVERSAL_ADDRESS_MAX_ENTRIES may be zero in which case this
     * code is optimized out) */
    for (size_t i = 0; i < UNIVERSAL_ADDRESS_MAX_ENTRIES; ++i) {
        if (universal_address_table[i].address_size == addr_size) {
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
    /* cppcheck-suppress unsignedLessThanZero
     * (reason: UNIVERSAL_ADDRESS_MAX_ENTRIES may be zero in which case this
     * code is optimized out) */
    if (universal_address_table_filled < UNIVERSAL_ADDRESS_MAX_ENTRIES) {
        /* cppcheck-suppress unsignedLessThanZero
         * (reason: UNIVERSAL_ADDRESS_MAX_ENTRIES may be zero, see above) */
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

uint8_t* universal_address_get_address(universal_address_container_t *entry,
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
                              uint8_t *addr, size_t *addr_size_in_bits)
{
    mutex_lock(&mtx_access);

    int ret = -ENOENT;

    /* If we have distinct sizes, the addresses are probably not comperable */
    if ((size_t)(entry->address_size<<3) != *addr_size_in_bits) {
        mutex_unlock(&mtx_access);
        return ret;
    }

    /* compare up to fist distinct byte, pretty clumsy method for now */
    int idx = -1;
    bool test_all_zeros = true;
    for (size_t i = 0; i < entry->address_size; i++) {
        if ((idx == -1) && (entry->address[i] != addr[i])) {
            idx = i;
        }
        if (test_all_zeros) {
            test_all_zeros = (entry->address[i] == 0);
        }
        if ((idx != -1) && !test_all_zeros) {
            break;
        }
    }

    /* if the address is all 0 its a default route address */
    if (test_all_zeros) {
        *addr_size_in_bits = 0;
        mutex_unlock(&mtx_access);
        return UNIVERSAL_ADDRESS_IS_ALL_ZERO_ADDRESS;
    }

    /* if we have no distinct bytes the addresses are equal */
    if (idx == -1) {
        mutex_unlock(&mtx_access);
        return UNIVERSAL_ADDRESS_EQUAL;
    }

    /* count equal bits */
    uint8_t xor = entry->address[idx]^addr[idx];
    int8_t j = 7;
    for ( ; j > 0; --j) {
        if ((xor >> j) & 0x01) {
            break;
        }
    }

    /* get the total number of matching bits */
    *addr_size_in_bits = (idx << 3) + j;
    ret = UNIVERSAL_ADDRESS_MATCHING_PREFIX;

    mutex_unlock(&mtx_access);
    return ret;
}

int universal_address_compare_prefix(universal_address_container_t *entry,
                              uint8_t *prefix, size_t prefix_size_in_bits)
{
    mutex_lock(&mtx_access);
    int ret = -ENOENT;
    /* If we have distinct sizes, the prefix is not comperable */
    if ((size_t)(entry->address_size<<3) != prefix_size_in_bits) {
        mutex_unlock(&mtx_access);
        return ret;
    }

    /* Get the index of the first trailing `0` */
    int i = 0;
    for (i = entry->address_size-1; i >= 0; --i) {
        if (prefix[i] != 0) {
            break;
        }
    }

    if (memcmp(entry->address, prefix, i) == 0) {
        /* if the bytes-1 equals we check the bits of the lowest byte */
        uint8_t bitmask = 0x00;
        /* get a bitmask for the trailing 0b */
        for (uint8_t j = 0; j < 8; ++j) {
            if ((prefix[i] >> j) & 0x01) {
                bitmask = 0xff << j;
                break;
            }
        }

        if ((entry->address[i] & bitmask) == (prefix[i] & bitmask)) {
            ret = entry->address[i] != prefix[i];
            if (ret == UNIVERSAL_ADDRESS_EQUAL) {
                /* check if the remaining bits from entry are significant */
                i++;
                for ( ; i < entry->address_size; ++i) {
                    if (entry->address[i] != 0) {
                        ret = UNIVERSAL_ADDRESS_MATCHING_PREFIX;
                        break;
                    }
                }
            }
        }
    }

    mutex_unlock(&mtx_access);
    return ret;
}

void universal_address_init(void)
{
    mutex_lock(&mtx_access);

    /* cppcheck-suppress unsignedLessThanZero
     * (reason: UNIVERSAL_ADDRESS_MAX_ENTRIES may be zero in which case this
     * code is optimized out) */
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

    /* cppcheck-suppress unsignedLessThanZero
     * (reason: UNIVERSAL_ADDRESS_MAX_ENTRIES may be zero in which case this
     * code is optimized out) */
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

    /* cppcheck-suppress unsignedLessThanZero
     * (reason: UNIVERSAL_ADDRESS_MAX_ENTRIES may be zero in which case this
     * code is optimized out) */
    for (size_t i = 0; i < UNIVERSAL_ADDRESS_MAX_ENTRIES; ++i) {
        universal_address_print_entry(&universal_address_table[i]);
    }
}
