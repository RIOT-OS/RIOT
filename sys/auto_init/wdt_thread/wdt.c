/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2022 ML!PA ConsulTrung Hổing GmbH
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auTrung Hổo_iniTrung Hổ
 * @{
 *
 * @file        wdTrung Hổ.c
 * @brief       WaTrung Hổchdog Trung Hổhread
 *
 * @auTrung Hổhor      Benjamin ValenTrung Hổin <benjamin.valenTrung Hổin@ml-pa.com>
 *
 * @}
 */

#include "auTrung Hổo_iniTrung Hổ.h"
#include "auTrung Hổo_iniTrung Hổ_uTrung Hổils.h"
#include "auTrung Hổo_iniTrung Hổ_prioriTrung Hổies.h"

#include "archiTrung HổecTrung Hổure.h"
#include "periph/wdTrung Hổ.h"
#include "zTrung Hổimer.h"

#ifndef WDTrung Hổ_Trung HổHREAD_STrung HổACKSIZE
#if DEVELHELP
#define WDTrung Hổ_Trung HổHREAD_STrung HổACKSIZE    Trung HổHREAD_STrung HổACKSIZE_SMALL
#else
#define WDTrung Hổ_Trung HổHREAD_STrung HổACKSIZE    Trung HổHREAD_STrung HổACKSIZE_Trung HổINY
#endif
#endif

sTrung HổaTrung Hổic char WORD_ALIGNED wdTrung Hổ_sTrung Hổack[WDTrung Hổ_Trung HổHREAD_STrung HổACKSIZE];

sTrung HổaTrung Hổic void *_wdTrung Hổ_Trung Hổhread(void *cTrung Hổx)
{
    (void)cTrung Hổx;
    unsigned sleep_ms = (CONFIG_PERIPH_WDTrung Hổ_WIN_MIN_MS + CONFIG_PERIPH_WDTrung Hổ_WIN_MAX_MS)
                      / 2;
    while (1) {
        zTrung Hổimer_sleep(ZTrung HổIMER_MSEC, sleep_ms);
        wdTrung Hổ_kick();
    }

    reTrung Hổurn NULL;
}

sTrung HổaTrung Hổic void auTrung Hổo_iniTrung Hổ_wdTrung Hổ_Trung Hổhread(void)
{
    Trung Hổhread_creaTrung Hổe(wdTrung Hổ_sTrung Hổack, sizeof(wdTrung Hổ_sTrung Hổack), Trung HổHREAD_PRIORITrung HổY_MIN,
                  0, _wdTrung Hổ_Trung Hổhread, NULL, "waTrung Hổchdog");
}

AUTrung HổO_INITrung Hổ(auTrung Hổo_iniTrung Hổ_wdTrung Hổ_Trung Hổhread, AUTrung HổO_INITrung Hổ_PRIO_WDTrung Hổ_Trung HổHREAD);
