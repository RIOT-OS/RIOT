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
 * @}
 */

#include <stddef.h> /* for NULL */
#include "sens0.h"
#include "sens0/specs/fxos8700cq.h"
#ifdef MODULE_SAUL
#include "saul.h"
#endif

static const sens0_reg_t sens0_fxos8700cq_reset_regs[] = {
    { .reg = 0x2bu, .value = 0x40u, /* CTRL_REG2, perform reset, will auto clear */ },
};

/* Hybrid mode (accelerometer + magnetometer) */
static const sens0_reg_t sens0_fxos8700cq_init_regs_hybrid[] = {
    { .reg = 0x2au, .value = 0x10u, /* CTRL_REG1, set 100 Hz hybrid ODR */ },
    { .reg = 0x0eu, .value = 0x02u, /* XYZ_DATA_CFG, +/- 8 g full scale */ },
    { .reg = 0x5bu, .value = 0x1fu, /* M_CTRL_REG1, hybrid mode, OSR=7 */ },
};

static const sens0_output_t sens0_fxos8700cq_outputs[] = {
    {
        /* Accelerometer */
        .flags = SENS0_BIG_ENDIAN | SENS0_SIGNED,
        .nelem = 3,
        .elem_size = 2,
        .reg = 0x01u,
        .conv_func = NULL,
        .conv_frac_num = 976, /* Raw value is 14 bits left adjusted, LSB=0.976 mG */
        .conv_frac_den = 4000, /* divide by 4 to eliminate the rightmost dummy bits */
        .unit = UNIT_G,
        .scale = -3,
    },
    {
        /* Magnetometer */
        .flags = SENS0_BIG_ENDIAN | SENS0_SIGNED,
        .nelem = 3,
        .elem_size = 2,
        .reg = 0x33u,
        .conv_func = NULL,
        .conv_frac_num = 1, /* Raw value is 16 bits signed, LSB=0.1 µT = 0.001 gauss */
        .conv_frac_den = 1, /* no need to scale the raw value */
        .unit = UNIT_GS,
        .scale = -3,
    },
};

const sens0_specs_t sens0_specs_fxos8700cq = {
    .id                 = { .reg = 0x0du, .mask = 0xffu, .value = 0xc7u, },
    .reset_regs_nelem   = sizeof(sens0_fxos8700cq_reset_regs) / sizeof(sens0_fxos8700cq_reset_regs[0]),
    .reset_regs         = sens0_fxos8700cq_reset_regs,
    .reset_delay        = 1000u, /* reset takes 1 ms */
    .init_regs_nelem    = sizeof(sens0_fxos8700cq_init_regs_hybrid) / sizeof(sens0_fxos8700cq_init_regs_hybrid[0]),
    .init_regs          = sens0_fxos8700cq_init_regs_hybrid,
    .measurement_req    = { .reg = 0, .mask = 0, .value = 0, }, /* No req needed */
    .measurement_delay  = 0, /* No delay needed */
    .outputs_nelem      = sizeof(sens0_fxos8700cq_outputs) / sizeof(sens0_fxos8700cq_outputs[0]),
    .outputs            = sens0_fxos8700cq_outputs,
    .power_reg          = 0x2au, /* CTRL_REG1 */
    .power_mask         = 0x01u, /* Active bit */
    .power_value_active = 0x01u,
    .power_value_standby = 0x00u,
};

#ifdef MODULE_SAUL
const saul_driver_t sens0_saul_drv_fxos8700cq_i2c_acc = {
    .read = sens0_i2c_read_output0,
    .write = saul_notsup,
    .type = SAUL_SENSE_ACCEL,
};

const saul_driver_t sens0_saul_drv_fxos8700cq_i2c_mag = {
    .read = sens0_i2c_read_output1,
    .write = saul_notsup,
    .type = SAUL_SENSE_MAG,
};
#endif /* MODULE_SAUL */
