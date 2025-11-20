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

void log_with_unit(void);
void log_without_unit(void);

int main(void)
{
    log_with_unit();
    log_without_unit();

    return 0;
}
