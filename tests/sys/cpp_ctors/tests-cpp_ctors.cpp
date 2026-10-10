/*
 * SPDX-FileCopyrightText: 2016-2017 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "tests-cpp_ctors.h"

using RIOTTestCPP::MyClass;

MyClass global_obj;

extern "C" {
long tests_cpp_ctors_global_value(void);
long tests_cpp_ctors_static_value(void);
long tests_cpp_ctors_local_value(long);
}
extern volatile long tests_cpp_ctors_magic2;

long tests_cpp_ctors_local_value(long number) {
    MyClass local_obj(number);
    local_obj.inc();
    return local_obj.value();
}

long tests_cpp_ctors_static_value() {
    static MyClass static_obj(tests_cpp_ctors_magic2);
    static_obj.inc();

    return static_obj.value();
}

long tests_cpp_ctors_global_value() {
    return global_obj.value();
}
