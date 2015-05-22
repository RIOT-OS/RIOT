/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_log_printfnoformat puts log module
 * @ingroup     sys
 * @brief       This module implements an example logging module
 * @{
 *
 * @file
 * @brief       log_module header
 *
 * @author      Jason Linehan <patientulysses@gmail.com>
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __LOG_FORMAT_H
#define __LOG_FORMAT_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief log_write overridden function
 *
 * This example function will only print the log's format string.
 * Use it where printf might be too heavy.
 *
 * @param[in] level (unused)
 * @param[in] format String that the function will print
 */
static inline void log_write(unsigned level, const char *format, ...) {
    (void)level;
    puts(format);
}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* __LOG_FORMAT_H */
