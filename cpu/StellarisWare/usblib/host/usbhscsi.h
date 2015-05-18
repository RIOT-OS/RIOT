//*****************************************************************************
//
// usbhscsi.h - Definitions for the USB host SCSI layer.
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

#ifndef __USBHSCSI_H__
#define __USBHSCSI_H__

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
//! \addtogroup usblib_host_class
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Prototypes for the APIs exported by the USB SCSI layer.
//
//*****************************************************************************
extern unsigned long USBHSCSIInquiry(unsigned long ulInPipe,
                                     unsigned long ulOutPipe,
                                     unsigned char *pucBuffer,
                                     unsigned long *pulSize);
extern unsigned long USBHSCSIReadCapacity(unsigned long ulInPipe,
                                          unsigned long ulOutPipe,
                                          unsigned char *pData,
                                          unsigned long *pulSize);
extern unsigned long USBHSCSIReadCapacities(unsigned long ulInPipe,
                                            unsigned long ulOutPipe,
                                            unsigned char *pData,
                                            unsigned long *pulSize);
extern unsigned long USBHSCSIModeSense6(unsigned long ulInPipe,
                                        unsigned long ulOutPipe,
                                        unsigned long ulFlags,
                                        unsigned char *pData,
                                        unsigned long *pulSize);
extern unsigned long USBHSCSITestUnitReady(unsigned long ulInPipe,
                                           unsigned long ulOutPipe);
extern unsigned long USBHSCSIRequestSense(unsigned long ulInPipe,
                                          unsigned long ulOutPipe,
                                          unsigned char *pucData,
                                          unsigned long *pulSize);
extern unsigned long USBHSCSIRead10(unsigned long ulInPipe,
                                    unsigned long ulOutPipe,
                                    unsigned long ulLBA,
                                    unsigned char *pucData,
                                    unsigned long *pulSize,
                                    unsigned long ulNumBlocks);
extern unsigned long USBHSCSIWrite10(unsigned long ulInPipe,
                                     unsigned long ulOutPipe,
                                     unsigned long ulLBA,
                                     unsigned char *pucData,
                                     unsigned long *pulSize,
                                     unsigned long ulNumBlocks);

//*****************************************************************************
//
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __USBHSCSI_H__
