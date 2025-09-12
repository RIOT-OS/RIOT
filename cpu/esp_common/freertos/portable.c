/*
 * SPDX-FileCopyrightText: 2021 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#ifndef DOXYGEN

#define ENABLE_DEBUG 0
#include "debug.h"

#include <string.h>

#include "esp_common.h"
#include "log.h"
#include "timex.h"

#include "freertos/FreeRTOS.h"

uint32_t xPortGetTickRateHz(void) {
    return MS_PER_SEC / portTICK_PERIOD_MS;
}

BaseType_t xPortInIsrContext(void)
{
    /* is working on single core in that way */
    return irq_is_in();
}

UBaseType_t xPortSetInterruptMaskFromISR(void)
{
    UBaseType_t state = irq_disable();
    return state;
}

void vPortClearInterruptMaskFromISR(UBaseType_t state)
{
    irq_restore(state);
}

bool xPortCanYield(void)
{
    return irq_is_enabled();
}

void vPortEnterCritical(void)
{
    vTaskEnterCritical(0);
}

void vPortExitCritical(void)
{
    vTaskExitCritical(0);
}

#endif /* DOXYGEN */
