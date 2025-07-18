/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of internal temperature sensor directly mapped to SAUL reg
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "log.h"
#include "saul_reg.h"
#include "saul/periph.h"

/**
 * @brief   Memory for the registry entries
 */
static saul_reg_t saul_reg_entry;

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t nrf_temperature_saul_driver;

/**
 * @brief   Reference the information for saul registry
 */
extern saul_reg_info_t nrf_temperature_saul_info;

void auto_init_nrf_temperature(void)
{
    saul_reg_entry.dev = NULL;
    saul_reg_entry.name = nrf_temperature_saul_info.name;
    saul_reg_entry.driver = &nrf_temperature_saul_driver;
    /* add to registry */
    saul_reg_add(&(saul_reg_entry));
}
