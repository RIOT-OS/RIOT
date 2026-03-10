/*
 * Copyright (C) 2026 Technische Universität Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_qmi8658 QMI8658 IMU
 * @ingroup     drivers_sensors
 * @brief       Device driver for the QMI8658 6D MEMS inertial measurement unit (IMU)
 *
 * @{
 *
 * @file
 *
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 */

/* Add header includes here */

#ifdef __cplusplus
extern "C" {
#endif

/* Declare the API of the driver */

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    /* add initialization params here */
} qmi8658_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    /** Device initialization parameters */
    qmi8658_params_t params;
} qmi8658_t;

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 *
 * @return                  0 on success
 */
int qmi8658_init(qmi8658_t *dev, const qmi8658_params_t *params);

#ifdef __cplusplus
}
#endif

/** @} */
