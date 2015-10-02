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
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <ti/net/network.h>

#ifdef __linux__
#include <sys/time.h>
#endif

#ifdef __NDK__
#include <ti/sysbios/knl/Task.h>
#endif

#include <ti/net/http/ssock.h>
#include <ti/net/http/urlhandler.h>
#include <logging.h>

#include <httpsrv.h>

#define TAG_GET         1
#define TAG_POST        2
#define TAG_PUT         3
#define TAG_DELETE      4
#define TAG_CLEN        5
#define TAG_AUTH        6
#define TAG_HOST        7
#define TAG_DONTCARE    8

 /* end of line - line content in buf */
#define EOLINE  1

/* end of header */
#define EOHEADER 0

/* unknown error */
#define SOCKETERROR -1

#define NUMSESSIONS 8

typedef struct Session {
    int s;
    Ssock_Handle ssock;
    char * line;
    char * uri;
    URLHandler_Handle urlh[];
} Session;

typedef struct HTTPSrv_Object {
    int s;
    int timeout;
    int maxLineLen;
    int maxURILen;
    int numURLh;
    int numSessions;
    Session * sessions[NUMSESSIONS];
    URLHandler_Setup setup[];
} HTTPSrv_Object;

Registry_Desc ti_net_http_HTTPSrv_desc;

static HTTPSrv_Params defParams = {
    60,
    128,
    112,
};

static void inline close_socket(int s)
{
    close(s);
}

static int httpExtractTag(char * tag)
{
    if (!strncmp("GET", tag, 3)) {
        return (TAG_GET) ;
    }
    if (!strncmp("PUT", tag, 3)) {
        return (TAG_PUT);
    }
    if (!strncmp("DELETE", tag, 6)) {
        return (TAG_DELETE);
    }
    if (!strncmp("POST", tag, 4)) {
        return (TAG_POST);
    }
    if (!strncmp("Content-Length: ", tag, 16)) {
        return (TAG_CLEN);
    }
    if (!strncmp("Host: ", tag, 6)) {
        return (TAG_HOST);
    }

    return (TAG_DONTCARE);
}

static int handle404(Ssock_Handle ssock, int contentLength)
{
    int len;
    uint8_t buf[32];

    /* dump the request body */
    while (contentLength > 0) {
        len = contentLength > sizeof(buf) ? sizeof(buf) : contentLength;
        len = Ssock_recv(ssock, buf, len, 0);
        if (len > 0) {
            contentLength -= len;
        }
        else {
            break;
        }
    }

    HTTPSrv_sendErrorResponse(ssock, HTTP_NOT_FOUND);

    return (contentLength);
}

static int httpRecvLine(Ssock_Handle ssock, char * buf, int bufLen)
{
    ssize_t nbytes;
    int len;

    if ((nbytes = Ssock_recv(ssock, buf, 2, 0)) <= 0) {
        if (nbytes < 0) {
            HTTPSrv_sendErrorResponse(ssock, HTTP_BAD_REQUEST);
        }
        return SOCKETERROR;
    }

    if (buf[0] == '\r' && buf[1] == '\n') {
        return EOHEADER;
    }

    len = 2;
    do {
        if ((nbytes = Ssock_recv(ssock, &buf[len], 1, 0)) <= 0) {
            if (nbytes < 0) {
                HTTPSrv_sendErrorResponse(ssock, HTTP_BAD_REQUEST);
            }
            return SOCKETERROR;
        }

        if (++len == bufLen) {
            HTTPSrv_sendErrorResponse(ssock, HTTP_TOO_BIG);
            return SOCKETERROR;
        }
    } while (!(buf[len - 2] == '\r' && buf[len - 1] == '\n'));

    buf[len] = 0;

    return EOLINE;
}

void HTTPSrv_init(void)
{
    static int regInit = false;

    if (!regInit) {
        Registry_addModule(&ti_net_http_HTTPSrv_desc, "ti.net.http.HTTPSrv");
        regInit = true;
    }
}

void HTTPSrv_exit(void)
{
    /* TODO: can you remove a Registry entry? */
}

HTTPSrv_Handle HTTPSrv_create(const URLHandler_Setup * setup, int numURLh,
                              HTTPSrv_Params * params)
{
    HTTPSrv_Handle srv;

    if ((srv = malloc(sizeof(HTTPSrv_Object) + numURLh * sizeof(URLHandler_Setup)))) {
        int i;

        srv->numURLh = numURLh;
        for (i = 0; i < numURLh; i++) {
            srv->setup[i] = setup[i];
        }

        if (params == NULL) {
            params = &defParams;
        }

        srv->timeout = params->timeout;
        srv->maxLineLen = params->maxLineLen;
        srv->maxURILen = params->maxURILen;

        srv->numSessions = NUMSESSIONS;
        for (i = 0; i < srv->numSessions; i++) {
            srv->sessions[i] = NULL;
        }

#ifdef __NDK__
        fdOpenSession(Task_self());
#endif
    }

    return (srv);
}

void HTTPSrv_delete(HTTPSrv_Handle * srv)
{
#ifdef __NDK__
        fdCloseSession(Task_self());
#endif

    if (srv && *srv) {
        free(*srv);
        *srv = NULL;
    }
}

int HTTPSrv_serve(HTTPSrv_Handle srv, const struct sockaddr * addr, int len, int backlog)
{
    int sl;
    int sc;
    int status;

    if ((sl = socket(addr->sa_family, SOCK_STREAM, 0)) == -1) {
        return (HTTPSrv_ESOCKETFAIL);
    }

#ifdef __linux__
    int opt = 1;
    setsockopt(sl, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    if ((status = bind(sl, addr, len)) == -1) {
        return (HTTPSrv_EBINDFAIL);
    }

    if ((status = listen(sl, backlog)) == -1) {
        return (HTTPSrv_ELISTENFAIL);
    }

    status = 0;
    while (1) {
        if ((sc = accept(sl, NULL, NULL)) == -1) {
            status = HTTPSrv_EACCEPTFAIL;
            break;
        }

        HTTPSrv_processClient(srv, sc);
        /* socket already closed */
    }

    close_socket(sl);

    return (status);
}


Session * createSession(HTTPSrv_Handle srv, int s)
{
    Session * session;
    int i;
    struct timeval to;
#ifndef SL_DRIVER_VERSION
    struct linger  lgr;
#endif

    session = malloc(sizeof(Session) + srv->numURLh * sizeof(URLHandler_Handle));
    session->s = s;
    session->line = malloc(srv->maxLineLen + srv->maxURILen);
    session->uri = session->line + srv->maxLineLen;

    for (i = 0; i < srv->numURLh; i++) {
        if (srv->setup[i].create) {
            if ((session->urlh[i] = srv->setup[i].create(srv->setup[i].params)) == NULL) {
                /* teardown */
            }
        }
        else {
            session->urlh[i] = NULL;
        }
    }

#ifndef SL_DRIVER_VERSION
    /* Init the socket parameters */
    lgr.l_onoff  = 1;
    lgr.l_linger = 5;
    if (setsockopt(s, SOL_SOCKET, SO_LINGER, &lgr, sizeof(lgr)) < 0) {
        /* teardown */
    }

    /* Configure our socket timeout to be 10 seconds */
    to.tv_sec  = 10;
    to.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof(to)) < 0) {
        /* teardown */
    }
#endif

    to.tv_sec  = srv->timeout;
    to.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to)) < 0) {
        /* teardown */
    }

    if ((session->ssock = Ssock_create(s)) == NULL) {
        /* teardown */
    }

    return (session);
}

void deleteSession(HTTPSrv_Handle srv, Session * session)
{
    int i;

    if (session) {
        for (i = 0; i < srv->numURLh; i++) {
            if (session->urlh[i]) {
                srv->setup[i].delete(&session->urlh[i]);
            }
        }
        if (session->line) {
            free(session->line);
        }
        if (session->ssock) {
            Ssock_delete(&session->ssock);
            close_socket(session->s);
        }
        free(session);
    }
}

int addSession(HTTPSrv_Handle srv, int s)
{
    int status = 0;
    int i;

    for (i = 0; i < srv->numSessions; i++) {
        if (srv->sessions[i] == NULL) {
            if ((srv->sessions[i] = createSession(srv, s))) {
                Log_print1(Diags_ANALYSIS, "adding %d", i);
                status = 1;
            }
            else {
                /* silently drop sessions we cannot handle now */
                close_socket(s);
            }
            break;
        }
    }

    if (i == srv->numSessions) {
        Log_print1(Diags_WARNING, "Too many clients: %p", (xdc_IArg)srv);
        close_socket(s);
    }

    return (status);
}

void removeSession(HTTPSrv_Handle srv, Session * session)
{
    int i;

    for (i = 0; i < srv->numSessions; i++) {
        if (srv->sessions[i] == session) {
            Log_print1(Diags_ANALYSIS, "rm'ing %d", i);
            deleteSession(srv, session);
            srv->sessions[i] = NULL;
            break;
        }
    }
}

Session * getSession(HTTPSrv_Handle srv, int n)
{
    Session * session = NULL;
    int i;
    int cur = 0;

    for (i = 0; i < srv->numSessions; i++) {
        if (srv->sessions[i]) {
            if (cur == n) {
                session = srv->sessions[i];
                break;
            }
            cur++;
        }
    }

    return (session);
}

int HTTPSrv_transact(HTTPSrv_Handle srv, Session * session)
{
    Ssock_Handle ssock = session->ssock;
    char * line = session->line;
    int lineLen = srv->maxLineLen;
    char * uri = session->uri;
    int uriLen = srv->maxURILen;
    char * uriArgs;
    int method;
    int status;
    int contentLength;
    char * beg = NULL;
    char * end = NULL;
    int i;

    if ((status = httpRecvLine(ssock, line, lineLen)) <= 0) {
        Log_print1(Diags_ANALYSIS, "processClient> httpRecvLine %d", (IArg)status);
        status = 1;
        goto END;
    }

    /*
     *  Parse the request line which should look like:
     *
     *  METHOD /uri HTTP/1.1
     *
     * TODO: handle cases of missing URI and or HTTP/1.1
     * TODO: look for orphan CR or LF in line
     */
    if ((method = httpExtractTag(line)) > TAG_DELETE) {
        HTTPSrv_sendErrorResponse(ssock, HTTP_BAD_REQUEST);
        status = 1;
        goto END;
    }

    beg = strchr(line, ' ');
    if (beg == NULL) {
        /* missing the URI */
        HTTPSrv_sendErrorResponse(ssock, HTTP_BAD_REQUEST);
        status = 1;
        goto END;
    }

    while (*beg == ' ') {
        beg++;
    }

    end = strchr(beg, ' ');
    if (end == NULL) {
        /* missing HTTP/1.x */
        HTTPSrv_sendErrorResponse(ssock, HTTP_BAD_REQUEST);
        status = 1;
        goto END;
    }

    if (uriLen > (end - beg)) {
        strncpy(uri, beg, end - beg);
        uri[end - beg] = 0;
    }
    else {
        HTTPSrv_sendErrorResponse(ssock, HTTP_TOO_BIG);
        status = 1;
        goto END;
    }

    /* extract any CGI args from the URI */
    if ((uriArgs = strchr(uri, '?'))) {
        *uriArgs++ = 0;
    }

    /*
     *  Receive and process all the remaining fields in the
     *  request header, looking for the ones we care about.
     *
     *  TODO: enable users to specify required fields and
     *  get the associated data, possibly via callbacks. Some
     *  of the fields should really go to the URLHandlers,
     *  like Content-Type.
     */
    contentLength = 0;
    while ((status = httpRecvLine(ssock, line, lineLen)) > 0) {
        int nTag;

        nTag = httpExtractTag(line);

        if (nTag == TAG_CLEN) {
            /* 16 == sizeof("Content-Length: ") */
            contentLength = atoi(line + 16);
        }

        for (i = 0; i < srv->numURLh; i++) {
            if (srv->setup[i].scan) {
                srv->setup[i].scan(session->urlh[i], method, uri, line);
            }
        }
    }

    if (status < 0) {
        status = 1;
        goto END;
    }

    Log_print4(Diags_ANALYSIS, "start handler> %p %d %s %d", (IArg)srv, method,
               (IArg)uri, contentLength);

    status = URLHandler_ENOTHANDLED;
    for (i = 0; i < srv->numURLh; i++) {
        /* TODO: add uriArgs to the process() call */
        Log_print1(Diags_ANALYSIS, "urlh: %p\n", (xdc_IArg)session->urlh[i]);
        status = srv->setup[i].process(session->urlh[i], method, uri, uriArgs,
                                       contentLength, ssock);
        if (status != URLHandler_ENOTHANDLED) {
            break;
        }
    }

    Log_print2(Diags_ANALYSIS, "finish handler> %p %d", (IArg)srv, status);

    if (status == URLHandler_ENOTHANDLED) {
        status = handle404(ssock, contentLength);
    }
    else if (status == URLHandler_EERRORHANDLED) {
        status = 1;
    }
    else {
        status = 0;
    }

END:
    return (status);
}

int HTTPSrv_serveSelect(HTTPSrv_Handle srv, const struct sockaddr * addr,
                        int len, int backlog)
{
    int sl;
    int sc;
    Session * session;
    int status;
    fd_set fds;
    int i;
    int maxs;
    int numClients = 0;

    if ((sl = socket(addr->sa_family, SOCK_STREAM, 0)) == -1) {
        return (HTTPSrv_ESOCKETFAIL);
    }

#ifdef __linux__
    int opt = 1;
    setsockopt(sl, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    if ((status = bind(sl, addr, len)) == -1) {
        close_socket(sl);
        return (HTTPSrv_EBINDFAIL);
    }

    if ((status = listen(sl, backlog)) == -1) {
        close_socket(sl);
        return (HTTPSrv_ELISTENFAIL);
    }

    status = 0;
    while (1) {

        FD_ZERO(&fds);
        FD_SET(sl, &fds);
        maxs = sl;
        for (i = 0; i < numClients; i++) {
            if ((session = getSession(srv, i)) == NULL) {
                break;
            }
            FD_SET(session->s, &fds);
            if (session->s > maxs) {
                maxs = session->s;
            }
        }

        /* TODO: add timeout support to close dormant clients */
        if (select(maxs + 1, &fds, NULL, NULL, NULL) == -1) {
            for (i = 0; i < numClients; i++) {
                session = getSession(srv, i);
                removeSession(srv, session);
            }
            status = -1;
            break;
        }

        if (FD_ISSET(sl, &fds)) {
            if ((sc = accept(sl, NULL, NULL)) == -1) {
                status = HTTPSrv_EACCEPTFAIL;
                break;
            }
            if (addSession(srv, sc)) {
                numClients++;
            }
        }

        for (i = 0; i < numClients; i++) {
            session = getSession(srv, i);
            if (FD_ISSET(session->s, &fds)) {
                if (HTTPSrv_transact(srv, session)) {
                    Log_print1(Diags_ANALYSIS, "closing %p", session->s);
                    removeSession(srv, session);
                    numClients--;
                }
            }
        }
    }

    close_socket(sl);

    return (status);
}

int HTTPSrv_processClient(HTTPSrv_Handle srv, int sock)
{
    Session * session;

    session = createSession(srv, sock);

    Log_print1(Diags_ANALYSIS, "processClient> start %p", (IArg)srv);

    while (HTTPSrv_transact(srv, session) == 0) {
    }

    Log_print1(Diags_ANALYSIS, "finish processClient> %p", (IArg)srv);

    deleteSession(srv, session);

    Log_print0(Diags_EXIT, "_processClient> exit");

    return (0);
}
