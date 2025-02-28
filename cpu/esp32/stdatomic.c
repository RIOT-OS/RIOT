/*
 * Copyright (C) 2025 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
