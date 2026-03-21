/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of the QMI8658 IMU
 *
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "qmi8658.h"
#include "qmi8658_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define QMI8658_NUM    ARRAY_SIZE(qmi8658_params)

/**
 * @brief   Define the number of entries of the device
 */
#define QMI8658_ENTRY_NUM    3

/**
 * @brief   Allocate memory for the device descriptors
 */
static qmi8658_t qmi8658_devs[QMI8658_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[QMI8658_NUM * QMI8658_ENTRY_NUM];

/**
 * @brief   Define the number of saul info
 */
#define QMI8658_INFO_NUM    ARRAY_SIZE(qmi8658_saul_info)

/**
 * @name    Reference the driver structs
 * @{
 */
extern saul_driver_t qmi8658_saul_acc_driver;
extern saul_driver_t qmi8658_saul_gyro_driver;
extern saul_driver_t qmi8658_saul_temp_driver;
/** @} */

void auto_init_qmi8658(void)
{
    assert(QMI8658_NUM == QMI8658_INFO_NUM);

    for (unsigned int i = 0; i < QMI8658_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing qmi8658 #%u\n", i);

        if (qmi8658_init(&qmi8658_devs[i], &qmi8658_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing qmi8658 #%u\n", i);
            continue;
        }

        qmi8658_set_mode(&qmi8658_devs[i], QMI8658_NORMAL_ACC_GYRO);

        saul_entries[(i * QMI8658_ENTRY_NUM)].dev = &(qmi8658_devs[i]);
        saul_entries[(i * QMI8658_ENTRY_NUM)].name = qmi8658_saul_info[i].name;
        saul_entries[(i * QMI8658_ENTRY_NUM)].driver = &qmi8658_saul_acc_driver;
        saul_reg_add(&(saul_entries[(i * QMI8658_ENTRY_NUM)]));
        saul_entries[(i * QMI8658_ENTRY_NUM) + 1].dev = &(qmi8658_devs[i]);
        saul_entries[(i * QMI8658_ENTRY_NUM) + 1].name = qmi8658_saul_info[i].name;
        saul_entries[(i * QMI8658_ENTRY_NUM) + 1].driver = &qmi8658_saul_gyro_driver;
        saul_reg_add(&(saul_entries[(i * QMI8658_ENTRY_NUM) + 1]));
        saul_entries[(i * QMI8658_ENTRY_NUM) + 2].dev = &(qmi8658_devs[i]);
        saul_entries[(i * QMI8658_ENTRY_NUM) + 2].name = qmi8658_saul_info[i].name;
        saul_entries[(i * QMI8658_ENTRY_NUM) + 2].driver = &qmi8658_saul_temp_driver;
        saul_reg_add(&(saul_entries[(i * QMI8658_ENTRY_NUM) + 2]));
    }
}
