/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_gnrc_netreg  Network protocol registry
 * @ingroup     net_gnrc
 * @brief       Registry to receive messages of a specified protocol type by GNRC.
 * @{
 *
 * @file
 * @brief   Definitions to register network protocol PIDs to use with
 *          @ref net_gnrc_netapi.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <inttypes.h>

#include "sched.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/pkt.h"

#ifdef MODULE_GNRC_NETAPI_MBOX
#include "mbox.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MODULE_GNRC_NETAPI_MBOX) || defined(MODULE_GNRC_NETAPI_CALLBACKS) || \
    defined(DOXYGEN)
/**
 *  @brief  The type of the netreg entry.
 *
 *          Different types are available dependent on the used modules.
 */
typedef enum {
    /**
     * @brief   Use [default IPC](@ref core_msg) for
     *          [netapi](@ref net_gnrc_netapi) operations.
     *
     * @note    Implicitly chosen without `gnrc_netapi_mbox` and
     *          `gnrc_netapi_callbacks` modules.
     */
    GNRC_NETREG_TYPE_DEFAULT = 0,
#if defined(MODULE_GNRC_NETAPI_MBOX) || defined(DOXYGEN)
    /**
     * @brief   Use [centralized IPC](@ref core_mbox) for
     *          [netapi](@ref net_gnrc_netapi) operations.
     *
     * @note    Only available with `gnrc_netapi_mbox` module.
     */
    GNRC_NETREG_TYPE_MBOX,
#endif
#if defined(MODULE_GNRC_NETAPI_CALLBACKS) || defined(DOXYGEN)
    /**
     * @brief   Use function callbacks for [netapi](@ref net_gnrc_netapi)
     *          operations.
     *
     * @note    Only available with `gnrc_netapi_callbacks` module.
     */
    GNRC_NETREG_TYPE_CB,
#endif
} gnrc_netreg_type_t;
#endif

/**
 * @brief   Demux context value to get all packets of a certain type.
 *
 * @see gnrc_netreg_entry_t::demux_ctx
 */
#define GNRC_NETREG_DEMUX_CTX_ALL   (0xffff0000)

/**
 * @name    Static entry initialization macros
 * @anchor  net_gnrc_netreg_init_static
 * @{
 */
/**
 * @brief   Initializes a netreg entry statically with PID
 *
 * @param[in] demux_ctx The @ref gnrc_netreg_entry_t::demux_ctx "demux context"
 *                      for the netreg entry
 * @param[in] pid       The PID of the registering thread
 *
 * @return  An initialized netreg entry
 */
#if defined(MODULE_GNRC_NETAPI_MBOX) || defined(MODULE_GNRC_NETAPI_CALLBACKS)
#define GNRC_NETREG_ENTRY_INIT_PID(demux_ctx, pid)  { NULL, demux_ctx, \
                                                      GNRC_NETREG_TYPE_DEFAULT, \
                                                      { pid } }
#else
#define GNRC_NETREG_ENTRY_INIT_PID(demux_ctx, pid)  { NULL, demux_ctx, { pid } }
#endif

#if defined(MODULE_GNRC_NETAPI_MBOX) || defined(DOXYGEN)
/**
 * @brief   Initializes a netreg entry statically with mbox
 *
 * @param[in] demux_ctx The @ref gnrc_netreg_entry_t::demux_ctx "demux context"
 *                      for the netreg entry
 * @param[in] _mbox      Target @ref core_mbox "mailbox" for the registry entry
 *
 * @note    Only available with @ref net_gnrc_netapi_mbox.
 *
 * @return  An initialized netreg entry
 */
#define GNRC_NETREG_ENTRY_INIT_MBOX(demux_ctx, _mbox) { NULL, demux_ctx, \
                                                       GNRC_NETREG_TYPE_MBOX, \
                                                       { .mbox = _mbox } }
#endif

#if defined(MODULE_GNRC_NETAPI_CALLBACKS) || defined(DOXYGEN)
/**
 * @brief   Initializes a netreg entry statically with callback
 *
 * @param[in] demux_ctx The @ref gnrc_netreg_entry_t::demux_ctx "demux context"
 *                      for the netreg entry
 * @param[in] _cbd       Target callback for the registry entry
 *
 * @note    Only available with @ref net_gnrc_netapi_callbacks.
 *
 * @return  An initialized netreg entry
 */
#define GNRC_NETREG_ENTRY_INIT_CB(demux_ctx, _cbd)   { NULL, demux_ctx, \
                                                      GNRC_NETREG_TYPE_CB, \
                                                      { .cbd = _cbd } }
/** @} */

/**
 * @brief   Packet handler callback for netreg entries with callback.
 *
 * @pre `cmd` &isin; { @ref GNRC_NETAPI_MSG_TYPE_RCV, @ref GNRC_NETAPI_MSG_TYPE_SND }
 *
 * @note    Only available with @ref net_gnrc_netapi_callbacks.
 *
 * @param[in] cmd   @ref net_gnrc_netapi command type. Must be either
 *                  @ref GNRC_NETAPI_MSG_TYPE_SND or
 *                  @ref GNRC_NETAPI_MSG_TYPE_RCV
 * @param[in] pkt   The packet to handle.
 * @param[in] ctx   Application context.
 */
typedef void (*gnrc_netreg_entry_cb_t)(uint16_t cmd, gnrc_pktsnip_t *pkt,
                                       void *ctx);

/**
 * @brief   Callback + Context descriptor
 * @note    Only available with @ref net_gnrc_netapi_callbacks.
 */
typedef struct {
    gnrc_netreg_entry_cb_t cb;  /**< the callback */
    void *ctx;                  /**< application context for the callback */
} gnrc_netreg_entry_cbd_t;
#endif

/**
 * @brief   Entry to the @ref net_gnrc_netreg
 */
typedef struct gnrc_netreg_entry {
    /**
     * @brief next element in list
     *
     * @internal
     */
    struct gnrc_netreg_entry *next;

    /**
     * @brief   The demultiplexing context for the registering thread.
     *
     * @details This can be defined by the network protocol themselves.
     *          E. g. protocol numbers / next header numbers in IPv4/IPv6,
     *          ports in UDP/TCP, or similar.
     */
    uint32_t demux_ctx;
#if defined(MODULE_GNRC_NETAPI_MBOX) || defined(MODULE_GNRC_NETAPI_CALLBACKS) || \
    defined(DOXYGEN)
    /**
     * @brief   Type of the registry entry
     *
     * @note    Only available with @ref net_gnrc_netapi_mbox or
     *          @ref net_gnrc_netapi_callbacks.
     */
    gnrc_netreg_type_t type;
#endif
    union {
        kernel_pid_t pid;       /**< The PID of the registering thread */
#if defined(MODULE_GNRC_NETAPI_MBOX) || defined(DOXYGEN)
        /**
         * @brief   Target @ref core_mbox "mailbox" for the registry entry
         *
         * @note    Only available with @ref net_gnrc_netapi_mbox.
         */
        mbox_t *mbox;
#endif

#if defined(MODULE_GNRC_NETAPI_CALLBACKS) || defined(DOXYGEN)
        /**
         * @brief   Target callback for the registry entry
         *
         * @note    Only available with @ref net_gnrc_netapi_callbacks.
         */
        gnrc_netreg_entry_cbd_t *cbd;
#endif
    } target;                   /**< Target for the registry entry */
} gnrc_netreg_entry_t;

/**
 * @brief   The global locking of netregs
 *
 * A shared lock must be held across calls to @ref gnrc_netreg_lookup, and
 * references obtained through that may only be used for the duration for which
 * the lock is held.
 *
 * The shared lock is counting (i.e. multiple sections of code can acquire it
 * concurrently), and may be held across blocking operations, as long as they
 * don't involve the exclusive lock (e.g. by blocking on a socket creation).
 * It's generally good practice to release them as soon as possible.
 *
 * There is an exclusive counterpart to the lock, which is
 * internal to netreg (and used through functions such as @ref
 * gnrc_netreg_register and @ref gnrc_netreg_unregister). The current
 * implementation prioritizes shared locks. This means that shared locks are
 * generally acquired fast (they only block if an exclusive operation has
 * already started), but constant access through shared locks might starve
 * registration and deregistration.
 *
 * @{
 */

/** @brief Acquire a shared lock on the GNRC netreg
 *
 * This needs to be held around all calls to @ref gnrc_netreg_lookup, and for
 * as long as any of its results are used. After that, call @ref
 * gnrc_netreg_release_shared.
 */
void gnrc_netreg_acquire_shared(void);
/** @brief Release a shared lock on the GNRC netreg
 *
 * @pre @ref gnrc_netreg_acquire_shared was called
 */
void gnrc_netreg_release_shared(void);

/** @} */

/**
 * @brief   Initializes module.
 */
void gnrc_netreg_init(void);

/**
 * @name    Dynamic entry initialization functions
 * @anchor  net_gnrc_netreg_init_dyn
 * @{
 */
/**
 * @brief   Initializes a netreg entry dynamically with PID
 *
 * @param[out] entry    A netreg entry
 * @param[in] demux_ctx The @ref gnrc_netreg_entry_t::demux_ctx "demux context"
 *                      for the netreg entry
 * @param[in] pid       The PID of the registering thread
 *
 */
static inline void gnrc_netreg_entry_init_pid(gnrc_netreg_entry_t *entry,
                                              uint32_t demux_ctx,
                                              kernel_pid_t pid)
{
    entry->next = NULL;
    entry->demux_ctx = demux_ctx;
#if defined(MODULE_GNRC_NETAPI_MBOX) || defined(MODULE_GNRC_NETAPI_CALLBACKS)
    entry->type = GNRC_NETREG_TYPE_DEFAULT;
#endif
    entry->target.pid = pid;
}

#if defined(MODULE_GNRC_NETAPI_MBOX) || defined(DOXYGEN)
/**
 * @brief   Initializes a netreg entry dynamically with mbox
 *
 * @param[out] entry    A netreg entry
 * @param[in] demux_ctx The @ref gnrc_netreg_entry_t::demux_ctx "demux context"
 *                      for the netreg entry
 * @param[in] mbox      Target @ref core_mbox "mailbox" for the registry entry
 *
 * @note    Only available with @ref net_gnrc_netapi_mbox.
 */
static inline void gnrc_netreg_entry_init_mbox(gnrc_netreg_entry_t *entry,
                                               uint32_t demux_ctx,
                                               mbox_t *mbox)
{
    entry->next = NULL;
    entry->demux_ctx = demux_ctx;
    entry->type = GNRC_NETREG_TYPE_MBOX;
    entry->target.mbox = mbox;
}
#endif

#if defined(MODULE_GNRC_NETAPI_CALLBACKS) || defined(DOXYGEN)
/**
 * @brief   Initializes a netreg entry dynamically with callback
 *
 * @param[out] entry    A netreg entry
 * @param[in] demux_ctx The @ref gnrc_netreg_entry_t::demux_ctx "demux context"
 *                      for the netreg entry
 * @param[in] cbd       Target callback for the registry entry
 *
 * @note    Only available with @ref net_gnrc_netapi_callbacks.
 */
static inline void gnrc_netreg_entry_init_cb(gnrc_netreg_entry_t *entry,
                                             uint32_t demux_ctx,
                                             gnrc_netreg_entry_cbd_t *cbd)
{
    entry->next = NULL;
    entry->demux_ctx = demux_ctx;
    entry->type = GNRC_NETREG_TYPE_CB;
    entry->target.cbd = cbd;
}
#endif
/** @} */

/**
 * @brief   Registers a thread to the registry.
 *
 * @details The semantics are: Thread gnrc_netreg_entry_t::pid is interested in
 *          packets or network events of protocol @p type with context
 *          gnrc_netreg_entry_t::demux_ctx.
 *
 * @param[in] type      Type of the protocol. Must not be < GNRC_NETTYPE_UNDEF or
 *                      >= GNRC_NETTYPE_NUMOF.
 * @param[in] entry     An entry you want to add to the registry. This needs to
 *                      be initialized before hand using the @ref
 *                      net_gnrc_netreg_init_static "static" or @ref
 *                      net_gnrc_netreg_init_dyn "dynamic" initialization
 *                      helpers.
 *
 * @warning Call gnrc_netreg_unregister() *before* you leave the context you
 *          allocated @p entry in. Otherwise it might get overwritten.
 *
 * @pre The calling thread must provide a [message queue](@ref msg_init_queue)
 *      when using @ref GNRC_NETREG_TYPE_DEFAULT for gnrc_netreg_entry_t::type
 *      of @p entry.
 *
 * @return  0 on success
 * @return  -EINVAL if @p type was < GNRC_NETTYPE_UNDEF or >= GNRC_NETTYPE_NUMOF
 */
int gnrc_netreg_register(gnrc_nettype_t type, gnrc_netreg_entry_t *entry);

/**
 * @brief   Removes a thread from the registry.
 *
 * @param[in] type      Type of the protocol.
 * @param[in] entry     An entry you want to remove from the registry.
 */
void gnrc_netreg_unregister(gnrc_nettype_t type, gnrc_netreg_entry_t *entry);

/**
 * @brief   Searches for entries with given parameters in the registry and
 *          returns the first found.
 *
 * @pre The caller must hold the lock of @ref gnrc_netreg_acquire_shared from
 *      before calling this function, and must stop using any obtained pointers
 *      before releasing the lock through @ref gnrc_netreg_release_shared.
 *
 * @param[in] type      Type of the protocol.
 * @param[in] demux_ctx The demultiplexing context for the registered thread.
 *                      See gnrc_netreg_entry_t::demux_ctx.
 *
 * @return  The first entry fitting the given parameters on success
 * @return  NULL if no entry can be found.
 */
gnrc_netreg_entry_t *gnrc_netreg_lookup(gnrc_nettype_t type, uint32_t demux_ctx);

/**
 * @brief   Returns number of entries with the same gnrc_netreg_entry_t::type and
 *          gnrc_netreg_entry_t::demux_ctx.
 *
 * @param[in] type      Type of the protocol.
 * @param[in] demux_ctx The demultiplexing context for the registered thread.
 *                      See gnrc_netreg_entry_t::demux_ctx.
 *
 * @return  Number of entries with the same gnrc_netreg_entry_t::type and
 *          gnrc_netreg_entry_t::demux_ctx as the given parameters.
 *
 * Note that this returns a snapshot value, which may change at any time after
 * that call. This is fine for most applications, as they just shortcut a code
 * path if the number is zero. Callers that need that number to stay constant
 * can acquire a shared lock through @ref gnrc_netreg_acquire_shared, and rely
 * on the number staying constant until that lock is released through @ref
 * gnrc_netreg_release_shared.
 */
int gnrc_netreg_num(gnrc_nettype_t type, uint32_t demux_ctx);

/**
 * @brief   Returns the next entry after @p entry with the same
 *          gnrc_netreg_entry_t::type and gnrc_netreg_entry_t::demux_ctx as the
 *          given entry.
 *
 * The requirement on holding the global lock through @ref
 * gnrc_netreg_acquire_shared from @ref gnrc_netreg_lookup extends to any
 * results of this function: It may only be released when none of the pointers
 * are used any more.
 *
 * @param[in] entry     A registry entry retrieved by gnrc_netreg_lookup() or
 *                      gnrc_netreg_getnext(). Must not be NULL.
 *
 * @return  The next entry after @p entry fitting the given parameters on success
 * @return  NULL if no entry new entry can be found.
 */
gnrc_netreg_entry_t *gnrc_netreg_getnext(gnrc_netreg_entry_t *entry);

/**
 * @brief   Calculates the checksum for a header.
 *
 * @param[in] hdr           The header the checksum should be calculated
 *                          for.
 * @param[in] pseudo_hdr    The header the pseudo header shall be generated
 *                          from. NULL if none is needed.
 *
 * @return  0, on success.
 * @return  -EINVAL, if @p pseudo_hdr is NULL but a pseudo header was required.
 * @return  -ENOENT, if @ref net_gnrc_netreg does not know how to calculate checksum
 *          for gnrc_pktsnip_t::type of @p hdr.
 */

int gnrc_netreg_calc_csum(gnrc_pktsnip_t *hdr, gnrc_pktsnip_t *pseudo_hdr);

#ifdef __cplusplus
}
#endif

/** @} */
