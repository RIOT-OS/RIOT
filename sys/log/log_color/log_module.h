/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_log_color Colored log module
 * @ingroup     sys
 * @brief       This module implements a logging module with colored output
 * @{
 *
 * @file
 * @brief       log_module header
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LOG_MODULE_H
#define LOG_MODULE_H

void log_write(unsigned level, const char *format, ...);
#endif /* LOG_MODULE_H */
