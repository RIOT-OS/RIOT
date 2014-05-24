#ifndef CPU_H
#define CPU_H

/**
 * @defgroup    lpc1768 NXP LPC1768
 * @brief       NXP LPC1768 specific code
 * @ingroup     cpu
 * @{
 */

#include "LPC17xx.h"
#include "core_cm3.h"
#include "core_cmFunc.h"

extern void dINT(void);
extern void eINT(void);

/**
 * @brief Save the thread's context
 */
void save_context(void);

/**
 * @brief Restores the before saved context of a thread
 */
void restore_context(void);

/**
 * @brief Let the thread yield
 */
void thread_yield(void);

/** @} */

#endif /* CPU_H */
