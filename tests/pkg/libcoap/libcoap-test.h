/*
 * libcoap-test.h -- RIOT client example
 *
 * Copyright (C) 2023-2025 Jon Shallow <supjps-libcoap@jpshallow.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

#ifndef LIBCOAP_TEST_H
#define LIBCOAP_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Start up the CoAP Server */
void libcoap_test_run(void);

#ifdef __cplusplus
}
#endif

#endif /* LIBCOAP_TEST_H */
