//*****************************************************************************
//
// bl_uart.c - Functions to transfer data via the UART port.
//
// Copyright (c) 2006-2012 Texas Instruments Incorporated.  All rights reserved.
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

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "bl_config.h"
#include "boot_loader/bl_uart.h"

//*****************************************************************************
//
//! \addtogroup bl_uart_api
//! @{
//
//*****************************************************************************
#if defined(UART_ENABLE_UPDATE) || defined(DOXYGEN)

//*****************************************************************************
//
//! Sends data over the UART port.
//!
//! \param pucData is the buffer containing the data to write out to the UART
//! port.
//! \param ulSize is the number of bytes provided in \e pucData buffer that
//! will be written out to the UART port.
//!
//! This function sends \e ulSize bytes of data from the buffer pointed to by
//! \e pucData via the UART port.
//!
//! \return None.
//
//*****************************************************************************
void
UARTSend(const unsigned char *pucData, unsigned long ulSize)
{
    //
    // Transmit the number of bytes requested on the UART port.
    //
    while(ulSize--)
    {
        //
        // Make sure that the transmit FIFO is not full.
        //
        while((HWREG(UART0_BASE + UART_O_FR) & UART_FR_TXFF))
        {
        }

        //
        // Send out the next byte.
        //
        HWREG(UART0_BASE + UART_O_DR) = *pucData++;
    }

    //
    // Wait until the UART is done transmitting.
    //
    UARTFlush();
}

//*****************************************************************************
//
//! Waits until all data has been transmitted by the UART port.
//!
//! This function waits until all data written to the UART port has been
//! transmitted.
//!
//! \return None.
//
//*****************************************************************************
void
UARTFlush(void)
{
    //
    // Wait for the UART FIFO to empty and then wait for the shifter to get the
    // bytes out the port.
    //
    while(!(HWREG(UART0_BASE + UART_O_FR) & UART_FR_TXFE))
    {
    }

    //
    // Wait for the FIFO to not be busy so that the shifter completes.
    //
    while((HWREG(UART0_BASE + UART_O_FR) & UART_FR_BUSY))
    {
    }
}

//*****************************************************************************
//
//! Receives data over the UART port.
//!
//! \param pucData is the buffer to read data into from the UART port.
//! \param ulSize is the number of bytes provided in the \e pucData buffer that
//! should be written with data from the UART port.
//!
//! This function reads back \e ulSize bytes of data from the UART port, into
//! the buffer that is pointed to by \e pucData.  This function will not return
//! until \e ulSize number of bytes have been received.
//!
//! \return None.
//
//*****************************************************************************
void
UARTReceive(unsigned char *pucData, unsigned long ulSize)
{
    //
    // Send out the number of bytes requested.
    //
    while(ulSize--)
    {
        //
        // Wait for the FIFO to not be empty.
        //
        while((HWREG(UART0_BASE + UART_O_FR) & UART_FR_RXFE))
        {
        }

        //
        // Receive a byte from the UART.
        //
        *pucData++ = HWREG(UART0_BASE + UART_O_DR);
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
#endif
