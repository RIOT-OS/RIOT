#include <stdint.h>
#include <buzzer.h>
#include <hwtimer.h>
#include <cc430f6137.h>

void buzzer_beep(uint8_t pitch, uint16_t duration)
{
    // Reset TA1R, set up mode, TA1 runs from 32768Hz ACLK
    TA1CTL = TACLR | MC_1 | TASSEL__ACLK;

    // Set PWM frequency
    TA1CCR0 = pitch;

    // Enable IRQ, set output mode "toggle"
    TA1CCTL0 = OUTMOD_4;

    // Allow buzzer PWM output on P2.7
    P2SEL |= BIT7;

    hwtimer_wait(duration);

    // Stop PWM timer
    TA1CTL &= ~(BIT4 | BIT5);

    // Reset and disable buzzer PWM output
    P2OUT &= ~BIT7;
    P2SEL &= ~BIT7;
    TA1CCTL0 &= ~CCIE;
}
