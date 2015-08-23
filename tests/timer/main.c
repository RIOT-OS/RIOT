/*
 * Copyright (C) 
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief timer test application
 *
 * @author      Gebremedhin Abreha <gete.kal@gmail.com>
 * 
 *
 * @}
 */

#include <stdio.h>
#include <unistd.h>  /*usleep(), abs()*/

#include "hwtimer.h" /*HWTIMER_SPEED */
#include "periph_conf.h" /*TIMER_NUMOF defined here */
#include "periph/timer.h"
//#include "vtimer.h"  /*vtimer_usleep() */

#define TIMER_SPEED HWTIMER_SPEED   /* value dependes on the type of platform architecture */
#define TIMER_MAXTIMERS TIMER_NUMOF 
/*#define TIMER_MAXTIMERS HWTIMER_MAXTIMERS */
//#define US_PER_TICK (1000000L/TIMER_SPEED)  
#define WAIT (1000000U)
#define TIMEOUT (100U)  
#define DEV TIMER_0
#define CHAN 0       /*TODO: define for the native, (example, TIMER_0_CHANNELS) */

/**
 * @brief       number of us passed for one timer tick
 * 
 * @return      microseconds
 */

#if TIMER_SPEED > 1000000L
#define US_PER_TICK        (TIMER_SPEED / 1000000L)
#else
#define US_PER_TICK        (1000000L / TIMER_SPEED)
#endif

int test_extra (tim_t);

void callback(int val)   /* callback function/ISR */
{  
   printf("callback %d\n",val);
}

int main(void)
{   
    int init, set, clear;
    //int extra;

    puts("**************************************");
    puts("Timer test application...");
    puts("--------------------------------------");
    puts("If test is successful, you should not see any error messages.");
    puts("Then the message \"callback 0\" should be printed three times.");
    puts("The first message should be printed immediatly (100 us) and the next two messages should be printed after a delay of 2 (approximate) seconds.");
    puts(""); 
    
    
    /*****test for initializing each timers *******/

    for(unsigned int i=0; i<TIMER_MAXTIMERS; i++) {
        init = timer_init(i, US_PER_TICK, callback);
	if(init != 0) {
            printf(" Error initializing: timer_dev_id=%u\n", i);
            return -1;
        }
    }
    
   
    /********************************************************************************\
     *In the following, test for timer device 0 (TIMER_0) and channel 0 (?) is considered: 
     *It can be extented to test all timer devices on a given platform
    \******************************************************************************/

    /**********test for stoping and starting timer (NOTE: this test fails)*********/

    /*******************************************************************************\
     *Unexpected behaviours with timer_stop(), timer_star(), timer_reset()
     * observed during development or did not implemented yet, and test fails. 
     *it can be checked by uncommenting the following code block
    \******************************************************************************/
    /* 
    extra = test_extra(DEV);
    if(extra != 1) {
        puts(" Error while trying to stop and start timer");
        return -1;
    }
    */
    /************testing timer_set***********************************************/
    
    set = timer_set(DEV, CHAN, TIMEOUT);   /*fires after TIMEOUT (almost immediatly--100 us) */
    if (set != 1) {
        puts(" Error while trying to set timer");
        return -1;
    }
    
   
    /*******test for timer_clear  *********************/
 
    usleep(WAIT); /*wait until previous interrupt (time_set) fires before overwriting by the next timer_set*/
    timer_set(DEV,CHAN, TIMEOUT); /*this interrupt should not fire, it will be cleared before it fires */
    clear = timer_clear(DEV, CHAN);
    if(clear != 1) {
        puts(" Error while trying to clear timer");
        return -1;
    }
    /*******timer_irg_disable, timer_irq_enable ***************/
    timer_irq_disable(DEV);  /*disables the next interrupt */
    timer_set(DEV, CHAN, TIMEOUT); /* fires if enabled by timer_irq_enable() (in this case, after (TIMEOUT + WAIT) us ~=1 sec), 
                                  otherwise it will stay disabled  */
    usleep(WAIT);              
    timer_irq_enable(DEV);/*enables the timer interrupt and restores interrupts that have been issued previously */ 
    timer_set(DEV, CHAN, TIMEOUT); /*fires after TIMEOUT (almost imediatly)  */                            
    
    return 0;
}

/**
 * @brief proposed test for timer_stop() and timer_start()
 *
 *-There is no real implementation of timer_stop() and timer_start() for the native platform yet. 
 * Right now, they are only dummy methods and do nothing. 
 *-Assuming that they will be implemented in the future, I would propose the following method 
 * for testing the methods.
 *
 * @param[in] dev           the timer device to test
 *
 * @return                  1 on success
 * @return                  -1 on error
 */
     
int test_extra (tim_t dev) 
{
    unsigned int last, after_stop,after_start;
      
    timer_stop(dev);
    last = timer_read(dev);
    usleep(WAIT);
    after_stop = timer_read(dev);
    if (!(last == after_stop)){
        return -1;
    }
    timer_start(dev);
    usleep(WAIT);
    after_start = timer_read(dev);
    if (after_stop == after_start) {  /* assuming the timer count value takes atleast more than 1 second to overflow and then restart counting up */ 
        return -1;
    }
    return 1;
}



