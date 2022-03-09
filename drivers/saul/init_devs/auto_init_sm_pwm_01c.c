/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 * @file
 * @brief       Auto initialization for SM_PWM_01C dust sensor
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "sm_pwm_01c_params.h"
#include "sm_pwm_01c.h"

/**
 * @brief   Allocate memory for the device descriptors
 */
static sm_pwm_01c_t sm_pwm_01c_devs[SM_PWM_01C_NUMOF];

#if IS_ACTIVE(MODULE_SAUL)
/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[SM_PWM_01C_NUMOF * 2];

/**
 * @brief   Define the number of saul info
 */
#define SM_PWM_01C_INFO_NUM ARRAY_SIZE(sm_pwm_01c_saul_info)

/**
 * @name    Import SAUL endpoints
 * @{
 */
extern const saul_driver_t sm_pwm_01c_saul_driver_mc_pm_2p5;
extern const saul_driver_t sm_pwm_01c_saul_driver_mc_pm_10;
/** @} */
#endif

void auto_init_sm_pwm_01c(void)
{
#if IS_ACTIVE(MODULE_SAUL)
    assert(SM_PWM_01C_INFO_NUM == SM_PWM_01C_NUMOF);
#endif

    for (unsigned int i = 0; i < SM_PWM_01C_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing sm_pwm_01c #%u\n", i);

        if (sm_pwm_01c_init(&sm_pwm_01c_devs[i], &sm_pwm_01c_params[i])) {
            LOG_ERROR("[auto_init_saul] error initializing sm_pwm_01c #%u\n",
                      i);
            continue;
        }
    sm_pwm_01c_start(&sm_pwm_01c_devs[i]);
#if IS_ACTIVE(MODULE_SAUL)
    saul_entries[(i * 2)].dev = &(sm_pwm_01c_devs[i]);
    saul_entries[(i * 2)].name = sm_pwm_01c_saul_info[i].name;
    saul_entries[(i * 2)].driver = &sm_pwm_01c_saul_driver_mc_pm_2p5;
    saul_entries[(i * 2) + 1].dev = &(sm_pwm_01c_devs[i]);
    saul_entries[(i * 2) + 1].name = sm_pwm_01c_saul_info[i].name;
    saul_entries[(i * 2) + 1].driver = &sm_pwm_01c_saul_driver_mc_pm_10;
    saul_reg_add(&(saul_entries[(i * 2)]));
    saul_reg_add(&(saul_entries[(i * 2) + 1]));
#endif
    }
}
