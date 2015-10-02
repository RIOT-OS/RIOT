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
#ifndef _HTTPSTD_H_
#define _HTTPSTD_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* HTTP Status Codes */
#define HTTP_CONTINUE                          100
#define HTTP_SWITCHING_PROTOCOLS               101
#define HTTP_OK                                200
#define HTTP_CREATED                           201
#define HTTP_ACCEPTED                          202
#define HTTP_NON_AUTHORITATIVE_INFORMATION     203
#define HTTP_NO_CONTENT                        204
#define HTTP_RESET_CONTENT                     205
#define HTTP_PARTIAL_CONTENT                   206
#define HTTP_MULTIPLE_CHOICES                  300
#define HTTP_MOVED_PERMANENTLY                 301
#define HTTP_FOUND                             302
#define HTTP_SEE_OTHER                         303
#define HTTP_NOT_MODIFIED                      304
#define HTTP_USE_PROXY                         305
#define HTTP_TEMPORARY_REDIRECT                307
#define HTTP_BAD_REQUEST                       400
#define HTTP_UNAUTHORIZED                      401
#define HTTP_PAYMENT_REQUIRED                  402
#define HTTP_FORBIDDEN                         403
#define HTTP_NOT_FOUND                         404
#define HTTP_METHOD_NOT_ALLOWED                405
#define HTTP_NOT_ACCEPTABLE                    406
#define HTTP_PROXY_AUTHENTICATION_REQUIRED     407
#define HTTP_REQUEST_TIMEOUT                   408
#define HTTP_CONFLICT                          409
#define HTTP_GONE                              410
#define HTTP_LENGTH_REQUIRED                   411
#define HTTP_PRECONDITION_FAILED               412
#define HTTP_REQUEST_ENTITY_TOO_LARGE          413
#define HTTP_REQUEST_URI_TOO_LONG              414
#define HTTP_UNSUPPORTED_MEDIA_TYPE            415
#define HTTP_REQUESTED_RANGE_NOT_SATISFAIABLE  416
#define HTTP_EXPECTATION_FAILED                417
#define HTTP_INTERNAL_SERVER_ERROR             500
#define HTTP_NOT_IMPLEMENTED                   501
#define HTTP_BAD_GATEWAY                       502
#define HTTP_SERVICE_UNAVAILABLE               503
#define HTTP_GATEWAY_TIMEOUT                   504
#define HTTP_HTTP_VERSION_NOT_SUPPORTED        505

/* Standard ports */
#define HTTP_PORT  80
#define HTTPS_PORT 443

static inline int is1xx(int status)
{
    return ((status >= 100) && (status < 200));
}

static inline int is2xx(int status)
{
    return ((status >= 200) && (status < 300));
}

static inline int is3xx(int status)
{
    return ((status >= 300) && (status < 400));
}

static inline int is4xx(int status)
{
    return ((status >= 400) && (status < 500));
}

static inline int is5xx(int status)
{
    return ((status >= 500) && (status < 600));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _HTTPSTD_H_ */

