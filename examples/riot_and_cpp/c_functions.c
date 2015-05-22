/*
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
 * @file
 * @brief       Some functions implementaion in C.
 *
 * @author      DangNhat Pham-Huu <51002279@hcmut.edu.vn>
 *
 * @}
 */

#include "c_functions.h"

#define IS_LEAP_YEAR(year) ((year%4 == 0) ? ((year%100 == 0) ? ((year%400 == 0) ? 1 : 0) : 1): 0)

int hello(void)
{
    puts("Hello world in a C function");

    return 0;
}

int day_of_week(int day, int month, int year)
{
    int count, sum_of_days;

    /* 1/1/2000 is Saturday */
    int first_day = 6;

    /* check input date */
    if (year < 2000) {
        printf("Sorry buddy, your date should be >= 1/1/2000!\n");
        return -1;
    }

    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
        if (day > 31 || day < 1) {
            printf("Sorry buddy, your date is not correct!\n");
            return -1;
        }
    }
    else if (month == 2) {

        if (IS_LEAP_YEAR(year)) {
            if (day > 29 || day < 1) {
                printf("Sorry buddy, your date is not correct!\n");
                return -1;
            }
        }
        else {
            if (day > 28 ||  day < 1) {
                printf("Sorry buddy, your date is not correct!\n");
                return -1;
            }
        }
    }
    else {
        if (day > 30 || day < 1) {
            printf("Sorry buddy, your date is not correct!\n");
            return -1;
        }
    }

    /* date is okay, convert... */
    sum_of_days = 0;

    for (count = 2000; count < year; count++) {
        sum_of_days += (((IS_LEAP_YEAR(count) == 1) ? 366 : 365)) % 7;
    }

    for (count = 1; count < month; count++) {
        if (count == 1 || count == 3 || count == 5 || count == 7 || count == 8 || count == 10 || count == 12) {
            sum_of_days += 31 % 7;
        }
        else if (count == 2) {

            if (IS_LEAP_YEAR(year)) {
                sum_of_days += 29 % 7;
            }
            else {
                sum_of_days += 28 % 7;
            }
        }
        else {
            sum_of_days += 30 % 7;
        }
    }

    sum_of_days += (day - 1) % 7;

    return (sum_of_days + first_day) % 7;
}
