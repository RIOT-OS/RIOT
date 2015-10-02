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
#ifndef _SSOCK_H_
#define _SSOCK_H_

#include <stdint.h>
#include <http/client/network.h>

#define Ssock_TIMEOUT (-11)

typedef ssize_t (*Ssock_EncryptFxn)(const void * ctx, uint8_t * oct,
        const uint8_t * ipt, size_t len);

typedef ssize_t (*Ssock_SendFxn)(const void * ctx, int s, const void * buf,
        size_t len, int flags);

typedef ssize_t (*Ssock_DecryptFxn)(const void * ctx, uint8_t * opt,
        const uint8_t * ict, size_t len);

typedef ssize_t (*Ssock_RecvFxn)(void * ctx, int s, void * buf,
        size_t len, int flags);

typedef void (*Ssock_DeleteFxn)(void * ctx);

typedef struct Ssock_SecureFxns {
    Ssock_EncryptFxn encrypt;
    Ssock_DecryptFxn decrypt;
    Ssock_SendFxn send;
    Ssock_RecvFxn recv;
    Ssock_DeleteFxn del;
    int32_t extraBytes;
} Ssock_SecureFxns;

typedef struct Ssock_Struct {
    int s;
    Ssock_SecureFxns sec;
    void * ctx;
} Ssock_Struct;

typedef struct Ssock_Struct * Ssock_Handle;

extern void Ssock_construct(Ssock_Struct * ssockP, int s);

extern Ssock_Handle Ssock_create(int s);

extern void Ssock_delete(Ssock_Handle * ss);

extern void Ssock_destruct(Ssock_Struct * ssockP);

extern int Ssock_getSocket(Ssock_Handle ss);

extern ssize_t Ssock_recv(Ssock_Handle ss, void * buf, size_t len, int flags);

extern ssize_t Ssock_recvall(Ssock_Handle ssock, void * buf, size_t len,
        int flags);

extern ssize_t Ssock_send(Ssock_Handle ss, const void * buf, size_t len,
        int flags);

extern int Ssock_startSecure(Ssock_Handle ss, Ssock_SecureFxns * sec,
        void * ctx);

#endif /* _SSOCK_H */

