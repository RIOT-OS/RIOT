/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#ifndef FREERTOS_TASK_H
#define FREERTOS_TASK_H

#ifndef DOXYGEN

#include <limits.h> /* for INT_MAX */

#include "thread.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

#define xTaskHandle     TaskHandle_t
#define tskNO_AFFINITY  INT_MAX

#define taskDISABLE_INTERRUPTS      portDISABLE_INTERRUPTS
#define taskENABLE_INTERRUPTS       portENABLE_INTERRUPTS

#define taskENTER_CRITICAL          portENTER_CRITICAL
#define taskEXIT_CRITICAL           portEXIT_CRITICAL

typedef void (*TaskFunction_t)(void *);

typedef void* TaskHandle_t;

BaseType_t xTaskCreate(TaskFunction_t pvTaskCode,
                       const char * const pcName,
                       const uint32_t usStackDepth,
                       void * const pvParameters,
                       UBaseType_t uxPriority,
                       TaskHandle_t * const pvCreatedTask);

BaseType_t xTaskCreatePinnedToCore(TaskFunction_t pvTaskCode,
                                   const char * const pcName,
                                   const uint32_t usStackDepth,
                                   void * const pvParameters,
                                   UBaseType_t uxPriority,
                                   TaskHandle_t * const pvCreatedTask,
                                   const BaseType_t xCoreID);

void vTaskDelete(TaskHandle_t xTaskToDelete);
void vTaskDelay(const TickType_t xTicksToDelay);

TaskHandle_t xTaskGetCurrentTaskHandle(void);

void vTaskEnterCritical(portMUX_TYPE *mux);
void vTaskExitCritical(portMUX_TYPE *mux);

TickType_t xTaskGetTickCount(void);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* FREERTOS_TASK_H */
