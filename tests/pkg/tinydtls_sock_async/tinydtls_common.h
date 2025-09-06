/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup tests
 *
 * @{
 * @file
 * @brief   Common definitions for client and server in tinyDTLS `sock_async` test
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "event.h"

#ifdef __cplusplus
extern "C"
{
#endif
#ifndef DTLS_DEFAULT_PORT
#define DTLS_DEFAULT_PORT (20220) /* DTLS default port */
#endif

typedef struct {
    event_t super;
    sock_dtls_t *sock;
} tinydtls_sock_event_t;

#ifdef __cplusplus
}
#endif
