/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of PWM pins directly mapped to SAUL reg
 *
 * @author      Francisco Acosta <francisco. acosta@inria.fr>
 *
 * @}
 */

#ifdef MODULE_SAUL_PWM

#include "log.h"
#include "saul_reg.h"
#include "saul/periph.h"
#include "pwm_params.h"
#include "periph/pwm.h"

/**
 * @brief   Define the number of configured sensors
 */
#define SAUL_PWM_NUMOF    (sizeof(saul_pwm_params)/sizeof(saul_pwm_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static const saul_pwm_params_t *saul_pwms[SAUL_PWM_NUMOF];

/**
 * @brief   Memory for the registry entries
 */
static saul_reg_t saul_reg_entries[SAUL_PWM_NUMOF];

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t pwm_saul_driver;

void auto_init_pwm(void)
{
    for (unsigned int i = 0; i < SAUL_PWM_NUMOF; i++) {
        const saul_pwm_params_t *p = &saul_pwm_params[i];

        LOG_DEBUG("[auto_init_saul] initializing PWM #%u\n", i);

        saul_pwms[i] = p;
        saul_reg_entries[i].dev = &(saul_pwms[i]);
        saul_reg_entries[i].name = p->name;
        saul_reg_entries[i].driver = &pwm_saul_driver;

        /* initialize the PWM pin */
        pwm_init(p->line, p->mode, p->freq, p->res);
        /* add to registry */
        saul_reg_add(&(saul_reg_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_SAUL_PWM */
