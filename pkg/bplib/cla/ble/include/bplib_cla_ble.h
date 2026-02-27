/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int bplib_cla_ble_start(uint32_t task_id);
void bplib_cla_ble_stop(void);

#ifdef __cplusplus
}
#endif