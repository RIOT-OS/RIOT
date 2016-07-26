/*  Copyright (C) 2004, 2005, 2006 Board of Trustees, Leland Stanford
 *  Jr. University. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// Copy of Pintos' threads/io.h, license header extracted from Pintos' LICENSE file.

#ifndef CPU__X86__PORTS_H__
#define CPU__X86__PORTS_H__

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief        Reads and returns a byte from PORT.
 * @param[in]    port   Port to read from.
 * @returns      Read value.
 */
static inline uint8_t __attribute__((always_inline)) inb(uint16_t port)
{
    /* See [IA32-v2a] "IN". */
    uint8_t data;
    __asm__ volatile("inb %w1, %b0" : "=a"(data) : "Nd"(port));
    return data;
}

/**
 * @brief        Reads multiple bytes from a port.
 * @param[in]    port   Port to read from.
 * @param[out]   addr   Buffer to write the read values into.
 * @param[in]    cnt    Number of bytes to read.
 */
static inline void __attribute__((always_inline)) insb(uint16_t port, void *addr, size_t cnt)
{
    /* See [IA32-v2a] "INS". */
    __asm__ volatile("rep insb" : "+D"(addr), "+c"(cnt) : "d"(port) : "memory");
}

/**
 * @brief        Reads and returns a word from PORT.
 * @param[in]    port   Port to read from.
 * @returns      Read value.
 */
static inline uint16_t __attribute__((always_inline)) inw(uint16_t port)
{
    uint16_t data;
    /* See [IA32-v2a] "IN". */
    __asm__ volatile("inw %w1, %w0" : "=a"(data) : "Nd"(port));
    return data;
}

/**
 * @brief        Reads multiple words from a port.
 * @param[in]    port   Port to read from.
 * @param[out]   addr   Buffer to write the read values into.
 * @param[in]    cnt    Number of words to read.
 */
static inline void __attribute__((always_inline)) insw(uint16_t port, void *addr, size_t cnt)
{
    /* See [IA32-v2a] "INS". */
    __asm__ volatile("rep insw" : "+D"(addr), "+c"(cnt) : "d"(port) : "memory");
}

/**
 * @brief        Reads and returns a long from PORT.
 * @param[in]    port   Port to read from.
 * @returns      Read value.
 */
static inline uint32_t __attribute__((always_inline)) inl(uint16_t port)
{
    /* See [IA32-v2a] "IN". */
    uint32_t data;
    __asm__ volatile("inl %w1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

/**
 * @brief        Reads multiple longs from a port.
 * @param[in]    port   Port to read from.
 * @param[out]   addr   Buffer to write the read values into.
 * @param[in]    cnt    Number of words to read.
 */
static inline void __attribute__((always_inline)) insl(uint16_t port, void *addr, size_t cnt)
{
    /* See [IA32-v2a] "INS". */
    __asm__ volatile("rep insl" : "+D"(addr), "+c"(cnt) : "d"(port) : "memory");
}

/**
 * @brief        Writes a byte into a port.
 * @param[in]    port   Port to write into.
 * @param[in]    data   Byte to write.
 */
static inline void __attribute__((always_inline)) outb(uint16_t port, uint8_t data)
{
    /* See [IA32-v2b] "OUT". */
    __asm__ volatile("outb %b0, %w1" : : "a"(data), "Nd"(port));
}

/**
 * @brief        Writes multiple bytes into a port.
 * @param[in]    port   Port to write into.
 * @param[in]    addr   Buffer to read from.
 * @param[in]    cnt    Number of bytes to write.
 */
static inline void __attribute__((always_inline)) outsb(uint16_t port, const void *addr, size_t cnt)
{
    /* See [IA32-v2b] "OUTS". */
    __asm__ volatile("rep outsb" : "+S"(addr), "+c"(cnt) : "d"(port));
}

/**
 * @brief        Writes a word into a port.
 * @param[in]    port   Port to write into.
 * @param[in]    data   Word to write.
 */
static inline void __attribute__((always_inline)) outw(uint16_t port, uint16_t data)
{
    /* See [IA32-v2b] "OUT". */
    __asm__ volatile("outw %w0, %w1" : : "a"(data), "Nd"(port));
}

/**
 * @brief        Writes multiple words into a port.
 * @param[in]    port   Port to write into.
 * @param[in]    addr   Buffer to read from.
 * @param[in]    cnt    Number of words to write.
 */
static inline void __attribute__((always_inline)) outsw(uint16_t port, const void *addr, size_t cnt)
{
    /* See [IA32-v2b] "OUTS". */
    __asm__ volatile("rep outsw" : "+S"(addr), "+c"(cnt) : "d"(port));
}

/**
 * @brief        Writes a long into a port.
 * @param[in]    port   Port to write into.
 * @param[in]    data   Long to write.
 */
static inline void __attribute__((always_inline)) outl(uint16_t port, uint32_t data)
{
    /* See [IA32-v2b] "OUT". */
    __asm__ volatile("outl %0, %w1" : : "a"(data), "Nd"(port));
}

/**
 * @brief        Writes multiple longs into a port.
 * @param[in]    port   Port to write into.
 * @param[in]    addr   Buffer to read from.
 * @param[in]    cnt    Number of longs to write.
 */
static inline void __attribute__((always_inline)) outsl(uint16_t port, const void *addr, size_t cnt)
{
    /* See [IA32-v2b] "OUTS". */
    __asm__ volatile("rep outsl" : "+S"(addr), "+c"(cnt) : "d"(port));
}

/**
 * @brief         Make sure a write to a port was already acknowledged.
 */
static inline void  __attribute__((always_inline)) io_wait(void)
{
    __asm__ volatile("   jmp 1f\n"
                 "1: jmp 2f\n"
                 "2:");
}

#ifdef __cplusplus
}
#endif

#endif
