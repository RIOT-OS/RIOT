//#include <stdio.h>
//#include "trace.h"

#include "led.c"

int main(void) {
    printf("Hello world!\n");

    int i = 0;
    while(1) {
    	i++;
		jumpToDummyFunction();
		jumpWithoutProfileFunction();
		if (i >= 20) {
				break;
		}
    }

    return 0;
}


void jumpToDummyFunction() {

        printf("This is a dummy.\n");
}

void __attribute__((__no_instrument_function__)) jumpWithoutProfileFunction() {
        printf("no profile\n");
}

