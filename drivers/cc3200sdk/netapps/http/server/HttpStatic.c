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

#include "HttpStatic.h"
#include <stdlib.h>

#ifdef HTTP_CORE_ENABLE_STATIC

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpCore.h"
#include "fs.h"

/**
 * @addtogroup HttpStatic
 * @{
 */

/**
 * @defgroup HttpStatic Static request handler module
 * This module implements static content processing for HTTP requests.
 * All requests are handled by looking up the URL's resource in the flash database, and returning the content in the response.
 * Note this module is only compiled if HTTP_CORE_ENABLE_STATIC is defined in HttpConfig.h
 *
 * @{
 */

#define FILE_NAME_SIZE_MAX    (40)

//Store File Name from HTTP Header
char g_cFileName[FILE_NAME_SIZE_MAX]; 				//String storing filename
long glFileHandle;							// file handle

/**
 * Initialize HttpStatic module state for a new request, and identify the request
 * This function examines the specified resource string, and looks it up in the Flash Database.
 * If found, it commits to process this request by returning nonzero. Otherwise it returns zero.
 * @param uConnection The number of the connection. This value is guaranteed to satisfy: 0 <= uConnection < HTTP_CORE_MAX_CONNECTIONS
 * @param resource The resource part of the URL, as specified by the browser in the request, including any query string (and hash).
 *                 Note: The resource string exists ONLY during the call to this function. The string pointer should not be copied by this function.
 * @return nonzero if request is to be handled by this module. zero if not.
 */
int HttpStatic_InitRequest(UINT16 uConnection, struct HttpBlob resource)
{
	char *pcWWWFsDir = "www";
	memset(g_cFileName,'\0',40);

	if(resource.uLength ==1 && *(resource.pData)=='/')
	{
		strcpy(g_cFileName,"www/main.html");
	}
	else
	{	
		strcpy(g_cFileName,pcWWWFsDir);
		strncat(g_cFileName,(char*)resource.pData,resource.uLength);
	}

	if(sl_FsOpen((unsigned char*)g_cFileName,FS_MODE_OPEN_READ,NULL,&glFileHandle)<0)
		return 0;
	else
	{
		return 1;
	}
}

/**
 * Process a static-content HTTP request
 * This function is called after a request was already initialized, and a Flash content entry was identified during a call to HttpStatic_InitRequest()
 * This function calls HttpResponse_*() to send the content data to the browser.
 * @param request Pointer to all data available about the request
 * @return nonzero if request was handled. zero if not.
 */
int HttpStatic_ProcessRequest(struct HttpRequest* request)
{
	struct HttpBlob location,contentType;
	struct HttpBlob* content = (struct HttpBlob*)malloc(sizeof(struct HttpBlob));
	unsigned long Offset = 0;
	SlFsFileInfo_t pFsFileInfo;
	UINT32 TotalLength;
	UINT8 HeaderFlag =0;
	UINT8 bRetVal = 1;
	UINT8 *buffer = NULL;


	if(content == NULL)
	{
		return 0;
	}
	location.pData = NULL;
	location.uLength = 0;
	contentType = location;

	/*  if HTTP_REQUEST_FLAG_METHOD_POST==1 (i.e. it is POST)
		HttpResponse_CannedError() responds to client with status HTTP_STATUS_ERROR_INTERNAL
		POST method is not supported for static pages	*/
	if (request->uFlags & HTTP_REQUEST_FLAG_METHOD_POST)
	{
		 /* HttpResponse_CannedError responds to client with 500 ERROR_INTERNAL  */
   		 if(!HttpResponse_CannedError(request->uConnection, HTTP_STATUS_ERROR_INTERNAL))
   		 {
   			bRetVal = 0;
   			goto end;
   		 }
   		 else
   		 {
   			bRetVal = 1;
   			goto end;
   		 }
	}

	sl_FsGetInfo((unsigned char *)g_cFileName, NULL, &pFsFileInfo);
	TotalLength = (&pFsFileInfo)->FileLen;

	while(TotalLength > 0)
	{
		content->uLength = ((TotalLength < 1000) ? (TotalLength):(1000));

		buffer = (UINT8*)realloc(buffer, content->uLength);
		if(buffer == NULL)
		{
			bRetVal = 0;
			goto end;
		}
		content->pData = buffer;

		/* if got here than it is a GET method
		HttpResponse_Headers() responds to client with status HTTP_STATUS_OK */
		if(sl_FsRead(glFileHandle, Offset, (unsigned char *) content->pData, content->uLength) < 0)
		{
			/* call HttpResponse_CannedError responds to client with 500 ERROR_INTERNAL  */
			if(!HttpResponse_CannedError(request->uConnection, HTTP_STATUS_ERROR_NOT_ACCEPTED))
			{
				bRetVal = 0;
				goto end;
			}
			else
			{
				bRetVal = 1;
				goto end;
			}
		}
		else
		{
			if(!HeaderFlag)
			{
				if(!HttpResponse_Headers(request->uConnection, HTTP_STATUS_OK, NULL,TotalLength, contentType, location))
				{
					bRetVal = 0;
					goto end;
				}
				HeaderFlag = 1;
			}

			/* HttpResponse_Content() sends requested page to the client */
			if(!HttpResponse_Content(request->uConnection, *content))
			{
				bRetVal = 0;
				goto end;
			}
		}

		TotalLength -= content->uLength;
		Offset += content->uLength;
	}

	sl_FsClose(glFileHandle,0,0,0);
end:
	if(buffer != NULL)
	{
		free(buffer);
	}
	free(content);

	return bRetVal;
}

/// @}

#endif // HTTP_CORE_ENABLE_STATIC
