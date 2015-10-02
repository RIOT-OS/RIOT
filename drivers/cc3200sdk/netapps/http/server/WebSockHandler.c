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
 * @addtogroup WebSockHandler
 *
 * @{
 */

//#include "HttpHeaders.h"
#include "HttpCore.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "HttpAuth.h"
#include "HttpDebug.h"
#include <string.h>
#include <stdlib.h>
#include "HTTPConfig.h"
#include "HTTPString.h"
#include "osi.h"
#include "WebSockHandler.h"


// Include CC3200 SimpleLink headers
#include "simplelink.h"

// Include Driverlin headers
#include "rom.h"
#include "rom_map.h"
#include "utils.h"

char *GlobRecvBuf;
long long GlobPayloadLen;
long long GlobRecvLen;
UINT8 RecvMore = 0;
char MaskKey[4];
UINT8 Mask;
UINT8 Ping = 0;
UINT8 Close = 0;

// WebSocket response status line strings
char    WS_STATUS_OK_STR[]                     =    "ok";
char    WS_STATUS_GOING_AWAY_STR[]   		   =    "server down";
char    WS_STATUS_ERROR_PROTOCOL_STR[]   	   =    "protocol error";
char    WS_STATUS_ERROR_DATATYPE_STR[]   	   =    "datatype not supported";
char    WS_STATUS_ERROR_ENCODING_STR[]         =    "data not interpreted";
char    WS_STATUS_ERROR_OVERFLOW_STR[]   	   =    "data too large";
char    WS_STATUS_ERROR_UNEXPECTED_STR[]       =    "unexpected event server";


/* This function parses the incoming data packet
* @return 1 if successful
		0 if failure
*/
/*

		0				1					2					3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-------+-+-------------+-------------------------------+
	|F|R|R|R| opcode|M| Payload len |	 Extended payload length	|
	|I|S|S|S|  (4)	|A| 	(7) 	|			  (16/64)			|
	|N|V|V|V|		|S| 			|	(if payload len==126/127)	|
	| |1|2|3|		|K| 			|								|
	+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
	|	  Extended payload length continued, if payload len == 127	|
	+ - - - - - - - - - - - - - - - +-------------------------------+
	|								|Masking-key, if MASK set to 1	|
	+-------------------------------+-------------------------------+
	| Masking-key (continued)		|		   Payload Data 		|
	+-------------------------------- - - - - - - - - - - - - - - - +
	:					  Payload Data continued ...				:
	+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
	|					  Payload Data continued ...				|
	+---------------------------------------------------------------+


*/

long long Payloadlength(struct HttpBlob * pData, UINT8 iter)
{
	long long result = 0x0;

	while(iter != 0)
	{
		result = ((long long) (*(pData->pData)) | result);
		if(iter != 1)
			result = result << 8;
		pData->pData += sizeof(UINT8);
		pData->uLength -= sizeof(UINT8);
		iter--;
	}

	return result;
}


int WSCore_DataRecv(UINT16 uConnection,struct HttpBlob * pData)
{
	UINT8 NextBlock;
	long long lengthLeft;
	UINT16 final,RSV,Opcode;
	long long PayloadLength;
	UINT8 iter;
	char *PayloadData;
	int RecvLength = (int)pData->uLength;


	if(!RecvMore)
	{
		//Parse the first byte
		NextBlock= *(pData->pData);
		pData->pData += sizeof(UINT8);
		pData->uLength -= sizeof(UINT8);

		//Mask bits to get header fields
		final = (NextBlock&(0x80));
		RSV = (NextBlock&(0x70));
		Opcode = ((NextBlock&(0x0F)));

		//If Final bit is set, this is the last data byte from client - terminating condition
		if(final)
			HttpDebug("Final datablock received \n\r");


		//Inform user about the opcode
			HttpDebug("Opcode is %d\n\r",Opcode);

			switch(Opcode)
			{
				case WS_TEXT:
					HttpDebug("Text data\n\r");
					break;
				case WS_BINARY:
					HttpDebug("Binary data\n\r");
					break;
				case WS_CLOSE:
					HttpDebug("Client has requested connection to be closed\n\r");
					Close = 1;
					WSStatusString(WS_STATUS_OK,pData);
					WSCore_DataSend(uConnection,*pData,Opcode);
					return 1;
				case WS_CONTINUATION:
					break;
				case WS_PING:
					HttpDebug("Ping received\n\r");
					Ping = 1;
					break;
				case WS_PONG:
					HttpDebug("Pong received\n\r");
					break;
				default:
					HttpDebug("This is unsupported\n\r");
					break;
			}


		///If header supports extension, send back error
		///Send error frame : TODO
		if(RSV)
			HttpDebug("Unsupported extension\n\r");

		//Parse the second byte
		NextBlock= *(pData->pData);
		pData->pData += sizeof(UINT8);
		pData->uLength -= sizeof(UINT8);

		Mask = (UINT8)(NextBlock&(0x80));
		PayloadLength = (NextBlock&(0x7F));

		//Payload length cases
		// If the payload length is 0x7E, the next 2 bytes represent the length
		if(PayloadLength == 0x7E)
		{
			iter = 2;
			PayloadLength = Payloadlength(pData, iter);
		}

		// If the payload length is 0x7F, the next 8 bytes represent the length
		if(PayloadLength == 0x7F)
		{
			iter = 8;
			PayloadLength = Payloadlength(pData, iter);
		}

		PayloadData = (char *)malloc(PayloadLength);
		if(PayloadData == NULL)
		{
			return 0;
		}
		memset(PayloadData,'\0',PayloadLength);
		GlobRecvBuf = PayloadData;
		GlobPayloadLen = PayloadLength;

		// If mask bit is set, the 4 bytes after payload length represent the masking key
		if(Mask)
		{
			memcpy(MaskKey,pData->pData,4);
			pData->pData += sizeof(UINT32);
			pData->uLength -= sizeof(UINT32);
		}

		int RecvLength = (int)pData->uLength;

		//Now, extract payload data
		memcpy(GlobRecvBuf,(const char *)(pData->pData),RecvLength);

		// Go back and get more data.
		if(PayloadLength > RecvLength)
		{
			GlobRecvLen = RecvLength; 	// Websocket header has 8 bytes that were also received.
			RecvMore = 1;
			return 1;
		}
	}

	else
	{
		memcpy(GlobRecvBuf+GlobRecvLen,(const char *)(pData->pData),RecvLength);
		GlobRecvLen += RecvLength;

		if(GlobRecvLen < GlobPayloadLen)
			return 1;
	}

	RecvMore = 0;


	if(Mask)
	{
		//UINT8 MaskBlock;
		char *pData = GlobRecvBuf;
		lengthLeft = GlobPayloadLen;
		UINT8 MaskIndex = 0;
		while(MaskIndex < 4)
		{
			NextBlock = (UINT8)(*pData);
			NextBlock ^= MaskKey[MaskIndex];
			MaskIndex++;
			*pData = NextBlock;
			pData += sizeof(UINT8);
			lengthLeft -= sizeof(UINT8);
			if(lengthLeft == 0)
				break;
			if(MaskIndex == 4)
				MaskIndex = 0;
		}

	}

	*(GlobRecvBuf+GlobPayloadLen) = '\0';

	struct HttpBlob PayLoad;
	PayLoad.pData = (UINT8 *)GlobRecvBuf;
	PayLoad.uLength = (UINT16)GlobPayloadLen;

	if(Ping)
	{
		HttpDebug("You were Pinged\n\r");
		//You must pong
		Opcode = 0x0A;
		WSCore_DataSend(uConnection,PayLoad,Opcode);
	}

	//free(pData->pData);

	//Send this data to main
	WebSocketRecvEventHandler(uConnection,GlobRecvBuf);


	return 1;

}

/*!
 * 	\brief 						 Sends data to a websocket client								
 *
 * 	\param[in] uConnection			Connection number on HTTP server. 
 * 	\param[in] PayLoad			Structure holding the payload data and the size of the data
 * 	\param[in] Opcode				User provides data type (text/binary/ping/pong/close).
 *
 * 	\return						1 - If packet was successfully received, parsed and sent to the user API
 *     							0 - Error
 */
int WSCore_DataSend(UINT16 uConnection, struct HttpBlob PayLoad, UINT8 Opcode)
{
	UINT16 usTotalLength;
	UINT8 usNextBlock;
	UINT16 usPayloadLen;	// The heap cannot support beyond 65kb
	char *pucPayLoadData = (char *)PayLoad.pData;

	usNextBlock = 0;

	if(Opcode != 0x02)
		usTotalLength = strlen(pucPayLoadData);
	else
		usTotalLength = PayLoad.uLength;

	do{

		///Is this the final packet?
		if(usTotalLength < FRAGMENT_LENGTH)
		{
			//final = 0x1;
			usNextBlock |= 0x80;
		}
		///Add opcode to the header
		usNextBlock |= Opcode;

		/// Add this byte to the sendpacket
		HttpResponse_AddCharToResponseHeaders(usNextBlock);

		///Reset byte
		usNextBlock = 0x0;
		///Mask bit is always set to 0 from server to client

		///PayloadLen field
		if(usTotalLength <= 125)
			usPayloadLen = usTotalLength;
		else
			usPayloadLen = (126);

		usNextBlock |= (UINT8)usPayloadLen;

		/// Add this byte to the sendpacket
		HttpResponse_AddCharToResponseHeaders(usNextBlock);

		/// If payload length is more than 125 bytes, we need 16 bits to represent it.
		if(usPayloadLen == (126))
		{

			if(usTotalLength < FRAGMENT_LENGTH)
				usPayloadLen = usTotalLength;
			else
				usPayloadLen = FRAGMENT_LENGTH;

			if(usPayloadLen >= FRAGMENT_LENGTH - 4)
				usPayloadLen = FRAGMENT_LENGTH - 4;

			usNextBlock = (char)(usPayloadLen>>8);
			HttpResponse_AddCharToResponseHeaders(usNextBlock);
			usNextBlock = (char)(usPayloadLen);
			HttpResponse_AddCharToResponseHeaders(usNextBlock);
		}

		HttpResponse_AddStringToResponseHeaders(pucPayLoadData,(usPayloadLen));

		if(!WS_SendPacket(uConnection))
			return 0;

		if(Opcode == WS_CLOSE)
		{
			sl_WebSocketCloseEvtHdlr();
			MAP_UtilsDelay(8000000);
			(HttpCore_CloseConnection(uConnection));
			return 1;
		}

		usTotalLength -= (usPayloadLen);

		// Reset to continuation frame if packet is larger than max fragment size
		Opcode = WS_CONTINUATION;

		///Reset byte
		usNextBlock = 0x0;

		//Update pointer
		pucPayLoadData += usPayloadLen;

	}while(usTotalLength > 0);

	return 1;
}


/**
 * Returns status string according to status code - CHANGE
 */
void WSStatusString(UINT32 WS_Status, struct HttpBlob* status)
{
	HttpString_utoa(WS_Status, status);
	struct HttpBlob blob = {0, NULL};
    switch (WS_Status)
    {
    case WS_STATUS_OK:
        HttpBlobSetConst(blob, WS_STATUS_OK_STR);
        break;
    case WS_STATUS_GOING_AWAY:
        HttpBlobSetConst(blob, WS_STATUS_GOING_AWAY_STR);
        break;
    case WS_STATUS_ERROR_DATATYPE:
        HttpBlobSetConst(blob, WS_STATUS_ERROR_DATATYPE_STR);
        break;
    case WS_STATUS_ERROR_ENCODING:
        HttpBlobSetConst(blob, WS_STATUS_ERROR_ENCODING_STR);
        break;
    case WS_STATUS_ERROR_OVERFLOW:
        HttpBlobSetConst(blob, WS_STATUS_ERROR_OVERFLOW_STR);
        break;
    case WS_STATUS_ERROR_PROTOCOL:
        HttpBlobSetConst(blob, WS_STATUS_ERROR_PROTOCOL_STR);
        break;
    case WS_STATUS_ERROR_UNEXPECTED:
        HttpBlobSetConst(blob, WS_STATUS_ERROR_UNEXPECTED_STR);
        break;
    default:
        HttpDebug("Unknown response status \n\r");
        HttpAssert(0);
        break;
    }
    memcpy(status->pData + status->uLength, blob.pData, blob.uLength);
    return;
}

/// @}
