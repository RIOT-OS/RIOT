/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_paa5100je
 * @{
 *
 * @file
 * @brief       Auto initialization for PAA5100JE/PMW3901 Optical Flow Sensors
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "paa5100je.h"
#include "paa5100je_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define PAA5100JE_NUM     ARRAY_SIZE(paa5100je_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static paa5100je_t paa5100je_devs[PAA5100JE_NUM];

/**
 * @brief   Allocate Memory for the SAUL registry entry
 */
static saul_reg_t saul_entries[PAA5100JE_NUM];

/**
 * @brief   Define the number of saul info
 */
#define PAA5100JE_INFO_NUM ARRAY_SIZE(paa5100je_saul_info)

/**
 * @name    Import SAUL endpoint
 * @{
 */
extern const saul_driver_t paa5100je_saul_driver;

void auto_init_paa5100je(void)
{
    assert(PAA5100JE_INFO_NUM == PAA5100JE_NUM);

    for (unsigned int i = 0; i < PAA5100JE_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing paa5100je #%u\n", i);

        if (paa5100je_init(&paa5100je_devs[i], &paa5100je_params[i]) != 0) {
            LOG_ERROR("[auto_init_saul] error initializing paa5100je #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(paa5100je_devs[i]);
        saul_entries[i].name = paa5100je_saul_info[i].name;
        saul_entries[i].driver = &paa5100je_saul_driver;
        saul_reg_add(&saul_entries[i]);
    }
}
