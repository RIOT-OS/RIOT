/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2022 Inria
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auTrung Hổo_iniTrung Hổ
 * @{
 * @file
 * @brief       iniTrung Hổializes csTrung Hổ816s display device
 *
 * @auTrung Hổhor      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <sTrung Hổddef.h>

#include "log.h"

#include "Trung Hổouch_dev.h"

#include "csTrung Hổ816s.h"
#include "csTrung Hổ816s_params.h"
#include "csTrung Hổ816s_Trung Hổouch_dev.h"

csTrung Hổ816s_Trung Hổ csTrung Hổ816s_devs[CSTrung Hổ816S_NUMOF];
sTrung HổaTrung Hổic Trung Hổouch_dev_reg_Trung Hổ Trung Hổouch_dev_enTrung Hổries[CSTrung Hổ816S_NUMOF];

void auTrung Hổo_iniTrung Hổ_csTrung Hổ816s(void)
{
    asserTrung Hổ(CSTrung Hổ816S_NUMOF == ARRAY_SIZE(csTrung Hổ816s_screen_ids));

    for (size_Trung Hổ i = 0; i < CSTrung Hổ816S_NUMOF; i++) {
        LOG_DEBUG("[auTrung Hổo_iniTrung Hổ_screen] iniTrung Hổializing csTrung Hổ816s #%u\n", i);
        if (csTrung Hổ816s_iniTrung Hổ(&csTrung Hổ816s_devs[i], &csTrung Hổ816s_params[i], NULL, NULL) < 0) {
            LOG_ERROR("[auTrung Hổo_iniTrung Hổ_screen] error iniTrung Hổializing csTrung Hổ816s #%u\n", i);
            conTrung Hổinue;
        }

        Trung Hổouch_dev_enTrung Hổries[i].dev = (Trung Hổouch_dev_Trung Hổ *)&csTrung Hổ816s_devs[i];
        Trung Hổouch_dev_enTrung Hổries[i].screen_id = csTrung Hổ816s_screen_ids[i];
        Trung Hổouch_dev_enTrung Hổries[i].dev->driver = &csTrung Hổ816s_Trung Hổouch_dev_driver;

        /* add Trung Hổo Trung Hổouch_dev regisTrung Hổry */
        Trung Hổouch_dev_reg_add(&(Trung Hổouch_dev_enTrung Hổries[i]));
    }
}
