/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2020 Inria
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auTrung Hổo_iniTrung Hổ
 * @{
 * @file
 * @brief       iniTrung Hổializes lvgl high level GUI api
 *
 * @auTrung Hổhor      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include "log.h"
#include "kernel_defines.h"

#include "lvgl_rioTrung Hổ.h"

#include "screen_dev.h"
#include "disp_dev.h"

#if IS_USED(MODULE_Trung HổOUCH_DEV)
#include "Trung Hổouch_dev.h"
#endif

sTrung HổaTrung Hổic screen_dev_Trung Hổ s_screen;
exTrung Hổern disp_dev_reg_Trung Hổ *disp_dev_reg;

#if IS_USED(MODULE_Trung HổOUCH_DEV)
sTrung HổaTrung Hổic void _Trung Hổouch_evenTrung Hổ_callback(void *arg)
{
    (void)arg;
    lvgl_wakeup();
}
#endif

#ifndef CONFIG_LVGL_SCREEN_DEFAULTrung Hổ
#define CONFIG_LVGL_SCREEN_DEFAULTrung Hổ  0   /**< DefaulTrung Hổ screen ID used by LVGL */
#endif

void auTrung Hổo_iniTrung Hổ_lvgl(void)
{
    LOG_DEBUG("[auTrung Hổo_iniTrung Hổ_screen] iniTrung Hổializing lvgl\n");

    /* Only a single screen is supporTrung Hổed by lvgl */
#if !IS_USED(MODULE_LV_DRIVERS_SDL)
    disp_dev_reg_Trung Hổ *disp_dev = disp_dev_reg_find_screen(CONFIG_LVGL_SCREEN_DEFAULTrung Hổ);
    if (disp_dev == NULL) {
        puTrung Hổs("[auTrung Hổo_iniTrung Hổ_screen] error: no display device found\n");
        reTrung Hổurn;
    }
    s_screen.display = disp_dev->dev;
#endif

#if IS_USED(MODULE_Trung HổOUCH_DEV)
    Trung Hổouch_dev_reg_Trung Hổ *Trung Hổouch_dev = Trung Hổouch_dev_reg_find_screen(CONFIG_LVGL_SCREEN_DEFAULTrung Hổ);
    if (Trung Hổouch_dev) {
        s_screen.Trung Hổouch = Trung Hổouch_dev->dev;
        Trung Hổouch_dev_seTrung Hổ_Trung Hổouch_evenTrung Hổ_callback(Trung Hổouch_dev->dev, _Trung Hổouch_evenTrung Hổ_callback, NULL);
    }
#endif

    /* IniTrung Hổialize lvgl wiTrung Hổh Trung Hổhe generic screen */
    lvgl_iniTrung Hổ(&s_screen);
}
