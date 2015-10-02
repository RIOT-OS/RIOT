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


#include "simplelink.h"
#include "tftpinc.h"

#ifdef _INCLUDE_IPv6_CODE

static int  tftp6GetFile( TFTP *pTftp);
static int  tftp6SocketSetup( TFTP *pTftp, UINT32 scope_id );
static void tftp6RRQBuild(TFTP *pTftp);
static void tftp6ACKBuild(TFTP *pTftp);
static void tftp6FlushPackets(TFTP *pTftp);
static int  tftp6ReadPacket( TFTP *pTftp );
static int  tftp6ProcessPacket( TFTP *pTftp );
static int  tftp6Send( TFTP *pTftp);

/** 
 *  @b Description
 *  @n  
 *      The function sets up the initial socket for the TFTP transfer.
 *
 *  @param[in]  pTFTP
 *      Pointer to the TFTP Control block.
 *  @param[in]  scope_id
 *      This is the scope identifier i.e. the interface index on which
 *      the TFTP server is present. Required only for Link Local addresses.
 *
 *  @retval
 *      Success - 0
 *  @retval
 *      Error   - <0
 */
static int tftp6SocketSetup( TFTP *pTftp, UINT32 scope_id )
{
    int            rc;   // Return Code
    struct timeval timeout;

    // Create UDP socket
    pTftp->Sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    if( pTftp->Sock == INVALID_SOCKET )
    {
        rc = TFTPERROR_SOCKET;
        goto ABORT;
    }

    // Initialize the bind the local address
    bzero( &pTftp->tmp6addr, sizeof(struct sockaddr_in6) );
    pTftp->tmp6addr.sin6_family   = AF_INET6;    

    // Assign local name to the unnamed socket
    // Do not care about local Host Address or Port (both 0)
    // If no error, bind returns 0
    if( bind( pTftp->Sock, (PSA)&pTftp->tmp6addr, sizeof(pTftp->tmp6addr) ) < 0 )
    {
        rc = TFTPERROR_SOCKET;
        goto ABORT;
    }

    // Set the socket IO timeout
    timeout.tv_sec  = TFTP_SOCK_TIMEOUT;
    timeout.tv_usec = 0;
    if( setsockopt( pTftp->Sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0 )
    {
        rc = TFTPERROR_SOCKET;
        goto ABORT;
    }
    if( setsockopt( pTftp->Sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0 )
    {
        rc = TFTPERROR_SOCKET;
        goto ABORT;
    }

    // Initialize the foreign address - we don't use "connect", since
    // the peer port will change. We use sendto()
    bzero( &pTftp->peer6addr, sizeof(struct sockaddr_in) );
    pTftp->peer6addr.sin6_family      = AF_INET6;
    pTftp->peer6addr.sin6_port        = htons(PORT_TFTP);
    pTftp->peer6addr.sin6_scope_id    = scope_id;
    mmCopy((void *)&pTftp->peer6addr.sin6_addr, &pTftp->Peer6Address, sizeof(struct in6_addr));
    rc = 0;

ABORT:
    return(rc);
}

/** 
 *  @b Description
 *  @n  
 *      The function build an initial file request packet.
 *
 *  @param[in]  pTFTP
 *      Pointer to the TFTP Control block.
 *
 *  @retval
 *      Not Applicable.
 */
static void tftp6RRQBuild(TFTP *pTftp)
{
    struct tftphdr *RRQ_Packet;
    static char *pszOctet = "octet";
    char   *pRRQ_Data;

    RRQ_Packet = (struct tftphdr *)pTftp->PacketBuffer;

    // A request packet consists of an opcode (RRQ) followed
    // by a NULL terminated filename and mode ("octet")

    // Opcode = RRQ
    RRQ_Packet->opcode = htons(RRQ);

    // Get a pointer to the rest of the packet
    pRRQ_Data = (char *)&RRQ_Packet->block;

    // Copy NULL terminated filename string to request
    // increment data pointer by length of Filename (and terminating '0')
    strcpy(pRRQ_Data, pTftp->szFileName);
    pRRQ_Data += strlen(pTftp->szFileName) + 1;

    // Copy NULL terminated mode string to request
    // increment data pointer by length of mode (and terminating '0')
    strcpy(pRRQ_Data, pszOctet);
    pRRQ_Data += strlen(pszOctet) + 1;

    //  calculate length of packet
    pTftp->Length = (int)(pRRQ_Data - (char *)RRQ_Packet);

    return;
}

/** 
 *  @b Description
 *  @n  
 *      The function build a TFTP Block ACK Packet.
 *
 *  @param[in]  pTFTP
 *      Pointer to the TFTP Control block.
 *
 *  @retval
 *      Not Applicable.
 */
static void tftp6ACKBuild(TFTP *pTftp)
{
    struct tftphdr *ACK_Packet;

    ACK_Packet = (struct tftphdr *)pTftp->PacketBuffer;

    ACK_Packet->opcode = htons(ACK);

    // Build the rest of the ACK packet

    // Block Number
    ACK_Packet->block = htons( pTftp->NextBlock );

    //  Set length of packet
    pTftp->Length = 4;  //  Opcode + Block
}

/** 
 *  @b Description
 *  @n  
 *      The function sends a pre-built packet to the peer.
 *
 *  @param[in]  pTFTP
 *      Pointer to the TFTP Control block.
 *
 *  @retval
 *      Success - Number of bytes sent.
 *  @retval
 *      Error   - <0
 */
static int tftp6Send( TFTP *pTftp)
{
    int rc = 0;
    int BytesSent;

    BytesSent = sendto(pTftp->Sock, pTftp->PacketBuffer,
                       (int)pTftp->Length, 0,(struct sockaddr *)&pTftp->peer6addr,
                       sizeof(pTftp->peer6addr));

    if ( BytesSent != (int)pTftp->Length )
        rc = TFTPERROR_SOCKET;

    return(rc);
}

/** 
 *  @b Description
 *  @n  
 *      The function reads a data packet from the peer.
 *
 *  @param[in]  pTFTP
 *      Pointer to the TFTP Control block.
 *
 *  @retval
 *      Success - Number of bytes read.
 *  @retval
 *      Error   - 0 (Timeout)
 */
static int tftp6ReadPacket( TFTP *pTftp )
{
    int                rc = 0;
    int                addrLength;
    struct tftphdr     *ReadBuffer;
    INT32              BytesRead;
    UINT32             TimeStart;
    IP6N               TmpAddress;

    ReadBuffer = (struct tftphdr *)pTftp->PacketBuffer;
 //   TimeStart = llTimerGetTime(0);

RETRY:
/*
    // Don't allow stray traffic to keep us alive
    if( (TimeStart+TFTP_SOCK_TIMEOUT) <= llTimerGetTime(0) )
    {
        BytesRead = 0;
        goto ABORT;
    }
*/
    // Attempt to read data
    addrLength = sizeof(pTftp->tmp6addr);
    BytesRead  = recvfrom( pTftp->Sock, ReadBuffer, DATA_SIZE, 0,
                           (struct sockaddr *)&pTftp->tmp6addr, &addrLength );

    // Handle read errors first
    if( BytesRead < 0 )
    {
        // On a timeout error, ABORT with no error
        // Else report error
        if( BytesRead == EWOULDBLOCK )
            BytesRead = 0;
        else
            rc = TFTPERROR_SOCKET;
        goto ABORT;
    }

    // If this packet is not from our peer, retry
    mmCopy ((void *)&TmpAddress, (void *)&pTftp->tmp6addr.sin6_addr, sizeof(IP6N));
    if (IPv6CompareAddress(TmpAddress, pTftp->Peer6Address) == 0)
        goto RETRY;

    // If the peer port is NOT TFTP, then it must match our expected
    // peer.
    if( pTftp->peer6addr.sin6_port != htons(PORT_TFTP) )
    {
        if( pTftp->tmp6addr.sin6_port != pTftp->peer6addr.sin6_port )
            goto RETRY;
    }

ABORT:
    pTftp->Length = (UINT32)BytesRead;  //  Store number of bytes read
    return(rc);
}

/** 
 *  @b Description
 *  @n  
 *      The function flushes all packets which are pending on the receive
 *      socket.
 *
 *  @param[in]  pTftp
 *      Pointer to the TFTP Control block.
 *
 *  @retval
 *      Not Applicable.
 */
static void tftp6FlushPackets(TFTP *pTftp)
{
    int bytesread;

    //  Sleep for a second
    TaskSleep( 1000 );

    do
    {
        bytesread = recv( pTftp->Sock, pTftp->PacketBuffer,
                          DATA_SIZE, MSG_DONTWAIT );
    } while( bytesread > 0 );
}

/** 
 *  @b Description
 *  @n  
 *      The function attempts to resyn after a failed transfer.
 *
 *  @param[in]  pTftp
 *      Pointer to the TFTP Control block.
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   0
 */
static int tftp6ReSync( TFTP *pTftp )
{
    int rc = 0;

    // Fluch pending input packets
    tftp6FlushPackets( pTftp );

    // Abort if too many Sync errors
    pTftp->MaxSyncError--;
    if( pTftp->MaxSyncError == 0 )
    {
        rc = TFTPERROR_FAILED;
        goto ABORT;   // Too Many Sync Errors
    }

    // Back up expected block
    pTftp->NextBlock--;

    // Resend last packet - if we're on block ZERO, resend the initial
    // request.
    if( !pTftp->NextBlock )
        tftp6RRQBuild( pTftp );
    else
        tftp6ACKBuild( pTftp );

    // Send the packet
    rc = tftp6Send( pTftp );
    if( rc < 0 )
        goto ABORT;

    pTftp->NextBlock++;  //  Increment next expected BLOCK

ABORT:
    return(rc);
}

/**
 *  @b Description
 *  @n  
 *      The function processes a received data packet.
 *
 *  @param[in]  pTFTP
 *      Pointer to the TFTP Control block.
 *
 *  @retval
 *      Operation Complete      -   1
 *  @retval
 *      Operation Progressing   -   0
 *  @retval
 *      Error Condition         -   <0
 */
static int tftp6ProcessPacket( TFTP *pTftp )
{
    int    rc = 0, done = 0;
    UINT16 OpCode;
    UINT16 ServerBlock;
    UINT32 CopySize;
    struct tftphdr *ReadBuffer;

    ReadBuffer = (struct tftphdr *)pTftp->PacketBuffer;

    // Check for a bad packet - resynch on error
    if( !pTftp->Length )
        return( tftp6ReSync( pTftp ) );

    OpCode = (UINT16) ntohs(ReadBuffer->opcode);
    switch (OpCode)
    {
    case ERROR :
        // Copy the error code
        pTftp->ErrorCode = (UINT16) ntohs(ReadBuffer->block);

        // If the buffer is large enough, copy the error message
        pTftp->Length -= TFTP_HEADER;          // Get payload length

        // Copy file data if room left in buffer is large enough
        if( pTftp->BufferSize >= pTftp->Length )
        {
            bcopy( ReadBuffer->data, pTftp->Buffer, (int)pTftp->Length );
            pTftp->BufferUsed = pTftp->Length;
        }
        else
            pTftp->BufferUsed = 0;

        rc = TFTPERROR_ERRORREPLY;
        break;

   case DATA :
        // Received Data, verify BLOCK correct
        ServerBlock = (UINT16) ntohs(ReadBuffer->block);

        // If this is not the block we're expecting, resync
        if (pTftp->NextBlock != ServerBlock)
        {
            rc = tftp6ReSync( pTftp );
            pTftp->Length = 0;
            break;
        }

        // If this is the first block, reset our port number.
        if( pTftp->NextBlock == 1 )
            pTftp->peer6addr.sin6_port = pTftp->tmp6addr.sin6_port;  // Update TID

        //  Block is for me!
        pTftp->MaxSyncError = MAX_SYNC_TRIES;  // reset Sync Counter
        pTftp->Length -= TFTP_HEADER;          // Get payload length
        CopySize = pTftp->Length;              // Copy length
        pTftp->FileSize += CopySize;           // Track the file length

        // Copy file data if room left in buffer is large enough
        if( pTftp->BufferSize > pTftp->BufferUsed )
        {
            if( (pTftp->BufferSize - pTftp->BufferUsed) < CopySize)
               CopySize = pTftp->BufferSize - pTftp->BufferUsed;

            if( CopySize )
            {
                // Add it to our receive buffer
                bcopy( ReadBuffer->data, (pTftp->Buffer+pTftp->BufferUsed),
                       (int)CopySize );

                // Track the number of bytes used
                pTftp->BufferUsed += CopySize;
            }
        }

        // If we received a partial block, we're done
        if( pTftp->Length < SEGSIZE )
            done = 1;

        // Need to acknowledge this block
        tftp6ACKBuild( pTftp );
        rc = tftp6Send( pTftp );
        if( rc < 0 )
            break;

        //  Increment next expected BLOCK
        pTftp->NextBlock++;

        // Our done flag is the return code on success
        rc = done;
        break;

    default:
        rc = TFTPERROR_FAILED;
        break;
    }

    return(rc);
}

/**
 *  @b Description
 *  @n  
 *      The function receives a file using TFTP.
 *
 *  @param[in]  pTFTP
 *      Pointer to the TFTP Control block.
 *
 *  @retval
 *      Success                         -   1
 *  @retval
 *      File downloaded exceeds buffer  -   0
 *  @retval
 *      Error                           -   <0
 */
static int tftp6GetFile( TFTP *pTftp )
{
    int rc = 0;

    // Build the request packet
    tftp6RRQBuild( pTftp );

    // Send the request packet
    rc = tftp6Send( pTftp );
    if( rc < 0 )
       goto ABORT;

    //
    // Now look for response packets
    //
    pTftp->MaxSyncError   = MAX_SYNC_TRIES;  // set sync error counter
    pTftp->NextBlock      = 1;               // first block expected is "1"

    for(;;)
    {
        // Try and get a reply packet
        rc = tftp6ReadPacket( pTftp );
        if( rc < 0 )
            goto ABORT;

        // Process the reply packet
        rc = tftp6ProcessPacket( pTftp );
        if( rc < 0 )
            goto ABORT;

        // If done, break out of loop
        if( rc == 1 )
            break;
    }

    // If the receive buffer was too small, return 0, else return 1
    if( pTftp->BufferUsed < pTftp->FileSize )
       rc = 0;
    else
       rc = 1;

ABORT:
    return(rc);
}

/**
 *  @b Description
 *  @n  
 *      The function retreives a file using TFTP.
 *
 *  @param[in]  TftpIP
 *      IPv6 Address of the Server.
 *  @param[in]  scope_id
 *      This is the scope id. If the IP Address of the server is Link
 *      Local then this needs to be populated to the device index for
 *      the interface on which the TFTP Server is present. For Global
 *      Addresses this is ignored.
 *  @param[in]  szFileName
 *      File Name to be retreived.
 *  @param[in]  FileBuffer
 *      Buffer in which the downloaded file is stored.
 *  @param[in]  FileSize
 *      Size of the buffer
 *  @param[out] pErrorCode
 *      Size of the buffer
 *
 *  @retval
 *      Success                         -   1
 *  @retval
 *      File downloaded exceeds buffer  -   0
 *  @retval
 *      Error                           -   <0
 */
int Nt6TftpRecv (IP6N TftpIP, UINT32 scope_id, char *szFileName, char *FileBuffer,
                 UINT32 *FileSize, UINT16 *pErrorCode)
{
    TFTP *pTftp;
    int rc;          // Return Code

    // Quick parameter validation
    if( !szFileName || !FileSize || (*FileSize != 0 && !FileBuffer) )
        return( TFTPERROR_BADPARAM );

    // Malloc Parameter Structure
    if( !(pTftp = mmAlloc( sizeof(TFTP) )) )
        return( TFTPERROR_RESOURCES );

    // Initialize parameters to "NULL"
    bzero( pTftp, sizeof(TFTP) );
    pTftp->Sock = INVALID_SOCKET;

    // Malloc Packet Data Buffer
    if( !(pTftp->PacketBuffer = mmAlloc( DATA_SIZE )) )
    {
        rc = TFTPERROR_RESOURCES;
        goto ABORT;
    }

    // store IP in network byte order
    pTftp->Peer6Address = TftpIP;

    // Setup initial socket
    rc = tftp6SocketSetup( pTftp, scope_id);
    if( rc < 0 )
        goto ABORT;

    //  Socket now registered and available for use. Get the data
    pTftp->szFileName  = szFileName;
    pTftp->Buffer      = FileBuffer;
    pTftp->BufferSize  = *FileSize;   // Do not read more than can fit in file

    // Get the requested file
    rc = tftp6GetFile( pTftp );
    if( rc < 0 )
    {
        // ERROR CONDITION

        // Set the "FileSize" to the actual number of bytes copied
        *FileSize = pTftp->BufferUsed;

        // If the ErrorCode pointer is valid, copy it
        if( pErrorCode )
            *pErrorCode = pTftp->ErrorCode;
    }
    else
    {
        // SUCCESS CONDITION

        // Set the "FileSize" to the file size (regardless of bytes copied)
        *FileSize = pTftp->FileSize;
    }

ABORT:
    if( pTftp->Sock != INVALID_SOCKET )
        close( pTftp->Sock );
    if( pTftp->PacketBuffer )
        mmFree( pTftp->PacketBuffer );
    mmFree( pTftp );

    return(rc);
}

#endif /* _INCLUDE_IPv6_CODE */

