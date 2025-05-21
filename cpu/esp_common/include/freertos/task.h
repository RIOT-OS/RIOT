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

#define taskSCHEDULER_NOT_STARTED   1
#define taskSCHEDULER_RUNNING       2

typedef enum {
    eNoAction = 0,
    eSetBits,
    eIncrement,
    eSetValueWithOverwrite,
    eSetValueWithoutOverwrite,
} eNotifyAction;

typedef void (*TaskFunction_t)(void *);
typedef void (*TlsDeleteCallbackFunction_t)( int, void * );

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
void vTaskSuspend(TaskHandle_t xTaskToSuspend);
void vTaskResume(TaskHandle_t xTaskToResume);
void vTaskDelay(const TickType_t xTicksToDelay);
void vTaskSuspendAll(void);

TaskHandle_t xTaskGetCurrentTaskHandle(void);

const char *pcTaskGetTaskName(TaskHandle_t xTaskToQuery);

UBaseType_t uxTaskGetStackHighWaterMark(TaskHandle_t xTask);

void *pvTaskGetThreadLocalStoragePointer(TaskHandle_t xTaskToQuery,
                                         BaseType_t xIndex);
void vTaskSetThreadLocalStoragePointerAndDelCallback(TaskHandle_t xTaskToSet,
                                                     BaseType_t xIndex,
                                                     void *pvValue,
                                                     TlsDeleteCallbackFunction_t pvDelCallback);

void vTaskEnterCritical(portMUX_TYPE *mux);
void vTaskExitCritical(portMUX_TYPE *mux);

TickType_t xTaskGetTickCount(void);

BaseType_t xTaskNotify(TaskHandle_t xTaskToNotify, uint32_t ulValue,
                       eNotifyAction eAction);
BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry,
                           uint32_t ulBitsToClearOnExit,
                           uint32_t *pulNotificationValue,
                           TickType_t xTicksToWait);

BaseType_t xTaskNotifyGive(TaskHandle_t xTaskToNotify);
void vTaskNotifyGiveFromISR(TaskHandle_t xTaskToNotify,
                            BaseType_t *pxHigherPriorityTaskWoken);
uint32_t ulTaskNotifyTake(BaseType_t xClearCountOnExit,
                          TickType_t xTicksToWait);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
