/* SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0 */
/**
 * @file
 * @brief Task creation and timing interface for the uD3TN HAL.
 */
#ifndef HAL_TASK_H_INCLUDED
#define HAL_TASK_H_INCLUDED

#include "ud3tn/result.h"
#include "platform/hal_types.h"

#include <stdbool.h>

/**
 * @brief Create a task using the platform's task implementation.
 * @param task_function Function executed by the task.
 * @param task_parameters Argument passed to @p task_function.
 * @param detach Detach preference; the RIOT implementation currently does not
 *        distinguish detached from joinable tasks.
 * @param task_id Optional output location for the created task identifier.
 * @return `UD3TN_OK` on success, otherwise an error result.
 */
enum ud3tn_result hal_task_create(
    void (*task_function)(void *),
    void *task_parameters,
    bool detach,
    TaskIdentifier_t *task_id);

/**
 * @brief Keep the RIOT scheduler active.
 *
 * The RIOT implementation remains in its scheduler loop indefinitely.
 * @return `UD3TN_OK` if the loop exits.
 */
enum ud3tn_result hal_task_start_scheduler(void);

/**
 * @brief Delay the calling task.
 * @param delay Delay duration in milliseconds; non-positive values do nothing.
 */
void hal_task_delay(int delay);

#endif /* HAL_TASK_H_INCLUDED */
