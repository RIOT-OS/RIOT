/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Test logging with and without log unit.
 *
 * @author      Mihai Renea <mihai.renea@ml-pa.com>
 */
#include "log.h"

void log_without_unit(void)
{
    LOG_INFO("Hello!\n");
    LOG_INFO("Hello %s!\n", "world");
}
