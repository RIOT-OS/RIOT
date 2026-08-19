/*
 * SPDX-FileCopyrightText: 2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ANSI color escape code for black
 */
#define ANSI_COLOR_BLACK        "\x1b[0;30m"

/**
 * @brief   ANSI color escape code for bold black
 */
#define ANSI_COLOR_BLACK_BOLD   "\x1b[1;30m"

/**
 * @brief   ANSI color escape code for red
 */
#define ANSI_COLOR_RED          "\x1b[0;31m"

/**
 * @brief   ANSI color escape code for bold red
 */
#define ANSI_COLOR_RED_BOLD     "\x1b[1;31m"

/**
 * @brief   ANSI color escape code for green
 */
#define ANSI_COLOR_GREEN        "\x1b[0;32m"

/**
 * @brief   ANSI color escape code for bold green
 */
#define ANSI_COLOR_GREEN_BOLD   "\x1b[1;32m"

/**
 * @brief   ANSI color escape code for yellow
 */
#define ANSI_COLOR_YELLOW       "\x1b[0;33m"

/**
 * @brief   ANSI color escape code for bold yellow
 */
#define ANSI_COLOR_YELLOW_BOLD  "\x1b[1;33m"

/**
 * @brief   ANSI color escape code for blue
 */
#define ANSI_COLOR_BLUE         "\x1b[0;34m"

/**
 * @brief   ANSI color escape code for bold blue
 */
#define ANSI_COLOR_BLUE_BOLD    "\x1b[1;34m"

/**
 * @brief   ANSI color escape code for magenta
 */
#define ANSI_COLOR_MAGENTA      "\x1b[0;35m"

/**
 * @brief   ANSI color escape code for bold magenta
 */
#define ANSI_COLOR_MAGENTA_BOLD "\x1b[1;35m"

/**
 * @brief   ANSI color escape code for cyan
 */
#define ANSI_COLOR_CYAN         "\x1b[0;36m"

/**
 * @brief   ANSI color escape code for bold cyan
 */
#define ANSI_COLOR_CYAN_BOLD    "\x1b[1;36m"

/**
 * @brief   ANSI color escape code for white
 */
#define ANSI_COLOR_WHITE        "\x1b[0;37m"

/**
 * @brief   ANSI color escape code for bold white
 */
#define ANSI_COLOR_WHITE_BOLD   "\x1b[1;37m"

/**
 * @brief   ANSI color escape code for resetting
 */
#define ANSI_COLOR_RESET        "\x1b[0m"

#ifdef __cplusplus
}
#endif
