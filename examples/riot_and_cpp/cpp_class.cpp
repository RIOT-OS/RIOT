/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences (HAW)
 * Copyright (C) 2014 Ho Chi Minh city University of Technology (HCMUT)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file        cpp_class.cpp
 * @brief       implementation of declared functions of object cpp_class
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * @author      DangNhat Pham-Huu <51002279@hcmut.edu.vn>
 *
 * @}
 */

#include "cpp_class.h"

cpp_class::cpp_class()
{
    printf("Instanciating Object [constructor called]\n");
    greet();
}

cpp_class::~cpp_class()
{
    printf("Destroying Object [destructor called]\n");
    printf("Im shutting down!\n");
}

void cpp_class::say_hello(void)
{
    printf("Hello!\n");
}

void cpp_class::say_hello(int n)
{
    printf("Hello![int: %d]\n", n);
}

void cpp_class::say_hello(float f)
{
    printf("Hello![float: %f]\n", f);
}

void cpp_class::greet(void)
{
    printf("Im starting!\n");
}
