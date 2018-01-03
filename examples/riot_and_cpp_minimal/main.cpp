/*
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Demonstration of minimal c++ user application with pure c RIOT,
 *              e.g. when no STL or standrd library are available
 *              - using private and public member functions, e.g. 'cpp_obj.greet()' cannot be accessed from main.cpp
 *              - overloading of function 'cpp_obj.say_hello(...)' for 'none', 'int' or 'float'
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
}

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
    thread_create(threadA_stack, sizeof(threadA_stack), 0,
                                        THREAD_CREATE_WOUT_YIELD,
                                        threadA_func, NULL, "thread A");

    printf("******** Hello, you're in thread #%" PRIkernel_pid " ********\n", sched_active_pid);
    printf("We'll test C++ class and methods here!\n");

    cpp_class cpp_obj;
    printf("\n-= Test overloading functions =-\n");
    cpp_obj.say_hello();
    cpp_obj.say_hello(42);
    cpp_obj.say_hello(3.141592f);

    return 0;
}

/* thread A function implemetation */
void *threadA_func(void *)
{
    printf("******** Hello, you're in thread #%" PRIkernel_pid " ********\n", sched_active_pid);
    printf("\nThis demo ends here, press Ctrl-C to exit (if you're on native)!\n");

    return NULL;
}
