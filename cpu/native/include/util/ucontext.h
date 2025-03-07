/*
 * Copyright (C) 2013 - 2016 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * Copyright (C) 2025 carl-tud
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef UTIL_UCONTEXT_H
#define UTIL_UCONTEXT_H

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

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* UTIL_UCONTEXT_H */
