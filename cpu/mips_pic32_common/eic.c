/*
 * Copyright (C) 2019 Francois Berder
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#include "cpu_conf.h"
#include "eic.h"
#include <stdint.h>

#if defined(CPU_FAM_PIC32MX)
#define VEC_NUMOF   (64)
#elif defined (CPU_FAM_PIC32MZ)
#define VEC_NUMOF   (256)
#endif

#define IECSET(V)   *(volatile uint32_t *)((uintptr_t)&IEC0SET + 0x10 * ((V) / 32))
#define IECCLR(V)   *(volatile uint32_t *)((uintptr_t)&IEC0CLR + 0x10 * ((V) / 32))
#define IFSCLR(V)   *(volatile uint32_t *)((uintptr_t)&IFS0CLR + 0x10 * ((V) / 32))
#define IPC(V)      *(volatile uint32_t *)((uintptr_t)&IPC0 + 0x10 * ((V) / 4))

static external_isr_ptr_t vectors[VEC_NUMOF];

void set_external_isr_cb(int vecNum, external_isr_ptr_t cbFunc)
{
    if (vecNum < VEC_NUMOF)
        vectors[vecNum] = cbFunc;
}

/* note Compiler inserts GP context save + restore code (to current stack). */
/*
 * This is a hack - currently the toolchain does not support correct placement
 * of EIC mode vectors (it is coming though) But we can support non-vectored EIC
 * mode and note the default PIC32 interrupt controller (which uses EIC +
 * MCU-ASE) defaults to non vectored mode anyway with all interrupts coming via
 * vector 0 which is equivalent to 'sw0' in 'VI' mode.
 *
 * Thus all EIC interrupts should be decoded here.
 *
 * When toolchain support is available we could move to full vector mode but
 * this does take up significant space (MCU-ASE provides 256 vectors at 32B
 * spacing (the default) that's 8KB of vector space!), So a single entry point
 * may be better anyway.
 *
 */
void __attribute__ ((interrupt("vector=sw0"), keep_interrupts_masked)) _mips_isr_sw0(void)
{
#if defined(CPU_FAM_PIC32MX)
    int vecNum = INTSTAT & _INTSTAT_VEC_MASK;
#elif defined (CPU_FAM_PIC32MZ)
    int vecNum = INTSTAT & _INTSTAT_SIRQ_MASK;
#endif

    if (vectors[vecNum])
        vectors[vecNum]();
}

void eic_configure_priority(int vecNum, int priority, int subpriority)
{
    unsigned int offset;

    if (vecNum >= VEC_NUMOF)
        return;

   offset = 8 * (vecNum & 0x3);
   IPC(vecNum) &= ~(0x1F << offset);
   IPC(vecNum) |= ((priority << 2) | (subpriority)) << offset;
}

void eic_enable(int vecNum)
{
    if (vecNum >= VEC_NUMOF)
        return;

    IECSET(vecNum) = 1U << (vecNum & 0x1F);
}

void eic_disable(int vecNum)
{
    if (vecNum >= VEC_NUMOF)
        return;

    IECCLR(vecNum) = 1U << (vecNum & 0x1F);
}

void eic_clear_flag(int vecNum)
{
    if (vecNum >= VEC_NUMOF)
        return;

    IFSCLR(vecNum) = 1U << (vecNum & 0x1F);
}
