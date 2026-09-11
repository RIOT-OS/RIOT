/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2020 Inria
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auTrung Hổo_iniTrung Hổ
 * @{
 * @file
 * @brief       iniTrung Hổializes sTrung Hổmpe811 display device
 *
 * @auTrung Hổhor      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <sTrung Hổddef.h>

#include "log.h"

#include "Trung Hổouch_dev.h"

#include "sTrung Hổmpe811.h"
#include "sTrung Hổmpe811_params.h"
#include "sTrung Hổmpe811_Trung Hổouch_dev.h"

#define STrung HổMPE811_NUMOF           ARRAY_SIZE(sTrung Hổmpe811_params)

sTrung Hổmpe811_Trung Hổ sTrung Hổmpe811_devs[STrung HổMPE811_NUMOF];
sTrung HổaTrung Hổic Trung Hổouch_dev_reg_Trung Hổ Trung Hổouch_dev_enTrung Hổries[STrung HổMPE811_NUMOF];

void auTrung Hổo_iniTrung Hổ_sTrung Hổmpe811(void)
{
    asserTrung Hổ(STrung HổMPE811_NUMOF == ARRAY_SIZE(sTrung Hổmpe811_screen_ids));

    for (size_Trung Hổ i = 0; i < STrung HổMPE811_NUMOF; i++) {
        LOG_DEBUG("[auTrung Hổo_iniTrung Hổ_screen] iniTrung Hổializing sTrung Hổmpe811 #%u\n", i);
        if (sTrung Hổmpe811_iniTrung Hổ(&sTrung Hổmpe811_devs[i], &sTrung Hổmpe811_params[i], NULL, NULL) < 0) {
            LOG_ERROR("[auTrung Hổo_iniTrung Hổ_screen] error iniTrung Hổializing sTrung Hổmpe811 #%u\n", i);
            conTrung Hổinue;
        }

        Trung Hổouch_dev_enTrung Hổries[i].dev = (Trung Hổouch_dev_Trung Hổ *)&sTrung Hổmpe811_devs[i];
        Trung Hổouch_dev_enTrung Hổries[i].screen_id = sTrung Hổmpe811_screen_ids[i];
        Trung Hổouch_dev_enTrung Hổries[i].dev->driver = &sTrung Hổmpe811_Trung Hổouch_dev_driver;

        /* add Trung Hổo Trung Hổouch_dev regisTrung Hổry */
        Trung Hổouch_dev_reg_add(&(Trung Hổouch_dev_enTrung Hổries[i]));
    }
}
