/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_qmc5883l
 * @{
 *
 * @file
 * @brief       SAUL mapping for the QMC5883L sensor driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */


#include <string.h>

#include "saul.h"
#include "qmc5883l.h"

static int read(const void *dev, phydat_t *res)
{
    int ret = qmc5883l_read((const qmc5883l_t *)dev, res->val);
    if ((ret == QMC5883L_OK) || (ret == QMC5883L_OVERFLOW)) {
        res->unit = UNIT_GS;
        res->scale = -3;
        return 3;
    }
    return -ECANCELED;
}

const saul_driver_t qmc5883l_saul_driver = {
    .read = read,
    .write = saul_notsup,
    .type = SAUL_SENSE_MAG,
};
