/*
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of mapping battery voltage to SAUL
 *
 * @author      Martine S. Lenders <martine.lenders@tu-dresden.de>
 *
 * @}
 */

#include "log.h"
#include "saul_reg.h"
#include "saul/bat_voltage.h"
#include "bat_voltage_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define SAUL_BAT_VOLTAGE_NUMOF  ARRAY_SIZE(saul_bat_voltage_params)

/**
 * @brief   Allocate memory for pointers to the BAT voltage parameter structs
 *
 * We use this extra level of indirection to be able to keep the saul_bat_voltage_params
 * array const and residing in ROM.
 */
static const saul_bat_voltage_params_t *saul_bat_voltages[SAUL_BAT_VOLTAGE_NUMOF];

/**
 * @brief   Memory for the registry entries
 */
static saul_reg_t saul_reg_entries[SAUL_BAT_VOLTAGE_NUMOF];

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t bat_voltage_saul_driver;

void auto_init_saul_bat_voltage(void)
{
    for (unsigned i = 0; i < SAUL_BAT_VOLTAGE_NUMOF; i++) {
        const saul_bat_voltage_params_t *p = &saul_bat_voltage_params[i];
        saul_bat_voltages[i] = p;

        printf("[auto_init_saul] initializing BAT voltage #%u\n", i);

        saul_reg_entries[i].dev = &saul_bat_voltages[i];
        saul_reg_entries[i].name = p->name;
        saul_reg_entries[i].driver = &bat_voltage_saul_driver;
        /* initialize the ADC line */
        adc_init(p->line);
        /* add to registry */
        saul_reg_add(&(saul_reg_entries[i]));
    }
}
