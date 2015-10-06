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
//*****************************************************************************
//
// Protocol Name     -      Hyper Text Transfer Protocol
// Protocol Overview -  The Hypertext Transfer Protocol (HTTP) is an application-level
//                                            protocol for distributed, collaborative, hypermedia information
//                                           systems.
//                           Refer: https://www.ietf.org/rfc/rfc2616.txt
//*****************************************************************************

/**
 * @defgroup HttpCore
 *
 * @{
 */

#include "HttpHeaders.h"
#include "HttpCore.h"
#include "HttpSocket.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "HttpAuth.h"
#include "HttpDebug.h"
#include "HttpDynamic.h"
#include "HttpStatic.h"
#include <string.h>
#include "HTTPConfig.h"
#include "HTTPString.h"
#include "sha1.h"
#include "osi.h"
#include "WebSockHandler.h"
#include <stdlib.h>

// Include CC3200 bridged networking stack headers
#include "simplelink.h"

// Include Driverlib Headers
#include "rom.h"
#include "rom_map.h"
#include "utils.h"

OsiSyncObj_t g_WaitSyncObj;

#define OSI_DELAY(x)    osi_Sleep(x)

/**
 * @addtogroup HttpCore
 * @{
 */

/**
 * This enumeration defines all possible states for a connection
 */
enum HttpConnectionState {
    /// Connection is inactive. The connection's socket should be INVALID_SOCKET_HANDLE
    Inactive,
    /// Waiting for packet(s) with the request method
    RequestMethod,
    /// Waiting for packet(s) with request headers
    RequestHeaders,
    /// Currently receiving a header which is too long - Drop it and continue waiting for more headers
    DropCurrentHeader,
    /// Done parsing headers, waiting for POST data pakcets
    RequestData,
    /// Request is at the hands of the content module (static and/or dynamic), waiting for response headers.
    Processing,
    /// Response headers have been sent. Sending response content.
    ResponseData,
    /// Response complete. Possibility of another request.
    ResponseComplete,
    /// WebSocket opening handshake from client
    WebSocketRequest,
    /// WebSocket opening handshake from server
    WebSocketResponse,
    /// WebSocket data packets received
    WebSocketDataRecv,
    /// Websocket data packets sent
    WebSocketDataSend
};

/**
 * This enumeration defines all possible request-handler modules
 */
enum HttpHandler {
    /// No module is going to process this request (use the default 404 error)
    None,
    /// The HttpStatic module is going to process this request
    HttpStatic,
    /// The HttpDynamic module is going to process this request
    HttpDynamic
/// The websocket module is going to process this request
};

/**
 * This structure holds all information for an HTTP connection
 */
//#ifdef __CCS__
//struct HttpConnectionData
//#endif
//#ifdef __IAR_SYSTEMS_ICC__
//#pragma pack(1)
struct HttpConnectionData
//#endif
{
    /// The state of this connection
    enum HttpConnectionState connectionState;
    /// The data socket for this connection. Should be INVALID_SOCKET_HANDLE when in Inactive state
    int dataSocket;
    /// The current HTTP request on this connection
    struct HttpRequest request;
    /// Which handler is going to process the current request
    enum HttpHandler handler;
    /// An un-parsed chunk of header line
    UINT8 headerStart[HTTP_CORE_MAX_HEADER_LINE_LENGTH];
    /// Amount of content left to be read in the request or sent in the response
    UINT32 uContentLeft;
    /// timeout count
    UINT32 timeout;
    /// If the headers will arrive in several packets the content will be buffered in the headers start buffer until a whole line is available
    UINT16 uSavedBufferSize;
    /// Check weather the authentication is done or not
    UINT8 HttpAuth;
};

/**
 * This structure holds all of the global state of the HTTP server
 */
//#ifdef __CCS__
//struct __attribute__ ((__packed__)) HttpGlobalState
//#elif __IAR_SYSTEMS_ICC__
//#pragma pack(1)
struct HttpGlobalState
//#endif
{
    /// The listening socket
    int listenSocket;
    /// Number of active connections
    UINT16 uOpenConnections;
    /// All possible connections
    struct HttpConnectionData connections[HTTP_CORE_MAX_CONNECTIONS];
    /// The packet-receive buffer
    UINT8 packetRecv[HTTP_CORE_MAX_PACKET_SIZE_RECEIVED];
    /// Size of data in the packet-receive buffer
    int packetRecvSize;
    /// The packet-send buffer
    UINT8 packetSend[HTTP_CORE_MAX_PACKET_SIZE_SEND];
    /// Size of data in the packet-send buffer
    UINT16 packetSendSize;
};

/// The global state of the HTTP server
struct HttpGlobalState g_state;

// Forward declarations for static functions
static void HttpCore_InitWebServer();
static int HttpCore_HandleRequestPacket(UINT16 uConnection,
        struct HttpBlob packet);
static int HttpCore_HandleMethodLine(UINT16 uConnection, struct HttpBlob line);
static int HttpCore_HandleHeaderLine(UINT16 uConnection, struct HttpBlob line);
static int WSCore_HandshakeRequest(UINT16 uConnection, struct HttpBlob line);
static int WSCore_HandshakeResponse(UINT16 uConnection, struct HttpBlob line);
static int WS_HandshakeHash(char *InKey, char *OutKey, char *EncOutKey);
static int HttpCore_HandleRequestData(UINT16 uConnection,
        struct HttpBlob* pData);
static void HttpCore_ProcessNotFound(UINT16 uConnection);
static void RunHttpServer(void);

struct HttpBlob nullBlob = { NULL, 0 };

void HttpCloseServer() {
    if (g_state.listenSocket < 0) {
        return;
    }

    if (CloseTCPServerSocket(g_state.listenSocket) >= 0) {
        g_state.listenSocket = -1;
    }
}

void HttpServerInitAndRun(void * param) {
    // Initialize the server's global state
    HttpCore_InitWebServer();

#ifdef HTTP_CORE_ENABLE_AUTH
    struct HttpBlob user,pass;
    user.pData = (UINT8*)"cc3200";
    user.uLength= 6;
    pass.pData = (UINT8*)"admin";
    pass.uLength = 5;

    HttpAuth_Init(user, pass);
#endif

    HttpDebug("HttpServerApp Initialized \n\r");

    // Main loop of the server. Note: This is an infinite loop
    RunHttpServer();
}

void RunHttpServer(void) {
    int iRecvLen = 0, sock = -1, uConnection = 0;
    struct HttpBlob blob;

    while (1) {
        // Create the listener socket for HTTP Server.
        if (g_state.listenSocket < 0) {
            sock = CreateTCPServerSocket(HTTP_CORE_SERVER_PORT);
            if (sock >= 0) {
                g_state.listenSocket = sock;
            }
            HttpDebug("Http server socket %d \n\r", g_state.listenSocket);
        }

        if (g_state.listenSocket >= 0) {
            if (g_state.uOpenConnections < HTTP_CORE_MAX_CONNECTIONS) {
                sock = CreateTCPClientSocket(g_state.listenSocket); //non blocking func
                if (sock >= 0) {
                    for (uConnection = 0;
                            uConnection < HTTP_CORE_MAX_CONNECTIONS;
                            ++uConnection) {
                        if (g_state.connections[uConnection].connectionState
                                == Inactive)
                            break;
                    }
                    if (uConnection != HTTP_CORE_MAX_CONNECTIONS) {
                        g_state.connections[uConnection].dataSocket = sock;
                        g_state.uOpenConnections++;
                        g_state.connections[uConnection].connectionState =
                                RequestMethod;
                        HttpDebug("Accepting new connection number %d \n\r",
                                uConnection);
                    }
                } else if ((sock != SL_EAGAIN) && (sock != SL_POOL_IS_EMPTY)
                        && (sock != SL_INEXE) && (sock != SL_ENSOCK)) {
                    HttpDebug("\n Http server socket error %d\n\r", sock);
                    HttpCloseServer();
                } else {
                    HttpDebug("Http accept retry %d\n\r", sock);
                }
            }

            if ((g_state.uOpenConnections > 0) && 1) {
                for (uConnection = 0; uConnection < HTTP_CORE_MAX_CONNECTIONS;
                        ++uConnection) {
                    // if connection is open and in one of the receiving states, then add this socket to the set
                    if (g_state.connections[uConnection].connectionState
                            == RequestMethod
                            || g_state.connections[uConnection].connectionState
                                    == RequestHeaders
                            || g_state.connections[uConnection].connectionState
                                    == RequestData
                            || g_state.connections[uConnection].connectionState
                                    == DropCurrentHeader
                            || g_state.connections[uConnection].connectionState
                                    == WebSocketDataRecv) {
                        // This connection has data that can be received now

                        // Receive the data into the global packet-receive buffer
                        memset(g_state.packetRecv, 0,
                        HTTP_CORE_MAX_PACKET_SIZE_RECEIVED);
                        iRecvLen = (int) recv(
                                g_state.connections[uConnection].dataSocket,
                                (char *) (g_state.packetRecv),
                                HTTP_CORE_MAX_PACKET_SIZE_RECEIVED, 0); //blocking func with timeout 5sec

                        if (iRecvLen > 0) {
                            g_state.packetRecvSize = iRecvLen;
                            HttpDebug("\n Http Client Data received %d\n\r",
                                    iRecvLen);
                            g_state.connections[uConnection].timeout = 0;

                            blob.uLength = (UINT16) g_state.packetRecvSize;
                            blob.pData = g_state.packetRecv;
                            if (!HttpCore_HandleRequestPacket(uConnection,
                                    blob)) {
                                HttpCore_CloseConnection(uConnection);
                            }
                        } else if ((iRecvLen != SL_EAGAIN)
                                && (iRecvLen != SL_POOL_IS_EMPTY)) {
                            HttpDebug("\n Http Client close %d\n\r", iRecvLen);
                            HttpCore_CloseConnection(uConnection);
                            continue;
                        } else {
                            HttpDebug("Http receive retry %d\n\r", iRecvLen);
                            g_state.connections[uConnection].timeout++;
                            if (g_state.connections[uConnection].connectionState
                                    != WebSocketDataRecv) {
                                if (g_state.connections[uConnection].timeout
                                        >= 60) //60 sec timeout
                                        {
                                    HttpCore_CloseConnection(uConnection);
                                }
                            }
                            continue;
                        }
                    }
                }
            } else if (g_state.uOpenConnections == 0) {
                /* wait 500ms */
                MAP_UtilsDelay(4000000);
            }
        }
    }
}

/**
 * Reset open connection after finishing HTPP transaction
 */
static void HttpCore_ResetConnection(UINT16 uConnection) {
    g_state.connections[uConnection].uContentLeft = 0;
    g_state.connections[uConnection].uSavedBufferSize = 0;
    g_state.connections[uConnection].handler = None;
    g_state.connections[uConnection].request.requestContent.pData = NULL;
    g_state.connections[uConnection].request.requestContent.uLength = 0;
    g_state.connections[uConnection].request.uFlags = 0;
}

/**
 * Initialize the server's global state structure
 */
static void HttpCore_InitWebServer() {
    UINT16 uConnection;
    g_state.packetRecvSize = 0;
    g_state.packetSendSize = 0;
    g_state.uOpenConnections = 0;
    g_state.listenSocket = -1;

    for (uConnection = 0; uConnection < HTTP_CORE_MAX_CONNECTIONS;
            ++uConnection) {
        g_state.connections[uConnection].connectionState = Inactive;
        g_state.connections[uConnection].dataSocket = -1;
        g_state.connections[uConnection].request.uConnection = uConnection;
        g_state.connections[uConnection].HttpAuth = 0;
        HttpCore_ResetConnection(uConnection);
    }

}

/**
 * Close a connection and clean up its state
 */
void HttpCore_CloseConnection(UINT16 uConnection) {
    HttpDebug("Close connection\n\r");

    if (CloseTCPClientSocket(g_state.connections[uConnection].dataSocket)
            >= 0) {
        g_state.connections[uConnection].connectionState = Inactive;
        g_state.connections[uConnection].dataSocket = -1;
        g_state.connections[uConnection].HttpAuth = 0;
        g_state.connections[uConnection].timeout = 0;
        HttpCore_ResetConnection(uConnection);
        if (g_state.uOpenConnections > 0)
            g_state.uOpenConnections--;

        /* Clear send and receive size for this connection */
        g_state.packetRecvSize = 0;
        g_state.packetSendSize = 0;
    }
}

/**
 * Getting the next line in the HTTP headers section
 * This function is called to get the header lines one by one until an empty line is encountered which means the end of the header section
 * The input is the connection and the remaining blob of the received packet
 *
 * @return zero if the whole packet was handled, and need to wait for more data (pLine is not set to anything yet)
 *         negative if some error occurred, and the connection should be closed.
 *         positive if successful. In this case pCurrentLocation is advanced to skip the line and pLine returns the next line, or NULL and 0 if it should be discarded
 */
static int HttpCore_GetNextLine(UINT16 uConnection,
        struct HttpBlob* pCurrentLocation, struct HttpBlob* pLine) {
    UINT16 uLength;
    UINT8* nextLocation;

    // Keep a pointer to the connection state object
    struct HttpConnectionData* pConnection = &g_state.connections[uConnection];

    // search for the line delimiter in the received data
    nextLocation = HttpString_nextDelimiter(HTTP_HEADER_DELIMITER,
            sizeof(HTTP_HEADER_DELIMITER) - 1, *pCurrentLocation);
    uLength = (UINT16) (nextLocation - pCurrentLocation->pData);

    if (pConnection->uSavedBufferSize > 0) {
        // There is previous saved data for this line, so need to concatenate
        if ((pConnection->headerStart[pConnection->uSavedBufferSize - 1] == '\r')
                && (pCurrentLocation->pData[0] == '\n')) {
            // Handle special case where the headers were splitted exactly at the delimiter
            pConnection->headerStart[pConnection->uSavedBufferSize + 1] =
                    pCurrentLocation->pData[0];
            pLine->pData = pConnection->headerStart;
            // Account for the excessive \r in the buffer
            pLine->uLength = pConnection->uSavedBufferSize - 1;
            pConnection->uSavedBufferSize = 0;
            // Advance the current location past this line
            pCurrentLocation->pData += 1;
            pCurrentLocation->uLength -= 1;
            return 1;
        } else {
            // There is saved data, and the delimiter is not split between packets
            if (nextLocation == NULL) {
                // No line delimiter was found in the current packet
                if ((pConnection->uSavedBufferSize + pCurrentLocation->uLength)
                        < HTTP_CORE_MAX_HEADER_LINE_LENGTH) {
                    // There is enough space to append remaining header into saved buffer
                    memcpy(
                            pConnection->headerStart
                                    + pConnection->uSavedBufferSize,
                            pCurrentLocation->pData, pCurrentLocation->uLength);
                    pConnection->uSavedBufferSize += pCurrentLocation->uLength;
                    return 0;
                } else
                // There is not enough space in the saved buffer. This header line will be discarded
                if (pConnection->connectionState == RequestMethod) {
                    // Connection awaits to receive the the HTTP method line
                    // The method line cannot be discarded, drop the connection
                    return -1;
                } else {
                    // Connection awaits to receive the next header line which is not the method
                    // Clean saved buffer, and drop this header line
                    pConnection->uSavedBufferSize = 0;
                    pConnection->connectionState = DropCurrentHeader;
                    return 0;
                }
            } else {
                // Header line delimiter was found in the current packet
                if ((pConnection->uSavedBufferSize + uLength)
                        < HTTP_CORE_MAX_HEADER_LINE_LENGTH) {
                    // This header length is of legal size
                    // Concatenate data from the saved buffer and the current packet
                    memcpy(
                            pConnection->headerStart
                                    + pConnection->uSavedBufferSize,
                            pCurrentLocation->pData, uLength);
                    pConnection->uSavedBufferSize += uLength;
                    pLine->pData = pConnection->headerStart;
                    pLine->uLength = pConnection->uSavedBufferSize;
                    pConnection->uSavedBufferSize = 0;
                } else {
                    // There is not enough space in the saved buffer. This header line will be discarded
                    if (pConnection->connectionState == RequestMethod) {
                        // Connection awaits to receive the the HTTP method line
                        // The method line cannot be discarded, drop the connection
                        return -1;
                    }
                    // Return an epmty line since the header is too long
                    pLine->pData = NULL;
                    pLine->uLength = 0;
                }
                // Advance the current location past this line
                pCurrentLocation->pData += uLength
                        + sizeof(HTTP_HEADER_DELIMITER) - 1;
                pCurrentLocation->uLength -= uLength
                        + sizeof(HTTP_HEADER_DELIMITER) - 1;
                return 1;

            }
        }
    } else {
        // There is no previously saved data for this line
        if (nextLocation == NULL) {
            // No line delimiter was found in the current packet
            if (pCurrentLocation->uLength < HTTP_CORE_MAX_HEADER_LINE_LENGTH) {
                // There is enough space to append remaining header into saved buffer
                memcpy(pConnection->headerStart, pCurrentLocation->pData,
                        pCurrentLocation->uLength);
                pConnection->uSavedBufferSize = pCurrentLocation->uLength;
                return 0;
            } else
            // There is not enough space in the saved buffer
            // This header line will be discarded
            if (pConnection->connectionState == RequestMethod) {
                // Connection awaits to receive the the HTTP method line
                // The method line cannot be discarded, drop the connection
                return -1;
            } else {
                // Connection awaits to receive the next header line which is not the method
                // Clean saved buffer, and drop this header line
                pConnection->uSavedBufferSize = 0;
                pConnection->connectionState = DropCurrentHeader;
                return 0;
            }
        } else {
            // Header line delimiter was found in the current packet
            if (uLength < HTTP_CORE_MAX_HEADER_LINE_LENGTH) {
                // This header length is of legal size
                // The whole line is in the packet buffer
                pLine->pData = pCurrentLocation->pData;
                pLine->uLength = uLength;
            } else {
                // There is not enough space to append remaining header into saved buffer
                if (pConnection->connectionState == RequestMethod) {
                    // Connection awaits to receive the HTTP method line
                    // The method line cannot be discarded, drop the connection
                    return -1;
                }
                // Return an epmty line since the header is too long
                pLine->pData = NULL;
                pLine->uLength = 0;
                pConnection->connectionState = DropCurrentHeader;
            }
            // Advance the current location past this line
            pCurrentLocation->pData += uLength + sizeof(HTTP_HEADER_DELIMITER)
                    - 1;
            pCurrentLocation->uLength -= uLength + sizeof(HTTP_HEADER_DELIMITER)
                    - 1;
            return 1;
        }
    }
}

/**
 * The main state machine to handle an HTTP transaction
 * Every received data packet for a connection is passed to this function for parsing and handling
 *
 * If there is an error the connection will be closed in the end of the transaction
 * It will also be closed if "connection: close" header is received or HTTP version is 1.0
 *
 * @return zero to close the connection
 *         nonzero if packet was consumed successfully, and the connection can handle more data
 */
static int HttpCore_HandleRequestPacket(UINT16 uConnection,
        struct HttpBlob packet) {
    struct HttpBlob currentLocation, line = { 0, NULL };
    int ret;

    currentLocation.pData = packet.pData;
    currentLocation.uLength = packet.uLength;

    //  when no more data is left and the HTTP transaction is not complete then return to wait for more data
    while (1) {
        if (g_state.connections[uConnection].connectionState == RequestMethod
                || g_state.connections[uConnection].connectionState
                        == RequestHeaders
                || g_state.connections[uConnection].connectionState
                        == DropCurrentHeader
                || g_state.connections[uConnection].connectionState
                        == WebSocketRequest
                || g_state.connections[uConnection].connectionState
                        == WebSocketResponse
                || g_state.connections[uConnection].connectionState
                        == WebSocketDataRecv) {
            // Parsing HTTP headers
            int result;

            // Move to WebSockHandler.c if we are in the DataRecv state
            if (g_state.connections[uConnection].connectionState
                    == WebSocketDataRecv)
                if (WSCore_DataRecv(uConnection, &packet))
                    return 1;

            // Respond to websocket handshake
            if (g_state.connections[uConnection].connectionState
                    == WebSocketResponse)
                WSCore_HandshakeResponse(uConnection, line);

            // The received blob is empty, return to wait for more data
            if ((currentLocation.uLength < 1)) {
                return 1;
            }

            // Get next header line if available
            result = HttpCore_GetNextLine(uConnection, &currentLocation, &line);

            if (result > 0)
                HttpDebug("line: %.*s \n\r", line.uLength, line.pData);

            // Method line is too long, or some other error
            if (result < 0)
                return 0;

            // Whole packet was consumed, and no line-break found. Wait for more data
            if (result == 0)
                return 1;

            // Otherwise a new and legal header line is found
        }

        switch (g_state.connections[uConnection].connectionState) {
        case DropCurrentHeader:
            g_state.connections[uConnection].connectionState = RequestHeaders;
            break;
        case RequestMethod:
            HttpAssert((line.pData != NULL) && (line.uLength > 0));
            // If there is an error, then return error to drop the connection
            if (!HttpCore_HandleMethodLine(uConnection, line))
                return 0;
            break;
        case RequestHeaders:
            if (!HttpCore_HandleHeaderLine(uConnection, line))
                return 0;
            break;
        case WebSocketRequest:
            if (!WSCore_HandshakeRequest(uConnection, line))
                return 0;
            break;
        case RequestData:
            ret = HttpCore_HandleRequestData(uConnection, &currentLocation);
            if (ret == 0)
                return 1;
            else if (ret < 0)
                return 0;
            break;
        case Processing:
            // All the request data was received - start final processing of the headers and post data if exists
            switch (g_state.connections[uConnection].handler) {
#ifdef HTTP_CORE_ENABLE_STATIC
            case HttpStatic:
                if (!HttpStatic_ProcessRequest(
                        &g_state.connections[uConnection].request))
                    return 0;
                break;
#endif
#ifdef HTTP_CORE_ENABLE_DYNAMIC
            case HttpDynamic:
                if (!HttpDynamic_ProcessRequest(
                        &g_state.connections[uConnection].request))
                    return 0;
                break;
#endif
            default:
                HttpCore_ProcessNotFound(uConnection);
                break;
            }
            break;
        case ResponseData:
            // This state should never be reached, it is set internally during the processing
            HttpDebug("Response data state in request handling main loop!\n\r");
            HttpAssert(0);
            break;
        case ResponseComplete:
            if ((g_state.connections[uConnection].request.uFlags
                    & HTTP_REQUEST_FLAG_CLOSE) != 0
                    || (g_state.connections[uConnection].request.uFlags
                            & HTTP_REQUEST_1_1) == 0) {
                // Connection should be closed - either "Connection: close" was received or the HTTP version is 1.0
                // Return 0 to close the connection
                return 0;
            }
            // The current HTTP transaction is complete - reset state for the next transaction on this connection
            g_state.connections[uConnection].connectionState = RequestMethod;
            HttpCore_ResetConnection(uConnection);
            break;
        default:
            HttpDebug("Bad state in HttpCore!\n\r");
            HttpAssert(0);
            break;
        }
    }
}

/**
 * This function handles connection initial state
 * Parse the first header line as a method header
 *
 * Method line should be in the form:
 *     GET /resource.html HTTP/1.1\r\n
 *
 * @return nonzero if success
 */
static int HttpCore_HandleMethodLine(UINT16 uConnection, struct HttpBlob line) {
    struct HttpBlob resource;
    UINT8* methodLocation;
    UINT8* versionLocation;
    UINT16 uMethodLength;
    UINT8 method = 0;

    //UINT8* hostLocation;

    // Search for GET token in the input blob
    methodLocation = HttpString_nextToken(HTTP_METHOD_GET,
            sizeof(HTTP_METHOD_GET) - 1, line);
    uMethodLength = sizeof(HTTP_METHOD_GET) - 1;
    if (methodLocation == NULL) {
        // The method is not GET
        // Search for the POST token in the input blob
        methodLocation = HttpString_nextToken(HTTP_METHOD_POST,
                sizeof(HTTP_METHOD_POST) - 1, line);
        uMethodLength = sizeof(HTTP_METHOD_POST) - 1;
        g_state.connections[uConnection].request.uFlags |=
        HTTP_REQUEST_FLAG_METHOD_POST;
        method = POST; 	// This means POST
    } else {
        // Method is GET
        g_state.connections[uConnection].request.requestContent.uLength = 0;
        g_state.connections[uConnection].request.uFlags &=
                ~HTTP_REQUEST_FLAG_METHOD_POST;
        method = GET;		//This means GET
    }
    if (methodLocation != line.pData) {
        methodLocation = HttpString_nextToken(HTTP_METHOD_POST,
                sizeof(HTTP_METHOD_POST) - 1, line);
        uMethodLength = sizeof(HTTP_METHOD_POST) - 1;
        g_state.connections[uConnection].request.uFlags |=
        HTTP_REQUEST_FLAG_METHOD_POST;
        if (methodLocation == NULL || methodLocation != line.pData) {
            // Header does not begin line with GET or POST as it should
            HttpDebug("Unsupported method\n\r");
            return 0;
        }
    }

    // Search for "HTTP/1.1" version token
    versionLocation = HttpString_nextToken(HTTP_VERSION_1P1,
            sizeof(HTTP_VERSION_1P1) - 1, line);
    // Version is 1.1
    if (versionLocation != NULL)
        g_state.connections[uConnection].request.uFlags |= HTTP_REQUEST_1_1;
    else {
        // Search for "HTTP/1.1" version token
        versionLocation = HttpString_nextToken(HTTP_VERSION_1P0,
                sizeof(HTTP_VERSION_1P0) - 1, line);
        // Version is 1.0
        if (versionLocation != NULL)
            g_state.connections[uConnection].request.uFlags &=
                    ~HTTP_REQUEST_1_1;
        else {
            HttpDebug("Bad protocol version\n\r");
            return 0;
        }
    }

    HttpDebug("method Header: %.*s \n\r", line.uLength, line.pData);

    // Find the URL part of the header
    resource.pData = methodLocation + uMethodLength + 1;
    resource.uLength = (UINT16) (versionLocation
            - (methodLocation + uMethodLength + 1) - 1);

    // Determine which handler is supposed to handle this request
    // The handler functions are called according to a hardcoded priority - dynamic, static, default
    // The first handler that returns non zero will handle this request
#ifdef HTTP_CORE_ENABLE_STATIC
    if (HttpStatic_InitRequest(uConnection, resource) != 0)
        g_state.connections[uConnection].handler = HttpStatic;
    else
#endif
#ifdef HTTP_CORE_ENABLE_DYNAMIC
    if (HttpDynamic_InitRequest(uConnection, resource, method) != 0)
        g_state.connections[uConnection].handler = HttpDynamic;
    else
#endif
        g_state.connections[uConnection].handler = None;
    g_state.connections[uConnection].connectionState = RequestHeaders;
    return 1;
}

/**
 * Handle The HTTP headers (after method) one by one
 * If an empty header is received then the headers section is complete
 * Searches for the headers tokens. If important data is found then it is saved in the connection object
 *
 * returns nonzero if sucessful
 */
static int HttpCore_HandleHeaderLine(UINT16 uConnection, struct HttpBlob line) {
    struct HttpBlob blobValue;
    UINT8* pFound;
    UINT32 length;

    // NULL line is received when a header line is too long.
    if (line.pData == NULL)
        return 1;

    // Length is 0, this means than End-Of-Headers marker was reached
    // State of this connection is set to RequestData
    if (line.uLength == 0) {
        g_state.connections[uConnection].connectionState = RequestData;
        return 1;
    }

    //Upgrade: websocket
    //if(g_state.connections[uConnection].request.uFlags == (~HTTP_REQUEST_FLAG_METHOD_POST)|HTTP_REQUEST_1_1)
    //{
    if (HttpString_nextToken(WS_UPGRADE, sizeof(WS_UPGRADE) - 1, line)
            == line.pData) {
        line.pData += sizeof(WS_UPGRADE) - 1 + 1;
        line.uLength -= sizeof(WS_UPGRADE) - 1 + 1;
        pFound = HttpString_nextToken(WS_WEBSOCKET, sizeof(WS_WEBSOCKET) - 1,
                line);
        if (pFound != NULL) {
            g_state.connections[uConnection].connectionState = WebSocketRequest;
            return 1;
        } else
            return 0;
    }
    //}

    // If "Accept-encoding" header then set or clear HTTP_REQUEST_FLAG_ACCEPT_GZIP flag
    if (HttpString_nextToken(HTTP_ACCEPT_ENCODING,
            sizeof(HTTP_ACCEPT_ENCODING) - 1, line) == line.pData) {
        line.pData += sizeof(HTTP_ACCEPT_ENCODING) - 1 + 2;
        line.uLength -= sizeof(HTTP_ACCEPT_ENCODING) - 1 + 2;
        pFound = HttpString_nextToken(HTTP_GZIP, sizeof(HTTP_GZIP) - 1, line);
        if (pFound != NULL)
            g_state.connections[uConnection].request.uFlags |=
            HTTP_REQUEST_FLAG_ACCEPT_GZIP;
        else
            g_state.connections[uConnection].request.uFlags &=
                    ~HTTP_REQUEST_FLAG_ACCEPT_GZIP;
        return 1;
    }

    // If "Content-Length" header then parse the lenght and set uContentLeft to it
    // GET and POST method behave the same
    if (HttpString_nextToken(HTTP_CONTENT_LENGTH,
            sizeof(HTTP_CONTENT_LENGTH) - 1, line) == line.pData) {
        line.pData += sizeof(HTTP_CONTENT_LENGTH) - 1 + 2;
        line.uLength -= sizeof(HTTP_CONTENT_LENGTH) - 1 + 2;
        blobValue.pData = line.pData;
        blobValue.uLength = line.uLength - 2;
        length = HttpString_atou(blobValue);
        g_state.connections[uConnection].uContentLeft = length;
        // Set ignore flag
        if (g_state.connections[uConnection].uContentLeft
                > HTTP_CORE_MAX_HEADER_LINE_LENGTH)
            g_state.connections[uConnection].request.uFlags |=
            HTTP_REQUEST_CONTENT_IGNORED;
        // Prepare the request blob to buffer the content
        g_state.connections[uConnection].request.requestContent.pData =
                g_state.connections[uConnection].headerStart;
        g_state.connections[uConnection].request.requestContent.uLength = 0;
        return 1;
    }
    // If "Connection" header then look for "close" and set or clear HTTP_REQUEST_FLAG_CLOSE flag
    // The default behaviour for keep alive or no such header is to keep the connection open in http version 1.1
    // In http version 1.0 the default behavior is to always close the socket
    if (HttpString_nextToken(HTTP_CONNECTION_CLOSE,
            sizeof(HTTP_CONNECTION_CLOSE) - 1, line) == line.pData) {
        line.pData += sizeof(HTTP_CONNECTION_CLOSE) - 1 + 2;
        line.uLength -= sizeof(HTTP_CONNECTION_CLOSE) - 1 + 2;
        pFound = HttpString_nextToken(HTTP_CLOSE, sizeof(HTTP_CLOSE) - 1, line);
        if (pFound != 0)
            g_state.connections[uConnection].request.uFlags |=
            HTTP_REQUEST_FLAG_CLOSE;
        else
            g_state.connections[uConnection].request.uFlags &=
                    ~HTTP_REQUEST_FLAG_CLOSE;
        return 1;
    }
    // If "Authorization" header the  handle authentication
    if (HttpString_nextToken(HTTP_AUTHORIZATION, sizeof(HTTP_AUTHORIZATION) - 1,
            line) == line.pData) {
        line.pData += sizeof(HTTP_AUTHORIZATION) - 1 + 2;
        line.uLength -= sizeof(HTTP_AUTHORIZATION) - 1 + 2;
        blobValue.pData = line.pData;
        blobValue.uLength = line.uLength;
        //TODO: handle the case when we don't support authentication
#ifdef HTTP_CORE_ENABLE_AUTH
        HttpAuth_RequestAuthenticate(&g_state.connections[uConnection].request, blobValue);
#endif
        return 1;
    }
    // None of the above mentioned headers was recognized so just ignore this header
    return 1;
}

/**
 * Handle The WebSocket headers (after method) one by one
 * If an empty header is received then the headers section is complete
 * Searches for the headers tokens. If important data is found then it is saved in the connection object
 *
 * returns nonzero if sucessful
 */
static int WSCore_HandshakeRequest(UINT16 uConnection, struct HttpBlob line) {
    UINT8* pFound;
    UINT8 bRetVal = 1;
    static UINT16 Origin = 0;
    char *dataCopy = (char *) malloc(strlen((const char *) line.pData));
    memset(dataCopy, '\0', strlen((const char *) line.pData));
    memcpy(dataCopy, (const char *) line.pData,
            strlen((const char *) line.pData));

    // NULL line is received when a header line is too long.
    if (line.pData == NULL) {
        bRetVal = 0;
        goto exit;
    }

    // Length is 0, this means than End-Of-Headers marker was reached
    // State of this connection is set to WebSocketResponse
    if (line.uLength == 0) {
        g_state.connections[uConnection].connectionState = WebSocketResponse;
        bRetVal = 1;
        goto exit;
    }

    //Connection: Upgrade
    if (HttpString_nextToken(WS_CONNECTION, sizeof(WS_CONNECTION) - 1, line)
            == line.pData) {
        line.pData += sizeof(WS_CONNECTION) + 1;
        line.uLength -= sizeof(WS_CONNECTION) + 1;
        pFound = HttpString_nextToken(WS_UPGRADE, sizeof(WS_UPGRADE) - 1, line);
        if (pFound != NULL)
            bRetVal = 1;
        else
            bRetVal = 0;
        goto exit;
    }

    //host: IP address
    if (HttpString_nextToken(WS_HOST, sizeof(WS_HOST) - 1, line)
            == line.pData) {
        line.pData += sizeof(WS_HOST) + 1;
        line.uLength -= sizeof(WS_HOST) + 1;
        bRetVal = 1;
        goto exit;
    }

    //Sec-WebSocket-Key: "Client generated key"
    if (HttpString_nextToken(WS_KEY_REQUEST, sizeof(WS_KEY_REQUEST) - 1, line)
            == line.pData) {
        memcpy(line.pData, dataCopy, strlen((const char *) line.pData));
        // We increment twice to circumvent ':' and ' ' and point at the beginning of the key in base64 encoding
        line.pData += sizeof(WS_KEY_REQUEST) + 1;
        line.uLength -= sizeof(WS_KEY_REQUEST) + 1;
        memset(WS_KEY, '\0', WS_KEY_LENGTH + 1);
        memcpy(WS_KEY, line.pData, WS_KEY_LENGTH);
        HttpDebug("key: %.*s \n\r", WS_KEY);
        if (WS_KEY != NULL) {
            bRetVal = 1;
            goto exit;
        }
    }

    //Sec-WebSocket-Version = 13
    if (HttpString_nextToken(WS_VERSION_REQUEST, sizeof(WS_VERSION_REQUEST) - 1,
            line) == line.pData) {
        line.pData += sizeof(WS_VERSION_REQUEST) + 1;
        line.uLength -= sizeof(WS_VERSION_REQUEST) + 1;
        pFound = HttpString_nextToken(WS_VERSION, sizeof(WS_VERSION) - 1, line);
        if (pFound != NULL) {
            bRetVal = 1;
            goto exit;
        }
    }

    //Sec-WebSocket-Extension - Later versions may support -TODO
    if (HttpString_nextToken(WS_EXTENSION, sizeof(WS_EXTENSION) - 1, line)
            == line.pData) {
        line.pData += sizeof(WS_EXTENSION) + 1;
        line.uLength -= sizeof(WS_EXTENSION) + 1;
        bRetVal = 1;
        goto exit;
    }

    //pragma field - TODO
    if (HttpString_nextToken(WS_PRAGMA, sizeof(WS_PRAGMA) - 1, line)
            == line.pData) {
        line.pData += sizeof(WS_PRAGMA) + 1;
        line.uLength -= sizeof(WS_PRAGMA) + 1;
        bRetVal = 1;
        goto exit;
    }

    //cache-control field - TODO
    if (HttpString_nextToken(WS_CACHE_CONTROL, sizeof(WS_CACHE_CONTROL) - 1,
            line) == line.pData) {
        line.pData += sizeof(WS_CACHE_CONTROL) + 1;
        line.uLength -= sizeof(WS_CACHE_CONTROL) + 1;
        bRetVal = 1;
        goto exit;
    }

    //Origin
    // If this header field doesn't exist, the client is not a browser. Flag will be set accordingly.
    // The server may block communication with an origin it does not validate by sending HTTP error code - Not implemented
    // If it doesn't recognise the origin it will accept data packets from anywhere
    if (Origin == 0) {
        if (HttpString_nextToken(WS_ORIGIN, sizeof(WS_ORIGIN) - 1, line)
                == line.pData) {
            line.pData += sizeof(WS_ORIGIN) + 1;
            line.uLength -= sizeof(WS_ORIGIN) + 1;
            g_state.connections[uConnection].request.uFlags |=
            WS_REQUEST_BROWSER;
            WS_ORIGIN_NAME = (char *) realloc(WS_ORIGIN_NAME, line.uLength + 1);
            if (WS_ORIGIN_NAME == NULL) {
                bRetVal = 0;
                goto exit;
            }
            memset(WS_ORIGIN_NAME, '\0', line.uLength + 1);
            memcpy(WS_ORIGIN_NAME, line.pData, line.uLength);
            Origin = 1;
        } else {
            g_state.connections[uConnection].request.uFlags &=
                    ~WS_REQUEST_BROWSER;
            HttpDebug("Non Browser Client\n");
        }
    }
    exit: free(dataCopy);
    return bRetVal;
    //Sec-WebSocket-Protocol and Extensions not implemented - Optional

}

/**
 * Handles request data for this transaction
 * Behaves the same for POST and GET methods -
 * If content length header was present then read the content for further processing
 * If the content is too long then ignore it
 *
 * @return 1 if successful, pData is updated to skip the handled data
 0 if all data is consumed and need to read more data
 *         negative if an error occurs and the connection should be closed.
 */
static int HttpCore_HandleRequestData(UINT16 uConnection,
        struct HttpBlob* pData) {
    UINT32 uLengthToHandle;

    if (g_state.connections[uConnection].uContentLeft == 0) {
        HttpDebug("Received content. Length=%d, content:\r\n%.*s \n\r",
                g_state.connections[uConnection].request.requestContent.uLength,
                g_state.connections[uConnection].request.requestContent.uLength,
                g_state.connections[uConnection].headerStart);
        g_state.connections[uConnection].connectionState = Processing;
        return 1;
    }

    // Find minimum between the content left to handle and the current blob
    uLengthToHandle = g_state.connections[uConnection].uContentLeft;
    if (uLengthToHandle > pData->uLength)
        uLengthToHandle = pData->uLength;

    // If no new data is received - return and wait for more
    if (uLengthToHandle == 0)
        return 0;

    if ((g_state.connections[uConnection].request.uFlags
            & HTTP_REQUEST_CONTENT_IGNORED) != 0) {
        // Ignore Content
        pData->pData += uLengthToHandle;
        pData->uLength -= (UINT16) uLengthToHandle;
        g_state.connections[uConnection].uContentLeft -= uLengthToHandle;
    } else {
        // Read content
        memcpy(
                g_state.connections[uConnection].headerStart
                        + g_state.connections[uConnection].request.requestContent.uLength,
                pData->pData, uLengthToHandle);
        pData->pData += uLengthToHandle;
        pData->uLength -= (UINT16) uLengthToHandle;
        g_state.connections[uConnection].uContentLeft -= uLengthToHandle;
        g_state.connections[uConnection].request.requestContent.uLength +=
                (UINT16) uLengthToHandle;
    }

    return 1;
}

/**
 * Returns HTTP 404 not found response
 */
static void HttpCore_ProcessNotFound(UINT16 uConnection) {
    // call HttpResponse_CannedError with 404 NOT_FOUND
    HttpResponse_CannedError(uConnection, HTTP_STATUS_ERROR_NOT_FOUND);
}

/**
 * Sends the input blob over the connection socket
 */
static int HttpCore_SendPacket(UINT16 uConnection, struct HttpBlob buffer) {
    //  Send the buffer over the data socket until all the buffer is sent
    while (buffer.uLength > 0) {
        int sent;

        //Don't print more than what can be allocated
        //if(buffer.uLength < 32767)
        HttpDebug("line: %.*s \n\r", buffer.uLength, buffer.pData);

        if (buffer.uLength > HTTP_CORE_MAX_PACKET_SIZE_SEND) {
            if (buffer.pData != g_state.packetSend) {
                memcpy((void *) &g_state.packetSend[0], (void *) buffer.pData,
                HTTP_CORE_MAX_PACKET_SIZE_SEND);
            } else {
                HttpAssert(0);
            }
            sent = ClientSocketSend(g_state.connections[uConnection].dataSocket,
                    (char*) g_state.packetSend, HTTP_CORE_MAX_PACKET_SIZE_SEND);
            if (sent <= 0) {
                // Connection must be closed if send has failed
                return 0;
            }
        } else {
            if (buffer.pData != g_state.packetSend) {
                memcpy((void *) &g_state.packetSend[0], (void *) buffer.pData,
                        buffer.uLength);
            }

            sent = ClientSocketSend(g_state.connections[uConnection].dataSocket,
                    (char*) g_state.packetSend, buffer.uLength);
            if (sent <= 0) {
                // Connection must be closed if send has failed
                return 0;
            }
        }
        buffer.pData += sent;
        buffer.uLength -= (UINT16) sent;
    }
    return 1;
}

/**
 * Add char to the response buffer
 */
void HttpResponse_AddCharToResponseHeaders(char ch) {
    //add char
    g_state.packetSend[g_state.packetSendSize] = ch;
    g_state.packetSendSize++;
}

/**
 * Add UINT32 number to the response buffer
 */
static void HttpResponse_AddNumberToResponseHeaders(UINT32 num) {
    struct HttpBlob resource;
    resource.pData = g_state.packetSend + g_state.packetSendSize;
    resource.uLength = 0;
    HttpString_utoa(num, &resource);
    g_state.packetSendSize += resource.uLength;
}

/**
 * Add a string to the response buffer
 */
void HttpResponse_AddStringToResponseHeaders(char * str, UINT16 len) {
    memcpy(g_state.packetSend + g_state.packetSendSize, str, len);
    g_state.packetSendSize += len;
}

/**
 *  Add header line to response buffer
 *  Adds a line to the header with the provided name value pair
 *  Precondition to this function is that g_state.packetSendSize and g_state.packetSend are correct
 */
static void HttpResponse_AddHeaderLine(char * headerName, UINT16 headerNameLen,
        char * headerValue, UINT16 headerValueLen) {
    HttpResponse_AddStringToResponseHeaders(headerName, headerNameLen);
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders(headerValue, headerValueLen);
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            sizeof(HTTP_HEADER_DELIMITER) - 1);
}

/**
 *  Add Header line to response buffer
 *  Adds a line to the header with the provided name value pair when the value is numeric
 *  precondition to this function is that g_state.packetSendSize and g_state.packetSend are correct
 */
static void HttpResponse_AddHeaderLineNumValue(char * headerName,
        UINT16 uHeaderNameLen, UINT32 headerValue) {
    HttpResponse_AddStringToResponseHeaders(headerName, uHeaderNameLen);
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddNumberToResponseHeaders(headerValue);
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            sizeof(HTTP_HEADER_DELIMITER) - 1);
}

/**
 * Returns status string according to status code
 */
static void HttpStatusString(UINT16 uHttpStatus, struct HttpBlob* status) {
    switch (uHttpStatus) {
    case HTTP_STATUS_OK:
        HttpBlobSetConst(*status, HTTP_STATUS_OK_STR)
        ;
        break;
    case HTTP_STATUS_REDIRECT_PERMANENT:
        HttpBlobSetConst(*status, HTTP_STATUS_REDIRECT_PERMANENT_STR)
        ;
        break;
    case HTTP_STATUS_REDIRECT_TEMPORARY:
        HttpBlobSetConst(*status, HTTP_STATUS_REDIRECT_TEMPORARY_STR)
        ;
        break;
    case HTTP_STATUS_ERROR_UNAUTHORIZED:
        HttpBlobSetConst(*status, HTTP_STATUS_ERROR_UNAUTHORIZED_STR)
        ;
        break;
    case HTTP_STATUS_ERROR_NOT_FOUND:
        HttpBlobSetConst(*status, HTTP_STATUS_ERROR_NOT_FOUND_STR)
        ;
        break;
    case HTTP_STATUS_ERROR_NOT_ACCEPTED:
        HttpBlobSetConst(*status, HTTP_STATUS_ERROR_NOT_ACCEPTED_STR)
        ;
        break;
    case HTTP_STATUS_ERROR_INTERNAL:
        HttpBlobSetConst(*status, HTTP_STATUS_ERROR_INTERNAL_STR)
        ;
        break;
    default:
        HttpDebug("Unknown response status \n\r");
        HttpAssert(0);
        break;
    }
}

int HttpResponse_Headers(UINT16 uConnection, UINT16 uHttpStatus, UINT16 uFlags,
        UINT32 uContentLength, struct HttpBlob contentType,
        struct HttpBlob location) {
    struct HttpBlob status;
    struct HttpBlob packet;
    HttpAssert(g_state.packetSendSize == 0);
    HttpAssert(g_state.connections[uConnection].connectionState == Processing);

    // Get status string according to uHttpStatus
    HttpStatusString(uHttpStatus, &status);

    // Build the response status line in the packet-send buffer: "HTTP/1.1 " followed by the status number as string, a space, the status string, and "\r\n"
    // For example: HTTP/1.1 200 OK\r\n

    // Add http version to sent packet according to the version that was received in the request
    if ((g_state.connections[uConnection].request.uFlags & HTTP_REQUEST_1_1)
            != 0)
        HttpResponse_AddStringToResponseHeaders(HTTP_VERSION_1P1,
                sizeof(HTTP_VERSION_1P1) - 1);
    else
        HttpResponse_AddStringToResponseHeaders(HTTP_VERSION_1P0,
                sizeof(HTTP_VERSION_1P0) - 1);
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddNumberToResponseHeaders(uHttpStatus);
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders((char*) status.pData,
            status.uLength);
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            sizeof(HTTP_HEADER_DELIMITER) - 1);

    // Handle Authentication
    if (uHttpStatus == HTTP_STATUS_ERROR_UNAUTHORIZED) {
#ifdef HTTP_CORE_ENABLE_AUTH
        HttpResponse_GetPacketSendBuffer(&packet);
        packet.pData = packet.pData + packet.uLength;
        packet.uLength = HTTP_CORE_MAX_PACKET_SIZE_SEND - packet.uLength;
        HttpAuth_ResponseAuthenticate(&g_state.connections[uConnection].request, &packet);
        if (packet.uLength > 0)
        g_state.packetSendSize += packet.uLength;
#endif
    }

    // Handle content type
    //    e.g. "Content-Type: text/html\r\n"
    if ((contentType.pData != NULL) && (contentType.uLength > 0))
        HttpResponse_AddHeaderLine(HTTP_CONTENT_TYPE,
                sizeof(HTTP_CONTENT_TYPE) - 1, (char*) contentType.pData,
                contentType.uLength);

    // Handle Content-length
    //    e.g. "Content-Length: 562\r\n"
    if (uContentLength > 0)
        HttpResponse_AddHeaderLineNumValue(HTTP_CONTENT_LENGTH,
                sizeof(HTTP_CONTENT_LENGTH) - 1, uContentLength);
    g_state.connections[uConnection].uContentLeft = uContentLength;

    // Handle compression
    //    e.g. "Content-Encoding: gzip\r\n"
    if ((uFlags & HTTP_RESPONSE_FLAG_COMPRESSED) != 0)
        HttpResponse_AddHeaderLine(HTTP_CONTENT_ENCODING,
                sizeof(HTTP_CONTENT_ENCODING) - 1, HTTP_GZIP,
                sizeof(HTTP_GZIP) - 1);

    // Handle redirection/Location
    //    e.g. "Location: /otherpage.html\r\n"
    if ((location.pData != NULL) && (location.uLength > 0))
        HttpResponse_AddHeaderLine(HTTP_LOCATION, sizeof(HTTP_LOCATION) - 1,
                (char*) location.pData, location.uLength);

    // Add the end of headers marker (\r\n)
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            sizeof(HTTP_HEADER_DELIMITER) - 1);

    // Send all response headers over the connection socket
    packet.pData = g_state.packetSend;
    packet.uLength = g_state.packetSendSize;
    if (!HttpCore_SendPacket(uConnection, packet))
        return 0;

    g_state.packetSendSize = 0;

    // advance state according to need to send content
    if (uContentLength > 0)
        g_state.connections[uConnection].connectionState = ResponseData;
    else
        g_state.connections[uConnection].connectionState = ResponseComplete;

    /*
     Todo: add logic to send the header packet at any point in the middle, if there is not enough room left in it.
     */
    return 1;
}

void HttpResponse_GetPacketSendBuffer(struct HttpBlob* pPacketSendBuffer) {
    pPacketSendBuffer->pData = g_state.packetSend;
    pPacketSendBuffer->uLength = g_state.packetSendSize;
}

int HttpResponse_Content(UINT16 uConnection, struct HttpBlob content) {
    HttpAssert(
            g_state.connections[uConnection].connectionState == ResponseData);
    HttpAssert(
            g_state.connections[uConnection].uContentLeft >= content.uLength);

    // Send the specified content over the data socket
    if (!HttpCore_SendPacket(uConnection, content))
        return 0;

    // Update uContentLeft
    g_state.connections[uConnection].uContentLeft -= content.uLength;

    // If no more content left to send then HTTP transaction is complete
    if (g_state.connections[uConnection].uContentLeft == 0)
        g_state.connections[uConnection].connectionState = ResponseComplete;

    return 1;
}

/*
 * This function creates a response handshake.
 * 1. Version 1.1
 * 2. Upgrade, connection
 * 3. SHA-1 algo for accept key
 * Send Packet
 * @ return : Non zero value if successful
 0 if failure
 */
int WSCore_HandshakeResponse(UINT16 uConnection, struct HttpBlob line) {
    UINT16 WS_STATUS_CODE = 101;
    char WS_HEADER[] = "Web Socket Protocol Handshake";
    struct HttpBlob packet;
    char InKey[WS_KEY_LENGTH + sizeof(WS_MAGIC_STRING) + 1];
    char OutKey[WS_KEY_LENGTH + sizeof(WS_MAGIC_STRING) + 1];
    char EncOutKey[WS_KEY_LENGTH + sizeof(WS_MAGIC_STRING) + 1];
    ;

    // InKey is the key client sent. It is concatenated with magic string stored in server.
    // OutKey is SHA-1 output of InKey
    // EncOutKey is OutKey after base64 encoding.
    memset(InKey, '\0', WS_KEY_LENGTH + sizeof(WS_MAGIC_STRING) + 1);
    memcpy(InKey, (const char*) WS_KEY, strlen((const char*) WS_KEY));
    memset(OutKey, '\0', WS_KEY_LENGTH + sizeof(WS_MAGIC_STRING) + 1);
    memset(EncOutKey, '\0', WS_KEY_LENGTH + sizeof(WS_MAGIC_STRING) + 1);
    strncat(InKey, WS_MAGIC_STRING, sizeof(WS_MAGIC_STRING));

    // The header field stating this is HTTP version 1.1. Version 1.0 is not supported by WebSockets
    // Status code 101 shows successful handshake.

    // HTTP 1.1 101 Web Socket Protocol Handshake
    HttpResponse_AddStringToResponseHeaders(HTTP_VERSION_1P1,
            strlen(HTTP_VERSION_1P1));
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddNumberToResponseHeaders(WS_STATUS_CODE);
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders(WS_HEADER, strlen(WS_HEADER));
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            strlen(HTTP_HEADER_DELIMITER));

    // The Upgrade token tells the client we are moving to websocket protocol.
    HttpResponse_AddStringToResponseHeaders("Upgrade", strlen("Upgrade"));
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders(WS_WEBSOCKET, strlen(WS_WEBSOCKET));
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            strlen(HTTP_HEADER_DELIMITER));

    // Connection: upgrade
    HttpResponse_AddStringToResponseHeaders("Connection", strlen("Connection"));
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders("Upgrade", strlen("Upgrade"));
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            strlen(HTTP_HEADER_DELIMITER));

    // This is where the client sent key is first sent to SHA-1 hash and then base 64 encoded.
    // The sent key must match what the client expects. Otherwise connection will be dropped immediately by client.
    HttpResponse_AddStringToResponseHeaders(WS_ACCEPT, strlen(WS_ACCEPT));
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    if (!WS_HandshakeHash(InKey, OutKey, EncOutKey))
        return 0;
    //CHANGE to KEY after SHA-1 Algorithm
    HttpResponse_AddStringToResponseHeaders(EncOutKey, strlen(EncOutKey));
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            strlen(HTTP_HEADER_DELIMITER));

    // Server : CC3200
    // This field informs the client the server name
    HttpResponse_AddStringToResponseHeaders("Server", strlen("Server"));
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders("CC3200", strlen("CC3200"));
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            strlen(HTTP_HEADER_DELIMITER));

    // This is the same origin supplied by the client. The origin tells the server whether the client is a browser or not.
    // Use this header only on URL's that need to be accessed cross domain. Avoid using for entire domain.
    HttpResponse_AddStringToResponseHeaders("Access-Control-Allow-Origin",
            strlen("Access-Control-Allow-Origin"));
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders(WS_ORIGIN_NAME,
            strlen(WS_ORIGIN_NAME));
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            strlen(HTTP_HEADER_DELIMITER));

    HttpResponse_AddStringToResponseHeaders("Access-Control-Allow-Credentials",
            strlen("Access-Control-Allow-Credentials"));
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders("true", strlen("true"));
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            strlen(HTTP_HEADER_DELIMITER));

    HttpResponse_AddStringToResponseHeaders("Access-Control-Allow-Headers",
            strlen("Access-Control-Allow-Headers"));
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders("content-type",
            strlen("content-type"));
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            strlen(HTTP_HEADER_DELIMITER));

    HttpResponse_AddStringToResponseHeaders("Access-Control-Allow-Headers",
            strlen("Access-Control-Allow-Headers"));
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders("authorization",
            strlen("authorization"));
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            strlen(HTTP_HEADER_DELIMITER));

    HttpResponse_AddStringToResponseHeaders("Access-Control-Allow-Headers",
            strlen("Access-Control-Allow-Headers"));
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders("x-websocket-version",
            strlen("x-websocket-version"));
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            strlen(HTTP_HEADER_DELIMITER));

    HttpResponse_AddStringToResponseHeaders("Access-Control-Allow-Headers",
            strlen("Access-Control-Allow-Headers"));
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders("x-websocket-protocol",
            strlen("x-websocket-protocol"));
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            strlen(HTTP_HEADER_DELIMITER));

    HttpResponse_AddStringToResponseHeaders("Access-Control-Allow-Headers",
            strlen("Access-Control-Allow-Headers"));
    HttpResponse_AddCharToResponseHeaders(':');
    HttpResponse_AddCharToResponseHeaders(' ');
    HttpResponse_AddStringToResponseHeaders("x-websocket-extensions",
            strlen("x-websocket-extensions"));
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            strlen(HTTP_HEADER_DELIMITER));

    // Add the end of headers marker (\r\n)
    HttpResponse_AddStringToResponseHeaders(HTTP_HEADER_DELIMITER,
            sizeof(HTTP_HEADER_DELIMITER) - 1);

    // Send all response headers over the connection socket
    packet.pData = g_state.packetSend;
    packet.uLength = g_state.packetSendSize;
    if (!HttpCore_SendPacket(uConnection, packet))
        return 0;

    g_state.packetSendSize = 0;

    g_state.connections[uConnection].connectionState = WebSocketDataRecv;

    sl_WebSocketHandshakeEvtHdlr(uConnection);

    return 1;

    //Advance to next state ie WebSocketDataResponse
}

/*
 * This function concatenates client generated key with websocket magic string
 * Performs SHA-1 algorithm on this string

 * @return non zero if successful.
 0 if failure
 */

int WS_HandshakeHash(char *InKey, char *OutKey, char *EncOutKey) {
    SHA1((unsigned char *) InKey, (unsigned char *) OutKey);
    ConvertToBase64(EncOutKey, OutKey, strlen(OutKey));
    return 1;
}

int WS_SendPacket(UINT16 uConnection) {
    struct HttpBlob packet;
    int retval;

    // Send all response headers over the connection socket
    packet.pData = g_state.packetSend;
    packet.uLength = g_state.packetSendSize;
    retval = HttpCore_SendPacket(uConnection, packet);

    g_state.packetSendSize = 0;

    g_state.connections[uConnection].connectionState = WebSocketDataRecv;

    return retval;

}

int HttpResponse_CannedRedirect(UINT16 uConnection, struct HttpBlob location,
        UINT16 bPermanent) {
    struct HttpBlob status;
    HttpStatusString((bPermanent == 1 ?
    HTTP_STATUS_REDIRECT_PERMANENT :
                                        HTTP_STATUS_REDIRECT_TEMPORARY),
            &status);

    if (!HttpResponse_Headers(uConnection,
            (bPermanent == 1 ?
            HTTP_STATUS_REDIRECT_PERMANENT :
                               HTTP_STATUS_REDIRECT_TEMPORARY), 0, 0, nullBlob,
            location))
        return 0;
    else
        return 1;
}

int HttpResponse_CannedError(UINT16 uConnection, UINT16 uHttpStatus) {
    struct HttpBlob status;
    HttpStatusString(uHttpStatus, &status);

    if (!HttpResponse_Headers(uConnection, uHttpStatus, 0, status.uLength,
            nullBlob, nullBlob))
        return 0;

    if (!HttpResponse_Content(uConnection, status))
        return 0;
    else
        return 1;
}

/// @}
