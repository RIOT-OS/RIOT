/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_zynq7000
 * @{
 *
 * @file
 * @brief       Implementation of the kernels atomic interface
 *
 * @author      Simon Vincent <simon.vincent@xsilon.com>
 *
 * @}
 */

#include <stdint.h>
#include "atomic.h"
#include "irq.h"
#include "cpu.h"

#if ARCH_HAS_ATOMIC_COMPARE_AND_SWAP

static inline uint32_t __LDREXW(volatile uint32_t *addr)
{
    uint32_t result;

   __asm__ __volatile__ ("ldrex %0, %1" : "=r" (result) : "Q" (*addr) );
   return(result);
}

static inline uint32_t __STREXW(uint32_t value, volatile uint32_t *addr)
{
   uint32_t result;

   __asm__ __volatile__ ("strex %0, %2, %1" : "=&r" (result), "=Q" (*addr) : "r" (value) );
   return(result);
}

static inline void __CLREX(void)
{
	__asm__ __volatile__ ("clrex" ::: "memory");
}

int atomic_cas(atomic_int_t *var, int old, int now)
{
    int tmp;
    int status;

    /* Load exclusive */
    tmp = __LDREXW((volatile uint32_t *)(&ATOMIC_VALUE(*var)));

    if (tmp != old) {
        /* Clear memory exclusivity */
        __CLREX();
        return 0;
    }

    /* Try to write the new value */
    status = __STREXW(now, (volatile uint32_t *)(&ATOMIC_VALUE(*var)));

    return (status == 0);
}
#endif
