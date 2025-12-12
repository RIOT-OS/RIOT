/*
 * SPDX-FileCopyrightText: 2024 Otto-von-Guericke-Universität Magdeburg
 * SPDX-FileCopyrightText: 2025 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    config_riotbuild_board  buildinfo/boardinfo.h: board name as macro
 * @ingroup     config_buildinfo
 *
 * ## Usage
 *
 * ```c
 * #include <stdio.h>
 *
 * #include "buildinfo/boardinfo.h"
 *
 * int main(void)
 * {
 *     puts("Hello from the \"" RIOT_BOARD_LITERAL "\" board!");
 * #ifdef BOARD_NATIVE64
 *     puts("You are running on native64!");
 * #endif
 *     return 0;
 * }
 * ```
 *
 * If possible, rather use this instead:
 *
 * ```c
 * #include <stdio.h>
 *
 * // no `#include "buildinfo/boardinfo.h"` needed!
 *
 * int main(void)
 * {
 *     printf("Hello from the \"%s\" board!\n", RIOT_BOARD);
 *     // No `#ifdef BOARD_<NAME>` possible without `#include "buildinfo/boardinfo.h"`
 *     return 0;
 * }
 * ```
 *
 * @{
 */


/**
 * @brief   Name of the board the app is compiled for as string literal
 */
#define RIOT_BOARD_LITERAL  "<BOARD_NAME>"

/**
 * @brief   Same as @ref RIOT_BOARD, but can be useful for conditional
 *          compilation
 *
 * @note    The name of the macro matches the board name. E.g. this would be
 *          generated for the `samr21-xpro` board.
 */
#define BOARD_SAMR21_XPRO   "samr21-xpro"

/** @} */
