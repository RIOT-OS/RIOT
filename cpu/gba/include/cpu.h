
#ifndef CPU_H
#define CPU_H

/**
 * @defgroup    cpu_gba     Nintendo ARM7TDMI
 * @ingroup     cpu
 * @brief       Nintendo ARM7TDMI Gameboy Advance specific code
 * @{
 */

#include <stdio.h>
#include <stdbool.h>

#include "arm_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

    extern uintptr_t __stack_start; /**< end of user stack memory space */

    /**
     * @brief   Returns the current content of the link register (lr)
     */
    static inline uintptr_t cpu_get_caller_pc(void)
    {
        register uintptr_t lr_ptr;
        __asm__ __volatile__("mov %0, lr"
                             : "=r"(lr_ptr));
        return lr_ptr;
    }

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_H */
