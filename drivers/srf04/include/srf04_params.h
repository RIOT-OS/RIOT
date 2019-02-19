/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_srf04
 *
 * @{
 * @file
 * @brief       Default configuration for srf04 devices
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 */

#ifndef SRF04_PARAMS_H
#define SRF04_PARAMS_H

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
#define SRF04_PARAM_TRIGGER     GPIO_PIN(0,13)
#endif
#ifndef SRF04_PARAM_ECHO
#define SRF04_PARAM_ECHO        GPIO_PIN(0,14)
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
#define SRF04_NUMOF     (sizeof(srf04_params) / sizeof(srf04_params[0]))

#ifdef __cplusplus
}
#endif

#endif /* SRF04_PARAMS_H */
/** @} */
