#ifndef HWTIMER_CPU_H_
#define HWTIMER_CPU_H_

#include "periph_conf.h"

#if TIMER_2_EN == 0
// This causes EPIT1 to be used for sys-time.
// Without, time-related functions like the delay or gettimeofday
// wont work
#define USE_SDK_SYS_TIMER
#else
/*  TODO Is hal_delay used anywhere? Must there be an alternative implementation? */
#endif

#if TIMER_0_EN == 1
	#define PROXY_TIMER_0_CHANNELS TIMER_0_CHANNELS
#else
	#define PROXY_TIMER_0_CHANNELS 0
#endif
#if TIMER_1_EN == 1
	#define PROXY_TIMER_1_CHANNELS TIMER_1_CHANNELS
#else
	#define PROXY_TIMER_1_CHANNELS 0
#endif
#if TIMER_2_EN == 1
	#define PROXY_TIMER_2_CHANNELS TIMER_2_CHANNELS
#else
	#define PROXY_TIMER_2_CHANNELS 0
#endif

#define HWTIMER_MAXTIMERS (PROXY_TIMER_0_CHANNELS + PROXY_TIMER_1_CHANNELS + PROXY_TIMER_2_CHANNELS)

#define HWTIMER_SPEED       1000000         /**< the HW timer runs with 1MHz (actually, this depends on the coice for clocksource) */
#define HWTIMER_MAXTICKS    (0xFFFFFFFF) 

#endif /* HWTIMER_CPU_H_ */
