/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Sample module C++ class
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <cstdio>

#include "module.hh"

module_class::module_class() {}

module_class::~module_class() {}

void module_class::print_hello(void)
{
    puts("Hello from C++ module");
}
