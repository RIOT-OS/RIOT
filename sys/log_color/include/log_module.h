/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
