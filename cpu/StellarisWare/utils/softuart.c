//*****************************************************************************
//
// softuart.c - Driver for the SoftUART.
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
//! \addtogroup softuart_api
//! @{
//
//*****************************************************************************

#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/uart.h"
#include "utils/softuart.h"

//*****************************************************************************
//
// The states in the SoftUART transmit state machine.  The code depends upon
// the fact that the value of TXSTATE_DATA_n is n + 1, and that TXSTATE_DATA_0
// is 1.
//
//*****************************************************************************
#define SOFTUART_TXSTATE_IDLE   0
#define SOFTUART_TXSTATE_DATA_0 1
#define SOFTUART_TXSTATE_DATA_1 2
#define SOFTUART_TXSTATE_DATA_2 3
#define SOFTUART_TXSTATE_DATA_3 4
#define SOFTUART_TXSTATE_DATA_4 5
#define SOFTUART_TXSTATE_DATA_5 6
#define SOFTUART_TXSTATE_DATA_6 7
#define SOFTUART_TXSTATE_DATA_7 8
#define SOFTUART_TXSTATE_START  9
#define SOFTUART_TXSTATE_PARITY 10
#define SOFTUART_TXSTATE_STOP_0 11
#define SOFTUART_TXSTATE_STOP_1 12
#define SOFTUART_TXSTATE_BREAK  13

//*****************************************************************************
//
// The states of the SoftUART receive state machine.  The code depends upon the
// the fact that the value of RXSTATE_DATA_n is n, and that RXSTATE_DATA_0 is
// 0.
//
//*****************************************************************************
#define SOFTUART_RXSTATE_DATA_0 0
#define SOFTUART_RXSTATE_DATA_1 1
#define SOFTUART_RXSTATE_DATA_2 2
#define SOFTUART_RXSTATE_DATA_3 3
#define SOFTUART_RXSTATE_DATA_4 4
#define SOFTUART_RXSTATE_DATA_5 5
#define SOFTUART_RXSTATE_DATA_6 6
#define SOFTUART_RXSTATE_DATA_7 7
#define SOFTUART_RXSTATE_IDLE   8
#define SOFTUART_RXSTATE_PARITY 9
#define SOFTUART_RXSTATE_STOP_0 10
#define SOFTUART_RXSTATE_STOP_1 11
#define SOFTUART_RXSTATE_BREAK  12
#define SOFTUART_RXSTATE_DELAY  13

//*****************************************************************************
//
// The flags in the SoftUART ucFlags structure member.
//
//*****************************************************************************
#define SOFTUART_FLAG_ENABLE    0x01
#define SOFTUART_FLAG_TXBREAK   0x02

//*****************************************************************************
//
// The flags in the SoftUART ucRxFlags structure member.
//
//*****************************************************************************
#define SOFTUART_RXFLAG_OE      0x08
#define SOFTUART_RXFLAG_BE      0x04
#define SOFTUART_RXFLAG_PE      0x02
#define SOFTUART_RXFLAG_FE      0x01

//*****************************************************************************
//
// Additional internal configuration stored in the SoftUART usConfig structure
// member.
//
//*****************************************************************************
#define SOFTUART_CONFIG_BASE_M  0x00ff
#define SOFTUART_CONFIG_EXT_M   0xff00
#define SOFTUART_CONFIG_TXLVL_M 0x0700
#define SOFTUART_CONFIG_TXLVL_1 0x0000
#define SOFTUART_CONFIG_TXLVL_2 0x0100
#define SOFTUART_CONFIG_TXLVL_4 0x0200
#define SOFTUART_CONFIG_TXLVL_6 0x0300
#define SOFTUART_CONFIG_TXLVL_7 0x0400
#define SOFTUART_CONFIG_RXLVL_M 0x3800
#define SOFTUART_CONFIG_RXLVL_1 0x0000
#define SOFTUART_CONFIG_RXLVL_2 0x0800
#define SOFTUART_CONFIG_RXLVL_4 0x1000
#define SOFTUART_CONFIG_RXLVL_6 0x1800
#define SOFTUART_CONFIG_RXLVL_7 0x2000

//*****************************************************************************
//
// The odd parity of each possible data byte.  The odd parity of N can be found
// by looking at bit N % 32 of word N / 32.
//
//*****************************************************************************
static unsigned long g_pulParityOdd[] =
{
    0x69969669, 0x96696996, 0x96696996, 0x69969669,
    0x96696996, 0x69969669, 0x69969669, 0x96696996
};

//*****************************************************************************
//
//! Initializes the SoftUART module.
//!
//! \param pUART specifies the soft UART data structure.
//!
//! This function initializes the data structure for the SoftUART module,
//! putting it into the default configuration.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTInit(tSoftUART *pUART)
{
    //
    // Clear the SoftUART data structure.
    //
    memset(pUART, 0, sizeof(tSoftUART));

    //
    // Set the default transmit and receive buffer interrupt level.
    //
    pUART->usConfig = SOFTUART_CONFIG_TXLVL_4 | SOFTUART_CONFIG_RXLVL_4;
}

//*****************************************************************************
//
//! Sets the configuration of a SoftUART module.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param ulConfig is the data format for the port (number of data bits,
//! number of stop bits, and parity).
//!
//! This function configures the SoftUART for operation in the specified data
//! format, as specified in the \e ulConfig parameter.
//!
//! The \e ulConfig parameter is the logical OR of three values: the number of
//! data bits, the number of stop bits, and the parity.
//! \b SOFTUART_CONFIG_WLEN_8, \b SOFTUART_CONFIG_WLEN_7,
//! \b SOFTUART_CONFIG_WLEN_6, and \b SOFTUART_CONFIG_WLEN_5 select from eight
//! to five data bits per byte (respectively).  \b SOFTUART_CONFIG_STOP_ONE and
//! \b SOFTUART_CONFIG_STOP_TWO select one or two stop bits (respectively).
//! \b SOFTUART_CONFIG_PAR_NONE, \b SOFTUART_CONFIG_PAR_EVEN,
//! \b SOFTUART_CONFIG_PAR_ODD, \b SOFTUART_CONFIG_PAR_ONE, and
//! \b SOFTUART_CONFIG_PAR_ZERO select the parity mode (no parity bit, even
//! parity bit, odd parity bit, parity bit always one, and parity bit always
//! zero, respectively).
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTConfigSet(tSoftUART *pUART, unsigned long ulConfig)
{
    //
    // See if a GPIO pin has been set for Tx.
    //
    if(pUART->ulTxGPIO != 0)
    {
        //
        // Configure the Tx pin.
        //
        MAP_GPIOPinTypeGPIOOutput(pUART->ulTxGPIO & 0xfffff000,
                                  (pUART->ulTxGPIO & 0x00000fff) >> 2);

        //
        // Set the Tx pin high.
        //
        HWREG(pUART->ulTxGPIO) = 255;
    }

    //
    // See if a GPIO pin has been set for Rx.
    //
    if(pUART->ulRxGPIOPort != 0)
    {
        //
        // Configure the Rx pin.
        //
        MAP_GPIOPinTypeGPIOInput(pUART->ulRxGPIOPort, pUART->ucRxPin);

        //
        // Set the Rx pin to generate an interrupt on the next falling edge.
        //
        MAP_GPIOIntTypeSet(pUART->ulRxGPIOPort, pUART->ucRxPin,
                           GPIO_FALLING_EDGE);

        //
        // Enable the Rx pin interrupt.
        //
        MAP_GPIOPinIntClear(pUART->ulRxGPIOPort, pUART->ucRxPin);
        MAP_GPIOPinIntEnable(pUART->ulRxGPIOPort, pUART->ucRxPin);
    }

    //
    // Make sure that the transmit and receive buffers are empty.
    //
    pUART->usTxBufferRead = 0;
    pUART->usTxBufferWrite = 0;
    pUART->usRxBufferRead = 0;
    pUART->usRxBufferWrite = 0;

    //
    // Save the data format.
    //
    pUART->usConfig = ((pUART->usConfig & SOFTUART_CONFIG_EXT_M) |
                       (ulConfig & SOFTUART_CONFIG_BASE_M));

    //
    // Enable the SoftUART module.
    //
    pUART->ucFlags |= SOFTUART_FLAG_ENABLE;

    //
    // The next value to be written to the Tx pin is one since the SoftUART is
    // idle.
    //
    pUART->ucTxNext = 255;

    //
    // Start the SoftUART state machines in the idle state.
    //
    pUART->ucTxState = SOFTUART_TXSTATE_IDLE;
    pUART->ucRxState = SOFTUART_RXSTATE_IDLE;
}

//*****************************************************************************
//
//! Performs the periodic update of the SoftUART transmitter.
//!
//! \param pUART specifies the SoftUART data structure.
//!
//! This function performs the periodic, time-based updates to the SoftUART
//! transmitter.  The transmission of data from the SoftUART is performed by
//! the state machine in this function.
//!
//! This function must be called at the desired SoftUART baud rate.  For
//! example, to run the SoftUART at 115,200 baud, this function must be called
//! at a 115,200 Hz rate.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTTxTimerTick(tSoftUART *pUART)
{
    unsigned long ulTemp;

    //
    // Write the next value to the Tx data line.  This value was computed on
    // the previous timer tick, which helps to reduce the jitter on the Tx
    // edges (which is important since a UART connection does not contain a
    // clock signal).
    //
    HWREG(pUART->ulTxGPIO) = pUART->ucTxNext;

    //
    // Determine the current state of the state machine.
    //
    switch(pUART->ucTxState)
    {
        //
        // The state machine is idle.
        //
        case SOFTUART_TXSTATE_IDLE:
        {
            //
            // See if the SoftUART module is enabled.
            //
            if(!(pUART->ucFlags & SOFTUART_FLAG_ENABLE))
            {
                //
                // The SoftUART module is not enabled, so do nothing and stay
                // in the idle state.
                //
                break;
            }

            //
            // See if the break signal should be asserted.
            //
            else if(pUART->ucFlags & SOFTUART_FLAG_TXBREAK)
            {
                //
                // The data line should be driven low while in the break state.
                //
                pUART->ucTxNext = 0;

                //
                // Move to the break state.
                //
                pUART->ucTxState = SOFTUART_TXSTATE_BREAK;
            }

            //
            // Otherwise, see if there is data in the transmit buffer.
            //
            else if(pUART->usTxBufferRead != pUART->usTxBufferWrite)
            {
                //
                // The data line should be driven low to indicate a start bit.
                //
                pUART->ucTxNext = 0;

                //
                // Move to the start bit state.
                //
                pUART->ucTxState = SOFTUART_TXSTATE_START;
            }

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The state machine is in the start bit state.
        //
        case SOFTUART_TXSTATE_START:
        {
            //
            // Get the next byte to be transmitted.
            //
            pUART->ucTxData = pUART->pucTxBuffer[pUART->usTxBufferRead];

            //
            // The next value to be written to the data line is the LSB of the
            // next data byte.
            //
            pUART->ucTxNext = (pUART->ucTxData & 1) ? 255 : 0;

            //
            // Move to the data bit 0 state.
            //
            pUART->ucTxState = SOFTUART_TXSTATE_DATA_0;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // In each of these states, a bit of the data byte must be output.
        // This depends upon TXSTATE_DATA_n and TXSTATE_DATA_(n+1) being
        // consecutively numbered.
        //
        case SOFTUART_TXSTATE_DATA_0:
        case SOFTUART_TXSTATE_DATA_1:
        case SOFTUART_TXSTATE_DATA_2:
        case SOFTUART_TXSTATE_DATA_3:
        {
            //
            // The next value to be written to the data line is the next bit of
            // the data byte.
            //
            pUART->ucTxNext =
                (pUART->ucTxData & (1 << pUART->ucTxState)) ? 255 : 0;

            //
            // Advance to the next state.
            //
            pUART->ucTxState++;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // In each of these states, a bit of the data byte must be output.
        // Additionally, based on the configuration of the SoftUART, this bit
        // might be the last data bit of the data byte.  This depends upon
        // TXSTATE_DATA_n and TXSTATE_DATA_(n+1) being consecutively numbered.
        //
        case SOFTUART_TXSTATE_DATA_4:
        case SOFTUART_TXSTATE_DATA_5:
        case SOFTUART_TXSTATE_DATA_6:
        case SOFTUART_TXSTATE_DATA_7:
        {
            //
            // See if the bit that was just transferred is the last bit of the
            // data byte (based on the configuration of the SoftUART).
            //
            if(((pUART->usConfig & SOFTUART_CONFIG_WLEN_MASK) >>
                SOFTUART_CONFIG_WLEN_S) ==
               (pUART->ucTxState - SOFTUART_TXSTATE_DATA_4))
            {
                //
                // See if parity is enabled.
                //
                if((pUART->usConfig & SOFTUART_CONFIG_PAR_MASK) !=
                   SOFTUART_CONFIG_PAR_NONE)
                {
                    //
                    // See if the parity is set to one.
                    //
                    if((pUART->usConfig & SOFTUART_CONFIG_PAR_MASK) ==
                       SOFTUART_CONFIG_PAR_ONE)
                    {
                        //
                        // The next value to be written to the data line is
                        // one.
                        //
                        pUART->ucTxNext = 255;
                    }

                    //
                    // Otherwise, see if the parity is set to zero.
                    //
                    else if((pUART->usConfig & SOFTUART_CONFIG_PAR_MASK) ==
                            SOFTUART_CONFIG_PAR_ZERO)
                    {
                        //
                        // The next value to be written to the data line is
                        // zero.
                        //
                        pUART->ucTxNext = 0;
                    }

                    //
                    // Otherwise, there is either even or odd parity.
                    //
                    else
                    {
                        //
                        // Find the odd parity for the data byte.
                        //
                        pUART->ucTxNext =
                            ((g_pulParityOdd[pUART->ucTxData >> 5] &
                              (1 << (pUART->ucTxData & 31))) ? 255 : 0);

                        //
                        // If the parity is set to even, then invert the
                        // parity just computed (making it even parity).
                        //
                        if((pUART->usConfig & SOFTUART_CONFIG_PAR_MASK) ==
                           SOFTUART_CONFIG_PAR_EVEN)
                        {
                            pUART->ucTxNext ^= 255;
                        }
                    }

                    //
                    // Advance to the parity state.
                    //
                    pUART->ucTxState = SOFTUART_TXSTATE_PARITY;
                }

                //
                // Parity is not enabled.
                //
                else
                {
                    //
                    // The next value to write to the data line is the stop
                    // bit.
                    //
                    pUART->ucTxNext = 255;

                    //
                    // See if there are one or two stop bits.
                    //
                    if((pUART->usConfig & SOFTUART_CONFIG_STOP_MASK) ==
                       SOFTUART_CONFIG_STOP_TWO)
                    {
                        //
                        // Advance to the two stop bits state.
                        //
                        pUART->ucTxState = SOFTUART_TXSTATE_STOP_0;
                    }
                    else
                    {
                        //
                        // Advance to the one stop bit state.
                        //
                        pUART->ucTxState = SOFTUART_TXSTATE_STOP_1;
                    }
                }
            }

            //
            // Otherwise, there are more data bits to transfer.
            //
            else
            {
                //
                // The next value to be written to the data line is the next
                // bit of the data byte.
                //
                pUART->ucTxNext =
                    (pUART->ucTxData & (1 << pUART->ucTxState)) ? 255 : 0;

                //
                // Advance to the next state.
                //
                pUART->ucTxState++;
            }

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The state machine is in the parity bit state.
        //
        case SOFTUART_TXSTATE_PARITY:
        {
            //
            // The next value to write to the data line is the stop bit.
            //
            pUART->ucTxNext = 255;

            //
            // See if there are one or two stop bits.
            //
            if((pUART->usConfig & SOFTUART_CONFIG_STOP_MASK) ==
               SOFTUART_CONFIG_STOP_TWO)
            {
                //
                // Advance to the two stop bits state.
                //
                pUART->ucTxState = SOFTUART_TXSTATE_STOP_0;
            }
            else
            {
                //
                // Advance to the one stop bit state.
                //
                pUART->ucTxState = SOFTUART_TXSTATE_STOP_1;
            }

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The state machine is in the two stop bits state.
        //
        case SOFTUART_TXSTATE_STOP_0:
        {
            //
            // Advance to the one stop bit state.
            //
            pUART->ucTxState = SOFTUART_TXSTATE_STOP_1;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The state machine is in the one stop bit state.
        //
        case SOFTUART_TXSTATE_STOP_1:
        {
            //
            // The data byte has been completely transferred, so advance the
            // read pointer.
            //
            pUART->usTxBufferRead++;
            if(pUART->usTxBufferRead == pUART->usTxBufferLen)
            {
                pUART->usTxBufferRead = 0;
            }

            //
            // Determine the number of characters in the transmit buffer.
            //
            if(pUART->usTxBufferRead > pUART->usTxBufferWrite)
            {
                ulTemp = (pUART->usTxBufferLen -
                          (pUART->usTxBufferRead - pUART->usTxBufferWrite));
            }
            else
            {
                ulTemp = pUART->usTxBufferWrite - pUART->usTxBufferRead;
            }

            //
            // If the transmit buffer fullness just crossed the programmed
            // level, generate a transmit "interrupt".
            //
            if(ulTemp == pUART->usTxBufferLevel)
            {
                pUART->usIntStatus |= SOFTUART_INT_TX;
            }

            //
            // See if the SoftUART module is enabled.
            //
            if(!(pUART->ucFlags & SOFTUART_FLAG_ENABLE))
            {
                //
                // The SoftUART module is not enabled, so do advance to the
                // idle state.
                //
                pUART->ucTxState = SOFTUART_TXSTATE_IDLE;
            }

            //
            // See if the break signal should be asserted.
            //
            else if(pUART->ucFlags & SOFTUART_FLAG_TXBREAK)
            {
                //
                // The data line should be driven low while in the break state.
                //
                pUART->ucTxNext = 0;

                //
                // Move to the break state.
                //
                pUART->ucTxState = SOFTUART_TXSTATE_BREAK;
            }

            //
            // Otherwise, see if there is data in the transmit buffer.
            //
            else if(pUART->usTxBufferRead != pUART->usTxBufferWrite)
            {
                //
                // The data line should be driven low to indicate a start bit.
                //
                pUART->ucTxNext = 0;

                //
                // Move to the start bit state.
                //
                pUART->ucTxState = SOFTUART_TXSTATE_START;
            }

            //
            // Otherwise, there is nothing to do.
            //
            else
            {
                //
                // Assert the end of transmission "interrupt".
                //
                pUART->usIntStatus |= SOFTUART_INT_EOT;

                //
                // Advance to the idle state.
                //
                pUART->ucTxState = SOFTUART_TXSTATE_IDLE;
            }

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The state machine is in the break state.
        //
        case SOFTUART_TXSTATE_BREAK:
        {
            //
            // See if the break should be deasserted.
            //
            if(!(pUART->ucFlags & SOFTUART_FLAG_ENABLE) ||
               !(pUART->ucFlags & SOFTUART_FLAG_TXBREAK))
            {
                //
                // The data line should be driven high to indicate it is idle.
                //
                pUART->ucTxNext = 255;

                //
                // Advance to the idle state.
                //
                pUART->ucTxState = SOFTUART_TXSTATE_IDLE;
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
    // the UART peripheral.
    //
    while(((pUART->usIntStatus & pUART->usIntMask) != 0) &&
          (pUART->pfnIntCallback != 0))
    {
        //
        // Call the callback function.
        //
        pUART->pfnIntCallback();
    }
}

//*****************************************************************************
//
//! Handles the assertion of the receive ``interrupt''.
//!
//! \param pUART specifies the SoftUART data structure.
//!
//! This function is used to determine when to assert the receive ``interrupt''
//! as a result of writing data into the receive buffer (when characters are
//! received from the Rx pin).
//!
//! \return None.
//
//*****************************************************************************
static void
SoftUARTRxWriteInt(tSoftUART *pUART)
{
    unsigned long ulTemp;

    //
    // Determine the number of characters in the receive buffer.
    //
    if(pUART->usRxBufferWrite > pUART->usRxBufferRead)
    {
        ulTemp = pUART->usRxBufferWrite - pUART->usRxBufferRead;
    }
    else
    {
        ulTemp = (pUART->usRxBufferLen + pUART->usRxBufferWrite -
                  pUART->usRxBufferRead);
    }

    //
    // If the receive buffer fullness just crossed the programmed level,
    // generate a receive "interrupt".
    //
    if(ulTemp == pUART->usRxBufferLevel)
    {
        pUART->usIntStatus |= SOFTUART_INT_RX;
    }
}

//*****************************************************************************
//
//! Performs the periodic update of the SoftUART receiver.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param bEdgeInt should be \b true if this function is being called because
//! of a GPIO edge interrupt and \b false if it is being called because of a
//! timer interrupt.
//!
//! This function performs the periodic, time-based updates to the SoftUART
//! receiver.  The reception of data to the SoftUART is performed by the state
//! machine in this function.
//!
//! This function must be called by the GPIO interrupt handler, and then
//! periodically at the desired SoftUART baud rate.  For example, to run the
//! SoftUART at 115,200 baud, this function must be called at a 115,200 Hz
//! rate.
//!
//! \return Returns \b SOFTUART_RXTIMER_NOP if the receive timer should
//! continue to operate or \b SOFTUART_RXTIMER_END if it should be stopped.
//
//*****************************************************************************
unsigned long
SoftUARTRxTick(tSoftUART *pUART, tBoolean bEdgeInt)
{
    unsigned long ulPinState, ulTemp, ulRet;

    //
    // Read the current state of the Rx data line.
    //
    ulPinState = MAP_GPIOPinRead(pUART->ulRxGPIOPort, pUART->ucRxPin);

    //
    // The default return code inidicates that the receive timer does not need
    // to be stopped.
    //
    ulRet = SOFTUART_RXTIMER_NOP;

    //
    // See if this is an edge interrupt while delaying for the receive timeout
    // interrupt.
    //
    if(bEdgeInt && (pUART->ucRxState == SOFTUART_RXSTATE_DELAY))
    {
        //
        // The receive timeout has been cancelled since the next character has
        // started, so go to the idle state.
        //
        pUART->ucRxState = SOFTUART_RXSTATE_IDLE;
    }

    //
    // Determine the current state of the state machine.
    //
    switch(pUART->ucRxState)
    {
        //
        // The state machine is idle.
        //
        case SOFTUART_RXSTATE_IDLE:
        {
            //
            // The falling edge of the start bit was just sampled, so disable
            // the GPIO edge interrupt since the remainder of the character
            // will be read using a timer tick.
            //
            MAP_GPIOPinIntClear(pUART->ulRxGPIOPort, pUART->ucRxPin);
            MAP_GPIOPinIntDisable(pUART->ulRxGPIOPort, pUART->ucRxPin);

            //
            // Clear the receive data buffer.
            //
            pUART->ucRxData = 0;

            //
            // Clear all reception errors other than overrun (which is cleared
            // only when the first character after the overrun is written into
            // the receive buffer), and set the break error (which is cleared
            // if any non-zero bits are read during this character).
            //
            pUART->ucRxFlags = ((pUART->ucRxFlags & SOFTUART_RXFLAG_OE) |
                                SOFTUART_RXFLAG_BE);

            //
            // Advance to the first data bit state.
            //
            pUART->ucRxState = SOFTUART_RXSTATE_DATA_0;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // In each of these states, a bit of the data byte is read.  This
        // depends upon RXSTATE_DATA_n and RXSTATE_DATA_(n+1) being
        // consecutively numbered.
        //
        case SOFTUART_RXSTATE_DATA_0:
        case SOFTUART_RXSTATE_DATA_1:
        case SOFTUART_RXSTATE_DATA_2:
        case SOFTUART_RXSTATE_DATA_3:
        {
            //
            // See if the Rx pin is high.
            //
            if(ulPinState != 0)
            {
                //
                // Set this bit of the received character.
                //
                pUART->ucRxData |= 1 << pUART->ucRxState;

                //
                // Clear the break error since a non-zero bit was received.
                //
                pUART->ucRxFlags &= ~(SOFTUART_RXFLAG_BE);
            }

            //
            // Advance to the next state.
            //
            pUART->ucRxState++;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // In each of these states, a bit of the data byte is read.
        // Additionally, based on the configuration of the SoftUART, this bit
        // might be the last bit of the data byte.  This depends upon
        // RXSTATE_DATA_n and RXSTATE_DATA_(n+1) being consecutively numbered.
        //
        case SOFTUART_RXSTATE_DATA_4:
        case SOFTUART_RXSTATE_DATA_5:
        case SOFTUART_RXSTATE_DATA_6:
        case SOFTUART_RXSTATE_DATA_7:
        {
            //
            // See if the Rx pin is high.
            //
            if(ulPinState != 0)
            {
                //
                // Set this bit of the received character.
                //
                pUART->ucRxData |= 1 << pUART->ucRxState;

                //
                // Clear the break error since a non-zero bit was received.
                //
                pUART->ucRxFlags &= ~(SOFTUART_RXFLAG_BE);
            }

            //
            // See if the bit that was just transferred is the last bit of the
            // data byte (based on the configuration of the SoftUART).
            //
            if(((pUART->usConfig & SOFTUART_CONFIG_WLEN_MASK) >>
                SOFTUART_CONFIG_WLEN_S) ==
               (pUART->ucRxState - SOFTUART_RXSTATE_DATA_4))
            {
                //
                // See if parity is enabled.
                //
                if((pUART->usConfig & SOFTUART_CONFIG_PAR_MASK) !=
                   SOFTUART_CONFIG_PAR_NONE)
                {
                    //
                    // Advance to the parity state.
                    //
                    pUART->ucRxState = SOFTUART_RXSTATE_PARITY;
                }

                //
                // Otherwise, see if there are one or two stop bits.
                //
                else if((pUART->usConfig & SOFTUART_CONFIG_STOP_MASK) ==
                        SOFTUART_CONFIG_STOP_TWO)
                {
                    //
                    // Advance to the two stop bits state.
                    //
                    pUART->ucRxState = SOFTUART_RXSTATE_STOP_0;
                }

                //
                // Otherwise, advance to the one stop bit state.
                //
                else
                {
                    pUART->ucRxState = SOFTUART_RXSTATE_STOP_1;
                }
            }

            //
            // Otherwise, there are more bits to receive.
            //
            else
            {
                //
                // Advance to the next state.
                //
                pUART->ucRxState++;
            }

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The state machine is in the parity bit state.
        //
        case SOFTUART_RXSTATE_PARITY:
        {
            //
            // See if the parity is set to one.
            //
            if((pUART->usConfig & SOFTUART_CONFIG_PAR_MASK) ==
               SOFTUART_CONFIG_PAR_ONE)
            {
                //
                // Set the expected parity to one.
                //
                ulTemp = pUART->ucRxPin;
            }

            //
            // Otherwise, see if the parity is set to zero.
            //
            else if((pUART->usConfig & SOFTUART_CONFIG_PAR_MASK) ==
                    SOFTUART_CONFIG_PAR_ZERO)
            {
                //
                // Set the expected parity to zero.
                //
                ulTemp = 0;
            }

            //
            // Otherwise, there is either even or odd parity.
            //
            else
            {
                //
                // Find the odd parity for the data byte.
                //
                ulTemp = ((g_pulParityOdd[pUART->ucRxData >> 5] &
                           (1 << (pUART->ucRxData & 31))) ?
                          pUART->ucRxPin : 0);

                //
                // If the parity is set to even, then invert the parity just
                // computed (making it even parity).
                //
                if((pUART->usConfig & SOFTUART_CONFIG_PAR_MASK) ==
                   SOFTUART_CONFIG_PAR_EVEN)
                {
                    ulTemp ^= pUART->ucRxPin;
                }
            }

            //
            // See if the pin state matches the expected parity.
            //
            if(ulPinState != ulTemp)
            {
                //
                // The parity does not match, so set the parity error flag.
                //
                pUART->ucRxFlags |= SOFTUART_RXFLAG_PE;
            }

            //
            // See if the Rx pin is high.
            //
            if(ulPinState != 0)
            {
                //
                // Clear the break error since a non-zero bit was received.
                //
                pUART->ucRxFlags &= ~(SOFTUART_RXFLAG_BE);
            }

            //
            // See if there are one or two stop bits.
            //
            if((pUART->usConfig & SOFTUART_CONFIG_STOP_MASK) ==
               SOFTUART_CONFIG_STOP_TWO)
            {
                //
                // Advance to the two stop bits state.
                //
                pUART->ucRxState = SOFTUART_RXSTATE_STOP_0;
            }
            else
            {
                //
                // Advance to the one stop bit state.
                //
                pUART->ucRxState = SOFTUART_RXSTATE_STOP_1;
            }

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The state machine is in the two stop bits state.
        //
        case SOFTUART_RXSTATE_STOP_0:
        {
            //
            // See if the Rx pin is low.
            //
            if(ulPinState == 0)
            {
                //
                // Since the Rx pin is low, there is a framing error.
                //
                pUART->ucRxFlags |= SOFTUART_RXFLAG_FE;
            }
            else
            {
                //
                // Clear the break error since a non-zero bit was received.
                //
                pUART->ucRxFlags &= ~(SOFTUART_RXFLAG_BE);
            }

            //
            // Advance to the one stop bit state.
            //
            pUART->ucRxState = SOFTUART_RXSTATE_STOP_1;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The state machine is in the one stop bit state.
        //
        case SOFTUART_RXSTATE_STOP_1:
        {
            //
            // See if the Rx pin is low.
            //
            if(ulPinState == 0)
            {
                //
                // Since the Rx pin is low, there is a framing error.
                //
                pUART->ucRxFlags |= SOFTUART_RXFLAG_FE;
            }
            else
            {
                //
                // Clear the break error since a non-zero bit was received.
                //
                pUART->ucRxFlags &= ~(SOFTUART_RXFLAG_BE);
            }

            //
            // See if the break error is still asserted (meaning that every bit
            // received was zero).
            //
            if(pUART->ucRxFlags & SOFTUART_RXFLAG_BE)
            {
                //
                // Since every bit was zero, advance to the break state.
                //
                pUART->ucRxState = SOFTUART_RXSTATE_BREAK;

                //
                // This state has been handled.
                //
                break;
            }

            //
            // Compute the value of the write pointer advanced by one.
            //
            ulTemp = pUART->usRxBufferWrite + 1;
            if(ulTemp == pUART->usRxBufferLen)
            {
                ulTemp = 0;
            }

            //
            // See if there is space in the receive buffer.
            //
            if(ulTemp == pUART->usRxBufferRead)
            {
                //
                // Set the overrun error flag.  This will remain set until a
                // new character can be placed into the receive buffer, which
                // will then be given this status.
                //
                pUART->ucRxFlags |= SOFTUART_RXFLAG_OE;

                //
                // Set the receive overrun "interrupt" and status if it is not
                // already set.
                //
                if(!(pUART->ucRxStatus & SOFTUART_RXERROR_OVERRUN))
                {
                    pUART->ucRxStatus |= SOFTUART_RXERROR_OVERRUN;
                    pUART->usIntStatus |= SOFTUART_INT_OE;
                }
            }

            //
            // Otherwise, there is space in the receive buffer.
            //
            else
            {
                //
                // Write this data byte, along with the receive flags, into the
                // receive buffer.
                //
                pUART->pusRxBuffer[pUART->usRxBufferWrite] =
                    pUART->ucRxData | (pUART->ucRxFlags << 8);

                //
                // Advance the write pointer.
                //
                pUART->usRxBufferWrite = ulTemp;

                //
                // Clear the receive flags, most importantly the overrun flag
                // since it was just written into the receive buffer.
                //
                pUART->ucRxFlags = 0;

                //
                // Assert the receive "interrupt" if appropriate.
                //
                SoftUARTRxWriteInt(pUART);
            }

            //
            // See if this character had a parity error.
            //
            if(pUART->ucRxFlags & SOFTUART_RXFLAG_PE)
            {
                //
                // Assert the parity error "interrupt".
                //
                pUART->usIntStatus |= SOFTUART_INT_PE;
            }

            //
            // See if this character had a framing error.
            //
            if(pUART->ucRxFlags & SOFTUART_RXFLAG_FE)
            {
                //
                // Assert the framing error "interrupt".
                //
                pUART->usIntStatus |= SOFTUART_INT_FE;
            }

            //
            // Enable the falling edge interrupt on the Rx pin so that the next
            // start bit can be detected.
            //
            MAP_GPIOPinIntClear(pUART->ulRxGPIOPort, pUART->ucRxPin);
            MAP_GPIOPinIntEnable(pUART->ulRxGPIOPort, pUART->ucRxPin);

            //
            // Advance to the receive timeout delay state.
            //
            pUART->ucRxData = 0;
            pUART->ucRxState = SOFTUART_RXSTATE_DELAY;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The state machine is in the break state.
        //
        case SOFTUART_RXSTATE_BREAK:
        {
            //
            // See if the Rx pin is high.
            //
            if(ulPinState != 0)
            {
                //
                // Clear the break error since a non-zero bit was received.
                //
                pUART->ucRxFlags &= ~(SOFTUART_RXFLAG_BE);
            }

            //
            // Compute the value of the write pointer advanced by one.
            //
            ulTemp = pUART->usRxBufferWrite + 1;
            if(ulTemp == pUART->usRxBufferLen)
            {
                ulTemp = 0;
            }

            //
            // See if there is space in the receive buffer.
            //
            if(ulTemp == pUART->usRxBufferRead)
            {
                //
                // Set the overrun error flag.  This will remain set until a
                // new character can be placed into the receive buffer, which
                // will then be given this status.
                //
                pUART->ucRxFlags |= SOFTUART_RXFLAG_OE;

                //
                // Set the receive overrun "interrupt" and status if it is not
                // already set.
                //
                if(!(pUART->ucRxStatus & SOFTUART_RXERROR_OVERRUN))
                {
                    pUART->ucRxStatus |= SOFTUART_RXERROR_OVERRUN;
                    pUART->usIntStatus |= SOFTUART_INT_OE;
                }
            }

            //
            // Otherwise, there is space in the receive buffer.
            //
            else
            {
                //
                // Write this data byte, along with the receive flags, into the
                // receive buffer.
                //
                pUART->pusRxBuffer[pUART->usRxBufferWrite] =
                    pUART->ucRxData | (pUART->ucRxFlags << 8);

                //
                // Advance the write pointer.
                //
                pUART->usRxBufferWrite = ulTemp;

                //
                // Clear the receive flags, most importantly the overrun flag
                // since it was just written into the receive buffer.
                //
                pUART->ucRxFlags = 0;

                //
                // Assert the receive "interrupt" if appropriate.
                //
                SoftUARTRxWriteInt(pUART);
            }

            //
            // See if this was a break error.
            //
            if(pUART->ucRxFlags & SOFTUART_RXFLAG_BE)
            {
                //
                // Assert the break error "interrupt".
                //
                pUART->usIntStatus |= SOFTUART_INT_BE;
            }

            //
            // See if this character had a parity error.
            //
            if(pUART->ucRxFlags & SOFTUART_RXFLAG_PE)
            {
                //
                // Assert the parity error "interrupt".
                //
                pUART->usIntStatus |= SOFTUART_INT_PE;
            }

            //
            // Assert the framing error "interrupt".
            //
            pUART->usIntStatus |= SOFTUART_INT_FE;

            //
            // Enable the falling edge interrupt on the Rx pin so that the next
            // start bit can be detected.
            //
            MAP_GPIOPinIntClear(pUART->ulRxGPIOPort, pUART->ucRxPin);
            MAP_GPIOPinIntEnable(pUART->ulRxGPIOPort, pUART->ucRxPin);

            //
            // Advance to the receive timeout delay state.
            //
            pUART->ucRxData = 0;
            pUART->ucRxState = SOFTUART_RXSTATE_DELAY;

            //
            // This state has been handled.
            //
            break;
        }

        //
        // The state machine is in the receive timeout delay state.
        //
        case SOFTUART_RXSTATE_DELAY:
        {
            //
            // See if the receive timeout has expired.
            //
            if(pUART->ucRxData++ == 32)
            {
                //
                // Assert the receive timeout "interrupt".
                //
                pUART->usIntStatus |= SOFTUART_INT_RT;

                //
                // Tell the caller that the receive timer can be disabled.
                //
                ulRet = SOFTUART_RXTIMER_END;
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
    // the UART peripheral.
    //
    while(((pUART->usIntStatus & pUART->usIntMask) != 0) &&
          (pUART->pfnIntCallback != 0))
    {
        //
        // Call the callback function.
        //
        pUART->pfnIntCallback();
    }

    //
    // Return to the caller.
    //
    return(ulRet);
}

//*****************************************************************************
//
//! Sets the type of parity.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param ulParity specifies the type of parity to use.
//!
//! Sets the type of parity to use for transmitting and expect when receiving.
//! The \e ulParity parameter must be one of \b SOFTUART_CONFIG_PAR_NONE,
//! \b SOFTUART_CONFIG_PAR_EVEN, \b SOFTUART_CONFIG_PAR_ODD,
//! \b SOFTUART_CONFIG_PAR_ONE, or \b SOFTUART_CONFIG_PAR_ZERO.  The last two
//! allow direct control of the parity bit; it is always either one or zero
//! based on the mode.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTParityModeSet(tSoftUART *pUART, unsigned long ulParity)
{
    //
    // Check the arguments.
    //
    ASSERT((ulParity == SOFTUART_CONFIG_PAR_NONE) ||
           (ulParity == SOFTUART_CONFIG_PAR_EVEN) ||
           (ulParity == SOFTUART_CONFIG_PAR_ODD) ||
           (ulParity == SOFTUART_CONFIG_PAR_ONE) ||
           (ulParity == SOFTUART_CONFIG_PAR_ZERO));

    //
    // Set the parity mode.
    //
    pUART->usConfig = (pUART->usConfig & SOFTUART_CONFIG_PAR_MASK) | ulParity;
}

//*****************************************************************************
//
//! Gets the type of parity currently being used.
//!
//! \param pUART specifies the SoftUART data structure.
//!
//! This function gets the type of parity used for transmitting data and
//! expected when receiving data.
//!
//! \return Returns the current parity settings, specified as one of
//! \b SOFTUART_CONFIG_PAR_NONE, \b SOFTUART_CONFIG_PAR_EVEN,
//! \b SOFTUART_CONFIG_PAR_ODD, \b SOFTUART_CONFIG_PAR_ONE, or
//! \b SOFTUART_CONFIG_PAR_ZERO.
//
//*****************************************************************************
unsigned long
SoftUARTParityModeGet(tSoftUART *pUART)
{
    //
    // Return the current parity setting.
    //
    return(pUART->usConfig & SOFTUART_CONFIG_PAR_MASK);
}

//*****************************************************************************
//
//! Sets the transmit ``interrupt'' buffer level.
//!
//! \param pUART specifies the soft UART data structure.
//!
//! This function computes the transmit buffer level at which the transmit
//! ``interrupt'' is generated.
//!
//! \return None.
//
//*****************************************************************************
static void
SoftUARTTxLevelSet(tSoftUART *pUART)
{
    //
    // Determine the transmit buffer "interrupt" fullness setting.
    //
    switch(pUART->usConfig & SOFTUART_CONFIG_TXLVL_M)
    {
        //
        // The transmit "interrupt" should be generated when the buffer is 1/8
        // full.
        //
        case SOFTUART_CONFIG_TXLVL_1:
        {
            //
            // Set the transmit buffer level to 1/8 of the buffer length.
            //
            pUART->usTxBufferLevel = pUART->usTxBufferLen / 8;

            //
            // This setting has been handled.
            //
            break;
        }

        //
        // The transmit "interrupt" should be generated when the buffer is 1/4
        // (2/8) full.
        //
        case SOFTUART_CONFIG_TXLVL_2:
        {
            //
            // Set the transmit buffer level to 1/4 of the buffer length.
            //
            pUART->usTxBufferLevel = pUART->usTxBufferLen / 4;

            //
            // This setting has been handled.
            //
            break;
        }

        //
        // The transmit "interrupt" should be generated when the buffer is 1/2
        // (4/8) full.
        //
        case SOFTUART_CONFIG_TXLVL_4:
        {
            //
            // Set the transmit buffer level to 1/2 of the buffer length.
            //
            pUART->usTxBufferLevel = pUART->usTxBufferLen / 2;

            //
            // This setting has been handled.
            //
            break;
        }

        //
        // The transmit "interrupt" should be generated when the buffer is 3/4
        // (6/8) full.
        //
        case SOFTUART_CONFIG_TXLVL_6:
        {
            //
            // Set the transmit buffer level to 3/4 of the buffer length.
            //
            pUART->usTxBufferLevel = (pUART->usTxBufferLen * 3) / 4;

            //
            // This setting has been handled.
            //
            break;
        }

        //
        // The transmit "interrupt" should be generated when the buffer is 7/8
        // full.
        //
        case SOFTUART_CONFIG_TXLVL_7:
        {
            //
            // Set the transmit buffer level to 7/8 of the buffer length.
            //
            pUART->usTxBufferLevel = (pUART->usTxBufferLen * 7) / 8;

            //
            // This setting has been handled.
            //
            break;
        }
    }
}

//*****************************************************************************
//
//! Sets the receive ``interrupt'' buffer level.
//!
//! \param pUART specifies the soft UART data structure.
//!
//! This function computes the receive buffer level at which the receive
//! ``interrupt'' is generated.
//!
//! \return None.
//
//*****************************************************************************
static void
SoftUARTRxLevelSet(tSoftUART *pUART)
{
    //
    // Determine the receive buffer "interrupt" fullness setting.
    //
    switch(pUART->usConfig & SOFTUART_CONFIG_RXLVL_M)
    {
        //
        // The receive "interrupt" should be generated when the buffer is 1/8
        // full.
        //
        case SOFTUART_CONFIG_RXLVL_1:
        {
            //
            // Set the receive buffer level to 1/8 of the buffer length.
            //
            pUART->usRxBufferLevel = pUART->usRxBufferLen / 8;

            //
            // This setting has been handled.
            //
            break;
        }

        //
        // The receive "interrupt" should be generated when the buffer is 1/4
        // (2/8) full.
        //
        case SOFTUART_CONFIG_RXLVL_2:
        {
            //
            // Set the receive buffer level to 1/4 of the buffer length.
            //
            pUART->usRxBufferLevel = pUART->usRxBufferLen / 4;

            //
            // This setting has been handled.
            //
            break;
        }

        //
        // The receive "interrupt" should be generated when the buffer is 1/2
        // (4/8) full.
        //
        case SOFTUART_CONFIG_RXLVL_4:
        {
            //
            // Set the receive buffer level to 1/2 of the buffer length.
            //
            pUART->usRxBufferLevel = pUART->usRxBufferLen / 2;

            //
            // This setting has been handled.
            //
            break;
        }

        //
        // The receive "interrupt" should be generated when the buffer is 3/4
        // (6/8) full.
        //
        case SOFTUART_CONFIG_RXLVL_6:
        {
            //
            // Set the receive buffer level to 3/4 of the buffer length.
            //
            pUART->usRxBufferLevel = (pUART->usRxBufferLen * 3) / 4;

            //
            // This setting has been handled.
            //
            break;
        }

        //
        // The receive "interrupt" should be generated when the buffer is 7/8
        // full.
        //
        case SOFTUART_CONFIG_RXLVL_7:
        {
            //
            // Set the receive buffer level to 7/8 of the buffer length.
            //
            pUART->usRxBufferLevel = (pUART->usRxBufferLen * 7) / 8;

            //
            // This setting has been handled.
            //
            break;
        }
    }
}

//*****************************************************************************
//
//! Sets the buffer level at which ``interrupts'' are generated.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param ulTxLevel is the transmit buffer ``interrupt'' level, specified as
//! one of \b UART_FIFO_TX1_8, \b UART_FIFO_TX2_8, \b UART_FIFO_TX4_8,
//! \b UART_FIFO_TX6_8, or \b UART_FIFO_TX7_8.
//! \param ulRxLevel is the receive buffer ``interrupt'' level, specified as
//! one of \b UART_FIFO_RX1_8, \b UART_FIFO_RX2_8, \b UART_FIFO_RX4_8,
//! \b UART_FIFO_RX6_8, or \b UART_FIFO_RX7_8.
//!
//! This function sets the buffer level at which transmit and receive
//! ``interrupts'' are generated.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTFIFOLevelSet(tSoftUART *pUART, unsigned long ulTxLevel,
                     unsigned long ulRxLevel)
{
    //
    // Check the arguments.
    //
    ASSERT((ulTxLevel == SOFTUART_FIFO_TX1_8) ||
           (ulTxLevel == SOFTUART_FIFO_TX2_8) ||
           (ulTxLevel == SOFTUART_FIFO_TX4_8) ||
           (ulTxLevel == SOFTUART_FIFO_TX6_8) ||
           (ulTxLevel == SOFTUART_FIFO_TX7_8));
    ASSERT((ulRxLevel == SOFTUART_FIFO_RX1_8) ||
           (ulRxLevel == SOFTUART_FIFO_RX2_8) ||
           (ulRxLevel == SOFTUART_FIFO_RX4_8) ||
           (ulRxLevel == SOFTUART_FIFO_RX6_8) ||
           (ulRxLevel == SOFTUART_FIFO_RX7_8));

    //
    // Save the buffer "interrupt" levels.
    //
    pUART->usConfig = ((pUART->usConfig & SOFTUART_CONFIG_BASE_M) |
                       ((ulTxLevel | ulRxLevel) << 8));

    //
    // Compute the new buffer "interrupt" levels.
    //
    SoftUARTTxLevelSet(pUART);
    SoftUARTRxLevelSet(pUART);
}

//*****************************************************************************
//
//! Gets the buffer level at which ``interrupts'' are generated.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param pulTxLevel is a pointer to storage for the transmit buffer level,
//! returned as one of \b UART_FIFO_TX1_8, \b UART_FIFO_TX2_8,
//! \b UART_FIFO_TX4_8, \b UART_FIFO_TX6_8, or \b UART_FIFO_TX7_8.
//! \param pulRxLevel is a pointer to storage for the receive buffer level,
//! returned as one of \b UART_FIFO_RX1_8, \b UART_FIFO_RX2_8,
//! \b UART_FIFO_RX4_8, \b UART_FIFO_RX6_8, or \b UART_FIFO_RX7_8.
//!
//! This function gets the buffer level at which transmit and receive
//! ``interrupts'' are generated.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTFIFOLevelGet(tSoftUART *pUART, unsigned long *pulTxLevel,
                 unsigned long *pulRxLevel)
{
    //
    // Extract the transmit and receive buffer levels.
    //
    *pulTxLevel = (pUART->usConfig & SOFTUART_CONFIG_TXLVL_M) >> 8;
    *pulRxLevel = (pUART->usConfig & SOFTUART_CONFIG_RXLVL_M) >> 8;
}

//*****************************************************************************
//
//! Gets the current configuration of a UART.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param pulConfig is a pointer to storage for the data format.
//!
//! Returns the data format of the SoftUART.  The data format returned in
//! \e pulConfig is enumerated the same as the \e ulConfig parameter of
//! SoftUARTConfigSet().
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTConfigGet(tSoftUART *pUART, unsigned long *pulConfig)
{
    //
    // Get the data format.
    //
    *pulConfig = pUART->usConfig & SOFTUART_CONFIG_BASE_M;
}

//*****************************************************************************
//
//! Enables the SoftUART.
//!
//! \param pUART specifies the SoftUART data structure.
//!
//! This function enables the SoftUART, allowing data to be transmitted and
//! received.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTEnable(tSoftUART *pUART)
{
    //
    // Enable the SoftUART.
    //
    pUART->ucFlags |= SOFTUART_FLAG_ENABLE;
}

//*****************************************************************************
//
//! Disables the SoftUART.
//!
//! \param pUART specifies the SoftUART data structure.
//!
//! This function disables the SoftUART after waiting for it to become idle.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTDisable(tSoftUART *pUART)
{
    //
    // Wait for end of TX.
    //
    while(SoftUARTBusy(pUART))
    {
    }

    //
    // Disable the SoftUART.
    //
    pUART->ucFlags &= ~(SOFTUART_FLAG_ENABLE);
}

//*****************************************************************************
//
//! Determines if there are any characters in the receive buffer.
//!
//! \param pUART specifies the SoftUART data structure.
//!
//! This function returns a flag indicating whether or not there is data
//! available in the receive buffer.
//!
//! \return Returns \b true if there is data in the receive buffer or \b false
//! if there is no data in the receive buffer.
//
//*****************************************************************************
tBoolean
SoftUARTCharsAvail(tSoftUART *pUART)
{
    //
    // Return the availability of characters.
    //
    return((pUART->usRxBufferRead == pUART->usRxBufferWrite) ? false : true);
}

//*****************************************************************************
//
//! Determines if there is any space in the transmit buffer.
//!
//! \param pUART specifies the SoftUART data structure.
//!
//! This function returns a flag indicating whether or not there is space
//! available in the transmit buffer.
//!
//! \return Returns \b true if there is space available in the transmit buffer
//! or \b false if there is no space available in the transmit buffer.
//
//*****************************************************************************
tBoolean
SoftUARTSpaceAvail(tSoftUART *pUART)
{
    unsigned short usTemp;

    //
    // Determine the values of the write pointer once incremented.
    //
    usTemp = pUART->usTxBufferWrite + 1;
    if(usTemp == pUART->usTxBufferLen)
    {
        usTemp = 0;
    }

    //
    // Return the availability of space.
    //
    return((pUART->usTxBufferRead == usTemp) ? false : true);
}

//*****************************************************************************
//
//! Handles the deassertion of the receive ``interrupts''.
//!
//! \param pUART specifies the SoftUART data structure.
//!
//! This function is used to determine when to deassert the receive
//! ``interrupt'' as a result of reading data from the receive buffer.
//!
//! \return None.
//
//*****************************************************************************
static void
SoftUARTRxReadInt(tSoftUART *pUART)
{
    unsigned long ulTemp;

    //
    // Determine the number of characters in the receive buffer.
    //
    if(pUART->usRxBufferWrite > pUART->usRxBufferRead)
    {
        ulTemp = pUART->usRxBufferWrite - pUART->usRxBufferRead;
    }
    else
    {
        ulTemp = (pUART->usRxBufferLen + pUART->usRxBufferWrite -
                  pUART->usRxBufferRead);
    }

    //
    // See if the number of characters in the receive buffer have dropped below
    // the receive trigger level.
    //
    if(ulTemp < pUART->usRxBufferLevel)
    {
        //
        // Deassert the receive "interrupt".
        //
        pUART->usIntStatus &= ~(SOFTUART_INT_RX);
    }

    //
    // See if the receive buffer is now empty.
    //
    if(ulTemp == 0)
    {
        //
        // Deassert the receive timeout "interrupt".
        //
        pUART->usIntStatus &= ~(SOFTUART_INT_RT);
    }
}

//*****************************************************************************
//
//! Receives a character from the specified port.
//!
//! \param pUART specifies the SoftUART data structure.
//!
//! Gets a character from the receive buffer for the specified port.
//!
//! \return Returns the character read from the specified port, cast as a
//! \e long.  A \b -1 isreturned if there are no characters present in the
//! receive buffer.  The SoftUARTCharsAvail() function should be called before
//! attempting to call this function.
//
//*****************************************************************************
long
SoftUARTCharGetNonBlocking(tSoftUART *pUART)
{
    long lTemp;

    //
    // See if there are any characters in the receive buffer.
    //
    if(pUART->usRxBufferRead != pUART->usRxBufferWrite)
    {
        //
        // Read the next character.
        //
        lTemp = pUART->pusRxBuffer[pUART->usRxBufferRead];
        pUART->usRxBufferRead++;
        if(pUART->usRxBufferRead == pUART->usRxBufferLen)
        {
            pUART->usRxBufferRead = 0;
        }

        //
        // Deassert the receive "interrupt(s)" if appropriate.
        //
        SoftUARTRxReadInt(pUART);

        //
        // Set the receive status to match this character.
        //
        pUART->ucRxStatus = ((pUART->ucRxStatus & SOFTUART_RXERROR_OVERRUN) |
                             ((lTemp >> 8) & ~(SOFTUART_RXERROR_OVERRUN)));

        //
        // Return this character.
        //
        return(lTemp);
    }
    else
    {
        //
        // There are no characters, so return a failure.
        //
        return(-1);
    }
}

//*****************************************************************************
//
//! Waits for a character from the specified port.
//!
//! \param pUART specifies the SoftUART data structure.
//!
//! Gets a character from the receive buffer for the specified port.  If there
//! are no characters available, this function waits until a character is
//! received before returning.
//!
//! \return Returns the character read from the specified port, cast as a
//! \e long.
//
//*****************************************************************************
long
SoftUARTCharGet(tSoftUART *pUART)
{
    long lTemp;

    //
    // Wait until a char is available.
    //
    while(pUART->usRxBufferRead ==
          *(volatile unsigned short *)(&(pUART->usRxBufferWrite)))
    {
    }

    //
    // Read the next character.
    //
    lTemp = pUART->pusRxBuffer[pUART->usRxBufferRead];
    pUART->usRxBufferRead++;
    if(pUART->usRxBufferRead == pUART->usRxBufferLen)
    {
        pUART->usRxBufferRead = 0;
    }

    //
    // Deassert the receive "interrupt(s)" if appropriate.
    //
    SoftUARTRxReadInt(pUART);

    //
    // Set the receive status to match this character.
    //
    pUART->ucRxStatus = ((pUART->ucRxStatus & SOFTUART_RXERROR_OVERRUN) |
                         ((lTemp >> 8) & ~(SOFTUART_RXERROR_OVERRUN)));

    //
    // Return this character.
    //
    return(lTemp);
}

//*****************************************************************************
//
//! Sends a character to the specified port.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param ucData is the character to be transmitted.
//!
//! Writes the character \e ucData to the transmit buffer for the specified
//! port.  This function does not block, so if there is no space available,
//! then a \b false is returned, and the application must retry the function
//! later.
//!
//! \return Returns \b true if the character was successfully placed in the
//! transmit buffer or \b false if there was no space available in the
//! transmit buffer.
//
//*****************************************************************************
tBoolean
SoftUARTCharPutNonBlocking(tSoftUART *pUART, unsigned char ucData)
{
    unsigned short usTemp;

    //
    // Determine the values of the write pointer once incremented.
    //
    usTemp = pUART->usTxBufferWrite + 1;
    if(usTemp == pUART->usTxBufferLen)
    {
        usTemp = 0;
    }

    //
    // See if there is space in the transmit buffer.
    //
    if(usTemp != pUART->usTxBufferRead)
    {
        //
        // Write this character to the transmit buffer.
        //
        pUART->pucTxBuffer[pUART->usTxBufferWrite] = ucData;
        pUART->usTxBufferWrite = usTemp;

        //
        // Success.
        //
        return(true);
    }
    else
    {
        //
        // There is no space in the transmit buffer, so return a failure.
        //
        return(false);
    }
}

//*****************************************************************************
//
//! Waits to send a character from the specified port.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param ucData is the character to be transmitted.
//!
//! Sends the character \e ucData to the transmit buffer for the specified
//! port.  If there is no space available in the transmit buffer, this function
//! waits until there is space available before returning.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTCharPut(tSoftUART *pUART, unsigned char ucData)
{
    unsigned short usTemp;

    //
    // Wait until space is available.
    //
    usTemp = pUART->usTxBufferWrite + 1;
    if(usTemp == pUART->usTxBufferLen)
    {
        usTemp = 0;
    }
    while(usTemp == *(volatile unsigned short *)(&(pUART->usTxBufferRead)))
    {
    }

    //
    // Send the char.
    //
    pUART->pucTxBuffer[pUART->usTxBufferWrite] = ucData;
    pUART->usTxBufferWrite = usTemp;
}

//*****************************************************************************
//
//! Causes a BREAK to be sent.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param bBreakState controls the output level.
//!
//! Calling this function with \e bBreakState set to \b true asserts a break
//! condition on the SoftUART.  Calling this function with \e bBreakState set
//! to \b false removes the break condition.  For proper transmission of a
//! break command, the break must be asserted for at least two complete frames.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTBreakCtl(tSoftUART *pUART, tBoolean bBreakState)
{
    //
    // Set the break condition as requested.
    //
    if(bBreakState)
    {
        pUART->ucFlags |= SOFTUART_FLAG_TXBREAK;
    }
    else
    {
        pUART->ucFlags &= ~(SOFTUART_FLAG_TXBREAK);
    }
}

//*****************************************************************************
//
//! Determines whether the UART transmitter is busy or not.
//!
//! \param pUART specifies the SoftUART data structure.
//!
//! Allows the caller to determine whether all transmitted bytes have cleared
//! the transmitter hardware.  If \b false is returned, the transmit buffer is
//! empty and all bits of the last transmitted character, including all stop
//! bits, have left the hardware shift register.
//!
//! \return Returns \b true if the UART is transmitting or \b false if all
//! transmissions are complete.
//
//*****************************************************************************
tBoolean
SoftUARTBusy(tSoftUART *pUART)
{
    //
    // Determine if the UART is busy.
    //
    return(((pUART->ucTxState == SOFTUART_TXSTATE_IDLE) &&
            (((pUART->ucFlags & SOFTUART_FLAG_ENABLE) == 0) ||
             (pUART->usTxBufferRead == pUART->usTxBufferWrite))) ?
           false : true);
}

//*****************************************************************************
//
//! Enables individual SoftUART ``interrupt'' sources.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param ulIntFlags is the bit mask of the ``interrupt'' sources to be
//! enabled.
//!
//! Enables the indicated SoftUART ``interrupt'' sources.  Only the sources
//! that are enabled can be reflected to the SoftUART callback.
//!
//! The \e ulIntFlags parameter is the logical OR of any of the following:
//!
//! - \b SOFTUART_INT_OE - Overrun Error ``interrupt''
//! - \b SOFTUART_INT_BE - Break Error ``interrupt''
//! - \b SOFTUART_INT_PE - Parity Error ``interrupt''
//! - \b SOFTUART_INT_FE - Framing Error ``interrupt''
//! - \b SOFTUART_INT_RT - Receive Timeout ``interrupt''
//! - \b SOFTUART_INT_TX - Transmit ``interrupt''
//! - \b SOFTUART_INT_RX - Receive ``interrupt''
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTIntEnable(tSoftUART *pUART, unsigned long ulIntFlags)
{
    //
    // Enable the specified interrupts.
    //
    pUART->usIntMask |= ulIntFlags;
}

//*****************************************************************************
//
//! Disables individual SoftUART ``interrupt'' sources.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param ulIntFlags is the bit mask of the ``interrupt'' sources to be
//! disabled.
//!
//! Disables the indicated SoftUART ``interrupt'' sources.  Only the sources
//! that are enabled can be reflected to the SoftUART callback.
//!
//! The \e ulIntFlags parameter has the same definition as the \e ulIntFlags
//! parameter to SoftUARTIntEnable().
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTIntDisable(tSoftUART *pUART, unsigned long ulIntFlags)
{
    //
    // Disable the specified interrupts.
    //
    pUART->usIntMask &= ~(ulIntFlags);
}

//*****************************************************************************
//
//! Gets the current SoftUART ``interrupt'' status.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param bMasked is \b false if the raw ``interrupt'' status is required and
//! \b true if the masked ``interrupt'' status is required.
//!
//! This returns the ``interrupt'' status for the SoftUART.  Either the raw
//! ``interrupt'' status or the status of ``interrupts'' that are allowed to
//! reflect to the SoftUART callback can be returned.
//!
//! \return Returns the current ``interrupt'' status, enumerated as a bit field
//! of values described in SoftUARTIntEnable().
//
//*****************************************************************************
unsigned long
SoftUARTIntStatus(tSoftUART *pUART, tBoolean bMasked)
{
    //
    // Return either the interrupt status or the raw interrupt status as
    // requested.
    //
    if(bMasked)
    {
        return(pUART->usIntStatus & pUART->usIntMask);
    }
    else
    {
        return(pUART->usIntStatus);
    }
}

//*****************************************************************************
//
//! Clears SoftUART ``interrupt'' sources.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param ulIntFlags is a bit mask of the ``interrupt'' sources to be cleared.
//!
//! The specified SoftUART ``interrupt'' sources are cleared, so that they no
//! longer assert.  This function must be called in the callback function to
//! keep the ``interrupt'' from being recognized again immediately upon exit.
//!
//! The \e ulIntFlags parameter has the same definition as the \e ulIntFlags
//! parameter to SoftUARTIntEnable().
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTIntClear(tSoftUART *pUART, unsigned long ulIntFlags)
{
    //
    // Clear the requested interrupt sources.
    //
    pUART->usIntStatus &= ~(ulIntFlags);
}

//*****************************************************************************
//
//! Gets current receiver errors.
//!
//! \param pUART specifies the SoftUART data structure.
//!
//! This function returns the current state of each of the 4 receiver error
//! sources.  The returned errors are equivalent to the four error bits
//! returned via the previous call to SoftUARTCharGet() or
//! SoftUARTCharGetNonBlocking() with the exception that the overrun error is
//! set immediately when the overrun occurs rather than when a character is
//! next read.
//!
//! \return Returns a logical OR combination of the receiver error flags,
//! \b SOFTUART_RXERROR_FRAMING, \b SOFTUART_RXERROR_PARITY,
//! \b SOFTUART_RXERROR_BREAK and \b SOFTUART_RXERROR_OVERRUN.
//
//*****************************************************************************
unsigned long
SoftUARTRxErrorGet(tSoftUART *pUART)
{
    //
    // Return the current value of the receive status.
    //
    return(pUART->ucRxStatus);
}

//*****************************************************************************
//
//! Clears all reported receiver errors.
//!
//! \param pUART specifies the SoftUART data structure.
//!
//! This function is used to clear all receiver error conditions reported via
//! SoftUARTRxErrorGet().  If using the overrun, framing error, parity error or
//! break interrupts, this function must be called after clearing the interrupt
//! to ensure that later errors of the same type trigger another interrupt.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTRxErrorClear(tSoftUART *pUART)
{
    //
    // Clear any receive error status.
    //
    pUART->ucRxStatus = 0;
}

//*****************************************************************************
//
//! Sets the callback used by the SoftUART module.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param pfnCallback is a pointer to the callback function.
//!
//! This function sets the address of the callback function that is called when
//! there is an ``interrupt'' produced by the SoftUART module.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTCallbackSet(tSoftUART *pUART, void (*pfnCallback)(void))
{
    //
    // Save the callback function address.
    //
    pUART->pfnIntCallback = pfnCallback;
}

//*****************************************************************************
//
//! Sets the GPIO pin to be used as the SoftUART Tx signal.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param ulBase is the base address of the GPIO module.
//! \param ucPin is the bit-packed representation of the pin to use.
//!
//! This function sets the GPIO pin that is used when the SoftUART must assert
//! the Tx signal.
//!
//! The pin is specified using a bit-packed byte, where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTTxGPIOSet(tSoftUART *pUART, unsigned long ulBase, unsigned char ucPin)
{
    //
    // Save the base address and pin for the Tx signal.
    //
    if(ulBase == 0)
    {
        pUART->ulTxGPIO = 0;
    }
    else
    {
        pUART->ulTxGPIO = ulBase + (ucPin << 2);
    }
}

//*****************************************************************************
//
//! Sets the GPIO pin to be used as the SoftUART Rx signal.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param ulBase is the base address of the GPIO module.
//! \param ucPin is the bit-packed representation of the pin to use.
//!
//! This function sets the GPIO pin that is used when the SoftUART must sample
//! the Rx signal.  If there is not a GPIO pin allocated for Rx, the SoftUART
//! module will not read data from the slave device.
//!
//! The pin is specified using a bit-packed byte, where bit 0 of the byte
//! represents GPIO port pin 0, bit 1 represents GPIO port pin 1, and so on.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTRxGPIOSet(tSoftUART *pUART, unsigned long ulBase, unsigned char ucPin)
{
    //
    // Save the base address and pin for the Rx signal.
    //
    if(ulBase == 0)
    {
        pUART->ulRxGPIOPort = 0;
        pUART->ucRxPin = 0;
    }
    else
    {
        pUART->ulRxGPIOPort = ulBase;
        pUART->ucRxPin = ucPin;
    }
}

//*****************************************************************************
//
//! Sets the transmit buffer for a SoftUART module.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param pucTxBuffer is the address of the transmit buffer.
//! \param usLen is the size, in 8-bit bytes, of the transmit buffer.
//!
//! This function sets the address and size of the transmit buffer.  It also
//! resets the read and write pointers, marking the transmit buffer as empty.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTTxBufferSet(tSoftUART *pUART, unsigned char *pucTxBuffer,
                   unsigned short usLen)
{
    //
    // Save the transmit buffer address and length.
    //
    pUART->pucTxBuffer = pucTxBuffer;
    pUART->usTxBufferLen = usLen;

    //
    // Reset the transmit buffer read and write pointers.
    //
    pUART->usTxBufferRead = 0;
    pUART->usTxBufferWrite = 0;

    //
    // Compute the new buffer "interrupt" level.
    //
    SoftUARTTxLevelSet(pUART);
}

//*****************************************************************************
//
//! Sets the receive buffer for a SoftUART module.
//!
//! \param pUART specifies the SoftUART data structure.
//! \param pusRxBuffer is the address of the receive buffer.
//! \param usLen is the size, in 16-bit half-words, of the receive buffer.
//!
//! This function sets the address and size of the receive buffer.  It also
//! resets the read and write pointers, marking the receive buffer as empty.
//!
//! \return None.
//
//*****************************************************************************
void
SoftUARTRxBufferSet(tSoftUART *pUART, unsigned short *pusRxBuffer,
                   unsigned short usLen)
{
    //
    // Save the receive buffer address and length.
    //
    pUART->pusRxBuffer = pusRxBuffer;
    pUART->usRxBufferLen = usLen;

    //
    // Reset the receive read and write pointers.
    //
    pUART->usRxBufferRead = 0;
    pUART->usRxBufferWrite = 0;

    //
    // Compute the new buffer "interrupt" level.
    //
    SoftUARTRxLevelSet(pUART);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
