/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    test_utils_main_exit_cb Callback on exit of main()
 * @ingroup     sys
 * @brief       Run a callback with the return value of `main()`
 *
 * When the pseudomodule `test_utils_main_exit_cb` is used,
 * @ref test_utils_main_exit_cb is called with the return value of `main()`
 * after `main()` returned. The application has to implement the callback.
 *
 * @{
 * @file
 * @brief       Callback on exit of main()
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Callback called after `main()` returned
 *
 * @note    Has to be implemented by the application.
 *
 * @param[in]   res     return value of `main()`
 */
void test_utils_main_exit_cb(int res);

#ifdef __cplusplus
}
#endif
/** @} */
