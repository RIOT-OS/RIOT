// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
#ifndef HAL_TASK_H_INCLUDED
#define HAL_TASK_H_INCLUDED

#include "ud3tn/result.h"
#include "platform/hal_types.h"

#include <stdbool.h>

enum ud3tn_result hal_task_create(
    void (*task_function)(void *),
    void *task_parameters,
    bool detach,
    TaskIdentifier_t *task_id);

enum ud3tn_result hal_task_start_scheduler(void);
void hal_task_delay(int delay);

#endif /* HAL_TASK_H_INCLUDED */
