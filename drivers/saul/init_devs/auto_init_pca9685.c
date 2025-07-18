/*
 * SPDX-FileCopyrightText: 2017 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup     drivers_pca9685
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of PCA9685 I2C PWM controller
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include "assert.h"
#include "log.h"

#include "saul_reg.h"
#include "saul/periph.h"

#include "pca9685.h"
#include "pca9685_params.h"

/**
 * @brief   Number of configured PCA9685 DAC devices
 */
#define PCA9685_NUM    (ARRAY_SIZE(pca9685_params))

/**
 * @brief   Number of configured SAUL PCA9685 DAC channels
 */
#define PCA9685_SAUL_DAC_NUMOF (ARRAY_SIZE(pca9685_saul_pwm_params))

/**
 * @brief   Number of saul info
 */
#define PCA9685_INFO_NUM    (ARRAY_SIZE(pca9685_saul_info))

/**
 * @brief   Allocate the memory for the PCA9685 DAC device descriptors
 */
pca9685_t pca9685_devs[PCA9685_NUM];

/**
 * @brief   Allocate the memory for PCA9685 DAC SAUL registry entries
 */
static saul_reg_t pca9685_saul_reg_entries[PCA9685_SAUL_DAC_NUMOF];

/**
 * @brief   Reference the PCA9685 DAC input mode driver struct
 */
extern saul_driver_t pca9685_pwm_in_saul_driver;

/**
 * @brief   Reference to the PCA9685 DAC output mode driver struct
 */
extern saul_driver_t pca9685_pwm_saul_driver;

void auto_init_pca9685(void)
{
    for (unsigned int i = 0; i < PCA9685_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing PCA9685 PWM dev #%u\n", i);
        pca9685_init(&pca9685_devs[i], &pca9685_params[i]);
        pca9685_pwm_init(&pca9685_devs[i], pca9685_params[i].mode,
                                           pca9685_params[i].freq,
                                           pca9685_params[i].res);
    }

    for (unsigned int i = 0; i < PCA9685_SAUL_DAC_NUMOF; i++) {
        const pca9685_saul_pwm_params_t *p = &pca9685_saul_pwm_params[i];

        LOG_DEBUG("[auto_init_saul] initializing PCA9685 channel #%u\n", i);

        /* check the PCA9685 device index */
        assert(p->dev < PCA9685_NUM);

        pca9685_saul_reg_entries[i].dev = (void *)p;
        pca9685_saul_reg_entries[i].name = p->name;
        pca9685_saul_reg_entries[i].driver = &pca9685_pwm_saul_driver;

        /* initialize the PCA9685 pin */
        pca9685_pwm_set(&pca9685_devs[p->dev], p->channel, p->initial);

        /* add to registry */
        saul_reg_add(&(pca9685_saul_reg_entries[i]));
    }
}
