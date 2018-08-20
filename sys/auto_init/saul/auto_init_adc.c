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
 * @brief   Memory for the registry entries
 */
static saul_reg_t saul_reg_entries[SAUL_ADC_NUMOF];

/**
 * @name    driver struct references
 * @{
 */
extern saul_driver_t adc_saul_driver;
#if MODULE_ANALOG_UTIL
extern saul_driver_t adc_saul_temp_driver;
extern saul_driver_t adc_saul_analog_driver;
#endif /* MODULE_ANALOG_UTIL */
/** @} */

void auto_init_adc(void)
{
    for (unsigned i = 0; i < SAUL_ADC_NUMOF; i++) {
        const saul_adc_params_t *p = &saul_adc_params[i];

        LOG_DEBUG("[auto_init_saul] initializing direct ADC #%u\n", i);

        /* discarding const qualifier of target type, the driver must only use
         * it as if it is const though. */
        saul_reg_entries[i].dev = (void *)p;
        saul_reg_entries[i].name = p->name;
#if MODULE_ANALOG_UTIL
        if (p->val_min != p->val_max) {
            /* Apply scaling */
            switch (p->unit) {
                /* Pick the correct sensor type based on the unit */
                case UNIT_TEMP_C:
                case UNIT_TEMP_F:
                case UNIT_TEMP_K:
                    saul_reg_entries[i].driver = &adc_saul_temp_driver;
                    break;
                case UNIT_A:
                case UNIT_V:
                default:
                    /* Fall back: use sensor type SAUL_SENSE_ANALOG */
                    saul_reg_entries[i].driver = &adc_saul_analog_driver;
                    break;
            }
        }
        else {
            /* Unscaled raw values */
            saul_reg_entries[i].driver = &adc_saul_driver;
        }
#else /* MODULE_ANALOG_UTIL */
        saul_reg_entries[i].driver = &adc_saul_driver;
#endif /* MODULE_ANALOG_UTIL */
        /* initialize the ADC line */
        adc_init(p->line);
        /* add to registry */
        saul_reg_add(&(saul_reg_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_SAUL_ADC */
