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
#include <http/client/httpcli.h>
#include <http/client/common.h>

#if defined(__OSI__)/* __linux__ */
#include <osi.h>
#endif


/* Configurable lengths */
#define CONTENT_BUFLEN 128
#define URI_BUFLEN 128
#define SEND_BUFLEN 128
#define MAX_FIELD_NAME_LEN 24

#define STATUS_BUFLEN 16

#define HTTP_VER "HTTP/1.1"
#define HTTP_PREFIX "http://"
#define HTTPS_PREFIX "https://"

/* HTTP Client instance state flags */
#define READ_FLAG       (0x01)
#define REDIRECT_FLAG   (0x02)
#define CHUNKED_FLAG    (0x04)
#define INPROGRESS_FLAG (0x08)

typedef void (*ThreadFxnPtr)(HTTPCli_Handle cli);

static HTTPCli_SecureParams tlsParams = {0};
static struct sockaddr proxyAddr = {0};

static int checkContentField(HTTPCli_Handle cli, char *fname, char *fvalue,
        bool moreFlag);
static int getChunkedData(HTTPCli_Handle cli, char *body, int len,
        bool *moreFlag);
static bool getCliState(HTTPCli_Handle cli, int flag);
static int getStatus(HTTPCli_Handle cli);
static bool isValidSocket(int skt);
static int lookUpResponseFields(HTTPCli_Handle cli, char *field);
static int readLine(HTTPCli_Handle cli, char *line, int len, bool *moreFlag);
static void setCliState(HTTPCli_Handle cli, int flag, bool value);
static int skipLine(HTTPCli_Handle cli);
static int sprsend(HTTPCli_Handle cli, const char *fmt, ...);
static int stringCompare(const char *str1, const char *str2);
#ifndef __SLP__
static int stringCompareByLen(const char *str1, const char *str2, int len);
static int startSecureMode(HTTPCli_Handle cli);
#endif /* __SLP__ */
static unsigned char toLowerCase(unsigned char c);

#if defined(__NDK__) || defined(__linux__)
static int httpProxyTunnel(HTTPCli_Handle cli, const struct sockaddr *addr);
#endif /* __NDK__ || __linux__ */

#ifndef HTTPCli_LIBTYPE_MIN
static void asyncThread(HTTPCli_Handle cli);
static void contentHandler(HTTPCli_Handle cli, int status);
static int redirect(HTTPCli_Handle cli, int status);
static int threadCreate(ThreadFxnPtr fxn, void *cli);
static void threadExit();
#endif /* HTTPCli_LIBTYPE_MIN */

#ifdef __CYASSL__
static int xCyaSSLSend(const void *ctx, int s, const void *buf, size_t len,
        int flags)
{
    int ret;

    ret = CyaSSL_send((CYASSL *)ctx, buf, len, flags);
    if (ret < 0) {
        ret = CyaSSL_get_error((CYASSL *)ctx, ret);
    }

    return (ret);
}

static int xCyaSSLRecv(void *ctx, int s, void *buf, size_t len, int flags)
{
    int ret;

    if (flags & MSG_DONTWAIT) {
        ret = (CyaSSL_peek((CYASSL *)ctx, buf, len));
        if (ret < 0) {
            return (CyaSSL_get_error((CYASSL *)ctx, ret));
        }

        len = ret;
    }

    ret = CyaSSL_recv((CYASSL *)ctx, buf, len, flags);

    if (ret < 0) {
        ret = CyaSSL_get_error((CYASSL *)ctx, ret);
    }

    return (ret);
}

#endif /* __CYASSL__ */

#ifndef HTTPCli_LIBTYPE_MIN
/*
 *  ======== asyncThread ========
 *  Thread handling HTTP response asynchronously
 */
static void asyncThread(HTTPCli_Handle cli)
{
    int status;

    status = getStatus(cli);

    if (is2xx(status)  || (status == HTTP_NOT_MODIFIED)) {
        if (cli->chandle != NULL) {
            contentHandler(cli, status);
        }
        else {
            cli->shandle->handle2xx(cli, status);
        }
    }
    else if (is1xx(status)) {
        cli->shandle->handle1xx(cli, status);
    }
    else {
        cli->shandle->handle4xx(cli, status);
    }

    threadExit();
}

/*
 *  ======== contentHandler ========
 *  Invokes registered content handles based on response content type
 */
static void contentHandler(HTTPCli_Handle cli, int status)
{
    bool moreFlag = false;
    char respBuf[CONTENT_BUFLEN];
    int i = 0;
    int ret = 0;
    HTTPCli_ContentHandler *chandle;
    HTTPCli_ContentCallback callback = NULL;
    char **respFields = NULL;
    const char *fields[2] = {
        HTTPCli_FIELD_NAME_CONTENT_TYPE,
        NULL
    };

    if ((status == HTTP_OK) && cli->chandle) {
        /* Save the previously set field Ids by the app */
        respFields = HTTPCli_setResponseFields(cli, fields);

        do {
            ret = HTTPCli_getResponseField(cli, respBuf, sizeof(respBuf),
                    &moreFlag);

            /* Zero is the index to HTTPCli_FIELD_NAME_CONTENT_TYPE */
            if (ret == 0) {
                if (moreFlag) {
                    status = HTTPCli_ECONTENTTYPELONG;
                    cli->shandle->handle2xx(cli, status);
                    return;
                }

                chandle = cli->chandle;
                for (i = 0; chandle[i].contentType != NULL; i++) {
                    ret = strncmp(chandle[i].contentType, respBuf,
                            sizeof(respBuf));
                    if (ret == 0) {
                        callback = chandle[i].handle;
                        break;
                    }
                }
            }
        }
        while (ret != HTTPCli_FIELD_ID_END);

        if (respFields) {
            HTTPCli_setResponseFields(cli, (const char **)respFields);
        }
        else {
            /* force assign */
            cli->respFields = respFields;
        }

        if (callback == NULL) {
            status = HTTPCli_ENOCONTENTCALLBACK;
            cli->shandle->handle2xx(cli, status);
            return;
        }

        do {
            moreFlag = false;
            ret = HTTPCli_readResponseBody(cli, respBuf, sizeof(respBuf),
                    &moreFlag);
            if (ret < 0) {
                status = ret;
            }
            i = callback(cli, status, respBuf, ret, moreFlag);
        }
        while ((ret > 0) && moreFlag && i);

        /* If the app wants to flush the remaining data */
        if ((ret >= 0) && (i == 0)) {
            do {
                ret = HTTPCli_readResponseBody(cli, respBuf, sizeof(respBuf),
                        &moreFlag);
                if (ret < 0) {
                    status = ret;
                    callback(cli, status, respBuf, ret, moreFlag);
                }
            }
            while ((ret > 0) && moreFlag);
        }
    }
    else {
        cli->shandle->handle2xx(cli, status);
    }

}
#endif /* HTTPCli_LIBTYPE_MIN */

/*
 *  ======== bufferedRecv ========
 */
ssize_t bufferedRecv(HTTPCli_Handle cli, Ssock_Handle ssock,
        void *buf, size_t len, int flags)
{
    ssize_t numRead = 0;

#ifdef __SLP__
    if (len > HTTPCli_BUF_LEN) {
        return (HTTPCli_EINTERNALBUFSMALL);
    }

    if (cli->buflen < len) {
        if (cli->buflen) {
            memcpy(buf, cli->bufptr, cli->buflen);
            buf = (uint8_t *)buf + cli->buflen;
        }

        numRead = Ssock_recv(ssock, cli->buf, HTTPCli_BUF_LEN, MSG_PEEK);
        if (numRead > 0) {
            numRead = Ssock_recv(ssock, cli->buf, numRead, flags);
        }

        if (numRead >= 0) {
            if (numRead < (len - cli->buflen)) {
                if (cli->buflen) {
                    memcpy(buf, cli->buf, numRead);
                    /* Not enough bytes yet, copy into the internal buf */
                    cli->buflen += numRead;
                    memcpy(cli->buf, buf, cli->buflen);
                }
                else {
                    cli->buflen = numRead;
                }

                cli->bufptr = cli->buf;
                numRead = HTTPCli_EINPROGRESS;
            }
            else {
                memcpy(buf, cli->buf, (len - cli->buflen));
                cli->bufptr = cli->buf + (len - cli->buflen);
                cli->buflen = numRead - (len - cli->buflen);
                numRead = len;
            }
        }
        else if (numRead == Ssock_TIMEOUT) {
            numRead = HTTPCli_EINPROGRESS;
        }
        else {
            numRead = HTTPCli_ERECVFAIL;
        }
    }
    else {
        memcpy(buf, cli->bufptr, len);
        cli->bufptr += len;
        cli->buflen -= len;
        numRead = len;
    }

#else /* __SLP__ */

    if (cli->buflen) {
        if (len > cli->buflen) {
            memcpy(buf, cli->bufptr, cli->buflen);

            numRead = Ssock_recv(ssock, ((uint8_t *)buf + cli->buflen),
                    (len - cli->buflen), flags);
            if (numRead >= 0) {
                numRead += cli->buflen;
                cli->buflen = 0;
            }
        }
        else {
            memcpy(buf, cli->bufptr, len);
            cli->buflen -= len;
            cli->bufptr += len;
            numRead = len;
        }
    }
    else {
        if (len > HTTPCli_BUF_LEN) {
            numRead = Ssock_recv(ssock, buf, len, flags);
        }
        else {
            numRead = Ssock_recv(ssock, cli->buf, HTTPCli_BUF_LEN,
                    MSG_DONTWAIT);
            if (numRead > 0) {
                cli->bufptr = cli->buf;
                cli->buflen = numRead;
            }

            if (numRead == Ssock_TIMEOUT || (numRead >= 0 && numRead < (ssize_t)len)) {
                if (numRead == Ssock_TIMEOUT) {
                    numRead = 0;
                }

                numRead = Ssock_recv(ssock, (cli->buf + numRead),
                        (len - numRead), flags);
                if (numRead > 0) {
                    cli->buflen += numRead;
                }
            }

            if (numRead >= 0) {
                if (len > cli->buflen) {
                    len = cli->buflen;
                }

                memcpy(buf, cli->buf, len);
                cli->buflen -= len;
                cli->bufptr += len;
                numRead = len;
            }
        }
    }

#endif /* __SLP__ */

    if (numRead == Ssock_TIMEOUT) {
        return (HTTPCli_ERECVFAIL);
    }

    return (numRead);
}

/*
 *  ======== checkContentField ========
 *  Check field for information on content length/transfer encoding
 */
static int checkContentField(HTTPCli_Handle cli, char *fname, char *fvalue,
        bool moreFlag)
{
    const char chunk[] = "chunked";

    if ((strcmp(fname, HTTPCli_FIELD_NAME_TRANSFER_ENCODING) == 0)
            && (strcmp(chunk, fvalue) == 0)) {
        setCliState(cli, CHUNKED_FLAG, true);
    }
    else if (strcmp(fname, HTTPCli_FIELD_NAME_CONTENT_LENGTH) == 0) {
        if (moreFlag) {
            return (HTTPCli_ECONTENTLENLARGE);
        }
        cli->clen = strtoul(fvalue, NULL, 10);
    }

    return (0);
}

/*
 *  ======== getChunkedData ========
 *  Get chunked transfer encoded data
 */
static int getChunkedData(HTTPCli_Handle cli, char *body, int len,
        bool *moreFlag)
{
    bool lastFlag = true;
    bool mFlag = false;
    char crlf;
    int ret = 0;
    unsigned long chunkLen;

    if (!(getCliState(cli, CHUNKED_FLAG))) {
        return (HTTPCli_ENOTCHUNKDATA);
    }

    *moreFlag = true;

    /* Check if in the middle of reading respone body? */
    chunkLen = cli->clen;
    if (chunkLen == 0) {
        ret = readLine(cli, body, len, &mFlag);
        if (ret < 0) {
            return (ret);
        }

        chunkLen = strtoul(body, NULL, 16);
        /* don't need the remaining buffer */
        if (mFlag) {
            ret = readLine(cli, body, len, &mFlag);
            if (ret < 0) {
                return (ret);
            }
        }

        if (chunkLen == 0) {
            /* skip the rest (trailer headers) */
            do {
                ret = readLine(cli, body, len, &mFlag);
                if (ret < 0) {
                    return (ret);
                }

                /* Avoids fake do-while check */
                if (lastFlag) {
                    body[0] = 'a';
                    lastFlag = false;
                }

                if (mFlag) {
                    lastFlag = true;
                }

            }
            while (body[0] == '\0');

            *moreFlag = false;
            cli->clen = 0;

            return (0);
        }

        cli->clen = chunkLen;
    }

    if (chunkLen < (unsigned long)len) {
        len = chunkLen;
    }

    ret = HTTPCli_readRawResponseBody(cli, body, len);
    if (ret > 0) {
        cli->clen -= ret;
        /* Clean up the CRLF */
        if (cli->clen == 0) {
            chunkLen = readLine(cli, &crlf, sizeof(crlf), &mFlag);
            if (chunkLen != 0) {
                return (chunkLen);
            }
        }
    }

    return (ret);
}

/*
 *  ======== getCliState ========
 *  Get the state of HTTP client instance for the input flag
 */
static bool getCliState(HTTPCli_Handle cli, int flag)
{
    return ((cli->state) & flag);
}

/*
 *  ======== getStatus ========
 *  Processes the response line to get the status
 */
static int getStatus(HTTPCli_Handle cli)
{
    bool moreFlag = false;
    char statusBuf[STATUS_BUFLEN];
    int status;
    int ret;
    int vlen;
    int rflag;

    do {
        /* Set redirect repeat getStatus flag to zero */
        rflag = 0;

        vlen = sizeof(HTTP_VER) - 1;
        ret = bufferedRecv(cli, &(cli->ssock), statusBuf, vlen, 0);
        if (ret < 0) {
            return (ret);
        }

        /* match HTTP/1. */
        if (strncmp(statusBuf, HTTP_VER, (vlen - 1))) {
            /* not a valid HTTP header - give up */
            return (HTTPCli_ERESPONSEINVALID);
        }
        /* get the numeric status code (and ignore the readable status) */
        status = readLine(cli, statusBuf, sizeof(statusBuf), &moreFlag);

        /* Skip the rest of the status */
        if ((status == 0) && moreFlag) {
            status = skipLine(cli);
        }

        if (status == 0) {
            status = strtoul(statusBuf, NULL, 10);
        }

#ifndef HTTPCli_LIBTYPE_MIN
        if (cli->rhandle && is3xx(status)) {
            rflag = redirect(cli, status);
            if (rflag < 0) {
                status = rflag;
                rflag = 0;
            }
        }
#endif /* HTTPCli_LIBTYPE_MIN */
    }
    while (rflag);

    return (status);
}


/*
 *  ======== httpProxyTunnel ========
 *  HTTP tunnel through proxy
 */
#if defined(__NDK__) || defined(__linux__)
static int httpProxyTunnel(HTTPCli_Handle cli, const struct sockaddr *addr)
{
    bool moreFlag;
    char tmpBuf[STATUS_BUFLEN];
    char ip[INET6_ADDRSTRLEN];
    int ret = 0;
    int port;
    void *saddr;

    if (addr->sa_family == AF_INET6) {
        saddr = &(((struct sockaddr_in6 *)addr)->sin6_addr);
        port = ntohs(((struct sockaddr_in6 *)addr)->sin6_port);
    }
    else {
        saddr = &(((struct sockaddr_in *)addr)->sin_addr);
        port = ntohs(((struct sockaddr_in *)addr)->sin_port);
    }

    inet_ntop(addr->sa_family, saddr, ip, INET6_ADDRSTRLEN);

    ret = sprsend(cli, "%s %s:%d %s\r\n", HTTPCli_METHOD_CONNECT, ip, port,
            HTTP_VER);
    if (ret < 0) {
        return (ret);
    }

    ret = sprsend(cli, "%s: %s\r\n", HTTPCli_FIELD_NAME_HOST, ip);
    if (ret < 0) {
        return (ret);
    }

    ret = HTTPCli_sendField(cli, NULL, NULL, true);
    if (ret < 0) {
        return (ret);
    }

    ret = getStatus(cli);
    if (ret != HTTP_OK) {
        return (ret);
    }

    /* Drop all the fields */
    do {
        ret = readLine(cli, tmpBuf, sizeof(tmpBuf), &moreFlag);
        if (ret < 0) {
            return (ret);
        }
    } while (tmpBuf[0] != '\0');

    return (0);
}
#endif /* __NDK__ || __linux__ */

/*
 *  ======== isValidSocket ========
 */
static bool isValidSocket(int skt)
{
#if defined (__SL__) || defined (__SLP__)
    return (skt >= 0);
#else /* __SL__ || __SLP__ */
    return (skt != -1);
#endif /* __SL__ || __SLP__ */
}

/*
 *  ======== lookUpResponseFields ========
 *  Is the input field in the set response field array?
 */
static int lookUpResponseFields(HTTPCli_Handle cli, char *field)
{
    char **respFields = cli->respFields;
    int index;

    if (respFields && field) {
        for (index = 0; respFields[index] != NULL; index++) {
             if (stringCompare(field, respFields[index]) == 0) {
               return (index);
            }
        }
    }

    return (-1);
}

/*
 *  ======== readLine ========
 *  Read a header line
 */
static int readLine(HTTPCli_Handle cli, char *line, int len, bool *moreFlag)
{
   char c;
   int i = 0;
   int ret;

    *moreFlag = true;
    setCliState(cli, READ_FLAG, true);

    for (i = 0; i < len; i++) {

        ret = bufferedRecv(cli, &(cli->ssock), &c, 1, 0);
        if (ret < 0) {
            *moreFlag = false;
            setCliState(cli, READ_FLAG, false);
            return (ret);
        }

        if (c == '\n') {
            line[i] = 0;
            /* Line read completed */
            *moreFlag = false;
            setCliState(cli, READ_FLAG, false);
            break;
        }
        else if ((c == '\r') || (i == 0 && c == ' ')) {
            i--;
            /* drop CR or drop leading SP*/
        }
        else {
            line[i] = c;
        }

    }

    return (0);
}

#ifndef HTTPCli_LIBTYPE_MIN
static int redirect(HTTPCli_Handle cli, int status)
{
    bool moreFlag = false;
    char uri[URI_BUFLEN] = {0};
    char buf[16] = {0};
    int ret = 0;
    char **respFields;
    const char *fields[2] = {
        HTTPCli_FIELD_NAME_LOCATION,
        NULL
    };

    switch (status) {
        case HTTP_MOVED_PERMANENTLY:
        case HTTP_FOUND:
        case HTTP_SEE_OTHER:
        case HTTP_USE_PROXY:
        case HTTP_TEMPORARY_REDIRECT:
            respFields = HTTPCli_setResponseFields(cli, fields);

            ret = HTTPCli_getResponseField(cli, uri, sizeof(uri),
                    &moreFlag);
            if (ret < 0) {
                return (ret);
            }

            if (moreFlag) {
                return (HTTPCli_EREDIRECTURILONG);
            }

            do {
                ret = HTTPCli_getResponseField(cli, buf, sizeof(buf),
                        &moreFlag);
                if ((ret != HTTPCli_FIELD_ID_END) && (ret < 0)) {
                    return (ret);
                }
            }
            while (ret != HTTPCli_FIELD_ID_END);

            if (respFields) {
                HTTPCli_setResponseFields(cli, (const char **)respFields);
            }
            else {
                /* force assign */
                cli->respFields = respFields;
            }

            do {
                ret = HTTPCli_readResponseBody(cli, buf, sizeof(buf),
                        &moreFlag);
                if (ret < 0) {
                    return (ret);
                }
            }
            while ((ret > 0) && moreFlag);

            setCliState(cli, REDIRECT_FLAG, true);
            cli->rhandle(cli, status, uri);
            setCliState(cli, REDIRECT_FLAG, false);

            ret = 1;
            break;

        default:
            ret = 0;
            break;
    }

    return (ret);
}
#endif /* HTTPCli_LIBTYPE_MIN */

/*
 *  ======== startSecureMode ========
 */
#ifndef __SLP__
static int startSecureMode(HTTPCli_Handle cli)
{
#ifdef __CYASSL__
    int skt = 0;
    CYASSL *ssl;
    Ssock_SecureFxns sFxn;

    if ((ssl = CyaSSL_new(tlsParams.ctx)) == NULL) {
        return (HTTPCli_ETLSFAIL);
    }

    skt = Ssock_getSocket(&(cli->ssock));
    CyaSSL_set_fd(ssl, skt);

    sFxn.send = &xCyaSSLSend;
    sFxn.recv = &xCyaSSLRecv;
    sFxn.del = (Ssock_DeleteFxn)&CyaSSL_free;
    sFxn.encrypt = NULL;
    sFxn.decrypt = NULL;
    sFxn.extraBytes = 0;

    if (Ssock_startSecure(&(cli->ssock), &sFxn, ssl) == -1) {
        CyaSSL_free(ssl);
        return (HTTPCli_ETLSFAIL);
    }

    return (0);

#elif defined(__SL__) /* __CYASSL__ */
    int skt;

    skt =  Ssock_getSocket(&(cli->ssock));

    if (tlsParams.method.secureMethod != 0) {
        if (setsockopt(skt, SL_SOL_SOCKET, SL_SO_SECMETHOD,
                &(tlsParams.method.secureMethod),
                sizeof(tlsParams.method.secureMethod)) < 0) {
            return (HTTPCli_ETLSFAIL);
        }
    }

    if (tlsParams.mask.secureMask != 0) {
        if (setsockopt(skt, SL_SOL_SOCKET, SL_SO_SECURE_MASK,
                &(tlsParams.mask.secureMask), sizeof(tlsParams.mask.secureMask))
                 < 0) {
            return (HTTPCli_ETLSFAIL);
        }
    }

    if (tlsParams.cafile[0] != 0) {
        if (setsockopt(skt, SL_SOL_SOCKET, SL_SO_SECURE_FILES_CA_FILE_NAME,
                tlsParams.cafile, HTTPCli_CERT_NAME_LEN)  < 0) {
            return (HTTPCli_ETLSFAIL);
        }
    }

    if (tlsParams.privkey[0] != 0) {
        if (setsockopt(skt, SL_SOL_SOCKET,
                SL_SO_SECURE_FILES_PRIVATE_KEY_FILE_NAME,
                tlsParams.privkey, HTTPCli_CERT_NAME_LEN)  < 0) {
            return (HTTPCli_ETLSFAIL);
        }
    }

    if (tlsParams.cert[0] != 0) {
        if (setsockopt(skt, SL_SOL_SOCKET,
                SL_SO_SECURE_FILES_CERTIFICATE_FILE_NAME,
                tlsParams.cert, HTTPCli_CERT_NAME_LEN)  < 0) {
            return (HTTPCli_ETLSFAIL);
        }
    }

    if (tlsParams.dhkey[0] != 0) {
        if (setsockopt(skt, SL_SOL_SOCKET, SL_SO_SECURE_FILES_DH_KEY_FILE_NAME,
                tlsParams.dhkey, HTTPCli_CERT_NAME_LEN)  < 0) {
            return (HTTPCli_ETLSFAIL);
        }
    }

    return (0);

#else /* __CYASSL_ */
    (void)tlsParams;
    return (HTTPCli_ETLSFAIL);

#endif /* __CYASSL__ */
}
#endif /* __SLP__ */

/*
 *  ======== setCliState ========
 *  Set or clear the flag in the HTTP client instance
 */
static void setCliState(HTTPCli_Handle cli, int flag, bool value)
{
    if (value) {
        cli->state |= flag;
    }
    else {
        cli->state &= ~flag;
    }
}

/*
 *  ======== skipLine ========
 *  Skip the rest of the header line
 */
static int skipLine(HTTPCli_Handle cli)
{
    char c = 0;
    int ret;

    while (c != '\n') {
        ret = bufferedRecv(cli, &(cli->ssock), &c, 1, 0);
        if (ret < 0) {
            return (ret);
        }
    }

    return (0);
}

/*
 *  ======== sprsend ========
 *  Constructs a HTTP Request line to send
 */
static int sprsend(HTTPCli_Handle cli, const char * fmt, ...)
{
    char sendbuf[SEND_BUFLEN];
    int len = 0;
    int ret;
    int sendbuflen = sizeof(sendbuf);
    va_list ap;

    if (!getCliState(cli, INPROGRESS_FLAG)) {
        va_start(ap, fmt);
        len = xvsnprintf(sendbuf, sendbuflen, fmt, ap);
        va_end(ap);
    }
#ifdef __SLP__
    else {
        memcpy(sendbuf, cli->buf, cli->buflen);
        len = cli->buflen;
    }
#endif /* __SLP__ */

    if (len > sendbuflen) {
        return (HTTPCli_ESENDBUFSMALL);
    }

    ret = Ssock_send(&(cli->ssock), sendbuf, len, 0);
    if (ret < 0) {
        return (HTTPCli_ESENDFAIL);
    }

#ifdef __SLP__
    if (ret < len) {
        cli->buflen = len - ret;
        if (cli->buflen > HTTPCli_BUF_LEN) {
            return (HTTPCli_EINTERNALBUFSMALL);
        }

        memcpy(cli->buf, ((uint8_t *)sendbuf + ret), cli->buflen);
        return (HTTPCli_EINPROGRESS);
    }
    cli->buflen = 0;

#endif /* __SLP__ */

    return (ret);
}

/*
 *  ======== stringCompare ========
 *  Case insensitive string compare
 */
static int stringCompare(const char *str1, const char *str2)
{
   unsigned char c1;
   unsigned char c2;

   for(;;) {
        c1 = toLowerCase((unsigned char)*str1);
        c2 = toLowerCase((unsigned char)*str2);
        if (c1 != 0 && c1 == c2) {
            str1++;
            str2++;
            continue;
        }
        return (int)c1 - (int)c2;
   }
}

/*
 *  ======== stringCompareByLen ========
 *  Case insensitive string compare upto the input length
 */
#ifndef __SLP__
static int stringCompareByLen(const char *str1, const char *str2, int len)
{
   unsigned char c1;
   unsigned char c2;
   int i;

   for(i = 0; i < len; i++) {
        c1 = toLowerCase((unsigned char)*str1);
        c2 = toLowerCase((unsigned char)*str2);
        if (c1 != 0 && c1 == c2) {
            str1++;
            str2++;
            continue;
        }
        return (int)c1 - (int)c2;
   }

   return (0);
}
#endif /* __SLP__ */

#ifndef HTTPCli_LIBTYPE_MIN
/*
 *  ======== threadCreate ========
 */
static int threadCreate(ThreadFxnPtr fxn, void *cli)
{
#if defined(__linux__)
    pthread_t thread;
    int ret;

    ret = pthread_create(&thread, NULL, (void *)fxn, cli);
    if (ret != 0) {
        return (HTTPCli_ETHREADFAIL);
    }

    pthread_detach(thread);

#elif defined(__OSI__)/* __linux__ */
    long lRetVal = -1;
    
    lRetVal = osi_TaskCreate((P_OSI_TASK_ENTRY) fxn,
                    (const signed char *)"Async HTTP",
                    ((HTTPCli_Handle)cli)->stackSize,
                    cli, 
                    ((HTTPCli_Handle)cli)->priority, 
                    NULL );
    if(lRetVal < 0)
    {
      return (HTTPCli_ETHREADFAIL);
    }
    
#else
    
    Task_Handle asyncHandle = NULL;
    Task_Params asyncParams;
    Task_Params_init(&asyncParams);

/*    if (asyncHandle != NULL) {
        while (Task_getMode(asyncHandle) != Task_Mode_TERMINATED);
        Task_delete(&asyncHandle);
    }
*/
    asyncParams.arg0 = (UArg)cli;
    asyncHandle = Task_create((Task_FuncPtr)fxn, &asyncParams, NULL);
    if (asyncHandle == NULL) {
        return (HTTPCli_ETHREADFAIL);
    }
#endif /* __linux__ */

    return (0);
}

/*
 *  ======== threadExit ========
 */
static void threadExit()
{
#if defined(__linux__)
    pthread_exit(NULL);
#elif defined(__OSI__)    
    osi_TaskDelete(NULL);
#else
    Task_exit();
#endif
}

#endif /* HTTPCli_LIBTYPE_MIN */

/*
 *  ======== toLowerCase ========
 */
static unsigned char toLowerCase(unsigned char c)
{
    if (c >= 65 && c <= 90) {
        c += 32;
    }

    return (c);
}

/*
 *  ======== HTTPCli_initSockAddr ========
 */
#ifndef __SLP__
int HTTPCli_initSockAddr(struct sockaddr *addr, const char *uri, int flags)
{
    char luri[URI_BUFLEN];
    char *domain;
    char *colon;
    int dlen;
    int port = 80;
    xassert(addr != NULL);
    xassert(uri != NULL);

    dlen = strlen(uri);
    if (dlen >= URI_BUFLEN) {
        return (HTTPCli_EURILENLONG);
    }

    strcpy(luri, uri);
    domain = luri;

    if (stringCompareByLen(HTTP_PREFIX, domain, (sizeof(HTTP_PREFIX) - 1))
            == 0) {
        port = 80;
        domain = domain + sizeof(HTTP_PREFIX) - 1;
    }
    else if (stringCompareByLen(HTTPS_PREFIX, domain,
            (sizeof(HTTPS_PREFIX) - 1)) == 0) {
        port = 443;
        domain = domain + sizeof(HTTPS_PREFIX) - 1;
    }
    colon = strchr(domain, ':');
    if (colon != NULL) {
        dlen = colon - domain;
        port = strtoul((colon + 1), NULL, 10);
        domain[dlen] = '\0';
    }
    else {
        dlen = strlen(domain);
    }

#ifdef __SL__
    int ret = 0;
    struct sockaddr_in taddr = {0};
    unsigned long ip;

    /* SL supports IPv4 DNS only */
    if (flags & HTTPCli_TYPE_IPV6) {
        return (HTTPCli_EHOSTNAME);
    }
    else {
        ret = gethostbyname((signed char *)domain, dlen, &ip, AF_INET);
        if (ret < 0) {
            return (HTTPCli_EHOSTNAME);
        }

        taddr.sin_family = AF_INET;
        taddr.sin_port = htons(port);
        taddr.sin_addr.s_addr = htonl(ip);
        *addr = *((struct sockaddr *)&taddr);
    }

    return (0);

#elif defined(__linux__)  || defined(__NDK__)/* __SL__ */
    int ret = 0;
    struct addrinfo hints;
    struct addrinfo *addrs;

    memset(&hints, 0, sizeof(struct addrinfo));
    if (flags & HTTPCli_TYPE_IPV6) {
        hints.ai_family = AF_INET6;
    }
    else {
        hints.ai_family = AF_INET;
    }

    ret = getaddrinfo(domain, "0", &hints, &addrs);
    if (ret != 0) {
        return (HTTPCli_EHOSTNAME);
    }

    *addr = *(addrs->ai_addr);

    if (flags & HTTPCli_TYPE_IPV6) {
        ((struct sockaddr_in6 *)addr)->sin6_port = htons(port);
    }
    else {
        ((struct sockaddr_in *)addr)->sin_port = htons(port);
    }
    freeaddrinfo(addrs);

    return (0);

#else  /* __SL__ */

    /* Not supported */
    return (HTTPCli_EHOSTNAME);
#endif /* __SL__ */
}
#endif /* __SLP__ */

/*
 *  ======== HTTPCli_connect ========
 */
int HTTPCli_connect(HTTPCli_Struct *cli, const struct sockaddr *addr,
        int flags, const HTTPCli_Params *params)
{
    int skt;
    int ret;
    int slen;
    int sopt = 0;
    struct timeval tv;
    struct sockaddr *sa;
    Ssock_Struct ssock;

    xassert(cli != NULL);
    xassert(addr != NULL);

    if (proxyAddr.sa_family != 0) {
        sa = &proxyAddr;
    }
    else {
        sa = (struct sockaddr *)addr;
    }

    if (sa->sa_family == AF_INET6) {
        slen = sizeof(struct sockaddr_in6);
    }
    else {
        slen = sizeof(struct sockaddr_in);
    }

    if (!getCliState(cli, INPROGRESS_FLAG)) {

#ifndef HTTPCli_LIBTYPE_MIN
        if (params != NULL) {
            cli->shandle = params->shandle;
            cli->chandle = params->chandle;
            cli->rhandle = params->rhandle;

#ifndef __linux__
            cli->stackSize = params->stackSize;
            cli->priority  = params->priority;
#endif /* __linux__ */

            if (cli->chandle) {
                xassert(cli->shandle != NULL);
            }

            if (cli->shandle) {
                xassert(params->shandle->handle1xx != NULL);
                xassert(params->shandle->handle2xx != NULL);
                xassert(params->shandle->handle4xx != NULL);
            }
        }
#endif /* HTTPCli_LIBTYPE_MIN */

#ifdef __SL__
        if (flags & HTTPCli_TYPE_TLS) {
            sopt = SL_SEC_SOCKET;
        }
#endif /* __SL__ */

        skt = socket(sa->sa_family, SOCK_STREAM, sopt);
        if (isValidSocket(skt)) {
            Ssock_construct(&ssock, skt);
            cli->ssock = ssock;

            if (params != NULL && params->timeout) {
                tv.tv_sec = params->timeout;
                tv.tv_usec = 0;
                if (setsockopt(skt, SOL_SOCKET, SO_RCVTIMEO, &tv,
                        sizeof(tv)) < 0) {
                    HTTPCli_disconnect(cli);

                    return (HTTPCli_ESOCKETFAIL);
                }
            }

#ifdef __SL__
            /* In SL, the secure params have to be set before connect */
            if (flags & HTTPCli_TYPE_TLS) {
                ret = startSecureMode(cli);
                if (ret < 0) {
                    HTTPCli_disconnect(cli);

                    return (ret);
                }
            }
#endif /*__SL__*/
        }
        else {
            return (HTTPCli_ESOCKETFAIL);
        }
    }

    ret = connect(skt, sa, slen);
    if (ret < 0) {
#ifdef __SLP__
        if (errno(skt) == EINPROGRESS) {
            setCliState(cli, INPROGRESS_FLAG, 1);

            return (HTTPCli_EINPROGRESS);
        }
#endif /* __SLP__ */
        HTTPCli_disconnect(cli);

        return (HTTPCli_ECONNECTFAIL);
    }
    setCliState(cli, INPROGRESS_FLAG, 0);

#if !defined(__SL__) && !defined(__SLP__)
    /* In CYASSL, the secure params are set after connect */
    if (flags & HTTPCli_TYPE_TLS) {

        /* Tunnel using HTTP CONNECT for TLS through proxy */
        if (proxyAddr.sa_family != 0)  {
            ret = httpProxyTunnel(cli, addr);
            if (ret < 0) {
                return (HTTPCli_EPROXYTUNNELFAIL);
            }
        }

        ret = startSecureMode(cli);
        if (ret < 0) {
            HTTPCli_disconnect(cli);

            return (ret);
        }
    }

#endif /* __SL__  || __SLP__ */

#ifdef __SLP__
    if (params->rnotify) {
        ret = setsocknotify(skt, SO_READNOTIFY, params->rnotify, cli);
        if (ret < 0) {
            return (HTTPCli_ESETNOTIFYFAIL);
        }
    }

    if (params->wnotify) {
        ret = setsocknotify(skt, SO_WRITENOTIFY, params->wnotify, cli);
        if (ret < 0) {
            return (HTTPCli_ESETNOTIFYFAIL);
        }
    }

    if (params->enotify) {
        ret = setsocknotify(skt, SO_EXCEPTNOTIFY, params->enotify, cli);
        if (ret < 0) {
            return (HTTPCli_ESETNOTIFYFAIL);
        }
    }
#endif /* __SLP__ */


    return (0);
}

/*
 *  ======== HTTPCli_construct ========
 */
void HTTPCli_construct(HTTPCli_Handle cli)
{
    xassert(cli != NULL);

    memset(&(cli->ssock), 0, sizeof(Ssock_Struct));
    cli->state = 0;
    cli->fields = NULL;
    cli->respFields = NULL;
    cli->clen = 0;
    cli->buflen = 0;
    cli->bufptr = cli->buf;

#ifndef HTTPCli_LIBTYPE_MIN
    cli->shandle = NULL;
    cli->chandle = NULL;
    cli->rhandle = NULL;
#ifndef __linux__
    cli->stackSize = 0;
    cli->priority = 0;
#endif /* __linux__ */
#endif /* HTTPCli_LIBTYPE_MIN */
}

/*
 *  ======== HTTPCli_create ========
 */
HTTPCli_Handle HTTPCli_create()
{
    HTTPCli_Handle cli;

    cli = (HTTPCli_Handle)xmalloc(sizeof(HTTPCli_Struct));
    if (cli != NULL) {
        HTTPCli_construct(cli);
    }

    return (cli);
}

/*
 *  ======== HTTPCli_delete ========
 */
void HTTPCli_delete(HTTPCli_Handle cli)
{
    xassert(cli != NULL);

    HTTPCli_destruct(cli);
    xfree(cli);
    cli = NULL;
}

/*
 *  ======== HTTPCli_destruct ========
 */
void HTTPCli_destruct(HTTPCli_Struct *cli)
{
    xassert(cli != NULL);

    memset(&(cli->ssock), 0, sizeof(Ssock_Struct));
    cli->state = 0;
    cli->clen = 0;
    cli->fields = NULL;
    cli->respFields = NULL;
    cli->buflen = 0;
    cli->bufptr = cli->buf;


#ifndef HTTPCli_LIBTYPE_MIN
    cli->shandle = NULL;
    cli->chandle = NULL;
    cli->rhandle = NULL;
#ifndef __linux__
    cli->stackSize = 0;
    cli->priority = 0;
#endif /* __linux__ */
#endif /* HTTPCli_LIBTYPE_MIN */
}

/*
 *  ======== HTTPCli_disconnect ========
 */
void HTTPCli_disconnect(HTTPCli_Struct *cli)
{
    int skt;

    xassert(cli != NULL);

    skt = Ssock_getSocket(&(cli->ssock));
    Ssock_destruct(&(cli->ssock));
    close(skt);

    HTTPCli_destruct(cli);
}

/*
 *  ======== HTTPCli_setRequestFields ========
 */
HTTPCli_Field *HTTPCli_setRequestFields(HTTPCli_Handle cli,
        const HTTPCli_Field *fields)
{
    HTTPCli_Field *prevField;

    xassert(cli != NULL);

    prevField = cli->fields;
    if (fields) {
        cli->fields = (HTTPCli_Field *)fields;
    }

    return (prevField);
}

/*
 *  ======== HTTPCli_setResponseFields ========
 */
char **HTTPCli_setResponseFields(HTTPCli_Handle cli, const char *fields[])
{
    char **prevField;

    xassert(cli != NULL);

    prevField = cli->respFields;
    if (fields) {
        cli->respFields = (char **)fields;
    }

    return (prevField);
}

/*
 *  ======== HTTPCli_setSecureParams ========
 */
void HTTPCli_setSecureParams(HTTPCli_SecureParams *sparams)
{
    xassert(sparams != NULL);

    tlsParams = *sparams;
}

/*
 *  ======== HTTPCli_sendRequest ========
 */
int HTTPCli_sendRequest(HTTPCli_Handle cli, const char *method,
        const char *requestURI, bool moreFlag)
{
    int i = 0;
    int ret;
    HTTPCli_Field *fields = NULL;

    xassert(cli != NULL);
    xassert(method != NULL);
    xassert(requestURI != NULL);

    ret = sprsend(cli, "%s %s %s\r\n", method, requestURI? requestURI : "/",
            HTTP_VER);
    if (ret < 0) {
        return (ret);
    }

    if (cli->fields) {
        fields = cli->fields;
        for (i = 0; fields[i].name != NULL; i++) {
            ret = HTTPCli_sendField(cli, fields[i].name, fields[i].value,
                    false);
            if (ret < 0) {
                return (ret);
            }
        }
    }

    if (!moreFlag) {
        ret = HTTPCli_sendField(cli, NULL, NULL, true);
        if (ret < 0) {
            return (ret);
        }
    }

    return (0);
}

/*
 *  ======== HTTPCli_sendField ========
 */
int HTTPCli_sendField(HTTPCli_Handle cli, const char *name,
        const char *value, bool lastFlag)
{
    int ret;

    xassert(cli != NULL);

    if (name != NULL) {
        ret = sprsend(cli, "%s: %s\r\n", name, value);
        if (ret < 0) {
            return (ret);
        }
    }

    if (lastFlag) {
        ret = sprsend(cli, "\r\n");
        if (ret < 0) {
            return (ret);
        }

#ifndef HTTPCli_LIBTYPE_MIN
        if (cli->shandle && !(getCliState(cli, REDIRECT_FLAG))) {
            ret = threadCreate(asyncThread, (void *)cli);
            if (ret < 0) {
                return (ret);
            }
        }
#endif /* HTTPCli_LIBTYPE_MIN */
    }

    return (0);
}

/*
 *  ======== HTTPCli_sendRequestBody ========
 */
int HTTPCli_sendRequestBody(HTTPCli_Handle cli, const char *body, int len)
{
    int ret;

    xassert(cli != NULL);

    ret = Ssock_send(&(cli->ssock), body, len, 0);
    if (ret < 0) {
        return (ret);
    }

    return (0);
}

/*
 *  ======== HTTPCli_getResponseStatus ========
 */
int HTTPCli_getResponseStatus(HTTPCli_Handle cli)
{
    int ret;

    xassert(cli != NULL);

#ifndef HTTPCli_LIBTYPE_MIN
    if (cli->shandle) {
        return (HTTPCli_EASYNCMODE);
    }
#endif /* HTTPCli_LIBTYPE_MIN */

    ret = getStatus(cli);

    return (ret);
}

/*
 *  ======== HTTPCli_getResponseField ========
 */
int HTTPCli_getResponseField(HTTPCli_Handle cli, char *body, int len,
        bool *moreFlag)
{
    char c;
    char name[MAX_FIELD_NAME_LEN] = {0};
    int respFieldIndex = HTTPCli_FIELD_ID_DUMMY;
    int i;
    int ret;

    xassert(cli != NULL);
    xassert(body != NULL);
    xassert(moreFlag != NULL);

    /* Minimum size required to hold content length value */
    if (len < 16) {
        return (HTTPCli_ERECVBUFSMALL);
    }

    if (!(getCliState(cli, READ_FLAG))) {
        while (1) {
            for (i = 0; i < MAX_FIELD_NAME_LEN; i++) {

                ret = bufferedRecv(cli, &(cli->ssock), &c, 1, 0);
                if (ret < 0) {
                    return (ret);
                }

                if (c == ':') {
                    name[i] = 0;
                    break;
                }
                else if (c == ' ' || c == '\r') {
                    i--;
                    /* drop SP */
                }
                else if (c == '\n') {
                    return (HTTPCli_FIELD_ID_END);
                }
                else {
                    name[i] = c;
                }
            }

            /* We cannot recognise this header, its too big. Skip it */
            if ((i == MAX_FIELD_NAME_LEN) && (name[i - 1] != 0)) {
                ret = skipLine(cli);
                if (ret < 0) {
                    return (ret);
                }
                continue;
            }

            respFieldIndex = lookUpResponseFields(cli, name);
            ret = readLine(cli, body, len, moreFlag);
            if (ret < 0) {
                return (ret);
            }
            
            ret = checkContentField(cli, name, body, *moreFlag);
            if (ret < 0) {
                return (ret);
            }
            if (respFieldIndex >= 0) {
                break;
            }

            if (*moreFlag) {
                ret = skipLine(cli);
                if (ret < 0) {
                    return (ret);
                }
            }
        }
    }
    else {
        /* Read field value */
        ret = readLine(cli, body, len, moreFlag);
        if (ret < 0) {
            return (ret);
        }
    }

    return (respFieldIndex);
}

/*
 *  ======== HTTPCli_readResponseBody ========
 */
int HTTPCli_readResponseBody(HTTPCli_Handle cli, char *body, int len,
        bool *moreFlag)
{
    int ret = 0;

    xassert(cli != NULL);

    *moreFlag = false;
    if (getCliState(cli, CHUNKED_FLAG)) {
        ret = getChunkedData(cli, body, len, moreFlag);
    }
    else {
        if (cli->clen) {
            if (cli->clen < (unsigned long)len) {
                len = cli->clen;
            }
            ret = HTTPCli_readRawResponseBody(cli, body, len);
            if (ret > 0) {
                cli->clen -= ret;
                *moreFlag = cli->clen ? true : false;
            }
        }
    }

    return (ret);
}

/*
 *  ======== HTTPCli_readRawResponseBody ========
 */
int HTTPCli_readRawResponseBody(HTTPCli_Handle cli, char *body, int len)
{
    int ret = 0;

    xassert(cli != NULL);

    ret = bufferedRecv(cli, &(cli->ssock), body, len, 0);

	if (ret < 0) {
        return (ret);
    }

    return (ret);
}

/*
 *  ======== HTTPCli_setProxy ========
 */
void HTTPCli_setProxy(const struct sockaddr *addr)
{
    proxyAddr = *addr;
}

