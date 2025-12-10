/*
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @{
 *
 * @file
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "congure/quic.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef congure_quic_snd_t congure_test_snd_t;

int congure_test_snd_setup(congure_test_snd_t *c, unsigned id);
unsigned congure_quic_test_get_event_cb_calls(void);
void *congure_quic_test_get_event_cb_arg(void);

#ifdef __cplusplus
}
#endif

/** @} */
