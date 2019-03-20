/*
 * Copyright (C) 2019 Freie Universitaet Berlin,
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdlib.h>

#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/FreeRTOS.h"

#include "semaphore_test.h"

/**
 * @brief   tests the take function for a freertos mutex semaphore
 *
 * @return pdPASS when the test is passed, pdFail otherwise
 */
int semaphore_test_mutex_take(void)
{
    SemaphoreHandle_t testing_semaphore = xSemaphoreCreateMutex();

    if (testing_semaphore == NULL) {
        puts("test failed: mutex semaphore not created");
        return pdFAIL;
    }
    /*
     * Freertos Documentation:
     * "pdPASS: Returned only if the call to xSemaphoreTake()
     * was successful in obtaining the semaphore."
     */
    /* first call should be successful */
    if (xSemaphoreTake(testing_semaphore, 0) == pdFAIL) {
        puts("error in Take");
        vSemaphoreDelete(testing_semaphore);
        return pdFAIL;
    }
    /* after the fist call every call should fail */
    if (xSemaphoreTake(testing_semaphore, 0) == pdPASS) {
        puts("error in Take");
        vSemaphoreDelete(testing_semaphore);
        return pdFAIL;
    }
    vSemaphoreDelete(testing_semaphore);
    return pdPASS;
}

/**
 * @brief   tests the take function for a freertos recursive mutex semaphore
 *
 * @return pdPASS when the test is passed, pdFAIL otherwise
 */
int semaphore_test_recursive_mutex_take(void)
{
    SemaphoreHandle_t testing_semaphore = xSemaphoreCreateRecursiveMutex();

    if (testing_semaphore == NULL) {
        puts("test failed: recursive mutex semaphore not created");
        return pdFAIL;
    }
    /*
     * Freertos Documentation:
     * "pdPASS: Returned only if the call to xSemaphoreTakeRecursive()
     * was successful in obtaining the semaphore.",
     * "once a recursive mutex has been successfully ‘taken’ by a task,
     * further calls to xSemaphoreTakeRecursive() made by the
     * same task will also be successful."
     */
    for (size_t i = 0; i < 3; i++) {
        if (xSemaphoreTakeRecursive(testing_semaphore, 0) == pdFAIL) {
            puts("error in Take");
            vSemaphoreDelete(testing_semaphore);
            return pdFAIL;
        }
    }
    vSemaphoreDelete(testing_semaphore);
    return pdPASS;
}
