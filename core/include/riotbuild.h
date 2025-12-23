/*
 * SPDX-FileCopyrightText: 2024 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    config_riotbuild    riotbuild.h: Macros injected into every compilation unit
 * @ingroup     config
 *
 * @note    The RIOT build system injects this header into every compilation
 *          unit with `CFLAGS += -include riotbuild.h`. Some of the macros
 *          documented here are dynamic and actually provided as `CFLAGS
 *          += -D<MACRO_NAME>=<MACRO_VALUE>` and guarded behind and `#ifdef
 *          DOXYGEN`; but from the user point of view this should be an
 *          implementation detail.
 *
 * @{
 */

#ifdef DOXYGEN
/**
 * @brief   Size of the RAM in Bytes
 *
 * @warning Not every CPU family provides this, it will be undefined if
 *          not provided
 */
#define CPU_RAM_SIZE        /* RAM Size in Bytes */
#endif /* DOXYGEN */

/**
 * @brief   Mark a preprocessor macro as deprecated by including this
 *          macro in the definition
 *
 * Usage:
 * ``` C
 * /// @deprecated, use @ref BAR instead of FOO
 * #define FOO          MACRO_DEPRECATED BAR
 * ```
 */
#if defined(DOXYGEN)
#  define MACRO_DEPRECATED    /* implementation */
#elif defined(__GNUC__) || defined(__clang__)
#  define MACRO_DEPRECATED _Pragma("GCC warning \"Code is using a deprecated macro\"")
#else
#  define MACRO_DEPRECATED
#endif

/**
 * @brief   Name of the MCU the app is compiled for as string literal
 *
 * @deprecated  Use @ref RIOT_CPU instead. This will be removed soonest in
 *              release 2025.04
 *
 * This has been renamed to `RIOT_CPU` for consistency. Even though MCU would
 * technically be the better name, CPU is used every else in the source code
 * and folder structure.
 */
#define RIOT_MCU        MACRO_DEPRECATED RIOT_CPU

/** @} */
