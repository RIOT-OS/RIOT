/*
 * SPDX-FileCopyrightText: 2024 Otto-von-Guericke-Universität Magdeburg
 * SPDX-FileCopyrightText: 2025 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    config_riotbuild_app    buildinfo/app.h: application name as macro
 * @ingroup     config_buildinfo
 *
 * # Usage
 *
 * ```C
 * #include "buildinfo/app.h"
 *
 * int main(void)
 * {
 *     puts("Hello from the \"" RIOT_APPLICATION "\" app!");
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
#define RIOT_APPLICATION    "<RIOT_APP_NAME>"

/** @} */
