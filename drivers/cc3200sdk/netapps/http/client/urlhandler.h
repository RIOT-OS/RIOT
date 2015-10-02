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
#ifndef _URLHANDLER_H_
#define _URLHANDLER_H_

#include <stddef.h>

#include "ssock.h"

#define URLHandler_GET 1
#define URLHandler_POST 2
#define URLHandler_PUT 3
#define URLHandler_DELETE 4

#define URLHandler_ENOTHANDLED 0
#define URLHandler_EHANDLED 1
#define URLHandler_EERRORHANDLED 2

typedef struct URLHandler_Object * URLHandler_Handle;

typedef URLHandler_Handle (*URLHandler_CreateFxn)(void * arg);
typedef int (*URLHandler_ProcessFxn)(URLHandler_Handle u, int method, const char * url,
                                     const char * urlArgs, int contentLength,
                                     Ssock_Handle s);
typedef void (*URLHandler_ScanFieldFxn)(URLHandler_Handle u, int method, const char * url,
                                        const char * field);
typedef void (*URLHandler_DeleteFxn)(URLHandler_Handle * u);

typedef struct URLHandler_Setup {
    void * params;
    URLHandler_CreateFxn create;
    URLHandler_DeleteFxn delete;
    URLHandler_ProcessFxn process;
    URLHandler_ScanFieldFxn scan;
} URLHandler_Setup;

#endif
