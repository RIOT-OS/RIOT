/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * The various non-general purpose registers of x86 CPUs.
 *
 * @ingroup x86
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__REGISTERS__H__
#define CPU__X86__REGISTERS__H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* see "Intel® Quark SoC X1000 Core Developer’s Manual", § 4.4.1.1 (p. 47) */
#define CR0_PE (1u << 0)  /**< 1 = protected mode */
#define CR0_MP (1u << 1)  /**< 1 = monitor coprocessor (FWAIT causes an interrupt) */
#define CR0_EM (1u << 2)  /**< 1 = FPU emulation (x87 instructions cause
                                   #X86_INT_NM, SSE causes #X86_INT_UD) */
#define CR0_TS (1u << 3)  /**< 1 = task switched flag (causes #X86_INT_NM on
                                   x87/SSE instructions, set by CPU on hardware
                                   task switch) */
#define CR0_ET (1u << 4)  /**< 1 = 80387; 0 = 80287 */
#define CR0_NE (1u << 5)  /**< 1 = numeric error */
#define CR0_WP (1u << 16) /**< 1 = write proctected pages aren't writable in ring 0 either */
#define CR0_AM (1u << 18) /**< 1 = alignment mask */
#define CR0_NW (1u << 29) /**< 1 = not write-through */
#define CR0_CD (1u << 30) /**< 1 = disable caching */
#define CR0_PG (1u << 31) /**< 1 = enable paging */

#define CR3_PWT (1u << 3) /**< 1 = page-level writes transparent */
#define CR3_PCD (1u << 4) /**< 1 = page-level cache disabled */

#define CR4_VME        (1u << 0)  /**< 1 = virtual 8086 mode */
#define CR4_PVI        (1u << 1)  /**< 1 = protected-mode virtual interrupts */
#define CR4_TSD        (1u << 2)  /**< 1 = restrict RDTSC instruction to ring 0 */
#define CR4_DE         (1u << 3)  /**< 1 = debugging extension */
#define CR4_PSE        (1u << 4)  /**< 1 = page size extension */
#define CR4_PAE        (1u << 5)  /**< 1 = physical address extension */
#define CR4_MCE        (1u << 6)  /**< 1 = machine-check enable */
#define CR4_PGE        (1u << 7)  /**< 1 = enable G flag in PT */
#define CR4_PCE        (1u << 8)  /**< 1 = allow RDPMC instruction in rings 1-3, too */
#define CR4_OSFXSR     (1u << 9)  /**< 1 = disable #X86_INT_NM if CR0.TS=1 */
#define CR4_OSXMMEXCPT (1u << 10) /**< 1 = enable unmasked SSE exceptions */
#define CR4_SMEP       (1u << 10) /**< 1 = enables supervisor-mode execution prevention */

#define X86_CR_ATTR __attribute__ ((always_inline, no_instrument_function))

/**
 * @brief   Read the Control Register 0, various control flags for the CPU.
 */
static inline uint32_t X86_CR_ATTR cr0_read(void)
{
    uint32_t result;
    __asm__ volatile ("mov %%cr0, %%eax" : "=a"(result));
    return result;
}

/**
 * @brief   Set the Control Register 0, various control flags for the CPU.
 *
 * You must not blank out unknown bits.
 * First use cr0_read(), then set and reset the bit you want to manipulate,
 * then call this function.
 */
static inline void X86_CR_ATTR cr0_write(uint32_t value)
{
    __asm__ volatile ("mov %%eax, %%cr0" :: "a"(value));
}

/**
 * @brief   Read the Page Fault Linear Address.
 *
 * The PFLA is the address which was accessed when the page fauled occured,
 * i.e. this is not the PC of the \#PF!
 */
static inline uint32_t X86_CR_ATTR cr2_read(void)
{
    uint32_t result;
    __asm__ volatile ("mov %%cr2, %%eax" : "=a"(result));
    return result;
}

/**
 * @brief   Read the (physical) address of the page table.
 *
 * You are doing it wrong if you ever find you need to call this function ...
 */
static inline uint32_t X86_CR_ATTR cr3_read(void)
{
    uint32_t result;
    __asm__ volatile ("mov %%cr3, %%eax" : "=a"(result));
    return result;
}

/**
 * @brief   Set the (pyhsical) address of the page table.
 *
 * This flushes the TLB for all pages that do not have the PT_G flag.
 */
static inline void X86_CR_ATTR cr3_write(uint32_t value)
{
    __asm__ volatile ("mov %%eax, %%cr3" :: "a"(value));
}

/**
 * @brief   Set the Control Register 4, various control flags for the CPU.
 */
static inline uint32_t X86_CR_ATTR cr4_read(void)
{
    uint32_t result;
    __asm__ volatile ("mov %%cr4, %%eax" : "=a"(result));
    return result;
}

/**
 * @brief   Set the Control Register 4, various control flags for the CPU.
 *
 * You must not blank out unknown bits.
 * First use cr0_read(), then set and reset the bit you want to manipulate,
 * then call this function.
 */
static inline void X86_CR_ATTR cr4_write(uint32_t value)
{
    __asm__ volatile ("mov %%eax, %%cr4" :: "a"(value));
}

#define EFER_SCE   (1u <<  0)
#define EFER_LME   (1u <<  8)
#define EFER_LMA   (1u << 10)
#define EFER_NXE   (1u << 11)
#define EFER_SVME  (1u << 12)
#define EFER_LMSLE (1u << 12)
#define EFER_FFXSR (1u << 14)

#define MSR_EFER (0xC0000080)

/**
 * @brief   Read a Machine Specific Register.
 */
static inline uint64_t X86_CR_ATTR msr_read(uint32_t msr)
{
    uint32_t eax, edx;
    __asm__ volatile (
        "rdmsr"
        : "=a"(eax), "=d"(edx)
        : "c"(msr)
    );
    return (((uint64_t) edx) << 32) | eax;
}

/**
 * @brief   Set a Machine Specific Register.
 *
 * You must not blank out unknown bits.
 * First use msr_read(), then set and reset the bit you want to manipulate,
 * then call this function.
 */
static inline void X86_CR_ATTR msr_set(uint32_t msr, uint64_t value)
{
    __asm__ volatile (
        "wrmsr"
        :: "a"((uint32_t) value), "d"((uint32_t) (value >> 32)), "c"(msr)
    );
}

#define CPUID_FPU     (1ull << 0)
#define CPUID_VME     (1ull << 1)
#define CPUID_DE      (1ull << 2)
#define CPUID_PSE     (1ull << 3)
#define CPUID_TSC     (1ull << 4)
#define CPUID_MSR     (1ull << 5)
#define CPUID_PAE     (1ull << 6)
#define CPUID_MCE     (1ull << 7)
#define CPUID_CX8     (1ull << 8)
#define CPUID_APIC    (1ull << 9)
#define CPUID_SEP     (1ull << 11)
#define CPUID_MTRR    (1ull << 12)
#define CPUID_PGE     (1ull << 13)
#define CPUID_MCA     (1ull << 14)
#define CPUID_CMOV    (1ull << 15)
#define CPUID_PAT     (1ull << 16)
#define CPUID_PSE36   (1ull << 17)
#define CPUID_PSN     (1ull << 18)
#define CPUID_CLF     (1ull << 19)
#define CPUID_DTES    (1ull << 21)
#define CPUID_ACPI    (1ull << 22)
#define CPUID_MMX     (1ull << 23)
#define CPUID_FXSR    (1ull << 24)
#define CPUID_SSE     (1ull << 25)
#define CPUID_SSE2    (1ull << 26)
#define CPUID_SS      (1ull << 27)
#define CPUID_HTT     (1ull << 28)
#define CPUID_TM1     (1ull << 29)
#define CPUID_IA64    (1ull << 30)
#define CPUID_PBE     (1ull << 31)
#define CPUID_SSE3    (1ull << (32 + 0))
#define CPUID_PCLMUL  (1ull << (32 + 1))
#define CPUID_DTES64  (1ull << (32 + 2))
#define CPUID_MONITOR (1ull << (32 + 3))
#define CPUID_DS_CPL  (1ull << (32 + 4))
#define CPUID_VMX     (1ull << (32 + 5))
#define CPUID_SMX     (1ull << (32 + 6))
#define CPUID_EST     (1ull << (32 + 7))
#define CPUID_TM2     (1ull << (32 + 8))
#define CPUID_SSSE3   (1ull << (32 + 9))
#define CPUID_CID     (1ull << (32 + 10))
#define CPUID_FMA     (1ull << (32 + 12))
#define CPUID_CX16    (1ull << (32 + 13))
#define CPUID_ETPRD   (1ull << (32 + 14))
#define CPUID_PDCM    (1ull << (32 + 15))
#define CPUID_DCA     (1ull << (32 + 18))
#define CPUID_SSE4_1  (1ull << (32 + 19))
#define CPUID_SSE4_2  (1ull << (32 + 20))
#define CPUID_x2APIC  (1ull << (32 + 21))
#define CPUID_MOVBE   (1ull << (32 + 22))
#define CPUID_POPCNT  (1ull << (32 + 23))
#define CPUID_AES     (1ull << (32 + 25))
#define CPUID_XSAVE   (1ull << (32 + 26))
#define CPUID_OSXSAVE (1ull << (32 + 27))
#define CPUID_AVX     (1ull << (32 + 28))

/**
 * @brief   Read the basic features of the CPU.
 *
 * http://www.sandpile.org/x86/cpuid.htm
 */
static inline uint64_t X86_CR_ATTR cpuid_caps(void)
{
    uint32_t edx, ecx;
    __asm__ volatile ("cpuid" : "=d"(edx), "=c"(ecx) : "a"(1) : "ebx");
    return ((uint64_t) ecx << 32) | edx;
}

#ifdef __cplusplus
}
#endif

#endif

/** @} */
