/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
