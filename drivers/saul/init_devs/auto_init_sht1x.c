/*
 * SPDX-FileCopyrightText: 2018 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup sys_auto_init
 * @{
 *
 * @file
 * @brief       Auto initialization for SHT1X temperature/humidity sensors
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * Please note that the sht1x differs from other sensor modules, as
 * initialization is performed regardless of the use of SAUL. However, when
 * the module `auto_init_saul` is *not* used, the SAUL registration is skipped
 * and the initialization does *not* depend on SAUL in this case.
 *
 * @}
 */

#include "log.h"
#include "sht1x_params.h"
#include "sht1x.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Define the number of configured sensors
 */
#define SHT1X_NUM     ARRAY_SIZE(sht1x_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
sht1x_dev_t sht1x_devs[SHT1X_NUM];

#ifdef MODULE_AUTO_INIT_SAUL

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[SHT1X_NUM * 2];

/**
 * @name    Import SAUL endpoints
 * @{
 */
extern const saul_driver_t sht1x_saul_temp_driver;
extern const saul_driver_t sht1x_saul_hum_driver;
/** @} */

#endif /* MODULE_AUTO_INIT_SAUL */

static void sht1x_error(unsigned int num, const char *reason)
{
    LOG_ERROR("[auto_init] error initializing SHT10/SHT11/SHT15 sensor "
              "#%u: %s\n", num, reason);
}

void auto_init_sht1x(void)
{
    for (unsigned int i = 0; i < SHT1X_NUM; i++) {
        DEBUG("[auto_init_sht1x] Initializing SHT1X sensor #%u\n", i);

        switch (sht1x_init(&sht1x_devs[i], &sht1x_params[i])) {
            case 0:
                break;
            case -EIO:
                sht1x_error(i, "Failed to initialize GPIOs");
                continue;
            case -EINVAL:
                sht1x_error(i, "Invalid configuration for VDD");
                continue;
            case -EPROTO:
                sht1x_error(i, "Reset command not acknowledged");
                continue;
            default:
                /* Should not happen, but better safe than sorry */
                sht1x_error(i, "?");
                continue;
        }

#ifdef MODULE_AUTO_INIT_SAUL
        saul_entries[(i * 2)    ].dev = &(sht1x_devs[i]);
        saul_entries[(i * 2) + 1].dev = &(sht1x_devs[i]);
        saul_entries[(i * 2)    ].name = sht1x_saul_info[(i * 2)    ].name;
        saul_entries[(i * 2) + 1].name = sht1x_saul_info[(i * 2) + 1].name;
        saul_entries[(i * 2)    ].driver = &sht1x_saul_temp_driver;
        saul_entries[(i * 2) + 1].driver = &sht1x_saul_hum_driver;
        saul_reg_add(&(saul_entries[(i * 2)    ]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
#endif /* MODULE_AUTO_INIT_SAUL */
    }
}
