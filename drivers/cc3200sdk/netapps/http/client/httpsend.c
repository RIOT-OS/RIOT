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
 * ======== httpsend.c ========
 *
 * Send functions used by HTTP server and applications
 *
 */
#include <ti/net/http/httpsrv.h>
#include <logging.h>

#ifdef __linux__
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#define esnprintf(fmt, len, ...) System_snprintf(fmt, len, __VA_ARGS__)

#elif __TIRTOS__
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#define esnprintf(fmt, len, ...) System_snprintf(fmt, len, __VA_ARGS__)

#else
#error "undefined printf() macros"
#endif

/*
 *  Size of a response header. Worst case is:
 *
 *  HTTP/1.1 NNN Explanation\r\n
 *  Content-type: some-mime-type\r\n
 *
 *  Currently, "Explanation" is max 18 char, so 33 for status line; plus
 *  "Content-type: " and some-mime-type, assume 95 or less char.
 */
#define MAXRESPONSESIZE  128

typedef struct StatusMap {
    int  status;
    const char * message;
} StatusMap;

static StatusMap httpStatusMap[] =
{
    {HTTP_OK, "OK"},
    {HTTP_NO_CONTENT, "No Content"},
    {HTTP_BAD_REQUEST, "Bad Request"},
    {HTTP_AUTH_REQUIRED, "Authorization Required"},
    {HTTP_NOT_FOUND, "File Not Found"},
    {HTTP_NOT_IMPLEMENTED, "Not Implemented"},
    {HTTP_NOT_ALLOWED, "Not Allowed"},
    {0, "Unknown"}
};

int (*HTTPSrv_errorResponseHook)(Ssock_Handle ssock, int StatusCode) = 0;

static const char * getStatusMessage(int status)
{
    int  i;

    for (i = 0; httpStatusMap[i].status && httpStatusMap[i].status != status; i++);

    return (httpStatusMap[i].message);
}

/*
 * Sends a formatted response message to Sock with the given
 * status code and content type. The value of ContentType can
 * be NULL if no ContentType is required.
 */
static void httpSendStatusLine(Ssock_Handle ssock, int status, const char * contentType)
{
    char buf[MAXRESPONSESIZE];
    const char * msg;
    int len;

    Log_print1(Diags_ANALYSIS, "sendStatus> %d", (IArg)status);

    if (status < 0 || status > 999) {
        status = 999;
    }

    msg = getStatusMessage(status);

    if (contentType) {
        len = esnprintf(buf, sizeof(buf), "%s %3d %s\r\n%s%s\r\n", HTTP_VER,
                        status, msg, CONTENT_TYPE, contentType);
    }
    else {
        len = esnprintf(buf, sizeof(buf), "%s %3d %s\r\n\r\n", HTTP_VER,
                        status, msg);
    }

    if (len == sizeof(buf)) {
        Log_print1(Diags_WARNING, "sendStatus> Buffer size too small: %d", sizeof(buf));
    }

    Ssock_send(ssock, buf, len, 0);
}

/*
 * Write out the entiry length tag, and terminates the header
 * with an additional CRLF.
 *
 * Since the header is terminated, this must be the last tag
 * written. Entity data should follow immediately.
 */
static void httpSendEntityLength(Ssock_Handle ssock, int32_t entityLen)
{
    char buf[32]; /* sizeof("Content-length: ") + 10 + 4 + 1]; */
    int len;

    len = esnprintf(buf, sizeof(buf), "%s%d\r\n\r\n", CONTENT_LENGTH, entityLen);
    if (len == sizeof(buf)) {
        Log_print1(Diags_WARNING, "sendEntityLength> Buffer size too small: %d",
                   sizeof(buf));
    }

    Ssock_send(ssock, buf, len, 0);
}

void HTTPSrv_sendErrorResponse(Ssock_Handle ssock, int status)
{
    /* send a default response if there is no user callback */
    if (!HTTPSrv_errorResponseHook || !HTTPSrv_errorResponseHook(ssock, status)) {
        char buf[33];
        int len;

        len = esnprintf(buf, sizeof(buf), "Failed: %d %s", status,
                        getStatusMessage(status));
        //Log_print2(Diags_ANALYSIS, "len = %d: %s\n", len, (xdc_IArg)buf);
        HTTPSrv_sendResponse(ssock, status, CONTENT_TYPE_PLAIN, len, buf);
    }
}

void HTTPSrv_sendResponse(Ssock_Handle ssock, int status, const char * type,
                        size_t len, const void * buf)
{
    httpSendStatusLine(ssock, status, type);
    httpSendEntityLength(ssock, len);
    if (len > 0 && buf) {
        Ssock_send(ssock, buf, len, 0);
    }
}
