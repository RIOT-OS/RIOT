//*****************************************************************************
//
// tftp.h - Public function prototypes and globals related to the lwIP TFTP
//          server.
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
#ifndef _TFTP_H_
#define _TFTP_H_

//*****************************************************************************
//
//! \addtogroup tftp_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! TFTP error codes.  Note that this enum is mapped so that all positive
//! values match the TFTP protocol-defined error codes.
//
//*****************************************************************************
typedef enum
{
    TFTP_OK = -1,
    TFTP_ERR_NOT_DEFINED = 0,
    TFTP_FILE_NOT_FOUND = 1,
    TFTP_ACCESS_VIOLATION = 2,
    TFTP_DISK_FULL = 3,
    TFTP_ILLEGAL_OP = 4,
    TFTP_UNKNOWN_TID = 5,
    TFTP_FILE_EXISTS = 6,
    TFTP_NO_SUCH_USER = 7
} tTFTPError;

//*****************************************************************************
//
//! TFTP file transfer modes.  This enum contains members defining ASCII
//! text transfer mode (TFTP_MODE_NETASCII), binary transfer mode
//! (TFTP_MODE_OCTET) and a marker for an invalid mode (TFTP_MODE_INVALID).
//
//*****************************************************************************
typedef enum
{
    TFTP_MODE_NETASCII,
    TFTP_MODE_OCTET,
    TFTP_MODE_INVALID
} tTFTPMode;

//*****************************************************************************
//
//! Data transfer under TFTP is performed using fixed-size blocks.  This label
//! defines the size of a block of TFTP data.
//
//*****************************************************************************
#define TFTP_BLOCK_SIZE 512

//*****************************************************************************
//
// Callback function prototypes passed to TFTPInit.  These functions receive
// notification of incoming GET and PUT requests, allowing the client to
// to decide whether to accept the request or not.
//
//*****************************************************************************
struct _tTFTPConnection;

typedef tTFTPError (* tTFTPRequest)(struct _tTFTPConnection *psTFTP,
                                    tBoolean bGet, char *pucFileName,
                                    tTFTPMode eMode);
typedef tTFTPError (* tTFTPTransfer)(struct _tTFTPConnection *psTFTP);
typedef void (* tTFTPClose)(struct _tTFTPConnection *psTFTP);

//*****************************************************************************
//
//! The TFTP connection control structure.  This is passed to a client on all
//! callbacks relating to a given TFTP connection.  Depending upon the callback,
//! the client may need to fill in values to various fields or use field values
//! to determine where to transfer data from or to.
//
//*****************************************************************************
typedef struct _tTFTPConnection
{
    //
    //! Pointer to the start of the buffer into which GET data should be copied
    //! or from which PUT data should be read.
    //
    unsigned char *pucData;

    //
    //! The length of the data requested in response to a single pfnGetData
    //! callback or the size of the received data for a pfnPutData callback.
    //
    unsigned long ulDataLength;

    //
    //! Count of remaining bytes to send during a GET request or the byte
    //! offset within a block during a PUT request.  The application must set
    //! this field to the size of the requested file during the tTFTPRequest
    // callback if a GET request is to be accepted.
    //
    unsigned long ulDataRemaining;

    //
    //! Application function which is called whenever more data is required to
    //! satisfy a GET request.  The function must copy ulDataLength bytes into
    //! the buffer pointed to by pucData.
    //
    tTFTPTransfer pfnGetData;

    //
    //! Application function which is called whenever a packet of file data is
    //! received during a PUT request.  The function must save the data to the
    //! target file using ulBlockNum and ulDataRemaining to indicate the
    //! position of the data in the file, and return an appropriate error code.
    //! Note that several calls to this function may be made for a given
    //! received TFTP block since the underlying networking stack may have
    //! split the TFTP packet between several packets and a callback is made
    //! for each of these.  This avoids the need for a 512 byte buffer.  The
    //! ulDataRemaining is used in these cases to indicate the offset of the
    //! data within the current block.
    //
    tTFTPTransfer pfnPutData;

    //
    //! Application function which is called when the TFTP connection is to
    //! be closed.  The function should tidy up and free any resources
    //! associated with the connection prior to returning.
    //
    tTFTPClose pfnClose;

    //
    //! This field may be used by the client to store an application-specific
    //! pointer that will be accessible on all callbacks from the TFTP module
    //! relating to this connection.
    //
    unsigned char *pucUser;

    //
    //! Pointer to an error string which the client must fill in if reporting
    //! an error.  This string will be sent to the TFTP client in any case
    //! where pfnPutData or pfnGetData return a value other than TFTP_OK.
    //
    char *pcErrorString;

    //
    //! A pointer to the underlying UDP connection.  Applications must not
    //! modify this field.
    //
    struct udp_pcb *pPCB;

    //
    //! The current block number for an ongoing TFTP transfer.  Applications
    //! may read this value to determine which data to return on a pfnGetData
    //! callback or where to write incoming data on a pfnPutData callback but
    //! must not modify it.
    //
    unsigned long ulBlockNum;
} tTFTPConnection;

//*****************************************************************************
//
// Public function prototypes.
//
//*****************************************************************************
void TFTPInit(tTFTPRequest pfnRequest);

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
#endif // _TFTP_H_
