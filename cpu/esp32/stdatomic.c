/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Missing atomic built-in functions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <stdbool.h>

__attribute__((weak))
bool __atomic_test_and_set(volatile void *ptr, int memorder)
{
    return __atomic_exchange_1(ptr, true, memorder);
}
