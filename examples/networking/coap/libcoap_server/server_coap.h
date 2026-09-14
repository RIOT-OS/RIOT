/*
 * SPDX-FileCopyrightText: 2023-2026 Jon Shallow <supjps-libcoap@jpshallow.com>
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       libcoap Server Example Implementation
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 *
 * @author      Jon Shallow <supjps-libcoap@jpshallow.com>
 *
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Start up the CoAP Server */
void server_coap_init(int argc, char **argv);

#ifdef __cplusplus
}
#endif
