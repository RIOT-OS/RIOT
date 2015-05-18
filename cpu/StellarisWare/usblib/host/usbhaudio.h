//*****************************************************************************
//
// usbhaudio.h - USB host audio class driver.
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
// This is part of revision 9453 of the Stellaris USB Library.
//
//*****************************************************************************

#ifndef __USBHAUDIO_H__
#define __USBHAUDIO_H__

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

typedef void (* tUSBHostAudioCallback)(void *pvBuffer,
                                       unsigned long ulParam,
                                       unsigned long ulEvent);

//*****************************************************************************
//
//! This is the size in bytes of the private data for the host audio class.
//
//*****************************************************************************
#define USB_HOST_AUDIO_INSTANCE_SIZE sizeof(tHostAudioInstance);

//*****************************************************************************
//
// USB host audio specific events
//
//*****************************************************************************

//*****************************************************************************
//
//! This USB host audio event indicates that the device is connected and
//! ready to send or receive buffers.  The \e pvBuffer and \e ulParam
//! values are not used in this event.
//
//*****************************************************************************
#define USBH_AUDIO_EVENT_OPEN   (USBH_AUDIO_EVENT_BASE + 0)

//*****************************************************************************
//
//! This USB host audio event indicates that the previously connected device
//! has been disconnected. The \e pvBuffer and \e ulParam values are not used
//! in this event.
//
//*****************************************************************************
#define USBH_AUDIO_EVENT_CLOSE  (USBH_AUDIO_EVENT_BASE + 1)

//*****************************************************************************
//
// This definition is used with the USBHostAudioFormatGet() and
// USBHostAudioFormatSet() API's to determine if the audio input is being
// accesses(USBH_AUDIO_FORMAT_IN set) or audio output(USBH_AUDIO_FORMAT clear).
//
//*****************************************************************************
#define USBH_AUDIO_FORMAT_IN    0x00000001
#define USBH_AUDIO_FORMAT_OUT   0x00000000

typedef struct
{
    unsigned char ucChannels;
    unsigned char ucBits;
    unsigned long ulSampleRate;
} tUSBAudioFormat;

//*****************************************************************************
//
// API Function Prototypes
//
//*****************************************************************************
extern unsigned long USBHostAudioOpen(unsigned long ulIndex,
                                      tUSBHostAudioCallback pfnCallback);
extern void USBHostAudioClose(unsigned long ulInstance);
extern long USBHostAudioPlay(unsigned long ulInstance, void *pvBuffer,
                             unsigned long ulSize,
                             tUSBHostAudioCallback pfnCallback);

extern unsigned long USBHostAudioFormatGet(unsigned long ulInstance,
                                           unsigned long ulSampleRate,
                                           unsigned long ulBits,
                                           unsigned long ulChannels,
                                           unsigned long ulFlags);
extern unsigned long USBHostAudioFormatSet(unsigned long ulInstance,
                                           unsigned long ulSampleRate,
                                           unsigned long ulBits,
                                           unsigned long ulChannels,
                                           unsigned long ulFlags);

extern long USBHostAudioRecord(unsigned long ulInstance, void *pvBuffer,
                               unsigned long ulSize,
                               tUSBHostAudioCallback);

extern unsigned long USBHostAudioVolumeGet(unsigned long ulInstance,
                                           unsigned long ulInterface,
                                           unsigned long ulChannel);

extern void USBHostAudioVolumeSet(unsigned long ulInstance,
                                  unsigned ulInterface,
                                  unsigned long ulChannel,
                                  unsigned long ulValue);

extern unsigned long USBHostAudioVolumeMaxGet(unsigned long ulInstance,
                                              unsigned long ulInterface,
                                              unsigned long ulChannel);

extern unsigned long USBHostAudioVolumeMinGet(unsigned long ulInstance,
                                              unsigned long ulInterface,
                                              unsigned long ulChannel);

extern unsigned long USBHostAudioVolumeResGet(unsigned long ulInstance,
                                              unsigned long ulInterface,
                                              unsigned long ulChannel);

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

#endif

