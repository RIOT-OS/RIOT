/*
 * SPDX-FileCopyrightText: 2020 Beuth Hochschule für Technik Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_periph_dac
 * @{
 *
 * @file
 * @brief       Default configuration for the DAC DDS driver
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 */

#include "board.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the DAC DDS driver
 * @{
 */
#ifndef DAC_DDS_PARAM_DAC
#define DAC_DDS_PARAM_DAC       DAC_LINE(0)
#endif
#ifndef DAC_DDS_PARAM_TIMER
#define DAC_DDS_PARAM_TIMER     (TIMER_NUMOF - 1)
#endif
#ifndef DAC_DDS_PARAM_TIMER_HZ
#define DAC_DDS_PARAM_TIMER_HZ  MHZ(1)
#endif

#ifndef DAC_DDS_PARAMS
#define DAC_DDS_PARAMS          { .dac      = DAC_DDS_PARAM_DAC,      \
                                  .timer    = DAC_DDS_PARAM_TIMER,    \
                                  .timer_hz = DAC_DDS_PARAM_TIMER_HZ, \
                                }
#endif
/** @} */

/**
 * @brief   DAC DDS configuration
 */
static const dac_dds_params_t dac_dds_params[] =
{
    DAC_DDS_PARAMS
};

/**
 * @brief   DAC DDS instances
 */
#define DAC_DDS_NUMOF           ARRAY_SIZE(dac_dds_params)

#ifdef __cplusplus
}
#endif

/** @} */
