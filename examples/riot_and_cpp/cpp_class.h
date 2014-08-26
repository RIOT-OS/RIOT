/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences (HAW)
 * Copyright (C) 2014 Ho Chi Minh city University of Technology (HCMUT)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup
 * @brief
 * @ingroup     examples
 * @{
 *
 * @file        cpp_class.h
 * @brief       simple c++ object declaration with public and private functions
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * @author      DangNhat Pham-Huu <51002279@stu.hcmut.edu.vn>
 */

#ifndef CPP_CLASS_H_
#define CPP_CLASS_H_

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
#endif //__CPP_CLASS__
