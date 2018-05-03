/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sens0_specs
 * @brief       NXP FXOS8700CQ accelerometer + magnetometer 6DOF sensor
 *
 * @{
 * @file
 * @brief       Sens0 specifications for NXP FXOS8700CQ
 *
 * @see Data sheet https://www.nxp.com/docs/en/data-sheet/FXOS8700CQ.pdf
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef SENS0_SPECS_FXOS8700CQ_H
#define SENS0_SPECS_FXOS8700CQ_H

#include "sens0.h"
#include "saul.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Sens0 specification for NXP FXOS8700CQ accelerometer + magnetometer 6DOF sensor
 */
extern const sens0_specs_t sens0_specs_fxos8700cq;
/**
 * @brief   SAUL driver declaration for NXP FXOS8700CQ accelerometer over I2C
 */
extern const saul_driver_t sens0_saul_drv_fxos8700cq_i2c_acc;
/**
 * @brief   SAUL driver declaration for NXP FXOS8700CQ magnetometer over I2C
 */
extern const saul_driver_t sens0_saul_drv_fxos8700cq_i2c_mag;

#ifdef __cplusplus
}
#endif

#endif /* SENS0_SPECS_FXOS8700CQ_H */

/** @} */
