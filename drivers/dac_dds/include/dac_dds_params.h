/*
 * Copyright (C) 2020 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_dac
 * @{
 *
 * @file
 * @brief       Default configuration for the DAC DDS driver
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 */

#ifndef DAC_DDS_PARAMS_H
#define DAC_DDS_PARAMS_H

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
#endif /* DAC_DDS_PARAMS_H */
