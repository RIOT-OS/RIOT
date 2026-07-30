/*
 * SPDX-FileCopyrightText: 2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ANSI color escape code for red
 */
#define ANSI_COLOR_RED      "\x1b[31m"

/**
 * @brief   ANSI color escape code for green
 */
#define ANSI_COLOR_GREEN    "\x1b[32m"

/**
 * @brief   ANSI color escape code for yellow
 */
#define ANSI_COLOR_YELLOW   "\x1b[33m"

/**
 * @brief   ANSI color escape code for blue
 */
#define ANSI_COLOR_BLUE     "\x1b[34m"

/**
 * @brief   ANSI color escape code for magenta
 */
#define ANSI_COLOR_MAGENTA  "\x1b[35m"

/**
 * @brief   ANSI color escape code for cyan
 */
#define ANSI_COLOR_CYAN     "\x1b[36m"

/**
 * @brief   ANSI color escape code for white
 */
#define ANSI_COLOR_WHITE    "\x1b[37m"

/**
 * @brief   ANSI color escape code for resetting
 */
#define ANSI_COLOR_RESET    "\x1b[0m"

#ifdef __cplusplus
}
#endif
