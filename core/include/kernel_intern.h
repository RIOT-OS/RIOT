/**
 * @ingroup	kernel
 * @{
 * @file
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		Kaspar Schleiser <kaspar@schleiser.de>
 */

/*
 * kernel_intern.h
 *
 *  Created on: 10.02.09
 *      Author: kaspar
 */

#ifndef KERNEL_INTERN_H_
#define KERNEL_INTERN_H_

void kernel_init(void);
void board_init_drivers(void);
char *thread_stack_init(void  *task_func, void *stack_start, int stack_size);
void sched_task_exit(void);
void thread_print_stack (void);
int thread_measure_stack_usage(char* stack);

/** @} */
#endif /* KERNEL_INTERN_H_ */
