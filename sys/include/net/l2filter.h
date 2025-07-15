/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @experimental
 * @defgroup    net_l2filter Link layer address filter
 * @ingroup     net
 * @brief       Module for filtering (black- or white-listing) link layer
 *              addresses
 *
 * This module is used as a base for filtering link layer addresses. It allows
 * to be (globally) configured in either blacklist or in whitelist mode. This
 * configuration is done via RIOTs module system, for whitelist mode simply
 * include the module `L2FILTER_WHITELIST`, for blacklist mode include
 * `L2FILTER_BLACKLIST`.
 *
 * The actual memory for the filter lists should be allocated for every network
 * device. This is done centrally in netdev_t type.
 *
 * @{
 * @file
 * @brief       Link layer address filter interface definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_l2filter_conf Link layer address filter compile configurations
 * @ingroup config
 * @{
 */
/**
 * @brief   Maximal length of addresses that can be stored in the filter list
 */
#ifndef CONFIG_L2FILTER_ADDR_MAXLEN
#define CONFIG_L2FILTER_ADDR_MAXLEN            (8U)
#endif

/**
 * @brief   Number of slots in each filter list (filter entries per device)
 */
#ifndef CONFIG_L2FILTER_LISTSIZE
#define CONFIG_L2FILTER_LISTSIZE               (8U)
#endif
/** @} */

/**
 * @brief   Filter list entries
 *
 * The filter list supports address entries with differing length. This is
 * useful e.g. for IEEE802.15.4, where the list can store short and long
 * addresses at the same time.
 */
typedef struct {
    uint8_t addr[CONFIG_L2FILTER_ADDR_MAXLEN];     /**< link layer address */
    size_t addr_len;                               /**< address length in byte */
} l2filter_t;

/**
 * @brief   Add an entry to a devices filter list
 *
 * @param[in,out] list  pointer to the filter list
 * @param[in] addr      address to be added to list
 * @param[in] addr_len  size of @p addr [in byte]
 *
 * @pre     @p list != NULL
 * @pre     @p addr != NULL
 * @pre     @p addr_maxlen <= @ref CONFIG_L2FILTER_ADDR_MAXLEN
 *
 * @retval  0 on success
 * @retval  -ENOMEM if no empty slot left in list
 * @retval  -EINVAL if `addr_len > CONFIG_L2FILTER_ADDR_MAXLEN` **AND** `NDEBUG` is enabled.
 *          Otherwise this condition will trip an `assert()`.
 */
int l2filter_add(l2filter_t *list, const void *addr, size_t addr_len);

/**
 * @brief   Remove an entry from the given filter list
 *
 * @param[in,out] list  pointer to the filter list
 * @param[in] addr      address to remove from the list
 * @param[in] addr_len  length of @p addr [in byte]
 *
 * @pre     @p list != NULL
 * @pre     @p addr != NULL
 * @pre     @p addr_maxlen <= @ref CONFIG_L2FILTER_ADDR_MAXLEN
 *
 * @return  0 on success
 * @return  -ENOENT if @p addr was not found in @p list
 */
int l2filter_rm(l2filter_t *list, const void *addr, size_t addr_len);

/**
 * @brief   Check if the given address passes the set filters
 *
 * Independent if the l2filter module is used in blacklist or in whitelist mode,
 * this function returns whether the given address passes the filter. In
 * whitelist mode, this means that the given address has to be in the list, in
 * blacklist mode this means that the given address is not in the list.
 *
 * @param[in] list      list with black-/whitelisted addresses
 * @param[in] addr      address to check against the entries in @p list
 * @param[in] addr_len  length of @p addr [in byte]
 *
 * @pre     @p list != NULL
 * @pre     @p addr != NULL
 * @pre     @p addr_maxlen <= @ref CONFIG_L2FILTER_ADDR_MAXLEN
 *
 * @return  in whitelist mode: true if @p addr is in @p list
 * @return  in whitelist mode: false if @p addr is not in @p list
 * @return  in blacklist mode: true if @p addr is not in @p list
 * @return  in blacklist mode: false if @p addr is in @p list
 */
bool l2filter_pass(const l2filter_t *list, const void *addr, size_t addr_len);

#ifdef __cplusplus
}
#endif

/** @} */
