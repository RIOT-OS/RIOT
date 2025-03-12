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

#if defined(__APPLE__)
#  include <mach/thread_status.h>
#endif

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
 *
 * You may look at the following links for the `ucontext.h` implementation.
 * - On macOS/Darwin, `ucontext` is implemented in [libplatform](https://github.com/apple-oss-distributions/libplatform/tree/main/src/ucontext)
 * - On GNU/Linux, `ucontext` is implemented
 *   - in [`glibc`](https://github.com/bminor/glibc/tree/9e51ae3cd0c7f65bdeba93b7f1d780cdb21fc269/sysdeps/unix/sysv/linux)
 *   - but not in `musl`
 *
 * @{
 */

/**
 * @brief Retrieves function pointer generated during calls to `makecontext`/`setcontext`/`swapcontext`
 * @param context `ucontext_t`
 * @param presigned A boolean value indicating whether to obtain the presigned pointer value (PAC on arm64e)
 * @returns Function pointer (does not need to be start of a function, points to instruction)
 */
static inline uintptr_t _context_get_fptr(ucontext_t *context, bool presigned) {
    (void)presigned;
# if defined(__FreeBSD__) /* FreeBSD */
    return (uintptr_t)((struct sigcontext *)context)->sc_eip;
# elif defined(__linux__) /* Linux */
#  if defined(__arm64__) || defined(__aarch64__)
    return (uintptr_t)((ucontext_t *)context)->uc_mcontext.pc;
#  elif defined(__arm__)
    return (uintptr_t)((ucontext_t *)context)->uc_mcontext.arm_pc;
#  elif defined(__x86_64__)
    return (uintptr_t)((ucontext_t *)context)->uc_mcontext.gregs[REG_RIP];
#  elif defined(__i386__)
    return (uintptr_t)((ucontext_t *)context)->uc_mcontext.gregs[REG_EIP];
#  else
#   error "Unsupported Linux architecture"
#  endif
# elif defined(__APPLE__) /* Darwin */
#  if defined(__arm64__) || defined(__aarch64__) /* Darwin/arm64 */
    /* Get unsigned version of LR register as function pointer */
    return presigned ? arm_thread_state64_get_lr(context->uc_mcontext->__ss) :
        (uintptr_t)arm_thread_state64_get_lr_fptr(context->uc_mcontext->__ss);
#  elif defined(__x86_64__) /* Darwin/x86 64-bit */
    return (uintptr_t)context->uc_mcontext->__ss.__rip;
#  elif defined(__i386__) /* Darwin/x86 32-bit */
    return (uintptr_t)context->uc_mcontext->__ss.__eip;
#  else
#   error "Unsupported Darwin/macOS architecture"
#  endif
# else
#  error "Operating system unsupported"
# endif
}

/**
 * @brief Retrieves function pointer generated during calls to `makecontext`/`setcontext`/`swapcontext`
 * @param context `ucontext_t`
 * @param func Function pointer
 * @param presigned A boolean value indicating whether to treat @p func as a presigned pointer (PAC on arm64e)
 */
static inline void _context_set_fptr(ucontext_t *context, uintptr_t func, bool presigned) {
# if defined(__FreeBSD__) /* FreeBSD */
    ((struct sigcontext *)context)->sc_eip = (unsigned int)func;
# elif defined(__linux__) /* Linux */
#  if defined(__arm64__) || defined(__aarch64__)
    ((ucontext_t *)context)->uc_mcontext.pc = func;
#  elif defined(__arm__)
    ((ucontext_t *)context)->uc_mcontext.arm_lr = func;
    ((ucontext_t *)context)->uc_mcontext.arm_pc = func;
#  elif defined(__x86_64__)
    ((ucontext_t *)context)->uc_mcontext.gregs[REG_RIP] = (greg_t)func;
#  elif defined(__i386__)
    ((ucontext_t *)context)->uc_mcontext.gregs[REG_EIP] = func;
#  else
#   error "Unsupported Linux architecture"
#  endif
# elif defined(__APPLE__) /* Darwin */
#  if defined(__arm64__) || defined(__aarch64__) /* Darwin/arm64 */
    /* Get unsigned version of LR register as function pointer */
    if (presigned) {
        arm_thread_state64_set_lr_presigned_fptr(context->uc_mcontext->__ss, func);
    } else {
        arm_thread_state64_set_lr_fptr(context->uc_mcontext->__ss, func);
    }
#  elif defined(__x86_64__) /* Darwin/x86 64-bit */
    context->uc_mcontext->__ss.__rip = func;
#  elif defined(__i386__) /* Darwin/x86 32-bit */
    context->uc_mcontext->__ss.__eip = func;
#  else
#   error "Unsupported Darwin/macOS architecture"
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
 *
 * @note This function does not authenticate the task function pointer on arm64e/ARMv8.3 (unimplemented).
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
 *
 * @note This function does not sign the task function pointer (unimplemented).
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
#   elif defined(__APPLE__)
     context->uc_mcontext->__ss.__r14 = (intptr_t)func;
     context->uc_mcontext->__ss.__r15 = (intptr_t)arg;
#   endif
#  elif defined(__arm64__) || defined(__aarch64__)
#   if defined(__linux__)
     context->uc_mcontext.regs[27] = (greg_t)func;
     context->uc_mcontext.regs[28] = (greg_t)arg;
#   elif defined(__APPLE__)
     context->uc_mcontext->__ss.__x[27] = (uintptr_t)func;
     context->uc_mcontext->__ss.__x[28] = (uintptr_t)arg;
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

#endif /* UTIL_UCONTEXT_H */
