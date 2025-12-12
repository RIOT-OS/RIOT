/*
 * SPDX-FileCopyrightText: 2024 Otto-von-Guericke-Universität Magdeburg
 * SPDX-FileCopyrightText: 2025-2026 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    config_riotbuild_app    buildinfo/appinfo.h: application name as macro
 * @ingroup     config_buildinfo
 *
 * ## Usage
 *
 * ```c
 * #include <stdio.h>
 * #include "buildinfo/appinfo.h"
 *
 * int main(void)
 * {
 *     puts("Hello from the \"" RIOT_APPLICATION_LITERAL "\" app!");
 *     return 0;
 * }
 * ```
 *
 * or, if possible, better use:
 *
 * ```c
 * #include <stdio.h>
 * // No `#include "buildinfo/appinfo.h"` needed!
 *
 * int main(void)
 * {
 *     printf("Hello from the \"%s\" app!\n", RIOT_APPLICATION);
 *     return 0;
 * }
 * ```
 *
 * @{
 */

/**
 * @brief   Name of the RIOT application as string literal
 *
 * The string is defined in the applications `Makefile` using the `APPLICATION`
 * variable.
 */
#define RIOT_APPLICATION_LITERAL    "<RIOT_APP_NAME>"

/** @} */
