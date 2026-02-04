/*
 * SPDX-FileCopyrightText: 2020 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       trace module test application
 *
 * This test application tests basic functionality of `sys/trace`.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "trace.h"

int main(void)
{
    trace(0);
    trace(1);

    trace_dump();

    return 0;
}
