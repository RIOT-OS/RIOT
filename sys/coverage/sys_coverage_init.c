/*
 * Copyright (c) 2022 University of Bremen
 *
 * SPDX-License-Identifier: Apache-2.0
 */

void auto_init_sys_coverage(void)
{
    extern void gcov_static_init(void);
    gcov_static_init();
}
