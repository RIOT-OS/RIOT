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

#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include "datatypes.h"
#include "HttpString.h"

/**
 * @defgroup HttpResponse HTTP and WebSocket Response modules
 * This module implements routines to allow content handler modules to build and send HTTP responses back to the client.
 * There are two layers in this module:
 * - The lower layer consists of HttpResponse_Headers() and HttpResponse_Content(). These routines allow the caller to specify all details of the response.
 * - The higher layer consists of HttpResponse_Canned*(). These routines emit canned (pre-made) responses, such as redirects and errors, which are useful in many situations.
 *
 * @{
 */

#define HTTP_STATUS_OK                 200
#define HTTP_STATUS_REDIRECT_PERMANENT 301
#define HTTP_STATUS_REDIRECT_TEMPORARY 302
#define HTTP_STATUS_ERROR_UNAUTHORIZED 401
#define HTTP_STATUS_ERROR_NOT_FOUND    404
#define HTTP_STATUS_ERROR_NOT_ACCEPTED 406
#define HTTP_STATUS_ERROR_INTERNAL     500


/// The response data is gzip-compressed. Implies the header Content-Encoding: gzip
#define HTTP_RESPONSE_FLAG_COMPRESSED   (1 << 0)

#define WS_STATUS_OK				   1000			// Purpose of connection has been fulfilled
#define WS_STATUS_GOING_AWAY		   1001			//Server going going down or browser re-navigating
#define WS_STATUS_ERROR_PROTOCOL	   1002			//Protocol error
#define WS_STATUS_ERROR_DATATYPE	   1003			//Text frame or binary sent not supported by the application
#define WS_STATUS_ERROR_ENCODING	   1007			//Type not consistent with the rest of the message
#define WS_STATUS_ERROR_POLICY 	   1008			//Policy violation for server
#define WS_STATUS_ERROR_OVERFLOW	   1009			//Data too large for buffer
#define WS_STATUS_ERROR_UNEXPECTED   1011			//Unexpected event


/**
 * Respond with the specified HTTP status and headers
 * @param uConnection The connection number, as it appears in the HttpRequest structure
 * @param uHttpStatus The HTTP status number to response with. Must be one of HTTP_STATUS_*
 * @param uFlags Flags which are manifested in the response headers. See HTTP_RESPONSE_FLAG_*
 * @param uContentLength The total length of content which will be sent via HttpResponse_Content()
 * @param contentType The content type string, or NULL to omit the content type
 * @param location A string which will be used for the Location header, or NULL to omit the Location header
 */
int HttpResponse_Headers(UINT16 uConnection, UINT16 uHttpStatus, UINT16 uFlags, UINT32 uContentLength, struct HttpBlob contentType, struct HttpBlob location);

/**
 * Retrieves the pointer and size of the packet-send buffer
 * This function should be called by content handlers that wish to use the already-allocated packet-send buffer in calls to HttpResponse_Content()
 * @param[out] pPacketSendBuffer Returns the pointer and size of the packet-send buffer
 */
void HttpResponse_GetPacketSendBuffer(struct HttpBlob* pPacketSendBuffer);

/**
 * Send response content to the client.
 * This function may be called more than once, until all the content is sent.
 * @param uConnection The connection number, as it appears in the HttpRequest structure
 * @param content Content blob to send to the client.
 */
int HttpResponse_Content(UINT16 uConnection, struct HttpBlob content);

/**
 * Sends a canned response, with an HTTP redirect
 * This function should be called *instead* of HttpResponse_Status(), HttpResponse_Headers() and HttpResponse_Content()
 * @param uConnection The connection number, as it appears in the HttpRequest structure
 * @param pLocation The redirect URL
 * @param bPermanent zero for temporary redirect, nonzero for permanent redirect
 */
int HttpResponse_CannedRedirect(UINT16 uConnection, struct HttpBlob location, UINT16 bPermanent);

/**
 * Sends a canned response, with an error message
 * This function should be called *instead* of HttpResponse_Status(), HttpResponse_Headers() and HttpResponse_Content()
 * @param uConnection The connection number, as it appears in the HttpRequest structure
 * @param uHttpStatus The HTTP error status. Must be one of HTTP_STATUS_ERROR_*
 */
int HttpResponse_CannedError(UINT16 uConnection, UINT16 uHttpStatus);

/// @}

#endif //_HTTP_RESPONSE_H_
