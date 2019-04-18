/*
 * Copyright (C) 2019 Philipp-Alexander Blum
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
 * @brief       Auto initialization for HX711 devices
 *
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *
 * @}
 */

#ifdef MODULE_HX711

#include "log.h"
#include "saul_reg.h"
#include "hx711.h"
#include "hx711_params.h"

/**
 * @brief   Allocate memory for the device descriptors
 */
static int hx711_devs;

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[1];

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t hx711_saul_driver;

void auto_init_hx711(void)
{
    hx711_init();
    saul_entries[0].dev = &hx711_devs;
    saul_entries[0].name = hx711_saul_info.name;
    saul_entries[0].driver = &hx711_saul_driver;
    saul_reg_add(&(saul_entries[0]));
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_HTS221 */
