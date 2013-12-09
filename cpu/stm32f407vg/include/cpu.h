/**
 *
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @file
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 *
 */

#ifndef __CPU_H
#define __CPU_H

#include <stdbool.h>
#include "stm32f4xx.h"


//#define F_CCO					288000000
//#define CL_CPU_DIV				4									///< CPU clock divider
#define F_CPU					168000000						///< CPU target speed in Hz
//#define F_RC_OSCILLATOR			4000000								///< Frequency of internal RC oscillator
//#define F_RTC_OSCILLATOR		32767								///< Frequency of RTC oscillator
//#define PCLK_DIV                0x2                                 ///< PCLK clock divider, F_CPU/PCLK_DIV=PCLK

#define VIC_SIZE                32

#define GPIO_INT 17
#define IRQP_GPIO 4



void thread_yield(void);
void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t* prescale);

__attribute__( ( always_inline ) ) static __INLINE int inISR(void)
{
	return (__get_IPSR() & 0xFF);
}

__attribute__( ( always_inline ) ) static __INLINE unsigned int disableIRQ(void)
{
	// FIXME PRIMASK is the old CPSR (FAULTMASK ??? BASEPRI ???)
	//PRIMASK lesen
	unsigned int uiPriMask = __get_PRIMASK();
	__disable_irq();
	return uiPriMask;
}

__attribute__( ( always_inline ) ) static __INLINE void restoreIRQ(unsigned oldPRIMASK)
{
	//PRIMASK lesen setzen
	__set_PRIMASK(oldPRIMASK);
}

__attribute__( ( always_inline ) ) static __INLINE void dINT(void)
{
	__disable_irq();
}

__attribute__( ( always_inline ) ) static __INLINE void eINT(void)
{
	__enable_irq();
}

__attribute__( ( always_inline ) ) static __INLINE void save_context(void)
{
	/* {r0-r3,r12,LR,PC,xPSR} are saved automatically on exception entry */

	/* save unsaved registers */
	asm("mrs r0, psp"); 			// get stack pointer from user mode
	asm("stmdb r0!,{r4-r11}");// save regs
	asm("stmdb r0!,{lr}");// exception return value
	/*vstmdb	sp!, {s16-s31}	*///FIXME save fpu registers if needed
	/* load address of currend pdc */
	asm("ldr     r1, =active_thread");
	/* deref pdc */
	asm("ldr     r1, [r1]");
	/* write r0 to pdc->sp means current threads stack pointer */
	asm("str     r0, [r1]");

}

__attribute__( ( always_inline ) ) static __INLINE void restore_context(void)
{
	/* load address of currend pdc */
	asm("ldr     r0, =active_thread");
	/* deref pdc */
	asm("ldr     r0, [r0]");
	/* load pdc->sp to register 1 */
	asm("ldr     r1, [r0]");

	/* restore exception retrun value from stack */
	asm("ldmia r1!,	{r0}");

	/*pop		{s16-s31}		*/ //FIXME load fpu register if needed depends on r0 exret
	/* restore other registers */
	asm("ldmia r1!, {r4-r11}");

	/* restore PSP register (user mode sp)*/
	asm("msr psp, r1");

	asm("bx		r0"); /* load exception return value to pc causes end of exception*/
	/* {r0-r3,r12,LR,PC,xPSR} are restored automatically on exception return */
}

__attribute__( ( always_inline ) ) static __INLINE void interrupt_entry(void)
{
	/* {r0-r3,r12,LR,PC,xPSR} are saved automatically on exception entry */
	asm("push 	{LR}");
	/* save exception return value */
}

__attribute__( ( always_inline ) ) static __INLINE void interrupt_return(void)
{
	asm("pop		{r0}");
	/* restore exception retrun value from stack */
//	asm("pop 		{r4}"); /*foo*/
	asm("bx		r0"); /* load exception return value to pc causes end of exception*/
	/* {r0-r3,r12,LR,PC,xPSR} are restored automatically on exception return */
}
/** @} */
#endif /* __CPU_H */
