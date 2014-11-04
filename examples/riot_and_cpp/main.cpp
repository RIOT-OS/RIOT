/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences (HAW)
 * Copyright (C) 2014 Ho Chi Minh University of Technology (HCMUT)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file        main.cpp
 * @brief       Demonstration of mixed c++ and c user application with pure c RIOT
 *              - mixing of c and c++ source to test name mangling
 *              - introducing a namespace to declarative block, avoiding to qualify calls, e.g. std::vector
 *              - using private and public member functions, e.g. 'cpp_obj.greet()' cannot be accessed from main.cpp
 *              - overloading of function 'cpp_obj.say_hello(...)' for 'none', 'int' or 'float'
 *              - demonstration of templated c++ container 'std::vector'
 *              - usage of iterator to access elements of the container type
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * @author      DangNhat Pham-Huu <51002279@hcmut.edu.vn>
 */

/*
 * all included headers defining c functions, i.e. all RIOT functions, must be marked as extern "C"
 */
extern "C" {
#include "thread.h"

#include "c_functions.h"
}

#include <cstdio>
#include <vector>

#include "cpp_class.hpp"

using namespace std;


/* main */
int main()
{
    printf("\n************ RIOT and C++ demo program ***********\n");
    printf("******** Hello, you're in thread #%" PRIkernel_pid " ********\n", sched_active_pid);
    printf("The vector vInts has been filled with %d numbers.\n", (int)vInts.size());

    return 0;
}

    printf("******** Hello, you're in thread #%" PRIkernel_pid " ********\n", sched_active_pid);