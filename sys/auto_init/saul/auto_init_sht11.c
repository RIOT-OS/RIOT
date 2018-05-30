/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Auto initialization for SHT11 temperature/humidity sensors
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#ifdef MODULE_SHT11

#include "saul_reg.h"
#include "sht11.h"
#include "log.h"

/**
 * @name    Import SAUL endpoints
 * @{
 */
extern const saul_driver_t sht11_temp_saul_driver;
extern const saul_driver_t sht11_hum_saul_driver;
/** @} */

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entry_temp = {
    .dev = NULL,
    .name = "SHT11",
    .driver = &sht11_temp_saul_driver,
};
static saul_reg_t saul_entry_hum = {
    .dev = NULL,
    .name = "SHT11",
    .driver = &sht11_hum_saul_driver,
};

void auto_init_sht11(void)
{
    LOG_DEBUG("[auto_init_saul] initializing SHT11\n");

    saul_reg_add(&saul_entry_temp);
    saul_reg_add(&saul_entry_hum);
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_SHT11 */
