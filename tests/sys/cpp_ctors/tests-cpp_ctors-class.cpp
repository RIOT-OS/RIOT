/*
 * SPDX-FileCopyrightText: 2016-2017 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "tests-cpp_ctors.h"

volatile long tests_cpp_ctors_magic1 = 12345;
volatile long tests_cpp_ctors_magic2 = 11111111;
void *tests_cpp_ctors_order[8];

namespace RIOTTestCPP {

MyClass::MyClass()
{
    var = tests_cpp_ctors_magic1;
}

MyClass::MyClass(long value) : var(value)
{
}

long MyClass::value()
{
    return var;
}

void MyClass::inc()
{
    ++var;
}

} /* namespace RIOTTestCPP */
