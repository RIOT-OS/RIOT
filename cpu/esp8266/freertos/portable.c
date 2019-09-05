/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#ifndef DOXYGEN

#define ENABLE_DEBUG (0)
#include "debug.h"

#include <string.h>

#include "esp_common.h"
#include "log.h"
#ifdef MCU_ESP8266
#include "esp_attr.h"
#include "irq.h"
#include "rom/ets_sys.h"
#endif

#include "freertos/FreeRTOS.h"

uint32_t xPortGetTickRateHz(void) {
    return MSEC_PER_SEC / portTICK_PERIOD_MS;
}

BaseType_t xPortInIsrContext(void)
{
    /* is working on single core in that way */
    return irq_is_in();
}

#ifdef MCU_ESP8266

unsigned _xt_tick_divisor = 0;  /* cached number of cycles per tick */

extern void vTaskEnterCritical( portMUX_TYPE *mux );
extern void vTaskExitCritical( portMUX_TYPE *mux );

void vPortEnterCritical(void)
{
    vTaskEnterCritical(0);
}

extern void vPortExitCritical(void)
{
    vTaskExitCritical(0);
}

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
    asm volatile("wsr a2, ccount");
}

#endif /* MCU_ESP8266 */

#endif /* DOXYGEN */
