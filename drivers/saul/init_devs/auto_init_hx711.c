/*
 * Copyright (C) 2018 - 2020 Philipp-Alexander Blum
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

#define HX711_NUM      ARRAY_SIZE(hx711_params)

#define HX711_SAUL_NUM ARRAY_SIZE(hx711_saul_info)
/**
 * @brief   Allocate memory for the device descriptors
 */
static hx711_t hx711_devs[HX711_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[HX711_SAUL_NUM];

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t hx711_saul_driver;

void auto_init_hx711(void)
{
    assert(HX711_NUM == HX711_SAUL_NUM);

    for(unsigned int i = 0; i < HX711_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing hx711 #%u\n", i);

        hx711_t *dev = &(hx711_devs[i]);
        hx711_init(&(hx711_devs[i]), &(hx711_params[i]));
        hx711_power_up(dev);

        saul_entries[i].dev = &(hx711_devs[i]);
        saul_entries[i].name = hx711_saul_info[i].name;
        saul_entries[i].driver = &hx711_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_HX711 */
