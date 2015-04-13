#include <stdio.h>

#include "hwtimer.h"
#include "thread.h"
#include "ps.h"
#include "board.h"

int main(void) {

    LED_RED_OFF;
    LED_GREEN_OFF;
    LED_ORANGE_OFF;

    while (1) {
        hwtimer_wait(HWTIMER_TICKS(100 * 1000));
        LED_RED_TOGGLE;
        hwtimer_wait(HWTIMER_TICKS(200 * 1000));
        LED_GREEN_TOGGLE;
        hwtimer_wait(HWTIMER_TICKS(400 * 1000));
        LED_ORANGE_TOGGLE;
    }
    return 0;
}
