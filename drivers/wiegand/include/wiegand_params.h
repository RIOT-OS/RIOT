/*
 * Copyright (C) 2020 Hackerspace San Salvador
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_wiegand
 *
 * @{
 * @file
 * @brief       Default configuration for WIEGAND devices
 *
 * @author      Mario Gomez <mario.gomez@teubi.co>
 */

#ifndef WIEGAND_PARAMS_H
#define WIEGAND_PARAMS_H

#include "board.h"
#include "wiegand.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIEGAND_D0
#define WIEGAND_D0        GPIO_PIN(0, 1)
#endif

#ifndef WIEGAND_D1
#define WIEGAND_D1        GPIO_PIN(0, 2)
#endif

#ifndef WIEGAND_FLANK
#define WIEGAND_FLANK     GPIO_RISING
#endif

#ifndef WIEGAND_PARAMS
#define WIEGAND_PARAMS     { .d0 = WIEGAND_D0, \
                             .d1 = WIEGAND_D1, \
                             .flank = WIEGAND_FLANK }
#endif

#ifndef WIEGAND_SAUL_INFO
#define WIEGAND_SAUL_INFO    { .name = "wiegand driver" }
#endif

/**
 * @brief   WIEGAND configuration
 */
static const wiegand_params_t wiegand_params[] =
{
    WIEGAND_PARAMS,
};

/**
 * @brief   Additional meta information to keep in the SAUL registry
 */
static const saul_reg_info_t wiegand_saul_info[] =
{
    WIEGAND_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* WIEGAND_PARAMS_H */
/** @} */
