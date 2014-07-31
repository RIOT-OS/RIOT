/**
 * CPU specific functions for the RIOT scheduler on NXP LPC1768
 *
 * Copyright (C) 2013 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @file   cpu.c
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @author Oliver Hahm <oliver.hahm@inria.fr>
 */

#include <stdint.h>
#include "cpu.h"
#include "kernel.h"

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

unsigned enableIRQ(void)
{
    unsigned int uiPriMask = __get_PRIMASK();
    __enable_irq();
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
    asm("push   {r4-r11}");
    /* save unsaved registers */
    asm("push   {LR}");
    /* save exception return value */

    asm("ldr     r1, =sched_active_thread");
    /* load address of currend pdc */
    asm("ldr     r1, [r1]");
    /* deref pdc */
    asm("str     sp, [r1]");
    /* write sp to pdc->sp means current threads stack pointer */
}

void restore_context(void)
{
    asm("ldr     r0, =sched_active_thread");
    /* load address of currend pdc */
    asm("ldr     r0, [r0]");
    /* deref pdc */
    asm("ldr     sp, [r0]");
    /* load pdc->sp to sp register */

    asm("pop        {r0}");
    /* restore exception retrun value from stack */
    asm("pop        {r4-r11}");
    /* load unloaded register */
//  asm("pop        {r4}"); /*foo*/
    asm("bx     r0");               /* load exception return value to pc causes end of exception*/
                            /* {r0-r3,r12,LR,PC,xPSR} are restored automatically on exception return */
}

#define USR_RESET   (0x102)
#define SWI         (0xAB)

__attribute__((naked,noreturn)) void arm_reset(void)
{
     int value;

     asm volatile (
        "mov r0, %1"          "\n\t"
        "mov r1, %2"          "\n\t"
        "bkpt" " %a3"   "\n\t"
        "mov %0, r0"
        : "=r" (value)                                         /* output operands             */
        : "r" USR_RESET, "r" NULL, "i" SWI                     /* input operands              */
        : "r0", "r1", "r2", "r3", "ip", "lr", "memory", "cc"   /* list of clobbered registers */
     );
}

int reboot_arch(int mode)
{
    (void) mode;

    while (1) {
        arm_reset();
    }

    return -1;
}
