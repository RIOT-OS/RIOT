/*
 * Copyright(C) 2017, 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_mips32r2_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels thread interface
 *
 * @author      Neil Jones <neil.jones@imgtec.com>
 */
#ifndef THREAD_ARCH_H
#define THREAD_ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_API_INLINED

#ifndef DOXYGEN /* Doxygen is in core/include/thread.h */

static inline __attribute__((always_inline)) void thread_yield_higher(void)
{
    /*
     * throw a syscall exception to get into exception level
     * we context switch at exception level.
     *
     * Note syscall 1 is reserved for UHI see:
     * http://wiki.prplfoundation.org/w/images/4/42/UHI_Reference_Manual.pdf
     */
    __asm volatile ("syscall 2");
}

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* THREAD_ARCH_H */
/** @} */
