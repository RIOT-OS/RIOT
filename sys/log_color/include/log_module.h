/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_log_color log_color: Colored log module
 * @ingroup     sys
 * @brief       This module implements a logging module with colored output
 * @{
 *
 * @file
 * @brief       log_module header
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @brief log_write overridden function for colored output
 *
 * @param[in] level  Logging level
 * @param[in] format String format to print
 */
void log_write(unsigned level, const char *format, ...);

#ifdef __cplusplus
}
#endif
/**@}*/
