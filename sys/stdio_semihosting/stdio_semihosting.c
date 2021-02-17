/*
 * Copyright (C) 2020 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       STDIO over ARM and RISC-V Semihosting implementation
 *
 * RISC-V semihosting closely mimics ARM semihosting. Only the break sequence is
 * different, but all defined values are also used with RISC-V
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <errno.h>
#include <string.h>

#include "stdio_semihosting.h"
#include "xtimer.h"

#if MODULE_VFS
#include "vfs.h"
#endif

/**
 * @brief Rate at which the stdin read polls (breaks) the debugger for input
 * data
 */
#define STDIO_SEMIHOSTING_POLL_RATE     (10 * US_PER_MS)

/**
 * @brief ARM Semihosting STDIN file descriptor. Also used with RISC-V
 */
#define STDIO_SEMIHOSTING_F_STDIN       (1)

/**
 * @brief ARM Semihosting STDOUT file descriptor. Also used with RISC-V
 */
#define STDIO_SEMIHOSTING_F_STDOUT      (1)

/**
 * @name ARM Semihosting commands.
 *
 * RISC-V copied over these command names and values
 *
 * Extend when required
 * @{
 */
#define STDIO_SEMIHOSTING_SYS_WRITE     (0x05) /**< Write command */
#define STDIO_SEMIHOSTING_SYS_READ      (0x06) /**< Read command  */
/** @} */

#if defined(MODULE_RISCV_COMMON)
static bool _semihosting_connected(void) {
    return true;
}

static uint32_t _semihosting_raw(int cmd, uint32_t *args)
{
    uint32_t result = 0;
    /* Moves cmd and args to r0 and r1. Then triggers a breakpoint.
     * Finally moves the results stored in r0 to result
     */
    __asm__(
        ".option norvc      \n"
        "mv a0, %[cmd]      \n"
        "mv a1, %[args]     \n"
        /* Wrapping the ebreak instruction in two NOP SLLI and SRAI instructions
         * act as indicator to the GDB session that this is a
         * semihosting trap */
        "slli x0, x0, 0x1f  \n"
        "ebreak             \n"
        "srai x0, x0, 7     \n"
        "mv %[result], a0   \n"
        : /* Outputs */
        [result] "=r" (result)
        : /* Inputs */
        [cmd] "r" (cmd),
        [args] "r" (args)
        : /* Clobbered registers */
        "a0", "a1", "memory"
    );
    return result;
}

#elif defined(MODULE_CORTEXM_COMMON)

static bool _semihosting_connected(void) {
#ifdef CoreDebug_DHCSR_C_DEBUGEN_Msk
    /* Best effort attempt to detect if a debug session is active */
    return CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk;
#else
    return true;
#endif
}

static uint32_t _semihosting_raw(int cmd, uint32_t *args)
{
    uint32_t result = 0;
    /* Moves cmd and args to r0 and r1. Then triggers a breakpoint.
     * Finally moves the results stored in r0 to result
     */
    __asm__(
        "mov r0, %[cmd] \n"
        "mov r1, %[args] \n"
        "bkpt #0xAB \n"
        "mov %[result], r0\n"
        : /* Outputs */
        [result] "=r" (result)
        : /* Inputs */
        [cmd] "r" (cmd),
        [args] "r" (args)
        : /* Clobbered registers */
        "r0", "r1", "memory"
    );
    return result;
}
#endif

static size_t _semihosting_write(const uint8_t *buffer, size_t len)
{
    uint32_t args[3] = {
        STDIO_SEMIHOSTING_F_STDOUT,
        (uint32_t)buffer,
        (uint32_t)len,
    };
    return _semihosting_raw(STDIO_SEMIHOSTING_SYS_WRITE, args);
}

static ssize_t _semihosting_read(uint8_t *buffer, size_t len)
{
    uint32_t args[3] = {
        STDIO_SEMIHOSTING_F_STDIN,
        (uint32_t)buffer,
        (uint32_t)len,
    };
    size_t remaining = _semihosting_raw(STDIO_SEMIHOSTING_SYS_READ, args);
    return len - remaining;
}

void stdio_init(void)
{
#if MODULE_VFS
    vfs_bind_stdio();
#endif
}

ssize_t stdio_read(void* buffer, size_t count)
{
    if (STDIO_SEMIHOSTING_RX) {
        xtimer_ticks32_t last_wakeup = xtimer_now();
        ssize_t bytes_read = _semihosting_read(buffer, count);
        while (bytes_read == 0) {
            xtimer_periodic_wakeup(&last_wakeup, STDIO_SEMIHOSTING_POLL_RATE);
            bytes_read = _semihosting_read(buffer, count);
        }
        return bytes_read;
    }
    return -ENOTSUP;
}

ssize_t stdio_write(const void* buffer, size_t len)
{
    if (!_semihosting_connected()) {
        return len;
    }
    size_t remaining = _semihosting_write(buffer, len);
    return len - remaining;
}
