/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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

class module_class
{
public:
    /**
     * @brief constructor
     */
    module_class();

    /**
     * @brief destructor
     */
    ~module_class();

    /**
     * @brief public function
     */
    void print_hello(void);
};

/** @} */
