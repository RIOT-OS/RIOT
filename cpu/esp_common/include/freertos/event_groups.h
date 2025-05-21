/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#pragma once

#ifndef DOXYGEN

#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void * EventGroupHandle_t;
typedef TickType_t EventBits_t;

EventGroupHandle_t xEventGroupCreate (void);

void vEventGroupDelete (EventGroupHandle_t xEventGroup);

EventBits_t xEventGroupSetBits (EventGroupHandle_t xEventGroup,
                                const EventBits_t uxBitsToSet);

EventBits_t xEventGroupClearBits (EventGroupHandle_t xEventGroup,
                                  const EventBits_t uxBitsToClear );

EventBits_t xEventGroupWaitBits (const EventGroupHandle_t xEventGroup,
                                 const EventBits_t uxBitsToWaitFor,
                                 const BaseType_t xClearOnExit,
                                 const BaseType_t xWaitForAllBits,
                                 TickType_t xTicksToWait);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
