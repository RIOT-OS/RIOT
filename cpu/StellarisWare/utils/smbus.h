//*****************************************************************************
//
// smbus.h - Prototypes for the SMBus driver.
//
// Copyright (c) 2010-2012 Texas Instruments Incorporated.  All rights reserved.
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

#ifndef __SMBUS_H__
#define __SMBUS_H__

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
//! \addtogroup smbus_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! This structure holds the SMBus Unique Device ID (UDID).  For detailed
//! information, please refer to the SMBus Specification.
//
//*****************************************************************************
typedef struct
{
    //
    //! Device capabilities field.  This 8-bit field reports generic SMBus
    //! capabilities such as address type for ARP.
    //
    unsigned char ucDeviceCapabilities;

    //
    //! Version Revision field.  This 8-bit field reports UDID revision
    //! information as well as some vendor-specific things such as silicon
    //! revision.
    //
    unsigned char ucVersion;

    //
    //! Vendor ID.  This 16-bit field contains the manufacturer's ID as
    //! assigned by the SBS Implementers' Forum of the PCI SIG.
    //
    unsigned short usVendorID;

    //
    //! Device ID.  This 16-bit field contains the device ID assigned by the
    //! device manufacturer.
    //
    unsigned short usDeviceID;

    //
    //! Interface.  This 16-bit field identifies the protocol layer interfaces
    //! supported over the SMBus connection.
    //
    unsigned short usInterface;

    //
    //! Subsystem Vendor ID.  This 16-bit field holds additional information
    //! that may be derived from the vendor ID or other information.
    //
    unsigned short usSubSystemVendorID;

    //
    //! Subsystem Device ID.  This 16-bit field holds additional information
    //! that may be derived from the device ID or other information.
    //
    unsigned short usSubSystemDeviceID;

    //
    //! Vendor-specific ID.  This 32-bit field contains a unique number that
    //! can be assigned per device by the manufacturer.
    //
    unsigned long ulVendorSpecificID;
}
tSMBusUDID;

//*****************************************************************************
//
//! This structure contains the state of a single instance of an SMBus module.
//! Master and slave instances require unique configuration structures.
//
//*****************************************************************************
typedef struct
{
    //
    //! The SMBus Unique Device ID (UDID) for this SMBus instance.  If
    //! operating as a host, master-only, or on a bus that does not use Address
    //! Resolution Protocol (ARP), this is not required.  This member can be
    //! set via a  direct structure access or using the SMBusSlaveInit
    //! function.  For detailed information about the UDID, refer to the SMBus
    //! spec.
    //
    tSMBusUDID *pUDID;

    //
    //! The base address of the I2C master peripheral.  This member can be set
    //! via a direct structure access or using the SMBusMasterInit or
    //! SMBusSlaveInit functions.
    //
    unsigned long ulI2CBase;

    //
    //! The address of the data buffer used for transmit operations.  For
    //! master operations, this member is set by the SMBusMasterxxxx functions
    //! that pass a buffer pointer (for example, SMBusMasterBlockWrite).  For
    //! slave operations, this member can be set via direct structure access or
    //! using the SMBusSlaveTxBufferSet function.
    //
    unsigned char *pucTxBuffer;

    //
    //! The address of the data buffer used for receive operations.  For master
    //! operations, this member is set by the SMBusMasterxxxx functions that
    //! pass a buffer pointer (for example, SMBusMasterBlockRead).  For slave
    //! operations, this member can be set via direct structure access or using
    //! the SMBusSlaveRxBufferSet function.
    //
    unsigned char *pucRxBuffer;

    //
    //! The amount of data to transmit from pucTxBuffer.  For master operations
    //! this member is set by the SMBusMasterxxxx functions either via an input
    //! argument (example SMBusMasterByteWordWrite) or explicitly
    //! (example SMBusMasterSendByte).  In master mode, this member should not
    //! be accessed or modified by the application.  For slave operations, this
    //! member can be set via direct structure access of using the
    //! SMBusSlaveTxBufferSet function.
    //
    unsigned char ucTxSize;

    //
    //! The current index in the transmit buffer.  This member should not be
    //! accessed or modified by the application.
    //
    unsigned char ucTxIndex;

    //
    //! The amount of data to receive into pucRxBuffer.  For master operations,
    //! this member is set by the SMBusMasterxxxx functions either via an input
    //! argument (example SMBusMasterByteWordRead), explicitly (example
    //! SMBusMasterReceiveByte), or by the slave (example
    //! SMBusMasterBlockRead).  In master mode, this member should not be
    //! accessed or modified by the application.  For slave operations, this
    //! member can be set via direct structure access of using the
    //! SMBusSlaveRxBufferSet function.
    //
    unsigned char ucRxSize;

    //
    //! The current index in the receive buffer.  This member should not be
    //! accessed or modified by the application.
    //
    unsigned char ucRxIndex;

    //
    //! The active slave address of the I2C peripheral on the Stellaris device.
    //! When using dual address in slave mode, the active address is store
    //! here.  In master mode, this member is not used.  This member is updated
    //! as requests come in from the master.
    //
    unsigned char ucOwnSlaveAddress;

    //
    //! The address of the targeted slave device.  In master mode, this member
    //! is set by the ucTargetSlaveAddress argument in the SMBusMasterxxxx
    //! transfer functions.  In slave mode, it is not used.  This member should
    //! not be modified by the application.
    //
    unsigned char ucTargetSlaveAddress;

    //
    //! The last used command.  In master mode, this member is set by the
    //! ucCommand argument in the SMBusMasterxxxx transfer functions.  In
    //! slave mode, the first received byte will always be considered the
    //! command.  This member should not be modified by the application.
    //
    unsigned char ucCurrentCommand;

    //
    //! The running CRC calculation used for transfers that require Packet Error
    //! Checking (PEC).  This member is updated by the SMBus software and should
    //! not be modified by the application.
    //
    unsigned char ucCalculatedCRC;

    //
    //! The received CRC calculation used for transfers that require Packet
    //! Error Checking (PEC).  This member is updated by the SMBus software and
    //! should not be modified by the application.
    //
    unsigned char ucReceivedCRC;

    //
    //! The current state of the SMBusMasterISRProcess state machine.  This
    //! member should not be accessed or modified by the application.
    //
    unsigned char ucMasterState;

    //
    //! The current state of the SMBusSlaveISRProcess state machine.  This
    //! member should not be accessed or modified by the application.
    //
    unsigned char ucSlaveState;

    //
    //! Flags used for various items in the SMBus state machines for different
    //! transaction types and status.
    //!
    //! FLAG_PEC can be modified via the SMBusPECEnable or SMBusPECDisable
    //! functions or via direct structure access.
    //!
    //! FLAG_BLOCK_TRANSFER can be set via the SMBusSlaveBlockTransferEnable
    //! function and is cleared automatically by the SMBusSlaveTransferInit
    //! function or manually using the SMBusSlaveBlockTransferDisable function.
    //!
    //! FLAG_RAW_I2C can be modified via the SMBusSlaveI2CEnable or
    //! SMBusSlaveI2CDisable functions or via direct structure access.
    //!
    //! FLAG_TRANSFER_IN_PROGRESS should not be modified by the application,
    //! but can be read via the SMBusStatusGet function.
    //!
    //! FLAG_PROCESS_CALL can be set via the SMBusSlaveProcessCallEnable
    //! function and is cleared automatically by the SMBusSlaveTransferInit
    //! function or manually using the SMBusSlaveProcessCallDisable function.
    //!
    //! FLAG_ADDRESS_RESOLVED is only used by an SMBus Slave that supports ARP.
    //! This flag can be modified via the SMBusSlaveARPFlagARSet function and
    //! read via SMBusSlaveARPFlagARGet.  It can also be modified by direct
    //! structure access.
    //!
    //! FLAG_ADDRESS_VALID is only used by an SMBus Slave that supports ARP.
    //! This flag can be modified via the SMBusSlaveARPFlagAVSet function and
    //! read via SMBusSlaveARPFlagAVGet.  It can also be modified by direct
    //! structure access.
    //!
    //! FLAG_ARP is used to indicate that ARP is currently active.  This flag
    //! is not used by the SMBus stack and can (optionally) be used by the
    //! application to keep track of the ARP session.
    //
    unsigned short usFlags;
}
tSMBus;

//*****************************************************************************
//
// ! Return codes.
//
//*****************************************************************************
typedef enum
{
    SMBUS_OK = 0,               // General "OK" return code
    SMBUS_TIMEOUT,              // Master detected bus timeout from slave
    SMBUS_PERIPHERAL_BUSY,      // The I2C peripheral is currently in use
    SMBUS_BUS_BUSY,             // The I2C bus is currently in use
    SMBUS_ARB_LOST,             // Bus arbitration was lost (master mode)
    SMBUS_ADDR_ACK_ERROR,       // In master mode, the address was NAK'd
    SMBUS_DATA_ACK_ERROR,       // Data transfer was NAK'd by receiver
    SMBUS_PEC_ERROR,            // PEC mismatch occurred
    SMBUS_DATA_SIZE_ERROR,      // Data size error has occurred
    SMBUS_MASTER_ERROR,         // Error occurred in the master ISR
    SMBUS_SLAVE_ERROR,          // Error occurred in the slave ISR
    SMBUS_SLAVE_QCMD_0,         // Slave transaction is Quick Command with
                                // data value 0.
    SMBUS_SLAVE_QCMD_1,         // Slave transaction is Quick Command with
                                // data value 1.
    SMBUS_SLAVE_FIRST_BYTE,     // The first byte has been received
    SMBUS_SLAVE_ADDR_PRIMARY,   // Primary address was detected
    SMBUS_SLAVE_ADDR_SECONDARY, // Secondary address was detected
    SMBUS_TRANSFER_IN_PROGRESS, // A transfer is currently in progress
    SMBUS_TRANSFER_COMPLETE,    // The last active transfer is complete
    SMBUS_SLAVE_NOT_READY,        // A slave transmit has been requested, but is
                                // not ready (TX buffer not set).
    SMBUS_FIFO_ERROR,           // A master receive operation did not receive
                                // enough data from the slave.
}
tSMBusStatus;

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// ARP Commands
//
//*****************************************************************************
#define SMBUS_CMD_PREPARE_TO_ARP            0x01
#define SMBUS_CMD_ARP_RESET_DEVICE          0x02
#define SMBUS_CMD_ARP_GET_UDID              0x03
#define SMBUS_CMD_ARP_ASSIGN_ADDRESS        0x04

//*****************************************************************************
//
// Fixed addresses defined by the SMBus specification.
//
//*****************************************************************************
#define SMBUS_ADR_HOST                      0x08
#define SMBUS_ADR_SMART_BATTERY_CHARGER     0x09
#define SMBUS_ADR_SMART_BATTERY_SELECTOR    0x0A
#define SMBUS_ADR_SMART_BATTERY             0x0B
#define SMBUS_ADR_DEFAULT_DEVICE            0x61

//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
extern void SMBusPECEnable(tSMBus *pSMBus);
extern void SMBusPECDisable(tSMBus *pSMBus);
extern void SMBusARPEnable(tSMBus *pSMBus);
extern void SMBusARPDisable(tSMBus *pSMBus);
extern tSMBusStatus SMBusStatusGet(tSMBus *pSMBus);
extern void SMBusARPUDIDPacketEncode(tSMBusUDID *pUDID,
                                     unsigned char ucAddress,
                                     unsigned char *pucData);
extern void SMBusARPUDIDPacketDecode(tSMBusUDID *pUDID,
                                     unsigned char *pucAddress,
                                     unsigned char *pucData);
extern unsigned char SMBusRxPacketSizeGet(tSMBus *pSMBus);
extern void SMBusUDIDDataGet(tSMBus *pSMBus, tSMBusUDID *pUDID);
extern tSMBusStatus SMBusMasterQuickCommand(tSMBus *pSMBus,
                                            unsigned char ucTargetAddress,
                                            tBoolean bData);
extern tSMBusStatus SMBusMasterByteSend(tSMBus *pSMBus,
                                        unsigned char ucTargetAddress,
                                        unsigned char ucData);
extern tSMBusStatus SMBusMasterByteReceive(tSMBus *pSMBus,
                                           unsigned char ucTargetAddress,
                                           unsigned char *pucData);
extern tSMBusStatus SMBusMasterByteWordWrite(tSMBus *pSMBus,
                                             unsigned char ucTargetAddress,
                                             unsigned char ucCommand,
                                             unsigned char *pucData,
                                             unsigned char ucSize);
extern tSMBusStatus SMBusMasterBlockWrite(tSMBus *pSMBus,
                                          unsigned char ucTargetAddress,
                                          unsigned char ucCommand,
                                          unsigned char *pucData,
                                          unsigned char ucSize);
extern tSMBusStatus SMBusMasterByteWordRead(tSMBus *pSMBus,
                                            unsigned char ucTargetAddress,
                                            unsigned char ucCommand,
                                            unsigned char *pucData,
                                            unsigned char ucSize);
extern tSMBusStatus SMBusMasterBlockRead(tSMBus *pSMBus,
                                         unsigned char ucTargetAddress,
                                         unsigned char ucCommand,
                                         unsigned char *pucData);
extern tSMBusStatus SMBusMasterProcessCall(tSMBus *pSMBus,
                                           unsigned char ucTargetAddress,
                                           unsigned char ucCommand,
                                           unsigned char *pucTxData,
                                           unsigned char *pucRxData);
extern tSMBusStatus SMBusMasterBlockProcessCall(tSMBus *pSMBus,
                                                unsigned char ucTargetAddress,
                                                unsigned char ucCommand,
                                                unsigned char *pucTxData,
                                                unsigned char ucTxSize,
                                                unsigned char *pucRxData);
extern tSMBusStatus SMBusMasterHostNotify(tSMBus *pSMBus,
                                          unsigned char ucOwnSlaveAddress,
                                          unsigned char *pucData);
extern tSMBusStatus SMBusMasterI2CWrite(tSMBus *pSMBus,
                                        unsigned char ucTargetAddress,
                                        unsigned char *pucData,
                                        unsigned char ucSize);
extern tSMBusStatus SMBusMasterI2CRead(tSMBus *pSMBus,
                                       unsigned char ucTargetAddress,
                                       unsigned char *pucData,
                                       unsigned char ucSize);
extern tSMBusStatus SMBusMasterI2CWriteRead(tSMBus *pSMBus,
                                            unsigned char ucTargetAddress,
                                            unsigned char *pucTxData,
                                            unsigned char ucTxSize,
                                            unsigned char *pucRxData,
                                            unsigned char ucRxSize);
extern tSMBusStatus SMBusMasterARPGetUDIDGen(tSMBus *pSMBus,
                                             unsigned char *pucData);
extern tSMBusStatus SMBusMasterARPGetUDIDDir(tSMBus *pSMBus,
                                             unsigned char ucTargetAddress,
                                             unsigned char *pucData);
extern tSMBusStatus SMBusMasterARPResetDeviceGen(tSMBus *pSMBus);
extern tSMBusStatus SMBusMasterARPResetDeviceDir(tSMBus *pSMBus,
                                                 unsigned char ucTargetAddress);
extern tSMBusStatus SMBusMasterARPAssignAddress(tSMBus *pSMBus,
                                                unsigned char *pucData);
extern tSMBusStatus SMBusMasterARPNotifyMaster(tSMBus *pSMBus,
                                               unsigned char *pucData);
extern tSMBusStatus SMBusMasterARPPrepareToARP(tSMBus *pSMBus);
extern tSMBusStatus SMBusMasterIntProcess(tSMBus *pSMBus);
extern void SMBusMasterIntEnable(tSMBus *pSMBus);
extern void SMBusMasterInit(tSMBus *pSMBus, unsigned long ulI2CBase,
                            unsigned long ulSMBusClock);
extern void SMBusSlaveTxBufferSet(tSMBus *pSMBus, unsigned char *pucData,
                                  unsigned char ucSize);
extern void SMBusSlaveRxBufferSet(tSMBus *pSMBus, unsigned char *pucData,
                                  unsigned char ucSize);
extern unsigned char SMBusSlaveCommandGet(tSMBus *pSMBus);
extern void SMBusSlaveProcessCallEnable(tSMBus *pSMBus);
extern void SMBusSlaveProcessCallDisable(tSMBus *pSMBus);
extern void SMBusSlaveBlockTransferEnable(tSMBus *pSMBus);
extern void SMBusSlaveBlockTransferDisable(tSMBus *pSMBus);
extern void SMBusSlaveI2CEnable(tSMBus *pSMBus);
extern void SMBusSlaveI2CDisable(tSMBus *pSMBus);
extern void SMBusSlaveARPFlagARSet(tSMBus *pSMBus, tBoolean bValue);
extern tBoolean SMBusSlaveARPFlagARGet(tSMBus *pSMBus);
extern void SMBusSlaveARPFlagAVSet(tSMBus *pSMBus, tBoolean bValue);
extern tBoolean SMBusSlaveARPFlagAVGet(tSMBus *pSMBus);
extern void SMBusSlaveTransferInit(tSMBus *pSMBus);
extern tSMBusStatus SMBusSlaveIntProcess(tSMBus *pSMBus);
extern tSMBusStatus SMBusSlaveDataSend(tSMBus *pSMBus);
extern void SMBusSlaveACKSend(tSMBus *pSMBus, tBoolean bACK);
extern void SMBusSlaveManualACKEnable(tSMBus *pSMBus);
extern void SMBusSlaveManualACKDisable(tSMBus *pSMBus);
extern tBoolean SMBusSlaveManualACKStatusGet(tSMBus *pSMBus);
extern tSMBusStatus SMBusSlaveIntAddressGet(tSMBus *pSMBus);
extern void SMBusSlaveIntEnable(tSMBus *pSMBus);
extern void SMBusSlaveUDIDSet(tSMBus *pSMBus, tSMBusUDID *pUDID);
extern void SMBusSlaveAddressSet(tSMBus *pSMBus, unsigned char ucAddressNum,
                                 unsigned char ucSlaveAddress);
extern void SMBusSlaveInit(tSMBus *pSMBus, unsigned long ulI2CBase);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __SMBUS_H__
