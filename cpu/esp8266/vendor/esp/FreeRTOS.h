/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module
 */

#ifndef FREERTOS_H
#define FREERTOS_H

#ifndef DOXYGEN

#include "stdlib.h"

#include "mutex.h"
#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SemaphoreHandle_t mutex_t*

static inline SemaphoreHandle_t xSemaphoreCreateMutex(void)
{
    mutex_t* _tmp = malloc (sizeof(mutex_t));
    mutex_init(_tmp);
    return _tmp;
}

#define xSemaphoreTake(s,to)   mutex_lock(s)
#define xSemaphoreGive(s)      mutex_unlock(s)

#define vPortEnterCritical()   int _irq_state = irq_disable ()
#define vPortExitCritical()    irq_restore(_irq_state)

#ifdef __cplusplus
}
#endif

#endif // DOXYGEN
#endif /* FREERTOS_H */
