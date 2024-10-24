/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Inria
 *               2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_suit
 * @defgroup    sys_suit_transport_worker SUIT firmware worker thread
 * @brief       SUIT secure firmware updates worker thread
 *
 * @{
 *
 * @brief       SUIT CoAP helper API
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#ifndef SUIT_TRANSPORT_WORKER_H
#define SUIT_TRANSPORT_WORKER_H

#include "net/nanocoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Trigger a SUIT update via a worker thread
 *
 * @param[in] url       url pointer containing the full coap url to the manifest
 * @param[in] len       length of the url
 */
void suit_worker_trigger(const char *url, size_t len);

/**
 * @brief   Trigger a SUIT update via a worker thread
 *
 * @pre The caller called @ref suit_worker_try_prepare to obtain the @p buf, and
 * populated @p size bytes into it.
 *
 * This can be called with a size of 0 when writing a manifest was started, but
 * could not be completed.
 *
 * @param[in] manifest  Pointer to the manifest. This must be the return value
 *                      of a previous call to @ref suit_worker_try_prepare, and is
 *                      invalidated at some point during or after the call.
 * @param[in] size      Number of bytes that have been prepared in
 *                      the buffer before the call.
 */
void suit_worker_trigger_prepared(const uint8_t *manifest, size_t size);

/**
 * @brief    Prepare for a worker run with a preloaded manifest
 *
 * This obtains a lock on the SUIT worker, and provides a pointer to a memory
 * area into which the manifest is to be written. The lock must be released by
 * calling @ref suit_worker_trigger_prepared later.
 *
 * @param[out]   buffer    On success, buffer into which the image may be
 *                         written.
 * @param[inout] size      Requested buffer size. On some errors, this will be
 *                         decreased to a size that would be acceptable.
 *
 * @return 0 on success
 * @return -EAGAIN if the worker is currently busy.
 * @return -ENOMEM if the worker is available but the requested size is too
 *         large. (In this case, an acceptable size is indicated in size).
 *
 * @note There is no blocking version of this (it behaves like @ref
 * mutex_trylock, not like @ref mutex_lock). This allows a simpler
 * implementation on the thread handling side<!-- possibly it's not even
 * possible without priority inversion tricks -->, and is also what typical use
 * cases need.
 */
int suit_worker_try_prepare(uint8_t **buffer, size_t *size);

/**
 * @brief   Callback that is executed after the SUIT process has finished
 *
 * @param[in] res   Result of the SUIT update, 0 on success
 *
 * By default this will reboot the board, can be overwritten by the application.
 */
void suit_worker_done_cb(int res);

/**
 * @brief   Trigger a SUIT update
 *
 * @note Make sure the thread calling this function has enough stack space to fit
 *       a whole flash page.
 *
 * This function accesses global state shared with @ref
 * suit_handle_manifest_buf; only one of those may be called at the same time.
 * You may use @ref suit_worker_trigger instead, which manages locking and also
 * does away with the stack space requirement by being executed on an own
 * stack.
 *
 * @param[in] url       url pointer containing the full coap url to the manifest
 *
 * @return 0 on success
 *        <0 on error
 */
int suit_handle_url(const char *url);

/**
 * @brief   Trigger a SUIT update on an in-memory manifest
 *
 * @note Make sure the thread calling this function has enough stack space to fit
 *       a whole flash page.
 *
 * This function accesses global state shared with @ref suit_handle_url; only
 * one of those may be called at the same time. You may use @ref
 * suit_worker_try_prepare / @ref suit_worker_trigger_prepared instead, which
 * manage locking and also do away with the stack space requirement by being
 * executed on an own stack.
 *
 * @param[in] buffer     Memory area containing a SUIT manifest.
 * @param[in] size       Size of the manifest in @p buffer.
 *
 * @return 0 on success
 *        <0 on error
 */
int suit_handle_manifest_buf(const uint8_t *buffer, size_t size);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SUIT_TRANSPORT_WORKER_H */
