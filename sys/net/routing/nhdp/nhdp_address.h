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
 * @brief       Centralized address storage interface for NHDP
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef NHDP_ADDRESS_H_
#define NHDP_ADDRESS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   NHDP address representation
 */
typedef struct nhdp_addr_t {
    uint8_t *addr;                      /**< Pointer to the address data */
    size_t addr_size;                   /**< Size in bytes of the address */
    uint8_t addr_type;                  /**< AF type for the address */
    uint8_t usg_count;                  /**< Usage count in information bases */
    uint8_t in_tmp_table;               /**< Signals usage in a writers temp table */
    struct nhdp_addr_t *next;           /**< Pointer to next address (used in central storage) */
} nhdp_addr_t;

/**
 * @brief   Container for NHDP address storage in a list
 */
typedef struct nhdp_addr_entry_t {
    struct nhdp_addr_t *address;        /**< Pointer to NHDP address storage entry */
    struct nhdp_addr_entry_t *next;     /**< Pointer to the next address list element */
} nhdp_addr_entry_t;

/**
 * @name    NHDP address temp usage helper macros
 *
 * @{
 */
#define NHDP_ADDR_TMP_NONE          (0x00)
#define NHDP_ADDR_TMP_ANY           (0x01)
#define NHDP_ADDR_TMP_SYM           (0x03)

#define NHDP_ADDR_TMP_IN_SYM(addr)  ((addr->in_tmp_table & 0x02) >> 1)
#define NHDP_ADDR_TMP_IN_ANY(addr)  ((addr->in_tmp_table & 0x01))
/** @} */

/**
 * @brief                   Get or create a NHDP address for the given address
 *
 * @param[in] addr          Pointer to the given address
 * @param[in] addr_size     Length in bytes of the given address
 * @param[in] addr_type     AF type of the given address
 *
 * @return                  Pointer to the NHDP address representation of the given address
 * @return                  NULL on error
 */
nhdp_addr_t *nhdp_addr_db_get_address(uint8_t *addr, size_t addr_size, uint8_t addr_type);

/**
 * @brief                   Decrement the usage counter of a given NHDP address
 *
 * The NHDP address is deleted if the usage counter reaches zero.
 *
 * @param[in] addr          Pointer to the NHDP address
 */
void nhdp_decrement_addr_usage(nhdp_addr_t *addr);

/**
 * @brief                   Free the given address list
 *
 * This function frees every address list entry of the given address list.
 *
 * @param[in] list_head     Pointer to the head of the address list to free
 */
void nhdp_free_addr_list(nhdp_addr_entry_t *list_head);

/**
 * @brief                   Free the given address list entry
 *
 * Additionally to freeing the address entry, this function takes care that
 * the usage counter of the list entry's address is decremented.
 *
 * @param[in] addr_entry    Pointer to the address list entry to free
 */
void nhdp_free_addr_entry(nhdp_addr_entry_t *addr_entry);

/**
 * @brief                   Construct an address list containing the addresses of the given list
 *
 * @param[in] orig_list     Pointer to the head of the address list to 'clone'
 *
 * @return                  Pointer to the head of the newly created address list
 * @return                  NULL on error
 */
nhdp_addr_entry_t *nhdp_generate_new_addr_list(nhdp_addr_entry_t *orig_list);

/**
 * @brief                   Reset in_tmp_table flag of all NHDP addresses
 *
 * @note
 * Must not be called from outside the NHDP writer's message creation process.
 */
void nhdp_reset_addresses_tmp_usg(void);

#ifdef __cplusplus
}
#endif

#endif /* NHDP_ADDRESS_H_ */
/** @} */
