/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of ADC lines directly mapped to SAUL reg
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "log.h"
#include "saul_reg.h"
#include "saul/periph.h"
#include "adc_params.h"
#include "periph/adc.h"

/**
 * @brief   Define the number of configured sensors
 */
#define SAUL_ADC_NUMOF    ARRAY_SIZE(saul_adc_params)

/**
 * @brief   Allocate memory for pointers to the ADC parameter structs
 *
 * We use this extra level of indirection to be able to keep the saul_adc_params
 * array const and residing in ROM.
 */
static const saul_adc_params_t *saul_adcs[SAUL_ADC_NUMOF];

/**
 * @brief   Memory for the registry entries
 */
static saul_reg_t saul_reg_entries[SAUL_ADC_NUMOF];

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t adc_saul_driver;

void auto_init_saul_adc(void)
{
    for (unsigned i = 0; i < SAUL_ADC_NUMOF; i++) {
        const saul_adc_params_t *p = &saul_adc_params[i];
        saul_adcs[i] = p;

        LOG_DEBUG("[auto_init_saul] initializing direct ADC #%u\n", i);

        saul_reg_entries[i].dev = &saul_adcs[i];
        saul_reg_entries[i].name = p->name;
        saul_reg_entries[i].driver = &adc_saul_driver;
        /* initialize the ADC line */
        adc_init(p->line);
        /* add to registry */
        saul_reg_add(&(saul_reg_entries[i]));
    }
}
