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

#ifndef _HTTP_CORE_H_
#define _HTTP_CORE_H_

#include "datatypes.h"

/**
 * @addtogroup HttpCore
 *
 * @{
 */

/// The TCP port to listen on.
#define HTTP_CORE_SERVER_PORT 80

/// Maximum number of concurrent client connections. This should be one less than the CC3200 maximum number of sockets
#define HTTP_CORE_MAX_CONNECTIONS 10

/// Maximum size for a received or sent packet. Two buffers of this size must be allocated at all times.
#define HTTP_CORE_MAX_PACKET_SIZE_RECEIVED 1000

/// Maximum size for a received or sent packet. Two buffers of this size must be allocated at all times.
#define HTTP_CORE_MAX_PACKET_SIZE_SEND 1000

/// Maximum length of header line which might be buffered, per connection, if a line is broken between packets
#define HTTP_CORE_MAX_HEADER_LINE_LENGTH 320

/**
 * Initialize and start the HTTP server.
 * The Wifi interface of the CC3200 chip should be initialized by now, and connected to the network
 */
void HttpServerInitAndRun();

void HttpResponse_AddCharToResponseHeaders(char ch);
void HttpResponse_AddStringToResponseHeaders(char * str, UINT16 len);
int WS_SendPacket(UINT16 uConnection);
void HttpCore_CloseConnection(UINT16 uConnection);

#define sl_HttpServerCb				SimpleLinkWebSocketEventHandler

/// @}

#endif //_HTTP_CORE_H_
