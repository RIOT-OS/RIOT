/*
 * SPDX-FileCopyrightText: 2024 Otto-von-Guericke-Universität Magdeburg
 * SPDX-FileCopyrightText: 2025 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    config_riotbuild_cpu    buildinfo/cpuinfo.h: cpu name as macro
 * @ingroup     config_buildinfo
 *
 * ## Usage
 *
 * ```c
 * #include <stdio.h>
 *
 * #include "buildinfo/cpuinfo.h"
 *
 * int main(void)
 * {
 *     puts("Hello from the \"" RIOT_CPU_LITERAL "\" cpu!");
 * #ifdef BOARD_SAMD5X
 *     puts("You are running on an SAMD5X CPU!");
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
 * // no `#include "buildinfo/cpuinfo.h"` needed!
 *
 * int main(void)
 * {
 *     printf("Hello from the \"%s\" cpu!\n", RIOT_CPU);
 *     // No `#ifdef BOARD_SAMD5X` possible without `#include "buildinfo/cpuinfo.h"`
 *     puts("You are running on an SAMD5X CPU!");
 *     return 0;
 * }
 * ```
 *
 * @{
 */

/**
 * @brief   Name of the MCU the app is compiled for as string literal
 *
 * This is the name of the MCU family in terms of RIOT's peripheral drivers,
 * or in other words, the folder name in the `cpu` folder in RIOT's repo root
 * used.
 */
#define RIOT_CPU_LITERAL    "<CPU_FOLDER_NAME>"

/**
 * @brief   Same as @ref RIOT_CPU, but useful for conditional compilation
 *
 * @note    The name of the macro contains the CPU name, e.g. this define
 *          is provided if and only if building for an SAMD5X CPU
 */
#define CPU_SAMD5X          "samd5x"

/** @} */
