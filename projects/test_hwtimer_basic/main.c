#include <stdio.h>
#include <hwtimer.h>
#include <kernel.h>
#include <board.h>

void callback(void* ptr) {
    puts((char*)ptr);       
}

extern uint32_t hwtimer_now();

int main(void)
{
    puts("hwtimer test project.");

    puts("Initializing hwtimer...");
    hwtimer_init();

    puts("Initializing hwtimer [OK].");
   

//    while (TA0R < 20000);

    hwtimer_set(20000LU, callback, (void*)"callback1");
    hwtimer_set(50000LU, callback, (void*)"callback2");
    hwtimer_set(30000LU, callback, (void*)"callback3");
    
    puts("hwtimer set.");
}
