/*
 * Copyright (C) 2021 Otto-von-Guericke Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test auto-initialization in custom order
 *
 * @author      Fabian Hüßler <fabian@huessler.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include "auto_init.h"
#include "auto_init_priorities.h"
#include "ztimer.h"

static unsigned _counter;
static char _init[6];

void my_module_a_init(void)
{
    _init[_counter++] = 'a';
}
void my_module_b_init(void)
{
    _init[_counter++] = 'b';
}
void my_module_c_init(void)
{
    _init[_counter++] = 'c';
}
void my_module_d_init(void)
{
    ztimer_sleep(ZTIMER_MSEC, 5);
    _init[_counter++] = 'd';
}
void my_module_e_init(void)
{
    ztimer_sleep(ZTIMER_MSEC, 5);
    _init[_counter++] = 'e';
}
void my_module_f_init(void)
{
    ztimer_sleep(ZTIMER_MSEC, 5);
    _init[_counter++] = 'f';
}

#define AUTO_INIT_PRIO_MY_GROUP_BEFORE  AUTO_INIT_PRIO_ADD_BEFORE_GROUP(AUTO_INIT_PRIO_GP_CORE, AUTO_INIT_PRIO_1)
#define AUTO_INIT_PRIO_MY_MODULE_A      AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_MY_GROUP_BEFORE, AUTO_INIT_PRIO_1)
#define AUTO_INIT_PRIO_MY_MODULE_B      AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_MY_GROUP_BEFORE, AUTO_INIT_PRIO_2)
AUTO_INIT_MODULE(my_module_a, AUTO_INIT_PRIO_MY_MODULE_A, my_module_a_init);
AUTO_INIT_MODULE(my_module_b, AUTO_INIT_PRIO_MY_MODULE_B, my_module_b_init);

#define AUTO_INIT_PRIO_MY_MODULE_C  AUTO_INIT_PRIO_ADD_BEFORE_MODULE(AUTO_INIT_PRIO_GP_TIMERS,  \
                                                                     AUTO_INIT_PRIO_ZTIMER,     \
                                                                     AUTO_INIT_PRIO_1)
AUTO_INIT_MODULE(my_module_c, AUTO_INIT_PRIO_MY_MODULE_C, my_module_c_init);

#define AUTO_INIT_PRIO_MY_MODULE_D  AUTO_INIT_PRIO_ADD_AFTER_MODULE(AUTO_INIT_PRIO_GP_TIMERS,   \
                                                                    AUTO_INIT_PRIO_ZTIMER,      \
                                                                    AUTO_INIT_PRIO_1)
AUTO_INIT_MODULE(my_module_d, AUTO_INIT_PRIO_MY_MODULE_D, my_module_d_init);

#define AUTO_INIT_PRIO_MY_GROUP_AFTER   AUTO_INIT_PRIO_ADD_AFTER_GROUP(AUTO_INIT_PRIO_GP_CORE, AUTO_INIT_PRIO_1)
#define AUTO_INIT_PRIO_MY_MODULE_E      AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_MY_GROUP_AFTER, AUTO_INIT_PRIO_1)
#define AUTO_INIT_PRIO_MY_MODULE_F      AUTO_INIT_PRIO_ADD(AUTO_INIT_PRIO_MY_GROUP_AFTER, AUTO_INIT_PRIO_2)
AUTO_INIT_MODULE(my_module_e, AUTO_INIT_PRIO_MY_MODULE_E, my_module_e_init);
AUTO_INIT_MODULE(my_module_f, AUTO_INIT_PRIO_MY_MODULE_F, my_module_f_init);

int main(void)
{
    if (_init[0] == 'a' && _init[1] == 'b' &&
        _init[2] == 'c' && _init[3] == 'd' &&
        _init[4] == 'e' && _init[5] == 'f') {
        puts("Success");
    }
    else {
        puts("Failure");
    }
}
