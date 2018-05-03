/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sens0_specs
 * @brief       NXP FXAS21002C gyroscope
 *
 * @{
 * @file
 * @brief       Sens0 specifications for NXP FXAS21002C gyroscope
 *
 * @see Data sheet https://www.nxp.com/docs/en/data-sheet/FXAS21002.pdf
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef SENS0_SPECS_FXAS21002C_H
#define SENS0_SPECS_FXAS21002C_H

#include "sens0.h"
#include "saul.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Sens0 specification for NXP FXAS21002C gyroscope
 */
extern const sens0_specs_t sens0_specs_fxas21002c;
/**
 * @brief   SAUL driver declaration for NXP FXAS21002C gyroscope over I2C
 */
extern const saul_driver_t sens0_saul_drv_fxas21002c_i2c;

#ifdef __cplusplus
}
#endif

#endif /* SENS0_SPECS_FXAS21002C_H */

/** @} */
