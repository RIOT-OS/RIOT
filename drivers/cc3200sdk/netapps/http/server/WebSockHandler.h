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
 * @defgroup WebSocketHandler
 *
 * @{
 */

#define FRAGMENT_LENGTH		HTTP_CORE_MAX_PACKET_SIZE_SEND

/*!
 * 	\brief 							Parse entire websocket packet and forward only the payload to the user API.
 *									The API can handle packets received in parts. It is blocked till entire packet is received.
 *
 *
 * 	\param[in] uConnection			Connection number on HTTP server. The library supports 4.
 * 	\param[in] *pData				Pointer to the HttpBlob structure that holds the data.
 *
 * 	\return							1 - If packet was successfully received, parsed and sent to the user API
 *     								0 - Error
 */

int WSCore_DataRecv(UINT16 uConnection, struct HttpBlob* pData);

/*!
 * 	\brief 							Returns status string according to status code.
 *
 * 	\param[in] WSStatus				Status code of the websocket packet
 * 	\param[in] *status				String pointer to the message for the status
 *
 * 	\return							void
 */

void WSStatusString(UINT32 WSStatus, struct HttpBlob* status);



/*!
 * 	\brief 						 Sends data to a websocket client . * 								
 *
 * 	\param[in] uConnection			Connection number on HTTP server. 
 * 	\param[in] PayLoad			Structure holding the payload data and the size of the data
 * 	\param[in] Opcode				User provides data type (text/binary/ping/pong/close).
 *
 * 	\return						1 - If packet was successfully received, parsed and sent to the user API
 *     							0 - Error
 */

int WSCore_DataSend(UINT16 uConnection, struct HttpBlob PayLoad, UINT8 Opcode);


/*!
 * 	\brief 							Parses the payload length to the header if it is more than 125 ie (16 bit/64 bit)
 *
 * 	\param[in] *pData				Pointer to the websocket packet
 * 	\param[in] iter					iter = 2 for 16 bit length
 * 									iter = 8 for 64 bit length
 *
 *
 * 	\return							size of the payload
 */
long long Payloadlength(struct HttpBlob * pData, UINT8 iter);


/*!
 * 	\brief 					This websocket Event is called when WebSocket Server receives data
 * 							from client.
 *
 *
 * 	\param[in] puConnection	Websocket Client Id
 * 	\param[in] *ReadBuffer		Pointer to the buffer that holds the payload.
 *
 * 	\return					none.
 *     					
 */

void WebSocketRecvEventHandler(UINT16 uConnection, char *ReadBuffer);

/*!
 * 	\brief 							Callback function that indicates that handshake was a success
 * 									Once this is called the server can start sending data packets over websocket using
 * 									the sl_WebSocketSend API.
 *
 *
 * 	\param[in] uConnection				Websocket Client Id
 *
 * 	\return							void
 */

void WebSocketHandshakeEventHandler(UINT16 uConnection);

/*!
 * 	\brief 							Callback function that indicates that Websocket is closed
 * 									Once this is called the server acts as HTTP Server
 *
 *
 * 	\return							None
 */
void WebSocketCloseSessionHandler(void);

#define sl_WebSocketHandshakeEvtHdlr		WebSocketHandshakeEventHandler
#define	sl_WebSocketRecvEvtHdlr				WebSocketRecvEventHandler
#define sl_WebSocketSend					WSCore_DataSend
/*
 * This is a call back function from the library. User is informed that the connection has been closed.
 */
#define sl_WebSocketCloseEvtHdlr			WebSocketCloseSessionHandler




