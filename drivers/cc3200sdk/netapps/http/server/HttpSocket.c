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
 * @defgroup HttpSocket
 * This module performs all HTTP socket operations
 *
 * @{
 */

#include <string.h>
#include "HttpDebug.h"
#include "HttpSocket.h"
#include "simplelink.h"

#include "osi.h"

#define OSI_DELAY(x)    osi_Sleep(x); //os idle

int OpenTCPServerSocket(unsigned int uiPortNum)
{
  int iSockDesc, iRetVal;
  sockaddr_in sServerAddress;
  SlSockNonblocking_t enableOption;
  enableOption.NonblockingEnabled = 1;

  //
  // opens a secure socket
  //
  if(443 == uiPortNum)
  {
  	iSockDesc = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, SL_SEC_SOCKET);
	}
	else //non secure
	{
		iSockDesc = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, SL_IPPROTO_TCP);
	}

  if( iSockDesc < 0 )
  {
     return -1;
  }
  //non blocking socket - Enable nonblocking mode
  iRetVal = sl_SetSockOpt(iSockDesc,SOL_SOCKET,SL_SO_NONBLOCKING, &enableOption,sizeof(enableOption));
  if(iRetVal < 0)
  {
    CloseTCPServerSocket(iSockDesc);
    return -1;
  }

  if(443 == uiPortNum)
  {
		iRetVal = sl_SetSockOpt(iSockDesc, SL_SOL_SOCKET, SL_SO_SECURE_FILES_PRIVATE_KEY_FILE_NAME,SL_SSL_SRV_KEY, strlen(SL_SSL_SRV_KEY));
		if( iRetVal < 0 )
		{
				CloseTCPServerSocket(iSockDesc);
				return -1;
		}
		iRetVal = sl_SetSockOpt(iSockDesc, SL_SOL_SOCKET, SL_SO_SECURE_FILES_CERTIFICATE_FILE_NAME,SL_SSL_SRV_CERT, strlen(SL_SSL_SRV_CERT));
		if( iRetVal < 0 )
		{
				CloseTCPServerSocket(iSockDesc);
				return -1;
		}
	}

  //
  // Bind - Assign a port to the socket
  //
  sServerAddress.sin_family = AF_INET;
  sServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  sServerAddress.sin_port = htons(uiPortNum);
  if ( bind(iSockDesc, (struct sockaddr*)&sServerAddress, sizeof(sServerAddress)) != 0 )
  {
    CloseTCPServerSocket(iSockDesc);
    return -1;
  }

  return iSockDesc;
}

int CreateTCPServerSocket(unsigned int uiPortNum)
{
  int iSockDesc = -1;
  unsigned char connectRetries = 0;

  while (connectRetries++ < SERVER_MAX_SETUP_RETRY_COUNT)
  {
    iSockDesc = OpenTCPServerSocket(uiPortNum);

    if (iSockDesc < 0)
    {
      continue;
    }

    if(listen(iSockDesc, HTTP_CORE_MAX_CONNECTIONS) != 0)
    {
      CloseTCPServerSocket(iSockDesc);
      iSockDesc = -1;
      continue;
    }
    else
    {
      connectRetries = 0;
      break;
    }
  }

  return iSockDesc;
}

int CloseTCPServerSocket(int iSockDesc)
{
  int ittr = 0;

  if(iSockDesc < 0)
  {
    return 0 ;
  }

  do
  {
    if(sl_Close(iSockDesc) >= 0)
    {
      iSockDesc = -1;
      HttpDebug("Http server socket closed\n\r");
      return 0;
    }
    else
    {
    	HttpDebug("\n Http client socket close error\n\r");
      OSI_DELAY(500);//wait 500ms
    }
    ittr++;
  }while(ittr < 3);

  return -1;
}

int CreateTCPClientSocket(int iSockDesc)
{
  sockaddr sClientAddr;
  SlSocklen_t uiClientAddrLen = sizeof(sClientAddr);
  int sock = -1;
  SlTimeval_t timeVal;
  SlSockNonblocking_t enableOption;

  sock = accept(iSockDesc, &sClientAddr, &uiClientAddrLen);
  if(sock >= 0)
  {
    enableOption.NonblockingEnabled = 0;

    //Blocking socket - Enable blocking mode
    if(sl_SetSockOpt(sock,SOL_SOCKET,SL_SO_NONBLOCKING, &enableOption,sizeof(enableOption)) < 0)
    {
      CloseTCPClientSocket(sock);
      return -1;
    }

    timeVal.tv_sec =  1;             // 1 Seconds
    timeVal.tv_usec = 0;             // Microseconds. 10000 microseconds resoultion
    if((sl_SetSockOpt(sock,SOL_SOCKET,SL_SO_RCVTIMEO, &timeVal, sizeof(timeVal))) < 0)
    {
      CloseTCPClientSocket(sock);
      return -1;
    }// Enable receive timeout
  }

  return sock;
}

int CloseTCPClientSocket(int iSockDesc)
{
  int ittr = 0;

  if(iSockDesc < 0)
  {
    return 0;
  }

  do
  {
    if(sl_Close(iSockDesc) >= 0)
    {
      iSockDesc = -1;
      HttpDebug("\n Http client socket closed\n\r");

	    return 0;
    }
    else
    {
    	HttpDebug("\n client socket close error\n\r");
      OSI_DELAY(500);//wait 500ms
    }
    ittr++;
  }while(ittr < 3);

  return -1;
}

int ClientSocketSend(long socket, char * buffer, unsigned int len)
{
  int send_len = 0, Ittr = 0;

  do
  {
    send_len = (int)send((int)socket, buffer, (int)len, 0);

    if(send_len > 0)
    {
      if(len != send_len)
      {
    	  HttpDebug("client Send length is not matching %d \n\r", send_len);
        send_len = -1;
      }
      return send_len;
    }
    else if(send_len != SL_EAGAIN)
    {
    	HttpDebug("\n client socket send error %d\n\r", send_len);
    	return -1;
    }

    Ittr++;

  }  while((SL_EAGAIN == send_len));

  HttpDebug("\n client send time out %d\n\r", send_len);

  return -1;
}


/// @}
