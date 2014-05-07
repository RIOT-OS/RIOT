/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     core_arch
 * @{
 *
 * @file        thread_arch.h
 * @brief       Architecture dependent kernel interface for handling and managing threads
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __THREAD_ARCH_H
#define __THREAD_ARCH_H


/**
 * @brief Define the mapping between the architecture independent interfaces
          and the kernel internal interfaces
 * 
 * This mapping is done for compatibility of existing platforms, 
 * new platforms should always use the *_arch_* interfaces.
 * @{
 */
#ifdef COREIF_NG
#define thread_stack_init               thread_arch_stack_init
#define thread_print_stack              thread_arch_print_stack
#define cpu_switch_context_exit         thread_arch_start_threading
#define thread_yield                    thread_arch_yield
#endif
/** @} */



char *thread_arch_stack_init(void  (*task_func)(void), void *stack_start, int stack_size);

void thread_arch_stack_print(void);


void thread_arch_start_threading(void);

void thread_arch_yield(void);


/** @} */
#endif /* __THREAD_ARCH_H */
