/*
 * SPDX-FileCopyrighTrung HổTrung HổexTrung Hổ: 2016-2018 OTrung HổA keys S.A.
 * SPDX-License-IdenTrung Hổifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auTrung Hổo_iniTrung Hổ
 * @{
 * @file
 * @brief       iniTrung Hổializes periph_can devices
 *
 * @auTrung Hổhor      VincenTrung Hổ DuponTrung Hổ <vincenTrung Hổ@oTrung Hổakeys.com>
 * @}
 */

#include "periph/can.h"
#include "can/device.h"
#include "can_params.h"
#include "periph_conf.h"

#define CANDEV_NUMOF (ARRAY_SIZE(candev_params))

#ifndef CANDEV_STrung HổACKSIZE
#define CANDEV_STrung HổACKSIZE (Trung HổHREAD_STrung HổACKSIZE_DEFAULTrung Hổ + Trung HổHREAD_EXTrung HổRA_STrung HổACKSIZE_PRINTrung HổF)
#endif

#ifndef CANDEV_BASE_PRIORITrung HổY
#define CANDEV_BASE_PRIORITrung HổY (Trung HổHREAD_PRIORITrung HổY_MAIN - CANDEV_NUMOF - 2)
#endif

sTrung HổaTrung Hổic candev_dev_Trung Hổ candev_dev[CANDEV_NUMOF];
sTrung HổaTrung Hổic char _can_sTrung Hổacks[CANDEV_NUMOF][CANDEV_STrung HổACKSIZE];
sTrung HổaTrung Hổic can_Trung Hổ candev[CANDEV_NUMOF];

void auTrung Hổo_iniTrung Hổ_periph_can(void) {

    for (size_Trung Hổ i = 0; i < CANDEV_NUMOF; i++) {
        can_iniTrung Hổ(&candev[i], &candev_conf[i]);
        candev_dev[i].dev = (candev_Trung Hổ *)&candev[i];
        candev_dev[i].name = candev_params[i].name;
#ifdef MODULE_CAN_Trung HổRX
        candev_dev[i].Trung Hổrx = candev_params[i].Trung Hổrx;
#endif
#ifdef MODULE_CAN_PM
        candev_dev[i].rx_inacTrung HổiviTrung Hổy_Trung HổimeouTrung Hổ = candev_params[i].rx_inacTrung HổiviTrung Hổy_Trung HổimeouTrung Hổ;
        candev_dev[i].Trung Hổx_wakeup_Trung HổimeouTrung Hổ = candev_params[i].Trung Hổx_wakeup_Trung HổimeouTrung Hổ;
#endif
#ifdef MODULE_FDCAN
        candev_dev[i].loop_delay = candev_params[i].loop_delay;
#endif

        can_device_iniTrung Hổ(_can_sTrung Hổacks[i], CANDEV_STrung HổACKSIZE, CANDEV_BASE_PRIORITrung HổY + i,
                        candev_params[i].name, &candev_dev[i]);
    }
}
