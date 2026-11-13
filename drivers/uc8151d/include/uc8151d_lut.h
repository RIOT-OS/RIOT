/*
 * Copyright (C) 2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_uc8151d
 * @{
 *
 * @file
 * @brief       LUT tables for custom refresh patterns
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef UC8151D_LUT_H
#define UC8151D_LUT_H

#include "uc8151d_constants.h"
#include "uc8151d.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Struct definition for a single LUT phase
 */
typedef struct __attribute__((packed)) {
    uc8151d_voltage_pattern_t pattern; /**< Voltage pattern used for the 4 sections */
    uint8_t num_frames_0; /**< Number of frames in section 0 */
    uint8_t num_frames_1; /**< Number of frames in section 1 */
    uint8_t num_frames_2; /**< Number of frames in section 2 */
    uint8_t num_frames_3; /**< Number of frames in section 3 */
    uint8_t repeat;       /**< Number of times the sequence is applied to the display */
} uc8151d_lut_phase_t;

/**
 * @brief Full LUT in BW mode content
 *
 * A full zero phase is skipped
 */
typedef struct __attribute__((packed)) {
    uc8151d_lut_phase_t phase[6]; /**< The phases used in the display */
} uc8151d_lut_t;

/**
 * @brief LUT pattern for fast partial updates
 * @{
 */
#define UC8151D_LUT_PARTIAL_UPDATE_TIME   20
/**
 * @brief VCOM table
 */
static const uc8151d_lut_phase_t uc8151d_lut_partial_update_vcom = {
    .pattern = UC8151D_VOLTAGE_PATTERN_0000,
    .num_frames_0 = UC8151D_LUT_PARTIAL_UPDATE_TIME,
    .num_frames_1 = 0,
    .num_frames_2 = 0,
    .num_frames_3 = 0,
    .repeat = 1,
};

/**
 * @brief pattern for pixels to black, pull them high for a while
 */
static const uc8151d_lut_phase_t uc8151d_lut_partial_update_w2b = {
    .pattern = UC8151D_VOLTAGE_PATTERN_H000,
    .num_frames_0 = UC8151D_LUT_PARTIAL_UPDATE_TIME,
    .num_frames_1 = 0,
    .num_frames_2 = 0,
    .num_frames_3 = 0,
    .repeat = 1,
};

/**
 * @brief pattern for pixels to white, pull them low for a while
 */
static const uc8151d_lut_phase_t uc8151d_lut_partial_update_b2w = {
    .pattern = UC8151D_VOLTAGE_PATTERN_L000,
    .num_frames_0 = UC8151D_LUT_PARTIAL_UPDATE_TIME,
    .num_frames_1 = 0,
    .num_frames_2 = 0,
    .num_frames_3 = 0,
    .repeat = 1,
};
/** @} */

/**
 * @brief LUT pattern for turbo full updates, blinks a few times fast to update
 * @{
 */
/**
 * @brief VCOM table
 */
static const uc8151d_lut_phase_t uc8151d_lut_turbo_vcom[] = {
    {
        .pattern = UC8151D_VOLTAGE_PATTERN_0000,
        .num_frames_0 = 1,
        .num_frames_1 = 1,
        .num_frames_2 = 2,
        .num_frames_3 = 0,
        .repeat = 1,
    },
    {
        .pattern = UC8151D_VOLTAGE_PATTERN_0000,
        .num_frames_0 = 2,
        .num_frames_1 = 2,
        .num_frames_2 = 0,
        .num_frames_3 = 0,
        .repeat = 2,
    },
    {
        .pattern = UC8151D_VOLTAGE_PATTERN_0000,
        .num_frames_0 = 2,
        .num_frames_1 = 2,
        .num_frames_2 = 3,
        .num_frames_3 = 0,
        .repeat = 2,
    },
};

/**
 * @brief pattern for white pixels, pull them to black, blink a few times and pull to white
 */
static const uc8151d_lut_phase_t uc8151d_lut_turbo_b2w[] = {
    {
        .pattern = UC8151D_VOLTAGE_PATTERN_HHH0,
        .num_frames_0 = 1,
        .num_frames_1 = 1,
        .num_frames_2 = 2,
        .num_frames_3 = 0,
        .repeat = 1,
    },
    {
        .pattern = UC8151D_VOLTAGE_PATTERN_HL00,
        .num_frames_0 = 2,
        .num_frames_1 = 2,
        .num_frames_2 = 0,
        .num_frames_3 = 0,
        .repeat = 2,
    },
    {
        .pattern = UC8151D_VOLTAGE_PATTERN_LLL0,
        .num_frames_0 = 2,
        .num_frames_1 = 2,
        .num_frames_2 = 3,
        .num_frames_3 = 0,
        .repeat = 2,
    },
};

/**
 * @brief pattern for black pixels, pull them to white , blink a few times and pull to black
 */
static const uc8151d_lut_phase_t uc8151d_lut_turbo_w2b[] = {
    {
        .pattern = UC8151D_VOLTAGE_PATTERN_LLL0,
        .num_frames_0 = 1,
        .num_frames_1 = 1,
        .num_frames_2 = 2,
        .num_frames_3 = 0,
        .repeat = 1,
    },
    {
        .pattern = UC8151D_VOLTAGE_PATTERN_LH00,
        .num_frames_0 = 2,
        .num_frames_1 = 2,
        .num_frames_2 = 0,
        .num_frames_3 = 0,
        .repeat = 2,
    },
    {
        .pattern = UC8151D_VOLTAGE_PATTERN_HHH0,
        .num_frames_0 = 2,
        .num_frames_1 = 2,
        .num_frames_2 = 3,
        .num_frames_3 = 0,
        .repeat = 2,
    },
};
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* UC8151D_LUT_H */
/** @} */
