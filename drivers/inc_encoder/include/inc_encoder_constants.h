/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_inc_encoder
 * @{
 *
 * @file
 * @brief       Constants used in the incremental rotary encoder driver
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Scaling factor to apply to adjust for the gear reduction ratio being in tenths.
 */
#define GEAR_RED_RATIO_SCALE 10

/**
 * @brief Scaling factor to convert revolutions per minute to millirevolutions per minute.
 */
#define UNIT_SCALE 1000

#ifdef __cplusplus
}
#endif
/** @} */
