/*
 * SPDX-FileCopyrightText: 2013-2016 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * SPDX-FileCopyrightText: 2025 carl-tud
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#if USE_LIBUCONTEXT
#  include <libucontext/libucontext.h>
#else
#  include <ucontext.h>
#endif /* USE_LIBUCONTEXT */

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup cpu_native
 * @{
 */

/* MARK: - Context accessors */
/**
 * @name Context accessors
 * @{
 */

/**
 * @brief Retrieves function pointer generated during calls to `makecontext`/`setcontext`/`swapcontext`
 * @param context `ucontext_t`
 * @returns Function pointer (does not need to be start of a function, points to instruction)
 */
static inline uintptr_t _context_get_fptr(ucontext_t *context) {
# if defined(__FreeBSD__) /* FreeBSD */
    return (uintptr_t)((struct sigcontext *)context)->sc_eip;
# elif defined(__linux__) /* Linux */
#  if defined(__arm__)
    return (uintptr_t)((ucontext_t *)context)->uc_mcontext.arm_pc;
#  elif defined(__x86_64__)
    return (uintptr_t)((ucontext_t *)context)->uc_mcontext.gregs[REG_RIP];
#  elif defined(__i386__)
    return (uintptr_t)((ucontext_t *)context)->uc_mcontext.gregs[REG_EIP];
#  else
#   error "Unsupported Linux architecture"
#  endif
# else
#  error "Operating system unsupported"
# endif
}

/**
 * @brief Retrieves function pointer generated during calls to `makecontext`/`setcontext`/`swapcontext`
 * @param context `ucontext_t`
 * @param func Function pointer
 */
static inline void _context_set_fptr(ucontext_t *context, uintptr_t func) {
# if defined(__FreeBSD__) /* FreeBSD */
    ((struct sigcontext *)context)->sc_eip = (unsigned int)func;
# elif defined(__linux__) /* Linux */
#  if defined(__arm__)
    ((ucontext_t *)context)->uc_mcontext.arm_lr = func;
    ((ucontext_t *)context)->uc_mcontext.arm_pc = func;
#  elif defined(__x86_64__)
    ((ucontext_t *)context)->uc_mcontext.gregs[REG_RIP] = (greg_t)func;
#  elif defined(__i386__)
    ((ucontext_t *)context)->uc_mcontext.gregs[REG_EIP] = func;
#  else
#   error "Unsupported Linux architecture"
#  endif
# else
#  error "Operating system unsupported"
# endif
}
/** @} */

/* MARK: - 64-bit support for makecontext */
/**
 * @name 64-bit support for makecontext
 * @{
 */
#if defined(__LP64__) || defined(DOXYGEN)
/**
 * @brief Invokes thread task function
 *
 * This function fixes the otherwise necessary cast from `void*` to `int` of the thread
 * task function pointer. We pass the task func and its argument in registers.
 * See @ref makecontext64.
 */
extern void _start_task_func64(void);
#endif

/**
 * @brief Like `makecontext`, allows 64-bit wide function argument on 64-bit platforms
 * @param context `ucontext_t`
 * @param func Function to be executed when context is applied
 * @param arg Function argument, previously limited to `int` width, can now be up to 64 bits wide on 64-bit platforms
 *
 * Internally, we circumvent the 32-bit `int` limitation by passing the parameter in a register. This is done using
 * a custom function defined in `native.S`
 */
static inline void makecontext64(ucontext_t *context, void (*func)(void), void* arg) {
# if defined(__LP64__)
    /* makecontext accepts int arguments. In RIOT, we use void* for the optional argument.
     * To not truncate the argument pointer, we pass it in a register to _start_task_func64. */
    makecontext(context, (void (*)(void))_start_task_func64, 0);

#  if defined(__x86_64__)
#   if defined(__linux__)
     context->uc_mcontext.gregs[REG_R14] = (greg_t)func;
     context->uc_mcontext.gregs[REG_R15] = (greg_t)arg;
#   endif
#  endif

# else
    /* On 32-bit platforms, the width of an int is enough to fit a pointer. */
    makecontext(context, (void (*)(void))func, 1, arg);
# endif
}
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif
