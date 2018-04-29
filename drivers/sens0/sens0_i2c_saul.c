/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_sens0
 * @{
 *
 * @file
 * @brief       SAUL helpers for Sens0
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "phydat.h"
#include "sens0/i2c.h"

/* Quick and dirty C alternative to argument binding without lambdas/closures */

int sens0_i2c_read_output0(const void *dev, phydat_t *res)
{
    return sens0_i2c_read((const sens0_i2c_t *)dev, res, 0);
}

int sens0_i2c_read_output1(const void *dev, phydat_t *res)
{
    return sens0_i2c_read((const sens0_i2c_t *)dev, res, 1);
}

int sens0_i2c_read_output2(const void *dev, phydat_t *res)
{
    return sens0_i2c_read((const sens0_i2c_t *)dev, res, 2);
}
