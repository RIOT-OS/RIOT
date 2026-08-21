/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-FileCopyrightText: 2026 Léandre Le Duc <leandre.leduc38@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of the QMA6100P accelerometer
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 * @author      Léandre Le Duc <leandre.leduc38@gmail.com>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "qma6100p.h"
#include "qma6100p_params.h"
#include "saul_reg.h"

/**
 * @brief   Define the number of configured sensors
 */
#define QMA6100P_NUM      ARRAY_SIZE(qma6100p_params)

/**
 * @brief   Define the number of saul info
 */
#define QMA6100P_INFO_NUM ARRAY_SIZE(qma6100p_saul_info)

/**
 * @brief   Allocate memory for the device descriptors
 */
static qma6100p_t qma6100p_devs[QMA6100P_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[QMA6100P_NUM];

/**
 * @name    Reference the driver structs
 * @{
 */
extern saul_driver_t qma6100p_t_saul_driver;
/** @} */

void auto_init_qma6100p(void)
{
    assert(QMA6100P_NUM == QMA6100P_INFO_NUM);

    for (unsigned int i = 0; i < QMA6100P_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing qma6100p #%u\n", i);

        if (qma6100p_init(&qma6100p_devs[i], &qma6100p_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing qma6100p #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(qma6100p_devs[i]);
        saul_entries[i].name = qma6100p_saul_info[i].name;
        saul_entries[i].driver = &qma6100p_t_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}
