//*****************************************************************************
//
// tftp.c - A very simple lwIP TFTP server.
//
// Copyright (c) 2009-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "utils/uartstdio.h"
#include "utils/lwiplib.h"
#include "utils/ustdlib.h"

//*****************************************************************************
//
//! \addtogroup tftp_api
//! @{
//
//*****************************************************************************
#include "utils/tftp.h"

//*****************************************************************************
//
// The TFTP commands.
//
//*****************************************************************************
#define TFTP_RRQ                1
#define TFTP_WRQ                2
#define TFTP_DATA               3
#define TFTP_ACK                4
#define TFTP_ERROR              5

//*****************************************************************************
//
// The UDP port for the TFTP server.
//
//*****************************************************************************
#define TFTP_PORT               69

//*****************************************************************************
//
// Application connection notification callback.
//
//*****************************************************************************
static tTFTPRequest g_pfnRequest;

//*****************************************************************************
//
// Close the TFTP connection and free associated resources.
//
//*****************************************************************************
static void
TFTPClose(tTFTPConnection *psTFTP)
{
    //
    // Tell the application we are closing the connection.
    //
    if(psTFTP->pfnClose)
    {
        psTFTP->pfnClose(psTFTP);
    }

    //
    // Close the underlying UDP connection.
    //
    udp_remove(psTFTP->pPCB);

    //
    // Free the instance data structure.
    //
    mem_free(psTFTP);
}

//*****************************************************************************
//
// Sends a TFTP error packet.
//
//*****************************************************************************
static void
TFTPErrorSend(tTFTPConnection *psTFTP, tTFTPError eError)
{
    unsigned long ulLength;
    unsigned char *pucData;
    struct pbuf *p;

    //
    // How big is this packet going to be?
    //
    ulLength = 5 + strlen(psTFTP->pcErrorString);

    //
    // Allocate a pbuf for this data packet.
    //
    p = pbuf_alloc(PBUF_TRANSPORT, ulLength, PBUF_RAM);
    if(!p)
    {
        return;
    }

    //
    // Get a pointer to the data packet.
    //
    pucData = (unsigned char *)p->payload;

    //
    // Fill in the packet.
    //
    pucData[0] = (TFTP_ERROR >> 8) & 0xff;
    pucData[1] = TFTP_ERROR & 0xff;
    pucData[2] = ((unsigned long)eError >> 8) & 0xff;
    pucData[3] = (unsigned long)eError & 0xff;
    memcpy(&pucData[4], psTFTP->pcErrorString, ulLength - 5);

    //
    // Send the data packet.
    //
    udp_send(psTFTP->pPCB, p);

    //
    // Free the pbuf.
    //
    pbuf_free(p);
}

//*****************************************************************************
//
// Sends a TFTP data packet.
//
//*****************************************************************************
static void
TFTPDataSend(tTFTPConnection *psTFTP)
{
    unsigned long ulLength;
    unsigned char *pucData;
    tTFTPError eError;
    struct pbuf *p;

    //
    // Determine the number of bytes to place into this packet.
    //
    if(psTFTP->ulDataRemaining < (psTFTP->ulBlockNum * TFTP_BLOCK_SIZE))
    {
        ulLength = psTFTP->ulDataRemaining & (TFTP_BLOCK_SIZE - 1);
    }
    else
    {
        ulLength = TFTP_BLOCK_SIZE;
    }

    //
    // Allocate a pbuf for this data packet.
    //
    p = pbuf_alloc(PBUF_TRANSPORT, ulLength + 4, PBUF_RAM);
    if(!p)
    {
        return;
    }

    //
    // Get a pointer to the data packet.
    //
    pucData = (unsigned char *)p->payload;

    //
    // Fill in the packet header.
    //
    pucData[0] = (TFTP_DATA >> 8) & 0xff;
    pucData[1] = TFTP_DATA & 0xff;
    pucData[2] = (psTFTP->ulBlockNum >> 8) & 0xff;
    pucData[3] = psTFTP->ulBlockNum & 0xff;

    //
    // Ask the application to provide the data we need.
    //
    psTFTP->pucData = pucData + 4;
    psTFTP->ulDataLength = ulLength;
    eError = psTFTP->pfnGetData(psTFTP);

    //
    // Send the data packet or, if an error was reported, send an error.
    //
    if(eError == TFTP_OK)
    {
        udp_send(psTFTP->pPCB, p);
    }
    else
    {
        TFTPErrorSend(psTFTP, eError);
        TFTPClose(psTFTP);
    }

    //
    // Free the pbuf.
    //
    pbuf_free(p);
}

//*****************************************************************************
//
// Send an ACK packet back to the TFTP client.
//
//*****************************************************************************
static void
TFTPDataAck(tTFTPConnection *psTFTP)
{
    unsigned char *pucData;
    struct pbuf *p;

    //
    // Allocate a pbuf for this data packet.
    //
    p = pbuf_alloc(PBUF_TRANSPORT, 4, PBUF_RAM);
    if(!p)
    {
        return;
    }

    //
    // Get a pointer to the data packet.
    //
    pucData = (unsigned char *)p->payload;

    //
    // Fill in the packet header.
    //
    pucData[0] = (TFTP_ACK >> 8) & 0xff;
    pucData[1] = TFTP_ACK & 0xff;
    pucData[2] = (psTFTP->ulBlockNum >> 8) & 0xff;
    pucData[3] = psTFTP->ulBlockNum & 0xff;

    //
    // Send the data packet.
    //
    udp_send(psTFTP->pPCB, p);

    //
    // Free the pbuf.
    //
    pbuf_free(p);
}

//*****************************************************************************
//
// Handles datagrams received from the TFTP data connection.
//
//*****************************************************************************
static void
TFTPDataRecv(void *arg, struct udp_pcb *upcb, struct pbuf *p,
             struct ip_addr *addr, u16_t port)
{
    unsigned char *pucData;
    unsigned long ulBlock;
    struct pbuf *pBuf;
    tTFTPConnection *psTFTP;
    tTFTPError eRetcode;

    //
    // Initialize our return code.
    //
    eRetcode = TFTP_ERR_NOT_DEFINED;

    //
    // Get a pointer to the connection instance data.
    //
    psTFTP = (tTFTPConnection *)arg;

    //
    // Get a pointer to the TFTP packet.
    //
    pucData = (unsigned char *)(p->payload);

    //
    // If this is an ACK packet, send back the next block to satisfy an
    // ongoing GET (read) request.
    //
    if((pucData[0] == ((TFTP_ACK >> 8) & 0xff)) &&
       (pucData[1] == (TFTP_ACK & 0xff)))
    {
        //
        // Extract the block number from the acknowledge.
        //
        ulBlock = (pucData[2] << 8) + pucData[3];

        //
        // DEBUG ONLY!
        //
        UARTprintf("ACK %d\n", ulBlock);

        //
        // See if there is more data to be sent.  Note that we need the "<="
        // here to ensure that we send back a zero length packet in the case
        // that the file is a multiple of 512 bytes (i.e. the last packet
        // of valid data was a full packet).
        //
        if((ulBlock * TFTP_BLOCK_SIZE) <= psTFTP->ulDataRemaining)
        {
            //
            // Send the next block of the file.
            //
            psTFTP->ulBlockNum = ulBlock + 1;
            TFTPDataSend(psTFTP);
        }
        else
        {
            //
            // The transfer is complete, so close the data connection.
            //
            TFTPClose(psTFTP);
            psTFTP = NULL;
        }
    }
    else
    {
        //
        // If this is a DATA packet, get the payload and write it to the
        // appropriate location in the serial flash.
        //
        if((pucData[0] == ((TFTP_DATA >> 8) & 0xff)) &&
           (pucData[1] == (TFTP_DATA & 0xff)))
        {
            //
            // This is a data packet.  Extract the block number from the packet
            // and set the offset within the block (stored in ulDataRemaining)
            // to zero.
            //
            psTFTP->ulBlockNum = (pucData[2] << 8) + pucData[3];
            psTFTP->ulDataRemaining = 0;
            psTFTP->ulDataLength = p->len - 4;

            //
            // Pass the data back to the application for handling.  Remember
            // that the data may be stored across several pbufs in the chain.
            // We can't assume it is in a contiguous block.
            //
            psTFTP->pucData = pucData + 4;
            pBuf = p;

            //
            // Keep writing until we run out of data.
            //
            while(pBuf)
            {
                //
                // Pass this block to the application.
                //
                eRetcode = psTFTP->pfnPutData(psTFTP);

                //
                // Was the data written successfully?
                //
                if(eRetcode != TFTP_OK)
                {
                    //
                    // No - drop out.
                    //
                    break;
                }

                //
                // Update the offset so that it is correct for the next pbuf
                // in the chain.
                //
                psTFTP->ulDataRemaining += psTFTP->ulDataLength;

                //
                // Move to the next pbuf in the chain
                //
                pBuf = pBuf->next;
                if(pBuf)
                {
                    psTFTP->pucData = pBuf->payload;
                    psTFTP->ulDataLength = pBuf->len;
                }
            }

            //
            // If we get here and there was an error reported, pass the error
            // back to the TFTP client.
            //
            if(psTFTP && (eRetcode != TFTP_OK))
            {
                //
                // Send the error code to the client.
                //
                TFTPErrorSend(psTFTP, eRetcode);

                //
                // Close the connection.
                //
                TFTPClose(psTFTP);
                psTFTP = NULL;
            }
            else
            {
                //
                // Acknowledge this block.
                //
                TFTPDataAck(psTFTP);

                //
                // Is the transfer finished?
                //
                if(p->tot_len < (TFTP_BLOCK_SIZE + 4))
                {
                    //
                    // We got a short packet so the transfer is complete.  Close
                    // the connection.
                    //
                    TFTPClose(psTFTP);
                    psTFTP = NULL;
                }
            }
        }
        else
        {
            //
            // Is the client reporting an error?
            //
            if((pucData[0] == ((TFTP_ERROR >> 8) & 0xff)) &&
               (pucData[1] == (TFTP_ERROR & 0xff)))
            {
                //
                // Yes - we got an error so close the connection.
                //
                TFTPClose(psTFTP);
                psTFTP = NULL;
            }
        }
    }

    //
    // Free the pbuf.
    //
    pbuf_free(p);
}

//*****************************************************************************
//
// Parses the request string to determine the transfer mode, netascii, octet or
// mail, for this request.
//
//*****************************************************************************
static tTFTPMode
TFTPModeGet(unsigned char *pucRequest, unsigned long ulLen)
{
    unsigned long ulLoop, ulMax;

    //
    // Look for the first zero after the start of the filename string (skipping
    // the first two bytes of the request packet).
    //
    for(ulLoop = 2; ulLoop < ulLen; ulLoop++)
    {
        if(pucRequest[ulLoop] == (unsigned char)0)
        {
            break;
        }
    }

    //
    // Skip past the zero.
    //
    ulLoop++;

    //
    // Did we run off the end of the string?
    //
    if(ulLoop >= ulLen)
    {
        //
        // Yes - this appears to be an invalid request.
        //
        return(TFTP_MODE_INVALID);
    }

    //
    // How much data do we have left to look for the mode string?
    //
    ulMax = ulLen - ulLoop;

    //
    // Now determine which of the modes this request asks for.  Is it ASCII?
    //
    if(!ustrnicmp("netascii", (char *)&pucRequest[ulLoop], ulMax))
    {
        //
        // This is an ASCII file transfer.
        //
        return(TFTP_MODE_NETASCII);
    }

    //
    // Binary transfer?
    //
    if(!ustrnicmp("octet", (char *)&pucRequest[ulLoop], ulMax))
    {
        //
        // This is a binary file transfer.
        //
        return(TFTP_MODE_OCTET);
    }

    //
    // All other strings are invalid or obsolete ("mail" for example).
    //
    return(TFTP_MODE_INVALID);
}

//*****************************************************************************
//
// Handles datagrams received on the TFTP server port.
//
//*****************************************************************************
static void
TFTPRecv(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr,
         u16_t port)
{
    unsigned char *pucData;
    tBoolean bGetRequest;
    tTFTPMode eMode;
    tTFTPError eRetcode;
    tTFTPConnection *psTFTP;

    //
    // Get a pointer to the TFTP packet.
    //
    pucData = (unsigned char *)(p->payload);

    //
    // Is this a read (GET) request?
    //
    if((pucData[0] == ((TFTP_RRQ >> 8) & 0xff)) &&
       (pucData[1] == (TFTP_RRQ & 0xff)))
    {
        //
        // Yes - remember that this is a GET request.
        //
        bGetRequest = true;
    }

    //
    // Is this a write (PUT) request?
    //
    else if((pucData[0] == ((TFTP_WRQ >> 8) & 0xff)) &&
            (pucData[1] == (TFTP_WRQ & 0xff)))
    {
        //
        // Yes - remember that this is a PUT request.
        //
        bGetRequest = false;
    }
    else
    {
        //
        // The request is neither GET nor PUT so just ignore it.
        //
        pbuf_free(p);
        return;
    }

    //
    // What is the mode for this request?
    //
    eMode = TFTPModeGet(pucData, p->len);

    //
    // Was the transfer mode valid?
    //
    if(eMode != TFTP_MODE_INVALID)
    {
        //
        // The transfer mode is valid so allocate a new connection instance
        // and pass this to the client to have it tell us how to proceed.
        //
        psTFTP = (tTFTPConnection *)mem_malloc(sizeof(tTFTPConnection));

        //
        // If we can't allocate the connection instance, all we can do is
        // ignore the datagram.
        //
        if(!psTFTP)
        {
            pbuf_free(p);
            return;
        }

        //
        // Clear out the structure and initialize a few fields.
        //
        memset(psTFTP, 0, sizeof(tTFTPConnection));
        psTFTP->pcErrorString = "Unknown error";

        //
        // Yes - create the new UDP connection and set things up to
        // handle this request.
        //
        psTFTP->pPCB = udp_new();
        udp_recv(psTFTP->pPCB, TFTPDataRecv, psTFTP);
        udp_connect(psTFTP->pPCB, addr, port);

        //
        // Ask the application if it wants to proceed with this request.
        //
        eRetcode = g_pfnRequest(psTFTP, bGetRequest, (char *)(pucData + 2),
                                eMode);

        //
        // Does it want to go on?
        //
        if(eRetcode == TFTP_OK)
        {
            //
            // Yes - what kind of request is this?
            //
            if(bGetRequest)
            {
                //
                // For a GET request, we send back the first block of data.
                //
                psTFTP->ulBlockNum = 1;
                TFTPDataSend(psTFTP);
            }
            else
            {
                //
                // For a PUT request, we acknowledge the transfer which tells
                // the TFTP client that it can start sending us data.
                //
                psTFTP->ulBlockNum = 0;
                TFTPDataAck(psTFTP);
            }
        }
        else
        {
            //
            // The application indicated that there was an error.  Send the
            // error report and close the connection.
            //
            TFTPErrorSend(psTFTP, eRetcode);
            TFTPClose(psTFTP);
            psTFTP = NULL;
        }
    }

    //
    // Free the pbuf.
    //
    pbuf_free(p);
}

//*****************************************************************************
//
//! Initializes the TFTP server module.
//!
//! \param pfnRequest - A pointer to the function which the server will call
//! whenever a new incoming TFTP request is received.  This function must
//! determine whether the request can be handled and return a value telling the
//! server whether to continue processing the request or ignore it.
//!
//! This function initializes the lwIP TFTP server and starts listening for
//! incoming requests from clients.  It must be called after the network stack
//! is initialized using a call to lwIPInit().
//!
//! \return None.
//
//*****************************************************************************
void
TFTPInit(tTFTPRequest pfnRequest)
{
    void *pcb;

    //
    // Remember the application's notification callback.
    //
    g_pfnRequest = pfnRequest;

    //
    // Start listening for incoming TFTP requests.
    //
    pcb = udp_new();
    udp_recv(pcb, TFTPRecv, NULL);
    udp_bind(pcb, IP_ADDR_ANY, TFTP_PORT);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
