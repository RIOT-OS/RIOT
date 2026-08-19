/*
 * SPDX-FileCopyrightText: 2024 Otto-von-Guericke-Universität Magdeburg
 * SPDX-FileCopyrightText: 2025 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    config_riotbuild_riotver    buildinfo/riotver.h: RIOT version as macro
 * @ingroup     config_buildinfo
 *
 * ## Usage
 *
 * ```c
 * #include <stdio.h>
 *
 * #include "buildinfo/riotver.h" // to query the used RIOT version
 * #include "riot_version.h" // to compare the version
 *
 * int main(void)
 * {
 *     puts("You are running RIOT version \"" RIOT_VERSION_LITERAL "\" on your board.");
 * #if RIOT_VERSION_CODE < RIOT_VERSION_NUM(2025, 7, 0, 0)
 *     puts("Oh, looks like your running an old release! Time to upgrade :-)");
 * #endif
 *     return 0;
 * }
 * ```
 *
 * or, if possible, better use:
 *
 * ```c
 * #include <stdio.h>
 *
 * // no `#include "buildinfo/riotver.h"` needed
 *
 * int main(void)
 * {
 *     printf("You are running RIOT version \"%s\" on your board.\n", RIOT_VERSION);
 *     // No access to `RIOT_VERSION_CODE` without
 *     // `#include "buildinfo/riotver.h"`
 *     return 0;
 * }
 * ```
 *
 *
 * @{
 */

/**
 * @brief   The used RIOT version as human readable string literal, e.g., for
 *          printing to stdio or writing to a log.
 */
#define RIOT_VERSION "<YEAR_OF_RELEASE>.<MONTH_OF_RELEASE>-<POSTFIX>"

/**
 * @brief   The used RIOT version as machine readable number, e.g., for testing
 *          whether new APIs are available.
 *
 * @see     RIOT_VERSION_NUM
 */
#define RIOT_VERSION_CODE   RIOT_VERSION_NUM(<YEAR>,<MONTH>,<PATCH>,<EXTRA>)

/** @} */
