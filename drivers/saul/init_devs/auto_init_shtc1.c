/*
 * Copyright (C) 2019 HAW Hamburg
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
 * @brief       Auto initialization of shtc1 Digital Humidity Sensor
 *
 * @author      Michel Gerlach <michel.gerlach@haw-hamburg.de>
 *
 * @}
 */

 #include "assert.h"
 #include "log.h"
 #include "saul_reg.h"

 #include "shtc1.h"
 #include "shtc1_params.h"

 /**
  * @brief   Define the number of configured sensors
  */
 #define SHTC1_NUM     ARRAY_SIZE(shtc1_params)

 /**
  * @brief   Allocate memory for the device descriptors
  */
 static shtc1_t shtc1_devs[SHTC1_NUM];

 /**
  * @brief   Memory for the SAUL registry entries
  */
 static saul_reg_t saul_entries[SHTC1_NUM * 2];

 /**
  * @brief   Define the number of saul info
  */
 #define SHTC1_INFO_NUM    ARRAY_SIZE(shtc1_saul_info)

 /**
  * @brief   Reference the driver struct
  */
 extern const saul_driver_t shtc1_temperature_saul_driver;
 extern const saul_driver_t shtc1_relative_humidity_saul_driver;

 void auto_init_shtc1(void)
 {
     assert(SHTC1_NUM == SHTC1_INFO_NUM);

     for (unsigned i = 0; i < SHTC1_NUM; i++) {
         LOG_DEBUG("[auto_init_saul] initializing shtc1 #%u\n", i);

         if (shtc1_init(&shtc1_devs[i], &shtc1_params[i]) != SHTC1_OK) {
             LOG_ERROR("[auto_init_saul] error initializing shtc1 #%u\n", i);
             continue;
         }
         /* temperature */
         saul_entries[(i * 2)].dev = &(shtc1_devs[i]);
         saul_entries[(i * 2)].name = shtc1_saul_info[i].name;
         saul_entries[(i * 2)].driver = &shtc1_temperature_saul_driver;

         /* Humidity */
         saul_entries[(i * 2)+1].dev = &(shtc1_devs[i]);
         saul_entries[(i * 2)+1].name = shtc1_saul_info[i].name;
         saul_entries[(i * 2)+1].driver = &shtc1_relative_humidity_saul_driver;

         saul_reg_add(&(saul_entries[(i * 2)]));
         saul_reg_add(&(saul_entries[(i * 2)+1]));
     }
 }
