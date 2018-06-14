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

#ifdef MODULE_SAUL_ADC

#include "log.h"
#include "saul_reg.h"
#include "saul/periph.h"
#include "adc_params.h"
#include "periph/adc.h"

/**
 * @brief   Define the number of configured sensors
 */
#define SAUL_ADC_NUMOF    (sizeof(saul_adc_params)/sizeof(saul_adc_params[0]))

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

void auto_init_adc(void)
{
    for (unsigned i = 0; i < SAUL_ADC_NUMOF; i++) {
        const saul_adc_params_t *p = &saul_adc_params[i];
        saul_adcs[i] = p;

        LOG_DEBUG("[auto_init_saul] initializing direct ADC #%u\n", i);

        saul_reg_entries[i].dev = &saul_adcs[i];
        saul_reg_entries[i].name = p->name;
        saul_reg_entries[i].driver = &adc_saul_driver;

        /* initialize the ADC line(s) */
        if (p->linelist == 0) {
            /* ignore empty linelist */
            adc_init(p->line);
        }
        else {
            for (unsigned j = 0; j < 8 * sizeof(p->linelist); j++) {
                /* check to see if this context bit is enabled */
                if ((p->linelist >> j) & 0x1) {
                    adc_init(p->line + j);
                }
            }
        }

        /* populate context list */
        saul_reg_entries[i].ctxtlist = p->linelist;

        /* add to registry */
        saul_reg_add(&(saul_reg_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_SAUL_ADC */
