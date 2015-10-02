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


#ifndef _TFTPINC_H
#define _TFTPINC_H

#include <stdlib.h>
#include <string.h>
#include "tftp.h"
#include "simplelink.h"

#define TFTP_HEADER 4
#define DATA_SIZE (SEGSIZE + TFTP_HEADER) // SEGSIZE declared in TFTP.H as 512
#define PORT_TFTP 69

typedef unsigned long         IPN;             // IP Address in NETWORK format

/* IPV6 Address in Network Format. */
typedef struct IP6N
{
    union
    {
        unsigned char   addr8[16];
        unsigned short  addr16[8];
        unsigned long  addr32[4];
    }u;
}IP6N;

//typedef void *         HANDLE;
typedef int        HANDLE;
typedef HANDLE           SOCKET;          // OS Socket Type

typedef struct sockaddr      SA;
typedef struct sockaddr      *PSA;

// structure of a TFTP instance
typedef struct _tftp
{
    IPN    PeerAddress;             // Peer address supplied by caller
    char   *szFileName;             // Filename supplied by caller
    char   *Buffer;                 // Buffer supplied by caller
    unsigned long BufferSize;              // Buffer size supplied by caller
    SOCKET Sock;                    // Socket used for transfer
    char   *PacketBuffer;           // Packet Buffer
    unsigned long Length;                  // Length of packet send and reveive
    unsigned long BufferUsed;              // Amount of "Buffer" used
    unsigned long FileSize;                // Size of specified file
    unsigned short NextBlock;               // Next expected block
    unsigned short ErrorCode;               // TFTP error code from server
    int    MaxSyncError;            // Max SYNC errors remaining
    struct sockaddr_in tmpaddr;     // inaddr for RECV
    struct sockaddr_in peeraddr;    // inaddr for SEND

#ifdef _INCLUDE_IPv6_CODE
    IP6N   Peer6Address;            // Peer address supplied by caller
    struct sockaddr_in6 tmp6addr;   // inaddr for RECV
    struct sockaddr_in6 peer6addr;  // inaddr for SEND
#endif
} TFTP;

#define MAX_SYNC_TRIES          4       // Max retries
#define TFTP_SOCK_TIMEOUT       10      // Packet Timeout in Seconds

#endif
