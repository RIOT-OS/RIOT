//*****************************************************************************
//
// smbus.c - SMBus protocol layer API.
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

#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/udma.h"
#include "utils/crc.h"
#include "utils/smbus.h"

//*****************************************************************************
//
//! \addtogroup smbus_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// The states for the master and slave interrupt handler state machines.
//
//*****************************************************************************
#define SMBUS_STATE_IDLE                0
#define SMBUS_STATE_SLAVE_POST_COMMAND  1
#define SMBUS_STATE_WRITE_BLOCK_SIZE    2
#define SMBUS_STATE_WRITE_NEXT          3
#define SMBUS_STATE_WRITE_FINAL         4
#define SMBUS_STATE_WRITE_DONE          5
#define SMBUS_STATE_READ_ONE            6
#define SMBUS_STATE_READ_FIRST          7
#define SMBUS_STATE_READ_BLOCK_SIZE     8
#define SMBUS_STATE_READ_NEXT           9
#define SMBUS_STATE_READ_FINAL          10
#define SMBUS_STATE_READ_WAIT           11
#define SMBUS_STATE_READ_PEC            12
#define SMBUS_STATE_READ_DONE           13
#define SMBUS_STATE_READ_ERROR_STOP     14

//*****************************************************************************
//
// Status flags for various instance-specific tasks.
//
//*****************************************************************************
#define FLAG_PEC                        0
#define FLAG_PROCESS_CALL               1
#define FLAG_BLOCK_TRANSFER             2
#define FLAG_TRANSFER_IN_PROGRESS       3
#define FLAG_RAW_I2C                    4
#define FLAG_ADDRESS_RESOLVED           5
#define FLAG_ADDRESS_VALID              6
#define FLAG_ARP                        7
//*****************************************************************************
//
//! Enables Packet Error Checking (PEC).
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function enables the transmission and checking of a PEC byte in SMBus
//! transactions.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusPECEnable(tSMBus *pSMBus)
{
    //
    // Set the PEC flag in the configuration structure.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_PEC) = 1;
}

//*****************************************************************************
//
//! Disables Packet Error Checking (PEC).
//!
//!    \param pSMBus specifies the SMBus configuration structure.
//!
//! This function disables the transmission and checking of a PEC byte in SMBus
//! transactions.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusPECDisable(tSMBus *pSMBus)
{
    //
    // Clear the PEC flag in the configuration structure.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_PEC) = 0;
}

//*****************************************************************************
//
//! Sets the ARP flag in the configuration structure.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function sets the Address Resolution Protocol (ARP) flag in the
//! configuration structure.  This flag can be used to track the state of a
//! device during the ARP process.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusARPEnable(tSMBus *pSMBus)
{
    //
    // Set the ARP flag in the configuration structure.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_ARP) = 1;
}

//*****************************************************************************
//
//! Clears the ARP flag in the configuration structure.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function clears the Address Resolution Protocol (ARP) flag in the
//! configuration structure.  This flag can be used to track the state of a
//! device during the ARP process.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusARPDisable(tSMBus *pSMBus)
{
    //
    // Clear the ARP flag in the configuration structure.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_ARP) = 0;
}

//*****************************************************************************
//
//! Returns the number of bytes in the receive buffer.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function returns the number of bytes in the active receive buffer.
//! It can be used to determine how many bytes have been received in the slave
//! receive or master block read configurations.
//!
//! \return Number of bytes in the buffer.
//
//*****************************************************************************
unsigned char
SMBusRxPacketSizeGet(tSMBus *pSMBus)
{
    //
    // Return the number of bytes received.
    //
    return(pSMBus->ucRxIndex);
}

//*****************************************************************************
//
//! Returns the state of an SMBus transfer.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function returns the status of an SMBus transaction.  It can be used
//! to determine whether a transfer is ongoing or complete.
//!
//! \return Returns \b SMBUS_TRANSFER_IN_PROGRESS if transfer is ongoing, or
//! \b SMBUS_TRANSFER_COMPLETE if transfer has completed.
//
//*****************************************************************************
tSMBusStatus
SMBusStatusGet(tSMBus *pSMBus)
{
    //
    // Check to see if there is an ongoing transfer.
    //
    if(HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS))
    {
        //
        // If the flag is set, return in progress status.
        //
        return(SMBUS_TRANSFER_IN_PROGRESS);
    }

    //
    // If the transfer complete flag is cleared, transfer is done.
    //
    else
    {
        //
        // If the flag isn't set, return complete status.
        //
        return(SMBUS_TRANSFER_COMPLETE);
    }
}

//*****************************************************************************
//
//! Encodes a UDID structure and address into SMBus-transferable byte order.
//!
//! \param pUDID specifies the structure to encode.
//! \param ucAddress specifies the address to send with the UDID (byte 17).
//! \param pucData specifies the location of the destination data buffer.
//!
//! This function takes a tSMBusUDID structure and re-orders the bytes so that
//! it can be transferred on the bus.  The destination data buffer must contain
//! at least 17 bytes.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusARPUDIDPacketEncode(tSMBusUDID *pUDID, unsigned char ucAddress,
                         unsigned char *pucData)
{
    //
    // Place data from the UDID structure and address into the data buffer
    // using the correct MSB->LSB + address order.
    //
    pucData[0] = pUDID->ucDeviceCapabilities;
    pucData[1] = pUDID->ucVersion;
    pucData[2] = (unsigned char)((pUDID->usVendorID & 0xff00) >> 8);
    pucData[3] = (unsigned char)(pUDID->usVendorID & 0x00ff);
    pucData[4] = (unsigned char)((pUDID->usDeviceID & 0xff00) >> 8);
    pucData[5] = (unsigned char)(pUDID->usDeviceID & 0x00ff);
    pucData[6] = (unsigned char)((pUDID->usInterface & 0xff00) >> 8);
    pucData[7] = (unsigned char)(pUDID->usInterface & 0x00ff);
    pucData[8] = (unsigned char)((pUDID->usSubSystemVendorID & 0xff00) >> 8);
    pucData[9] = (unsigned char)(pUDID->usSubSystemVendorID & 0x00ff);
    pucData[10] = (unsigned char)((pUDID->usSubSystemDeviceID & 0xff00) >> 8);
    pucData[11] = (unsigned char)(pUDID->usSubSystemDeviceID & 0x00ff);
    pucData[12] = (unsigned char)((pUDID->ulVendorSpecificID & 0xff000000) >>
                                  24);
    pucData[13] = (unsigned char)((pUDID->ulVendorSpecificID & 0x00ff0000) >>
                                  16);
    pucData[14] = (unsigned char)((pUDID->ulVendorSpecificID & 0x0000ff00) >>
                                  8);
    pucData[15] = (unsigned char)(pUDID->ulVendorSpecificID & 0x000000ff);
    pucData[16] = ucAddress;
}

//*****************************************************************************
//
//! Decodes an SMBus packet into a UDID structure and address.
//!
//! \param pUDID specifies the structure that is updated with new data.
//! \param pucAddress specifies the location of the variable that holds the
//! the address sent with the UDID (byte 17).
//! \param pucData specifies the location of the source data.
//!
//! This function takes a data buffer and decodes it into a tSMBusUDID
//! structure and an address variable.  It is assumed that there are 17 bytes
//! in the data buffer.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusARPUDIDPacketDecode(tSMBusUDID *pUDID, unsigned char *pucAddress,
                         unsigned char *pucData)
{
    //
    // Populate the UDID structure with data from the input data buffer.
    //
    pUDID->ucDeviceCapabilities = pucData[0];
    pUDID->ucVersion = pucData[1];
    pUDID->usVendorID = (unsigned short)((pucData[2] << 8) | pucData[3]);
    pUDID->usDeviceID = (unsigned short)((pucData[4] << 8) | pucData[5]);
    pUDID->usInterface = (unsigned short)((pucData[6] << 8) | pucData[7]);
    pUDID->usSubSystemVendorID = (unsigned short)((pucData[8] << 8) |
                                                  pucData[9]);
    pUDID->usSubSystemDeviceID = (unsigned short)((pucData[10] << 8) |
                                                  pucData[11]);
    pUDID->ulVendorSpecificID = (unsigned long)((pucData[12] << 24) |
                                                (pucData[13] << 16) |
                                                (pucData[14] << 8) |
                                                (pucData[15]));

    //
    // Populate the address.
    //
    *pucAddress = pucData[16];
}

//*****************************************************************************
//
//! Initiates a master Quick Command transfer to an SMBus slave.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucTargetAddress specifies the slave address of the target device.
//! \param bData is the value of the single data bit sent to the slave.
//!
//! Quick Command is an SMBus protocol that sends a single data bit using the
//! I2C R/S bit.  This function issues a single I2C transfer with the slave
//! address and data bit.
//!
//! This protocol does not support PEC.  The PEC flag is explicitly cleared
//! within this function, so if PEC is enabled prior to calling it, it must
//! be re-enabled afterwards.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterQuickCommand(tSMBus *pSMBus, unsigned char ucTargetAddress,
                        tBoolean bData)
{
    //
    // Make sure that the peripheral is not currently active.
    //
    if(MAP_I2CMasterBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_PERIPHERAL_BUSY);
    }

    //
    // Update the configuration structure with the data for this transfer.
    //
    pSMBus->ucTargetSlaveAddress = ucTargetAddress;
    pSMBus->ucTxSize = 0;
    pSMBus->ucRxSize = 0;
    pSMBus->ucRxIndex = 0;
    pSMBus->ucCalculatedCRC = 0;

    //
    // Clear the block transfer, process call and raw I2C flags.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 0;

    //
    // This protocol does NOT support PEC, so the flag must be cleared.  If
    // PEC is needed again after this transaction, it should be explicitly
    // enabled again.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_PEC) = 0;

    //
    // Initialize the buffer index to 0 and the interrupt state machine to
    // the appropriate state so that there is a known starting point
    // for each transaction.
    //
    pSMBus->ucTxIndex = 0;
    pSMBus->ucMasterState = SMBUS_STATE_IDLE;

    //
    // Set the slave address and R/S bit.
    //
    MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase, pSMBus->ucTargetSlaveAddress,
                              bData);

    //
    // Make sure that the bus is idle.
    //
    if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_BUS_BUSY);
    }

    //
    // Initiate the write operation.
    //
    MAP_I2CMasterControl(pSMBus->ulI2CBase, I2C_MASTER_CMD_QUICK_COMMAND);

    //
    // Set the transfer in progress flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 1;

    //
    // Return to the caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Initiates a master Host Notify transfer to the SMBus Host.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucOwnSlaveAddress specifies the peripheral's own slave address.
//! \param pucData is a pointer to the two byte data payload.
//!
//! The Host Notify protocol is used by SMBus slaves to alert the bus Host
//! about an event.  Most slave devices that operate in this environment only
//! become a bus master when this packet type is used.  Host Notify always
//! sends two data bytes to the host along with the peripheral's own slave
//! address so that the Host knows which peripheral requested the Host's
//! attention.
//!
//! This protocol does not support PEC.  The PEC flag is explicitly cleared
//! within this function, so if PEC is enabled prior to calling it, it must
//! be re-enabled afterwards.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterHostNotify(tSMBus *pSMBus, unsigned char ucOwnSlaveAddress,
                      unsigned char *pucData)
{
    //
    // Make sure that the peripheral is not currently active.
    //
    if(MAP_I2CMasterBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_PERIPHERAL_BUSY);
    }

    //
    // Update the configuration structure with the data for this transfer.
    //
    pSMBus->ucTargetSlaveAddress = SMBUS_ADR_HOST;
    pSMBus->pucTxBuffer = pucData;
    pSMBus->ucTxSize = 2;
    pSMBus->ucRxSize = 0;
    pSMBus->ucRxIndex = 0;
    pSMBus->ucCalculatedCRC = 0;

    //
    // Clear the block transfer, process call and raw I2C flags.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 0;

    //
    // This protocol does NOT support PEC, so the flag must be cleared.  If
    // PEC is needed again after this transaction, it should be explicitly
    // enabled again.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_PEC) = 0;

    //
    // Initialize the buffer index to 0 and the interrupt state machine to
    // the appropriate state so that there is a known starting point
    // for each transaction.
    //
    pSMBus->ucTxIndex = 0;
    pSMBus->ucMasterState = SMBUS_STATE_WRITE_NEXT;

    //
    // Set the slave address and R/S bit.
    //
    MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase, pSMBus->ucTargetSlaveAddress,
                              false);

    //
    // Put the SMBus command code on the bus.
    //
    MAP_I2CMasterDataPut(pSMBus->ulI2CBase, ucOwnSlaveAddress);

    //
    // Make sure that the bus is idle.
    //
    if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_BUS_BUSY);
    }

    //
    // Initiate the write operation.
    //
    MAP_I2CMasterControl(pSMBus->ulI2CBase, I2C_MASTER_CMD_BURST_SEND_START);

    //
    // Set the transfer in progress flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 1;

    //
    // Return to the caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Initiates a master Send Byte transfer to an SMBus slave.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucTargetAddress specifies the slave address of the target device.
//! \param ucData is the data byte to send to the slave.
//!
//! The Send Byte protocol is a basic SMBus protocol that sends a single data
//! byte to the slave.  Unlike most of the other SMBus protocols, Send Byte
//! does not send a ``command'' byte before the data payload and is intended
//! for basic communication.
//!
//! This protocol supports the optional PEC byte for error checking.  To use
//! PEC, SMBusPECEnable() must be called before this function.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterByteSend(tSMBus *pSMBus, unsigned char ucTargetAddress,
                    unsigned char ucData)
{
    unsigned char ucTempData;

    //
    // Make sure that the peripheral is not currently active.
    //
    if(MAP_I2CMasterBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_PERIPHERAL_BUSY);
    }

    //
    // Update the configuration structure with the data for this transfer.
    //
    pSMBus->ucTargetSlaveAddress = ucTargetAddress;
    pSMBus->ucCurrentCommand = ucData;
    pSMBus->pucTxBuffer = &ucData;
    pSMBus->ucTxSize = 0;
    pSMBus->ucTxIndex = 0;
    pSMBus->ucRxSize = 0;
    pSMBus->ucRxIndex = 0;
    pSMBus->ucCalculatedCRC = 0;

    //
    // Clear the block transfer, process call and raw I2C flags.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 0;

    //
    // Set the slave address and R/S bit.
    //
    MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase, pSMBus->ucTargetSlaveAddress,
                              false);

    //
    // Put the data byte on the bus.
    //
    MAP_I2CMasterDataPut(pSMBus->ulI2CBase, ucData);

    //
    // Calculate the CRC for PEC (if used).
    //
    if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
    {
        //
        // Place the target slave address into a temporary data variable and
        // make sure the R/S bit is set to '0' for the CRC calculation.
        //
        ucTempData = (pSMBus->ucTargetSlaveAddress << 1) &  0xfe;

        //
        // Start off by calculating the CRC of the target slave address with
        // an initial value of 0.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(0, &ucTempData, 1);

        //
        // Add the data to the running CRC calculation.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                &pSMBus->pucTxBuffer[0], 1);

        //
        // Update the state machine.
        //
        pSMBus->ucMasterState = SMBUS_STATE_WRITE_FINAL;

        //
        // Make sure that the bus is idle.
        //
        if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
        {
            return(SMBUS_BUS_BUSY);
        }

        //
        // Initiate the write operation.
        //
        MAP_I2CMasterControl(pSMBus->ulI2CBase,
                             I2C_MASTER_CMD_BURST_SEND_START);
    }
    else
    {
        //
        // Update the state machine.  Since it's the only byte being sent,
        // the state machine's next state is idle.
        //
        pSMBus->ucMasterState = SMBUS_STATE_IDLE;

        //
        // Make sure that the bus is idle.
        //
        if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
        {
            return(SMBUS_BUS_BUSY);
        }

        //
        // Initiate the write operation.
        //
        MAP_I2CMasterControl(pSMBus->ulI2CBase, I2C_MASTER_CMD_SINGLE_SEND);
    }

    //
    // Set the transfer in progress flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 1;

    //
    // Return to the caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Initiates a master Receive Byte transfer to an SMBus slave.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucTargetAddress specifies the slave address of the target device.
//! \param pucData is a pointer to the location to store the received data
//! byte.
//!
//! The Receive Byte protocol is a basic SMBus protocol that receives a single
//! data byte from the slave.  Unlike most of the other SMBus protocols,
//! Receive Byte does not send a ``command'' byte before the data payload and
//! is intended for basic communication.
//!
//! This protocol supports the optional PEC byte for error checking.  To use
//! PEC, SMBusPECEnable() must be called before this function.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterByteReceive(tSMBus *pSMBus, unsigned char ucTargetAddress,
                       unsigned char *pucData)
{
    unsigned char ucTempData;

    //
    // Make sure that the peripheral is not currently active.
    //
    if(MAP_I2CMasterBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_PERIPHERAL_BUSY);
    }

    //
    // Update the configuration structure with the data for this transfer.
    //
    pSMBus->ucTargetSlaveAddress = ucTargetAddress;
    pSMBus->ucTxSize = 0;
    pSMBus->ucTxIndex = 0;
    pSMBus->pucRxBuffer = pucData;
    pSMBus->ucRxSize = 1;
    pSMBus->ucCalculatedCRC = 0;

    //
    // Clear the block transfer, process call and raw I2C flags.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 0;

    //
    // Set the slave address and R/S bit.
    //
    MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase, pSMBus->ucTargetSlaveAddress,
                              true);

    //
    // Calculate the CRC for PEC (if used).
    //
    if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
    {
        //
        // Place the target slave address into a temporary data variable and
        // set the R/S bit to '1' for the CRC calculation.
        //
        ucTempData = ((pSMBus->ucTargetSlaveAddress << 1) & 0xfe) | 1;

        //
        // Start off by calculating the CRC of the target slave address with
        // an initial value of 0.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(0, &ucTempData, 1);

        //
        // Update the state machine.
        //
        pSMBus->ucMasterState = SMBUS_STATE_READ_FINAL;

        //
        // Make sure that the bus is idle.
        //
        if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
        {
            return(SMBUS_BUS_BUSY);
        }

        //
        // Initiate the read operation.
        //
        MAP_I2CMasterControl(pSMBus->ulI2CBase,
                             I2C_MASTER_CMD_BURST_RECEIVE_START);
    }
    else
    {
        //
        // Update the state machine.
        //
        pSMBus->ucMasterState = SMBUS_STATE_READ_WAIT;

        //
        // Make sure that the bus is idle.
        //
        if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
        {
            return(SMBUS_BUS_BUSY);
        }

        //
        // Initiate the read operation.
        //
        MAP_I2CMasterControl(pSMBus->ulI2CBase, I2C_MASTER_CMD_SINGLE_RECEIVE);
    }

    //
    // Set the transfer in progress flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 1;

    //
    // Return to the caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Initiates a master Write Byte or Write Word transfer to an SMBus slave.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucTargetAddress specifies the slave address of the target device.
//! \param ucCommand is the command byte sent before the data payload.
//! \param pucData is a pointer to the transmit data buffer.
//! \param ucSize is the number of bytes to send to the slave.
//!
//! This function supports both the Write Byte and Write Word protocols.  The
//! amount of data to send is user defined, but limited to 1 or 2 bytes.
//!
//! This protocol supports the optional PEC byte for error checking.  To use
//! PEC, SMBusPECEnable() must be called before this function.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use,
//! \b SMBUS_DATA_SIZE_ERROR if ucSize is greater than 2, or \b SMBUS_OK if the
//! transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterByteWordWrite(tSMBus *pSMBus, unsigned char ucTargetAddress,
                         unsigned char ucCommand, unsigned char *pucData,
                         unsigned char ucSize)
{
    unsigned char ucTempData;

    //
    // Make sure that the peripheral is not currently active.
    //
    if(MAP_I2CMasterBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_PERIPHERAL_BUSY);
    }

    //
    // If more than 2 bytes are requested, indicate error.
    //
    if(ucSize > 2)
    {
        return(SMBUS_DATA_SIZE_ERROR);
    }

    //
    // Update the configuration structure with the data for this transfer.
    //
    pSMBus->ucTargetSlaveAddress = ucTargetAddress;
    pSMBus->ucCurrentCommand = ucCommand;
    pSMBus->pucTxBuffer = pucData;
    pSMBus->ucTxSize = ucSize;
    pSMBus->ucRxSize = 0;
    pSMBus->ucRxIndex = 0;
    pSMBus->ucCalculatedCRC = 0;

    //
    // Clear the block transfer, process call and raw I2C flags.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 0;

    //
    // Initialize the buffer index to 0 and the interrupt state machine to
    // the appropriate state so that there is a known starting point
    // for each transaction.
    //
    pSMBus->ucTxIndex = 0;

    //
    // Set the slave address and R/S bit.
    //
    MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase, pSMBus->ucTargetSlaveAddress,
                              false);

    //
    // Calculate the CRC for PEC (if used).
    //
    if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
    {
        //
        // Place the target slave address into a temporary data variable and
        // make sure the R/S bit is set to '0' for the CRC calculation.
        //
        ucTempData = (pSMBus->ucTargetSlaveAddress << 1) &  0xfe;

        //
        // Start off by calculating the CRC of the target slave address with
        // an initial value of 0.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(0, &ucTempData, 1);

        //
        // Add the command to the running CRC calculation.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                &pSMBus->ucCurrentCommand, 1);

        //
        // Add the data array to the calculation.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                pSMBus->pucTxBuffer,
                                                pSMBus->ucTxSize);

        //
        // Set the next state.
        //
        pSMBus->ucMasterState = SMBUS_STATE_WRITE_NEXT;
    }
    else
    {
        //
        // If only one byte to send, move to the final state.
        //
        if(ucSize == 1)
        {
            //
            // Set the next state.
            //
            pSMBus->ucMasterState = SMBUS_STATE_WRITE_FINAL;
        }
        else
        {
            //
            // Set the next state.
            //
            pSMBus->ucMasterState = SMBUS_STATE_WRITE_NEXT;
        }
    }

    //
    // Put the SMBus command code on the bus.
    //
    MAP_I2CMasterDataPut(pSMBus->ulI2CBase, pSMBus->ucCurrentCommand);

    //
    // Make sure that the bus is idle.
    //
    if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_BUS_BUSY);
    }

    //
    // Initiate the write operation.
    //
    MAP_I2CMasterControl(pSMBus->ulI2CBase, I2C_MASTER_CMD_BURST_SEND_START);

    //
    // Set the transfer in progress flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 1;

    //
    // Return to the caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Initiates a master Read Byte or Read Word transfer to an SMBus slave.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucTargetAddress specifies the slave address of the target device.
//! \param ucCommand is the command byte sent before the data is requested.
//! \param pucData is a pointer to the receive data buffer.
//! \param ucSize is the number of bytes to receive from the slave.
//!
//! This function supports both the Read Byte and Read Word protocols.  The
//! amount of data to receive is user defined, but limited to 1 or 2 bytes.
//!
//! This protocol supports the optional PEC byte for error checking.  To use
//! PEC, SMBusPECEnable() must be called before this function.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use,
//! \b SMBUS_DATA_SIZE_ERROR if ucSize is greater than 2, or \b SMBUS_OK if the
//! transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterByteWordRead(tSMBus *pSMBus, unsigned char ucTargetAddress,
                        unsigned char ucCommand, unsigned char *pucData,
                        unsigned char ucSize)
{
    unsigned char ucTempData;

    //
    // Make sure that the peripheral is not currently active.
    //
    if(MAP_I2CMasterBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_PERIPHERAL_BUSY);
    }

    //
    // If more than 2 bytes are requested, indicate error.
    //
    if(ucSize > 2)
    {
        return(SMBUS_DATA_SIZE_ERROR);
    }

    //
    // Update the configuration structure with the data for this transfer.
    //
    pSMBus->ucTargetSlaveAddress = ucTargetAddress;
    pSMBus->ucCurrentCommand = ucCommand;
    pSMBus->pucRxBuffer = pucData;
    pSMBus->ucTxSize = 0;
    pSMBus->ucTxIndex = 0;
    pSMBus->ucRxIndex = 0;
    pSMBus->ucRxSize = ucSize;
    pSMBus->ucCalculatedCRC = 0;

    //
    // Clear the block transfer, process call and raw I2C flags.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 0;

    //
    // Set the slave address and R/S bit.
    //
    MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase, pSMBus->ucTargetSlaveAddress,
                              false);

    //
    // Put the SMBus command code on the bus.
    //
    MAP_I2CMasterDataPut(pSMBus->ulI2CBase, pSMBus->ucCurrentCommand);

    //
    // Calculate the CRC for PEC (if used).
    //
    if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
    {
        //
        // Place the target slave address into a temporary data variable and
        // set the R/S bit to '1' for the CRC calculation.
        //
        ucTempData = pSMBus->ucTargetSlaveAddress << 1;

        //
        // Start off by calculating the CRC of the target slave address with
        // an initial value of 0.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(0, &ucTempData, 1);

        //
        // Add the command to the running CRC calculation.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                &pSMBus->ucCurrentCommand, 1);

        //
        // Update the state machine.
        //
        pSMBus->ucMasterState = SMBUS_STATE_READ_FIRST;
    }
    else
    {
        //
        // Update the state machine.
        //
        if(pSMBus->ucRxSize == 2)
        {
            pSMBus->ucMasterState = SMBUS_STATE_READ_FIRST;
        }
        else
        {
            pSMBus->ucMasterState = SMBUS_STATE_READ_ONE;
        }
    }

    //
    // Make sure that the bus is idle.
    //
    if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_BUS_BUSY);
    }

    //
    // Initiate the write operation.
    //
    MAP_I2CMasterControl(pSMBus->ulI2CBase, I2C_MASTER_CMD_BURST_SEND_START);

    //
    // Set the transfer in progress flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 1;

    //
    // Return to the caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Initiates a master Block Write transfer to an SMBus slave.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucTargetAddress specifies the slave address of the target device.
//! \param ucCommand is the command byte sent before the data is requested.
//! \param pucData is a pointer to the transmit data buffer.
//! \param ucSize is the number of bytes to send to the slave.
//!
//! This function supports the Block Write protocol.  The amount of data sent
//! to the slave is user defined, but limited to 32 bytes per the SMBus spec.
//!
//! This protocol supports the optional PEC byte for error checking.  To use
//! PEC, SMBusPECEnable() must be called before this function.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use,
//! \b SMBUS_DATA_SIZE_ERROR if ucSize is greater than 32, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterBlockWrite(tSMBus *pSMBus, unsigned char ucTargetAddress,
                      unsigned char ucCommand, unsigned char *pucData,
                      unsigned char ucSize)
{
    unsigned char ucTempData;

    //
    // Make sure that the peripheral is not currently active.
    //
    if(MAP_I2CMasterBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_PERIPHERAL_BUSY);
    }

    //
    // If more than 32 bytes are requested, indicate error.
    //
    if(ucSize > 32)
    {
        return(SMBUS_DATA_SIZE_ERROR);
    }

    //
    // Update the configuration structure with the data for this transfer.
    //
    pSMBus->ucTargetSlaveAddress = ucTargetAddress;
    pSMBus->ucCurrentCommand = ucCommand;
    pSMBus->pucTxBuffer = pucData;
    pSMBus->ucTxSize = ucSize;
    pSMBus->ucRxSize = 0;
    pSMBus->ucRxIndex = 0;
    pSMBus->ucCalculatedCRC = 0;

    //
    // Set the block transfer flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 1;

    //
    // Clear the process call and raw I2C flags.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 0;

    //
    // Initialize the buffer index to 0 and the interrupt state machine to
    // the appropriate state so that there is a known starting point
    // for each transaction.
    //
    pSMBus->ucTxIndex = 0;

    //
    // Calculate the CRC for PEC (if used).
    //
    if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
    {
        //
        // Place the target slave address into a temporary data variable and
        // make sure the R/S bit is set to '0' for the CRC calculation.
        //
        ucTempData = (pSMBus->ucTargetSlaveAddress << 1) &  0xfe;

        //
        // Start off by calculating the CRC of the target slave address with
        // an initial value of 0.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(0, &ucTempData, 1);

        //
        // Add the command to the running CRC calculation.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                &pSMBus->ucCurrentCommand, 1);

        //
        // Add the size to the running CRC calculation.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                &pSMBus->ucTxSize, 1);

        //
        // Add the data array to the calculation.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                pSMBus->pucTxBuffer,
                                                pSMBus->ucTxSize);
    }

    //
    // Set the slave address and R/S bit.
    //
    MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase, pSMBus->ucTargetSlaveAddress,
                              false);

    //
    // Write the first byte of the data.
    //
    MAP_I2CMasterDataPut(pSMBus->ulI2CBase, pSMBus->ucCurrentCommand);

    //
    // Update the state machine.
    //
    pSMBus->ucMasterState = SMBUS_STATE_WRITE_BLOCK_SIZE;

    //
    // Make sure that the bus is idle.
    //
    if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_BUS_BUSY);
    }

    //
    // Set the transfer in progress flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 1;

    //
    // Initiate the write operation.
    //
    MAP_I2CMasterControl(pSMBus->ulI2CBase, I2C_MASTER_CMD_BURST_SEND_START);

    //
    // Return to the caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Initiates a master Block Read transfer to an SMBus slave.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucTargetAddress specifies the slave address of the target device.
//! \param ucCommand is the command byte sent before the data is requested.
//! \param pucData is a pointer to the receive data buffer.
//!
//! This function supports the Block Read protocol.  The amount of data read
//! is defined by the slave device, but should never exceed 32 bytes per the
//! SMBus spec.  The receive size is the first data byte returned by the slave,
//! so this function assumes a size of 3 until the actual number is sent by
//! the slave.  In the application interrupt handler, SMBusRxPacketSizeGet()
//! can be used to obtain the amount of data sent by the slave.
//!
//! This protocol supports the optional PEC byte for error checking.  To use
//! PEC, SMBusPECEnable() must be called before this function.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterBlockRead(tSMBus *pSMBus, unsigned char ucTargetAddress,
                     unsigned char ucCommand, unsigned char *pucData)
{
    unsigned char ucTempData;

    //
    // Make sure that the peripheral is not currently active.
    //
    if(MAP_I2CMasterBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_PERIPHERAL_BUSY);
    }

    //
    // Update the configuration structure with the data for this transfer.
    //
    pSMBus->ucTargetSlaveAddress = ucTargetAddress;
    pSMBus->ucCurrentCommand = ucCommand;
    pSMBus->pucRxBuffer = pucData;
    pSMBus->ucRxIndex = 0;
    pSMBus->ucTxSize = 0;
    pSMBus->ucTxIndex = 0;
    pSMBus->ucCalculatedCRC = 0;

    //
    // Set the block transfer flag..
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 1;

    //
    // Clear the process call and raw I2C flags.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 0;

    //
    // Set the slave address and R/S bit.
    //
    MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase, pSMBus->ucTargetSlaveAddress,
                              false);

    //
    // Put the SMBus command code on the bus.
    //
    MAP_I2CMasterDataPut(pSMBus->ulI2CBase, pSMBus->ucCurrentCommand);

    //
    // Initially set the RX size to 3 to make the state machine work.
    // The slave will respond with the actual size of the transfer in the
    // first byte and that data will replace this initial value.
    //
    pSMBus->ucRxSize = 3;

    //
    // Calculate the CRC for PEC (if used).
    //
    if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
    {
        //
        // Place the target slave address into a temporary data variable and
        // set the R/S bit to '1' for the CRC calculation.
        //
        ucTempData = pSMBus->ucTargetSlaveAddress << 1;

        //
        // Start off by calculating the CRC of the target slave address with
        // an initial value of 0.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(0, &ucTempData, 1);

        //
        // Add the command to the running CRC calculation.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                &pSMBus->ucCurrentCommand, 1);
    }

    //
    // Update the state machine.
    //
    pSMBus->ucMasterState = SMBUS_STATE_READ_FIRST;

    //
    // Make sure that the bus is idle.
    //
    if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_BUS_BUSY);
    }

    //
    // Set the transfer in progress flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 1;

    //
    // Initiate the write operation.
    //
    MAP_I2CMasterControl(pSMBus->ulI2CBase, I2C_MASTER_CMD_BURST_SEND_START);

    //
    // Return to the caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Initiates a master Process Call transfer to an SMBus slave.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucTargetAddress specifies the slave address of the target device.
//! \param ucCommand is the command byte sent before the data is requested.
//! \param pucTxData is a pointer to the transmit data buffer.
//! \param pucRxData is a pointer to the receive data buffer.
//!
//! This function supports the Process Call protocol.  The amount of data sent
//! to and received from the slave is fixed to 2 bytes per direction (2 sent,
//! 2 received).
//!
//! This protocol supports the optional PEC byte for error checking.  To use
//! PEC, SMBusPECEnable() must be called before this function.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterProcessCall(tSMBus *pSMBus, unsigned char ucTargetAddress,
                       unsigned char ucCommand, unsigned char *pucTxData,
                       unsigned char *pucRxData)
{
    unsigned char ucTempData;

    //
    // Make sure that the peripheral is not currently active.
    //
    if(MAP_I2CMasterBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_PERIPHERAL_BUSY);
    }

    //
    // Update the configuration structure with the data for this transfer.
    //
    pSMBus->ucTargetSlaveAddress = ucTargetAddress;
    pSMBus->ucCurrentCommand = ucCommand;
    pSMBus->pucTxBuffer = pucTxData;
    pSMBus->pucRxBuffer = pucRxData;
    pSMBus->ucTxIndex = 0;
    pSMBus->ucTxSize = 2;
    pSMBus->ucRxIndex = 0;
    pSMBus->ucRxSize = 2;
    pSMBus->ucCalculatedCRC = 0;

    //
    // Set the process call flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 1;

    //
    // Clear the block transfer and raw I2C flags.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 0;

    //
    // Set the slave address and R/S bit.
    //
    MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase, pSMBus->ucTargetSlaveAddress,
                              false);

    //
    // Calculate the CRC for PEC (if used).
    //
    if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
    {
        //
        // Place the target slave address into a temporary data variable and
        // make sure the R/S bit is set to '0' for the CRC calculation.
        //
        ucTempData = (pSMBus->ucTargetSlaveAddress << 1) &  0xfe;

        //
        // Start off by calculating the CRC of the target slave address with
        // an initial value of 0.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(0, &ucTempData, 1);

        //
        // Add the command to the running CRC calculation.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                &pSMBus->ucCurrentCommand, 1);

        //
        // Add the data array to the calculation.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                pSMBus->pucTxBuffer,
                                                pSMBus->ucTxSize);
    }

    //
    // Put the SMBus command code on the bus.
    //
    MAP_I2CMasterDataPut(pSMBus->ulI2CBase, pSMBus->ucCurrentCommand);

    //
    // Update the state machine.
    //
    pSMBus->ucMasterState = SMBUS_STATE_WRITE_NEXT;

    //
    // Make sure that the bus is idle.
    //
    if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_BUS_BUSY);
    }

    //
    // Initiate the write operation.
    //
    MAP_I2CMasterControl(pSMBus->ulI2CBase, I2C_MASTER_CMD_BURST_SEND_START);

    //
    // Set the transfer in progress flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 1;

    //
    // Return to the caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Initiates a master Block Process Call transfer to an SMBus slave.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucTargetAddress specifies the slave address of the target device.
//! \param ucCommand is the command byte sent before the data is requested.
//! \param pucTxData is a pointer to the transmit data buffer.
//! \param ucTxSize is the number of bytes to send to the slave.
//! \param pucRxData is a pointer to the receive data buffer.
//!
//! This function supports the Block Write/Block Read Process Call protocol.
//! The amount of data sent to the slave is user defined but limited to 32 data
//! bytes.  The amount of data read is defined by the slave device, but should
//! never exceed 32 bytes per the SMBus spec.  The receive size is the first
//! data byte returned by the slave, so the actual size is populated in
//! SMBusMasterISRProcess().  In the application interrupt handler,
//! SMBusRxPacketSizeGet() can be used to obtain the amount of data sent by
//! the slave.
//!
//! This protocol supports the optional PEC byte for error checking.  To use
//! PEC, SMBusPECEnable() must be called before this function.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use,
//! \b SMBUS_DATA_SIZE_ERROR if ucTxSize is greater than 32, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterBlockProcessCall(tSMBus *pSMBus, unsigned char ucTargetAddress,
                            unsigned char ucCommand, unsigned char *pucTxData,
                            unsigned char ucTxSize, unsigned char *pucRxData)
{
    unsigned char ucTempData;

    //
    // Make sure that the peripheral is not currently active.
    //
    if(MAP_I2CMasterBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_PERIPHERAL_BUSY);
    }

    //
    // If more than 32 bytes are requested, indicate error.
    //
    if(ucTxSize > 32)
    {
        return(SMBUS_DATA_SIZE_ERROR);
    }

    //
    // Update the configuration structure with the data for this transfer.
    //
    pSMBus->ucTargetSlaveAddress = ucTargetAddress;
    pSMBus->ucCurrentCommand = ucCommand;
    pSMBus->pucTxBuffer = pucTxData;
    pSMBus->pucRxBuffer = pucRxData;
    pSMBus->ucTxIndex = 0;
    pSMBus->ucTxSize = ucTxSize;
    pSMBus->ucRxIndex = 0;
    pSMBus->ucRxSize = 3;
    pSMBus->ucCalculatedCRC = 0;

    //
    // Set the process call and block transfer flags.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 1;
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 1;

    //
    // Clear the raw I2C flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 0;

    //
    // Calculate the CRC for PEC (if used).
    //
    if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
    {
        //
        // Place the target slave address into a temporary data variable and
        // make sure the R/S bit is set to '0' for the CRC calculation.
        //
        ucTempData = (pSMBus->ucTargetSlaveAddress << 1) &  0xfe;

        //
        // Start off by calculating the CRC of the target slave address with
        // an initial value of 0.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(0, &ucTempData, 1);

        //
        // Add the command to the running CRC calculation.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                &pSMBus->ucCurrentCommand, 1);

        //
        // Add the size to the running CRC calculation.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                &pSMBus->ucTxSize, 1);

        //
        // Add the data array to the calculation.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                pSMBus->pucTxBuffer,
                                                pSMBus->ucTxSize);
    }

    //
    // Set the slave address and R/S bit.
    //
    MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase, pSMBus->ucTargetSlaveAddress,
                              false);

    //
    // Put the SMBus command code on the bus.
    //
    MAP_I2CMasterDataPut(pSMBus->ulI2CBase, pSMBus->ucCurrentCommand);

    //
    // Update the state machine.
    //
    pSMBus->ucMasterState = SMBUS_STATE_WRITE_BLOCK_SIZE;

    //
    // Make sure that the bus is idle.
    //
    if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_BUS_BUSY);
    }

    //
    // Initiate the write operation.
    //
    MAP_I2CMasterControl(pSMBus->ulI2CBase, I2C_MASTER_CMD_BURST_SEND_START);

    //
    // Set the transfer in progress flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 1;

    //
    // Return to the caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Initiates a ``raw'' I2C write transfer to a slave device.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucTargetAddress specifies the slave address of the target device.
//! \param pucData is a pointer to the transmit data buffer.
//! \param ucSize is the number of bytes to send to the slave.
//!
//! This function sends a user-defined number of bytes to an I2C slave without
//! using an SMBus protocol.  The data size is only limited to the size of the
//! ucSize variable, which is an unsigned character (8 bits, value of 255).
//!
//! Because this function uses ``raw'' I2C, PEC is not supported.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterI2CWrite(tSMBus *pSMBus, unsigned char ucTargetAddress,
                    unsigned char *pucData, unsigned char ucSize)
{
    //
    // Make sure that the peripheral is not currently active.
    //
    if(MAP_I2CMasterBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_PERIPHERAL_BUSY);
    }

    //
    // Update the configuration structure with the data for this transfer.
    //
    pSMBus->ucTargetSlaveAddress = ucTargetAddress;
    pSMBus->pucTxBuffer = pucData;
    pSMBus->ucTxSize = ucSize;
    pSMBus->ucTxIndex = 1;
    pSMBus->ucRxSize = 0;
    pSMBus->ucRxIndex = 0;

    //
    // PEC is not supported by raw I2C transfers, so force it to be disabled.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_PEC) = 0;

    //
    // Clear the block transfer and process call flags.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 0;

    //
    // Set the raw I2C flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 1;

    //
    // Set the slave address and R/S bit.
    //
    MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase, pSMBus->ucTargetSlaveAddress,
                              false);

    //
    // Put the first byte on the bus.
    //
    MAP_I2CMasterDataPut(pSMBus->ulI2CBase, pSMBus->pucTxBuffer[0]);

    //
    // Choose what to do based on the transmit size.
    //
    if(ucSize == 1)
    {
        //
        // Update the state machine.  Since it's the only byte being sent,
        // the state machine's next state is idle.
        //
        pSMBus->ucMasterState = SMBUS_STATE_IDLE;

        //
        // Make sure that the bus is idle.
        //
        if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
        {
            return(SMBUS_BUS_BUSY);
        }

        //
        // Initiate the write operation.
        //
        MAP_I2CMasterControl(pSMBus->ulI2CBase, I2C_MASTER_CMD_SINGLE_SEND);
    }
    else if(ucSize == 2)
    {
        //
        // If there are only 2 bytes to send just jump to the final write
        // state.
        //
        pSMBus->ucMasterState = SMBUS_STATE_WRITE_FINAL;

        //
        // Make sure that the bus is idle.
        //
        if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
        {
            return(SMBUS_BUS_BUSY);
        }

        //
        // Initiate the write operation.
        //
        MAP_I2CMasterControl(pSMBus->ulI2CBase,
                             I2C_MASTER_CMD_BURST_SEND_START);
    }
    else
    {
        //
        // Set the next state.
        //
        pSMBus->ucMasterState = SMBUS_STATE_WRITE_NEXT;

        //
        // Make sure that the bus is idle.
        //
        if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
        {
            return(SMBUS_BUS_BUSY);
        }

        //
        // Initiate the write operation.
        //
        MAP_I2CMasterControl(pSMBus->ulI2CBase,
                             I2C_MASTER_CMD_BURST_SEND_START);
    }

    //
    // Set the transfer in progress flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 1;

    //
    // Return to the caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Initiates a ``raw'' I2C read transfer to a slave device.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucTargetAddress specifies the slave address of the target device.
//! \param pucData is a pointer to the receive data buffer.
//! \param ucSize is the number of bytes to send to the slave.
//!
//! This function receives a user-defined number of bytes from an I2C slave
//! without using an SMBus protocol.  The data size is only limited to the size
//! of the ucSize variable, which is an unsigned character (8 bits, value of
//! 255).
//!
//! Because this function uses ``raw'' I2C, PEC is not supported.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterI2CRead(tSMBus *pSMBus, unsigned char ucTargetAddress,
                   unsigned char *pucData, unsigned char ucSize)
{
    //
    // Make sure that the peripheral is not currently active.
    //
    if(MAP_I2CMasterBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_PERIPHERAL_BUSY);
    }

    //
    // Update the configuration structure with the data for this transfer.
    //
    pSMBus->ucTargetSlaveAddress = ucTargetAddress;
    pSMBus->pucRxBuffer = pucData;
    pSMBus->ucTxSize = 0;
    pSMBus->ucTxIndex = 0;
    pSMBus->ucRxIndex = 0;
    pSMBus->ucRxSize = ucSize;

    //
    // PEC is not supported by raw I2C transfers, so force it to be disabled.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_PEC) = 0;

    //
    // Clear the block transfer and process call flags.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 0;

    //
    // Set the raw I2C flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 1;

    //
    // Set the slave address and R/S bit.
    //
    MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase, pSMBus->ucTargetSlaveAddress,
                              true);

    //
    // Make sure that the bus is idle.
    //
    if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_BUS_BUSY);
    }

    //
    // Choose what to do based on the receive size.
    //
    if(ucSize == 1)
    {
        //
        // Update the state machine.
        //
        pSMBus->ucMasterState = SMBUS_STATE_READ_WAIT;
    }
    else if(ucSize == 2)
    {
        pSMBus->ucMasterState = SMBUS_STATE_READ_FINAL;
    }
    else
    {
        //
        // Set the next state.
        //
        pSMBus->ucMasterState = SMBUS_STATE_READ_NEXT;
    }

    if(ucSize == 1)
    {
        //
        // Start the single receive.
        //
        MAP_I2CMasterControl(pSMBus->ulI2CBase, I2C_MASTER_CMD_SINGLE_RECEIVE);
    }
    else
    {
        //
        // Start the burst receive.
        //
        MAP_I2CMasterControl(pSMBus->ulI2CBase,
                             I2C_MASTER_CMD_BURST_RECEIVE_START);
    }

    //
    // Set the transfer in progress flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 1;

    //
    // Return to the caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Initiates a ``raw'' I2C write-read transfer to a slave device.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucTargetAddress specifies the slave address of the target device.
//! \param pucTxData is a pointer to the transmit data buffer.
//! \param ucTxSize is the number of bytes to send to the slave.
//! \param pucRxData is a pointer to the receive data buffer.
//! \param ucRxSize is the number of bytes to receive from the slave.
//!
//! This function initiates a write-read transfer to an I2C slave without using
//! an SMBus protocol.  The user-defined number of bytes is written to the
//! slave first, followed by the reception of the user-defined number of bytes.
//! The transmit and receive data sizes are only limited to the size of the
//! ucTxSize and ucRxSize variables, which are unsigned characters (8 bits,
//! value of 255).
//!
//! Because this function uses ``raw'' I2C, PEC is not supported.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterI2CWriteRead(tSMBus *pSMBus, unsigned char ucTargetAddress,
                        unsigned char *pucTxData, unsigned char ucTxSize,
                        unsigned char *pucRxData, unsigned char ucRxSize)
{
    //
    // Make sure that the peripheral is not currently active.
    //
    if(MAP_I2CMasterBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_PERIPHERAL_BUSY);
    }

    //
    // Update the configuration structure with the data for this transfer.
    //
    pSMBus->ucTargetSlaveAddress = ucTargetAddress;
    pSMBus->pucTxBuffer = pucTxData;
    pSMBus->pucRxBuffer = pucRxData;
    pSMBus->ucTxIndex = 1;
    pSMBus->ucTxSize = ucTxSize;
    pSMBus->ucRxIndex = 0;
    pSMBus->ucRxSize = ucRxSize;

    //
    // PEC is not supported by raw I2C transfers, so force it to be disabled.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_PEC) = 0;

    //
    // Set the process call flag.  Even though this is technically not an SMBus
    // process call, this flag is used in the interrupt state machine for
    // the bus turn around.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 1;

    //
    // Clear the block transfer flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 0;

    //
    // Set the raw I2C flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 1;

    //
    // Set the slave address and R/S bit.
    //
    MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase, pSMBus->ucTargetSlaveAddress,
                              false);

    //
    // Write the first byte of the data.
    //
    MAP_I2CMasterDataPut(pSMBus->ulI2CBase, pSMBus->pucTxBuffer[0]);

    //
    // Choose what to do based on the transmit size.
    //
    if(ucTxSize == 1)
    {
        //
        // Move to the read first state for the turn around.
        //
        pSMBus->ucMasterState = SMBUS_STATE_READ_FIRST;
    }
    else if(ucTxSize == 2)
    {
        pSMBus->ucMasterState = SMBUS_STATE_WRITE_FINAL;
    }
    else
    {
        //
        // Set the next state.
        //
        pSMBus->ucMasterState = SMBUS_STATE_WRITE_NEXT;
    }

    //
    // Make sure that the bus is idle.
    //
    if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
    {
        return(SMBUS_BUS_BUSY);
    }

    //
    // Initiate the write operation.
    //
    MAP_I2CMasterControl(pSMBus->ulI2CBase, I2C_MASTER_CMD_BURST_SEND_START);

    //
    // Set the transfer in progress flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 1;

    //
    // Return to the caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! \internal
//! Sends a ``general'' Get UDID packet.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param pucData is a pointer to the receive data buffer.
//!
//! This function sends a ``general'' Get UDID packet, used during Address
//! Resolution Protocol (ARP).  Since SMBus requires that data bytes be
//! transmitted in a certain order, the raw data in the pucData needs to be
//! treated as such.  To put the data in a known order, use
//! SMBusARPUDIDPacketDecode().
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterARPGetUDIDGen(tSMBus *pSMBus, unsigned char *pucData)
{
    //
    // Use the block read protocol to receive the UDID.
    //
    return(SMBusMasterBlockRead(pSMBus, SMBUS_ADR_DEFAULT_DEVICE,
                                SMBUS_CMD_ARP_GET_UDID, pucData));
}

//*****************************************************************************
//
//! \internal
//! Sends a ``directed'' Get UDID packet.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucTargetAddress specifies the slave address of the target device.
//! \param pucData is a pointer to the receive data buffer.
//!
//! This function sends a ``directed'' Get UDID packet, used during Address
//! Resolution Protocol (ARP).  A directed packet differs from a general packet
//! in that it targets a specific slave device.  Since SMBus requires that data
//! bytes be transmitted in a certain order, the raw data in the pucData needs
//! to be treated as such.  To put the data in a known order, use
//! SMBusARPUDIDPacketDecode().
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterARPGetUDIDDir(tSMBus *pSMBus, unsigned char ucTargetAddress,
                         unsigned char *pucData)
{
    //
    // Use the block read protocol to receive the UDID.
    //
    return(SMBusMasterBlockRead(pSMBus, SMBUS_ADR_DEFAULT_DEVICE,
                                (ucTargetAddress << 1 | 1), pucData));
}

//*****************************************************************************
//
//! \internal
//! Sends a ``general'' Reset Device packet.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function sends a ``general'' Reset Device packet, used during Address
//! Resolution Protocol (ARP).  This packet is used by an ARP Master to force
//! all non-PSA (Persistent Slave Address), ARP-capable devices to return to
//! their initial state.  This packet also tells the devices to clear their
//! Address Resolved (AR) and Address Valid (AV) flags.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterARPResetDeviceGen(tSMBus *pSMBus)
{
    //
    // Use the Send Byte protocol to send the packet.
    //
    return(SMBusMasterByteSend(pSMBus, SMBUS_ADR_DEFAULT_DEVICE,
                               SMBUS_CMD_ARP_RESET_DEVICE));
}

//*****************************************************************************
//
//! \internal
//! Sends a ``directed'' Reset Device packet.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function sends a ``directed'' Reset Device packet, used during Address
//! Resolution Protocol (ARP).  This packet is used by an ARP Master to force
//! a specific non-PSA (Persistent Slave Address), ARP-capable device to return
//! to its initial state.  This packet also tells the device to clear its
//! Address Resolved (AR) and Address Valid (AV) flags.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterARPResetDeviceDir(tSMBus *pSMBus, unsigned char ucTargetAddress)
{
    //
    // Use the Send Byte protocol to send the packet.
    //
    return(SMBusMasterByteSend(pSMBus, SMBUS_ADR_DEFAULT_DEVICE,
                               (ucTargetAddress << 1)));
}

//*****************************************************************************
//
//! Sends an ARP Assign Address packet.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param pucData is a pointer to the transmit data buffer.  This buffer
//!        should be correctly formatted using SMBusARPUDIDPacketEncode() and
//!        should contain the UDID data and the address for the slave.
//!
//! This function sends an Assign Address packet, used during Address
//! Resolution Protocol (ARP).  Because SMBus requires data bytes be sent out
//! MSB first, the UDID and target address should be formatted correctly by the
//! application or using SMBusARPUDIDPacketEncode() and placed into a data
//! buffer pointed to by pucData.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterARPAssignAddress(tSMBus *pSMBus, unsigned char *pucData)
{
    //
    // Use the Block Write protocol to send the packet.
    //
    return(SMBusMasterBlockWrite(pSMBus, SMBUS_ADR_DEFAULT_DEVICE,
                                 SMBUS_CMD_ARP_ASSIGN_ADDRESS, pucData, 17));
}

//*****************************************************************************
//
//! Sends a Notify ARP Master packet.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param pucData is a pointer to the transmit data buffer.  The data payload
//!        should be 0x0000 for this packet.
//!
//! This function sends a Notify ARP Master packet, used during Address
//! Resolution Protocol (ARP).  This packet is used by a slave to indicate
//! to the ARP Master that it needs attention.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterARPNotifyMaster(tSMBus *pSMBus, unsigned char *pucData)
{
    //
    // Use the Host Notify protocol to send the packet.
    //
    return(SMBusMasterHostNotify(pSMBus, (SMBUS_ADR_DEFAULT_DEVICE << 1),
                                 pucData));
}

//*****************************************************************************
//
//! Sends a Prepare to ARP packet.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function sends a Prepare to ARP packet, used during Address Resolution
//! Protocol (ARP).  This packet is used by an ARP Master to alert devices on
//! the bus that ARP is about to begin.    All ARP-capable devices must
//! acknowledge all bytes in this packet and clear their Address Resolved (AR)
//! flag.
//!
//! \return Returns \b SMBUS_PERIPHERAL_BUSY if the I2C peripheral is currently
//! active, \b SMBUS_BUS_BUSY if the bus is already in use, or \b SMBUS_OK if
//! the transfer has successfully been initiated.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterARPPrepareToARP(tSMBus *pSMBus)
{
    //
    // Use the Send Byte protocol to send the packet.
    //
    return(SMBusMasterByteSend(pSMBus, SMBUS_ADR_DEFAULT_DEVICE,
                               SMBUS_CMD_PREPARE_TO_ARP));
}

//*****************************************************************************
//
//! Master ISR processing function for the SMBus application.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function must be called in the application interrupt service routine
//! (ISR) to process SMBus master interrupts.
//!
//! \return Returns \b SMBUS_TIMEOUT if a bus timeout is detected,
//! \b SMBUS_ARB_LOST if I2C bus arbitration lost is detected,
//! \b SMBUS_ADDR_ACK_ERROR if the address phase of a transfer results in a
//! NACK, \b SMBUS_DATA_ACK_ERROR if the data phase of a transfer results in a
//! NACK, \b SMBUS_DATA_SIZE_ERROR if a receive buffer overrun is detected or
//! if a transmit operation tries to write more data than is allowed,
//! \b SMBUS_MASTER_ERROR if an unknown error occurs, \b SMBUS_PEC_ERROR if the
//! received PEC byte does not match the locally calculated value, or
//! \b SMBUS_OK if processing finished successfully.
//
//*****************************************************************************
tSMBusStatus
SMBusMasterIntProcess(tSMBus *pSMBus)
{
    unsigned long ulIntStatus;
    unsigned long ulErrorStatus;
    unsigned char ucTempData;

    //
    // Determine which interrupt made us get here.
    //
    ulIntStatus = MAP_I2CMasterIntStatusEx(pSMBus->ulI2CBase, true);

    //
    // Check for the timeout interrupt.  Since the peripheral will
    // automatically issue a stop, just clear the interrupt and return.
    //
    if(ulIntStatus & I2C_MASTER_INT_TIMEOUT)
    {
        //
        // Clear all pending interrupts and wait for the bus to become
        // free so we can issue a STOP.
        //
        MAP_I2CMasterIntClearEx(pSMBus->ulI2CBase, I2C_MASTER_INT_TIMEOUT |
                                I2C_MASTER_INT_DATA);

        //
        // Clear the transfer in progress flag.  New transactions will
        // be aborted until the bus is free.
        //
        HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 0;

        //
        // Return to caller.
        //
        return(SMBUS_TIMEOUT);
    }
    else
    {
        //
        // Clear the data interrupt.
        //
        MAP_I2CMasterIntClearEx(pSMBus->ulI2CBase, I2C_MASTER_INT_DATA);
    }

    //
    // Read the master interrupt status bits.
    //
    ulErrorStatus = HWREG(pSMBus->ulI2CBase + I2C_O_MCS);

    //
    // Check for arbitration lost.
    //
    if(ulErrorStatus & I2C_MCS_ARBLST)
    {
        //
        // Put the state machine back in the idle state.
        //
        pSMBus->ucMasterState = SMBUS_STATE_IDLE;

        //
        // Clear the transfer in progress flag.
        //
        HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 0;

        //
        // Return to caller.
        //
        return(SMBUS_ARB_LOST);
    }

    //
    // Check for an error.
    //
    if(ulErrorStatus & I2C_MCS_ERROR)
    {
        //
        // Put the state machine back in the idle state.
        //
        pSMBus->ucMasterState = SMBUS_STATE_IDLE;

        //
        // Check to see if the bus is free.  There are two interrupts when a
        // NACK happens, and the bus should only be free during the second
        // interrupt.  During the first interrupt (when the bus is busy),
        // generate the necessary STOP condition.
        //
        if(MAP_I2CMasterBusBusy(pSMBus->ulI2CBase))
        {
            //
            // Issue a STOP.
            //
            MAP_I2CMasterControl(pSMBus->ulI2CBase,
                                 I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
        }
        else
        {
            //
            // Clear the transfer in progress flag.
            //
            HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 0;
        }

        //
        // Check for ACK errors.
        //
        if(ulErrorStatus & I2C_MCS_ADRACK)
        {
            //
            // Return to caller.
            //
            return(SMBUS_ADDR_ACK_ERROR);
        }
        else if(ulErrorStatus & I2C_MCS_DATACK)
        {
            //
            // Return to caller.
            //
            return(SMBUS_DATA_ACK_ERROR);
        }
        else
        {
            //
            // Return to caller.  Should never get here.
            //
            return(SMBUS_MASTER_ERROR);
        }
    }

    //
    // If no error conditions, determine what to do based on the state.
    //
    switch(pSMBus->ucMasterState)
    {
        //
        // The idle state.  This state should only be reached after the last
        // byte of a master transmit.
        //
        case SMBUS_STATE_IDLE:
        {
            //
            // If the peripheral is not busy clear the transfer in progress
            // flag.  This means that the peripheral has given up the bus,
            // most likely due to the end of a transmit operation.
            //
            if(!MAP_I2CMasterBusy(pSMBus->ulI2CBase))
            {
                //
                // Clear the transfer in progress flag.
                //
                HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 0;
            }

            //
            // This state is done.
            //
            break;
        }

        //
        // When using a block write, the transfer size must be sent before the
        // data payload.
        //
        case SMBUS_STATE_WRITE_BLOCK_SIZE:
        {
            //
            // Write the block write size to the data register.
            //
            MAP_I2CMasterDataPut(pSMBus->ulI2CBase, pSMBus->ucTxSize);

            //
            // Continue the burst write.
            //
            MAP_I2CMasterControl(pSMBus->ulI2CBase,
                                 I2C_MASTER_CMD_BURST_SEND_CONT);

            //
            // The next data byte is from the data payload.
            //
            if((pSMBus->ucTxSize == 1) &&
               !(HWREGBITB(&pSMBus->usFlags, FLAG_PEC)))
            {
                pSMBus->ucMasterState = SMBUS_STATE_WRITE_FINAL;
            }
            else
            {
                pSMBus->ucMasterState = SMBUS_STATE_WRITE_NEXT;
            }

            //
            // This state is done.
            //
            break;
        }
        //
        // The state for the middle of a burst write.
        //
        case SMBUS_STATE_WRITE_NEXT:
        {
            //
            // Write the next byte to the data register.
            //
            MAP_I2CMasterDataPut(pSMBus->ulI2CBase,
                                 pSMBus->pucTxBuffer[pSMBus->ucTxIndex++]);

            //
            // Continue the burst write.
            //
            MAP_I2CMasterControl(pSMBus->ulI2CBase,
                                 I2C_MASTER_CMD_BURST_SEND_CONT);

            //
            // Determine the next state based on the values of the PEC and
            // process call flags.
            //

            //
            // If PEC is active and process call is not active.
            //
            if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
            {
                //
                // If a process call, there is no PEC byte on the transmit.
                //
                if(HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL))
                {
                    //
                    // Check to see if the TX index is equal to size minus 1.
                    //
                    if(pSMBus->ucTxIndex == (pSMBus->ucTxSize - 1))
                    {
                        pSMBus->ucMasterState = SMBUS_STATE_WRITE_FINAL;
                    }
                }
                else
                {
                    //
                    // If the TX index is the same as the size, we're done.
                    //
                    if(pSMBus->ucTxIndex == pSMBus->ucTxSize)
                    {
                        pSMBus->ucMasterState = SMBUS_STATE_WRITE_FINAL;
                    }
                }
            }

            //
            // If PEC is not used, regardless of whether this is a process
            // call.
            //
            else
            {
                //
                // Check to see if the TX index is equal to the size minus 1.
                //
                if(pSMBus->ucTxIndex == (pSMBus->ucTxSize - 1))
                {
                    pSMBus->ucMasterState = SMBUS_STATE_WRITE_FINAL;
                }
            }

            //
            // This state is done.
            //
            break;
        }

        //
        // The state for the final write of a burst sequence.
        //
        case SMBUS_STATE_WRITE_FINAL:
        {
            //
            // Determine what data to write to the data register based
            // on the values of the PEC and process call flags.
            //
            //
            // If PEC is active, write the PEC byte to the data register.
            //
            if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
            {
                //
                // If a process call is active, send data, not CRC.
                //
                if(HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL))
                {
                    //
                    // Write the final byte from TX buffer to the data
                    // register.
                    //
                    MAP_I2CMasterDataPut(pSMBus->ulI2CBase,
                                     pSMBus->pucTxBuffer[pSMBus->ucTxIndex++]);
                }
                else
                {
                    //
                    // Write the calculated CRC (PEC) byte to the data
                    // register.
                    //
                    MAP_I2CMasterDataPut(pSMBus->ulI2CBase,
                                         pSMBus->ucCalculatedCRC);
                }
            }
            else
            {
                //
                // Write the final byte from TX buffer to the data register.
                //
                MAP_I2CMasterDataPut(pSMBus->ulI2CBase,
                                     pSMBus->pucTxBuffer[pSMBus->ucTxIndex++]);
            }

            //
            // If a process call is active, send out the repeated start to
            // begin the RX portion.
            //
            if(HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL))
            {
                //
                // Move to the read first "turnaround" state.
                //
                pSMBus->ucMasterState = SMBUS_STATE_READ_FIRST;

                //
                // Continue the burst write.
                //
                MAP_I2CMasterControl(pSMBus->ulI2CBase,
                                     I2C_MASTER_CMD_BURST_SEND_CONT);
            }
            else
            {
                //
                // Finish the burst write.
                //
                MAP_I2CMasterControl(pSMBus->ulI2CBase,
                                     I2C_MASTER_CMD_BURST_SEND_FINISH);

                //
                // Since we end the transaction after the last byte is sent,
                // the next state is idle.
                //
                pSMBus->ucMasterState = SMBUS_STATE_IDLE;
            }

            //
            // This state is done.
            //
            break;
        }

        //
        // The state for a single byte read.
        //
        case SMBUS_STATE_READ_ONE:
        {
            //
            // Put the I2C master into receive mode.
            //
            MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase,
                                      pSMBus->ucTargetSlaveAddress, true);

            //
            // Perform a single byte read.
            //
            MAP_I2CMasterControl(pSMBus->ulI2CBase,
                                 I2C_MASTER_CMD_SINGLE_RECEIVE);

            //
            // The next state is the wait for final read state.
            //
            pSMBus->ucMasterState = SMBUS_STATE_READ_WAIT;

            //
            // This state is done.
            //
            break;
        }

        //
        // The state for the start of a burst read.
        //
        case SMBUS_STATE_READ_FIRST:
        {
            //
            // Put the I2C master into receive mode.
            //
            MAP_I2CMasterSlaveAddrSet(pSMBus->ulI2CBase,
                                      pSMBus->ucTargetSlaveAddress, true);

            //
            // Handle the case where PEC is used.
            //
            if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
            {
                //
                // Add the target address and R/S bit to the running CRC
                // calculation.
                //
                ucTempData = ((pSMBus->ucTargetSlaveAddress << 1) & 0xfe) | 1;

                //
                // Update the calculated CRC value in the configuration
                // structure.
                //
                pSMBus->ucCalculatedCRC =
                    MAP_Crc8CCITT(pSMBus->ucCalculatedCRC, &ucTempData, 1);

                //
                // Set the next state in the state machine.
                //
                if(pSMBus->ucRxSize > 1)
                {
                    //
                    // If this is a block transfer, the next state is to read
                    // back the number of bytes that the slave will be sending.
                    //
                    if(HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER))
                    {
                        pSMBus->ucMasterState = SMBUS_STATE_READ_BLOCK_SIZE;
                    }

                    //
                    // For every other case...
                    //
                    else
                    {
                        pSMBus->ucMasterState = SMBUS_STATE_READ_NEXT;
                    }
                }

                //
                // If 1 byte remains, move to the final read state.
                //
                else
                {
                    pSMBus->ucMasterState = SMBUS_STATE_READ_FINAL;
                }
            }
            else
            {
                //
                // Set the next state in the state machine.
                //
                if(pSMBus->ucRxSize > 2)
                {
                    //
                    // If this is a block transfer, the next state is to read
                    // back the number of bytes that the slave will be sending.
                    //
                    if(HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER))
                    {
                        pSMBus->ucMasterState = SMBUS_STATE_READ_BLOCK_SIZE;
                    }

                    //
                    // For every other case...
                    //
                    else
                    {
                        pSMBus->ucMasterState = SMBUS_STATE_READ_NEXT;
                    }
                }

                //
                // If 2 bytes remain, move to the final read state.
                //
                else
                {
                    pSMBus->ucMasterState = SMBUS_STATE_READ_FINAL;
                }
            }

            //
            // Start the burst receive.
            //
            MAP_I2CMasterControl(pSMBus->ulI2CBase,
                                 I2C_MASTER_CMD_BURST_RECEIVE_START);

            //
            // This state is done.
            //
            break;
        }

        //
        // The state for the size of a block read.
        //
        case SMBUS_STATE_READ_BLOCK_SIZE:
        {
            //
            // Update the RX size with the data byte.
            //
            pSMBus->ucRxSize = MAP_I2CMasterDataGet(pSMBus->ulI2CBase);

            //
            // If more than 32 bytes are going to be sent, error.
            //
            if((pSMBus->ucRxSize > 32) || (pSMBus->ucRxSize == 0))
            {
                //
                // Set the next state.
                //
                pSMBus->ucMasterState = SMBUS_STATE_READ_ERROR_STOP;

                //
                // If too many or too few bytes, error.
                //
                MAP_I2CMasterControl(pSMBus->ulI2CBase,
                                     I2C_MASTER_CMD_SINGLE_RECEIVE);

                //
                // Break from this case.
                //
                break;
            }

            //
            // If PEC is enabled, add the size byte to the calculation and
            // add one to the size variable to account for the extra PEC byte.
            //
            if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
            {
                //
                // Calculate the new CRC and update configuration structure.
                //
                pSMBus->ucCalculatedCRC =
                    MAP_Crc8CCITT(pSMBus->ucCalculatedCRC, &pSMBus->ucRxSize,
                                  1);
            }

            //
            // Update the state machine.
            //
            switch(pSMBus->ucRxSize)
            {
                //
                // 1 byte remaining.
                //
                case 1:
                {
                    //
                    // If only one byte remains and PEC, go to the second
                    // to last byte state.
                    //
                    if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
                    {
                        pSMBus->ucMasterState = SMBUS_STATE_READ_FINAL;
                    }

                    //
                    // If only one byte remains and no PEC, end the burst
                    // transfer.
                    //
                    else
                    {
                        pSMBus->ucMasterState = SMBUS_STATE_READ_WAIT;
                    }

                    //
                    // This switch is done.
                    //
                    break;
                }

                //
                // 2 bytes remaining.
                //
                case 2:
                {
                    //
                    // If two bytes and PEC remain, move to read next
                    // state.
                    //
                    if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
                    {
                        pSMBus->ucMasterState = SMBUS_STATE_READ_NEXT;
                    }

                    //
                    // If two bytes remain, move to the final read state.
                    //
                    else
                    {
                        pSMBus->ucMasterState = SMBUS_STATE_READ_FINAL;
                    }

                    //
                    // This switch is done.
                    //
                    break;
                }

                //
                // For every other situation (i.e. remaining bytes is
                // greater than 2).
                //
                default:
                {
                    //
                    // If more than 2 bytes to read, move to the next byte
                    // state.
                    //
                    pSMBus->ucMasterState = SMBUS_STATE_READ_NEXT;

                    //
                    // This switch is done.
                    //
                    break;
                }
            }

            //
            // Determine how to step the I2C state machine.
            //
            if((pSMBus->ucRxSize == 1) &&
               !HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
            {
                //
                // If exactly 1 byte remains, read the byte and send a STOP.
                //
                MAP_I2CMasterControl(pSMBus->ulI2CBase,
                                     I2C_MASTER_CMD_BURST_SEND_FINISH);
            }
            else
            {
                //
                // Otherwise, continue the burst read.
                //
                MAP_I2CMasterControl(pSMBus->ulI2CBase,
                                     I2C_MASTER_CMD_BURST_RECEIVE_CONT);
            }

            //
            // This state is done.
            //
            break;
        }

        //
        // The state for the middle of a burst read.
        //
        case SMBUS_STATE_READ_NEXT:
        {
            //
            // Check for a buffer overrun.
            //
            if(pSMBus->ucRxIndex >= pSMBus->ucRxSize)
            {
                //
                // Dummy read of data register.
                //
                ucTempData = MAP_I2CMasterDataGet(pSMBus->ulI2CBase);

                //
                // If too many or too few bytes, error.
                //
                MAP_I2CMasterControl(pSMBus->ulI2CBase,
                                     I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

                //
                // Set the next state.
                //
                pSMBus->ucMasterState = SMBUS_STATE_READ_ERROR_STOP;

                //
                // Break from this case.
                //
                break;
            }

            //
            // Read the received character.
            //
            pSMBus->pucRxBuffer[pSMBus->ucRxIndex] =
                MAP_I2CMasterDataGet(pSMBus->ulI2CBase);

            //
            // Continue the burst read.
            //
            MAP_I2CMasterControl(pSMBus->ulI2CBase,
                                 I2C_MASTER_CMD_BURST_RECEIVE_CONT);

            //
            // If PEC is enabled, add the received byte to the calculation.
            //
            if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
            {
                //
                // Calculate the new CRC and update configuration structure.
                //
                pSMBus->ucCalculatedCRC =
                    MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                  &pSMBus->pucRxBuffer[pSMBus->ucRxIndex], 1);

                //
                // Increment the receive buffer index.
                //
                pSMBus->ucRxIndex++;

                //
                // If there is 1 byte remaining, make next state be the
                // end of burst read state.
                //
                if((pSMBus->ucRxSize - pSMBus->ucRxIndex) == 1)
                {
                    pSMBus->ucMasterState = SMBUS_STATE_READ_FINAL;
                }
            }
            else
            {
                //
                // Increment the receive buffer index.
                //
                pSMBus->ucRxIndex++;

                //
                // If there are two bytes remaining, make next state be the
                // end of burst read state.
                //
                if((pSMBus->ucRxSize - pSMBus->ucRxIndex) == 2)
                {
                    pSMBus->ucMasterState = SMBUS_STATE_READ_FINAL;
                }
            }

            //
            // This state is done.
            //
            break;
        }

        //
        // The state for the end of a burst read.
        //
        case SMBUS_STATE_READ_FINAL:
        {
            //
            // Check for a buffer overrun.
            //
            if(pSMBus->ucRxIndex >= pSMBus->ucRxSize)
            {
                //
                // Dummy read of data register.
                //
                ucTempData = MAP_I2CMasterDataGet(pSMBus->ulI2CBase);

                //
                // If too many or too few bytes, error.
                //
                MAP_I2CMasterControl(pSMBus->ulI2CBase,
                                     I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

                //
                // Set the next state.
                //
                pSMBus->ucMasterState = SMBUS_STATE_READ_ERROR_STOP;

                //
                // Break from this case.
                //
                break;
            }

            //
            // Read the received character.
            //
            pSMBus->pucRxBuffer[pSMBus->ucRxIndex] =
                MAP_I2CMasterDataGet(pSMBus->ulI2CBase);

            //
            // The next state is the wait for final read state.
            //
            pSMBus->ucMasterState = SMBUS_STATE_READ_WAIT;

            //
            // Finish the burst read.
            //
            MAP_I2CMasterControl(pSMBus->ulI2CBase,
                                 I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

            //
            // If PEC is enabled, add the received byte to the calculation.
            //
            if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
            {
                //
                // Calculate the new CRC and update configuration structure.
                //
                pSMBus->ucCalculatedCRC =
                    MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                  &pSMBus->pucRxBuffer[pSMBus->ucRxIndex], 1);
            }

            //
            // Increment the receive buffer index.
            //
            pSMBus->ucRxIndex++;

            //
            // This state is done.
            //
            break;
        }

        //
        // This state is for the final read of a single or burst read.
        //
        case SMBUS_STATE_READ_WAIT:
        {
            //
            // Read the received byte.
            //
            ucTempData = MAP_I2CMasterDataGet(pSMBus->ulI2CBase);

            //
            // If PEC is enabled, check the value that just came in to see
            // if it matches.
            //
            if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
            {
                //
                // Check for a buffer overrun.
                //
                if(pSMBus->ucRxIndex > pSMBus->ucRxSize)
                {
                    //
                    // Clear the transfer in progress flag.
                    //
                    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 0;

                    //
                    // Return the error condition.
                    //
                    return(SMBUS_DATA_SIZE_ERROR);
                }

                //
                // Store the received CRC byte.
                //
                pSMBus->ucReceivedCRC = ucTempData;

                //
                // If the CRC doesn't match, send a NACK and indicate the
                // failure to the application.
                //
                if(pSMBus->ucReceivedCRC != pSMBus->ucCalculatedCRC)
                {
                    //
                    // Clear the transfer in progress flag.
                    //
                    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 0;

                    //
                    // Return the error condition.
                    //
                    return(SMBUS_PEC_ERROR);
                }
            }
            else
            {
                //
                // Check for a buffer overrun.
                //
                if(pSMBus->ucRxIndex >= pSMBus->ucRxSize)
                {
                    //
                    // Clear the transfer in progress flag.
                    //
                    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 0;

                    //
                    // Return the error condition.
                    //
                    return(SMBUS_DATA_SIZE_ERROR);
                }

                //
                // Read the received byte.
                //
                pSMBus->pucRxBuffer[pSMBus->ucRxIndex] = ucTempData;

                //
                // Increment the receive buffer index.
                //
                pSMBus->ucRxIndex++;
            }

            //
            // The state machine is now idle.
            //
            pSMBus->ucMasterState = SMBUS_STATE_IDLE;

            //
            // Clear the transfer in progress flag.
            //
            HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 0;

            //
            // This state is done.
            //
            break;
        }

        //
        // This state is for a transaction that needed to end due to a
        // size error.
        //
        case SMBUS_STATE_READ_ERROR_STOP:
        {
            //
            // Dummy read the received byte.
            //
            ucTempData = MAP_I2CMasterDataGet(pSMBus->ulI2CBase);

            //
            // The state machine is now idle.
            //
            pSMBus->ucMasterState = SMBUS_STATE_IDLE;

            //
            // Clear the transfer in progress flag.
            //
            HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 0;

            //
            // Return the error condition.
            //
            return(SMBUS_DATA_SIZE_ERROR);
        }
    }

    //
    // Return to caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Enables the appropriate master interrupts for stack processing.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function enables the I2C interrupts used by the SMBus master.  Both
//! the peripheral-level and NVIC-level interrupts are enabled.
//! SMBusMasterInit() must be called before this function because this function
//! relies on the I2C base address being defined.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusMasterIntEnable(tSMBus *pSMBus)
{
    //
    // Enable the master interrupts.
    //
    MAP_I2CMasterIntEnableEx(pSMBus->ulI2CBase, I2C_MASTER_INT_DATA |
                             I2C_MASTER_INT_TIMEOUT);

    //
    // Enable the interrupt in the NVIC.
    //
    switch(pSMBus->ulI2CBase)
    {
        case I2C0_MASTER_BASE:
        {
            MAP_IntEnable(INT_I2C0);
            break;
        }

        case I2C1_MASTER_BASE:
        {
            MAP_IntEnable(INT_I2C1);
            break;
        }

        case I2C2_MASTER_BASE:
        {
            MAP_IntEnable(INT_I2C2);
            break;
        }

        case I2C3_MASTER_BASE:
        {
            MAP_IntEnable(INT_I2C3);
            break;
        }

        case I2C4_MASTER_BASE:
        {
            MAP_IntEnable(INT_I2C4);
            break;
        }

        case I2C5_MASTER_BASE:
        {
            MAP_IntEnable(INT_I2C5);
            break;
        }
    }
}

//*****************************************************************************
//
//! Initializes an I2C master peripheral for SMBus functionality.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ulI2CBase specifies the base address of the I2C master peripheral.
//! \param ulSMBusClock specifies the system clock speed of the MCU.
//!
//! This function initializes an I2C peripheral for SMBus master use.  The
//! instance-specific configuration structure is initialized to a set of known
//! values and the I2C peripheral is configured for 100kHz use, which is
//! required by the SMBus specification.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusMasterInit(tSMBus *pSMBus, unsigned long ulI2CBase,
                unsigned long ulSMBusClock)
{
    //
    // Initialize the configuration structure.
    //
    pSMBus->pUDID = 0;
    pSMBus->ulI2CBase = ulI2CBase;
    pSMBus->usFlags = 0;
    pSMBus->ucMasterState = SMBUS_STATE_IDLE;
    pSMBus->ucOwnSlaveAddress = 0;
    pSMBus->ucTargetSlaveAddress = 0;
    pSMBus->ucCurrentCommand = 0;
    pSMBus->ucCalculatedCRC = 0;
    pSMBus->ucTxSize = 0;
    pSMBus->ucTxIndex = 0;
    pSMBus->ucRxSize = 0;
    pSMBus->ucRxIndex = 0;

    //
    // Enable and initialize the I2C master module Using the system clock.
    // The I2C transfer rate will always be 100kHz since fast mode is not
    // supported by SMBus.
    //
    MAP_I2CMasterInitExpClk(pSMBus->ulI2CBase, ulSMBusClock, false);

    //
    // Configure bus timeout to 25ms.  12-bit value for 25ms is 0x9C4 (2500
    // clocks), so round upper 8 bits to 0x9C.  Each clock is 10us since
    // 100kHz I2C is required for SMBus.
    //
    MAP_I2CMasterTimeoutSet(pSMBus->ulI2CBase, 0x9C);
}

//*****************************************************************************
//
//! Slave ISR processing function for the SMBus application.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function must be called in the application interrupt service routine
//! (ISR) to process SMBus slave interrupts.
//!
//! If manual acknowledge is enabled using SMBusSlaveManualACKEnable(), this
//! function processes the data byte, but does not send the ACK/NACK value.  In
//! this case, the user application is responsible for sending the acknowledge
//! bit based on the return code of this function.
//!
//! When receiving a Quick Command from the master, the slave has some set-up
//! requirements.  When the master sends the R/S (data) bit as '0', nothing
//! additional needs to be done in the slave and SMBusSlaveIntProcess() returns
//! \b SMBUS_SLAVE_QCMD_0.  However, when the master sends the R/S (data) bit
//! as '1', the slave must write the data register with data containing a '1'
//! in bit 7.  This means that when receiving a Quick Command, the slave must
//! set up the TX buffer to either have 1 data byte with bit 7 set to '1' or
//! set up the TX buffer to be zero length.  In the case where 1 data byte is
//! put in the TX buffer, SMBusSlaveIntProcess() returns \b SMBUS_OK the first
//! time its called and \b SMBUS_SLAVE_QCMD_0 the second.  In the case where
//! the TX buffer has no data, SMBusSlaveIntProcess() will return
//! \b SMBUS_SLAVE_ERROR the first time its called, and \b SMBUS_SLAVE_QCMD_1
//! the second time.
//!
//! \return Returns \b SMBUS_SLAVE_FIRST_BYTE if the first byte (typically the
//! SMBus command) has been received; \b SMBUS_SLAVE_NOT_READY if the slave's
//! transmit buffer is not yet initialized when the master requests data from
//! the slave; \b SMBUS_DATA_SIZE_ERROR if during a master block write, the
//! size sent by the master is greater than the amount of available space in
//! the receive buffer; \b SMBUS_SLAVE_ERROR if a buffer overrun is detected
//! during a slave receive operation or if data is sent and was not expected;
//! \b SMBUS_SLAVE_QCMD_0 if a Quick Command was received with data '0';
//! \b SMBUS_SLAVE_QCMD_1 if a Quick Command was received with data '1';
//! \b SMBUS_TRANSFER_COMPLETE if a STOP is detected on the bus, marking the
//! end of a transfer; \b SMBUS_PEC_ERROR if the received PEC byte does not
//! match the locally calculated value; or \b SMBUS_OK if processing finished
//! successfully.
//
//*****************************************************************************
tSMBusStatus
SMBusSlaveIntProcess(tSMBus *pSMBus)
{
    unsigned long ulInterruptStatus;
    unsigned long ulSlaveStatus = 0;
    unsigned char ucCRCTemp;
    unsigned char ucDataTemp;

    //
    // Determine which interrupt was asserted.
    //
    ulInterruptStatus = MAP_I2CSlaveIntStatusEx(pSMBus->ulI2CBase, true);

    //
    // Check the status register.
    //
    ulSlaveStatus = MAP_I2CSlaveStatus(pSMBus->ulI2CBase);

    //
    // Check for the START interrupt.
    //
    if(ulInterruptStatus & I2C_SLAVE_INT_START)
    {
        //
        // Clear the interrupt.
        //
        MAP_I2CSlaveIntClearEx(pSMBus->ulI2CBase, I2C_SLAVE_INT_START);


        //
        // This interrupt is not supported outside of using the FIFO.
        //
        return(SMBUS_OK);
    }

    //
    // Check for the STOP interrupt.
    //
    if(ulInterruptStatus & I2C_SLAVE_INT_STOP)
    {
        //
        // Make sure the transfer in progress flag is cleared.  In the case
        // of Quick Command, it should never be set, so this is safe.
        //
        HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 0;

        //
        // Clear the interrupt.
        //
        MAP_I2CSlaveIntClearEx(pSMBus->ulI2CBase, I2C_SLAVE_INT_STOP);

        //
        // Check to see if a Quick Command was sent.
        //
        if(ulSlaveStatus & 0x10)
        {
            //
            // Make sure the TX/RX index is 0.  If not, we should not be here.
            // Other data should not have been sent or received during a Quick
            // Command.
            //
            if((pSMBus->ucRxIndex != 0) || (pSMBus->ucTxIndex != 0))
            {
                //
                // Return an error.
                //
                return(SMBUS_SLAVE_ERROR);
            }

            //
            // Tell caller a Quick Command has occurred and the data value.
            //
            if(ulSlaveStatus & 0x20)
            {
                return(SMBUS_SLAVE_QCMD_1);
            }
            else
            {
                return(SMBUS_SLAVE_QCMD_0);
            }
        }

        //
        // Move to the idle state.
        //
        pSMBus->ucSlaveState = SMBUS_STATE_IDLE;

        //
        // Return end of transfer.
        //
        return(SMBUS_TRANSFER_COMPLETE);
    }

    //
    // Check for the DATA interrupt.
    //
    if(ulInterruptStatus & I2C_SLAVE_INT_DATA)
    {
        //
        // Clear the I2C interrupt.
        //
        MAP_I2CSlaveIntClearEx(pSMBus->ulI2CBase, I2C_SLAVE_INT_DATA);

        //
        // Make sure that at least one of the relevant status bits is set.
        //
        if(!(ulSlaveStatus & 0x07))
        {
            //
            // No status bits were set - this is bad.  Should never get here.
            //
            return(SMBUS_SLAVE_ERROR);
        }

        //
        // Every time this interrupt occurs, a transfer is in progress.  Make
        // sure the flag is set appropriately.
        //
        HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 1;

        //
        // Handle the request type.
        //
        switch((ulSlaveStatus & 0x07))
        {
            //
            // The first byte after the slave's own address has been received.
            // This is almost always the command byte in SMBus.  The only
            // exception is when the Send Byte protocol is used by the master.
            //
            case I2C_SLAVE_ACT_RREQ_FBR:
            {
                //
                // Check which slave address was called out.  Set the active
                // address to the matched address.
                //
                if(MAP_I2CSlaveStatus(pSMBus->ulI2CBase) & I2C_SCSR_OAR2SEL)
                {
                    pSMBus->ucOwnSlaveAddress = (HWREG(pSMBus->ulI2CBase +
                                                       I2C_O_SOAR2) & 0x7f);
                }
                else
                {
                    pSMBus->ucOwnSlaveAddress = HWREG(pSMBus->ulI2CBase +
                                                      I2C_O_SOAR);
                }

                //
                // If raw I2C, data goes into buffer.
                //
                if(HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C))
                {
                    pSMBus->pucRxBuffer[pSMBus->ucRxIndex++] =
                        MAP_I2CSlaveDataGet(pSMBus->ulI2CBase);
                }

                //
                // Read the first byte into the ucCurrentCommand member.
                //
                else
                {
                    pSMBus->ucCurrentCommand =
                        MAP_I2CSlaveDataGet(pSMBus->ulI2CBase);
                }

                //
                // If PEC is enabled, add the address to the CRC calculation.
                //
                if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
                {
                    //
                    // Add the address to the CRC calculation.  In this case
                    // R/S will always be 0.  Also, this is the start of the
                    // CRC calculation, so the initial value is 0.
                    //
                    ucCRCTemp = pSMBus->ucOwnSlaveAddress << 1;

                    //
                    // Calculate new CRC.
                    //
                    pSMBus->ucCalculatedCRC = Crc8CCITT(0, &ucCRCTemp, 1);

                    //
                    // Add the data byte (ucCurrentCommand) to the CRC
                    // calculation.
                    //
                    pSMBus->ucCalculatedCRC =
                        MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                      &pSMBus->ucCurrentCommand, 1);
                }

                //
                // Update the state machine.
                //
                pSMBus->ucSlaveState = SMBUS_STATE_SLAVE_POST_COMMAND;

                //
                // Actions for this case are complete.
                //
                return(SMBUS_SLAVE_FIRST_BYTE);
            }

            //
            // A data byte other than the first data byte has been received.
            //
            case I2C_SLAVE_ACT_RREQ:
            {
                //
                // Determine what to do based on the current state.
                //
                switch(pSMBus->ucSlaveState)
                {
                    //
                    // Receive first post-command byte.
                    //
                    case SMBUS_STATE_SLAVE_POST_COMMAND:
                    {
                        //
                        // Read the data into the a temporary variable.
                        //
                        ucDataTemp = MAP_I2CSlaveDataGet(pSMBus->ulI2CBase);

                        //
                        // Check if this is a block transfer.
                        //
                        if(HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER))
                        {
                            //
                            // Make sure there is enough space in the buffer.
                            // If not, NACK.  If there is, overwrite the
                            // current size with the size sent by the master.
                            //
                            if(ucDataTemp > pSMBus->ucRxSize)
                            {
                                //
                                // Update the state machine.
                                //
                                pSMBus->ucSlaveState = SMBUS_STATE_READ_DONE;

                                //
                                // Indicate a size error.
                                //
                                return(SMBUS_DATA_SIZE_ERROR);
                            }
                            else
                            {
                                //
                                // Update the size.
                                //
                                pSMBus->ucRxSize = ucDataTemp;

                                //
                                // Check to see if PEC is enabled.
                                //
                                if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
                                {
                                    //
                                    // Add the size byte to the CRC calculation.
                                    //
                                    pSMBus->ucCalculatedCRC =
                                        MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                      &ucDataTemp, 1);
                                }

                                //
                                // Update the state machine.
                                //
                                pSMBus->ucSlaveState = SMBUS_STATE_READ_NEXT;
                            }

                            //
                            // This state is done.
                            //
                            break;
                        }

                        //
                        // If there is no data to receive and no PEC, nothing
                        // to do.  Software should never get here.
                        //
                        if(pSMBus->ucRxIndex == pSMBus->ucRxSize)
                        {
                            //
                            // Update the state machine.
                            //
                            pSMBus->ucSlaveState = SMBUS_STATE_READ_DONE;

                            //
                            // Report an error.
                            //
                            return(SMBUS_SLAVE_ERROR);
                        }
                        else
                        {
                            //
                            // Put the data in the buffer.
                            //
                            pSMBus->pucRxBuffer[pSMBus->ucRxIndex++] =
                                ucDataTemp;

                            //
                            // If this is the last data byte.
                            //
                            if(pSMBus->ucRxIndex == pSMBus->ucRxSize)
                            {
                                //
                                // Check for PEC usage.
                                //
                                if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
                                {
                                    //
                                    // Add the size byte to the CRC
                                    // calculation.
                                    //
                                    pSMBus->ucCalculatedCRC =
                                        MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                      &ucDataTemp, 1);

                                    //
                                    // Update the state machine.
                                    //
                                    pSMBus->ucSlaveState =
                                        SMBUS_STATE_READ_PEC;
                                }
                                else
                                {
                                    //
                                    // Update the state machine.
                                    //
                                    pSMBus->ucSlaveState =
                                        SMBUS_STATE_READ_DONE;
                                }
                            }

                            //
                            // All other cases.
                            //
                            else
                            {
                                //
                                // Check for PEC usage.
                                //
                                if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
                                {
                                    //
                                    // Add the size byte to the CRC
                                    // calculation.
                                    //
                                    pSMBus->ucCalculatedCRC =
                                        MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                      &ucDataTemp, 1);
                                }

                                //
                                // Update the state machine.
                                //
                                pSMBus->ucSlaveState = SMBUS_STATE_READ_NEXT;
                            }
                        }

                        //
                        // Actions for this case are complete.
                        //
                        break;
                    }

                    //
                    // Read the next byte into the buffer.
                    //
                    case SMBUS_STATE_READ_NEXT:
                    {
                        //
                        // Read the data into the a temporary variable.
                        //
                        ucDataTemp = MAP_I2CSlaveDataGet(pSMBus->ulI2CBase);

                        //
                        // If there is no data to receive and no PEC, nothing
                        // to do.  Software should never get here.
                        //
                        if(pSMBus->ucRxIndex == pSMBus->ucRxSize)
                        {
                            //
                            // Update the state machine.
                            //
                            pSMBus->ucSlaveState = SMBUS_STATE_READ_DONE;

                            //
                            // Report an error.
                            //
                            return(SMBUS_SLAVE_ERROR);
                        }
                        else
                        {
                            //
                            // Put the data in the buffer.
                            //
                            pSMBus->pucRxBuffer[pSMBus->ucRxIndex++] =
                                ucDataTemp;

                            //
                            // If this is the last data byte.
                            //
                            if(pSMBus->ucRxIndex == pSMBus->ucRxSize)
                            {
                                //
                                // Check for PEC usage.
                                //
                                if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
                                {
                                    //
                                    // Add the size byte to the CRC
                                    // calculation.
                                    //
                                    pSMBus->ucCalculatedCRC =
                                        MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                      &ucDataTemp, 1);

                                    //
                                    // Update the state machine.
                                    //
                                    if(HWREGBITB(&pSMBus->usFlags,
                                                 FLAG_PROCESS_CALL))
                                    {
                                        pSMBus->ucSlaveState =
                                            SMBUS_STATE_READ_DONE;
                                    }
                                    else
                                    {
                                        pSMBus->ucSlaveState =
                                            SMBUS_STATE_READ_PEC;
                                    }
                                }
                                else
                                {
                                    //
                                    // Update the state machine.
                                    //
                                    pSMBus->ucSlaveState =
                                        SMBUS_STATE_READ_DONE;
                                }
                            }

                            //
                            // All other cases.
                            //
                            else
                            {
                                //
                                // Check for PEC usage.
                                //
                                if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
                                {
                                    //
                                    // Add the size byte to the CRC
                                    // calculation.
                                    //
                                    pSMBus->ucCalculatedCRC =
                                        MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                      &ucDataTemp, 1);
                                }

                                //
                                // Update the state machine.
                                //
                                pSMBus->ucSlaveState = SMBUS_STATE_READ_NEXT;
                            }
                        }

                        break;
                    }

                    //
                    // Read the PEC byte and compare it.
                    //
                    case SMBUS_STATE_READ_PEC:
                    {
                        //
                        // Read the data into the a temporary variable.
                        //
                        ucDataTemp = MAP_I2CSlaveDataGet(pSMBus->ulI2CBase);

                        //
                        // Compare PEC.
                        //
                        if(pSMBus->ucCalculatedCRC != ucDataTemp)
                        {
                            //
                            // Indicate PEC error.
                            //
                            return(SMBUS_PEC_ERROR);
                        }

                        //
                        // Update the state machine.
                        //
                        pSMBus->ucSlaveState = SMBUS_STATE_READ_DONE;

                        break;
                    }

                    //
                    // No more data to receive.  If we get here, read data
                    // into a dummy variable and NACK.
                    //
                    case SMBUS_STATE_READ_DONE:
                    {
                        //
                        // Read the data into the a temporary variable.
                        //
                        ucDataTemp = MAP_I2CSlaveDataGet(pSMBus->ulI2CBase);

                        //
                        // Report an error.
                        //
                        return(SMBUS_SLAVE_ERROR);
                    }
                }

                //
                // Actions for this case are complete.
                //
                break;
            }

            //
            // The master has requested that the slave transmit data back to
            // master.
            //
            case I2C_SLAVE_ACT_TREQ:
            {
                //
                // Initialize temporary variable that stores transmit byte to
                // 0xff.  If data is not set by another condition, the 0xff
                // carries through.  This happens if ucTxIndex is equal to or
                // greater than ucTxSize.
                //
                ucDataTemp = 0xff;

                //
                // Determine what to do based on the current state.
                //
                switch(pSMBus->ucSlaveState)
                {
                    //
                    // The state machine is currently idle, or if the last
                    // state was SMBUS_STATE_SLAVE_POST_COMMAND or
                    // SMBUS_READ_DONE, this is the first byte transmitted.  In
                    // the case of slave post command, this means that the
                    // command was received followed by a repeated start (with
                    // R/S = 1).  In the case of read next, this means that a
                    // raw I2C master transmit changed direction with a
                    // repeated start and is now a master receive.  In the case
                    // of read done, this means that a previous master transmit
                    // was finished (non-command followed by a repeated start).
                    //
                    case SMBUS_STATE_IDLE:
                    case SMBUS_STATE_SLAVE_POST_COMMAND:
                    case SMBUS_STATE_READ_NEXT:
                    case SMBUS_STATE_READ_DONE:
                    {
                        //
                        // Check which slave address was called out.  Set the
                        // active address to the matched address.
                        //
                        if(MAP_I2CSlaveStatus(pSMBus->ulI2CBase) &
                           I2C_SCSR_OAR2SEL)
                        {
                            pSMBus->ucOwnSlaveAddress =
                                (HWREG(pSMBus->ulI2CBase + I2C_O_SOAR2) &
                                 0x7f);
                        }
                        else
                        {
                            pSMBus->ucOwnSlaveAddress =
                                HWREG(pSMBus->ulI2CBase + I2C_O_SOAR);
                        }

                        //
                        // Check to see if the TX buffer is populated.  If not,
                        // return not ready without writing to the data
                        // register.
                        //
                        if(pSMBus->ucTxSize == 0)
                        {
                            return(SMBUS_SLAVE_NOT_READY);
                        }

                        //
                        // Is this a block transfer?
                        //
                        if(HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER))
                        {
                            //
                            // The first byte to send is the size.
                            //
                            ucDataTemp = pSMBus->ucTxSize;
                        }
                        else
                        {
                            //
                            // Is there data to send?
                            //
                            if(pSMBus->ucTxIndex < pSMBus->ucTxSize)
                            {
                                //
                                // Set the transmit data to the next item in
                                // the buffer.
                                //
                                ucDataTemp =
                                    pSMBus->pucTxBuffer[pSMBus->ucTxIndex++];
                            }
                            else
                            {
                                //
                                // Send 0xff per spec.
                                //
                                ucDataTemp = 0xff;
                            }
                        }

                        //
                        // Check to see if PEC is required.
                        //
                        if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
                        {
                            //
                            // Start calculating the CRC with the address.
                            //
                            ucCRCTemp = (pSMBus->ucOwnSlaveAddress << 1) | 1;

                            //
                            // Add the address and R/S bit to the CRC.
                            //
                            pSMBus->ucCalculatedCRC =
                                MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                              &ucCRCTemp, 1);

                            //
                            // Add the data byte to the CRC calculation.
                            //
                            pSMBus->ucCalculatedCRC =
                                MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                              &ucDataTemp, 1);

                            //
                            // Move to the next state.
                            //
                            if(pSMBus->ucTxIndex == pSMBus->ucTxSize)
                            {
                                //
                                // Final byte is the CRC byte.
                                //
                                pSMBus->ucSlaveState = SMBUS_STATE_WRITE_FINAL;
                            }
                            else
                            {
                                //
                                // All other cases, move to the next byte
                                // state.
                                //
                                pSMBus->ucSlaveState = SMBUS_STATE_WRITE_NEXT;
                            }
                        }
                        else
                        {
                            //
                            // Move to the next state.
                            //
                            switch(pSMBus->ucTxSize - pSMBus->ucTxIndex)
                            {
                                //
                                // If all of the data has been sent, move to
                                // the done state.
                                //
                                case 0:
                                {
                                    pSMBus->ucSlaveState =
                                        SMBUS_STATE_WRITE_DONE;

                                    break;
                                }

                                //
                                // If 1 left, move to the final byte state.
                                //
                                case 1:
                                {
                                    pSMBus->ucSlaveState =
                                        SMBUS_STATE_WRITE_FINAL;

                                    break;
                                }

                                //
                                // All other cases, move to the next byte
                                // state.
                                //
                                default:
                                {
                                    pSMBus->ucSlaveState =
                                        SMBUS_STATE_WRITE_NEXT;

                                    break;
                                }
                            }
                        }

                        //
                        // Send the data.
                        //
                        MAP_I2CSlaveDataPut(pSMBus->ulI2CBase, ucDataTemp);

                        //
                        // This state is done.
                        //
                        break;
                    }

                    //
                    // The first byte has already been sent, handle the rest.
                    //
                    case SMBUS_STATE_WRITE_NEXT:
                    {
                        //
                        // Set the transmit data to the next item in the
                        // buffer.
                        //
                        ucDataTemp = pSMBus->pucTxBuffer[pSMBus->ucTxIndex++];

                        //
                        // Check to see if PEC is required.
                        //
                        if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
                        {
                            //
                            // Add the byte to the CRC calculation.
                            //
                            pSMBus->ucCalculatedCRC =
                                MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                              &ucDataTemp, 1);

                            //
                            // Check if it's time to move to the next state.
                            //
                            if(pSMBus->ucTxIndex == pSMBus->ucTxSize)
                            {
                                //
                                // Final byte is the CRC byte.
                                //
                                pSMBus->ucSlaveState = SMBUS_STATE_WRITE_FINAL;
                            }
                        }
                        else
                        {
                            //
                            // Move to the next state.
                            //
                            if((pSMBus->ucTxSize - pSMBus->ucTxIndex) == 1)
                            {
                                //
                                // If only 1 byte remains, move to the final
                                // state.
                                //
                                pSMBus->ucSlaveState = SMBUS_STATE_WRITE_FINAL;
                            }
                        }

                        //
                        // Send the data.
                        //
                        MAP_I2CSlaveDataPut(pSMBus->ulI2CBase, ucDataTemp);

                        //
                        // This state is done.
                        //
                        break;
                    }

                    //
                    // Write the final byte, whether PEC or data.
                    //
                    case SMBUS_STATE_WRITE_FINAL:
                    {
                        //
                        // Check to see if PEC is required.
                        //
                        if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
                        {
                            //
                            // Send the CRC byte.
                            //
                            ucDataTemp = pSMBus->ucCalculatedCRC;
                        }
                        else
                        {
                            //
                            // Send the last data byte.
                            //
                            ucDataTemp =
                                pSMBus->pucTxBuffer[pSMBus->ucTxIndex++];
                        }

                        //
                        // Send the data.
                        //
                        MAP_I2CSlaveDataPut(pSMBus->ulI2CBase, ucDataTemp);

                        //
                        // Move to the write done state.
                        //
                        pSMBus->ucSlaveState = SMBUS_STATE_WRITE_DONE;

                        //
                        // This state is done.
                        //
                        break;
                    }

                    //
                    // All data has been sent, send 0xff.
                    //
                    case SMBUS_STATE_WRITE_DONE:
                    {
                        //
                        // Send 0xff because there is no more data to send.
                        //
                        MAP_I2CSlaveDataPut(pSMBus->ulI2CBase, 0xff);

                        //
                        // This state is done.
                        //
                        break;
                    }
                }

                //
                // Actions for this case are complete.
                //
                break;
            }
        }

        //
        // Return OK status.
        //
        return(SMBUS_OK);
    }

    //
    // Return OK.  Should never get here.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Sends data outside of the interrupt processing function.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function sends data outside the interrupt processing function, and
//! should only be used when SMBusSlaveIntProcess() returns
//! \b SMBUS_SLAVE_NOT_READY.  At this point, the application should set up the
//! transfer and call this function (it assumes that the transmit buffer has
//! already been populated when called).  When called, this function updates
//! the slave state machine as if SMBusSlaveIntProcess() were called.
//!
//! \return Returns \b SMBUS_SLAVE_NOT_READY if the slave's transmit buffer is
//! not yet initialized (ucTxSize is 0), or \b SMBUS_OK if processing finished
//! successfully.
//
//*****************************************************************************
tSMBusStatus
SMBusSlaveDataSend(tSMBus *pSMBus)
{
    unsigned char ucCRCTemp;
    unsigned char ucDataTemp;

    //
    // Check to see if the TX buffer is populated.  If not,
    // return not ready without writing to the data register.
    //
    if(pSMBus->ucTxSize == 0)
    {
        return(SMBUS_SLAVE_NOT_READY);
    }

    //
    // Is this a block transfer?
    //
    if(HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER))
    {
        //
        // The first byte to send is the size.
        //
        ucDataTemp = pSMBus->ucTxSize;
    }
    else
    {
        //
        // Is there data to send?
        //
        if(pSMBus->ucTxIndex < pSMBus->ucTxSize)
        {
            //
            // Set the transmit data to the next item in
            // the buffer.
            //
            ucDataTemp = pSMBus->pucTxBuffer[pSMBus->ucTxIndex++];
        }
        else
        {
            //
            // Send 0xff per spec.  Should not get here.
            //
            ucDataTemp = 0xff;
        }
    }

    //
    // Check to see if PEC is required.
    //
    if(HWREGBITB(&pSMBus->usFlags, FLAG_PEC))
    {
        //
        // Start calculating the CRC with the address.
        //
        ucCRCTemp = (pSMBus->ucOwnSlaveAddress << 1) | 1;

        //
        // Add the address and R/S bit to the CRC.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                &ucCRCTemp, 1);

        //
        // Add the data byte to the CRC calculation.
        //
        pSMBus->ucCalculatedCRC = MAP_Crc8CCITT(pSMBus->ucCalculatedCRC,
                                                &ucDataTemp, 1);

        //
        // Move to the next state.
        //
        if(pSMBus->ucTxIndex == pSMBus->ucTxSize)
        {
            //
            // Final byte is the CRC byte.
            //
            pSMBus->ucSlaveState = SMBUS_STATE_WRITE_FINAL;
        }
        else
        {
            //
            // All other cases, move to the next byte state.
            //
            pSMBus->ucSlaveState = SMBUS_STATE_WRITE_NEXT;
        }
    }
    else
    {
        //
        // Move to the next state.
        //
        switch(pSMBus->ucTxSize - pSMBus->ucTxIndex)
        {
            //
            // If all of the data has been sent, move to the
            // done state.
            //
            case 0:
            {
                pSMBus->ucSlaveState = SMBUS_STATE_WRITE_DONE;

                break;
            }

            //
            // If 1 left, move to the final byte state.
            //
            case 1:
            {
                pSMBus->ucSlaveState = SMBUS_STATE_WRITE_FINAL;

                break;
            }

            //
            // All other cases, move to the next byte state.
            //
            default:
            {
                pSMBus->ucSlaveState = SMBUS_STATE_WRITE_NEXT;

                break;
            }
        }
    }

    //
    // Send the data.
    //
    MAP_I2CSlaveDataPut(pSMBus->ulI2CBase, ucDataTemp);

    //
    // Return to caller.
    //
    return(SMBUS_OK);
}

//*****************************************************************************
//
//! Set the address and size of the slave transmit buffer.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param pucData is a pointer to the transmit data buffer.
//! \param ucSize is the number of bytes in the buffer.
//!
//! This function sets the address and size of the slave transmit buffer.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveTxBufferSet(tSMBus *pSMBus, unsigned char *pucData,
                      unsigned char ucSize)
{
    //
    // Set the trasmit buffer.
    //
    pSMBus->pucTxBuffer = pucData;

    //
    // Set the size.
    //
    pSMBus->ucTxSize = ucSize;
}

//*****************************************************************************
//
//! Set the address and size of the slave receive buffer.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param pucData is a pointer to the receive data buffer.
//! \param ucSize is the number of bytes in the buffer.
//!
//! This function sets the address and size of the slave receive buffer.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveRxBufferSet(tSMBus *pSMBus, unsigned char *pucData,
                      unsigned char ucSize)
{
    //
    // Set the receive buffer.
    //
    pSMBus->pucRxBuffer = pucData;

    //
    // Set the size.
    //
    pSMBus->ucRxSize = ucSize;
}

//*****************************************************************************
//
//! Get the current command byte.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! Returns the current value of the ucCurrentCommand variable in the SMBus
//! configuration structure.  This can be used to help the user application
//! set up the SMBus slave transmit and receive buffers.
//!
//! \return None.
//
//*****************************************************************************
unsigned char
SMBusSlaveCommandGet(tSMBus *pSMBus)
{
    //
    // Return the current command.
    //
    return(pSMBus->ucCurrentCommand);
}

//*****************************************************************************
//
//! Sets the process call flag for an SMBus slave transfer.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! Sets the process call flag in the configuration structure so that the SMBus
//! slave can respond correctly to a Process Call request.  This flag must be
//! set prior to the data portion of the packet.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveProcessCallEnable(tSMBus *pSMBus)
{
    //
    // Set the block transfer flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 1;
}

//*****************************************************************************
//
//! Clears the process call flag for an SMBus slave transfer.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! Clears the process call flag in the configuration structure.  The user
//! application can either call this function to clear the flag, or use
//! SMBusSlaveTransferInit() to clear out all transfer-specific flags.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveProcessCallDisable(tSMBus *pSMBus)
{
    //
    // Clear the block transfer flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 0;
}

//*****************************************************************************
//
//! Sets the block transfer flag for an SMBus slave transfer.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! Sets the block transfer flag in the configuration structure so that the
//! SMBus slave can respond correctly to a Block Write or Block Read request.
//! This flag must be set prior to the data portion of the packet.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveBlockTransferEnable(tSMBus *pSMBus)
{
    //
    // Set the block transfer flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 1;
}

//*****************************************************************************
//
//! Clears the block transfer flag for an SMBus slave transfer.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! Clears the block transfer flag in the configuration structure.  The user
//! application can either call this function to clear the flag, or use
//! SMBusSlaveTransferInit() to clear out all transfer-specific flags.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveBlockTransferDisable(tSMBus *pSMBus)
{
    //
    // Clear the block transfer flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 0;
}

//*****************************************************************************
//
//! Sets the ``raw'' I2C flag for an SMBus slave transfer.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! Sets the raw I2C flag in the configuration structure so that the
//! SMBus slave can respond correctly to raw I2C (non-SMBus protocol) requests.
//! This flag must be set prior to the transfer, and is a global setting.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveI2CEnable(tSMBus *pSMBus)
{
    //
    // Set the block transfer flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 1;
}

//*****************************************************************************
//
//! Clears the ``raw'' I2C flag for an SMBus slave transfer.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! Clears the raw I2C flag in the configuration structure. This flag is a
//! global setting similar to the PEC flag and cannot be cleared using
//! SMBusSlaveTransferInit().
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveI2CDisable(tSMBus *pSMBus)
{
    //
    // Clear the block transfer flag.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_RAW_I2C) = 0;
}

//*****************************************************************************
//
//! Sets the value of the AR (Address Resolved) flag.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param bValue is the value to set the flag.
//!
//! This function allows the application to set the value of the AR flag.  All
//! SMBus slaves must support the AR and AV flags.  On POR, the AR flag is
//! cleared.  It is also cleared when a slave receives the ARP Reset Device
//! command.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveARPFlagARSet(tSMBus *pSMBus, tBoolean bValue)
{
    //
    // Set the block address resolved flag to the desired value.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_ADDRESS_RESOLVED) = bValue;
}

//*****************************************************************************
//
//! Returns the current value of the AR (Address Resolved) flag.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This returns the value of the AR (Address Resolved) flag.
//!
//! \return Returns \b true if set, \b false if cleared.
//
//*****************************************************************************
tBoolean
SMBusSlaveARPFlagARGet(tSMBus *pSMBus)
{
    //
    // Get the value of the block address resolved flag.
    //
    return(HWREGBITB(&pSMBus->usFlags, FLAG_ADDRESS_RESOLVED));
}

//*****************************************************************************
//
//! Sets the value of the AV (Address Valid) flag.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param bValue is the value to set the flag.
//!
//! This function allows the application to set the value of the AV flag.  All
//! SMBus slaves must support the AR and AV flags.  On POR, the AV flag is
//! cleared.  It is also cleared when a slave receives the ARP Reset Device
//! command.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveARPFlagAVSet(tSMBus *pSMBus, tBoolean bValue)
{
    //
    // Set the block address valid flag to the desired value.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_ADDRESS_VALID) = bValue;
}

//*****************************************************************************
//
//! Returns the current value of the AV (Address Valid) flag.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This returns the value of the AV (Address Valid) flag.
//!
//! \return Returns \b true if set, or \b false if cleared.
//
//*****************************************************************************
tBoolean
SMBusSlaveARPFlagAVGet(tSMBus *pSMBus)
{
    //
    // Get the value of the block address valid flag.
    //
    return(HWREGBITB(&pSMBus->usFlags, FLAG_ADDRESS_VALID));
}

//*****************************************************************************
//
//! Sets up the SMBus slave for a new transfer.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function is used to re-initialize the configuration structure for a
//! new transfer.  Once a transfer is complete and the data has been processed,
//! unused flags, states, the data buffers and buffer indexes should be reset
//! to a known state before a new transfer.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveTransferInit(tSMBus *pSMBus)
{
    //
    // Clear the block transfer, process call and transfer in progress flags.
    //
    HWREGBITB(&pSMBus->usFlags, FLAG_BLOCK_TRANSFER) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_PROCESS_CALL) = 0;
    HWREGBITB(&pSMBus->usFlags, FLAG_TRANSFER_IN_PROGRESS) = 0;

    //
    // Set the configuration structure to a known, zeroed state.
    //
    pSMBus->ucMasterState = SMBUS_STATE_IDLE;
    pSMBus->ucSlaveState = SMBUS_STATE_IDLE;
    pSMBus->ucCurrentCommand = 0;
    pSMBus->ucCalculatedCRC = 0;
    pSMBus->ucTxSize = 0;
    pSMBus->ucTxIndex = 0;
    pSMBus->ucRxSize = 0;
    pSMBus->ucRxIndex = 0;
}

//*****************************************************************************
//
//! Sets the value of the ACK bit when using manual acknowledgement.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param bACK specifies whether to ACK (\b true) or NACK (\b false).
//!
//! This function sets the value of the ACK bit.  In order for the ACK bit to
//! take effect, manual acknowledgement must be enabled on the slave using
//! SMBusSlaveManualACKEnable().
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveACKSend(tSMBus *pSMBus, tBoolean bACK)
{
    //
    // Send ACK or NACK based on the value of bACK.
    //
    if(bACK)
    {
        MAP_I2CSlaveACKValueSet(pSMBus->ulI2CBase, true);
    }
    else
    {
        MAP_I2CSlaveACKValueSet(pSMBus->ulI2CBase, false);
    }
}

//*****************************************************************************
//
//! Enables manual acknowledgement for the SMBus slave.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function enables manual acknowledge capability in the slave.  If the
//! application requires that the slave NACK on a bad command or a bad PEC
//! calculation, manual acknowledgement allows this to happen.
//!
//! In the case of responding to a bad command with a NACK, the application
//! should use SMBusSlaveSendACK() to ACK/NACK the command.   The slave ISR
//! should check for the SMBUS_SLAVE_FIRST_BYTE return code from
//! SMBusSlaveISRProcess() and ACK/NACK accordingly.  All other cases should be
//! handled in the application based on the return code of
//! SMBusSlaveISRProcess().
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveManualACKEnable(tSMBus *pSMBus)
{
    //
    // Enable manual acknowledge.
    //
    MAP_I2CSlaveACKOverride(pSMBus->ulI2CBase, true);
}

//*****************************************************************************
//
//! Disables manual acknowledgement for the SMBus slave.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function disables manual acknowledge capability in the slave.  When
//! manual acknowledgement is disabled, the slave automatically ACKs every
//! byte sent by the master.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveManualACKDisable(tSMBus *pSMBus)
{
    //
    // Disable manual acknowledge.
    //
    MAP_I2CSlaveACKOverride(pSMBus->ulI2CBase, false);
}

//*****************************************************************************
//
//! Returns the manual acknowledgement status of the SMBus slave.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function returns the state of the I2C ACKOEN bit in the I2CSACKCTL
//! register.  This feature is disabled out of reset and must be enabled
//! using SMBusSlaveManualACKEnable().
//!
//! \return Returns \b true if manual acknowledge is enabled, or \b false if
//! manual acknowledge is disabled.
//
//*****************************************************************************
tBoolean
SMBusSlaveManualACKStatusGet(tSMBus *pSMBus)
{
    //
    // Return the value of the bit.
    //
    return(HWREG(pSMBus->ulI2CBase + I2C_O_SACKCTL) & 0x1);
}

//*****************************************************************************
//
//! Determine whether primary or secondary slave address has been requested by
//! the master.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! Tells the caller whether the I2C slave address requested by the master or
//! SMBus Host is the primary or secondary I2C slave address of the peripheral.
//! The primary is defined as the address programmed into I2CSOAR, and the
//! secondary as the address programmed into I2CSOAR2.
//!
//! \return Returns \b SMBUS_SLAVE_ADDR_PRIMARY if the primary address is
//! called out or \b SMBUS_SLAVE_ADDR_SECONDARY if the secondary address is
//! called out.
//
//*****************************************************************************
tSMBusStatus
SMBusSlaveIntAddressGet(tSMBus *pSMBus)
{
    //
    // Determine whether the primary or secondary address was called out.
    //
    if(MAP_I2CSlaveStatus(pSMBus->ulI2CBase) & I2C_SCSR_OAR2SEL)
    {
        return(SMBUS_SLAVE_ADDR_SECONDARY);
    }
    else
    {
        return(SMBUS_SLAVE_ADDR_PRIMARY);
    }
}

//*****************************************************************************
//
//! Enables the appropriate slave interrupts for stack processing.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//!
//! This function enables the I2C interrupts used by the SMBus slave.  Both
//! the peripheral-level and NVIC-level interrupts are enabled.
//! SMBusSlaveInit() must be called before this function because this function
//! relies on the I2C base address being defined.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveIntEnable(tSMBus *pSMBus)
{
    //
    // Enable the slave interrupts.
    //
    MAP_I2CSlaveIntEnableEx(pSMBus->ulI2CBase, I2C_SLAVE_INT_DATA |
                            I2C_SLAVE_INT_STOP);

    //
    // Enable the interrupt in the NVIC.
    //
    switch(pSMBus->ulI2CBase)
    {
        case I2C0_SLAVE_BASE:
        {
            MAP_IntEnable(INT_I2C0);
            break;
        }

        case I2C1_SLAVE_BASE:
        {
            MAP_IntEnable(INT_I2C1);
            break;
        }

        case I2C2_SLAVE_BASE:
        {
            MAP_IntEnable(INT_I2C2);
            break;
        }

        case I2C3_SLAVE_BASE:
        {
            MAP_IntEnable(INT_I2C3);
            break;
        }

        case I2C4_SLAVE_BASE:
        {
            MAP_IntEnable(INT_I2C4);
            break;
        }

        case I2C5_SLAVE_BASE:
        {
            MAP_IntEnable(INT_I2C5);
            break;
        }
    }
}

//*****************************************************************************
//
//! Sets the slave address for an SMBus slave peripheral.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ucAddressNum specifies which address (primary or secondary)
//! \param ucSlaveAddress is the address of the slave.
//!
//! This function sets the slave address.  Both the primary and secondary
//! addresses can be set using this function.  To set the primary address
//! (stored in I2CSOAR), ucAddressNum should be '0'.  To set the secondary
//! address (stored in I2CSOAR2), ucAddressNum should be '1'.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveAddressSet(tSMBus *pSMBus, unsigned char ucAddressNum,
                     unsigned char ucSlaveAddress)
{
    //
    // Write the slave address.
    //
    MAP_I2CSlaveAddressSet(pSMBus->ulI2CBase, ucAddressNum, ucSlaveAddress);
}

//*****************************************************************************
//
//! Sets a slave's UDID structure.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param pUDID is a pointer to the UDID configuration for the slave.  This
//! is only needed if the slave is on a bus that uses ARP.
//!
//! This function sets the UDID for a slave instance.
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveUDIDSet(tSMBus *pSMBus, tSMBusUDID *pUDID)
{
    pSMBus->pUDID = pUDID;
}

//*****************************************************************************
//
//! Initializes an I2C slave peripheral for SMBus functionality.
//!
//! \param pSMBus specifies the SMBus configuration structure.
//! \param ulI2CBase specifies the base address of the I2C slave peripheral.
//!
//! This function initializes an I2C peripheral for SMBus slave use.  The
//! instance-specific configuration structure is initialized to a set of known
//! values and the I2C peripheral is configured based on the input arguments.
//!
//! The default configuration of the SMBus slave uses automatic
//! acknowledgement.  If manual acknowledgement is required, call
//! SMBusSlaveManualACKEnable().
//!
//! \return None.
//
//*****************************************************************************
void
SMBusSlaveInit(tSMBus *pSMBus, unsigned long ulI2CBase)
{
    //
    // Initialize the configuration structure.
    //
    pSMBus->pUDID = 0;
    pSMBus->ulI2CBase = ulI2CBase;
    pSMBus->usFlags = 0;
    pSMBus->ucMasterState = SMBUS_STATE_IDLE;
    pSMBus->ucSlaveState = SMBUS_STATE_IDLE;
    pSMBus->ucOwnSlaveAddress = 0;
    pSMBus->ucTargetSlaveAddress = 0;
    pSMBus->ucCurrentCommand = 0;
    pSMBus->ucCalculatedCRC = 0;
    pSMBus->ucTxSize = 0;
    pSMBus->ucTxIndex = 0;
    pSMBus->ucRxSize = 0;
    pSMBus->ucRxIndex = 0;

    //
    // Enable the I2C slave module.  The slave is always enabled because the
    // SMBus spec requires that all devices respond whne their slave address
    // is put on the bus.
    //
    MAP_I2CSlaveEnable(pSMBus->ulI2CBase);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
