//*****************************************************************************
//
// softssi.c - Driver for the SoftSSI.
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

//*****************************************************************************
//
//! \addtogroup softssi_api
//! @{
//
//*****************************************************************************

#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "utils/softssi.h"

//*****************************************************************************
//
// The states in the SoftSSI state machine.
//
//*****************************************************************************
#define SOFTSSI_STATE_IDLE      0
#define SOFTSSI_STATE_START     1
#define SOFTSSI_STATE_IN        2
#define SOFTSSI_STATE_OUT       3
#define SOFTSSI_STATE_STOP1     4
#define SOFTSSI_STATE_STOP2     5

//*****************************************************************************
//
// The flags in the SoftSSI ucFlags structure member.
//
//*****************************************************************************
#define SOFTSSI_FLAG_ENABLE     0x80
#define SOFTSSI_FLAG_SPH        0x02
#define SOFTSSI_FLAG_SPO        0x01

//*****************************************************************************
//
//! Sets the configuration of a SoftSSI module.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param ucProtocol specifes the data transfer protocol.
//! \param ucBits specifies the number of bits transferred per frame.
//!
//! This function configures the data format of a SoftSSI module.  The
//! \e ucProtocol parameter can be one of the following values:
//! \b SOFTSSI_FRF_MOTO_MODE_0, \b SOFTSSI_FRF_MOTO_MODE_1,
//! \b SOFTSSI_FRF_MOTO_MODE_2, or \b SOFTSSI_FRF_MOTO_MODE_3.  These frame
//! formats imply the following polarity and phase configurations:
//!
//! <pre>
//! Polarity Phase         Mode
//!   0       0   SOFTSSI_FRF_MOTO_MODE_0
//!   0       1   SOFTSSI_FRF_MOTO_MODE_1
//!   1       0   SOFTSSI_FRF_MOTO_MODE_2
//!   1       1   SOFTSSI_FRF_MOTO_MODE_3
//! </pre>
//!
//! The \e ucBits parameter defines the width of the data transfers, and can be
//! a value between 4 and 16, inclusive.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSIConfigSet(tSoftSSI *pSSI, unsigned char ucProtocol,
                 unsigned char ucBits)
{
    //
    // See if a GPIO pin has been set for Fss.
    //
    if(pSSI->ulFssGPIO != 0)
    {
        //
        // Configure the Fss pin.
        //
        MAP_GPIOPinTypeGPIOOutput(pSSI->ulFssGPIO & 0xfffff000,
                                  (pSSI->ulFssGPIO & 0x00000fff) >> 2);

        //
        // Set the Fss pin high.
        //
        HWREG(pSSI->ulFssGPIO) = 255;
    }

    //
    // Configure the Clk pin.
    //
    MAP_GPIOPinTypeGPIOOutput(pSSI->ulClkGPIO & 0xfffff000,
                              (pSSI->ulClkGPIO & 0x00000fff) >> 2);

    //
    // Set the Clk pin high or low based on the configured clock polarity.
    //
    if((ucProtocol & SOFTSSI_FLAG_SPO) == 0)
    {
        HWREG(pSSI->ulClkGPIO) = 0;
    }
    else
    {
        HWREG(pSSI->ulClkGPIO) = 255;
    }

    //
    // Configure the Tx pin and set it low.
    //
    MAP_GPIOPinTypeGPIOOutput(pSSI->ulTxGPIO & 0xfffff000,
                              (pSSI->ulTxGPIO & 0x00000fff) >> 2);
    HWREG(pSSI->ulTxGPIO) = 0;

    //
    // See if a GPIO pin has been set for Rx.
    //
    if(pSSI->ulRxGPIO != 0)
    {
        //
        // Configure the Rx pin.
        //
        MAP_GPIOPinTypeGPIOInput(pSSI->ulRxGPIO & 0xfffff000,
                                 (pSSI->ulRxGPIO & 0x00000fff) >> 2);
    }

    //
    // Make sure that the transmit and receive FIFOs are empty.
    //
    pSSI->usTxBufferRead = 0;
    pSSI->usTxBufferWrite = 0;
    pSSI->usRxBufferRead = 0;
    pSSI->usRxBufferWrite = 0;

    //
    // Save the frame protocol.
    //
    pSSI->ucFlags = ucProtocol;

    //
    // Save the number of data bits.
    //
    pSSI->ucBits = ucBits;

    //
    // Since the FIFOs are empty, the transmit FIFO "interrupt" is asserted.
    //
    pSSI->ucIntStatus = SOFTSSI_TXFF;

    //
    // Reset the idle counter.
    //
    pSSI->ucIdleCount = 0;

    //
    // Disable the SoftSSI module.
    //
    pSSI->ucFlags &= ~(SOFTSSI_FLAG_ENABLE);

    //
    // Start the SoftSSI state machine in the idle state.
    //
    pSSI->ucState = SOFTSSI_STATE_IDLE;
}

//*****************************************************************************
//
//! Handles the assertion/deassertion of the transmit FIFO ``interrupt''.
//!
//! \param pSSI specifies the SoftSSI data structure.
//!
//! This function is used to determine when to assert or deassert the transmit
//! FIFO ``interrupt''.
//!
//! \return None.
//
//*****************************************************************************
static void
SoftSSITxInt(tSoftSSI *pSSI)
{
    unsigned short usTemp;

    //
    // Determine the number of words left in the transmit FIFO.
    //
    if(pSSI->usTxBufferRead > pSSI->usTxBufferWrite)
    {
        usTemp = (pSSI->usTxBufferLen + pSSI->usTxBufferWrite -
                  pSSI->usTxBufferRead);
    }
    else
    {
        usTemp = pSSI->usTxBufferWrite - pSSI->usTxBufferRead;
    }

    //
    // If the transmit FIFO is now half full or less, generate a transmit FIFO
    // "interrupt".  Otherwise, clear the transmit FIFO "interrupt".
    //
    if(usTemp <= (pSSI->usTxBufferLen / 2))
    {
        pSSI->ucIntStatus |= SOFTSSI_TXFF;
    }
    else
    {
        pSSI->ucIntStatus &= ~(SOFTSSI_TXFF);
    }
}

//*****************************************************************************
//
//! Handles the assertion/deassertion of the receive FIFO ``interrupt''.
//!
//! \param pSSI specifies the SoftSSI data structure.
//!
//! This function is used to determine when to assert or deassert the receive
//! FIFO ``interrupt''.
//!
//! \return None.
//
//*****************************************************************************
static void
SoftSSIRxInt(tSoftSSI *pSSI)
{
    unsigned short usTemp;

    //
    // Determine the number of words in the receive FIFO.
    //
    if(pSSI->usRxBufferRead > pSSI->usRxBufferWrite)
    {
        usTemp = (pSSI->usRxBufferLen + pSSI->usRxBufferWrite -
                  pSSI->usRxBufferRead);
    }
    else
    {
        usTemp = pSSI->usRxBufferWrite - pSSI->usRxBufferRead;
    }

    //
    // If the receive FIFO is now half full or more, generate a receive FIFO
    // "interrupt".  Otherwise, clear the receive FIFO "interrupt".
    //
    if(usTemp >= (pSSI->usRxBufferLen / 2))
    {
        pSSI->ucIntStatus |= SOFTSSI_RXFF;
    }
    else
    {
        pSSI->ucIntStatus &= ~(SOFTSSI_RXFF);
    }
}

//*****************************************************************************
//
//! Performs the periodic update of the SoftSSI module.
//!
//! \param pSSI specifies the SoftSSI data structure.
//!
//! This function performs the periodic, time-based updates to the SoftSSI
//! module.  The transmission and reception of data over the SoftSSI link is
//! performed by the state machine in this function.
//!
//! This function must be called at twice the desired SoftSSI clock rate.  For
//! example, to run the SoftSSI clock at 10 KHz, this function must be called
//! at a 20 KHz rate.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSITimerTick(tSoftSSI *pSSI)
{
    unsigned short usTemp;

    //
    // Determine the current state of the state machine.
    //
    switch(pSSI->ucState)
    {
        //
        // The state machine is idle.
        //
        case SOFTSSI_STATE_IDLE:
        {
            //
            // See if the SoftSSI module is enabled and there is data in the
            // transmit FIFO.
            //
            if(((pSSI->ucFlags & SOFTSSI_FLAG_ENABLE) != 0) &&
               (pSSI->usTxBufferRead != pSSI->usTxBufferWrite))
            {
                //
                // Assert the Fss signal if it is configured.
                //
                if(pSSI->ulFssGPIO != 0)
                {
                    HWREG(pSSI->ulFssGPIO) = 0;
                }

                //
                // Move to the start state.
                //
                pSSI->ucState = SOFTSSI_STATE_START;
            }

            //
            // Otherwise, see if there is data in the receive FIFO.
            //
            else if((pSSI->usRxBufferRead != pSSI->usRxBufferWrite) &&
                    (pSSI->ucIdleCount != 64))
            {
                //
                // Increment the idle counter.
                //
                pSSI->ucIdleCount++;

                //
                // See if the idle counter has become large enough to trigger
                // a timeout "interrupt".
                //
                if(pSSI->ucIdleCount == 64)
                {
                    //
                    // Trigger the receive timeout "interrupt".
                    //
                    pSSI->ucIntStatus |= SOFTSSI_RXTO;
                }
            }

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The start machine is in the transfer start state.
        //
        case SOFTSSI_STATE_START:
        {
            //
            // Get the next word to transfer from the transmit FIFO.
            //
            pSSI->usTxData = (pSSI->pusTxBuffer[pSSI->usTxBufferRead] <<
                              (16 - pSSI->ucBits));

            //
            // Initialize the receive buffer to zero.
            //
            pSSI->usRxData = 0;

            //
            // Initialize the count of bits tranferred.
            //
            pSSI->ucCurrentBit = 0;

            //
            // Write the first bit of the transmit word to the Tx pin.
            //
            HWREG(pSSI->ulTxGPIO) =
                (pSSI->usTxData & 0x8000) ? 255 : 0;

            //
            // Shift to the next bit of the transmit word.
            //
            pSSI->usTxData <<= 1;

            //
            // If in SPI mode 1 or 3, then the Clk signal needs to be toggled.
            //
            if((pSSI->ucFlags & SOFTSSI_FLAG_SPH) != 0)
            {
                HWREG(pSSI->ulClkGPIO) ^= 255;
            }

            //
            // Move to the data input state.
            //
            pSSI->ucState = SOFTSSI_STATE_IN;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The state machine is in the data input state.
        //
        case SOFTSSI_STATE_IN:
        {
            //
            // Read the next bit from the Rx signal if it is configured.
            //
            if(pSSI->ulRxGPIO != 0)
            {
                pSSI->usRxData = ((pSSI->usRxData << 1) |
                                  (HWREG(pSSI->ulRxGPIO) ? 1 : 0));
            }

            //
            // Toggle the Clk signal.
            //
            HWREG(pSSI->ulClkGPIO) ^= 255;

            //
            // Increment the number of bits transferred.
            //
            pSSI->ucCurrentBit++;

            //
            // See if the entire word has been transferred.
            //
            if(pSSI->ucCurrentBit != pSSI->ucBits)
            {
                //
                // There are more bits to transfer, so move to the data output
                // state.
                //
                pSSI->ucState = SOFTSSI_STATE_OUT;
            }
            else
            {
                //
                // Increment the transmit read pointer, removing the word that
                // was just transferred from the transmit FIFO.
                //
                pSSI->usTxBufferRead++;
                if(pSSI->usTxBufferRead == pSSI->usTxBufferLen)
                {
                    pSSI->usTxBufferRead = 0;
                }

                //
                // See if a transmit FIFO "interrupt" needs to be asserted.
                //
                SoftSSITxInt(pSSI);

                //
                // Determine the new value for the receive FIFO write pointer.
                //
                usTemp = pSSI->usRxBufferWrite + 1;
                if(usTemp >= pSSI->usRxBufferLen)
                {
                    usTemp = 0;
                }

                //
                // See if there is space in the receive FIFO for the word that
                // was just received.
                //
                if(usTemp == pSSI->usRxBufferRead)
                {
                    //
                    // The receive FIFO is full, so generate a receive FIFO
                    // overrun "interrupt".
                    //
                    pSSI->ucIntStatus |= SOFTSSI_RXOR;
                }
                else
                {
                    //
                    // Store the new word into the receive FIFO.
                    //
                    pSSI->pusRxBuffer[pSSI->usRxBufferWrite] = pSSI->usRxData;

                    //
                    // Save the new receive FIFO write pointer.
                    //
                    pSSI->usRxBufferWrite = usTemp;

                    //
                    // See if a receive FIFO "interrupt" needs to be asserted.
                    //
                    SoftSSIRxInt(pSSI);
                }

                //
                // See if the next word should be transmitted immediately.
                // This will occur when there is data in the transmit FIFO, the
                // SoftSSI module is enabled, and the SoftSSI module is in SPI
                // mode 1 or 3.
                //
                if(((pSSI->ucFlags & SOFTSSI_FLAG_ENABLE) != 0) &&
                   ((pSSI->ucFlags & SOFTSSI_FLAG_SPH) != 0) &&
                   (pSSI->usTxBufferRead != pSSI->usTxBufferWrite))
                {
                    //
                    // Get the next word to transfer from the transmit FIFO.
                    //
                    pSSI->usTxData =
                        (pSSI->pusTxBuffer[pSSI->usTxBufferRead] <<
                         (16 - pSSI->ucBits));

                    //
                    // Initialize the receive buffer to zero.
                    //
                    pSSI->usRxData = 0;

                    //
                    // Initialize the count of bits tranferred.
                    //
                    pSSI->ucCurrentBit = 0;

                    //
                    // Move to the data output state.
                    //
                    pSSI->ucState = SOFTSSI_STATE_OUT;
                }
                else
                {
                    //
                    // The next word should not be transmitted immediately, so
                    // move to the first step of the stop state.
                    //
                    pSSI->ucState = SOFTSSI_STATE_STOP1;
                }
            }

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The state machine is in the data output state.
        //
        case SOFTSSI_STATE_OUT:
        {
            //
            // Write the next bit of the transmit word to the Tx pin.
            //
            HWREG(pSSI->ulTxGPIO) = (pSSI->usTxData & 0x8000) ? 255 : 0;

            //
            // Toggle the Clk signal.
            //
            HWREG(pSSI->ulClkGPIO) ^= 255;

            //
            // Shift to the next bit of the transmit word.
            //
            pSSI->usTxData <<= 1;

            //
            // Move to the data input state.
            //
            pSSI->ucState = SOFTSSI_STATE_IN;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The state machine is in the first step of the stop state.
        //
        case SOFTSSI_STATE_STOP1:
        {
            //
            // Set the Tx pin low.
            //
            HWREG(pSSI->ulTxGPIO) = 0;

            //
            // If in SPI mode 1 or 3, then the Clk signal needs to be toggled.
            //
            if((pSSI->ucFlags & SOFTSSI_FLAG_SPH) == 0)
            {
                HWREG(pSSI->ulClkGPIO) ^= 255;
            }

            //
            // Move to the second step of the stop state.
            //
            pSSI->ucState = SOFTSSI_STATE_STOP2;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The state machine is in the second step of the stop state.
        //
        case SOFTSSI_STATE_STOP2:
        {
            //
            // Deassert the Fss signal if it is configured.
            //
            if(pSSI->ulFssGPIO != 0)
            {
                HWREG(pSSI->ulFssGPIO) = 255;
            }

            //
            // Move to the idle state.
            //
            pSSI->ucState = SOFTSSI_STATE_IDLE;

            //
            // Reset the idle counter.
            //
            pSSI->ucIdleCount = 0;

            //
            // See if the end of transfer "interrupt" should be generated.
            //
            if(pSSI->usTxBufferRead == pSSI->usTxBufferWrite)
            {
                pSSI->ucIntStatus |= SOFTSSI_TXEOT;
            }

            //
            // This state has been handled.
            //
            break;
        }
    }

    //
    // Call the "interrupt" callback while there are enabled "interrupts"
    // asserted.  By calling in a loop until the "interrupts" are no longer
    // asserted, this mimics the behavior of a real hardware implementation of
    // the SSI peripheral.
    //
    while(((pSSI->ucIntStatus & pSSI->ucIntMask) != 0) &&
          (pSSI->pfnIntCallback != 0))
    {
        //
        // Call the callback function.
        //
        pSSI->pfnIntCallback();
    }
}

//*****************************************************************************
//
//! Enables the SoftSSI module.
//!
//! \param pSSI specifies the SoftSSI data structure.
//!
//! This function enables operation of the SoftSSI module.  The SoftSSI module
//! must be configured before it is enabled.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSIEnable(tSoftSSI *pSSI)
{
    //
    // Enable the SoftSSI module.
    //
    pSSI->ucFlags |= SOFTSSI_FLAG_ENABLE;
}

//*****************************************************************************
//
//! Disables the SoftSSI module.
//!
//! \param pSSI specifies the SoftSSI data structure.
//!
//! This function disables operation of the SoftSSI module.  If a data transfer
//! is in progress, it is finished before the module is fully disabled.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSIDisable(tSoftSSI *pSSI)
{
    //
    // Disable the SoftSSI module.
    //
    pSSI->ucFlags &= ~(SOFTSSI_FLAG_ENABLE);
}

//*****************************************************************************
//
//! Enables individual SoftSSI ``interrupt'' sources.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param ulIntFlags is a bit mask of the ``interrupt'' sources to be enabled.
//!
//! Enables the indicated SoftSSI ``interrupt'' sources.  Only the sources that
//! are enabled can be reflected to the callback function; disabled sources do
//! not result in a callback.  The \e ulIntFlags parameter can be any of the
//! \b SOFTSSI_TXEOT, \b SOFTSSI_TXFF, \b SOFTSSI_RXFF, \b SOFTSSI_RXTO, or
//! \b SOFTSSI_RXOR values.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSIIntEnable(tSoftSSI *pSSI, unsigned long ulIntFlags)
{
    //
    // Enable the specified "interrupts".
    //
    pSSI->ucIntMask |= ulIntFlags;
}

//*****************************************************************************
//
//! Disables individual SoftSSI ``interrupt'' sources.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param ulIntFlags is a bit mask of the ``interrupt'' sources to be
//! disabled.
//!
//! Disables the indicated SoftSSI ``interrupt'' sources.  The \e ulIntFlags
//! parameter can be any of the \b SOFTSSI_TXEOT, \b SOFTSSI_TXFF,
//! \b SOFTSSI_RXFF, \b SOFTSSI_RXTO, or \b SOFTSSI_RXOR values.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSIIntDisable(tSoftSSI *pSSI, unsigned long ulIntFlags)
{
    //
    // Disable the specified "interrupts".
    //
    pSSI->ucIntMask &= ~(ulIntFlags);
}

//*****************************************************************************
//
//! Gets the current ``interrupt'' status.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param bMasked is \b false if the raw ``interrupt'' status is required or
//! \b true if the masked ``interrupt'' status is required.
//!
//! This function returns the ``interrupt'' status for the SoftSSI module.
//! Either the raw ``interrupt'' status or the status of ``interrupts'' that
//! are allowed to reflect to the callback can be returned.
//!
//! \return The current ``interrupt'' status, enumerated as a bit field of
//! \b SOFTSSI_TXEOT, \b SOFTSSI_TXFF, \b SOFTSSI_RXFF, \b SOFTSSI_RXTO, and
//! \b SOFTSSI_RXOR.
//
//*****************************************************************************
unsigned long
SoftSSIIntStatus(tSoftSSI *pSSI, tBoolean bMasked)
{
    //
    // Return either the "interrupt" status or the raw "interrupt" status as
    // requested.
    //
    if(bMasked)
    {
        return(pSSI->ucIntStatus & pSSI->ucIntMask);
    }
    else
    {
        return(pSSI->ucIntStatus);
    }
}

//*****************************************************************************
//
//! Clears SoftSSI ``interrupt'' sources.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param ulIntFlags is a bit mask of the ``interrupt'' sources to be cleared.
//!
//! The specified SoftSSI ``interrupt'' sources are cleared so that they no
//! longer assert.  This function must be called in the ``interrupt'' handler
//! to keep the ``interrupt'' from being recognized again immediately upon
//! exit.  The \e ulIntFlags parameter is the logical OR of any of the
//! \b SOFTSSI_TXEOT, \b SOFTSSI_RXTO, and \b SOFTSSI_RXOR values.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSIIntClear(tSoftSSI *pSSI, unsigned long ulIntFlags)
{
    //
    // Clear the requested "interrupt" sources.
    //
    pSSI->ucIntStatus &= ~(ulIntFlags) | SOFTSSI_TXFF | SOFTSSI_RXFF;
}

//*****************************************************************************
//
//! Determines if there is any data in the receive FIFO.
//!
//! \param pSSI specifies the SoftSSI data structure.
//!
//! This function determines if there is any data available to be read from the
//! receive FIFO.
//!
//! \return Returns \b true if there is data in the receive FIFO or \b false
//! if there is no data in the receive FIFO.
//
//*****************************************************************************
tBoolean
SoftSSIDataAvail(tSoftSSI *pSSI)
{
    //
    // Return the availability of data.
    //
    return((pSSI->usRxBufferRead == pSSI->usRxBufferWrite) ? false : true);
}

//*****************************************************************************
//
//! Determines if there is any space in the transmit FIFO.
//!
//! \param pSSI specifies the SoftSSI data structure.
//!
//! This function determines if there is space available in the transmit FIFO.
//!
//! \return Returns \b true if there is space available in the transmit FIFO or
//! \b false if there is no space available in the transmit FIFO.
//
//*****************************************************************************
tBoolean
SoftSSISpaceAvail(tSoftSSI *pSSI)
{
    unsigned short usTemp;

    //
    // Determine the values of the write pointer once incremented.
    //
    usTemp = pSSI->usTxBufferWrite + 1;
    if(usTemp == pSSI->usTxBufferLen)
    {
        usTemp = 0;
    }

    //
    // Return the availability of space.
    //
    return((pSSI->usTxBufferRead == usTemp) ? false : true);
}

//*****************************************************************************
//
//! Puts a data element into the SoftSSI transmit FIFO.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param ulData is the data to be transmitted over the SoftSSI interface.
//!
//! This function places the supplied data into the transmit FIFO of the
//! specified SoftSSI module.
//!
//! \note The upper 32 - N bits of the \e ulData are discarded, where N is the
//! data width as configured by SoftSSIConfigSet().  For example, if the
//! interface is configured for 8-bit data width, the upper 24 bits of
//! \e ulData are discarded.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSIDataPut(tSoftSSI *pSSI, unsigned long ulData)
{
    unsigned short usTemp;

    //
    // Wait until there is space.
    //
    usTemp = pSSI->usTxBufferWrite + 1;
    if(usTemp == pSSI->usTxBufferLen)
    {
        usTemp = 0;
    }
    while(usTemp == *(volatile unsigned short *)(&(pSSI->usTxBufferRead)))
    {
    }

    //
    // Write the data to the SoftSSI.
    //
    pSSI->pusTxBuffer[pSSI->usTxBufferWrite] = ulData;
    pSSI->usTxBufferWrite = usTemp;

    //
    // See if a transmit FIFO "interrupt" needs to be cleared.
    //
    SoftSSITxInt(pSSI);
}

//*****************************************************************************
//
//! Puts a data element into the SoftSSI transmit FIFO.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param ulData is the data to be transmitted over the SoftSSI interface.
//!
//! This function places the supplied data into the transmit FIFO of the
//! specified SoftSSI module.  If there is no space in the FIFO, then this
//! function returns a zero.
//!
//! \note The upper 32 - N bits of the \e ulData are discarded, where N is the
//! data width as configured by SoftSSIConfigSet().  For example, if the
//! interface is configured for 8-bit data width, the upper 24 bits of
//! \e ulData are discarded.
//!
//! \return Returns the number of elements written to the SSI transmit FIFO.
//
//*****************************************************************************
long
SoftSSIDataPutNonBlocking(tSoftSSI *pSSI, unsigned long ulData)
{
    unsigned short usTemp;

    //
    // Determine the values of the write pointer once incremented.
    //
    usTemp = pSSI->usTxBufferWrite + 1;
    if(usTemp == pSSI->usTxBufferLen)
    {
        usTemp = 0;
    }

    //
    // Check for space to write.
    //
    if(usTemp != pSSI->usTxBufferRead)
    {
        pSSI->pusTxBuffer[pSSI->usTxBufferWrite] = ulData;
        pSSI->usTxBufferWrite = usTemp;
        SoftSSITxInt(pSSI);
        return(1);
    }
    else
    {
        return(0);
    }
}

//*****************************************************************************
//
//! Gets a data element from the SoftSSI receive FIFO.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param pulData is a pointer to a storage location for data that was
//! received over the SoftSSI interface.
//!
//! This function gets received data from the receive FIFO of the specified
//! SoftSSI module and places that data into the location specified by the
//! \e pulData parameter.
//!
//! \note Only the lower N bits of the value written to \e pulData contain
//! valid data, where N is the data width as configured by SoftSSIConfigSet().
//! For example, if the interface is configured for 8-bit data width, only the
//! lower 8 bits of the value written to \e pulData contain valid data.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSIDataGet(tSoftSSI *pSSI, unsigned long *pulData)
{
    //
    // Wait until there is data to be read.
    //
    while(pSSI->usRxBufferRead ==
          *(volatile unsigned short *)(&(pSSI->usRxBufferWrite)))
    {
    }

    //
    // Read data from SoftSSI.
    //
    *pulData = pSSI->pusRxBuffer[pSSI->usRxBufferRead];
    pSSI->usRxBufferRead++;
    if(pSSI->usRxBufferRead == pSSI->usRxBufferLen)
    {
        pSSI->usRxBufferRead = 0;
    }

    //
    // See if a receive FIFO "interrupt" needs to be cleared.
    //
    SoftSSIRxInt(pSSI);
}

//*****************************************************************************
//
//! Gets a data element from the SoftSSI receive FIFO.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param pulData is a pointer to a storage location for data that was
//! received over the SoftSSI interface.
//!
//! This function gets received data from the receive FIFO of the specified
//! SoftSSI module and places that data into the location specified by the
//! \e ulData parameter.  If there is no data in the FIFO, then this function
//! returns a zero.
//!
//! \note Only the lower N bits of the value written to \e pulData contain
//! valid data, where N is the data width as configured by SoftSSIConfigSet().
//! For example, if the interface is configured for 8-bit data width, only the
//! lower 8 bits of the value written to \e pulData contain valid data.
//!
//! \return Returns the number of elements read from the SoftSSI receive FIFO.
//
//*****************************************************************************
long
SoftSSIDataGetNonBlocking(tSoftSSI *pSSI, unsigned long *pulData)
{
    //
    // Check for data to read.
    //
    if(pSSI->usRxBufferRead != pSSI->usRxBufferWrite)
    {
        *pulData = pSSI->pusRxBuffer[pSSI->usRxBufferRead];
        pSSI->usRxBufferRead++;
        if(pSSI->usRxBufferRead == pSSI->usRxBufferLen)
        {
            pSSI->usRxBufferRead = 0;
        }
        SoftSSIRxInt(pSSI);
        return(1);
    }
    else
    {
        return(0);
    }
}

//*****************************************************************************
//
//! Determines whether the SoftSSI transmitter is busy or not.
//!
//! \param pSSI specifies the SoftSSI data structure.
//!
//! Allows the caller to determine whether all transmitted bytes have cleared
//! the transmitter.  If \b false is returned, then the transmit FIFO is empty
//! and all bits of the last transmitted word have left the shift register.
//!
//! \return Returns \b true if the SoftSSI is transmitting or \b false if all
//! transmissions are complete.
//
//*****************************************************************************
tBoolean
SoftSSIBusy(tSoftSSI *pSSI)
{
    //
    // Determine if the SSI is busy.
    //
    return(((pSSI->ucState == SOFTSSI_STATE_IDLE) &&
            (((pSSI->ucFlags & SOFTSSI_FLAG_ENABLE) == 0) ||
             (pSSI->usTxBufferRead == pSSI->usTxBufferWrite))) ? false : true);
}

//*****************************************************************************
//
//! Sets the callback used by the SoftSSI module.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param pfnCallback is a pointer to the callback function.
//!
//! This function sets the address of the callback function that is called when
//! there is an ``interrupt'' produced by the SoftSSI module.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSICallbackSet(tSoftSSI *pSSI, void (*pfnCallback)(void))
{
    //
    // Save the callback function address.
    //
    pSSI->pfnIntCallback = pfnCallback;
}

//*****************************************************************************
//
//! Sets the GPIO pin to be used as the SoftSSI Fss signal.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param ulBase is the base address of the GPIO module.
//! \param ucPin is the bit-packed representation of the pin to use.
//!
//! This function sets the GPIO pin that is used for the SoftSSI Fss signal.
//! If there is not a GPIO pin allocated for Fss, the SoftSSI module does not
//! assert/deassert the Fss signal, leaving it to the application either to do
//! manually or to not do at all if the slave device has Fss tied to ground.
//!
//! The pin is specified using a bit-packed byte, where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSIFssGPIOSet(tSoftSSI *pSSI, unsigned long ulBase, unsigned char ucPin)
{
    //
    // Save the base address and pin for the Fss signal.
    //
    if(ulBase == 0)
    {
        pSSI->ulFssGPIO = 0;
    }
    else
    {
        pSSI->ulFssGPIO = ulBase + (ucPin << 2);
    }
}

//*****************************************************************************
//
//! Sets the GPIO pin to be used as the SoftSSI Clk signal.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param ulBase is the base address of the GPIO module.
//! \param ucPin is the bit-packed representation of the pin to use.
//!
//! This function sets the GPIO pin that is used for the SoftSSI Clk signal.
//!
//! The pin is specified using a bit-packed byte, where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSIClkGPIOSet(tSoftSSI *pSSI, unsigned long ulBase, unsigned char ucPin)
{
    //
    // Save the base address and pin for the Clk signal.
    //
    pSSI->ulClkGPIO = ulBase + (ucPin << 2);
}

//*****************************************************************************
//
//! Sets the GPIO pin to be used as the SoftSSI Tx signal.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param ulBase is the base address of the GPIO module.
//! \param ucPin is the bit-packed representation of the pin to use.
//!
//! This function sets the GPIO pin that is used for the SoftSSI Tx signal.
//!
//! The pin is specified using a bit-packed byte, where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSITxGPIOSet(tSoftSSI *pSSI, unsigned long ulBase, unsigned char ucPin)
{
    //
    // Save the base address and pin for the Tx signal.
    //
    pSSI->ulTxGPIO = ulBase + (ucPin << 2);
}

//*****************************************************************************
//
//! Sets the GPIO pin to be used as the SoftSSI Rx signal.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param ulBase is the base address of the GPIO module.
//! \param ucPin is the bit-packed representation of the pin to use.
//!
//! This function sets the GPIO pin that is used for the SoftSSI Rx signal.  If
//! there is not a GPIO pin allocated for Rx, the SoftSSI module does not read
//! data from the slave device.
//!
//! The pin is specified using a bit-packed byte, where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSIRxGPIOSet(tSoftSSI *pSSI, unsigned long ulBase, unsigned char ucPin)
{
    //
    // Save the base address and pin for the Rx signal.
    //
    if(ulBase == 0)
    {
        pSSI->ulRxGPIO = 0;
    }
    else
    {
        pSSI->ulRxGPIO = ulBase + (ucPin << 2);
    }
}

//*****************************************************************************
//
//! Sets the transmit FIFO buffer for a SoftSSI module.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param pusTxBuffer is the address of the transmit FIFO buffer.
//! \param usLen is the size, in 16-bit half-words, of the transmit FIFO
//! buffer.
//!
//! This function sets the address and size of the transmit FIFO buffer and
//! also resets the read and write pointers, marking the transmit FIFO as
//! empty.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSITxBufferSet(tSoftSSI *pSSI, unsigned short *pusTxBuffer,
                   unsigned short usLen)
{
    //
    // Save the transmit FIFO buffer address and length.
    //
    pSSI->pusTxBuffer = pusTxBuffer;
    pSSI->usTxBufferLen = usLen;

    //
    // Reset the transmit FIFO read and write pointers.
    //
    pSSI->usTxBufferRead = 0;
    pSSI->usTxBufferWrite = 0;
}

//*****************************************************************************
//
//! Sets the receive FIFO buffer for a SoftSSI module.
//!
//! \param pSSI specifies the SoftSSI data structure.
//! \param pusRxBuffer is the address of the receive FIFO buffer.
//! \param usLen is the size, in 16-bit half-words, of the receive FIFO buffer.
//!
//! This function sets the address and size of the receive FIFO buffer and also
//! resets the read and write pointers, marking the receive FIFO as empty.
//! When the buffer pointer and length are configured as zero, all data
//! received from the slave device is discarded.  This capability is useful
//! when there is no GPIO pin allocated for the Rx signal.
//!
//! \return None.
//
//*****************************************************************************
void
SoftSSIRxBufferSet(tSoftSSI *pSSI, unsigned short *pusRxBuffer,
                   unsigned short usLen)
{
    //
    // Save the receive FIFO buffer address and length.
    //
    pSSI->pusRxBuffer = pusRxBuffer;
    pSSI->usRxBufferLen = usLen;

    //
    // Reset the receive FIFO read and write pointers.
    //
    pSSI->usRxBufferRead = 0;
    pSSI->usRxBufferWrite = 0;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
