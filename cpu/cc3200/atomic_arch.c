/*
 * Original work Copyright (c) 2014-2015 Freie Universität Berlin
 * Modified work Copyright 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file
 * @brief       Implementation of the kernels atomic interface
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 * @author      Attilio Dona' <@attiliodona>
 *
 * @}
 */

#include <stdint.h>
#include "atomic.h"
#include "irq.h"
#include "cpu.h"

#if ARCH_HAS_ATOMIC_COMPARE_AND_SWAP

/** \brief  LDR Exclusive (32 bit)

    This function executes a exclusive LDR instruction for 32 bit values.

    \param [in]    ptr  Pointer to data
    \return        value of type uint32_t at (*ptr)
 */
__attribute__((always_inline)) __STATIC_INLINE uint32_t __LDREXW(volatile uint32_t *addr)
{
    uint32_t result;

   __ASM volatile ("ldrex %0, %1" : "=r" (result) : "Q" (*addr) );
   return(result);
}

/** \brief  STR Exclusive (32 bit)

    This function executes a exclusive STR instruction for 32 bit values.

    \param [in]  value  Value to store
    \param [in]    ptr  Pointer to location
    \return          0  Function succeeded
    \return          1  Function failed
 */
__attribute__((always_inline)) __STATIC_INLINE uint32_t __STREXW(uint32_t value, volatile uint32_t *addr)
{
   uint32_t result;

   __ASM volatile ("strex %0, %2, %1" : "=&r" (result), "=Q" (*addr) : "r" (value) );
   return(result);
}


/** \brief  Remove the exclusive lock

    This function removes the exclusive lock which is created by LDREX.

 */
__attribute__((always_inline)) __STATIC_INLINE void __CLREX(void)
{
  __ASM volatile ("clrex" ::: "memory");
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
