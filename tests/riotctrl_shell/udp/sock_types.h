/*
 * SPDX-FileCopyrightText: 2026 Martine Lenders <mlenders@riot-os.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup     tests
 * @{
 *
 * @file
 * @brief   Provides @ref net_sock mock types
 *
 * @author  Martine Lenders <mlenders@riot-os.org>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "net/sock/async/types.h"

struct sock_udp {
    sock_async_ctx_t async_ctx;
};

#ifdef __cplusplus
}
#endif

/** @} */
