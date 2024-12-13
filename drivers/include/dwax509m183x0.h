/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_dwax509m183x0 dwax509m183x0
 * @ingroup     drivers_sensors
 * @brief       Inductive sensor for distance measurements
 *
 * @{
 *
 * @file
 *
 * @author      Jan Thies <jan.thies@haw-hamburg.de>
 */

#ifndef DWAX509M183X0_H
#define DWAX509M183X0_H

/* Add header includes here */
#include "periph/adc.h"

#define DWAX509M183X0_MAX_VOLTAGE_OUTPUT 10

#ifdef __cplusplus
extern "C" {
#endif

/* Declare the API of the driver */

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    adc_t adc_line;
    adc_res_t resolution;
} dwax509m183x0_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef dwax509m183x0_params_t dwax509m183x0_t;

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 *
 * @return                  0 on success
 */
int dwax509m183x0_init(dwax509m183x0_t *dev, const dwax509m183x0_params_t *params);
int dwax509m183x0_distance_um(dwax509m183x0_t* dev);

#ifdef __cplusplus
}
#endif

#endif /* DWAX509M183X0_H */
/** @} */
