//*****************************************************************************
//
// softssi.h - Defines and macros for the SoftSSI.
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

#ifndef __SOFTSSI_H__
#define __SOFTSSI_H__

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
//! \addtogroup softssi_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! This structure contains the state of a single instance of a SoftSSI module.
//
//*****************************************************************************
typedef struct
{
    //
    //! The address of the callback function that is called to simulate the
    //! interrupts that would be produced by a hardware SSI implementation.
    //! This address can be set via a direct structure access or using the
    //! SoftSSICallbackSet function.
    //
    void (*pfnIntCallback)(void);

    //
    //! The address of the GPIO pin to be used for the Fss signal.  If this
    //! member is zero, the Fss signal is not generated.  This member can be
    //! set via a direct structure access or using the SoftSSIFssGPIOSet
    //! function.
    ///
    unsigned long ulFssGPIO;

    //
    //! The address of the GPIO pin to be used for the Clk signal.  This member
    //! can be set via a direct structure access or using the SoftSSIClkGPIOSet
    //! function.
    //
    unsigned long ulClkGPIO;

    //
    //! The address of the GPIO pin to be used for the Tx signal.  This member
    //! can be set via a direct structure access or using the SoftSSITxGPIOSet
    //! function.
    //
    unsigned long ulTxGPIO;

    //
    //! The address of the GPIO pin to be used for the Rx signal.  If this
    //! member is zero, the Rx signal is not read.  This member can be set via
    //! a direct structure access or using the SoftSSIRxGPIOSet function.
    //
    unsigned long ulRxGPIO;

    //
    //! The address of the data buffer used for the transmit FIFO.  This member
    //! can be set via a direct structure access or using the
    //! SoftSSITxBufferSet function.
    //
    unsigned short *pusTxBuffer;

    //
    //! The address of the data buffer used for the receive FIFO.  This member
    //! can be set via a direct structure access or using the
    //! SoftSSIRxBufferSet function.
    //
    unsigned short *pusRxBuffer;

    //
    //! The length of the transmit FIFO.  This member can be set via a direct
    //! structure access or using the SoftSSITxBufferSet function.
    //
    unsigned short usTxBufferLen;

    //
    //! The index into the transmit FIFO of the next word to be transmitted.
    //! This member should be initialized to zero, but should not be accessed
    //! or modified by the application.
    //
    unsigned short usTxBufferRead;

    //
    //! The index into the transmit FIFO of the next location to store data
    //! into the FIFO.  This member should be initialized to zero, but should
    //! not be accessed or modified by the application.
    //
    unsigned short usTxBufferWrite;

    //
    //! The length of the receive FIFO.  This member can be set via a direct
    //! structure access or using the SoftSSIRxBufferSet function.
    //
    unsigned short usRxBufferLen;

    //
    //! The index into the receive FIFO of the next word to be read from the
    //! FIFO.  This member should be initialized to zero, but should not be
    //! accessed or modified by the application.
    //
    unsigned short usRxBufferRead;

    //
    //! The index into the receive FIFO of the location to store the next word
    //! received.  This member should be initialized to zero, but should not be
    //! accessed or modified by the application.
    //
    unsigned short usRxBufferWrite;

    //
    //! The word that is currently being transmitted.  This member should not
    //! be accessed or modified by the application.
    //
    unsigned short usTxData;

    //
    //! The word that is currently being received.  This member should not be
    //! accessed or modified by the application.
    //
    unsigned short usRxData;

    //
    //! The flags that control the operation of the SoftSSI module.  This
    //! member should not be accessed or modified by the application.
    //
    unsigned char ucFlags;

    //
    //! The number of data bits in each SoftSSI frame, which also specifies the
    //! width of each data item in the transmit and receive FIFOs.  This member
    //! can be set via a direct structure access or using the SoftSSIConfigSet
    //! function.
    //
    unsigned char ucBits;

    //
    //! The current state of the SoftSSI state machine.  This member should not
    //! be accessed or modified by the application.
    //
    unsigned char ucState;

    //
    //! The number of bits that have been transmitted and received in the
    //! current frame.  This member should not be accessed or modified by the
    //! application.
    //
    unsigned char ucCurrentBit;

    //
    //! The set of virtual interrupts that should be sent to the callback
    //! function.  This member should not be accessed or modified by the
    //! application.
    //
    unsigned char ucIntMask;

    //
    //! The set of virtual interrupts that are currently asserted.  This member
    //! should not be accessed or modified by the application.
    //
    unsigned char ucIntStatus;

    //
    //! The number of tick counts that the SoftSSI module has been idle with
    //! data stored in the receive FIFO, which is used to generate the receive
    //! timeout interrupt.  This member should not be accessed or modified by
    //! the application.
    //
    unsigned char ucIdleCount;
}
tSoftSSI;

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Values that can be passed to SoftSSIIntEnable, SoftSSIIntDisable, and
// SoftSSIIntClear as the ulIntFlags parameter, and returned by
// SoftSSIIntStatus.
//
//*****************************************************************************
#define SOFTSSI_TXEOT           0x00000010  // TX end of transmit
#define SOFTSSI_TXFF            0x00000008  // TX FIFO half full or less
#define SOFTSSI_RXFF            0x00000004  // RX FIFO half full or more
#define SOFTSSI_RXTO            0x00000002  // RX timeout
#define SOFTSSI_RXOR            0x00000001  // RX overrun

//*****************************************************************************
//
// Values that can be passed to SoftSSIConfigSet.
//
//*****************************************************************************
#define SOFTSSI_FRF_MOTO_MODE_0 0x00000000  // Moto fmt, polarity 0, phase 0
#define SOFTSSI_FRF_MOTO_MODE_1 0x00000002  // Moto fmt, polarity 0, phase 1
#define SOFTSSI_FRF_MOTO_MODE_2 0x00000001  // Moto fmt, polarity 1, phase 0
#define SOFTSSI_FRF_MOTO_MODE_3 0x00000003  // Moto fmt, polarity 1, phase 1

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern tBoolean SoftSSIBusy(tSoftSSI *pSSI);
extern void SoftSSICallbackSet(tSoftSSI *pSSI, void (*pfnCallback)(void));
extern void SoftSSIClkGPIOSet(tSoftSSI *pSSI, unsigned long ulBase,
                              unsigned char ucPin);
extern void SoftSSIConfigSet(tSoftSSI *pSSI, unsigned char ucProtocol,
                             unsigned char ucBits);
extern tBoolean SoftSSIDataAvail(tSoftSSI *pSSI);
extern void SoftSSIDataGet(tSoftSSI *pSSI, unsigned long *pulData);
extern long SoftSSIDataGetNonBlocking(tSoftSSI *pSSI, unsigned long *pulData);
extern void SoftSSIDataPut(tSoftSSI *pSSI, unsigned long ulData);
extern long SoftSSIDataPutNonBlocking(tSoftSSI *pSSI, unsigned long ulData);
extern void SoftSSIDisable(tSoftSSI *pSSI);
extern void SoftSSIEnable(tSoftSSI *pSSI);
extern void SoftSSIFssGPIOSet(tSoftSSI *pSSI, unsigned long ulBase,
                              unsigned char ucPin);
extern void SoftSSIIntClear(tSoftSSI *pSSI, unsigned long ulIntFlags);
extern void SoftSSIIntDisable(tSoftSSI *pSSI, unsigned long ulIntFlags);
extern void SoftSSIIntEnable(tSoftSSI *pSSI, unsigned long ulIntFlags);
extern unsigned long SoftSSIIntStatus(tSoftSSI *pSSI, tBoolean bMasked);
extern void SoftSSIRxBufferSet(tSoftSSI *pSSI, unsigned short *pusRxBuffer,
                               unsigned short usLen);
extern void SoftSSIRxGPIOSet(tSoftSSI *pSSI, unsigned long ulBase,
                             unsigned char ucPin);
extern tBoolean SoftSSISpaceAvail(tSoftSSI *pSSI);
extern void SoftSSITimerTick(tSoftSSI *pSSI);
extern void SoftSSITxBufferSet(tSoftSSI *pSSI, unsigned short *pusTxBuffer,
                               unsigned short usLen);
extern void SoftSSITxGPIOSet(tSoftSSI *pSSI, unsigned long ulBase,
                             unsigned char ucPin);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __SOFTSSI_H__
