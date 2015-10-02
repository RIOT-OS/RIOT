/*
 * Copyright (c) 2012-2014, Texas Instruments Incorporated
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

/*
 * ======== httpsrv.h ========
 *
 */
#ifndef _HTTPSRV_H_
#define _HTTPSRV_H_

#include <ti/net/network.h>
#include <ti/net/http/ssock.h>
#include <ti/net/http/urlhandler.h>

#define HTTPSrv_EACCEPTFAIL   (-2)
#define HTTPSrv_ESOCKETFAIL   (-3)
#define HTTPSrv_EBINDFAIL     (-4)
#define HTTPSrv_ELISTENFAIL   (-5)

extern const char *CONTENT_TYPE_APPLET;
extern const char *CONTENT_TYPE_AU;
extern const char *CONTENT_TYPE_CSS;
extern const char *CONTENT_TYPE_DOC;
extern const char *CONTENT_TYPE_GIF;
extern const char *CONTENT_TYPE_HTML;
extern const char *CONTENT_TYPE_JPG;
extern const char *CONTENT_TYPE_MPEG;
extern const char *CONTENT_TYPE_PDF;
extern const char *CONTENT_TYPE_WAV;
extern const char *CONTENT_TYPE_ZIP;
extern const char *CONTENT_TYPE_PLAIN;
extern const char *CONTENT_TYPE;
extern const char *CONTENT_LENGTH;
extern const char *HTTP_VER;

/* Status codes for HTTPSrv_sendResponse() and HTTPSrv_sendErrorResponse() */
enum HTTP_STATUS_CODE
{
    HTTP_OK = 200,
    HTTP_NO_CONTENT = 204,
    HTTP_BAD_REQUEST = 400,
    HTTP_AUTH_REQUIRED = 401,
    HTTP_FORBIDDEN = 403,
    HTTP_NOT_FOUND = 404,
    HTTP_NOT_ALLOWED = 405,
    HTTP_TOO_BIG = 413,
    HTTP_INTERNAL_SERVER_ERROR = 500,
    HTTP_NOT_IMPLEMENTED = 501,
    HTTP_STATUS_CODE_END
};

typedef struct HTTPSrv_Params {
    int timeout;   /* receive timeout in seconds */
    int maxLineLen;
    int maxURILen;
} HTTPSrv_Params;

typedef struct HTTPSrv_Object * HTTPSrv_Handle;

extern void HTTPSrv_init(void);

/*!
 *  @brief Create an HTTPSrv instance
 *
 *  @param[in] urlh    Array of URLHandler setup descriptors
 *  @param numURLh     Number of elements in the @c urlh array
 *  @param[in] params  Optional parameters to specify characteristics - use
 *                     NULL for defaults
 *
 *  @retval HTTPSrv instance handle
 *  @retval NULL if unable to create the instance
 */
extern HTTPSrv_Handle HTTPSrv_create(const URLHandler_Setup * urlh, int numURLh,
                                     HTTPSrv_Params * params);

/*!
 *  @brief Delete an HTTPSrv instance
 *
 *  @param[in,out] srv    Pointer containing a handle to the instance to
 *                        delete.
 *
 *  @remarks    Upon successful return, the handle pointed to by @c handlePtr
 *              will be invalid.
 */
extern void HTTPSrv_delete(HTTPSrv_Handle * srv);

/*!
 *  @brief Process the HTTP transactions for a client socket
 *
 *  @param srv  server instance returned from HTTPSrv_create
 *  @param s    TCP/IP socket connected to a client (returned from accept())
 *
 */
extern int HTTPSrv_processClient(HTTPSrv_Handle srv, int s);

extern void HTTPSrv_sendResponse(Ssock_Handle ssock, int status, const char * type,
                                 size_t len, const void * buf);

extern void HTTPSrv_sendErrorResponse(Ssock_Handle ssock, int status);

extern int HTTPSrv_serve(HTTPSrv_Handle srv, const struct sockaddr * addr,
                         int len, int backlog);

extern int HTTPSrv_serveSelect(HTTPSrv_Handle srv, const struct sockaddr * addr,
                               int len, int backlog);

extern int (*HTTPSrv_errorResponseHook)(Ssock_Handle ssock, int status);

#endif
