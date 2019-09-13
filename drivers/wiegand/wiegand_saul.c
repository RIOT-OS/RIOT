/*
 * Copyright (C) 2920 Hackerspace San Salvador
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_wiegand
 * @{
 *
 * @file
 * @brief       SAUL adaption for Wiegand Protocol.
 *
 * @author      Mario Gomez <mario.gomez@teubi.co>
 *
 * @}
 */

#include "saul.h"

#include "wiegand.h"

static int read_wiegand_code(const void *dev, phydat_t *res)
{
    res->val[0] = wiegand_get_code((wiegand_t *)dev);
    res->unit = UNIT_NONE;
    res->scale = 1;

    return 1;
}

static int read_wiegand_type(const void *dev, phydat_t *res)
{
    res->val[0] = wiegand_get_type((wiegand_t *)dev);
    res->unit = UNIT_NONE;
    res->scale = 1;

    return 1;
}

const saul_driver_t wiegand_code_saul_driver = {
    .read = read_wiegand_code,
    .write = saul_notsup,
    .type = SAUL_SENSE_ANY,
};

const saul_driver_t wiegand_type_humidity_saul_driver = {
    .read = read_wiegand_type,
    .write = saul_notsup,
    .type = SAUL_SENSE_ANY,
};
