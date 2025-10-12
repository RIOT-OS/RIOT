/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

#include "net/unicoap/constants.h" /* IWYU pragma: export */
#include "net/unicoap/config.h" /* IWYU pragma: export */
#include "net/unicoap/message.h" /* IWYU pragma: export */
#include "net/unicoap/options.h" /* IWYU pragma: export */
#include "net/unicoap/application.h" /* IWYU pragma: export */
#include "net/unicoap/transport.h" /* IWYU pragma: export */

/**
 * @addtogroup net_unicoap
 * @{
 */

/**
 * @file
 * @brief  Main header for `unicoap`
 * @author Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

/* MARK: - Controlling the unicoap instance */
/**
 * @name Controlling the unicoap instance
 * @{
 */
/**
 * @brief Initializes the unicoap stack
 *
 * If you disable the `auto_init_unicoap` you will need to call this function manually.
 * Otherwise, and provided `auto_init` is used, unicoap will be initialized automatically.
 *
 * @returns unicoap thread PID
 */
kernel_pid_t unicoap_init(void);

/**
 * @brief Tears down the unicoap stack, closing the background thread.
 *
 * Provided @ref CONFIG_UNICOAP_CREATE_THREAD is enabled, this function will also zombify the
 * thread created on initialization.
 *
 * @returns Zero on success, `-1` otherwise
 */
int unicoap_deinit(void);

#if !defined(DOXYGEN) && !IS_ACTIVE(CONFIG_UNICOAP_CREATE_THREAD)
/* Internal thread function */
void* _unicoap_loop_run(void* arg);
#endif

#if defined(DOXYGEN) || !IS_ACTIVE(CONFIG_UNICOAP_CREATE_THREAD)
/**
 * @brief Runs `unicoap` processing loop
 *
 * This function never returns, unless explicitly instructed using @ref unicoap_deinit.
 *
 * @warning You must not call this function when @ref CONFIG_UNICOAP_CREATE_THREAD is enabled.
 * If @ref CONFIG_UNICOAP_CREATE_THREAD is enabled, this function is not defined.
 *
 * @returns Never
 */
static inline void unicoap_loop_run(void)
{
    _unicoap_loop_run(NULL);
}
#endif

/**
 * @brief Schedules @p event to be run in the internal processing loop
 *        at the next possible instance
 *
 * @param[in,out] event Event with function to be run in `unicoap` message processing loop
 *
 * @returns Negative error number in case of failure or zero on success.
 *
 * This function facilitates running client requests when @ref CONFIG_UNICOAP_CREATE_THREAD is
 * disabled.
 *
 * @remark You can start enqueuing jobs even before the `unicoap` processing loop has been started.
 * The jobs will become eligible for execution once the loop starts running, which usually is when
 * the `unicoap` thread has been created. If `CONFIG_UNICOAP_CREATE_THREAD` is disabled,
 * this will be after you have called @ref unicoap_loop_run, which blocks.
 *
 * ```
 * static event_t sample = { .handler = send_my_request };
 * ```
 */
int unicoap_loop_enqueue(event_t* event);
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */
