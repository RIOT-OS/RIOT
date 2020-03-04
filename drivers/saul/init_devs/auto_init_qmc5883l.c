/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of QML5883L magnetic sensors
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifdef MODULE_QMC5883L

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "qmc5883l.h"
#include "qmc5883l_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define QMC5883L_NUM        ARRAY_SIZE(qmc5883l_params)

/**
 * @brief   Define the number of saul info
 */
#define QMC5883L_INFO_NUM   ARRAY_SIZE(qmc5883l_saul_info)

/**
 * @brief   Allocate memory for the device descriptors
 */
static qmc5883l_t qmc5883l_devs[QMC5883L_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[QMC5883L_NUM];

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t qmc5883l_saul_driver;


void auto_init_qmc5883l(void)
{
    assert(QMC5883L_NUM == QMC5883L_INFO_NUM);

    for (unsigned int i = 0; i < QMC5883L_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing qmc5883l #%u\n", i);

        if (qmc5883l_init(&qmc5883l_devs[i], &qmc5883l_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing qmc5883l #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(qmc5883l_devs[i]);
        saul_entries[i].name = qmc5883l_saul_info[i].name;
        saul_entries[i].driver = &qmc5883l_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_QMC5883L */
