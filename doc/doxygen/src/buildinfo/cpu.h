/*
 * SPDX-FileCopyrightText: 2024 Otto-von-Guericke-Universität Magdeburg
 * SPDX-FileCopyrightText: 2025 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    config_riotbuild_cpu    buildinfo/cpu.h: cpu name as macro
 * @ingroup     config_buildinfo
 *
 * # Usage
 *
 * ```C
 * #include "buildinfo/cpu.h"
 *
 * int main(void)
 * {
 *     puts("Hello from the \"" RIOT_CPU "\" cpu!");
 * #ifdef BOARD_STM32
 *     puts("You are running on an STM32 CPU!");
 * #endif
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
#define RIOT_CPU            "<CPU_FOLDER_NAME>"

/**
 * @brief   Same as @ref RIOT_CPU, but useful for conditional compilation
 *
 * @note    The name of the macro contains the CPU name, e.g. this define
 *          is provided if and only if building for an STM32 CPU
 */
#define CPU_STM             "stm32"

/** @} */
