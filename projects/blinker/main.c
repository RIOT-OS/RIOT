//#include <stdio.h>
//#include <math.h>
#include <board.h>
#include "hwtimer.h"

void wait(void)
{ /* simple delay function */
	volatile int i;        /* declare i as volatile int */
	for(i = 0; i < 32000; i++);                      /* repeat 32000 times (nop)  */
}


int main(void)
{
	for(;;)
	{ /* infinite loop */
		LED_RED_ON;
		wait();              /* call delay function       */
		LED_RED_OFF;
		wait();
	}
}

