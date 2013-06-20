/**
 * @ingroup	kernel
 * @{
 * @file        kernel_intern.h
 * @brief       prototypes for kernel intern functions
 * @author      INRIA
 * @author		Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef KERNEL_INTERN_H_
#define KERNEL_INTERN_H_

/**
 * @brief   Initializes scheduler and creates main and idle task
 */
void kernel_init(void);

/**
 * @brief   Optionally: initializes platform specifics (devices, pin configuration etc.)
 */
void board_init(void);

/**
 * @brief   Gets called upon thread creation to set CPU registers
 *
 * @param[in] task_func     First function to call within the thread
 * @param[in] stack_start   Start address of the stack
 * @param[in] stack_size    Stack size
 *
 * @return stack pointer
 */
char *thread_stack_init(void  *task_func, void *stack_start, int stack_size);

/**
 * @brief  Removes thread from scheduler and set status to STATUS_STOPPED
 */
void sched_task_exit(void);

/**
 * @brief   Prints human readable, ps-like thread information for debugging purposes
 */
void thread_print_stack(void);

/**
 * @brief   Calculates stack usage if thread was created using CREATE_STACKTEST
 *
 * @param[in] stack The thread's stack
 *
 * @return  The current usage (overwritten addresses) of the thread's stack
 */
int thread_measure_stack_usage(char *stack);

/** @} */
#endif /* KERNEL_INTERN_H_ */
