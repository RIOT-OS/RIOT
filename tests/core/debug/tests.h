/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void disabled(void);

void noprefix(void);

void prefix(void);
void prefix_func(void);
void prefix_thread(void);
void prefix_thread_func(void);


#ifdef __cplusplus
}
#endif
