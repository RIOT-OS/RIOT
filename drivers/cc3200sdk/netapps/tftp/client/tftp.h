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


#ifndef _TFTP_H_
#define _TFTP_H_

//
// Trivial File Transfer Protocol
//
#define SEGSIZE         512     // data segment size

//
// TFTP Packet types.

 void   *mmAlloc( unsigned long Size );
 void   mmFree( void* pv );
 void   mmCopy( void* pDst, void* pSrc, unsigned long Size );
 void   mmZeroInit( void* pDst, unsigned long Size );

 /*!
  * 	\brief Recv data over TFTP
  * 	This function receives data to a specified filename from the destination IP.
  *
  * 	\param[in] TftpIP				Server IP
  * 	\param[in] *szFileName			Pointer to string holding filename.
  * 	\param[in] *FileBuffer			Pointer to buffer where received data will be stored
  * 	\param[in] *FileSize			Pointer to filesize
  * 	\param[in] *pErrorCode			Pointer to error code that will be populated if receive is unsuccessful
  *
  * 	\return							1 - If file was sucessfully transferred
  *     								0 - If the file was transferred but too large for the buffer
  * 									<0 - Error
  *
  * 	\note							TFTPERROR_ERRORCODE: TFTP server error code. The error code
  *          						is written to pErrorCode, and an error message is
  *         							written to FileBuffer. The length of the error message
  *          						is written to FileSize.
  *
  *          						This API is called from the application to initiate the receive.
  */

int sl_TftpRecv( unsigned long TftpIP, char *szFileName, char *FileBuffer,unsigned long *FileSize, unsigned short *pErrorCode );

/*!
 * 	\brief Send data over TFTP
 * 	This function sends data from a specified filename to the destination IP.
 *	This API is called from the application to initiate the send
 *
 * 	\param[in] TftpIP				Server IP
 * 	\param[in] *szFileName			Pointer to string holding filename.
 * 	\param[in] *FileBuffer			Pointer to buffer holding data to be sent
 * 	\param[in] *FileSize			Pointer to filesize
 * 	\param[in] *pErrorCode			Pointer to error code that will be populated if send is unsuccessful
 *
 * 	\return							1 - If file was sucessfully transferred
 *     								0 - If the file was transferred but too large for the buffer
 * 									<0 - Error
 *
 * 	\note							TFTPERROR_ERRORCODE: TFTP server error code. The error code
 *          						is written to pErrorCode, and an error message is
 *         							written to FileBuffer. The length of the error message
 *          						is written to FileSize.
 *
 *
 */

int sl_TftpSend( unsigned long TftpIP, char *szFileName, char *FileBuffer,unsigned long *FileSize, unsigned short *pErrorCode );


#define  bzero(ptr,sz)       mmZeroInit(ptr,sz)
#define  bcopy(src,dst,sz)   mmCopy(dst,src,sz)

//
#define TFTP_RRQ     01              // read request
#define TFTP_WRQ     02              // write request
#define TFTP_DATA    03              // data packet
#define TFTP_ACK     04              // acknowledgement
#define TFTP_ERROR   05              // error code

#define TFTPERROR_SOCKET 		(-1)
#define TFTPERROR_FAILED 		(-2)
#define TFTPERROR_ERRORREPLY 	(-3)
#define TFTPERROR_BADPARAM 		(-4)
#define TFTPERROR_RESOURCES 	(-5)
#define TFTPERROR_OPCODE_FAILED (-6)
#define TFTPERROR_DATA_FAILED   (-7)

struct tftphdr
{
    short   opcode;             // packet type
    short   block;              // block #
    char    data[1];            // data or error string
};

//
// Error codes.
//
#define EUNDEF          0       // not defined
#define ENOTFOUND       1       // file not found
#define EACCESS         2       // access violation
#define ENOSPACE        3       // disk full or allocation exceeded
#define EBADOP          4       // illegal TFTP operation
#define EBADID          5       // unknown transfer ID
#define EEXISTS         6       // file already exists
#define ENOUSER         7       // no such user

#endif


