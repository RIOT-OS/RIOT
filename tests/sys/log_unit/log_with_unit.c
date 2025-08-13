/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Test logging with and log unit.
 *
 * @author      Mihai Renea <mihai.renea@ml-pa.com>
 */

#define LOG_UNIT "log_unit"
#include "log.h"

void log_with_unit(void)
{
    LOG_INFO("Hello!\n");
    LOG_INFO("Hello %s!\n", "world");
}
