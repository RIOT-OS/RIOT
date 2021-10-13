/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef CONGURE_IMPL_H
#define CONGURE_IMPL_H

#include "congure/mock.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef congure_mock_snd_t congure_test_snd_t;

int congure_test_snd_setup(congure_test_snd_t *c, unsigned id);

#ifdef __cplusplus
}
#endif

#endif /* CONGURE_IMPL_H */
/** @} */
