/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup net_sock_async_msg
 * @brief
 * @{
 *
 * @file
 * @brief   Type definitions for asynchronous socks with @ref core_msg
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef SOCK_ASYNC_CTX_H
#define SOCK_ASYNC_CTX_H

#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Asynchronous context for @ref net_sock_async_msg
 */
typedef kernel_pid_t sock_async_ctx_t;

#ifdef __cplusplus
}
#endif

#endif /* SOCK_ASYNC_CTX_H */
/** @} */
