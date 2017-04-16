/*
 * Copyright (C) 2015
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
 * @brief       Satisfy LGPL example application
 *
 * @author
 *
 * @}
 */

#include <stdio.h>
#include "mutex.h"

int main(void)
{
    puts("Hello LGPL!");
    mutex_t mtx;
    mutex_init(&mtx);
    mutex_lock(&mtx);

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    mutex_unlock(&mtx);
    return 0;
}
