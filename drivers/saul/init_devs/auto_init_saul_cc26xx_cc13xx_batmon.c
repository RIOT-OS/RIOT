/*
 * Copyright (C) 2021 Jean Pierre Dudey
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
 * @brief       Auto initialization of internal temperature and battery voltage
 *              sensor directly mapped to SAUL reg
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "log.h"
#include "saul_reg.h"
#include "saul/periph.h"

extern void cc26xx_cc13xx_batmon_init(void);

extern saul_driver_t cc26xx_cc13xx_voltage_saul_driver;

extern saul_driver_t cc26xx_cc13xx_temperature_saul_driver;

/**
 * @brief   Memory for the registry entries
 */
static saul_reg_t saul_entries[] = {
    {
        .dev = NULL,
        .name = "CC26XX_CC13XX_BAT_V",
        .driver = &cc26xx_cc13xx_voltage_saul_driver,
    },
    {
        .dev = NULL,
        .name = "CC26XX_CC13XX_BAT_T",
        .driver = &cc26xx_cc13xx_temperature_saul_driver,
    }
};

void auto_init_cc26xx_cc13xx_batmon(void)
{
    cc26xx_cc13xx_batmon_init();

    /* Add to registry */
    saul_reg_add(&(saul_entries[0]));
    saul_reg_add(&(saul_entries[1]));
}
