#include <stdio.h>
#include "ztimer.h"

void timer_callback(void *arg)
{
    // Convert the argument to a string
    char *message = (char *)arg;

    // Print the message
    puts(message);
}


int main(void)
{
    // Create a timer
    ztimer_t timer = {
        .callback = timer_callback,
        .arg = "3 seconds have passed!"
    };

    // Set the timer to fire in 3 seconds
    ztimer_set(ZTIMER_SEC, &timer, 3);

    // Sleep for 5 seconds
    ztimer_sleep(ZTIMER_SEC, 5);

    puts("5 seconds have passed!");

    return 0;
}
