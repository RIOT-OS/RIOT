/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_qma6100p
 * @{
 *
 * @file
 * @brief       QMA6100P adaption to the RIOT actuator/sensor interface
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 * @author      Léandre Le Duc <leandre.leduc38@gmail.com>
 *
 * @}
 */

#include <string.h>

#include "qma6100p.h"
#include "saul.h"

static int saul_qma6100p_read(const void *dev, phydat_t *data)
{
    int res;

    memset(data, 0, sizeof(*data));
    const qma6100p_t *mydev = (const qma6100p_t *)dev;
    res = qma6100p_read(mydev, (qma6100p_data_t *)(&data->val));
    if (res < 0) {
        return res;
    }

    data->unit = UNIT_G_FORCE;
    data->scale = -6;
    if (res == QMA6100P_NO_NEW_DATA) {
        return 0;
    }

    return 3;
}

const saul_driver_t qma6100p_t_saul_driver = {
    .read = saul_qma6100p_read,
    .write = NULL,
    .type = SAUL_SENSE_ACCEL,
};
