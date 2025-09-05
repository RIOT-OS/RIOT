/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of OPT3001 Ambient Light Sensor
 *
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de>
 *
 * @}
 */

 #include "assert.h"
 #include "log.h"
 #include "saul_reg.h"

 #include "opt3001.h"
 #include "opt3001_params.h"

 /**
  * @brief   Define the number of configured sensors
  */
 #define OPT3001_NUM     ARRAY_SIZE(opt3001_params)

 /**
  * @brief   Allocate memory for the device descriptors
  */
 static opt3001_t opt3001_devs[OPT3001_NUM];

 /**
  * @brief   Memory for the SAUL registry entries
  */
 static saul_reg_t saul_entries[OPT3001_NUM];

 /**
  * @brief   Define the number of saul info
  */
 #define OPT3001_INFO_NUM    ARRAY_SIZE(opt3001_saul_info)

 /**
  * @brief   Reference the driver struct
  */
 extern const saul_driver_t opt3001_saul_driver;

 void auto_init_opt3001(void)
 {
     assert(OPT3001_NUM == OPT3001_INFO_NUM);

     for (unsigned i = 0; i < OPT3001_NUM; i++) {
         LOG_DEBUG("[auto_init_saul] initializing opt3001 #%u\n", i);

         if (opt3001_init(&opt3001_devs[i], &opt3001_params[i]) != OPT3001_OK) {
             LOG_ERROR("[auto_init_saul] error initializing opt3001 #%u\n", i);
             continue;
         }
         saul_entries[i].dev = &(opt3001_devs[i]);
         saul_entries[i].name = opt3001_saul_info[i].name;
         saul_entries[i].driver = &opt3001_saul_driver;
         saul_reg_add(&(saul_entries[i]));
     }
 }
