/*
 * Copyright (C) 2014  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_universal_address Universal Address Container
 * @ingroup     net
 * @brief       universal address container
 *
 * @{
 *
 * @file
 * @brief       Types and functions for operating universal addresses
 * @author      Martin Landsmann
 */

#ifndef UNIVERSAL_ADDRESS_H_
#define UNIVERSAL_ADDRESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define UNIVERSAL_ADDRESS_SIZE (16)         /**< size of the used addresses in bytes        */

/**
 * @brief The container descriptor used to identify a universal address entry
 */
typedef struct universal_address_container_t {
    uint8_t use_count;                       /**< The number of entries link here */
    uint8_t address_size;                    /**< Size in bytes of the used genereic address */
    uint8_t address[UNIVERSAL_ADDRESS_SIZE]; /**< the genereic address data */
} universal_address_container_t;

/**
 * @brief initializes the datastructure for the entries
 */
void universal_address_init(void);

/**
 * @brief resets the usecoumt for all entries
 */
void universal_address_reset(void);

/**
 * @brief Adds a given address to the universal address entries
 *        if the entry already exists, the use_count will be increased.
 *
 * @param[in] addr       pointer to the address
 * @param[in] addr_size  the number of bytes required for the address entry
 *
 * @return pointer to the universal_address_container_t containing the address on success
 *         NULL if the address could not be inserted
 */
universal_address_container_t *universal_address_add(uint8_t *addr, size_t addr_size);

/**
 * @brief Adds a given container from the universal address entries
 *        if the entry exists, the use_count will be decreased.
 *
 * @param[in] entry  pointer to the universal_address_container_t to be removed
 */
void universal_address_rem(universal_address_container_t *entry);

/**
 * @brief copies the address from the given container to the provided pointer
 *
 * @param[in] entry          pointer to the universal_address_container_t
 * @param[out] addr          pointer to store the address entry
 * @param[in, out] addr_size pointer providing the size of available memory on addr
 *                           this value is overwritten with the actual size required
 *
 * @return addr if the address is copied to the addr destination
 *         NULL if the size is unsufficient for copy
 */
uint8_t* universal_address_get_address(universal_address_container_t *entry,
                                       uint8_t *addr, size_t *addr_size);

/**
 * @brief determines if the entry equals the provided address
 *
 * @param[in] entry       pointer to the universal_address_container_t for compare
 * @param[in] addr        pointer to the address for compare
 * @param[in, out] addr_size   the number of bytes used for the address entry
 *                             on sucessfull return this value is overwritten
 *                             with the number of matching bytes till the
 *                             first of trailing `0`s indicating a prefix
 *
 * @return 0 if the entries are equal or comperable
 *         -ENOENT if the given adresses do not match
 */
int universal_address_compare(universal_address_container_t *entry,
                              uint8_t *addr, size_t *addr_size);

/**
 * @brief Prints the content of the given entry
 *
 * @param[in] entry  pointer to the universal_address_container_t to be printed
 */
void universal_address_print_entry(universal_address_container_t *entry);

/**
 * @brief returns the number of used entries
 */
int universal_address_get_num_used_entries(void);

/**
 * @brief Prints the content of the genereic address table up to the used element
 */
void universal_address_print_table(void);

#ifdef __cplusplus
}
#endif

#endif /* UNIVERSAL_ADDRESS_H_ */
/** @} */
