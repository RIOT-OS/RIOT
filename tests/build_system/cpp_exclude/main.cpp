/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Sample C++ application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <cstdio>
#include "module.hpp"

int main() {
    puts("Hello from C++");

    module_class obj;
    obj.print_hello();

    return 0;
}
