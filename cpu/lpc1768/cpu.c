/**
 * CPU specific functions for the RIOT scheduler on NXP LPC1768 
 *
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @file   cpu.c
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @author Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <stdint.h>
#include "cpu.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

int inISR(void)
{
	return (__get_IPSR() & 0xFF);
}

unsigned int disableIRQ(void)
{
	// FIXME PRIMASK is the old CPSR (FAULTMASK ??? BASEPRI ???)
	//PRIMASK lesen
	unsigned int uiPriMask = __get_PRIMASK();
	__disable_irq();
	return uiPriMask;
}

void restoreIRQ(unsigned oldPRIMASK)
{
	//PRIMASK lesen setzen
	 __set_PRIMASK(oldPRIMASK);
}


__attribute__((naked))
void HardFault_Handler(void) {
  DEBUG("HARD FAULT\n");
  while(1);
}

__attribute__((naked))
void BusFault_Handler(void) {
  DEBUG("BusFault_Handler\n");
  while(1);
}

__attribute__((naked))
void Usage_Handler(void) {
  DEBUG("Usage FAULT\n");
  while(1);
}

__attribute__((naked))
void WWDG_Handler(void) {
  DEBUG("WWDG FAULT\n");
  while(1);
}

void dINT(void)
{
  __disable_irq();
}

void eINT(void)
{
  __enable_irq();
}

void save_context(void)
{
	/* {r0-r3,r12,LR,PC,xPSR} are saved automatically on exception entry */
	asm("push 	{r4-r11}");
	/* save unsaved registers */
	asm("push 	{LR}");
	/* save exception return value */

	asm("ldr     r1, =active_thread");
	/* load address of currend pdc */
	asm("ldr     r1, [r1]");
	/* deref pdc */
	asm("str     sp, [r1]");
	/* write sp to pdc->sp means current threads stack pointer */
}

void restore_context(void)
{
	asm("ldr     r0, =active_thread");
	/* load address of currend pdc */
	asm("ldr     r0, [r0]");
	/* deref pdc */
	asm("ldr     sp, [r0]");
	/* load pdc->sp to sp register */

	asm("pop		{r0}");
	/* restore exception retrun value from stack */
	asm("pop		{r4-r11}");
	/* load unloaded register */
//	asm("pop 		{r4}"); /*foo*/
	asm("bx		r0");				/* load exception return value to pc causes end of exception*/
							/* {r0-r3,r12,LR,PC,xPSR} are restored automatically on exception return */
}
