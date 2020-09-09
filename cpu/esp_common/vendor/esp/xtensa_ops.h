/** xtensa_ops.h
 *
 * Special macros/etc which deal with Xtensa-specific architecture/CPU
 * considerations.
 *
 * Part of esp-open-rtos
 * Copyright (C) 2015 Superhouse Automation Pty Ltd
 * BSD Licensed as described in the file LICENSE
 */

/*
Copyright (c) 2015, SuperHouse Automation Pty Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef XTENSA_OPS_H
#define XTENSA_OPS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Read stack pointer to variable.
 *
 * Note that the compiler will push a stack frame (minimum 16 bytes)
 * in the prelude of a C function that calls any other functions.
 */
#define SP(var) __asm__ volatile ("mov %0, a1" : "=r" (var))

/* Read the function return address to a variable.
 *
 * Depends on the containing function being simple enough that a0 is
 * being used as a working register.
 */
#define RETADDR(var) __asm__ volatile ("mov %0, a0" : "=r" (var))

// GCC macros for reading, writing, and exchanging Xtensa processor special
// registers:

#define RSR(var, reg) __asm__ volatile ("rsr %0, " #reg : "=r" (var));
#define WSR(var, reg) __asm__ volatile ("wsr %0, " #reg : : "r" (var));
#define XSR(var, reg) __asm__ volatile ("xsr %0, " #reg : "+r" (var));

// GCC macros for performing associated "*sync" opcodes

#define ISYNC() __asm__ volatile ( "isync" )
#define RSYNC() __asm__ volatile ( "rsync" )
#define ESYNC() __asm__ volatile ( "esync" )
#define DSYNC() __asm__ volatile ( "dsync" )

#ifdef __cplusplus
}
#endif

#endif /* XTENSA_OPS_H */
