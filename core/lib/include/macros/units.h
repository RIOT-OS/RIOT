/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     core_macros
 * @{
 *
 * @file
 * @brief       Unit helper macros
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   A macro to return the bytes in x KiB
 */
#define KiB(x) ((unsigned long)(x) << 10)

/**
 * @brief   A macro to return the bytes in x MiB
 */
#define MiB(x) (KiB(x) << 10)

/**
 * @brief   A macro to return the bytes in x GiB
 */
#define GiB(x) ((unsigned long long)MiB(x) << 10)

/**
 * @brief   A macro to return the Hz in x kHz
 */
#define KHZ(x)    ((x) * 1000UL)

/**
 * @brief   A macro to return the Hz in x MHz
 */
#define MHZ(x) (KHZ(x) * 1000UL)

/**
 * @brief   A macro to return the Hz in x GHz
 */
#define GHZ(x) (MHZ(x) * 1000ULL)

#ifdef __cplusplus
}
#endif

/** @} */
