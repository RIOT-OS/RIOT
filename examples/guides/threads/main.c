#include <stdio.h>

#include "thread.h"

char my_thread_stack[THREAD_STACKSIZE_MAIN];

void *my_first_thread(void *arg) {
  (void)arg;  // Variable not used

  puts("Hello, from the thread!");

  return NULL;
}

int main(void) {
    thread_create(my_thread_stack, sizeof(my_thread_stack),
            THREAD_PRIORITY_MAIN - 1, 0, my_first_thread, NULL,
            "My first thread");

    puts("Hello, from the main thread!");
}
