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

/*!
 *  @file   ti/net/http/httpcli.h
 *
 *  @brief  HTTP/1.1 Client interface.
 */
/*!
 *  @addtogroup ti_net_http_httpcli HTTP Client
 *
 *  @brief  HTTP/1.1 Client interface.
 *
 *  This module provides a HTTP client implementation of IETF standard for
 *  HTTP/1.1 - RFC 2616 and TLS support to interact with HTTP/1.1 servers.
 *
 *  ### Features
 *    - Supports GET, POST, PUT, HEAD, OPTIONS, DELETE methods, request and
 *  response headers/bodies, and redirection response handling.
 *
 *    - Using this module, connections can be opened/closed to HTTP servers.
 *  Each connection is an instance which can be configured with a set of
 *  repeated requests/responses parameters so that they can be used across
 *  multiple transactions.
 *
 *    - For supporting small memory devices, the module has been specially
 *  designed with split request/response APIs which enable sending requests and
 *  responses in smaller chunks, and static memory based APIs to eliminate the
 *  need for dynamic memory allocations.
 *
 *    - Supports security features which include TLS and communication
 *  through proxy.
 *
 *    - Supports two types of programming model: synchronous and
 *  asynchronous modes. In the default synchronous mode, an application uses
 *  APIs to make request to the server and blocks till the server responds.
 *  With asynchronous mode, a thread to handle the response is created
 *  which invokes callbacks registered with the module.
 *
 *    - Supports TI-RTOS NDK, SimpleLink WiFi and Linux networking stacks. At
 *  IP level supports both IPv4 and IPv6.
 *
 *    - Supports conditional compilation of the module to include/exclude
 *  some of the features mentioned above.
 *
 *  ### Limitations
 *    - HTTP client on SimpleLink WiFi supports IPv4 only.
 *
 *    - HTTP client on TI-RTOS NDK does not support host name resolution. So the
 *  API HTTPCli_initSockAddr() does not resolve host name. The API can be used
 *  only if an IP string is passed instead of the host name.
 *
 * ### GET Example
 * @code
 * #include <ti/net/http/httpcli.h>
 *
 * HTTPCli_Struct cli;
 *
 * // Construct a static instance
 * HTTPCli_construct(&cli);
 *
 * // Connect to the HTTP Server
 * HTTPCli_connect(&cli, &addr, 0, NULL);
 *
 * // Set request fields
 * HTTPCli_Field fields[2] = {
 *     { HTTPCli_FIELD_NAME_HOST,  "www.example.com" },
 *     { NULL, NULL }
 * };
 *
 * HTTPCli_setRequestFields(&cli, fields);
 *
 * // Set response field filters
 * char *respFields[2] = {
 *     HTTPCli_FIELD_NAME_CONTENT_LENGTH,
 *     NULL
 * };
 *
 * HTTPCli_setResponseFields(&cli, respFields);
 *
 * // Make HTTP 1.1 GET request
 * //
 * // Send request to the server:
 * //
 * // GET /index.html HTTP/1.1
 * // Host: www.example.com
 * // <blank line>
 * HTTPCli_sendRequest(&cli, HTTPCli_METHOD_GET, "/index.html", false);
 *
 * // Get the processed response status
 * //
 * // HTTP/1.1 200 OK
 * //
 * status = HTTPCli_getResponseStatus(&cli);
 *
 * // Check the HTTP return status and process remaining response
 * if (status == 200) {
 *    do {
 *        // Filter the response headers and get the set response field
 *        //
 *        // ...
 *        // Content-type: text/xml; charset=utf-8\r\n
 *        // Content-length: 34
 *        // ...
 *        ret = HTTPCli_getResponseField(&cli, buf, sizeof(buf), &moreFlag);
 *
 *       // process data in buf if field is content length
 *       // Zero is the index of Content length in respFields array
 *       if (ret == 0) {
 *           len = (int)strtoul(buf, NULL, 0);
 *       }
 *
 *   //loop till the end of the response fields
 *   } while (ret != HTTPCli_FIELD_ID_END);
 *
 *   // Read message based on content length field value
 *   while (len > 0) {
 *       len -= HTTPCli_readRawResponseBody(&cli, buf, sizeof(buf));
 *       // process buf data and save
 *   }
 * }
 *
 * // Close connection
 * HTTPCli_disconnect(&cli);
 *
 * // Destroy the instance
 * HTTPCli_destruct(&cli);
 * @endcode
 *
 * ### POST Example
 * @code
 * #include <ti/net/http/httpcli.h>
 *
 * HTTPCli_Struct cli;
 *
 * // Construct a static instance
 * HTTPCli_construct(&cli);
 *
 * // Connect to the HTTP Server
 * HTTPCli_connect(&cli, &addr, 0, NULL);
 *
 * // Set request fields
 * HTTPCli_Field fields[2] = {
 *     { HTTPCli_FIELD_NAME_HOST,  "www.example.com" },
 *     { NULL, NULL }
 * };
 *
 * HTTPCli_setRequestFields(&cli, fields);
 *
 * // Set response field filters
 * char *respFields[2] = {
 *     HTTPCli_FIELD_NAME_CONTENT_LENGTH,
 *     NULL
 * };
 *
 * HTTPCli_setResponseFields(&cli, respFields);
 *
 * // Make HTTP 1.1 POST request
 * //
 * // Send request to the server:
 * //
 * // POST /index.html HTTP/1.1
 * // Host: www.example.com
 * HTTPCli_sendRequest(&cli, HTTPCli_METHOD_POST, "/index.html", true);
 *
 * // Send additional fields
 * //
 * // Content-Length: <length>
 * // <blank line>
 * HTTPCli_sendField(&cli, HTTPCli_FIELD_NAME_CONTENT_LENGTH, len, true);
 *
 * // Send request body
 * //
 * // <data>
 * HTTPCli_sendRequestBody(&cli, data, strlen(data));
 *
 * // Get the processed response status
 * //
 * // HTTP/1.1 200 OK
 * //
 * status = HTTPCli_getResponseStatus(&cli);
 *
 * // Check the HTTP return status and process remaining response
 * if (status == 200) {
 *    do {
 *        // Filter the response headers and get the set response field
 *        //
 *        // ...
 *        // Content-type: text/xml; charset=utf-8\r\n
 *        // Content-length: 34
 *        // ...
 *        ret = HTTPCli_getResponseField(&cli, buf, sizeof(buf), &moreFlag);
 *
 *       // process data in buf if field is content length
 *       // Zero is the index of Content length in respFields array
 *       if (ret == 0) {
 *           len = (int)strtoul(buf, NULL, 0);
 *       }
 *
 *   //loop till the end of the response fields
 *   } while (ret != HTTPCli_FIELD_ID_END);
 *
 *   // Read message based on content length field value
 *   while (len > 0) {
 *       len -= HTTPCli_readRawResponseBody(&cli, buf, sizeof(buf));
 *       // process buf data and save
 *   }
 * }
 *
 * // Close connection
 * HTTPCli_disconnect(&cli);
 *
 * // Destroy the instance
 * HTTPCli_destruct(&cli);
 * @endcode
 */

#ifndef _HTTPCli_H_
#define _HTTPCli_H_

/*! @ingroup ti_net_http_httpcli */
/*@{*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <http/client/httpstd.h>
#include <http/client/network.h>
#include <http/client/ssock.h>

#ifndef HTTPCli_BUF_LEN
#define HTTPCli_BUF_LEN 128
#endif /* HTTPCLI_BUF_LEN */

#ifndef HTTPCli_CERT_NAME_LEN
#define HTTPCli_CERT_NAME_LEN 16
#endif

/* HTTP Client Error Codes */
/*!
 *  @brief Socket create failed
 */
#define HTTPCli_ESOCKETFAIL        (-101)

/*!
 *  @brief Connect failed
 */
#define HTTPCli_ECONNECTFAIL       (-102)

/*!
 *  @brief Send failed
 */
#define HTTPCli_ESENDFAIL          (-103)

/*!
 *  @brief Recieve failed
 */
#define HTTPCli_ERECVFAIL          (-104)

/*!
 *  @brief TLS create failed
 */
#define HTTPCli_ETLSFAIL           (-105)

/*!
 *  @brief Cannot get IP address from host name
 */
#define HTTPCli_EHOSTNAME          (-106)

/*!
 *  @brief Send buffer is not big enough
 */
#define HTTPCli_ESENDBUFSMALL      (-107)

/*!
 *  @brief Recieve buffer is not big enough
 */
#define HTTPCli_ERECVBUFSMALL      (-108)

/*!
 *  @brief Cannot call getResponseStatus in async mode
 */
#define HTTPCli_EASYNCMODE         (-109)

/*!
 *  @brief Thread create failed
 */
#define HTTPCli_ETHREADFAIL        (-110)

/*!
 *  @brief Cannot create tunnel through proxy
 */
#define HTTPCli_EPROXYTUNNELFAIL   (-111)

/*!
 *  @brief Response recieved is not HTTP/1.1 response
 */
#define HTTPCli_ERESPONSEINVALID   (-112)

/*!
 *  @brief Content length returned is too large
 */
#define HTTPCli_ECONTENTLENLARGE   (-114)

/*!
 *  @brief Redirection URI returned is too long to be read into buffer
 */
#define HTTPCli_EREDIRECTURILONG   (-115)

/*!
 *  @brief Content type returned is too long to be read into buffer
 */
#define HTTPCli_ECONTENTTYPELONG   (-116)

/*!
 *  @brief Recieved content type does not match any registered callback
 */
#define HTTPCli_ENOCONTENTCALLBACK (-117)

/*!
 *  @brief Encoded data is not of chunked type
 */
#define HTTPCli_ENOTCHUNKDATA      (-118)

/*!
 *  @brief Operation could not be completed. Try again.
 */
#define HTTPCli_EINPROGRESS        (-119)

/*!
 *  @brief Internal instance buffer to send/recieve data is too small.
 */
#define HTTPCli_EINTERNALBUFSMALL  (-120)

/*!
 *  @brief Could not setup the notify callbacks
 */
#define HTTPCli_ESETNOTIFYFAIL     (-121)

/*!
 *  @brief Input URI length is too long to read into buffer.
 */
#define HTTPCli_EURILENLONG  (-120)

/* HTTP methods */
#define HTTPCli_METHOD_GET      "GET"
#define HTTPCli_METHOD_POST     "POST"
#define HTTPCli_METHOD_HEAD     "HEAD"
#define HTTPCli_METHOD_OPTIONS  "OPTIONS"
#define HTTPCli_METHOD_PUT      "PUT"
#define HTTPCli_METHOD_DELETE   "DELETE"
#define HTTPCli_METHOD_CONNECT  "CONNECT"

/* HTTP Request Field Name */
#define HTTPCli_FIELD_NAME_ACCEPT  "Accept"
#define HTTPCli_FIELD_NAME_ACCEPT_CHARSET  "Accept-Charset"
#define HTTPCli_FIELD_NAME_ACCEPT_ENCODING  "Accept-Encoding"
#define HTTPCli_FIELD_NAME_ACCEPT_LANGUAGE  "Accept-Language"
#define HTTPCli_FIELD_NAME_ACCEPT_RANGES "Accept-Ranges"
#define HTTPCli_FIELD_NAME_AGE "Age"
#define HTTPCli_FIELD_NAME_ALLOW  "Allow"
#define HTTPCli_FIELD_NAME_AUTHORIZATION  "Authorization"
#define HTTPCli_FIELD_NAME_CACHE_CONTROL  "Cache-Control"
#define HTTPCli_FIELD_NAME_CONNECTION  "Connection"
#define HTTPCli_FIELD_NAME_CONTENT_ENCODING  "Content-Encoding"
#define HTTPCli_FIELD_NAME_CONTENT_LANGUAGE  "Content-Language"
#define HTTPCli_FIELD_NAME_CONTENT_LENGTH  "Content-Length"
#define HTTPCli_FIELD_NAME_CONTENT_LOCATION  "Content-Location"
#define HTTPCli_FIELD_NAME_CONTENT_MD5  "Content-MD5"
#define HTTPCli_FIELD_NAME_CONTENT_RANGE  "Content-Range"
#define HTTPCli_FIELD_NAME_CONTENT_TYPE  "Content-Type"
#define HTTPCli_FIELD_NAME_COOKIE  "Cookie"
#define HTTPCli_FIELD_NAME_DATE  "Date"
#define HTTPCli_FIELD_NAME_ETAG "ETag"
#define HTTPCli_FIELD_NAME_EXPECT  "Expect"
#define HTTPCli_FIELD_NAME_EXPIRES  "Expires"
#define HTTPCli_FIELD_NAME_FROM  "From"
#define HTTPCli_FIELD_NAME_HOST  "Host"
#define HTTPCli_FIELD_NAME_IF_MATCH  "If-Match"
#define HTTPCli_FIELD_NAME_IF_MODIFIED_SINCE  "If-Modified-Since"
#define HTTPCli_FIELD_NAME_IF_NONE_MATCH  "If-None-Match"
#define HTTPCli_FIELD_NAME_IF_RANGE  "If-Range"
#define HTTPCli_FIELD_NAME_IF_UNMODIFIED_SINCE  "If-Unmodified-Since"
#define HTTPCli_FIELD_NAME_LAST_MODIFIED  "Last-Modified"
#define HTTPCli_FIELD_NAME_LOCATION  "Location"
#define HTTPCli_FIELD_NAME_MAX_FORWARDS  "Max-Forwards"
#define HTTPCli_FIELD_NAME_ORIGIN  "Origin"
#define HTTPCli_FIELD_NAME_PRAGMA  "Pragma"
#define HTTPCli_FIELD_NAME_PROXY_AUTHENTICATE "Proxy-Authenticate"
#define HTTPCli_FIELD_NAME_PROXY_AUTHORIZATION  "Proxy-Authorization"
#define HTTPCli_FIELD_NAME_RANGE  "Range"
#define HTTPCli_FIELD_NAME_REFERER  "Referer"
#define HTTPCli_FIELD_NAME_RETRY_AFTER "Retry-After"
#define HTTPCli_FIELD_NAME_SERVER "Server"
#define HTTPCli_FIELD_NAME_TE  "TE"
#define HTTPCli_FIELD_NAME_TRAILER  "Trailer"
#define HTTPCli_FIELD_NAME_TRANSFER_ENCODING  "Transfer-Encoding"
#define HTTPCli_FIELD_NAME_UPGRADE  "Upgrade"
#define HTTPCli_FIELD_NAME_USER_AGENT  "User-Agent"
#define HTTPCli_FIELD_NAME_VARY "Vary"
#define HTTPCli_FIELD_NAME_VIA  "Via"
#define HTTPCli_FIELD_NAME_WWW_AUTHENTICATE "WWW-Authenticate"
#define HTTPCli_FIELD_NAME_WARNING  "Warning"
#define HTTPCli_FIELD_NAME_X_FORWARDED_FOR  "X-Forwarded-For"

/* HTTP client instance configuration */
#define HTTPCli_TYPE_TLS   (0x02)
#define HTTPCli_TYPE_IPV6  (0x04)

/* HTTP client getResponseField() special return codes */
#define HTTPCli_FIELD_ID_DUMMY   (-11)
#define HTTPCli_FIELD_ID_END     (-12)

/*!
 *  @brief HTTPCli Request Header Field
 */
typedef struct HTTPCli_Field {
    const char *name;      /*!< Field name,  ex: HTTPCli_FIELD_NAME_ACCEPT */
    const char *value;     /*!< Field value, ex: "text/plain" */
} HTTPCli_Field;

/*!
 *  @brief HTTPCli callback function prototype for status handling
 *
 *  @param[in]  cli     Instance of the HTTP connection
 *
 *  @param[in]  status  Response status code
 *
 */
typedef void (*HTTPCli_StatusCallback)(void *cli, int status);

/*!
 *  @brief HTTPCli Response status code handlers
 */
typedef struct HTTPCli_StatusHandler {
    HTTPCli_StatusCallback  handle1xx;  /*!< 1xx status code callback */
    HTTPCli_StatusCallback  handle2xx;  /*!< 2xx status code callback */
    HTTPCli_StatusCallback  handle4xx;  /*!< 4xx/5xx status code callback */
} HTTPCli_StatusHandler;

/*!
 *  @brief HTTPCli callback function prototype for content handling
 *
 *  @param[in]  cli      Instance of the HTTP connection
 *
 *  @param[in]  status   Response status code
 *
 *  @param[in]  body     Data from the response body
 *
 *  @param[in]  len      Length of response body buffer
 *
 *  @param[in]  moreFlag Set if more response data is available
 *
 *  @return 1 to continue or 0 to stop further processing.
 */
typedef int (*HTTPCli_ContentCallback)(void *cli, int status, char *body,
        int len, bool moreFlag);

/*!
 *  @brief HTTPCli Content Handler type
 */
typedef struct HTTPCli_ContentHandler {
    char *contentType;               /*!< ex: application/json */
    HTTPCli_ContentCallback handle;  /*!< Callback for content Type */
} HTTPCli_ContentHandler;

/*!
 *  @brief HTTPCli callback function prototype for redirection handling
 *
 *  @param[in]  cli     Instance of the HTTP connection
 *
 *  @param[in]  status  Response status code
 *
 *  @param[in]  uri     The new URI string
 */
typedef void (*HTTPCli_RedirectCallback)(void *cli, int status, char *uri);

/*!
 *  @brief HTTPCli callback function prototype for asynchronous notify
 *         Supported for 6LoWPAN stack only.
 *
 *  @param[in]  skt  socket handle
 *
 *  @param[in]  cli  Instance of the HTTP connection
 */
typedef void (*HTTPCli_Notify)(long skt, void *cli);

/*!
 *  @brief HTTPCli instance type
 */
typedef struct HTTPCli_Struct {
    char **respFields;
    unsigned int state;
    unsigned long clen;
    Ssock_Struct ssock;
    HTTPCli_Field *fields;

    char buf[HTTPCli_BUF_LEN];
    unsigned int buflen;
    char *bufptr;

#ifndef HTTPCli_LIBTYPE_MIN
    HTTPCli_StatusHandler *shandle;
    HTTPCli_ContentHandler *chandle;
    HTTPCli_RedirectCallback rhandle;
#ifndef __linux__
    unsigned int stackSize;
    unsigned int priority;
#endif /* __linux__ */
#endif /* HTTPCli_LIBTYPE_MIN */

} HTTPCli_Struct;

/*!
 *  @brief HTTPCli instance paramaters
 */
typedef struct HTTPCli_Params {

#ifndef HTTPCli_LIBTYPE_MIN
    HTTPCli_StatusHandler *shandle;
    HTTPCli_ContentHandler *chandle;
    HTTPCli_RedirectCallback rhandle;
#ifndef __linux__
    unsigned int stackSize; /*!< Async thread stack size. 0 for default */
    unsigned int priority;  /*!< Async thread priority. 0 for default */
#endif /* __linux__ */
#endif /* HTTPCli_LIBTYPE_MIN */

#ifdef __SLP__
    HTTPCli_Notify rnotify;   /*!< Async read notify handle */
    HTTPCli_Notify wnotify;   /*!< Async write notify handle */
    HTTPCli_Notify enotify;   /*!< Async exception notify handle */
#endif /* __SLP__ */

    int timeout;
    /*!< Timeout value (in seconds) for socket. Set 0 for default value */

} HTTPCli_Params;

/*!
 *  @brief HTTPCli secure parameters for TLS
 */
typedef struct HTTPCli_SecureParams {
#ifdef __CYASSL__
    CYASSL_CTX *ctx; /* CYASSL context */

#elif defined(__SL__) /* __CYASSL__ */
    SlSockSecureMethod   method;
    SlSockSecureMask     mask;
    char                 cafile[HTTPCli_CERT_NAME_LEN];
    /*!< Set the CA certificate file name, or index 0 to 0 if not used */
    char                 privkey[HTTPCli_CERT_NAME_LEN];
    /*!< Set the client private key, or index 0 to 0 if not used */
    char                 cert[HTTPCli_CERT_NAME_LEN];
    /*!< Set the client certificate, or index 0 to 0 if not used */
    char                 dhkey[HTTPCli_CERT_NAME_LEN];
    /*!< Set the DH key, or index 0 to 0 if not used */

#else /* __CYASSL__ */
    char s;

#endif /* __CYASSL__ */
} HTTPCli_SecureParams;

typedef HTTPCli_Struct* HTTPCli_Handle;

/*!
 *  @brief  Initialize the socket address structure for the given URI.
 *          Supported URI formats are:
 *              "http://www.example.com:8000",
 *              "https://www.example.com:8000",
 *              "https://www.example.com",
 *              "www.example.com:8000",
 *              "www.example.com".
 *          For cases where port is not provided, the default port number is
 *          set.
 *
 *  @param[out] addr   Handle to the sockaddr structure
 *
 *  @param[in]  uri    A null terminated URI string
 *
 *  @param[in]  flags  Set @ref HTTPCli_TYPE_IPV6 for IPv6 addresses.
 *
 *  @return 0 on success or error code on failure.
 */
extern int HTTPCli_initSockAddr(struct sockaddr *addr, const char *uri,
        int flags);

/*!
 *  @brief  Create a new instance object in the provided structure
 *
 *  @param[out] cli    Instance of a HTTP client
 */
extern void HTTPCli_construct(HTTPCli_Struct *cli);

/*!
 *  @brief  Allocate and initialize a new instance object and return its handle
 *
 *  @return handle of the HTTP client instance on success or NULL on failure.
 */
extern HTTPCli_Handle HTTPCli_create();

/*!
 *  @brief  Open a connection to a HTTP server
 *
 *  @param[in]  cli    Instance of a HTTP client
 *
 *  @param[in]  addr   IP address of the server
 *
 *  @param[in]  flags  Sets the type of HTTP instance
 *                     (ex: @ref HTTPCli_TYPE_TLS). Multiple types can be
 *                     OR'ed together.
 *
 *  @param[in]  params Per-instance config params, or NULL for default values
 *
 *  @return 0 on success or error code on failure.
 */
extern int HTTPCli_connect(HTTPCli_Struct *cli, const struct sockaddr *addr,
        int flags, const HTTPCli_Params *params);

/*!
 *  @brief  Destory the HTTP client instance and free the previously allocated
 *          instance object.
 *
 *  @param[in]  cli Instance of the HTTP client
 */
extern void HTTPCli_delete(HTTPCli_Handle cli);

/*!
 *  @brief  Destory the HTTP client instance
 *
 *  @param[in]  cli Instance of the HTTP client
 */
extern void HTTPCli_destruct(HTTPCli_Struct *cli);

/*!
 *  @brief  Disconnect from the HTTP server and destroy the HTTP client instance
 *
 *  @param[in]  cli Instance of the HTTP client
 */
extern void HTTPCli_disconnect(HTTPCli_Struct *cli);

/*!
 *  @brief  Set an array of header fields to be sent for every HTTP request
 *
 *  @param[in]  cli     Instance of a HTTP client
 *
 *  @param[in]  fields  An array of HTTP request header fields terminated by
 *                      an object with NULL fields, or NULL to get
 *                      previously set array.
 *
 *  @remarks The array should be persistant for the lifetime of HTTP instance.
 *
 *  @return previously set array
 */
extern HTTPCli_Field *HTTPCli_setRequestFields(HTTPCli_Handle cli,
        const HTTPCli_Field *fields);

/*!
 *  @brief  Set the header fields to filter the response headers
 *
 *  @param[in]  cli      Instance of a HTTP client
 *
 *  @param[in]  fields   An array of HTTP response header field strings
 *                       terminated by a NULL, or NULL to get previously set
 *                       array.
 *
 *  @remarks The array should be persistant for the lifetime of HTTP instance.
 *
 *  @return previously set array
 */
extern char **HTTPCli_setResponseFields(HTTPCli_Handle cli,
        const char *fields[]);

/*!
 *  @brief  Make a HTTP 1.1 request to the HTTP server
 *
 *  Sends a HTTP 1.1 request-line and the header fields from the user set array
 *  (see @ref HTTPCli_setRequestFields()) to the server.
 *
 *  Additionally, more fields apart from the user set array of header fields
 *  can be sent to the server. To send more fields, set the `moreFlag` when
 *  calling this function and then call @ref HTTPCli_sendField() with more
 *  fields.
 *
 *  @param[in]  cli        Instance of a HTTP client
 *
 *  @param[in]  method     HTTP 1.1 method (ex: @ref HTTPCli_METHOD_GET)
 *
 *  @param[in]  requestURI the path on the server to open and any CGI
 *                         parameters
 *
 *  @param[in]  moreFlag   Set this flag when more fields will sent to the
 *                         server
 *
 *  @return 0 on success or error code on failure
 */
extern int HTTPCli_sendRequest(HTTPCli_Handle cli, const char *method,
        const char *requestURI, bool moreFlag);

/*!
 *  @brief  Send a header field to the HTTP server
 *
 *  This is a complementary function to @ref HTTPCli_sendRequest() when more
 *  header fields are to be sent to the server.
 *
 *  @param[in]  cli      Instance of a HTTP client
 *
 *  @param[in]  name     HTTP 1.1 request header field
 *                       (ex: @ref HTTPCli_FIELD_NAME_HOST)
 *
 *  @param[in]  value    HTTP 1.1 request header field value
 *
 *  @param[in]  lastFlag Set this flag when sending the last header field
 *
 *  @return 0 on success or error code on failure
 */
extern int HTTPCli_sendField(HTTPCli_Handle cli, const char *name,
        const char *value, bool lastFlag);

/*!
 *  @brief  Send the request message body to the HTTP server
 *
 *  Make a call to this function after @ref HTTPCli_sendRequest() (always)
 *  and @ref HTTPCli_sendField() (if applicable).
 *
 *  @param[in]  cli  Instance of a HTTP client
 *
 *  @param[in]  body  Request body buffer
 *
 *  @param[in]  len  Length of the request body buffer
 *
 *  @return 0 on success or error code on failure
 */
extern int HTTPCli_sendRequestBody(HTTPCli_Handle cli, const char *body,
        int len);

/*!
 *  @brief  Process the response header from the HTTP server and return status
 *
 *  @remarks  Do not call in asyncronous mode. This function will return
 *            @ref HTTPCli_EASYNCMODE.
 *
 *  @param[in]  cli    Instance of a HTTP client
 *
 *  @return  The status code from the server (1xx, 2xx, 3xx, 4xx, 5xx) on
 *           success or error code on failure.
 */
extern int HTTPCli_getResponseStatus(HTTPCli_Handle cli);

/*!
 *  @brief  Process the response header from the HTTP server and return field
 *
 *  Filters the response headers based on the array of fields (see
 *  @ref HTTPCli_setResponseFields()).
 *
 *  Repeatedly call this function till @ref HTTPCli_FIELD_ID_END is returned.
 *
 *  @param[in]  cli   Instance of a HTTP client
 *
 *  @param[out] value Field value string.
 *
 *  @param[in]  len   Length of field value string
 *
 *  @param[out] moreFlag  Flag set if the field value could not be completely
 *                        read into `value`. A subsequent call to this function
 *                        will read the remaining field value into `value` and
 *                        will return @ref HTTPCli_FIELD_ID_DUMMY.
 *
 *  @return On Success, the index of the field set in the
 *          @ref HTTPCli_setResponseFields() or @ref HTTPCli_FIELD_ID_END or
 *          @ref HTTPCli_FIELD_ID_DUMMY, or error code on failure.
 */
extern int HTTPCli_getResponseField(HTTPCli_Handle cli, char *value,
        int len, bool *moreFlag);

/*!
 *  @brief  Read the parsed response body data from the HTTP server
 *
 *  This function parses the response body if the content type is chunked
 *  transfer encoding or if the content length field is returned by the HTTP
 *  server.
 *
 *  Make a call to this function only after the call to
 *  @ref HTTPCli_getResponseStatus() and @ref HTTPCli_getResponseField().
 *
 *  @param[in]  cli  Instance of a HTTP client
 *
 *  @param[out] body Response body buffer
 *
 *  @param[in]  len  Length of response body buffer
 *
 *  @param[out] moreFlag  Set if more data is available
 *
 *  @return The number of characters read on success or error code on failure
 */
extern int HTTPCli_readResponseBody(HTTPCli_Handle cli, char *body,
        int len, bool *moreFlag);

/*!
 *  @brief  Read the raw response message body from the HTTP server
 *
 *  Make a call to this function only after the call to
 *  @ref HTTPCli_getResponseStatus() and @ref HTTPCli_getResponseField().
 *
 *  Repeatedly call this function till entire response message is read.
 *
 *  @param[in]  cli   Instance of a HTTP client
 *
 *  @param[out] body  Response body buffer
 *
 *  @param[in]  len   Length of response body buffer
 *
 *  @return The number of characters read on success or error code on failure
 */
extern int HTTPCli_readRawResponseBody(HTTPCli_Handle cli, char *body, int len);

/*!
 *  @brief Set the secure communication paramaters
 *
 *  @param[in]  sparams  pointer to secure communication params struct
 */
extern void HTTPCli_setSecureParams(HTTPCli_SecureParams *sparams);

/*!
 *  @brief Set the proxy address
 *
 *  @param[in]  addr IP address of the proxy server
 */
extern void HTTPCli_setProxy(const struct sockaddr *addr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*@}*/
#endif /* _HTTPCli_H_ */
