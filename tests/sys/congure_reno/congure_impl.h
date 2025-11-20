/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @{
 *
 * @file
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "congure/reno.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef congure_reno_snd_t congure_test_snd_t;

int congure_test_snd_setup(congure_test_snd_t *c, unsigned id);
unsigned congure_reno_test_get_fr_calls(void);
void congure_reno_test_set_same_wnd_adv_res(bool value);

#ifdef __cplusplus
}
#endif

/** @} */
