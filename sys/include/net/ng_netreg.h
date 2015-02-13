/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    ng_netreg  Network protocol registry
 * @ingroup     net
 * @{
 *
 * @file
 * @brief   Definitions to register network protocol PIDs to use with
 *          @ref net_ng_netapi.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NETREG_H_
#define NETREG_H_

#include <inttypes.h>

#include "kernel_types.h"
#include "net/ng_nettype.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Number of maximum entries to @ref ng_netreg
 * @def NG_NETREG_SIZE
 */
#ifndef NG_NETREG_SIZE
#define NG_NETREG_SIZE (NG_NETTYPE_NUMOF)
#endif

/**
 * @brief   Entry to the @ref ng_netreg
 */
typedef struct ng_netreg_entry {
    struct ng_netreg_entry *next;   /**< next element in list */

    /**
     * @brief   The demultiplexing context for the registering thread.
     *
     * @details This can be defined by the network protocol themselves.
     *          E. g. protocol numbers / next header numbers in IPv4/IPv6,
     *          ports in UDP/TCP, or similar.
     */
    uint16_t demux_ctx;
    kernel_pid_t pid;       /**< The PID of the registering thread */
} ng_netreg_entry_t;

/**
 * @brief   Initializes module.
 */
void ng_netreg_init(void);

/**
 * @brief   Registers a thread to the registry.
 *
 * @details The semantics are: Thread @p pid is interested in packets of
 *          protocol @p type with context @p demux_ctx.
 *
 * @param[in] type      Type of the protocol. Must not be NG_NETTYPE_UNDEF or
 *                      >= NG_NETTYPE_NUMOF.
 * @param[in] demux_ctx The demultiplexing context for the registering thread.
 *                      See ng_netreg_entry_t::demux_ctx.
 * @param[in] pid       The PID of the registering thread.
 *
 * @return  0 on success
 * @return  -ENOMEM if no space is left in the registry.
 * @return  -EINVAL if @p type was NG_NETTYPE_UNDEF or >= NG_NETTYPE_NUMOF
 */
int ng_netreg_register(ng_nettype_t type, uint16_t demux_ctx, kernel_pid_t pid);

/**
 * @brief   Removes a thread from the registry.
 *
 * @param[in] type      Type of the protocol.
 * @param[in] demux_ctx The demultiplexing context for the registered thread.
 *                      See ng_netreg_entry_t::demux_ctx.
 * @param[in] pid       The PID of the registered thread.
 */
void ng_netreg_unregister(ng_nettype_t type, uint16_t demux_ctx, kernel_pid_t pid);

/**
 * @brief   Searches for entries with given parameters in the registry and
 *          returns the first found.
 *
 * @param[in] type      Type of the protocol.
 * @param[in] demux_ctx The demultiplexing context for the registered thread.
 *                      See ng_netreg_entry_t::demux_ctx.
 *
 * @return  The first entry fitting the given parameters on success
 * @return  NULL if no entry can be found.
 */
ng_netreg_entry_t *ng_netreg_lookup(ng_nettype_t type, uint16_t demux_ctx);

/**
 * @brief   Returns number of entries with the same ng_netreg_entry_t::type and
 *          ng_netreg_entry_t::demux_ctx.
 *
 * @param[in] type      Type of the protocol.
 * @param[in] demux_ctx The demultiplexing context for the registered thread.
 *                      See ng_netreg_entry_t::demux_ctx.
 *
 * @return  Number of entries with the same ng_netreg_entry_t::type and
 *          ng_netreg_entry_t::demux_ctx as the given parameters.
 */
int ng_netreg_num(ng_nettype_t type, uint16_t demux_ctx);

/**
 * @brief   Returns the next entry after @p entry with the same
 *          ng_netreg_entry_t::type and ng_netreg_entry_t::demux_ctx as the
 *          given entry.
 *
 * @param[in] entry     A registry entry retrieved by ng_netreg_lookup() or
 *                      ng_netreg_getnext(). Must not be NULL.
 *
 * @return  The next entry after @p entry fitting the given parameters on success
 * @return  NULL if no entry new entry can be found.
 */
ng_netreg_entry_t *ng_netreg_getnext(ng_netreg_entry_t *entry);

#ifdef __cplusplus
}
#endif

#endif /* NETREG_H_ */
/** @} */
