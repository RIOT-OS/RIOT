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


#ifndef HTTPSOCKET_H_
#define HTTPSOCKET_H_

/**
 * @defgroup HttpSocket
 * This module performs all HTTP socket operations
 *
 * @{
 */


#include "HttpCore.h"

// Max number of times server must retry to setup connection
#define SERVER_MAX_SETUP_RETRY_COUNT           3

#define SL_SSL_SRV_KEY    			"/cert/serverkey.der"   /* Server key file ID */
#define SL_SSL_SRV_CERT  			  "/cert/servercert.der"  /* Server certificate file ID */


//****************************************************************************
//
//! Creates a TCP server socket. It calls OpenTCPServerSocket.
//!
//! \param uiPortNum is the port number to bind to
//!
//! This function
//!    1. Creates a TCP socket and binds to it
//!	   2. Sets the socket options
//!
//! \return Soceket Descriptor, < 1 if error.
//
//****************************************************************************
int CreateTCPServerSocket(unsigned int uiPortNum);

//****************************************************************************
//
//! Creates a TCP socket and binds to it
//!
//! \param uiPortNum is the port number to bind to
//!
//! This function
//!    1. Creates a TCP socket and binds to it
//!
//! \return Soceket Descriptor, < 1 if error.
//
//****************************************************************************
int OpenTCPServerSocket(unsigned int uiPortNum);

//****************************************************************************
//
//! Accepts the client socket. Sets it to blocking with receive timeout.
//!
//! \param iSockDesc is the socket ID obtained from CreateTCPServerSocket
//!
//! This function
//!    1. Accepts the client socket.
//!	   2. Sets to blocking mode with RECV timeout
//!
//! \return socket handler is returned after the SockID is put in listening state
//
//****************************************************************************
int CreateTCPClientSocket(int iSockDesc);

//****************************************************************************
//
//! Closes TCP client socket when the Http connection with that client is closed.
//!
//! \param iSockDesc is the socket ID obtained from CreateTCPServerSocket
//!
//! This function
//!    1. Calls sl_close. Tries 3 times before returning error
//!
//! \return 0 if success
//!			-1 if error
//
//****************************************************************************
int CloseTCPClientSocket(int iSockDesc);

//****************************************************************************
//
//! Sends over the socket to a specific client. In non blocking socket type, it keeps looping till send is success
//!
//! \param socket is the handle obtained from CreateTCPClientSocket
//!		   buffer is the pointer to the data to be sent
//!		   len is the length of the data to be sent
//!
//! This function
//!    1. Calls send and keeps trying as long as the return is SL_EAGAIN.
//!
//! \return length successfully sent if it was a success
//!			error code if there was a failure.
//!
//!			It will not return an error if simplelink returns SL_EAGAIN. Instead it will keep trying till it succeeds.
//
//****************************************************************************
int ClientSocketSend(long socket, char * buffer, unsigned int len);

//****************************************************************************
//
//! Closes TCP server socket.
//!
//! \param iSockDesc is the socket ID obtained from CreateTCPServerSocket
//!
//! This function
//!    1. Calls sl_close. Tries 3 times before returning error
//!
//! \return 0 if success
//!			-1 if error
//
//****************************************************************************
int CloseTCPServerSocket(int iSockDesc);




#endif /* HTTPSOCKET_H_ */

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
