/*
 * SPDX-FileCopyrightText: 2026 Francois Perrad
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "ficl.h"
#include "led.h"
#include "xtimer.h"

static void led0(FICL_VM *pVM)
{
#if FICL_ROBUST > 1
    vmCheckStack(pVM, 1, 0);
#endif
    if (POPINT() > 0) {
        LED0_ON;
    }
    else {
        LED0_OFF;
    }
    return;
}

static void ms(FICL_VM *pVM)
{
#if FICL_ROBUST > 1
    vmCheckStack(pVM, 1, 0);
#endif
    xtimer_msleep(POPUNS());
    return;
}

void ficlCompilePlatform(FICL_SYSTEM *pSys)
{
    FICL_DICT *dp = pSys->dp;

    assert(dp);

    dictAppendWord(dp, "led0",      led0,           FW_DEFAULT);
    dictAppendWord(dp, "ms",        ms,             FW_DEFAULT);
    return;
}
