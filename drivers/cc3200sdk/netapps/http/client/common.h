/*
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __linux__
#include <pthread.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#elif defined(__SL__) || defined(__NDK__) || defined(__SLP__) /* __linux__ */
//#include <ti/sysbios/knl/Task.h> //Aashish: Commented for Simplelink with FreeRTOS or NON-OS environment 
//#include <xdc/runtime/System.h>  //Aashish: Commented for Simplelink with FreeRTOS or NON-OS environment 	
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#else /* __linux__ */
#error Unsupported configuration specified
#endif /* __linux__ */

/*
 *  ======== xassert ========
 */
static inline void xassert(int expr)
{
    //assert(expr);
  if(!(expr)) {while(1);}
}

/*
 *  ======== xfree ========
 */
static inline void xfree(void *ptr)
{
    free(ptr);
}

/*
 *  ======== xmalloc ========
 */
static inline void *xmalloc(size_t size)
{
    return (malloc(size));
}

/*
 *  ======== xvsnprintf ========
 */
static inline int xvsnprintf(char *s, size_t n, const char *fmt, va_list arg)
{
#if defined(__SL__) || defined (__NDK__) || defined(__SLP__)
    //return (System_vsnprintf(s, n, fmt, arg)); //Aashish: Commented for Simplelink with FreeRTOS or NON-OS environment 
    return (vsnprintf(s, n, fmt, arg));
#else /* __SL__ || __NDK__ || __SLP__*/
    return (vsnprintf(s, n, fmt, arg));
#endif /* __SL__ || __NDK__ */
}

#endif /* _COMMON_H_ */
