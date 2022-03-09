/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Freie Universität Berlin
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
