//*****************************************************************************
//
// bl_can.c - Functions to transfer data via the CAN port.
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
// This is part of revision 9453 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#include "inc/hw_can.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_flash.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "bl_config.h"
#include "boot_loader/bl_can.h"
#include "boot_loader/bl_can_timing.h"
#include "boot_loader/bl_check.h"
#include "boot_loader/bl_crystal.h"
#include "boot_loader/bl_flash.h"
#include "boot_loader/bl_hooks.h"
#include "boot_loader/bl_uart.h"

//*****************************************************************************
//
//! \addtogroup bl_can_api
//! @{
//
//*****************************************************************************
#if defined(CAN_ENABLE_UPDATE) || defined(DOXYGEN)

//*****************************************************************************
//
// If CAN_REQUIRES_PLL is defined, make sure that CRYSTAL_FREQ is also defined.
//
//*****************************************************************************
#if defined(CAN_REQUIRES_PLL) && !defined(CRYSTAL_FREQ)
#error ERROR: CRYSTAL_FREQ must be defined when CAN_REQUIRES_PLL is defined!
#endif

//*****************************************************************************
//
// The results that can be returned by the CAN APIs.
//
//*****************************************************************************
#define CAN_CMD_SUCCESS         0x00
#define CAN_CMD_FAIL            0x01

//*****************************************************************************
//
// Macros used to generate correct pin definitions.
//
//*****************************************************************************
#define CAN_RX_PIN_M            (1 << CAN_RX_PIN)
#define CAN_TX_PIN_M            (1 << CAN_TX_PIN)

//*****************************************************************************
//
// These functions are used on parts that require the work around for the CAN
// controller errata, while parts that do not will use the direct access to
// the registers.
//
//*****************************************************************************
#ifdef CAN_REQUIRES_PLL
#define CAN_RW_DELAY            5
#else
#define CANRegWrite(ulAddress, ulValue)                                     \
                                HWREG(ulAddress) = ulValue

#define CANRegRead(ulAddress)                                               \
                                HWREG(ulAddress)
#endif

//*****************************************************************************
//
// The message object number and index to the local message object memory to
// use when accessing the messages.
//
//*****************************************************************************
#define MSG_OBJ_BCAST_RX_ID     1
#define MSG_OBJ_BCAST_TX_ID     2

//*****************************************************************************
//
// A prototype for the function (in the startup code) for calling the
// application.
//
//*****************************************************************************
extern void StartApplication(void);

//*****************************************************************************
//
// A prototype for the function (in the startup code) for a predictable length
// delay.
//
//*****************************************************************************
extern void Delay(unsigned long ulCount);

//*****************************************************************************
//
// Holds the current address to write to when data is received via the Send
// Data Command.
//
//*****************************************************************************
static unsigned long g_ulTransferAddress;

//*****************************************************************************
//
// Holds the remaining bytes expected to be received.
//
//*****************************************************************************
static unsigned long g_ulTransferSize;

//*****************************************************************************
//
// The buffer used to receive data from the update.
//
//*****************************************************************************
static unsigned char g_pucCommandBuffer[8];

//*****************************************************************************
//
// These globals are used to store the first two words to prevent a partial
// image from being booted.
//
//*****************************************************************************
static unsigned long g_ulStartValues[2];
static unsigned long g_ulStartSize;
static unsigned long g_ulStartAddress;

//*****************************************************************************
//
// The active interface when the UART bridge is enabled.
//
//*****************************************************************************
#ifdef CAN_UART_BRIDGE
static unsigned long g_ulInterface;
#define IFACE_UNKNOWN           0
#define IFACE_CAN               1
#define IFACE_UART              2
#endif

//*****************************************************************************
//
//! \internal
//!
//! Reads a CAN controller register.
//!
//! \param ulRegAddress is the full address of the CAN register to be read.
//!
//! This function performs the necessary synchronization to read from a CAN
//! controller register.
//!
//! \note This function provides the delay required to access CAN registers.
//! This delay is required when accessing CAN registers directly.
//!
//! \return Returns the value read from the register.
//
//*****************************************************************************
#ifdef CAN_REQUIRES_PLL
static unsigned long
CANRegRead(unsigned long ulRegAddress)
{
    volatile int iDelay;
    unsigned long ulRetVal;

    //
    // Trigger the inital read to the CAN controller.  The value returned at
    // this point is not valid.
    //
    HWREG(ulRegAddress);

    //
    // This delay is necessary for the CAN have the correct data on the bus.
    //
    for(iDelay = 0; iDelay < CAN_RW_DELAY; iDelay++)
    {
    }

    //
    // Do the final read that has the valid value of the register.
    //
    ulRetVal = HWREG(ulRegAddress);

    return(ulRetVal);
}
#endif

//*****************************************************************************
//
//! \internal
//!
//! Writes a CAN controller register.
//!
//! \param ulRegAddress is the full address of the CAN register to be written.
//! \param ulRegValue is the value to write into the register specified by
//! \e ulRegAddress.
//!
//! This function takes care of the synchronization necessary to write to a
//! CAN controller register.
//!
//! \note The delays in this function are required when accessing CAN registers
//! directly.
//!
//! \return None.
//
//*****************************************************************************
#ifdef CAN_REQUIRES_PLL
static void
CANRegWrite(unsigned long ulRegAddress, unsigned long ulRegValue)
{
    volatile int iDelay;

    //
    // Trigger the inital write to the CAN controller.  The value will not make
    // it out to the CAN controller for CAN_RW_DELAY cycles.
    //
    HWREG(ulRegAddress) = ulRegValue;

    //
    // Delay to allow the CAN controller to receive the new data.
    //
    for(iDelay = 0; iDelay < CAN_RW_DELAY; iDelay++)
    {
    }
}
#endif

//*****************************************************************************
//
//! Initializes the CAN controller after reset.
//!
//! After reset, the CAN controller is left in the disabled state.  However,
//! the memory used for message objects contains undefined values and must be
//! cleared prior to enabling the CAN controller the first time.  This prevents
//! unwanted transmission or reception of data before the message objects are
//! configured.  This function must be called before enabling the controller
//! the first time.
//!
//! \return None.
//
//*****************************************************************************
static void
CANInit(void)
{
    int iMsg;

    //
    // Place CAN controller in init state, regardless of previous state.  This
    // will put the controller in idle, and allow the message object RAM to be
    // programmed.
    //
    CANRegWrite(CAN0_BASE + CAN_O_CTL, CAN_CTL_INIT | CAN_CTL_CCE);

    //
    // Loop through to program all 32 message objects
    //
    for(iMsg = 1; iMsg <= 32; iMsg++)
    {
        //
        // Wait for busy bit to clear.
        //
        while(CANRegRead(CAN0_BASE + CAN_O_IF1CRQ) & CAN_IF1CRQ_BUSY)
        {
        }

        //
        // Clear the message value bit in the arbitration register.  This
        // indicates the message is not valid and is a "safe" condition to
        // leave the message object.
        //
        CANRegWrite(CAN0_BASE + CAN_O_IF1CMSK,
                    CAN_IF1CMSK_WRNRD | CAN_IF1CMSK_ARB | CAN_IF1CMSK_CONTROL);
        CANRegWrite(CAN0_BASE + CAN_O_IF1ARB2, 0);
        CANRegWrite(CAN0_BASE + CAN_O_IF1MCTL, 0);

        //
        // Initiate programming of the message object
        //
        CANRegWrite(CAN0_BASE + CAN_O_IF1CRQ, iMsg);
    }

    //
    // Acknowledge any pending status interrupts.
    //
    CANRegRead(CAN0_BASE + CAN_O_STS);
}

//*****************************************************************************
//
//! This function configures the message object used to receive commands.
//!
//! This function configures the message object used to receive all firmware
//! update messages.  This will not actually read the data from the message it
//! is used to prepare the message object to receive the data when it is sent.
//!
//! \return None.
//
//*****************************************************************************
static void
CANMessageSetRx(void)
{
    unsigned short usCmdMaskReg;
    unsigned short usMaskReg[2];
    unsigned short usArbReg[2];
    unsigned short usMsgCtrl;

    //
    // Wait for busy bit to clear
    //
    while(CANRegRead(CAN0_BASE + CAN_O_IF1CRQ) & CAN_IF1CRQ_BUSY)
    {
    }

    //
    // This is always a write to the Message object as this call is setting a
    // message object.  This call will also always set all size bits so it sets
    // both data bits.  The call will use the CONTROL register to set control
    // bits so this bit needs to be set as well.
    //
    // Set the MASK bit so that this gets transferred to the Message Object.
    // Set the Arb bit so that this gets transferred to the Message object.
    //
    usCmdMaskReg = (CAN_IF1CMSK_WRNRD | CAN_IF1CMSK_DATAA | CAN_IF1CMSK_DATAB |
                    CAN_IF1CMSK_CONTROL | CAN_IF1CMSK_MASK | CAN_IF1CMSK_ARB);

    //
    // Set the UMASK bit to enable using the mask register.
    // Set the data length since this is set for all transfers.  This is also a
    // single transfer and not a FIFO transfer so set EOB bit.
    //
    usMsgCtrl = CAN_IF1MCTL_UMASK | CAN_IF1MCTL_EOB;

    //
    // Configure the Mask Registers.
    //
    //
    // Set the 29 bits of Identifier mask that were requested.
    //
    usMaskReg[0] = (unsigned short)LM_API_UPD;

    //
    // If the caller wants to filter on the extended ID bit then set it.
    //
    usMaskReg[1] =
        (unsigned short)(CAN_IF1MSK2_MXTD | (LM_API_UPD >> 16));

    //
    // Set the 29 bit version of the Identifier for this message object.
    // Mark the message as valid and set the extended ID bit.
    //
    usArbReg[0] = LM_API_UPD & CAN_IF1ARB1_ID_M;
    usArbReg[1] = ((LM_API_UPD >> 16) & CAN_IF1ARB2_ID_M) |
                   (CAN_IF1ARB2_MSGVAL | CAN_IF1ARB2_XTD);

    //
    // Write out the registers to program the message object.
    //
    CANRegWrite(CAN0_BASE + CAN_O_IF1CMSK, usCmdMaskReg);
    CANRegWrite(CAN0_BASE + CAN_O_IF1MSK1, usMaskReg[0]);
    CANRegWrite(CAN0_BASE + CAN_O_IF1MSK2, usMaskReg[1]);
    CANRegWrite(CAN0_BASE + CAN_O_IF1ARB1, usArbReg[0]);
    CANRegWrite(CAN0_BASE + CAN_O_IF1ARB2, usArbReg[1]);
    CANRegWrite(CAN0_BASE + CAN_O_IF1MCTL, usMsgCtrl);

    //
    // Transfer the message object to the message object specific by
    // MSG_OBJ_BCAST_RX_ID.
    //
    CANRegWrite(CAN0_BASE + CAN_O_IF1CRQ,
                MSG_OBJ_BCAST_RX_ID & CAN_IF1CRQ_MNUM_M);
}

//*****************************************************************************
//
//! This function reads data from the receive message object.
//!
//! \param pucData is a pointer to the buffer to store the data read from the
//! CAN controller.
//! \param pulMsgID is a pointer to the ID that was received with the data.
//!
//! This function will reads and acknowledges the data read from the message
//! object used to receive all CAN firmware update messages.  It will also
//! return the message identifier as this holds the API number that was
//! attached to the data.  This message identifier should be one of the
//! LM_API_UPD_* definitions.
//!
//! \return The number of valid bytes returned in the \e pucData buffer or
//! 0xffffffff if data was overwritten in the buffer.
//
//*****************************************************************************
static unsigned long
CANMessageGetRx(unsigned char *pucData, unsigned long *pulMsgID)
{
    unsigned short usCmdMaskReg;
    unsigned short usArbReg0, usArbReg1;
    unsigned short usMsgCtrl;
    unsigned long ulBytes;
    unsigned short *pusData;

    //
    // This is always a read to the Message object as this call is setting a
    // message object.
    // Clear a pending interrupt and new data in a message object.
    //
    usCmdMaskReg = (CAN_IF2CMSK_DATAA | CAN_IF2CMSK_DATAB |
                    CAN_IF1CMSK_CONTROL | CAN_IF2CMSK_CLRINTPND |
                    CAN_IF2CMSK_ARB);

    //
    // Set up the request for data from the message object.
    //
    CANRegWrite(CAN0_BASE + CAN_O_IF2CMSK, usCmdMaskReg);

    //
    // Transfer the message object to the message object specific by
    // MSG_OBJ_BCAST_RX_ID.
    //
    CANRegWrite(CAN0_BASE + CAN_O_IF2CRQ,
                MSG_OBJ_BCAST_RX_ID & CAN_IF1CRQ_MNUM_M);

    //
    // Wait for busy bit to clear
    //
    while(CANRegRead(CAN0_BASE + CAN_O_IF2CRQ) & CAN_IF1CRQ_BUSY)
    {
    }

    //
    // Read out the IF Registers.
    //
    usArbReg0 = CANRegRead(CAN0_BASE + CAN_O_IF2ARB1);
    usArbReg1 = CANRegRead(CAN0_BASE + CAN_O_IF2ARB2);
    usMsgCtrl = CANRegRead(CAN0_BASE + CAN_O_IF2MCTL);

    //
    // Set the 29 bit version of the Identifier for this message object.
    //
    *pulMsgID = ((usArbReg1 & CAN_IF1ARB2_ID_M) << 16) | usArbReg0;

    //
    // See if there is new data available.
    //
    if((usMsgCtrl & (CAN_IF1MCTL_NEWDAT | CAN_IF1MCTL_MSGLST)) ==
        CAN_IF1MCTL_NEWDAT)
    {
        //
        // Get the amount of data needed to be read.
        //
        ulBytes = usMsgCtrl & CAN_IF1MCTL_DLC_M;

        //
        // Read out the data from the CAN registers 16 bits at a time.
        //
        pusData = (unsigned short *)pucData;

        pusData[0] = CANRegRead(CAN0_BASE + CAN_O_IF2DA1);
        pusData[1] = CANRegRead(CAN0_BASE + CAN_O_IF2DA2);
        pusData[2] = CANRegRead(CAN0_BASE + CAN_O_IF2DB1);
        pusData[3] = CANRegRead(CAN0_BASE + CAN_O_IF2DB2);

        //
        // Now clear out the new data flag.
        //
        CANRegWrite(CAN0_BASE + CAN_O_IF2CMSK, CAN_IF1CMSK_NEWDAT);

        //
        // Transfer the message object to the message object specific by
        // MSG_OBJ_BCAST_RX_ID.
        //
        CANRegWrite(CAN0_BASE + CAN_O_IF2CRQ, MSG_OBJ_BCAST_RX_ID);

        //
        // Wait for busy bit to clear
        //
        while(CANRegRead(CAN0_BASE + CAN_O_IF2CRQ) & CAN_IF2CRQ_BUSY)
        {
        }
    }
    else
    {
        //
        // Data was lost so inform the caller.
        //
        ulBytes = 0xffffffff;
    }
    return(ulBytes);
}

//*****************************************************************************
//
//! This function sends data using the transmit message object.
//!
//! \param ulId is the ID to use with this message.
//! \param pucData is a pointer to the buffer with the data to be sent.
//! \param ulSize is the number of bytes to send and should not be more than
//! 8 bytes.
//!
//! This function will reads and acknowledges the data read from the message
//! object used to receive all CAN firmware update messages.  It will also
//! return the message identifier as this holds the API number that was
//! attached to the data.  This message identifier should be one of the
//! LM_API_UPD_* definitions.
//!
//! \return None.
//
//*****************************************************************************
static void
CANMessageSetTx(unsigned long ulId, const unsigned char *pucData,
                unsigned long ulSize)
{
    unsigned short usCmdMaskReg;
    unsigned short usArbReg0, usArbReg1;
    unsigned short usMsgCtrl;
    unsigned short *pusData;

    //
    // Wait for busy bit to clear
    //
    while(CANRegRead(CAN0_BASE + CAN_O_IF1CRQ) & CAN_IF1CRQ_BUSY)
    {
    }

    //
    // This is always a write to the Message object as this call is setting a
    // message object.  This call will also always set all size bits so it sets
    // both data bits.  The call will use the CONTROL register to set control
    // bits so this bit needs to be set as well.
    //
    usCmdMaskReg = (CAN_IF1CMSK_WRNRD | CAN_IF1CMSK_DATAA | CAN_IF1CMSK_DATAB |
                    CAN_IF1CMSK_CONTROL | CAN_IF1CMSK_ARB);

    //
    // Set the 29 bit version of the Identifier for this message object.
    //
    usArbReg0 = ulId & CAN_IF1ARB1_ID_M;

    //
    // Mark the message as valid and set the extended ID bit.
    //
    usArbReg1 = (((ulId >> 16) & CAN_IF1ARB2_ID_M) |
                 (CAN_IF1ARB2_DIR | CAN_IF1ARB2_MSGVAL | CAN_IF1ARB2_XTD));

    //
    // Set the TXRQST bit and the reset the rest of the register.
    // Set the data length since this is set for all transfers.  This is also a
    // single transfer and not a FIFO transfer so set EOB bit.
    //
    //
    usMsgCtrl = (CAN_IF1MCTL_TXRQST | CAN_IF1MCTL_EOB |
                 (ulSize & CAN_IF1MCTL_DLC_M));

    pusData = (unsigned short *)pucData;

    //
    // Write the data out to the CAN Data registers if needed.
    //
    CANRegWrite(CAN0_BASE + CAN_O_IF1DA1, pusData[0]);
    CANRegWrite(CAN0_BASE + CAN_O_IF1DA2, pusData[1]);
    CANRegWrite(CAN0_BASE + CAN_O_IF1DB1, pusData[2]);
    CANRegWrite(CAN0_BASE + CAN_O_IF1DB2, pusData[3]);

    //
    // Write out the registers to program the message object.
    //
    CANRegWrite(CAN0_BASE + CAN_O_IF1CMSK, usCmdMaskReg);
    CANRegWrite(CAN0_BASE + CAN_O_IF1ARB1, usArbReg0);
    CANRegWrite(CAN0_BASE + CAN_O_IF1ARB2, usArbReg1);
    CANRegWrite(CAN0_BASE + CAN_O_IF1MCTL, usMsgCtrl);

    //
    // Transfer the message object to the message object specifiec by
    // MSG_OBJ_BCAST_RX_ID.
    //
    CANRegWrite(CAN0_BASE + CAN_O_IF1CRQ,
                (MSG_OBJ_BCAST_TX_ID) & CAN_IF1CRQ_MNUM_M);
}

//*****************************************************************************
//
//! Configures the CAN interface.
//!
//! \param ulSetTiming determines if the CAN bit timing should be configured.
//!
//! This function configures the CAN controller, preparing it for use by
//! the boot loader.   If the \e ulSetTiming parameter is 0, the bit timing
//! for the CAN bus will be left alone.  This occurs when the boot loader was
//! entered from a running application that already has configured the timing
//! for the system.  When \e ulSetTiming is non-zero the bit timing will be
//! set to the defaults defined in the <tt>bl_config.h</tt> file in the
//! project.
//!
//! \return None.
//
//*****************************************************************************
static void
ConfigureCANInterface(unsigned long ulSetTiming)
{
    //
    // Reset the state of all the message object and the state of the CAN
    // module to a known state.
    //
    CANInit();

    //
    // If a device identifier was specified then this was due to an update from
    // a running CAN application so don't change the CAN bit timing.
    //
    if(ulSetTiming != 0)
    {
        //
        // Set the bit fields of the bit timing register according to the
        // parms.
        //
        CANRegWrite(CAN0_BASE + CAN_O_BIT, CAN_BIT_TIMING);

        //
        // Set the divider upper bits in the extension register.
        //
        CANRegWrite(CAN0_BASE + CAN_O_BRPE, 0);
    }

    //
    // Take the CAN0 device out of INIT state.
    //
    CANRegWrite(CAN0_BASE + CAN_O_CTL, 0);

    //
    // Configure the broadcast receive message object.
    //
    CANMessageSetRx();
}

//*****************************************************************************
//
// Reads the next packet that is sent to the boot loader.
//
//*****************************************************************************
static unsigned long
PacketRead(unsigned char *pucData, unsigned long *pulSize)
{
    unsigned long ulMsgID;

#ifdef CAN_UART_BRIDGE
    unsigned long ulSize, ulLength, ulMode, ulChar;
    unsigned char pucBuffer[12];

    //
    // Initialize the size and length of the packet.
    //
    ulLength = 0;
    ulSize = 0;

    //
    // If no interface has been determined then wait for either CAN or UART
    // data until either responds.
    //
    if(g_ulInterface == IFACE_UNKNOWN)
    {
        //
        // Wait for CAN or UART data.
        //
        while((CANRegRead(CAN0_BASE + CAN_O_NWDA1) == 0) &&
              ((HWREG(UART0_BASE + UART_O_FR) & UART_FR_RXFE) == UART_FR_RXFE))
        {
        }

        //
        // If the UART FIFO was empty then the loop exited due to a CAN
        // message.
        //
        if((HWREG(UART0_BASE + UART_O_FR) & UART_FR_RXFE) == UART_FR_RXFE)
        {
            g_ulInterface = IFACE_CAN;
        }
        else
        {
            //
            // The UART FIFO was not empty so the UART interface was used.
            //
            g_ulInterface = IFACE_UART;
        }
    }

    //
    // Read a data packet from the CAN controller.
    //
    if(g_ulInterface == IFACE_CAN)
    {
#endif
        //
        // Wait until a packet has been received.
        //
        while(CANRegRead(CAN0_BASE + CAN_O_NWDA1) == 0)
        {
        }

        //
        // Read the packet.
        //
        *pulSize = CANMessageGetRx(pucData, &ulMsgID);

#ifdef CAN_UART_BRIDGE
    }
    else
    {
        //
        // Read a data packet from the UART controller.
        //
        ulMode = 0;

        while(1)
        {
            //
            // Wait until a char is available.
            //
            while(HWREG(UART0_BASE + UART_O_FR) & UART_FR_RXFE)
            {
            }

            //
            // Now get the char.
            //
            ulChar = HWREG(UART0_BASE + UART_O_DR);

            if(ulChar == 0xff)
            {
                ulMode = 1;
                ulLength = 0;
            }
            else if(ulMode == 1)
            {
                if(ulChar > 12)
                {
                    ulMode = 0;
                }
                else
                {
                    ulSize = ulChar;
                    ulMode = 2;
                }
            }
            else if(ulMode == 3)
            {
                if(ulChar == 0xfe)
                {
                    pucBuffer[ulLength++] = 0xff;
                    ulMode = 2;
                }
                else if(ulChar == 0xfd)
                {
                    pucBuffer[ulLength++] = 0xfe;
                    ulMode = 2;
                }
                else
                {
                    ulMode = 0;
                }
            }
            else if(ulMode == 2)
            {
                if(ulChar == 0xfe)
                {
                    ulMode = 3;
                }
                else
                {
                    pucBuffer[ulLength++] = ulChar;
                }
            }

            if((ulLength == ulSize) && (ulMode == 2))
            {
                ulMsgID = *(unsigned long *)pucBuffer;

                if((ulMsgID & (CAN_MSGID_MFR_M | CAN_MSGID_DTYPE_M)) ==
                   LM_API_UPD)
                {
                    *(unsigned long *)pucData =
                        *(unsigned long *)(pucBuffer + 4);
                    *(unsigned long *)(pucData + 4) =
                        *(unsigned long *)(pucBuffer + 8);
                    *pulSize = ulSize - 4;
                    break;
                }
            }
        }
    }
#endif

    //
    // Return the message ID of the packet that was received.
    //
    return(ulMsgID);
}

//*****************************************************************************
//
// This function writes out an individual character over the UART and
// handles sending out special sequences for handling 0xff and 0xfe values.
//
//*****************************************************************************
#ifdef CAN_UART_BRIDGE
static void
UARTBridgeWrite(unsigned long ulChar)
{
    //
    // See if the character being sent is 0xff.
    //
    if(ulChar == 0xff)
    {
        //
        // Send 0xfe 0xfe, the escaped version of 0xff.  A sign extended
        // version of 0xfe is used to avoid the check below for 0xfe, thereby
        // avoiding an infinite loop.  Only the lower 8 bits are actually sent,
        // so 0xfe is what is actually transmitted.
        //
        UARTBridgeWrite(0xfffffffe);
        UARTBridgeWrite(0xfffffffe);
    }

    //
    // Otherwise, see if the character being sent is 0xfe.
    //
    else if(ulChar == 0xfe)
    {
        //
        // Send 0xfe 0xfd, the escaped version of 0xfe.  A sign extended
        // version of 0xfe is used to avoid the check above for 0xfe, thereby
        // avoiding an infinite loop.  Only the lower 8 bits are actually sent,
        // so 0xfe is what is actually transmitted.
        //
        UARTBridgeWrite(0xfffffffe);
        UARTBridgeWrite(0xfd);
    }

    //
    // Otherwise, simply send this character.
    //
    else
    {
        //
        // Wait until space is available in the UART transmit FIFO.
        //
        while(HWREG(UART0_BASE + UART_O_FR) & UART_FR_TXFF)
        {
        }

        //
        // Send the char.
        //
        HWREG(UART0_BASE + UART_O_DR) = ulChar & 0xff;
    }
}
#endif

//*****************************************************************************
//
// Sends a packet to the controller that is communicating with the boot loader.
//
//*****************************************************************************
static void
PacketWrite(unsigned long ulId, const unsigned char *pucData,
            unsigned long ulSize)
{
    unsigned long ulIdx;

#ifdef CAN_UART_BRIDGE
    //
    // Check if the boot loader is in CAN mode.
    //
    if(g_ulInterface == IFACE_CAN)
    {
#endif
        //
        // Wait until the previous packet has been sent, providing a time out so
        // that the boot loader does not hang here.
        //
        for(ulIdx = 1000;
            (ulIdx != 0) && (CANRegRead(CAN0_BASE + CAN_O_TXRQ1) != 0);
            ulIdx--)
        {
        }

        //
        // If the previous packet was sent, then send this packet.
        //
        if(ulIdx != 0)
        {
            CANMessageSetTx(ulId, pucData, ulSize);
        }
#ifdef CAN_UART_BRIDGE
    }
    else
    {
        //
        // The boot loader is in UART modes so write the packet using the UART
        // functions.  Write the start pattern followed by the size, and the ID.
        //
        UARTBridgeWrite(0xffffffff);
        UARTBridgeWrite(ulSize + 4);
        UARTBridgeWrite(ulId & 0xff);
        UARTBridgeWrite((ulId >> 8) & 0xff);
        UARTBridgeWrite((ulId >> 16) & 0xff);
        UARTBridgeWrite((ulId >> 24) & 0xff);

        //
        // Now write out the remaining data bytes.
        //
        while(ulSize--)
        {
            UARTBridgeWrite(*pucData++);
        }
    }
#endif
}

//*****************************************************************************
//
//! This is the main routine for handling updating over CAN.
//!
//! This function accepts boot loader commands over CAN to perform a firmware
//! update over the CAN bus.  This function assumes that the CAN bus timing
//! and message objects have been configured elsewhere.
//!
//! \return None.
//
//*****************************************************************************
void
UpdaterCAN(void)
{
    unsigned long ulBytes;
    unsigned long ulCmd;
    unsigned long ulFlashSize;
    unsigned long ulTemp;
    unsigned char ucStatus;

#ifdef ENABLE_UPDATE_CHECK
    //
    // Check the application is valid and check the pin to see if an update is
    // being requested.
    //
    if(g_ulForced == 1)
    {
        //
        // Send out the CAN request.
        //
#ifdef CAN_UART_BRIDGE
        g_ulInterface = IFACE_CAN;
#endif
        PacketWrite(LM_API_UPD_REQUEST, 0, 0);

        //
        // Send out the UART request.
        //
#ifdef CAN_UART_BRIDGE
        g_ulInterface = IFACE_UART;
        PacketWrite(LM_API_UPD_REQUEST, 0, 0);
        g_ulInterface = IFACE_UNKNOWN;
#endif

        //
        // Wait only 50ms for the response and move on otherwise.
        //
        Delay(CRYSTAL_FREQ / 20);

        //
        // Wait until a packet has been received.
        //
#ifdef CAN_UART_BRIDGE
        if((CANRegRead(CAN0_BASE + CAN_O_NWDA1) == 0) &&
           ((HWREG(UART0_BASE + UART_O_FR) & UART_FR_RXFE) == UART_FR_RXFE))
#else
        if(CANRegRead(CAN0_BASE + CAN_O_NWDA1) == 0)
#endif
        {
            //
            // Call the application.
            //
            StartApplication();
        }
    }
#endif

    //
    // Loop forever processing packets.
    //
    while(1)
    {
        //
        // Read the next packet.
        //
        ulBytes = 0;
        ulCmd = PacketRead(g_pucCommandBuffer, &ulBytes);

        //
        // Handle this packet.
        //
        ucStatus = CAN_CMD_SUCCESS;
        switch(ulCmd)
        {
            //
            // This is an update request packet.
            //
            case LM_API_UPD_REQUEST:
            {
                //
                // This packet is ignored (other than generating an ACK).
                //
                break;
            }

            //
            // This is a ping packet.
            //
            case LM_API_UPD_PING:
            {
                //
                // This packet is ignored (other than generating an ACK).
                //
                break;
            }

            //
            // This is a reset packet.
            //
            case LM_API_UPD_RESET:
            {
                //
                // Perform a software reset request.  This will cause the
                // microcontroller to reset; no further code will be executed.
                //
                // Use the reset function in the flash patch if appropriate.
                //
#ifdef FLASH_PATCH_COMPATIBLE
                ((void (*)(void))0x801)();
#else
                HWREG(NVIC_APINT) = (NVIC_APINT_VECTKEY |
                                     NVIC_APINT_SYSRESETREQ);
#endif

                //
                // The microcontroller should have reset, so this should never
                // be reached.  Just in case, loop forever.
                //
                while(1)
                {
                }
            }

            //
            // This is a data packet.
            //
            case LM_API_UPD_SEND_DATA:
            {
                //
                // If this is overwriting the boot loader then the application
                // has already been erased so now erase the boot loader.
                //
                if(g_ulTransferAddress == 0)
                {
                    //
                    // Clear the flash access interrupt.
                    //
                    BL_FLASH_CL_ERR_FN_HOOK();

                    //
                    // Erase the application before the boot loader.
                    //
                    for(ulTemp = 0; ulTemp < APP_START_ADDRESS;
                        ulTemp += FLASH_PAGE_SIZE)
                    {
                        //
                        // Erase this block.
                        //
                        BL_FLASH_ERASE_FN_HOOK(ulTemp);
                    }

                    //
                    // Return an error if an access violation occurred.
                    //
                    if(BL_FLASH_ERROR_FN_HOOK())
                    {
                        //
                        // Setting g_ulTransferSize to zero makes
                        // COMMAND_SEND_DATA fail to accept any more data.
                        //
                        g_ulTransferSize = 0;

                        //
                        // Indicate that the flash erase failed.
                        //
                        ucStatus = CAN_CMD_FAIL;
                    }
                }

                //
                // Check if there are any more bytes to receive.
                //
                if(g_ulTransferSize >= ulBytes)
                {
                    //
                    // Decrypt the data if required.
                    //
#ifdef BL_DECRYPT_FN_HOOK
                    BL_DECRYPT_FN_HOOK(g_pucCommandBuffer, ulBytes);
#endif

                    //
                    // Clear the flash access interrupt.
                    //
                    BL_FLASH_CL_ERR_FN_HOOK();

                    //
                    // Skip the first transfer.
                    //
                    if(g_ulStartSize == g_ulTransferSize)
                    {
                        g_ulStartValues[0] =
                            *((unsigned long *)&g_pucCommandBuffer[0]);
                        g_ulStartValues[1] =
                            *((unsigned long *)&g_pucCommandBuffer[4]);
                    }
                    else
                    {
                        //
                        // Loop over the words to program.
                        //
                        BL_FLASH_PROGRAM_FN_HOOK(g_ulTransferAddress,
                                                 g_pucCommandBuffer,
                                                 ulBytes);
                    }

                    //
                    // Return an error if an access violation occurred.
                    //
                    if(BL_FLASH_ERROR_FN_HOOK())
                    {
                        //
                        // Indicate that the flash programming failed.
                        //
                        ucStatus = CAN_CMD_FAIL;
                    }
                    else
                    {
                        //
                        // Now update the address to program.
                        //
                        g_ulTransferSize -= ulBytes;
                        g_ulTransferAddress += ulBytes;

                        //
                        // If a progress hook function has been provided, call
                        // it here.
                        //
#ifdef BL_PROGRESS_FN_HOOK
                        BL_PROGRESS_FN_HOOK(g_ulStartSize - g_ulTransferSize,
                                            g_ulStartSize);
#endif
                    }
                }
                else
                {
                    //
                    // This indicates that too much data is being sent to the
                    // device.
                    //
                    ucStatus = CAN_CMD_FAIL;
                }

                //
                // If the last expected bytes were received then write out the
                // first two words of the image to allow it to boot.
                //
                if(g_ulTransferSize == 0)
                {
                    //
                    // Loop over the words to program.
                    //
                    BL_FLASH_PROGRAM_FN_HOOK(g_ulStartAddress,
                                             (unsigned char *)&g_ulStartValues,
                                             8);

                    //
                    // If an end signal hook function has been provided, call
                    // it here since we have finished a download.
                    //
#ifdef BL_END_FN_HOOK
                    BL_END_FN_HOOK();
#endif
                }
                break;
            }

            //
            // This is a start download packet.
            //
            case LM_API_UPD_DOWNLOAD:
            {
                //
                // Get the application address and size from the packet data.
                //
                g_ulTransferAddress =
                    *((unsigned long *)&g_pucCommandBuffer[0]);
                g_ulTransferSize = *((unsigned long *)&g_pucCommandBuffer[4]);
                g_ulStartSize = g_ulTransferSize;
                g_ulStartAddress = g_ulTransferAddress;

                //
                // Check for a valid starting address and image size.
                //
                if(!BL_FLASH_AD_CHECK_FN_HOOK(g_ulTransferAddress,
                                              g_ulTransferSize))
                {
                    //
                    // Set the code to an error to indicate that the last
                    // command failed.  This informs the updater program
                    // that the download command failed.
                    //
                    ucStatus = CAN_CMD_FAIL;

                    //
                    // This packet has been handled.
                    //
                    break;
                }

                //
                // Only erase the space that we need if we are not protecting
                // the code, otherwise erase the entire flash.
                //
#ifdef FLASH_CODE_PROTECTION
                ulFlashSize = BL_FLASH_SIZE_FN_HOOK();
#ifdef FLASH_RSVD_SPACE
                if((ulFlashSize - FLASH_RSVD_SPACE) != g_ulTransferAddress)
                {
                    ulFlashSize -= FLASH_RSVD_SPACE;
                }
#endif
#else
                ulFlashSize = g_ulTransferAddress + g_ulTransferSize;
#endif

                //
                // Clear the flash access interrupt.
                //
                BL_FLASH_CL_ERR_FN_HOOK();

                //
                // Leave the boot loader present until we start getting an
                // image.
                //
                for(ulTemp = g_ulTransferAddress; ulTemp < ulFlashSize;
                    ulTemp += FLASH_PAGE_SIZE)
                {
                    //
                    // Erase this block.
                    //
                    BL_FLASH_ERASE_FN_HOOK(ulTemp);
                }

                //
                // Return an error if an access violation occurred.
                //
                if(BL_FLASH_ERROR_FN_HOOK())
                {
                    ucStatus = CAN_CMD_FAIL;
                }

                //
                // See if the command was successful.
                //
                if(ucStatus != CAN_CMD_SUCCESS)
                {
                    //
                    // Setting g_ulTransferSize to zero makes COMMAND_SEND_DATA
                    // fail to accept any data.
                    //
                    g_ulTransferSize = 0;
                }
#ifdef BL_START_FN_HOOK
                else
                {
                    //
                    // If a start signal hook function has been provided, call
                    // it here since we are about to start a new download.
                    //
                    BL_START_FN_HOOK();
                }
#endif

                break;
            }

            //
            // This is an unknown packet.
            //
            default:
            {
                //
                // Set the status to indicate a failure.
                //
                ucStatus = CAN_CMD_FAIL;
                break;
            }
        }

        //
        // Send an ACK packet in response to indicate that the packet was
        // received.  The status in the ACK data indicates if the command was
        // successfully processed.
        //
        PacketWrite(LM_API_UPD_ACK, &ucStatus, 1);
    }
}

//*****************************************************************************
//
// Configures the UART used for CAN traffic bridging.
//
//*****************************************************************************
#ifdef CAN_UART_BRIDGE
void
ConfigureBridge(void)
{
    //
    // Enable the GPIO module if necessary.
    //
#if (CAN_RX_PERIPH != SYSCTL_RCGC2_GPIOA) && \
    (CAN_TX_PERIPH != SYSCTL_RCGC2_GPIOA)
    HWREG(SYSCTL_RCGC2) |= SYSCTL_RCGC2_GPIOA;
#endif

    //
    // Enable the UART module.
    //
    HWREG(SYSCTL_RCGC1) |= SYSCTL_RCGC1_UART0;

    //
    // Enable the GPIO pins used for the UART.
    //
    HWREG(GPIO_PORTA_BASE + GPIO_O_AFSEL) |= 0x3;
    HWREG(GPIO_PORTA_BASE + GPIO_O_DEN) |= 0x03;

    //
    // Configure the UART.
    //
    HWREG(UART0_BASE + UART_O_IBRD) = UART_BAUD_RATIO(115200) >> 6;
    HWREG(UART0_BASE + UART_O_FBRD) = (UART_BAUD_RATIO(115200) &
                                       UART_FBRD_DIVFRAC_M);
    HWREG(UART0_BASE + UART_O_LCRH) = UART_LCRH_WLEN_8 | UART_LCRH_FEN;
    HWREG(UART0_BASE + UART_O_CTL) = (UART_CTL_UARTEN | UART_CTL_TXE |
                                      UART_CTL_RXE);
}
#endif

//*****************************************************************************
//
//! This is the application entry point to the CAN updater.
//!
//! This function should only be entered from a running application and not
//! when running the boot loader with no application present.
//!
//! \return None.
//
//*****************************************************************************
void
AppUpdaterCAN(void)
{
    //
    // If the boot loader is being called from the application the UART needs
    // to be configured.
    //
#ifdef CAN_UART_BRIDGE
    ConfigureBridge();
#endif

    //
    // Configure the CAN controller but don't change the bit timing.
    //
    ConfigureCANInterface(0);

    //
    // Call the main update routine.
    //
    UpdaterCAN();
}

//*****************************************************************************
//
//! Generic configuration is handled in this function.
//!
//! This function is called by the start up code to perform any configuration
//! necessary before calling the update routine.
//!
//! \return None.
//
//*****************************************************************************
void
ConfigureCAN(void)
{
    //
    // Turn the LDO voltage up to 2.75V if configured to do so.  This is a
    // workaround for errata in revision A2 of Fury-class devices that allows
    // the PLL to operate reliably.
    //
#ifdef BOOST_LDO_VOLTAGE
    HWREG(SYSCTL_LDOPCTL) = SYSCTL_LDOPCTL_2_75V;
#endif

#ifdef CRYSTAL_FREQ
    //
    // Since the crystal frequency was specified, enable the main oscillator
    // and clock the processor from it.
    //
    HWREG(SYSCTL_RCC) &= ~(SYSCTL_RCC_MOSCDIS);

    //
    // Delay while the main oscillator starts up.
    //
    Delay(524288);

    //
    // See if the CAN peripheral requires the PLL.
    //
#ifdef CAN_REQUIRES_PLL
    //
    // Set the crystal frequency, switch to the main oscillator, and enable the
    // PLL.
    //
    HWREG(SYSCTL_RCC) = ((HWREG(SYSCTL_RCC) &
                          ~(SYSCTL_RCC_PWRDN | SYSCTL_RCC_XTAL_M |
                            SYSCTL_RCC_OSCSRC_M)) |
                         XTAL_VALUE | SYSCTL_RCC_OSCSRC_MAIN);

    //
    // Delay while the PLL locks.
    //
    Delay(524288);

    //
    // Disable the PLL bypass so that the part is clocked from the PLL, and set
    // sysdiv to 16.  This yields a system clock of 12.5MHz.
    //
    HWREG(SYSCTL_RCC) = ((HWREG(SYSCTL_RCC) &
                          ~(SYSCTL_RCC_BYPASS | SYSCTL_RCC_SYSDIV_M)) |
                         SYSCTL_RCC_SYSDIV_16 | SYSCTL_RCC_USESYSDIV);
#else
    //
    // Set the crystal frequency and switch to the main oscillator.
    //
    HWREG(SYSCTL_RCC) = ((HWREG(SYSCTL_RCC) &
                          ~(SYSCTL_RCC_XTAL_M | SYSCTL_RCC_OSCSRC_M)) |
                         XTAL_VALUE | SYSCTL_RCC_OSCSRC_MAIN);
#endif

    //
    // Set the flash programming time based on the specified crystal frequency.
    //
    HWREG(FLASH_USECRL) = ((CRYSTAL_FREQ + 999999) / 1000000) - 1;
#else
    //
    // Set the flash to program at 16 MHz since that is just beyond the fastest
    // that we could run.
    //
    HWREG(FLASH_USECRL) = 15;
#endif

    //
    // Enable the CAN controller.
    //
    HWREG(SYSCTL_RCGC0) |= SYSCTL_RCGC0_CAN0;

#if CAN_RX_PERIPH == CAN_TX_PERIPH
    //
    // Enable the GPIO associated with CAN0
    //
    HWREG(SYSCTL_RCGC2) |= CAN_RX_PERIPH;

    //
    // Wait a while before accessing the peripheral.
    //
    Delay(3);

    //
    // Set the alternate function selects.
    //
    HWREG(CAN_RX_PORT + GPIO_O_AFSEL) |= CAN_RX_PIN_M | CAN_TX_PIN_M;

    //
    // Set the pin type to it's digital function.
    //
    HWREG(CAN_RX_PORT + GPIO_O_DEN) |= CAN_RX_PIN_M | CAN_TX_PIN_M;

#else
    //
    // Enable the GPIO associated with CAN0
    //
    HWREG(SYSCTL_RCGC2) |= CAN_RX_PERIPH | CAN_TX_PERIPH;

    //
    // Wait a while before accessing the peripheral.
    //
    Delay(3);

    //
    // Set the alternate function selects.
    //
    HWREG(CAN_RX_PORT + GPIO_O_AFSEL) |= CAN_RX_PIN_M;
    HWREG(CAN_TX_PORT + GPIO_O_AFSEL) |= CAN_TX_PIN_M;

    //
    // Set the pin type to it's digital function.
    //
    HWREG(CAN_RX_PORT + GPIO_O_DEN) |= CAN_RX_PIN_M;
    HWREG(CAN_TX_PORT + GPIO_O_DEN) |= CAN_TX_PIN_M;
#endif

    //
    // Configure the UART used for bridging.
    //
#ifdef CAN_UART_BRIDGE
    ConfigureBridge();
#endif

    //
    // Configure the CAN interface.
    //
    ConfigureCANInterface(1);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
#endif
