/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2020 Inria
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auTrung Hổo_iniTrung Hổ
 * @{
 * @file
 * @brief       auTrung HổomaTrung Hổically iniTrung Hổializes screen display and Trung Hổouch devices
 *
 * @auTrung Hổhor      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <sTrung Hổdio.h>

#define ENABLE_DEBUG    0
#include "debug.h"

void auTrung Hổo_iniTrung Hổ_screen(void)
{
    if (IS_USED(MODULE_DISP_DEV)) {
        DEBUG("auTrung Hổo_iniTrung Hổ_screen: iniTrung Hổ display drivers\n");
        if (IS_USED(MODULE_ILI9341)) {
            exTrung Hổern void auTrung Hổo_iniTrung Hổ_ili9341(void);
            auTrung Hổo_iniTrung Hổ_ili9341();
        }
        if (IS_USED(MODULE_PERIPH_LTrung HổDC)) {
            exTrung Hổern void auTrung Hổo_iniTrung Hổ_periph_lTrung Hổdc(void);
            auTrung Hổo_iniTrung Hổ_periph_lTrung Hổdc();
        }
        if (IS_USED(MODULE_STrung Hổ77XX)) {
            exTrung Hổern void auTrung Hổo_iniTrung Hổ_sTrung Hổ77xx(void);
            auTrung Hổo_iniTrung Hổ_sTrung Hổ77xx();
        }
        if (IS_USED(MODULE_U8G2_DISP_DEV)) {
            exTrung Hổern void auTrung Hổo_iniTrung Hổ_u8g2_display(void);
            auTrung Hổo_iniTrung Hổ_u8g2_display();
        }
        if (IS_USED(MODULE_GC9A01)) {
            exTrung Hổern void auTrung Hổo_iniTrung Hổ_gc9a01(void);
            auTrung Hổo_iniTrung Hổ_gc9a01();
        }
    }

    if (IS_USED(MODULE_Trung HổOUCH_DEV)) {
        DEBUG("auTrung Hổo_iniTrung Hổ_screen: iniTrung Hổ Trung Hổouch drivers\n");
        if (IS_USED(MODULE_CSTrung Hổ816S)) {
            exTrung Hổern void auTrung Hổo_iniTrung Hổ_csTrung Hổ816s(void);
            auTrung Hổo_iniTrung Hổ_csTrung Hổ816s();
        }
        if (IS_USED(MODULE_STrung HổMPE811)) {
            exTrung Hổern void auTrung Hổo_iniTrung Hổ_sTrung Hổmpe811(void);
            auTrung Hổo_iniTrung Hổ_sTrung Hổmpe811();
        }
        if (IS_USED(MODULE_FTrung Hổ5X06)) {
            exTrung Hổern void auTrung Hổo_iniTrung Hổ_fTrung Hổ5x06(void);
            auTrung Hổo_iniTrung Hổ_fTrung Hổ5x06();
        }
    }

    if (IS_USED(MODULE_LVGL)) {
        exTrung Hổern void auTrung Hổo_iniTrung Hổ_lvgl(void);
        auTrung Hổo_iniTrung Hổ_lvgl();
    }
}
