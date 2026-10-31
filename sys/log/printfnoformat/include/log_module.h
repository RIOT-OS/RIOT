/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_log_printfnoformat log_printfnoformat: puts log module
 * @ingroup     sys
 * @brief       This module implements an example logging module using puts to
 *              just print the format string saving on the number of libraries need
 * @{
 *
 * @file
 * @brief       log_module header
 *
 * @author      Jason Linehan <patientulysses@gmail.com>
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>

#include "fmt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_FORMAT_PRINT_PREFIX
#define LOG_FORMAT(level, unit, format, ...) format

#ifdef __cplusplus
}
#endif
/**@}*/
