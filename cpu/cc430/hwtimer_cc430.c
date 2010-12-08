#include <signal.h>
#include <board.h>
#include <hwtimer.h>
#include <hwtimer_arch.h>
#include <cpu.h>

static uint32_t ticks = 0;

extern void (*int_handler)(int);
extern void TA0_unset(short timer);

void timerA_init()
{
  	ticks = 0;								 // Set tick counter value to 0
	TA0CTL = TASSEL_1 + TACLR;   	  		 // Clear the timer counter, set ACLK
	TA0CTL &= ~TAIE;						 // Clear the IFG
    
    volatile unsigned int *ccr = &TA0CCR0;
    volatile unsigned int *ctl = &TA0CCTL0;

    for (int i = 0; i < ARCH_MAXTIMERS; i++) {
        *(ccr+i) = 0;
        *(ctl+i) &= ~(CCIFG);
        *(ctl+i) &= ~(CCIE);
    }

    TA0CTL |= MC_2;
}

interrupt(TIMER0_A0_VECTOR) __attribute__ ((naked)) timer0_a0_isr(void) {
    __enter_isr();
	
    TA0_unset(0); 
    int_handler(0);
    __exit_isr();
}

interrupt(TIMER0_A1_VECTOR) __attribute__ ((naked)) timer0_a1_5_isr(void) {
    __enter_isr();
    
    short taiv = TA0IV;
    short timer;

    if (!(taiv & TAIFG)) {
//        case TAIFG:
            timer = (taiv/2);
            TA0_unset(timer);
            int_handler(timer);
  /*          break;
    
		// Timer0_A3	Configurable periodic IRQ (used by button_repeat and buzzer)			
		case 0x06:	// Disable IE 
					TA0CCTL3 &= ~CCIE;
					// Reset IRQ flag  
					TA0CCTL3 &= ~CCIFG;  
					// Enable timer interrupt    
					TA0CCTL3 |= CCIE; 	
					// Call function handler
                    // TODO
					break;
		
		// Timer0_A4	One-time delay			
		case 0x08:	// Disable IE 
					TA0CCTL4 &= ~CCIE;
					// Reset IRQ flag  
					TA0CCTL4 &= ~CCIFG;  
					break;*/
	}
	
    __exit_isr();
}
