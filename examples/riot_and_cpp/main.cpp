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

/* thread's stack */
char threadA_stack [THREAD_STACKSIZE_MAIN];

/* thread's function */
void *threadA_func(void *arg);

/* main */
int main()
{
    printf("\n************ RIOT and C++ demo program ***********\n");
    printf("\n");

    /* create thread A */
    thread_create(threadA_stack, sizeof(threadA_stack), 0, CREATE_WOUT_YIELD, threadA_func, NULL, "thread A");

    printf("******** Hello, you're in thread #%" PRIkernel_pid " ********\n", sched_active_pid);
    printf("We'll test C++ class and methods here!\n");

    cpp_class cpp_obj;
    printf("\n-= Test overloading functions =-\n");
    cpp_obj.say_hello();
    cpp_obj.say_hello(42);
    cpp_obj.say_hello(3.141592f);

    printf("\n-= Test namespace =-\n");
    printf("typing std::vector is obsolete when 'using namespace std;'\n");
    vector<int> vInts;
    vInts.push_back(1);
    vInts.push_back(3);
    vInts.push_back(2);
    printf("The vector vInts has been filled with %d numbers.\n", (int)vInts.size());

    printf("\n-= Test iterator =-\n");
    printf("The content of vInts = { ");

    for (vector<int>::iterator it = vInts.begin(); it != vInts.end(); ++it) {
        printf("%d ", *(it));
    }

    printf("}\n");

    return 0;
}

/* thread A function implemetation */
void *threadA_func(void *)
{
    int day = 13, month = 6, year = 2014;
    int ret_day;

    printf("******** Hello, you're in thread #%" PRIkernel_pid " ********\n", sched_active_pid);
    printf("We'll test some C functions here!\n");

    printf("\n-= hello function =-\n");
    hello();

    printf("\n-= day_of_week function =-\n");

    printf("day %d, month %d, year %d is ", day, month, year);

    ret_day = day_of_week(day, month, year);
    if (ret_day >= 0){
        char day_of_week_table[][32] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
        printf("%s\n", day_of_week_table[ret_day]);
    }

    printf("\nThis demo ends here, press Ctrl-C to exit (if you're on native)!\n");

    return NULL;
}
