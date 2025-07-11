/*
 * SPDX-FileCopyrightText: 2014 Hamburg University of Applied Sciences (HAW)
 * SPDX-FileCopyrightText: 2014 Ho Chi Minh city University of Technology (HCMUT)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup
 * @brief
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       simple c++ object declaration with public and private functions
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * @author      DangNhat Pham-Huu <51002279@stu.hcmut.edu.vn>
 */

#include <cstdio>

class cpp_class
{
public:
    /**
     * @brief constructor
     */
    cpp_class();

    /**
     * @brief destructor
     */
    ~cpp_class();

    /**
     * @brief public function
     */
    void say_hello(void);

    /**
     * @brief overloaded public function with int parameter
     */
    void say_hello(int n);

    /**
     * @brief overloaded public function with float parameter
     */
    void say_hello(float f);
private:
    /**
     * @brief private function
     */
    void greet(void);
};

/** @} */
