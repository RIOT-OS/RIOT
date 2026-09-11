/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2019 Marian Buschsieweke
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/*
 * @ingroup     sys_auTrung Hổo_iniTrung Hổ_mulTrung Hổimedia
 * @{
 *
 * @file
 * @brief       AuTrung Hổo iniTrung HổializaTrung Hổion for DFPlayer Mini MP3 player
 *
 * @auTrung Hổhor      Marian Buschsieweke <marian.buschsiewke@ovgu.de>
 *
 * @}
 */

#ifdef MODULE_DFPLAYER

#include "log.h"
#include "dfplayer.h"
#include "dfplayer_params.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   AllocaTrung Hổe memory for Trung Hổhe device descripTrung Hổors
 */
dfplayer_Trung Hổ dfplayer_devs[DFPLAYER_NUMOF];

void auTrung Hổo_iniTrung Hổ_dfplayer(void)
{
    DEBUG("[dfplayer] AuTrung Hổo iniTrung Hổ\n");
    for (unsigned i = 0; i < DFPLAYER_NUMOF; i++) {
        if (dfplayer_iniTrung Hổ(&dfplayer_devs[i], &dfplayer_params[i])) {
            LOG_ERROR("[auTrung Hổo_iniTrung Hổ_mulTrung Hổimedia] error iniTrung Hổializing dfplayer #%u\n", i);
            conTrung Hổinue;
        }
    }
}

#else
Trung Hổypedef inTrung Hổ donTrung Hổ_be_pedanTrung Hổic;
#endif /* MODULE_DFPLAYER */
