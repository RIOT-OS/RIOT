/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#ifndef FREERTOS_PORTMACRO_H
#define FREERTOS_PORTMACRO_H

#ifndef DOXYGEN

#include "stdbool.h"
#include "stdint.h"

#ifndef CPU_ESP8266
#include "esp_heap_caps.h"
#include "esp_timer.h"
#endif

#include "mutex.h"
#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

#define portBASE_TYPE                   int
#define portUBASE_TYPE                  unsigned portBASE_TYPE
#define portTICK_TYPE                   uint32_t
#define portSTACK_TYPE                  uint8_t

#define portMAX_DELAY                   0xFFFFFFFFUL

#define portMUX_TYPE                    mutex_t
#define portMUX_INITIALIZE              mutex_init
#define portMUX_INITIALIZER_UNLOCKED    MUTEX_INIT

#define portYIELD                       thread_yield_higher
#define portYIELD_FROM_ISR              thread_yield_higher

#define portENTER_CRITICAL              vTaskEnterCritical
#define portEXIT_CRITICAL               vTaskExitCritical
#define portENTER_CRITICAL_SAFE         vTaskEnterCritical
#define portEXIT_CRITICAL_SAFE          vTaskExitCritical
#define portENTER_CRITICAL_ISR          vTaskEnterCritical
#define portEXIT_CRITICAL_ISR           vTaskExitCritical
#define portENTER_CRITICAL_NESTED       irq_disable
#define portEXIT_CRITICAL_NESTED        irq_restore

#define portSET_INTERRUPT_MASK_FROM_ISR     xPortSetInterruptMaskFromISR
#define portCLEAR_INTERRUPT_MASK_FROM_ISR   vPortClearInterruptMaskFromISR

#define errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY    ( -1 )

#if defined(CPU_FAM_ESP32) || defined(CPU_FAM_ESP32S3)

#define portNUM_PROCESSORS              2
#define xPortGetCoreID()                PRO_CPU_NUM
#define vPortYield                      portYIELD

#else /* defined(CPU_FAM_ESP32) || defined(CPU_FAM_ESP32S3) */

#define portNUM_PROCESSORS              1
#define xPortGetCoreID()                PRO_CPU_NUM
#define vPortYield                      portYIELD

#endif /* defined(CPU_FAM_ESP32) || defined(CPU_FAM_ESP32S3) */

extern void vTaskEnterCritical(portMUX_TYPE *mux);
extern void vTaskExitCritical(portMUX_TYPE *mux);

bool xPortCanYield(void);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* FREERTOS_PORTMACRO_H */
