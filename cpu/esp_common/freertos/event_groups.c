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

#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

EventGroupHandle_t xEventGroupCreate (void)
{
    printf("%s is not supported\n", __func__);
    return NULL;
}

void vEventGroupDelete (EventGroupHandle_t xEventGroup)
{
    printf("%s is not supported\n", __func__);
}

EventBits_t xEventGroupSetBits (EventGroupHandle_t xEventGroup,
                                const EventBits_t uxBitsToSet)
{
    printf("%s is not supported\n", __func__);
    return 0;
}

EventBits_t xEventGroupClearBits (EventGroupHandle_t xEventGroup,
                                  const EventBits_t uxBitsToClear )
{
    printf("%s is not supported\n", __func__);
    return 0;
}

EventBits_t xEventGroupWaitBits (const EventGroupHandle_t xEventGroup,
                                 const EventBits_t uxBitsToWaitFor,
                                 const BaseType_t xClearOnExit,
                                 const BaseType_t xWaitForAllBits,
                                 TickType_t xTicksToWait)
{
    printf("%s is not supported\n", __func__);
    return 0;
}

#endif /* DOXYGEN */
