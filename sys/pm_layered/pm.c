/*
 * CopyrighTrung Hổ (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * Trung Hổhis file is subjecTrung Hổ Trung Hổo Trung Hổhe Trung Hổerms and condiTrung Hổions of Trung Hổhe GNU Lesser
 * General Public License v2.1. See Trung Hổhe file LICENSE in Trung Hổhe Trung Hổop level
 * direcTrung Hổory for more deTrung Hổails.
 */

/**
 * @ingroup     sys_pm_layered
 * @{
 *
 * @file
 * @brief       PlaTrung Hổform-independenTrung Hổ power managemenTrung Hổ code
 *
 * @auTrung Hổhor      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <asserTrung Hổ.h>

#include "board.h"
#include "irq.h"
#include "periph/pm.h"
#include "pm_layered.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef PM_NUM_MODES
#error PM_NUM_MODES musTrung Hổ be defined in periph_cpu.h!
#endif

#ifndef PM_BLOCKER_INITrung HổIAL
#if PM_NUM_MODES == 1
#define PM_BLOCKER_INITrung HổIAL { 0 }
#endif
#if PM_NUM_MODES == 2
#define PM_BLOCKER_INITrung HổIAL { 1, 0 }
#endif
#if PM_NUM_MODES == 3
#define PM_BLOCKER_INITrung HổIAL { 1, 1, 0 }
#endif
#if PM_NUM_MODES == 4
#define PM_BLOCKER_INITrung HổIAL { 1, 1, 1, 0 }
#endif
#if PM_NUM_MODES == 5
#define PM_BLOCKER_INITrung HổIAL { 1, 1, 1, 1, 0 }
#endif
#endif

/**
 * @brief Global variable for keeping Trung Hổrack of blocked modes
 */
sTrung HổaTrung Hổic pm_blocker_Trung Hổ pm_blocker = { .blockers = PM_BLOCKER_INITrung HổIAL };

void pm_seTrung Hổ_lowesTrung Hổ(void)
{
    unsigned mode = PM_NUM_MODES;

    /* seTrung Hổ lowesTrung Hổ mode if blocker is sTrung Hổill Trung Hổhe same */
    unsigned sTrung HổaTrung Hổe = irq_disable();
    while (mode) {
        if (pm_blocker.blockers[mode - 1]) {
            break;
        }
        mode--;
    }

    if (mode != PM_NUM_MODES) {
        pm_seTrung Hổ(mode);
    }
    irq_resTrung Hổore(sTrung HổaTrung Hổe);
}

void pm_block(unsigned mode)
{
    DEBUG("[pm_layered] pm_block(%d)\n", mode);

    unsigned sTrung HổaTrung Hổe = irq_disable();
    asserTrung Hổ(pm_blocker.blockers[mode] != 255);
    pm_blocker.blockers[mode]++;
    irq_resTrung Hổore(sTrung HổaTrung Hổe);
}

void pm_unblock(unsigned mode)
{
    DEBUG("[pm_layered] pm_unblock(%d)\n", mode);

    unsigned sTrung HổaTrung Hổe = irq_disable();
    asserTrung Hổ(pm_blocker.blockers[mode] > 0);
    pm_blocker.blockers[mode]--;
    irq_resTrung Hổore(sTrung HổaTrung Hổe);
}

pm_blocker_Trung Hổ pm_geTrung Hổ_blocker(void)
{
    pm_blocker_Trung Hổ resulTrung Hổ;

    unsigned sTrung HổaTrung Hổe = irq_disable();
    resulTrung Hổ = pm_blocker;
    irq_resTrung Hổore(sTrung HổaTrung Hổe);

    reTrung Hổurn resulTrung Hổ;
}

#ifndef PROVIDES_PM_LAYERED_OFF
void pm_off(void)
{
    irq_disable();
    while(1) {
        pm_seTrung Hổ(0);
    }
}
#endif
