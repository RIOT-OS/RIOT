//*****************************************************************************
// Copyright (C) 2014 Texas Instruments Incorporated
//
// All rights reserved. Property of Texas Instruments Incorporated.
// Restricted rights to use, duplicate or disclose this code are
// granted through contract.
// The program may not be used without the written permission of
// Texas Instruments Incorporated or against the terms and conditions
// stipulated in the agreement under which this program has been supplied,
// and under no circumstances can it be used with non-TI connectivity device.
//
//*****************************************************************************

#ifndef _HTTP_HEADERS_H_
#define _HTTP_HEADERS_H_

#include "datatypes.h"
/**
 * @defgroup HttpHeaders HTTP header strings
 * This header file predefines various HTTP request and response header strings
 *
 * @{
 */

char HTTP_HEADER_DELIMITER[] = "\r\n";

// HTTP method line strings
char HTTP_METHOD_GET[] = "get";
char HTTP_METHOD_POST[] = "post";
char HTTP_VERSION_1P1[] = "http/1.1";
char HTTP_VERSION_1P0[] = "http/1.0";

// HTTP request/response header line strings
char HTTP_CONTENT_TYPE[] = "content-type";
char HTTP_CONTENT_LENGTH[] = "content-length";
char HTTP_ACCEPT_ENCODING[] = "accept-encoding";
char HTTP_AUTHORIZATION[] = "authorization";
char HTTP_CONNECTION_CLOSE[] = "connection";
char HTTP_GZIP[] = "gzip";
char HTTP_CLOSE[] = "close";
char HTTP_LOCATION[] = "location";
char HTTP_CONTENT_ENCODING[] = "content-encoding";

// HTTP response status line strings
char HTTP_STATUS_OK_STR[] = "ok";
char HTTP_STATUS_REDIRECT_PERMANENT_STR[] = "moved permanently";
char HTTP_STATUS_REDIRECT_TEMPORARY_STR[] = "moved temporarily";
char HTTP_STATUS_ERROR_UNAUTHORIZED_STR[] = "unauthorized";
char HTTP_STATUS_ERROR_NOT_FOUND_STR[] = "not found";
char HTTP_STATUS_ERROR_NOT_ACCEPTED_STR[] = "not accepted";
char HTTP_STATUS_ERROR_INTERNAL_STR[] = "internal server error";

//WebSocket upgrade header line strings
char WS_HOST[] = "host";
char WS_UPGRADE[] = "upgrade";
char WS_CONNECTION[] = "connection";
char WS_VERSION_REQUEST[] = "sec-websocket-version";
char WS_KEY_REQUEST[] = "sec-websocket-key";
char WS_ORIGIN[] = "origin";
char WS_CACHE_CONTROL[] = "cache-control";
char WS_PRAGMA[] = "pragma";
char WS_EXTENSION[] = "sec-websocket-extensions";
char* WS_ORIGIN_NAME = NULL;

//WebSocket response line strings
char WS_WEBSOCKET[] = "websocket";
char WS_MAGIC_STRING[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
char WS_VERSION[] = "13";
char WS_ACCEPT[] = "Sec-websocket-accept";

///Length of client generated key
#define WS_KEY_LENGTH 			24
// The client generated key should be 16 bytes if base64 decoded
UINT8 WS_KEY[WS_KEY_LENGTH + 1];

/// @}
#endif //_HTTP_HEADERS_H_
