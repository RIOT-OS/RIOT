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

#ifndef _HTTP_STATIC_H_
#define _HTTP_STATIC_H_

#include "HttpConfig.h"

#ifdef HTTP_CORE_ENABLE_STATIC

/**
 * @defgroup HttpStatic Static request handler module
 * This module implements static content processing for HTTP requests.
 * All requests are handled by looking up the URL's resource in the flash database, and returning the content in the response.
 * Note this module is only compiled if HTTP_CORE_ENABLE_STATIC is defined in HttpConfig.h
 *
 * @{
 */

#include "HttpRequest.h"

/**
 * Initialize HttpStatic module state for a new request, and identify the request
 * This function examines the specified resource string, and looks it up in the Flash Database.
 * If found, it commits to process this request by returning nonzero. Otherwise it returns zero.
 * @param uConnection The number of the connection. This value is guaranteed to satisfy: 0 <= uConnection < HTTP_CORE_MAX_CONNECTIONS
 * @param resource The resource part of the URL, as specified by the browser in the request, including any query string (and hash).
 *                 Note: The resource string exists ONLY during the call to this function. The string pointer should not be copied by this function.
 * @return nonzero if request is to be handled by this module. zero if not.
 */
int HttpStatic_InitRequest(UINT16 uConnection, struct HttpBlob resource);

/**
 * Process a static-content HTTP request
 * This function is called after a request was already initialized, and a Flash content entry was identified during a call to HttpStatic_InitRequest()
 * This function calls HttpResponse_*() to send the content data to the browser.
 * @param request Pointer to all data available about the request
 * @return nonzero if request was handled. zero if not.
 */
int HttpStatic_ProcessRequest(struct HttpRequest* request);

/// @}

#endif // HTTP_CORE_ENABLE_STATIC

#endif // _HTTP_STATIC_H_
