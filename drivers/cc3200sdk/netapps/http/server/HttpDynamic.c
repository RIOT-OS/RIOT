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

/**
 * @addtogroup HttpDynamic
 * @{
 */

#include "HttpDynamic.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpCore.h"
#include "string.h"

#ifdef HTTP_CORE_ENABLE_DYNAMIC

/**
 * @defgroup HttpDynamic Dynamic request handler module
 * This module implements dynamic content processing for HTTP requests.
 * All requests are handled by C code functions, and the response contents is returned via HttpResopnse routines
 * Note this module is only compiled if HTTP_CORE_ENABLE_DYNAMIC is defined in HttpConfig.h
 *
 * @{
 */

Resource g_RestContent[MAX_RESOURCE];

int g_NumResource = 0;

int SetResources(unsigned char method, char* pBuf, unsigned char* (*pCbRestFunc)(void *pArgs) )
{

	// POST is 0 and GET is 1

	if(g_NumResource < MAX_RESOURCE)
	{
		g_RestContent[g_NumResource].rest_method = method;
		g_RestContent[g_NumResource].ResourceString = (unsigned char*)pBuf;
		g_RestContent[g_NumResource].pCbfunc = pCbRestFunc;
		g_NumResource++;
	}
	else
		return 0;

	return 1;

}

/**
 * Initialize HttpDynamic module state for a new request, and identify the request
 * This function must examine the specified resource string and determine whether it can commit to process this request
 * If this function returns nonzero, then the core will call HttpDynamic_ProcessRequest() with the rest of the request details.
 * @param uConnection The number of the connection. This value is guaranteed to satisfy: 0 <= uConnection < HTTP_CORE_MAX_CONNECTIONS
 * @param resource The resource part of the URL, as specified by the browser in the request, including any query string (and hash)
 *                 Note: The resource string exists ONLY during the call to this function. The string pointer should not be copied by this function.
 * @param method The HTTP method sent from the client for the resource
 * @return nonzero if request is to be handled by this module. zero if not.
 */
int HttpDynamic_InitRequest(UINT16 uConnection, struct HttpBlob resource, UINT8 method)
{

	/* look for known resource names according to g_RestContent*/
	for (g_NumResource = 0; g_NumResource < MAX_RESOURCE; g_NumResource++)
	{
		if (HttpString_nextToken((char*)g_RestContent[g_NumResource].ResourceString,  strlen((const char*)g_RestContent[g_NumResource].ResourceString), resource) != NULL)
			break;
	}

	/* Rest resource not found */
	if (g_NumResource == MAX_RESOURCE)
		return 0;

	/* Method doesn't match */
	if(g_RestContent[g_NumResource].rest_method != method)
		return 0;


	return 1;
}

/**
 * Process a dynamic-content HTTP request
 * This function is only be called by the core, if HttpDynamic_InitRequest() returns nonzero.
 * This function processes the specified HTTP request, and send the response on the connection specified by request->uConnection.
 * This function must call the HttpResponse_*() functions in order to send data back to the browser.
 * Please refer to HttpResponse.h for more information.
 * @param request Pointer to all data available about the request
 */
int HttpDynamic_ProcessRequest(struct HttpRequest* request)
{
	struct HttpBlob contentType,nullBlob;
	struct HttpBlob contentBody;
	void *pArgs = NULL;

	contentType.pData = NULL;
	contentType.uLength = 0;
	nullBlob = contentType;

	/* 1. Call user defined API */
	contentBody.pData = g_RestContent[g_NumResource].pCbfunc(pArgs);
	contentBody.uLength = strlen((const char*)contentBody.pData);

	/* 2. Set header for HTTP Response */
	if(!HttpResponse_Headers(request->uConnection, HTTP_STATUS_OK, 0, contentBody.uLength, contentType, nullBlob))
		return 0;

	/* 3. fill the content response (if exists) */
	if (contentBody.uLength != 0)
	{
		if(!HttpResponse_Content(request->uConnection, contentBody))
			return 0;
	}

	return 1;
}


/// @}

#endif // HTTP_CORE_ENABLE_DYNAMIC
