/*
 * lpc2387_timer_0_1.h
 *
 *  Created on: 15.01.2009
 *      Author: heiko, kaspar
 *
 *      Changelog:
 *                  26.01.09 kaspar: renamed file, misc changes for firekernel
 */

#ifndef HWTIMER_CPU_H_
#define HWTIMER_CPU_H_

#define HWTIMER_MAXTIMERS 4
#define HWTIMER_SPEED 1000000
#define HWTIMER_MAXTICKS (0xFFFFFFFF)

#define HWTIMER_MSEC  (HWTIMER_SPEED/1000)
#define HWTIMER_SEC   (HWTIMER_SPEED)

#endif /* HWTIMER_CPU_H_ */
