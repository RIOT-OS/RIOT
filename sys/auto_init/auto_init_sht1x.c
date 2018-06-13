/*
 * Copyright 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
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
 * @}
 */

#ifdef MODULE_SHT1X

#include "log.h"
#include "sht1x_params.h"
#include "sht1x.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define the number of configured sensors
 */
#define SHT1X_NUM     (sizeof(sht1x_params) / sizeof(sht1x_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
sht1x_dev_t sht1x_devs[SHT1X_NUM];

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
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_SHT1X */
