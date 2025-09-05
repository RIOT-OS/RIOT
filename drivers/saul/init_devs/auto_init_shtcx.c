/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of shtcx Digital Humidity Sensor
 *
 * @author      Michel Gerlach <michel.gerlach@haw-hamburg.de>
 *
 * @}
 */

 #include "assert.h"
 #include "log.h"
 #include "saul_reg.h"

 #include "shtcx.h"
 #include "shtcx_params.h"

 /**
  * @brief   Define the number of configured sensors
  */
 #define SHTCX_NUM     ARRAY_SIZE(shtcx_params)

 /**
  * @brief   Allocate memory for the device descriptors
  */
 static shtcx_t shtcx_devs[SHTCX_NUM];

 /**
  * @brief   Memory for the SAUL registry entries
  */
 static saul_reg_t saul_entries[SHTCX_NUM * 2];

 /**
  * @brief   Define the number of saul info
  */
 #define SHTCX_INFO_NUM    ARRAY_SIZE(shtcx_saul_info)

 /**
  * @brief   Reference the driver struct
  */
 extern const saul_driver_t shtcx_temperature_saul_driver;
 extern const saul_driver_t shtcx_relative_humidity_saul_driver;

 void auto_init_shtcx(void)
 {
     assert(2 * SHTCX_NUM == SHTCX_INFO_NUM);

     for (unsigned i = 0; i < SHTCX_NUM; i++) {
         LOG_DEBUG("[auto_init_saul] initializing shtcx #%u\n", i);

         if (shtcx_init(&shtcx_devs[i], &shtcx_params[i]) != SHTCX_OK) {
             LOG_ERROR("[auto_init_saul] error initializing shtcx #%u\n", i);
             continue;
         }
         /* temperature */
         saul_entries[(i * 2)].dev = &(shtcx_devs[i]);
         saul_entries[(i * 2)].name = shtcx_saul_info[i].name;
         saul_entries[(i * 2)].driver = &shtcx_temperature_saul_driver;

         /* Humidity */
         saul_entries[(i * 2)+1].dev = &(shtcx_devs[i]);
         saul_entries[(i * 2)+1].name = shtcx_saul_info[(i * 2)+1].name;
         saul_entries[(i * 2)+1].driver = &shtcx_relative_humidity_saul_driver;

         saul_reg_add(&(saul_entries[(i * 2)]));
         saul_reg_add(&(saul_entries[(i * 2)+1]));
     }
 }
