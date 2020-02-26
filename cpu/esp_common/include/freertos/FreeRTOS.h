/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#ifndef FREERTOS_FREERTOS_H
#define FREERTOS_FREERTOS_H

#ifndef DOXYGEN

#include "freertos/portmacro.h"

#ifdef __cplusplus
extern "C" {
#endif

#define configMAX_PRIORITIES    SCHED_PRIO_LEVELS

#ifndef configASSERT
#define configASSERT            assert
#endif

#define portTICK_PERIOD_MS      10
#define portTickType            TickType_t

#define portTICK_RATE_MS        portTICK_PERIOD_MS

#define BaseType_t              portBASE_TYPE
#define UBaseType_t             unsigned portBASE_TYPE

typedef uint32_t TickType_t;

uint32_t xPortGetTickRateHz(void);
BaseType_t xPortInIsrContext(void);

/*
 * PLEASE NOTE: Following definitions were copied directly from the FreeRTOS
 * distribution and are under the following copyright:
 *
 * FreeRTOS V8.2.0 - Copyright (C) 2015 Real Time Engineers Ltd.
 * All rights reserved
 *
 * FreeRTOS is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License (version 2) as published by the
 * Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.
 *
 * Full license text is available on the following
 * link: http://www.freertos.org/a00114.html
 */

#define pdFALSE  ( ( BaseType_t ) 0 )
#define pdTRUE   ( ( BaseType_t ) 1 )
#define pdPASS   ( pdTRUE )
#define pdFAIL   ( pdFALSE )

#ifdef __cplusplus
}
#endif

#include "freertos/semphr.h"
#include "freertos/queue.h"

#endif /* DOXYGEN */
#endif /* FREERTOS_FREERTOS_H */
