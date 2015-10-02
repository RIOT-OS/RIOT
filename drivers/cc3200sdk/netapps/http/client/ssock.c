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
#include "ssock.h"
#include <http/client/common.h>

/* should come from some SL header */
#define MAXSENDLEN 1460


/*
 *  ======== Ssock_construct ========
 */
void Ssock_construct(Ssock_Struct * ssockP, int s)
{
    if (ssockP) {
        ssockP->s = s;

        ssockP->sec.encrypt = NULL;
        ssockP->sec.decrypt = NULL;
        ssockP->sec.recv = NULL;
        ssockP->sec.send = NULL;
        ssockP->sec.del = NULL;
        ssockP->sec.extraBytes = 0;

        ssockP->ctx = NULL;
    }
}

/*
 *  ======== Ssock_create ========
 */
Ssock_Handle Ssock_create(int s)
{
    Ssock_Handle ssock;

    if ((ssock = xmalloc(sizeof(Ssock_Struct)))) {
        Ssock_construct(ssock, s);
    }

    return (ssock);
}

/*
 *  ======== Ssock_delete ========
 */
void Ssock_delete(Ssock_Handle * ssock)
{
    if (ssock && *ssock) {
        Ssock_Handle ss = *ssock;
        Ssock_destruct(ss);
        xfree(ss);
        *ssock = NULL;
    }
}

/*
 *  ======== Ssock_destruct ========
 */
void Ssock_destruct(Ssock_Struct * ssockP)
{
    if (ssockP) {
        if (ssockP->ctx && ssockP->sec.del) {
            ssockP->sec.del(ssockP->ctx);
        }

        ssockP->s = 0;

        ssockP->sec.encrypt = NULL;
        ssockP->sec.decrypt = NULL;
        ssockP->sec.recv = NULL;
        ssockP->sec.send = NULL;
        ssockP->sec.del = NULL;
        ssockP->sec.extraBytes = 0;

        ssockP->ctx = NULL;
    }
}

/*
 *  ======== Ssock_getSocket ========
 */
int Ssock_getSocket(Ssock_Handle ssock)
{
    return (ssock->s);
}

/*
 *  ======== Ssock_recv ========
 */
ssize_t Ssock_recv(Ssock_Handle ssock, void * buf, size_t len, int flags)
{
    ssize_t numRead;

    if (ssock->ctx) {
        numRead = ssock->sec.recv(ssock->ctx, ssock->s, buf, len, flags);
    }
    else {
        numRead = recv(ssock->s, buf, len, flags);
    }

#ifdef __SL__
    if (numRead == EAGAIN) {
        numRead = Ssock_TIMEOUT;
    }
#elif defined (__SLP__)
    if (numRead < 0) {
        if (errno(ssock->s) == EWOULDBLOCK) {
            numRead = Ssock_TIMEOUT;
        }
    }
#else /* __SL__*/
    if (numRead < 0) {
        if (errno == EAGAIN) {
            numRead = Ssock_TIMEOUT;
        }
    }

#endif /* __SL__ */

    return (numRead);
}

/*
 *  ======== Ssock_recvall ========
 */
ssize_t Ssock_recvall(Ssock_Handle ssock, void * buf, size_t len, int flags)
{
    ssize_t nbytes;

    while (len > 0) {
        nbytes = Ssock_recv(ssock, buf, len, flags);
        if (nbytes > 0) {
            len -= nbytes;
            buf = (uint8_t *)buf + nbytes;
        }
        else {
            break;
        }
    }

    return (nbytes);
}

/*
 *  ======== Ssock_send ========
 */
ssize_t Ssock_send(Ssock_Handle ssock, const void * buf, size_t len, int flags)
{
    ssize_t nbytes;
    ssize_t status = -1;
    uint8_t * cbuf = NULL;
    size_t maxSendLen = MAXSENDLEN - ssock->sec.extraBytes;

    if (ssock->ctx && ssock->sec.encrypt) {
        if ((cbuf = xmalloc(MAXSENDLEN)) == NULL) {
            return (-1);
        }
    }

#ifdef __linux__
    flags |= MSG_NOSIGNAL;
#endif

    /*
     * This is not needed on Linux and NDK stacks, but SL does not handle
     * fragmentation, so we run the same code on all stacks for now to get
     * test coverage on this implementation.
     */
    while (len > 0) {
        ssize_t ilen = len > maxSendLen ? maxSendLen : len;

        if (ssock->ctx && ssock->sec.encrypt) {
            ssize_t clen;

            clen = ssock->sec.encrypt(ssock->ctx, cbuf, buf, ilen);
            nbytes = send(ssock->s, cbuf, clen, flags);
            /* What if nbytes > 0 but < clen? */
            if (nbytes > 0) {
                /* assert(nbytes == clen); */
                /* set to how many bytes actually sent from the input buffer */
                nbytes = ilen;
            }
        }
        else if (ssock->ctx) {
            nbytes = ssock->sec.send(ssock->ctx, ssock->s, buf, ilen, flags);
        }
        else {
            nbytes = send(ssock->s, buf, ilen, flags);
        }

        if (nbytes >= 0) {
            len -= nbytes;
            buf = (uint8_t *)buf + nbytes;
            status = status == -1? nbytes : (status + nbytes);
        }
        else {
#ifdef __SLP__
            nbytes = errno(ssock->s);
            if (nbytes != EWOULDBLOCK) {
                status = -1;
            }
            else if (nbytes == EWOULDBLOCK && status == -1) {
                /* No byte was sent */
                status = 0;
            }
#else /* __SLP__ */
            status = nbytes;
#endif /* __SLP__ */
            break;
        }
    }

    if (cbuf) {
        xfree(cbuf);
    }

    return (status);
}

/*
 *  ======== Ssock_startSecure ========
 */
int Ssock_startSecure(Ssock_Handle ssock, Ssock_SecureFxns * sec, void * ctx)
{
    if (ssock->ctx || !sec || !ctx) {
        return (-1);
    }

    ssock->ctx = ctx;
    ssock->sec = *sec;

    return (0);
}
