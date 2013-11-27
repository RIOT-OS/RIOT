/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file        lifo.c
 * @brief       LIFO buffer implementation
 *
 * @file        lifo.c
 * @brief       LIFO buffer implementation
 * @author      unknown
 * @}
 */

#include <lifo.h>

int lifo_empty(int *array)
{
    return array[0] == -1;
}

void lifo_init(int *array, int n)
{
    for (int i = 0; i <= n; i++) {
        array[i] = -1;
    }
}

void lifo_insert(int *array, int i)
{
    int index = i + 1;
    array[index] = array[0];
    array[0] = i;
}

int lifo_get(int *array)
{
    int head = array[0];

    if (head != -1) {
        array[0] = array[head + 1];
    }

    return head;
}


#ifdef WITH_MAIN
#include <stdio.h>
int main()
{
    int array[5];

    lifo_init(array, 4);

    lifo_insert(array, 0);
    lifo_insert(array, 1);
    lifo_insert(array, 2);
    lifo_insert(array, 3);
    printf("get: %i\n", lifo_get(array));


    return 0;
}
#endif
