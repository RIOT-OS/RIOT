/*
 * Copyright (C) 2014  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_universal_address Universal Address Container
 * @ingroup     sys
 * @brief       universal address container
 *
 * @{
 *
 * @file
 * @brief       Types and functions for operating universal addresses
 * @author      Martin Landsmann
 */

#ifndef UNIVERSAL_ADDRESS_H
#define UNIVERSAL_ADDRESS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include "net/ipv6/addr.h"

/** @brief  size of the used addresses in bytes */
/* determine the widest possible address type */
#ifndef UNIVERSAL_ADDRESS_SIZE
#define UNIVERSAL_ADDRESS_SIZE (0)       /* rather senseless default, should
                                            trigger warnings */
#endif

/* IPv6 address has 128 bit -> 16 bytes */
#if defined(MODULE_IPV6_ADDR) && ((IPV6_ADDR_BIT_LEN >> 3) > UNIVERSAL_ADDRESS_SIZE)
#undef UNIVERSAL_ADDRESS_SIZE
#define UNIVERSAL_ADDRESS_SIZE (IPV6_ADDR_BIT_LEN >> 3)
#endif

/** @brief return value indicating the compared addresses are equal */
#define UNIVERSAL_ADDRESS_EQUAL (0)

/** @brief return value indicating the compared addresses match up to a certain prefix */
#define UNIVERSAL_ADDRESS_MATCHING_PREFIX (1)

/** @brief return value indicating all address bits of the entry are `0`.
 *         Its considered as default route address that matches any other prefix.
 */
#define UNIVERSAL_ADDRESS_IS_ALL_ZERO_ADDRESS (2)

/**
 * @brief The container descriptor used to identify a universal address entry
 */
typedef struct {
    uint8_t use_count;                       /**< The number of entries link here */
    uint8_t address_size;                    /**< Size in bytes of the used generic address */
    uint8_t address[UNIVERSAL_ADDRESS_SIZE]; /**< The generic address data */
} universal_address_container_t;

/**
 * @brief Initialize the data structure for the entries
 */
void universal_address_init(void);

/**
 * @brief Resets the universal_address_container_t::use_count for all entries
 */
void universal_address_reset(void);

/**
 * @brief Add a given address to the universal address entries. If the entry already exists,
 *        the universal_address_container_t::use_count will be increased.
 *
 * @param[in] addr       pointer to the address
 * @param[in] addr_size  the number of bytes required for the address entry
 *
 * @return pointer to the universal_address_container_t containing the address on success
 * @return NULL if the address could not be inserted
 */
universal_address_container_t *universal_address_add(uint8_t *addr, size_t addr_size);

/**
 * @brief Add a given container from the universal address entries. If the entry exists,
 *        the universal_address_container_t::use_count will be decreased.
 *
 * @param[in] entry  pointer to the universal_address_container_t to be removed
 */
void universal_address_rem(universal_address_container_t *entry);

/**
 * @brief Copy the address from the given container to the provided pointer
 *
 * @param[in] entry          pointer to the universal_address_container_t
 * @param[out] addr          pointer to store the address entry
 * @param[in, out] addr_size pointer providing the size of available memory on addr
 *                           this value is overwritten with the actual size required
 *
 * @return addr if the address is copied to the addr destination
 * @return NULL if the size is unsufficient for copy
 */
uint8_t* universal_address_get_address(universal_address_container_t *entry,
                                       uint8_t *addr, size_t *addr_size);

/**
 * @brief Determine if the entry equals the provided address
 *        This function requires to be provided with the full size of the used
 *        address type behind @p addr to be comparable with the address stored in @p entry.
 *
 * @param[in] entry       pointer to the universal_address_container_t for compare
 * @param[in] addr        pointer to the address for compare
 * @param[in, out] addr_size_in_bits  the number of bits  used for the address entry
 *                                    on sucessfull return this value is overwritten
 *                                    with the number of matching bits till the
 *                                    first of trailing `0`s
 *
 * @return UNIVERSAL_ADDRESS_EQUAL if the entries are equal
 * @return UNIVERSAL_ADDRESS_MATCHING_PREFIX if the entry matches to a certain prefix
 *                                           (trailing '0's in @p entry)
 * @return UNIVERSAL_ADDRESS_IS_ALL_ZERO_ADDRESS if the entry address is all `0`s
 *                                               and considered as default route
 * @return -ENOENT if the given adresses do not match
 */
int universal_address_compare(universal_address_container_t *entry,
                              uint8_t *addr, size_t *addr_size_in_bits);


/**
* @brief Determine if the entry equals the provided prefix
*        This function requires to be provided with the full size of the used
*        address type behind @p prefix to be comparable with the address stored in @p entry.
*
*
* @param[in] entry       pointer to the universal_address_container_t for compare
* @param[in] prefix      pointer to the address for compare
* @param[in] prefix_size_in_bits the number of bits used for the prefix entry.
*                                This size MUST be the full address size including trailing '0's,
*                                e.g. for an ipv6_addr_t it would be sizeof(ipv6_addr_t)
*                                regardless if the stored prefix is < ::/128
*
* @return UNIVERSAL_ADDRESS_EQUAL if the entries are equal
* @return UNIVERSAL_ADDRESS_MATCHING_PREFIX if the entry matches to a certain prefix
*                                           (trailing '0's in @p prefix)
* @return -ENOENT if the given adresses do not match
*/
int universal_address_compare_prefix(universal_address_container_t *entry,
                            uint8_t *prefix, size_t prefix_size_in_bits);

/**
 * @brief Print the content of the given entry
 *
 * @param[in] entry  pointer to the universal_address_container_t to be printed
 */
void universal_address_print_entry(universal_address_container_t *entry);

/**
 * @brief Return the number of used entries
 */
int universal_address_get_num_used_entries(void);

/**
 * @brief Print the content of the generic address table up to the used element
 */
void universal_address_print_table(void);

#ifdef __cplusplus
}
#endif

#endif /* UNIVERSAL_ADDRESS_H */
/** @} */
