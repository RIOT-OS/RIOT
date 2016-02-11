/*
 * Copyright (C) 2015  Kushal Singh <kushal.spiderman.singh@gmail.com
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
 * @brief       A basic example for dynamic linking in RIOT
 *
 * @author      Kushal Singh <kushal.spiderman.singh@gmail.com>
 *
 * @}
 */

volatile unsigned int test_var=4;

int test_func1(void)
{
    return 1;
}

int test_func2(int test_parm)
{
    return test_parm+1;
}

int test_val(void)
{
    return test_func1() + test_func2(3) + test_var;
}

int app(void)
{
   int ret_val = test_val();

   return ret_val;;
}
