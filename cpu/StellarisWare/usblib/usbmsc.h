//*****************************************************************************
//
// usbmsc.h - Generic types and defines use by the mass storage class.
//
// Copyright (c) 2008-2012 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 9453 of the Stellaris USB Library.
//
//*****************************************************************************

#ifndef __USBMSC_H__
#define __USBMSC_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// The request for the maximum number of logical units on a mass storage
// device.
//
//*****************************************************************************
#define USBREQ_GET_MAX_LUN      0xfe

//*****************************************************************************
//
// The signatures defined by USB MSC class specification.
//
//*****************************************************************************
#define CBW_SIGNATURE           0x43425355
#define CSW_SIGNATURE           0x53425355

//*****************************************************************************
//
// Flag for the bmCBWFlags member of tMSCCBW
//
//*****************************************************************************
#define CBWFLAGS_DIR_M          0x80
#define CBWFLAGS_DIR_IN         0x80
#define CBWFLAGS_DIR_OUT        0x00

//*****************************************************************************
//
// All structures defined in this section of the header require byte packing of
// fields.  This is usually accomplished using the PACKED macro but, for IAR
// Embedded Workbench, this requries a pragma.
//
//*****************************************************************************
#ifdef ewarm
#pragma pack(1)
#endif

//*****************************************************************************
//
// The following packed structure is used to access the Command Block Wrapper
// (CBW) data structure that is used when communicating with USB Mass Storage
// Class devices.
//
//*****************************************************************************
typedef struct
{
    //
    // Signature that helps identify this data packet as a CBW.  The signature
    // field shall contain the value 0x43425355 (little endian), indicating a
    // CBW.
    //
    unsigned long dCBWSignature;

    //
    // The Command Block Tag sent by the host controller.  The device shall
    // echo the contents of this field back to the host in the dCSWTag field
    // of the associated CSW.  The dCSWTag positively associates a CSW with the
    // corresponding CBW.
    //
    unsigned long dCBWTag;

    //
    // The number of bytes of data that the host expects to transfer on the
    // Bulk-In or Bulk-Out endpoint (as indicated by the Direction bit) during
    // the execution of this command.  If this field is zero, the device and
    // the host will not transfer data between the CBW and the associated CSW,
    // and the device will ignore the value of the Direction bit in
    // bmCBWFlags.
    //
    unsigned long dCBWDataTransferLength;

    //
    // The device will ignore these bits if the dCBWDataTransferLength value
    // is set to 0.
    //
    // The bits of this field are defined as follows:
    // Bit 7 Direction
    //   0 = Data-Out from host to the device,
    //   1 = Data-In from the device to the host.
    // Bit 6 Obsolete - The host shall set this bit to zero.
    // Bits 5..0 Reserved - the host shall set these bits to zero.
    //
    unsigned char bmCBWFlags;

    //
    // The device Logical Unit Number (LUN) to which the command block is being
    // sent.  For devices that support multiple LUNs, the host shall place into
    // this field the LUN to which this command block is addressed.  Otherwise,
    // the host shall set this field to zero.
    //
    unsigned char bCBWLUN;

    //
    // The valid length of the CBWCB in bytes.  This defines the valid length
    // of the command block.  The only legal values are 1 through 16.  All
    // other values are reserved.
    //
    unsigned char bCBWCBLength;

    //
    // This array holds the command block to be executed by the device.  The
    // MSC device will interpret the first bCBWCBLength bytes in this field as
    // a command block as defined by the command set identified by
    // bInterfaceSubClass.  If the command set supported by the device uses
    // command blocks of fewer than 16 bytes in length, the significant bytes
    // shall be transferred first, beginning with the byte at offset 15.  The
    // device will ignore the content of the CBWCB field past the byte at
    // offset (15 + bCBWCBLength - 1).
    //
    unsigned char CBWCB[16];
}
PACKED tMSCCBW;

//*****************************************************************************
//
// Flags for the bCSWStatus member of tMSCCSW
//
//*****************************************************************************
#define CSWSTATUS_CMD_SUCCESS   0
#define CSWSTATUS_CMD_FAILED    1
#define CSWSTATUS_PHASE_ERROR   2

//*****************************************************************************
//
// This structure encapsulates the Command Status Word (CSW) structure that is
// sent in response to all CBW commands.
//
//*****************************************************************************
typedef struct
{
    //
    // Signature that identifies this data packet as a CSW.  The signature
    // field must contain the value 53425355h (little endian) to indicate CSW.
    //
    unsigned long dCSWSignature;

    //
    // The device will set this field to the value received in the dCBWTag of
    // the associated CBW.
    //
    unsigned long dCSWTag;

    //
    // For OUT transactions the device will fill the dCSWDataResidue field with
    // the difference between the amount of data expected as stated in the
    // dCBWDataTransferLength, and the actual amount of data processed by the
    // device.  For IN transactions the device will fill the dCSWDataResidue
    // field with the difference between the amount of data expected as stated
    // in the dCBWDataTransferLength and the actual amount of relevant data
    // sent by the device.  The dCSWDataResidue will not exceed the value sent
    // in the dCBWDataTransferLength.
    //
    unsigned long dCSWDataResidue;

    //
    // The bCSWStatus field indicates the success or failure of the command.
    // The device shall set this byte to zero if the command completed
    // successfully.  A non-zero value shall indicate a failure during command
    // execution.
    //
    unsigned char bCSWStatus;
}
PACKED tMSCCSW;

//*****************************************************************************
//
// Return to default packing when using the IAR Embedded Workbench compiler.
//
//*****************************************************************************
#ifdef ewarm
#pragma pack()
#endif

//*****************************************************************************
//
// SCSI Command return codes.
//
//*****************************************************************************
#define SCSI_CMD_STATUS_PASS        0x00
#define SCSI_CMD_STATUS_FAIL        0x01

//*****************************************************************************
//
// SCSI commands.
//
//*****************************************************************************
#define SCSI_TEST_UNIT_READY        0x00
#define SCSI_REQUEST_SENSE          0x03
#define SCSI_INQUIRY_CMD            0x12
#define SCSI_MODE_SENSE_6           0x1a
#define SCSI_READ_CAPACITIES        0x23
#define SCSI_READ_CAPACITY          0x25
#define SCSI_READ_10                0x28
#define SCSI_WRITE_10               0x2a

//*****************************************************************************
//
// SCSI Test Unit Ready definitions.
//
//*****************************************************************************

//*****************************************************************************
//
// SCSI Inquiry command definitions.
//
//*****************************************************************************

//*****************************************************************************
//
// Size of the SCSI inquiry response data.
//
//*****************************************************************************
#define SCSI_INQUIRY_DATA_SZ    36

//*****************************************************************************
//
// Offset 0 of the Inquiry Data.
//
//*****************************************************************************
#define SCSI_INQ_PQ_M           0xe0  // Peripheral Qualifier Mask.
#define SCSI_INQ_PQ_CNCT        0x00  // Device connected.
#define SCSI_INQ_PQ_DISC        0x20  // Device disconnected.
#define SCSI_INQ_PDT_M          0x1f  // Peripheral Device Type Mask.
#define SCSI_INQ_PDT_SBC        0x00  // Direct Access device.

//*****************************************************************************
//
// Offset 1 of the Inquiry Data.
//
//*****************************************************************************
#define SCSI_INQ_RMB            0x80  // Device is removable.

//*****************************************************************************
//
// Macro to check if removeable.
//
//*****************************************************************************
#define SCSIIsRemovable(pData)                       \
        (((unsigned char *)pData)[1] & SCSI_INQ_RMB)

//*****************************************************************************
//
// SCSI Read Capacity definitions.
//
//*****************************************************************************

//*****************************************************************************
//
// Size of the SCSI Read Capacity response data.
//
//*****************************************************************************
#define SCSI_READ_CAPACITY_SZ   0x08

//*****************************************************************************
//
// SCSI Mode Sense definitions, these are passed in via the ulFlags parameter
// of the SCSIModeSense() function call.
//
//*****************************************************************************

//*****************************************************************************
//
// Disable block descriptors.
//
//*****************************************************************************
#define SCSI_MS_DBD             0x00000800

//*****************************************************************************
//
// Page Code values, used in combination with Page Control values.
//
//*****************************************************************************
#define SCSI_MS_PC_VENDOR       0x00000000
#define SCSI_MS_PC_DISCO        0x00020000
#define SCSI_MS_PC_CONTROL      0x000a0000
#define SCSI_MS_PC_LUN          0x00180000
#define SCSI_MS_PC_PORT         0x00190000
#define SCSI_MS_PC_POWER        0x001a0000
#define SCSI_MS_PC_INFORM       0x001c0000
#define SCSI_MS_PC_ALL          0x003f0000

//*****************************************************************************
//
// Page Control values.
//
//*****************************************************************************
#define SCSI_MS_PC_CURRENT      0x00000000
#define SCSI_MS_PC_CHANGEABLE   0x00400000
#define SCSI_MS_PC_DEFAULT      0x00800000
#define SCSI_MS_PC_SAVED        0x00c00000

//*****************************************************************************
//
// Request Sense Definitions.
//
//*****************************************************************************

//*****************************************************************************
//
// Size of the data returned by the Request Sense command.
//
//*****************************************************************************
#define SCSI_REQUEST_SENSE_SZ   18

#define SCSI_RS_SKEY            2     // Sense Key offset.
#define SCSI_RS_SKEY_AD_SKEY    12    // Additional Sense Key offset.

//*****************************************************************************
//
// Offset 0 in the Request Sense response.
//
//*****************************************************************************
#define SCSI_RS_VALID           0x80  // Response is valid.
#define SCSI_RS_CUR_ERRORS      0x70  // Current errors returned.
#define SCSI_RS_DEFER_ERRORS    0x71  // Deferred errors returned.

//*****************************************************************************
//
// Offset 2 in the Request Sense response.
//
//*****************************************************************************
#define SCSI_RS_KEY_M           0x0f  // Sense Key.
#define SCSI_RS_KEY_NO_SENSE    0x00  // No Sense Data.
#define SCSI_RS_KEY_RECOVRD_ERR 0x01  // Recovered Error.
#define SCSI_RS_KEY_NOT_READY   0x02  // Not Ready.
#define SCSI_RS_KEY_MEDIUM_ERR  0x03  // Error in the media.
#define SCSI_RS_KEY_HW_ERR      0x04  // Hardware Error, non recoverable.
#define SCSI_RS_KEY_ILGL_RQST   0x05  // Illegal request.
#define SCSI_RS_KEY_UNIT_ATTN   0x06  // Unit changed or reset.
#define SCSI_RS_KEY_DATA_PROT   0x07  // Write Protect error.
#define SCSI_RS_KEY_BLANK_CHK   0x08  // Write once error, block not clear.
#define SCSI_RS_KEY_ABORT       0x0b  // Last command was aborted.
#define SCSI_RS_ILI             0x20  // Incorrect length indicator.
#define SCSI_RS_EOM             0x40  // End of medium condition.
#define SCSI_RS_FILEMARK        0x80  // Command has read a filemark/setmark.
#define SCSI_RS_MED_NOT_PRSNT   0x003a  // Medium not present.
#define SCSI_RS_MED_NOTRDY2RDY  0x0028  // Not ready to ready transition.
#define SCSI_RS_PV_INVALID      0x0226  // Parameter Value Invalid.

//*****************************************************************************
//
// Additional information for SCSI_RS_KEY_NOT_READY
//
//*****************************************************************************
#define SCSI_RS_KEY_NOTPRSNT    0x3A  // Media Not Present.

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __USBMSC_H__
