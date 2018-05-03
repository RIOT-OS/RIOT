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
 * @}
 */

#include <stddef.h> /* for NULL */
#include "sens0.h"
#include "sens0/specs/fxas21002c.h"
#ifdef MODULE_SAUL
#include "saul.h"
#endif

static const sens0_reg_t sens0_fxas21002c_reset_regs[] = {
    { .reg = 0x13u, .value = 0x40u, /* CTRL_REG1, perform reset, will auto clear */ },
};

static const sens0_reg_t sens0_fxas21002c_init_regs[] = {
    { .reg = 0x0du, .value = 0x00u, /* CTRL_REG0, full scale +/- 2000 dps */ },
    { .reg = 0x13u, .value = 0x0cu, /* CTRL_REG1, 100 Hz ODR */ },
};

static const sens0_output_t sens0_fxas21002c_outputs[] = {
    {
        .flags = SENS0_BIG_ENDIAN | SENS0_SIGNED,
        .nelem = 3,
        .elem_size = 2,
        .reg = 0x01u,
        .conv_func = NULL,
        .conv_frac_num = 625, /* 0.0625 DPS per LSB */
        .conv_frac_den = 1000,
        .unit = UNIT_DPS,
        .scale = -1,
    },
};

const sens0_specs_t sens0_specs_fxas21002c = {
    .id                 = { .reg = 0x0cu, .mask = 0xffu, .value = 0xd7u, },
    .reset_regs_nelem   = sizeof(sens0_fxas21002c_reset_regs) / sizeof(sens0_fxas21002c_reset_regs[0]),
    .reset_regs         = sens0_fxas21002c_reset_regs,
    .reset_delay        = 1000u, /* unknown timing, arbitrary delay */
    .init_regs_nelem    = sizeof(sens0_fxas21002c_init_regs) / sizeof(sens0_fxas21002c_init_regs[0]),
    .init_regs          = sens0_fxas21002c_init_regs,
    .measurement_req    = { .reg = 0, .mask = 0, .value = 0, }, /* No req needed */
    .measurement_delay  = 0, /* No delay needed */
    .outputs_nelem      = sizeof(sens0_fxas21002c_outputs) / sizeof(sens0_fxas21002c_outputs[0]),
    .outputs            = sens0_fxas21002c_outputs,
    .power_reg          = 0x13u, /* CTRL_REG1 */
    .power_mask         = 0x03u, /* ACTIVE + READY bit */
    .power_value_active = 0x03u, /* ACTIVE mode */
    .power_value_standby = 0x00u, /* STANDBY mode */
};

#ifdef MODULE_SAUL
const saul_driver_t sens0_saul_drv_fxas21002c_i2c = {
    .read = sens0_i2c_read_output0,
    .write = saul_notsup,
    .type = SAUL_SENSE_GYRO,
};
#endif /* MODULE_SAUL */
