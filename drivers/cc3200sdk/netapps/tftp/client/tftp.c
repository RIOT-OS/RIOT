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
// Protocol Name     -          Trivial File Transfer Protcol
// Protocol Overview -  	TFTP is a simple protocol to transfer files. It has been implemented
//                                       on top of the Internet User Datagram protocol. so it may be
//                                       used to move files between machines on different
//                                       networks implementing UDP
//                            Refer: http://tools.ietf.org/html/rfc1350
//*****************************************************************************



#include "simplelink.h"
#include "tftpinc.h"
#include "tftp.h"
#include "utils.h"


static int  tftpGetFile( TFTP *pTftp);
static int  tftpSocketSetup( TFTP *pTftp );
static void tftpRRQBuild(TFTP *pTftp);
static void tftpWRQBuild(TFTP *pTftp);
static void tftpDATABuild(TFTP *tftp);
static void tftpACKBuild(TFTP *pTftp);
static void tftpFlushPackets(TFTP *pTftp);
static int  tftpReadPacket( TFTP *pTftp );
static int  tftpProcessPacket( TFTP *pTftp );
static int  tftpSend( TFTP *pTftp);

/*!
 * 	\brief  TFTP socket setup. Creates a UDP socket at known port 69 and binds to it.
 *
 *
 * 	\param[in] *pTftp				Pointer to the structure of type TFTP.
 *
 * 	\return							void
 */
static int tftpSocketSetup( TFTP *pTftp )
{
    int            rc;   // Return Code
    struct timeval timeout;

    // Create UDP socket
    pTftp->Sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if( pTftp->Sock < 0 )
    {
        rc = TFTPERROR_SOCKET;
        goto ABORT;
    }

    // Initialize the bind the local address
    bzero( &pTftp->tmpaddr, sizeof(struct sockaddr_in) );
    pTftp->tmpaddr.sin_family  = AF_INET;

    // Assign local name to the unnamed socket
    // Do not care about local Host Address or Port (both 0)
    // If no error, bind returns 0
    if( bind( pTftp->Sock, (PSA)&pTftp->tmpaddr, sizeof(pTftp->tmpaddr) ) < 0 )
    {
        rc = TFTPERROR_SOCKET;
        goto ABORT;
    }

    // Set the socket IO timeout
    timeout.tv_sec  = TFTP_SOCK_TIMEOUT;
    timeout.tv_usec = 0;
    if( setsockopt( pTftp->Sock, SOL_SOCKET, SO_RCVTIMEO,
        &timeout, sizeof(timeout)) < 0 )
    {
        rc = TFTPERROR_SOCKET;
        goto ABORT;
    }
    if( setsockopt( pTftp->Sock, SOL_SOCKET, SO_RCVTIMEO,
                    &timeout, sizeof(timeout)) < 0 )
    {
        rc = TFTPERROR_SOCKET;
        goto ABORT;
    }

    // Initialize the foreign address - we don't use "connect", since
    // the peer port will change. We use sendto()
    bzero( &pTftp->peeraddr, sizeof(struct sockaddr_in) );
    pTftp->peeraddr.sin_family      = AF_INET;
    pTftp->peeraddr.sin_addr.s_addr = sl_Htonl(pTftp->PeerAddress);
    pTftp->peeraddr.sin_port        = htons(PORT_TFTP);
    rc = 0;

ABORT:
    return(rc);
}


/*!
 * 	\brief  Build a RRQ packet. Contains file name to be read from server.
 * 			Mode is by default octet.
 *
 *
 * 	\param[in] *pTftp				Pointer to the structure of type TFTP.
 *
 * 	\return							void
 */
static void tftpRRQBuild(TFTP *pTftp)
{
    struct tftphdr *RRQ_Packet;
    static char *pszOctet = "octet";
    char   *pRRQ_Data;

    RRQ_Packet = (struct tftphdr *)pTftp->PacketBuffer;

    // A request packet consists of an opcode (RRQ) followed
    // by a NULL terminated filename and mode ("octet")

    // Opcode = RRQ
    RRQ_Packet->opcode = htons(TFTP_RRQ);

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


/*!
 * 	\brief  Build a WRQ packet. Contains file name to be written to server.
 * 			Mode is octet by default.
 *
 *
 * 	\param[in] *pTftp				Pointer to the structure of type TFTP.
 *
 * 	\return							void
 */
static void tftpWRQBuild(TFTP *pTftp)
{
    struct tftphdr *WRQ_Packet;
    static char *pszOctet = "octet";
    char   *pWRQ_Data;

    WRQ_Packet = (struct tftphdr *)pTftp->PacketBuffer;

    /* A request packet consists of an opcode (WRQ) followed */
    /* by a NULL terminated filename and mode ("octet") */

    /* Opcode = WRQ */
    WRQ_Packet->opcode = htons(TFTP_WRQ);

    /* Get a pointer to the rest of the packet */
    pWRQ_Data = (char *)&WRQ_Packet->block;

    /* Copy NULL terminated filename string to request */
    /* increment data pointer by length of Filename (and terminating '0') */
    strcpy(pWRQ_Data, pTftp->szFileName);
    pWRQ_Data += strlen(pTftp->szFileName) + 1;

    /* Copy NULL terminated mode string to request */
    /* increment data pointer by length of mode (and terminating '0') */
    strcpy(pWRQ_Data, pszOctet);
    pWRQ_Data += strlen(pszOctet) + 1;

    /*  calculate length of packet */
    pTftp->Length = (int)(pWRQ_Data - (char *)WRQ_Packet);

    return;
}

/*!
 * 	\brief  Build an ACK packet
 *
 *
 * 	\param[in] *pTftp				Pointer to the structure of type TFTP.
 *
 * 	\return							void
 */

static void tftpACKBuild(TFTP *pTftp)
{
    struct tftphdr *ACK_Packet;

    ACK_Packet = (struct tftphdr *)pTftp->PacketBuffer;

    ACK_Packet->opcode = htons(TFTP_ACK);

    // Build the rest of the ACK packet

    // Block Number
    ACK_Packet->block = htons( pTftp->NextBlock );

    //  Set length of packet
    pTftp->Length = 4;  //  Opcode + Block
}

/*!
 * 	\brief  Build a data packet
 *
 *
 * 	\param[in] *pTftp				Pointer to the structure of type TFTP.
 *
 * 	\return							void
 */

static void tftpDATABuild(TFTP *pTftp)
{
	struct tftphdr *Data_Packet;

	Data_Packet = (struct tftphdr *)pTftp->PacketBuffer;

	/* A Data packet consists of an opcode (DATA) followed */
	/* by a Block number and the Data bytes */
	/* All but last packet must have SEGSIZE bytes of data*/

	/* Opcode = DATA */
	Data_Packet->opcode = htons(TFTP_DATA);

	/* Copy Block number of file to packet */
	/* NextBlock must be incremented prior to this function call*/
	Data_Packet->block = htons( pTftp->NextBlock );

	/* Copy data from caller buffer into Tftp data string field*/
	mmCopy(Data_Packet->data,(pTftp->Buffer+pTftp->BufferUsed),(int)(pTftp->Length));

	return;
}

/*!
 * 	\brief  Send a pre built packet from the peer socket
 *
 *
 * 	\param[in] *pTftp				Pointer to the structure of type TFTP.
 *
 * 	\return							0 - Send successful
 * 									-1 - Error
 */

static int tftpSend( TFTP *pTftp)
{
    int rc = 0;
    int BytesSent;

    BytesSent = sendto(pTftp->Sock, pTftp->PacketBuffer,
                       (int)pTftp->Length, 0,(struct sockaddr *)&pTftp->peeraddr,
                       sizeof(pTftp->peeraddr));

    if ( BytesSent != (int)pTftp->Length )
        rc = TFTPERROR_SOCKET;

    return(rc);
}

/*!
 * 	\brief  Read a packet from the peer socket
 *
 *
 * 	\param[in] *pTftp				Pointer to the structure of type TFTP. Pointer to the data buffer is populated here.
 * 									Sets bytes read to ZERO on a timeout
 *
 * 	\return							0 - Receieve successful
 * 									-1 - Error
 */
static int tftpReadPacket( TFTP *pTftp )
{
    int                rc = 0;
    int                addrLength;
    struct tftphdr     *ReadBuffer;
    signed long              BytesRead;

    ReadBuffer = (struct tftphdr *)pTftp->PacketBuffer;

RETRY:
#if 0
    // Don't allow stray traffic to keep us alive
    if( (TimeStart+TFTP_SOCK_TIMEOUT) <= llTimerGetTime(0) )
    {
        BytesRead = 0;
        goto ABORT;
    }
#endif
    // Attempt to read data
    addrLength = sizeof(pTftp->tmpaddr);
    BytesRead  = recvfrom( pTftp->Sock, ReadBuffer, DATA_SIZE, 0,
                           (struct sockaddr *)&pTftp->tmpaddr, (SlSocklen_t *)&addrLength );

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
    if( pTftp->tmpaddr.sin_addr.s_addr != sl_Htonl(pTftp->PeerAddress) )
        goto RETRY;

    // If the peer port is NOT TFTP, then it must match our expected
    // peer.
    if( pTftp->peeraddr.sin_port != htons(PORT_TFTP) )
    {
        if( pTftp->tmpaddr.sin_port != pTftp->peeraddr.sin_port )
            goto RETRY;
    }

ABORT:
    pTftp->Length = (unsigned long)BytesRead;  //  Store number of bytes read
    return(rc);
}


/*!
 * 	\brief  Flush all input from socket
 *
 *
 * 	\param[in] *pTftp				Pointer to the structure of type TFTP.
 *
 * 	\return							void
 */
static void tftpFlushPackets(TFTP *pTftp)
{
    int bytesread;

    //  Sleep for a second
    UtilsDelay(80*1000*1000);

    do
    {
        bytesread = recv( pTftp->Sock, pTftp->PacketBuffer,
                          DATA_SIZE, MSG_DONTWAIT );
    } while( bytesread > 0 );
}

/*!
 * 	\brief  Attempt to ReSync a failed transfer
 *
 *
 * 	\param[in] *pTftp				Pointer to the structure of type TFTP.
 *
 * 	\return							0 - Success
 * 									-1 - Error
 */

int tftpReSync( TFTP *pTftp )
{
    int rc = 0;
    unsigned short OpCode;
    struct tftphdr *ReadBuffer;

    ReadBuffer = (struct tftphdr *)pTftp->PacketBuffer;

    /* Fluch pending input packets */
    tftpFlushPackets( pTftp );

    /* Abort if too many Sync errors */
    pTftp->MaxSyncError--;
    if( pTftp->MaxSyncError == 0 )
    {
        rc = TFTPERROR_FAILED;
        goto ABORT;   /* Too Many Sync Errors */
    }

    /* Back up expected block */
    pTftp->NextBlock--;


	OpCode = (unsigned short) ntohs(ReadBuffer->opcode);
		switch (OpCode)
		{
		case TFTP_DATA:
			/* Resend last packet - if we're on block ZERO, resend the initial */
			 /* request. */
			if( !pTftp->NextBlock )
				tftpRRQBuild( pTftp );
			else
				tftpACKBuild( pTftp );
			break;

		case TFTP_ACK:
			/* Resend last packet - if we're on block ZERO, resend the initial */
			 /* request. */
			if( pTftp->NextBlock == 0xFFFF)
				tftpWRQBuild( pTftp );
			else
				tftpDATABuild( pTftp );
			break;

		default:
			rc = TFTPERROR_FAILED;
			goto ABORT;

		}

    /* Send the packet */
    rc = tftpSend( pTftp );
    if( rc < 0 )
        goto ABORT;

    pTftp->NextBlock++;  /*  Increment next expected BLOCK */

ABORT:
    return(rc);
}

/*!
 * 	\brief  This is the core of the TFTP. The packet opcode is parsed and accordingly processed.
 * 			Processes a data packet obtained from ReadPacket()
 *
 *
 * 	\param[in] *pTftp				Pointer to the structure of type TFTP.
 *
 * 	\return							1 - Operation Complete
 *     								0 - Operation Progressing
 * 									<0 - Error
 */

static int tftpProcessPacket( TFTP *pTftp )
{
    int    rc = 0, done = 0;
    unsigned short OpCode;
    unsigned short ServerBlock;
    unsigned long CopySize;
    struct tftphdr *ReadBuffer;

    ReadBuffer = (struct tftphdr *)pTftp->PacketBuffer;

    // Check for a bad packet - resynch on error
    if( !pTftp->Length )
        return( tftpReSync( pTftp ) );

    OpCode = (unsigned short) ntohs(ReadBuffer->opcode);
    switch (OpCode)
    {
    case TFTP_ERROR :
        // Copy the error code
        pTftp->ErrorCode = (unsigned short) ntohs(ReadBuffer->block);

        // If the buffer is large enough, copy the error message
        pTftp->Length -= TFTP_HEADER;          /* Get payload length */

        //Copy file data if room left in buffer is large enough
        if( pTftp->BufferSize >= pTftp->Length )
        {
            bcopy( ReadBuffer->data, pTftp->Buffer, (int)pTftp->Length );
            pTftp->BufferUsed = pTftp->Length;
        }
        else
            pTftp->BufferUsed = 0;

        rc = TFTPERROR_ERRORREPLY;
        break;

   case TFTP_DATA :
        //Received Data, verify BLOCK correct
        ServerBlock = (unsigned short) ntohs(ReadBuffer->block);

        // If this is not the block we're expecting, resync
        if (pTftp->NextBlock != ServerBlock)
        {
            rc = tftpReSync( pTftp );
            pTftp->Length = 0;
            break;
        }

        // If this is the first block, reset our port number.
        if( pTftp->NextBlock == 1 )
            pTftp->peeraddr.sin_port = pTftp->tmpaddr.sin_port;  /* Update TID */

        //  Block is for me!
        pTftp->MaxSyncError = MAX_SYNC_TRIES;  /* reset Sync Counter */
        pTftp->Length -= TFTP_HEADER;          /* Get payload length */
        CopySize = pTftp->Length;              /* Copy length */
        pTftp->FileSize += CopySize;           /* Track the file length */

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

                /* Track the number of bytes used */
                pTftp->BufferUsed += CopySize;
            }
        }

        /* If we received a partial block, we're done */
        if( pTftp->Length < SEGSIZE )
            done = 1;

        /* Need to acknowledge this block */
        tftpACKBuild( pTftp );
        rc = tftpSend( pTftp );
        if( rc < 0 )
            break;

        /*  Increment next expected BLOCK */
        pTftp->NextBlock++;

        /* Our done flag is the return code on success */
        rc = done;
        break;

	case TFTP_ACK:
		/*Received Acknowledgement*/
		/*Is it for the right packet?*/
		ServerBlock = (unsigned short) ntohs(ReadBuffer->block);

		/* If this is not the block we're expecting, resync */
        if (pTftp->NextBlock != ServerBlock)
        {
            rc = tftpReSync( pTftp );
            pTftp->Length = 0;
            break;
        }

		/* If this is the first block, reset our port number. */
        if( pTftp->NextBlock == 0 )
        	pTftp->peeraddr.sin_port = pTftp->tmpaddr.sin_port;  /* Update TID */

		/*Set length of data to be sent*/
		CopySize = (pTftp->BufferSize - pTftp->BufferUsed);

		// If buffer has less than 512 bytes
		if(CopySize < SEGSIZE)
			pTftp->Length = CopySize;

		/*Else send SEGSIZE*/
		else
			pTftp->Length = SEGSIZE;

		// If nothing is left to send, we are done.
		if(CopySize==0)
			done = 1;

		/*  Increment next expected BLOCK */
        pTftp->NextBlock++;

		/* Need to send next DATA packet*/
        tftpDATABuild( pTftp );

		/* Add length copied to BufferUsed*/
		pTftp->BufferUsed += pTftp->Length;

		pTftp->Length += 4;

        rc = tftpSend( pTftp );
        if( rc < 0 )
            break;

        /* Our done flag is the return code on success */
         rc = done;

	break;


    default:
        rc = TFTPERROR_FAILED;
        break;
    }

    return(rc);
}

/*!
 * 	\brief Get specific file over TFTP
 * 	This function receives the entire file before returning.
 * 	It is called from sl_TftpRecv.
 *
 * 	\param[in] *pTftp				Pointer to the structure of type TFTP. The structure is populated
 * 									by data it receives via TFTP.
 *
 * 	\return							1 - If file was sucessfully transferred
 *     								0 - If the file was transferred but too large for the buffer
 * 									<0 - Error
 */
static int tftpGetFile( TFTP *pTftp )
{
    int rc = 0;

    // Build the request packet
    tftpRRQBuild( pTftp );

    // Send the request packet
    rc = tftpSend( pTftp );
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
        rc = tftpReadPacket( pTftp );
        if( rc < 0 )
            goto ABORT;

        // Process the reply packet
        rc = tftpProcessPacket( pTftp );
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

/*!
 * 	\brief Send specific file over TFTP
 * 	This function sends the entire file before returning.
 * 	It is called from sl_TftpSend.
 *
 * 	\param[in] *pTftp				Pointer to the structure of type TFTP holding the send packet along with the header
 *
 * 	\return							1 - If file was sucessfully transferred
 *     								0 - If the file was transferred but too large for the buffer
 * 									<0 - Error
 */

static int tftpSendFile( TFTP *pTftp )
{
    int rc = 0;

    /* Build the request packet */
    tftpWRQBuild( pTftp );

    /* Send the request packet */
    rc = tftpSend( pTftp );
    if( rc < 0 )
       goto ABORT;

    /* Now look for response packets */
    pTftp->MaxSyncError   = MAX_SYNC_TRIES;  /* set sync error counter */
    pTftp->NextBlock      = 0;               /* first block expected is "1" */

    for(;;)
    {
        /* Try and get a reply packet */
        rc = tftpReadPacket( pTftp );
        if( rc < 0 )
            goto ABORT;

        /* Process the reply packet */
        rc = tftpProcessPacket( pTftp );
        if( rc < 0 )
            goto ABORT;

        /* If done, break out of loop */
        if( rc == 1 )
            break;
    }

    /* If the receive buffer was too small, return 0, else return 1 */
    if( pTftp->BufferUsed < pTftp->FileSize )
       rc = 0;
    else
       rc = 1;

ABORT:
    return(rc);
}

int sl_TftpRecv( unsigned long TftpIP, char *szFileName, char *FileBuffer,
                unsigned long *FileSize, unsigned short *pErrorCode )
{
    TFTP *pTftp;
    int rc;          // Return Code

    // Quick parameter validation
    if( !szFileName || !FileSize || (*FileSize != 0 && !FileBuffer) )
        return( TFTPERROR_BADPARAM );

    // Malloc Parameter Structure
    if( !(pTftp = (TFTP* )mmAlloc( sizeof(TFTP) )) )
        return( TFTPERROR_RESOURCES );

    // Initialize parameters to "NULL"
    bzero( pTftp, sizeof(TFTP) );
    pTftp->Sock = -1;

    // Malloc Packet Data Buffer
    if( !(pTftp->PacketBuffer = (char*) mmAlloc( DATA_SIZE )) )
    {
        rc = TFTPERROR_RESOURCES;
        goto ABORT;
    }

    // store IP in network byte order
    pTftp->PeerAddress = TftpIP;

    // Setup initial socket
    rc = tftpSocketSetup( pTftp );
    if( rc < 0 )
        goto ABORT;

    //  Socket now registered and available for use. Get the data
    pTftp->szFileName  = szFileName;
    pTftp->Buffer      = FileBuffer;
    pTftp->BufferSize  = *FileSize;   // Do not read more than can fit in file

    // Get the requested file
    rc = tftpGetFile( pTftp );
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
    if( pTftp->Sock != -1 )
        close( pTftp->Sock );
    if( pTftp->PacketBuffer )
        mmFree( pTftp->PacketBuffer );
    mmFree( pTftp );

    return(rc);
}


int sl_TftpSend( unsigned long TftpIP, char *szFileName, char *FileBuffer,
                unsigned long *FileSize, unsigned short *pErrorCode )
{
    TFTP *pTftp;
    int rc;          /* Return Code */

    /* Quick parameter validation */
    if( !szFileName || !FileSize || (*FileSize != 0 && !FileBuffer) )
        return( TFTPERROR_BADPARAM );

    /* Malloc Parameter Structure */
    if( !(pTftp = (TFTP *)mmAlloc( sizeof(TFTP) )) )
        return( TFTPERROR_RESOURCES );

    /* Initialize parameters to "NULL" */
    bzero( pTftp, sizeof(TFTP) );
    pTftp->Sock = -1;

    /* Malloc Packet Data Buffer */
    if( !(pTftp->PacketBuffer = (char *)mmAlloc( DATA_SIZE )) )
    {
        rc = TFTPERROR_RESOURCES;
        goto ABORT;
    }

    /* store IP in network byte order */
    pTftp->PeerAddress = TftpIP;

    /* Setup initial socket */
    rc = tftpSocketSetup( pTftp );
    if( rc < 0 )
        goto ABORT;

    /*  Socket now registered and available for use. Get the data */
    pTftp->szFileName  = szFileName;
    pTftp->Buffer      = FileBuffer;
    pTftp->BufferSize  = *FileSize;   /* Do not read more than can fit in file */

    /* Get the requested file */
    rc = tftpSendFile( pTftp );
    if( rc < 0 )
    {
        /* ERROR CONDITION */

        /* Set the "FileSize" to the actual number of bytes copied */
        *FileSize = pTftp->BufferUsed;

        /* If the ErrorCode pointer is valid, copy it */
        if( pErrorCode )
            *pErrorCode = pTftp->ErrorCode;
    }
    else
    {
        /* SUCCESS CONDITION */

        /* Set the "FileSize" to the file size (regardless of bytes copied) */
        *FileSize = pTftp->FileSize;
    }

ABORT:
    if( pTftp->Sock != -1 )
        close( pTftp->Sock );
    if( pTftp->PacketBuffer )
        mmFree( pTftp->PacketBuffer );
    mmFree( pTftp );

    return(rc);
}

void*	mmAlloc( unsigned long Size )
{
	return (void*)malloc(Size);
}

void   mmFree( void* pv )
{
	free(pv);
}

void   mmCopy( void* pDst, void* pSrc, unsigned long Size )
{
	memcpy(pDst,pSrc,Size);
}

void   mmZeroInit( void* pDst, unsigned long Size )
{
	memset(pDst,0,Size);
}

