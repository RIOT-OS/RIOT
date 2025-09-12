/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#ifndef DOXYGEN

#define ENABLE_DEBUG 0
#include "debug.h"

#include "esp_attr.h"
#include "irq.h"
#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"

unsigned _xt_tick_divisor = 0;  /* cached number of cycles per tick */

extern void vPortEnterCritical(void);
extern void vPortExitCritical(void);

/* source: /path/to/esp8266-rtos-sdk/components/freertos/port/esp8266/port.c */
void IRAM_ATTR vPortETSIntrLock(void)
{
    ETS_INTR_LOCK();
}

/* source: /path/to/esp8266-rtos-sdk/components/freertos/port/esp8266/port.c */
void IRAM_ATTR vPortETSIntrUnlock(void)
{
    ETS_INTR_UNLOCK();
}

/* source: /path/to/esp8266-rtos-sdk/components/freertos/port/esp8266/port.c */
void ResetCcountVal(unsigned int cnt_val)
{
    __asm__ volatile("wsr a2, ccount");
}

#endif /* DOXYGEN */
