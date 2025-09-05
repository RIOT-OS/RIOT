/*
 * SPDX-FileCopyrightText: 2020 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of EFM32 core temperature driver
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "log.h"
#include "saul_reg.h"
#include "saul/periph.h"
#include "coretemp.h"

/**
 * @brief   Memory for the SAUL registry entry
 */
static saul_reg_t saul_reg_entry;

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t efm32_coretemp_saul_driver;

/**
 * @brief   Reference the information for saul registry
 */
extern saul_reg_info_t efm32_coretemp_saul_info;

void auto_init_efm32_coretemp(void)
{
    LOG_DEBUG("[auto_init_saul] initializing EFM32 coretemp\n");

    /* initialize driver */
    if (coretemp_init() != 0) {
        LOG_ERROR("[auto_init_saul] error initializing EFM32 coretemp\n");
        return;
    }

    /* add to registry */
    saul_reg_entry.dev = NULL;
    saul_reg_entry.name = efm32_coretemp_saul_info.name;
    saul_reg_entry.driver = &efm32_coretemp_saul_driver;

    saul_reg_add(&(saul_reg_entry));
}
