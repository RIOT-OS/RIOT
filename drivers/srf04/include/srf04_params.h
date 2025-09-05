/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_srf04
 *
 * @{
 * @file
 * @brief       Default configuration for srf04 devices
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "srf04.h"

/**
 * @name   Default configuration parameters for SRF04 device
 * @{
 */
#ifndef SRF04_PARAM_TRIGGER
#define SRF04_PARAM_TRIGGER     GPIO_PIN(0, 13)
#endif
#ifndef SRF04_PARAM_ECHO
#define SRF04_PARAM_ECHO        GPIO_PIN(0, 14)
#endif

#ifndef SRF04_PARAMS
#define SRF04_PARAMS                            \
    {   .trigger        = SRF04_PARAM_TRIGGER,  \
        .echo           = SRF04_PARAM_ECHO,     \
    }
#endif
/** @} */

/**
 * @brief   SRF04 configuration
 */
static const srf04_params_t srf04_params[] = {
    SRF04_PARAMS
};

/**
 * @brief   Number of SRF04 devices
 */
#define SRF04_NUMOF     ARRAY_SIZE(srf04_params)

#ifdef __cplusplus
}
#endif

/** @} */
