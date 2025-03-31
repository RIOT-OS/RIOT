/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CB_MUX_H
#define CB_MUX_H

/**
 * @defgroup    sys_cb_mux Callback multiplexer
 * @ingroup     sys
 * @brief       cb_mux provides utilities for storing, retrieving, and managing
 *              callback information in a singly linked list.
 *
 * If an API provides the ability to call multiple callbacks, cb_mux can
 * simplify handling of an arbitrary number of callbacks by requiring memory
 * for a cb_mux entry to be passed along with other arguments. The cb_mux entry
 * is then attached to a list using cb_mux_add. The code implementing that API
 * can manage the list using the various utility functions that cb_mux provides.
 *
 * @{
 *
 * @file
 * @brief       cb_mux interface definitions
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#include <stdint.h>

/* For alternate cb_mux_cbid_t */
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_CB_MUX_CBID_T
/**
 * @brief   cb_mux identifier type
 */
typedef unsigned int cb_mux_cbid_t;
#endif

/**
 * @brief   cb_mux callback type
 */
typedef void (*cb_mux_cb_t)(void *);

/**
 * @brief   cb_mux list entry structure
 */
typedef struct cb_mux {
    struct cb_mux *next;  /**< next entry in the cb_mux list */
    cb_mux_cbid_t cbid;   /**< identifier for this callback */
    void *info;           /**< optional extra information */
    cb_mux_cb_t cb;       /**< callback function */
    void *arg;            /**< argument for callback function */
} cb_mux_t;

/**
 * @brief   cb_mux iterate function callback type for cb_mux_iter
 */
typedef void (*cb_mux_iter_t)(cb_mux_t *, void *);

/**
 * @brief   Add a new entry to the end of a cb_mux list
 *
 * @param[in] head   double pointer to first list entry
 * @param[in] entry  entry to add
 */
void cb_mux_add(cb_mux_t **head, cb_mux_t *entry);

/**
 * @brief   Remove a entry from a cb_mux list
 *
 * @param[in] head   double pointer to first list entry
 * @param[in] entry  entry to remove
 */
void cb_mux_del(cb_mux_t **head, cb_mux_t *entry);

/**
 * @brief   Find an entry in the list by ID
 *
 * @param[in] head      pointer to first list entry
 * @param[in] cbid_val  ID to find
 *
 * @return pointer to the list entry
 */
cb_mux_t *cb_mux_find_cbid(cb_mux_t *head, cb_mux_cbid_t cbid_val);

/**
 * @brief   Find the entry with the lowest ID
 *
 * If there are multiple hits, this returns the oldest.
 *
 * @param[in] head   pointer to first list entry
 *
 * @return pointer to the list entry
 */
cb_mux_t *cb_mux_find_low(cb_mux_t *head);

/**
 * @brief   Find the entry with the highest ID
 *
 * If there are multiple hits, this returns the oldest.
 *
 * @param[in] head   pointer to first list entry
 *
 * @return pointer to the list entry
 */
cb_mux_t *cb_mux_find_high(cb_mux_t *head);

/**
 * @brief   Find the lowest unused ID
 *
 * Returns highest possible ID on failure
 *
 * @param[in] head   pointer to first list entry
 *
 * @return lowest unused ID
 */
cb_mux_cbid_t cb_mux_find_free_id(cb_mux_t *head);

/**
 * @brief   Run a function on every item in the cb_mux list
 *
 * @param[in] head  pointer to first list entry
 * @param[in] func  function to run on each entry
 * @param[in] arg   argument for the function
 */
void cb_mux_iter(cb_mux_t *head, cb_mux_iter_t func, void *arg);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* CB_MUX_H */
