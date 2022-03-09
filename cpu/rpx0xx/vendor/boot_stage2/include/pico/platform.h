/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _PICO_PLATFORM_H_
#define _PICO_PLATFORM_H_

#include <sys/cdefs.h>
#include "pico/types.h"
#include "hardware/platform_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \file platform.h
*  \defgroup pico_platform pico_platform
* Compiler definitions for the selected PICO_PLATFORM
*/

#define __isr

// Section naming macros
#define __after_data(group) __attribute__((section(".after_data." group)))
#define __not_in_flash(group) __attribute__((section(".time_critical." group)))
#define __scratch_x(group) __attribute__((section(".scratch_x." group)))
#define __scratch_y(group) __attribute__((section(".scratch_y." group)))
#define __uninitialized_ram(group) __attribute__((section(".uninitialized_ram." #group))) group
// For use with PICO_COPY_TO_RAM:
#define __in_flash(group) __attribute__((section(".flashdata" group)))

/**
 * Decorates a function name, such that the function will execute from RAM (assuming it is not inlined
 * into a flash function by the compiler)
 */
#define __not_in_flash_func(func_name) __not_in_flash(__STRING(func_name)) func_name
/**
 * Historical synonym for __not_in_flash_func()
 */
#define __time_critical_func(func_name) __not_in_flash_func(func_name)

/**
 * Decorates a function name, such that the function will execute from RAM, explicitly marking it as
 * noinline to prevent it being inlined into a flash function by the compiler
 */
#define __no_inline_not_in_flash_func(func_name) __noinline __not_in_flash_func(func_name)

#define __packed_aligned __packed __aligned(4)

#if defined(__GNUC__) && __GNUC__ < 7
#define __force_inline inline __always_inline
#else
#define __force_inline __always_inline
#endif
#ifndef count_of
#define count_of(a) (sizeof(a)/sizeof((a)[0]))
#endif

#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#endif

#ifndef MIN
#define MIN(a, b) ((b)>(a)?(a):(b))
#endif

/**
 * Execute a breakpoint instruction
 */
static inline void __breakpoint(void) {
    __asm__("bkpt #0");
}

/**
 * Ensure that the compiler does not move memory access across this method call
 */
__force_inline static void __compiler_memory_barrier(void) {
    __asm__ volatile ("" : : : "memory");
}

// return a 32 bit handle for a raw ptr; DMA chaining for example embeds pointers in 32 bit values
// which of course does not work if we're running the code natively on a 64 bit platforms. Therefore
// we provide this macro which allows that code to provide a 64->32 bit mapping in host mode
#define host_safe_hw_ptr(x) ((uintptr_t)(x))

/**
 * Panic (see panic()) with the message "Unsupported".
 */
void __attribute__((noreturn)) panic_unsupported(void);

/**
 * Panic with a message. An attempt is made to output the message to all registered STDOUT drivers
 * after which this method executes a BKPT instruction.
 *
 * @param fmt format string (printf-like)
 * @param ...  printf-like arguments
 */
void __attribute__((noreturn)) panic(const char *fmt, ...);

// PICO_CONFIG: PICO_NO_FPGA_CHECK, Remove the FPGA platform check for small code size reduction, type=bool, default=0, advanced=true, group=pico_runtime
#ifndef PICO_NO_FPGA_CHECK
#define PICO_NO_FPGA_CHECK 0
#endif

#if PICO_NO_FPGA_CHECK
static inline bool running_on_fpga(void) {return false;}
#else
bool running_on_fpga(void);
#endif

/**
 * @return the RP2040 chip revision number
 */
uint8_t rp2040_chip_version(void);

/**
 * @return the RP2040 rom version number
 */
static inline uint8_t rp2040_rom_version(void) {
    return *(uint8_t*)0x13;
}

/**
 * Empty function intended to be called by any tight hardware polling loop. using this ubiquitously
 * makes it much easier to find tight loops, but also in the future \#ifdef-ed support for lockup
 * debugging might be added
 */
static inline void tight_loop_contents(void) {}

/**
 * Helper macro for making chain DMA code portable to PICO_PLATFORM=host. The problem here is
 * that embedded pointers in the data are only 32 bit, which is a problem if the host
 * system is 64 bit. This macro is zero cost on the actual device, but in host mode
 * it provides a 64->32 bit mapping
 */
#define native_safe_hw_ptr(x) ((uintptr_t)(x))

/**
 * Multiplies a by b using multiply instruction using the ARM mul instruction regardless of values;
 * i.e. this is a 1 cycle operation.
 *
 * \param a the first operand
 * \param b the second operand
 * \return a * b
 */
__force_inline static int32_t __mul_instruction(int32_t a, int32_t b) {
    asm ("mul %0, %1" : "+l" (a) : "l" (b) : );
    return a;
}

/**
 * Efficiently Multiplies value a by possibly constant value b.
 * If b is known to be constant and not zero or a power of 2, then a mul instruction is used rather than gcc's default
 * which is often a slow combination of shifts and adds
 *
 * \param a the first operand
 * \param b the second operand
 * \return a * b
 */
#define __fast_mul(a, b) __builtin_choose_expr(__builtin_constant_p(b) && !__builtin_constant_p(a), \
(__builtin_popcount(b) >= 2 ? __mul_instruction(a,b) : (a)*(b)), \
(a)*(b))

#define WRAPPER_FUNC(x) __wrap_ ## x
#define REAL_FUNC(x) __real_ ## x

#define __check_type_compatible(type_a, type_b) static_assert(__builtin_types_compatible_p(type_a, type_b), __STRING(type_a) " is not compatible with " __STRING(type_b));

/**
 * Get the current exception level on this core
 * \return the exception number if the CPU is handling an exception, or 0 otherwise
 */
uint __get_current_exception(void);

#ifdef __cplusplus
}
#endif
#endif
