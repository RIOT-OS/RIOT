/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test case for assert.h
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <assert.h>

int main(void)
{
    printf("assert test app\n");

    int i = 1;

    printf("testing positive assertion: (shouldn't print anything)\n");

    assert(i==1);

    printf("testing negative assertion: (should print an error unless compiled with -DNDEBUG)\n");

    assert(i==0);

    printf("Test complete\n");

    return 0;
}
