/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>

#include "thread.h"

/*
 * Define a stack for the thread.
 * The size of the stack is defined by THREAD_STACKSIZE_MAIN.
 */
char my_thread_stack[THREAD_STACKSIZE_MAIN];

void *my_first_thread(void *arg)
{
    /* The argument we receive will not be used */
    (void)arg;

    /* We print a simple message from the thread */
    puts("Hello, from the thread!");

    /* We return NULL to indicate that the thread has finished */
    return NULL;
}

int main(void)
{
    /* Create a thread with the stack we defined above */
    thread_create(my_thread_stack, sizeof(my_thread_stack),
                  THREAD_PRIORITY_MAIN - 1, 0, my_first_thread, NULL,
                  "My first thread");

    /* The main thread can continue doing its work (e.g., printing a message) */
    puts("Hello, from the main thread!");
}
