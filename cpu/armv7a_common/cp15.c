/*
 * Copyright (C) 2017 PDTECTH Co., LTD
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "cp15.h"

/*
 * CR1 bits (CP#15 CR1)
 */
#define CR_M    (1 << 0)        /* MMU enable                           */
#define CR_A    (1 << 1)        /* Alignment abort enable               */
#define CR_C    (1 << 2)        /* Dcache enable                        */
#define CR_W    (1 << 3)        /* Write buffer enable                  */
#define CR_P    (1 << 4)        /* 32-bit exception handler             */
#define CR_D    (1 << 5)        /* 32-bit data address range            */
#define CR_L    (1 << 6)        /* Implementation defined               */
#define CR_B    (1 << 7)        /* Big endian                           */
#define CR_S    (1 << 8)        /* System MMU protection                */
#define CR_R    (1 << 9)        /* ROM MMU protection                   */
#define CR_F    (1 << 10)       /* Implementation defined               */
#define CR_Z    (1 << 11)       /* Branch prediction enable             */
#define CR_I    (1 << 12)       /* Icache enable                        */
#define CR_V    (1 << 13)       /* Vectors relocated to 0xffff0000      */
#define CR_RR   (1 << 14)       /* Round Robin cache replacement        */
#define CR_L4   (1 << 15)       /* LDR pc can set T bit                 */
#define CR_DT   (1 << 16)
#define CR_IT   (1 << 18)
#define CR_ST   (1 << 19)
#define CR_FI   (1 << 21)       /* Fast interrupt (lower latency mode)  */
#define CR_U    (1 << 22)       /* Unaligned access operation           */
#define CR_XP   (1 << 23)       /* Extended page tables                 */
#define CR_VE   (1 << 24)       /* Vectored interrupts                  */
#define CR_EE   (1 << 25)       /* Exception (Big) Endian               */
#define CR_TRE  (1 << 28)       /* TEX remap enable                     */
#define CR_AFE  (1 << 29)       /* Access flag enable                   */
#define CR_TE   (1 << 30)       /* Thumb exception enable               */

static uint32_t __read_p15_c1(void)
{
    uint32_t value;

    __asm__ __volatile__(
        "mrc p15, 0, %0, c1, c0, 0"
        : "=r" (value)
        :
        : "memory");

    return value;
}

static void __write_p15_c1(uint32_t value)
{
    __asm__ __volatile__(
        "mcr p15, 0, %0, c1, c0, 0"
        :
        : "r" (value)
        : "memory");

    __read_p15_c1();
}

void __irq_enable(void)
{
    uint32_t tmp;

    __asm__ __volatile__(
        "mrs %0, cpsr\n"
        "bic %0, %0, #(1<<7)\n"
        "msr cpsr_cxsf, %0"
        : "=r" (tmp)
        :
        : "memory");
}

void __irq_disable(void)
{
    uint32_t tmp;

    __asm__ __volatile__(
        "mrs %0, cpsr\n"
        "orr %0, %0, #(1<<7)\n"
        "msr cpsr_cxsf, %0"
        : "=r" (tmp)
        :
        : "memory");
}

void __fiq_enable(void)
{
    uint32_t tmp;

    __asm__ __volatile__(
        "mrs %0, cpsr\n"
        "bic %0, %0, #(1<<6)\n"
        "msr cpsr_cxsf, %0"
        : "=r" (tmp)
        :
        : "memory");
}

void __fiq_disable(void)
{
    uint32_t tmp;

    __asm__ __volatile__(
        "mrs %0, cpsr\n"
        "orr %0, %0, #(1<<6)\n"
        "msr cpsr_cxsf, %0"
        : "=r" (tmp)
        :
        : "memory");
}

void __icache_enable(void)
{
    uint32_t reg;

    reg = __read_p15_c1();
    __write_p15_c1(reg | CR_I);
}

void __icache_disable(void)
{
    uint32_t reg;

    reg = __read_p15_c1();
    __write_p15_c1(reg & ~CR_I);
}

int __icache_status(void)
{
    return ((__read_p15_c1() & CR_I) != 0);
}

void __dcache_enable(void)
{
    uint32_t reg;

    reg = __read_p15_c1();
    __write_p15_c1(reg | CR_C);
}

void __dcache_disable(void)
{
    uint32_t reg;

    reg = __read_p15_c1();
    __write_p15_c1(reg & ~CR_C);
}

int __dcache_status(void)
{
    return ((__read_p15_c1() & CR_C) != 0);
}

void __wbuffer_enable(void)
{
    uint32_t reg;

    reg = __read_p15_c1();
    __write_p15_c1(reg | CR_W);
}

void __wbuffer_disable(void)
{
    uint32_t reg;

    reg = __read_p15_c1();
    __write_p15_c1(reg & ~CR_W);
}

int __wbuffer_status(void)
{
    return ((__read_p15_c1() & CR_W) != 0);
}

void __mmu_enable(void)
{
    uint32_t reg;

    reg = __read_p15_c1();
    __write_p15_c1(reg | CR_M);
}

void __mmu_disable(void)
{
    uint32_t reg;

    reg = __read_p15_c1();
    __write_p15_c1(reg & ~CR_M);
}

int __mmu_status(void)
{
    return ((__read_p15_c1() & CR_M) != 0);
}

void __vic_enable(void)
{
    uint32_t reg;

    reg = __read_p15_c1();
    __write_p15_c1(reg | CR_VE);
}

void __vic_disable(void)
{
    uint32_t reg;

    reg = __read_p15_c1();
    __write_p15_c1(reg & ~CR_VE);
}

int __vic_status(void)
{
    return ((__read_p15_c1() & CR_VE) != 0);
}

void __branch_enable(void)
{
    uint32_t reg;

    reg = __read_p15_c1();
    __write_p15_c1(reg | CR_Z);
}

void __branch_disable(void)
{
    uint32_t reg;

    reg = __read_p15_c1();
    __write_p15_c1(reg & ~CR_Z);
}

int __branch_status(void)
{
    return ((__read_p15_c1() & CR_Z) != 0);
}

void __ttb_set(uint32_t base)
{
    __asm__ __volatile__(
        "mcr p15, 0, %0, c2, c0, 0"
        :
        : "r" (base)
        : "memory");
}

uint32_t __ttb_get(void)
{
    uint32_t base;

    __asm__ __volatile__(
        "mrc p15, 0, %0, c2, c0, 0"
        : "=r" (base)
        :
        : "memory");

    return base;
}

void __domain_set(uint32_t domain)
{
    __asm__ __volatile__(
        "mcr p15, 0, %0, c3, c0, 0"
        :
        : "r" (domain)
        : "memory");
}

uint32_t __domain_get(void)
{
    uint32_t domain;

    __asm__ __volatile__(
        "mrc p15, 0, %0, c3, c0, 0"
        : "=r" (domain)
        :
        : "memory");

    return domain;
}

void __tlb_invalidate(void)
{
    __asm__ __volatile__(
        "mov r0, #0\n"
        "mcr p15, 0, r0, c7, c10, 4\n"
        "mcr p15, 0, r0, c8, c6, 0\n"
        "mcr p15, 0, r0, c8, c5, 0\n"
        :
        :
        : "r0");
}

uint32_t __get_cpuid(void)
{
    uint32_t tmp;

    __asm__ __volatile__(
        "mrc     p15,0,%0,c0,c0,5\n"
        "and     %0,%0,#0x3\n"
        : "=r" (tmp)
        :
        : "memory");
    return tmp;
}
