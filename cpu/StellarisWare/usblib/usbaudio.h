//*****************************************************************************
//
// usbaudio.h - Definitions used by Audio Class devices.
//
// Copyright (c) 2009-2012 Texas Instruments Incorporated.  All rights reserved.
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

#ifndef __USBAUDIO_H__
#define __USBAUDIO_H__


//*****************************************************************************
//
// Standard Audio descriptor sub types.
//
//*****************************************************************************
#define USB_AI_UNDEFINED        0
#define USB_AI_HEADER           1
#define USB_AI_INPUT_TERMINAL   2
#define USB_AI_OUTPUT_TERMINAL  3
#define USB_AI_MIXER_UNIT       4
#define USB_AI_SELECTOR_UNIT    5
#define USB_AI_FEATURE_UNIT     6
#define USB_AI_PROCESSING_UNIT  7
#define USB_AI_EXTENSION_UNIT   8

//*****************************************************************************
//
// Standard Audio Streaming Interface descriptor types.
//
//*****************************************************************************
#define USB_AS_UNDEFINED        0
#define USB_AS_GENERAL          1
#define USB_AS_FORMAT_TYPE      2
#define USB_AS_FORMAT_SPECIFIC  3

//*****************************************************************************
//
// Standard USB terminal types used with audio terminal descriptors.  These
// are defined in the "Universal Serial Bus Device Class Definition for
// Terminal Types" version 1.0 released March 18, 1998.
//
//*****************************************************************************
#define USB_TTYPE_UNDEFINED     0x0100
#define USB_TTYPE_STREAMING     0x0101
#define USB_TTYPE_VENDOR        0x01ff

#define USB_TTYPE_OUT_UNDEF     0x0300
#define USB_TTYPE_OUT_SPEAKER   0x0301
#define USB_TTYPE_OUT_HEADPHONE 0x0302
#define USB_TTYPE_OUT_DESK_SPKR 0x0304
#define USB_TTYPE_OUT_ROOM_SPKR 0x0305
#define USB_TTYPE_OUT_COMM_SPKR 0x0306
#define USB_TTYPE_OUT_LFE       0x0307

#define USB_TTYPE_EXT_UNDEF     0x0600
#define USB_TTYPE_EXT_ANALOG    0x0601
#define USB_TTYPE_EXT_DIGITAL   0x0602
#define USB_TTYPE_EXT_LINE      0x0603
#define USB_TTYPE_EXT_LEGACY    0x0604
#define USB_TTYPE_EXT_SPDIF     0x0605
#define USB_TTYPE_EXT_1394_DA   0x0606
#define USB_TTYPE_EXT_1394_DV   0x0607

//*****************************************************************************
//
// Audio Interface Subclass Codes
//
//*****************************************************************************
#define USB_ASC_UNDEFINED       0x00
#define USB_ASC_AUDIO_CONTROL   0x01
#define USB_ASC_AUDIO_STREAMING 0x02
#define USB_ASC_MIDI_STREAMING  0x03

//*****************************************************************************
//
// Audio Class-Specific Descriptor Types
// (Table A-4)
//
//*****************************************************************************
#define USB_ACSDT_UNDEFINED     0x20
#define USB_ACSDT_DEVICE        0x21
#define USB_ACSDT_CONFIGURATION 0x22
#define USB_ACSDT_STRING        0x23
#define USB_ACSDT_INTERFACE     0x24
#define USB_ACSDT_ENDPOINT      0x25

//*****************************************************************************
//
// Audio Class-Specific AC Interface Descriptor Subtypes
// (Table A-5)
//
//*****************************************************************************
#define USB_ACDSTYPE_UNDEFINED       0x00
#define USB_ACDSTYPE_HEADER          0x01
#define USB_ACDSTYPE_IN_TERMINAL     0x02
#define USB_ACDSTYPE_OUT_TERMINAL    0x03
#define USB_ACDSTYPE_MIXER_UNIT      0x04
#define USB_ACDSTYPE_SELECTOR_UNIT   0x05
#define USB_ACDSTYPE_FEATURE_UNIT    0x06
#define USB_ACDSTYPE_PROCESSING_UNIT 0x07
#define USB_ACDSTYPE_EXTENSION_UNIT  0x08

//*****************************************************************************
//
// Audio Class-Specific AS Interface Descriptor Subtypes
// (Table A-6)
//
//*****************************************************************************
#define USB_ASDSTYPE_UNDEFINED          0x00
#define USB_ASDSTYPE_GENERAL            0x01
#define USB_ASDSTYPE_FORMAT_TYPE        0x02
#define USB_ASDSTYPE_FORMAT_SPECIFIC    0x03

//*****************************************************************************
//
// Audio Data Format Type I Codes.
//
//*****************************************************************************
#define USB_ADF_UNDEFINED       0x0000
#define USB_ADF_PCM             0x0001
#define USB_ADF_PCM8            0x0002
#define USB_ADF_IEEE_FLOAT      0x0003
#define USB_ADF_ALAW            0x0004
#define USB_ADF_MULAW           0x0005

//*****************************************************************************
//
// Audio Format Type Codes
//
//*****************************************************************************
#define USB_AF_TYPE_UNDEFINED   0x00
#define USB_AF_TYPE_TYPE_I      0x01
#define USB_AF_TYPE_TYPE_II     0x02
#define USB_AF_TYPE_TYPE_III    0x03

//*****************************************************************************
//
// Audio Class-Specific controls used with bmaControls values.
//
//*****************************************************************************
#define USB_ACONTROL_MUTE       0x0001  // Mute
#define USB_ACONTROL_VOLUME     0x0002  // Volume
#define USB_ACONTROL_BASS       0x0004  // Bass
#define USB_ACONTROL_MID        0x0008  // Mid
#define USB_ACONTROL_TREBLE     0x0010  // Treble
#define USB_ACONTROL_EQ         0x0020  // Graphic Equalizer
#define USB_ACONTROL_AGC        0x0040  // Automatic Gain
#define USB_ACONTROL_DELAY      0x0080  // Delay
#define USB_ACONTROL_BASS_BOOST 0x0100  // Bass Boost
#define USB_ACONTROL_LOUD       0x0200  // Loudness

//*****************************************************************************
//
// Audio Class-Specific Output terminal types.
//
//*****************************************************************************
#define USB_ATTYPE_UNDEFINED    0x0300  // Output Terminal, undefined Type.
#define USB_ATTYPE_SPEAKER      0x0301  // A generic speaker.
#define USB_ATTYPE_HEADPHONES   0x0302  // A head-mounted audio output device.
#define USB_ATTYPE_HMD          0x0303  // The audio part of a VR head mounted
                                        // display.
#define USB_ATTYPE_SPEAKER_DT   0x0304  // Desktop or Monitor speaker(s).
#define USB_ATTYPE_SPEAKER_RM   0x0305  // Larger room speaker(s).
#define USB_ATTYPE_SPEAKER_COM  0x0306  // Communications Speaker (phone).
#define USB_ATTYPE_SPEAKER_LFE  0x0307  // Speaker designed for low frequencies.

//*****************************************************************************
//
// USB Audio channel configuration bits for wChannelConfig values.
// wChannelConfig: a bit field that indicates which spatial locations are
// present in the cluster. The bit allocations are as follows:
//
//*****************************************************************************
#define USB_CHANNEL_L           0x0001  // Left Front (L)
#define USB_CHANNEL_R           0x0002  // Right Front (R)
#define USB_CHANNEL_C           0x0004  // Center Front (C)
#define USB_CHANNEL_LFE         0x0008  // Low Frequency Enhancement (LFE)
#define USB_CHANNEL_LS          0x0010  // Left Surround (LS)
#define USB_CHANNEL_RS          0x0020  // Right Surround (RS)
#define USB_CHANNEL_LC          0x0040  // Left of Center (LC)
#define USB_CHANNEL_RC          0x0080  // Right of Center (RC)
#define USB_CHANNEL_S           0x0100  // Surround (S)
#define USB_CHANNEL_SL          0x0200  // Side Left (SL)
#define USB_CHANNEL_SR          0x0400  // Side Right (SR)
#define USB_CHANNEL_T           0x0800  // Top (T)

//*****************************************************************************
//
// Endpoint attributes for Audio Class General type.
//
//*****************************************************************************
#define USB_EP_ATTR_ACG_SAMPLING    0x01    // Sampling Frequency
#define USB_EP_ATTR_ACG_PITCH       0x02    // Pitch
#define USB_EP_ATTR_ACG_MAXPACKET   0x80    // MaxPacketsOnly

//*****************************************************************************
//
// Indicates the units used for the wLockDelay field for Audio Class General
// type.
//
//*****************************************************************************
#define USB_EP_LOCKDELAY_UNDEF  0x00    // Undefined
#define USB_EP_LOCKDELAY_MS     0x01    // Milliseconds
#define USB_EP_LOCKDELAY_PCM    0x02    // Decoded PCM samples

//*****************************************************************************
//
// Audio Class-Specific Request Codes
//
//*****************************************************************************
#define USB_AC_SET_CUR          0x01
#define USB_AC_SET_MIN          0x02
#define USB_AC_SET_MAX          0x03
#define USB_AC_SET_RES          0x04
#define USB_AC_SET_MEM          0x05
#define USB_AC_GET_CUR          0x81
#define USB_AC_GET_MIN          0x82
#define USB_AC_GET_MAX          0x83
#define USB_AC_GET_RES          0x84
#define USB_AC_GET_MEM          0x85
#define USB_AC_GET_STAT         0xff

#define USB_CS_CONTROL_M        0xff00
#define USB_CS_CHANNEL_M        0x00ff

//*****************************************************************************
//
// Endpoint Control Selectors
//
//*****************************************************************************
#define EP_CONTROL_UNDEFINED    0x0000
#define SAMPLING_FREQ_CONTROL   0x0100
#define PITCH_CONTROL           0x0200

//*****************************************************************************
//
// Feature Unit Control Selectors
//
//*****************************************************************************
#define FU_CONTROL_UNDEFINED    0x0000
#define MUTE_CONTROL            0x0100
#define VOLUME_CONTROL          0x0200
#define BASS_CONTROL            0x0300
#define MID_CONTROL             0x0400
#define TREBLE_CONTROL          0x0500
#define EQUALIZER_CONTROL       0x0600
#define AUTOMATIC_GAIN_CONTROL  0x0700
#define DELAY_CONTROL           0x0800
#define BASS_BOOST_CONTROL      0x0900
#define LOUDNESS_CONTROL        0x0A00

//*****************************************************************************
//
// All structures defined in this section of the header require byte packing of
// fields.  This is usually accomplished using the PACKED macro but, for IAR
// Embedded Workbench, this requires a pragma.
//
//*****************************************************************************
#ifdef ewarm
#pragma pack(1)
#endif

//*****************************************************************************
//
//! This structure describes the Class-Specific Audio Class Interface Header
//! Descriptor as defined in Universal Serial Bus Device Class Definition
//! for Audio Devices Release 1.0.
//
//*****************************************************************************
typedef struct
{
    //
    //! The length of this descriptor in bytes.
    //
    unsigned char bLength;

    //
    //! The type of the descriptor.  For an interface descriptor, this will
    //! be USB_DTYPE_CS_INTERFACE (36).
    //
    unsigned char bDescriptorType;

    //
    //! This will be USB_DSUBTYPE_HEADER for the header.
    //
    unsigned char bDescriptorSubtype;

    //
    //! Audio Device Class Specification Release Number in Binary-Coded
    //! Decimal.
    //
    unsigned short bcdADC;

    //
    //! Total number of bytes returned for the class-specific AudioControl
    //! interface descriptor. Includes the combined length of this descriptor
    //! header and all Unit and Terminal descriptors.
    //
    unsigned short wTotalLength;

    //
    //! The number of AudioStreaming and MIDIStreaming interfaces in the Audio
    //! Interface Collection to which this AudioControl interface belongs.
    //
    unsigned char bInCollection;

    //
    //! Interface number of the first AudioStreaming or MIDIStreaming interface
    //! in the Collection.
    //
    unsigned char baInterfaceNr;
}
PACKED tACHeader;

//*****************************************************************************
//
// These are the possible bits set in the tACInputTerminal.wChannelConfig
// value.
//
//*****************************************************************************
#define USB_AC_CC_LEFT          0x0001
#define USB_AC_CC_RIGHT         0x0002
#define USB_AC_CC_CENTER        0x0004
#define USB_AC_CC_LFE           0x0008
#define USB_AC_CC_LEFTSURROUND  0x0010
#define USB_AC_CC_RIGHTSURROUND 0x0020
#define USB_AC_CC_LEFT_CENTER   0x0040
#define USB_AC_CC_RIGHT_CENTER  0x0080
#define USB_AC_CC_SURROUND      0x0100
#define USB_AC_CC_SIDE_LEFT     0x0200
#define USB_AC_CC_SIDE_RIGHT    0x0400
#define USB_AC_CC_TOP           0x0800

//*****************************************************************************
//
//! This structure describes the Feature Unit Descriptor as defined in
//! Universal Serial Bus Device Class Definition for Audio Devices Release 1.0.
//
//*****************************************************************************
typedef struct
{
    //
    //! The length of this descriptor in bytes.
    //
    unsigned char bLength;

    //
    //! The type of the descriptor.  For an interface descriptor, this will
    //! be USB_DTYPE_CS_INTERFACE (36).
    //
    unsigned char bDescriptorType;

    //
    //! This will be USB_DSUBTYPE_IN_TERM for the header.
    //
    unsigned char bDescriptorSubtype;

    //
    //! Constant uniquely identifying the Unit within the audio function. This
    //! value is used in all requests to address this Unit.
    //
    unsigned char bUnitID;

    //
    //! ID of the Unit or Terminal to which this Feature Unit is connected.
    //
    unsigned char bSourceID;

    //
    //! ID of the Output Terminal to which this Input Terminal is associated.
    //
    unsigned char bControlSize;

    //
    //! A bit set to 1 indicates that the mentioned Control is supported for
    //! a given channel.  See the USB_FU_* defines for the bit definitions.
    //! This actually an array of elements of size bControlSize so be
    //! careful when using this value directly.
    //
    unsigned short bmaControls;
}
PACKED tACFeatureUnit;

//*****************************************************************************
//
//! This structure describes the Output Terminal Descriptor as defined in
//! Universal Serial Bus Device Class Definition for Audio Devices Release 1.0.
//
//*****************************************************************************
typedef struct
{
    //
    //! The length of this descriptor in bytes.
    //
    unsigned char bLength;

    //
    //! The type of the descriptor.  For an interface descriptor, this will
    //! be USB_DTYPE_CS_INTERFACE (36).
    //
    unsigned char bDescriptorType;

    //
    //! This will be USB_DSUBTYPE_OUT_TERM for the header.
    //
    unsigned char bDescriptorSubtype;

    //
    //! Constant uniquely identifying the Terminal within the audio function.
    //! This value is used in all requests to address this Terminal.
    //
    unsigned char bTerminalID;

    //
    //! Constant characterizing the type of Terminal. See USB Audio Terminal
    //! Types.
    //
    unsigned short wTerminalType;

    //
    //! Constant, identifying the Input Terminal to which this Output Terminal
    //! is associated.
    //
    unsigned char bAssocTerminal;

    //
    //! ID of the Unit or Terminal to which this Terminal is connected.
    //
    unsigned char bSourceID;

    //
    //! Index of a string descriptor, describing the Output Terminal.
    //
    unsigned char iTerminal;
}
PACKED tACOutputTerminal;

//*****************************************************************************
//
//! This structure describes the Input Terminal Descriptor as defined in
//! Universal Serial Bus Device Class Definition for Audio Devices Release 1.0.
//
//*****************************************************************************
typedef struct
{
    //
    //! The length of this descriptor in bytes.
    //
    unsigned char bLength;

    //
    //! The type of the descriptor.  For an interface descriptor, this will
    //! be USB_DTYPE_CS_INTERFACE (36).
    //
    unsigned char bDescriptorType;

    //
    //! This will be USB_DSUBTYPE_OUT_TERM for the header.
    //
    unsigned char bDescriptorSubtype;

    //
    //! Constant uniquely identifying the Terminal within the audio function.
    //! This value is used in all requests to address this Terminal.
    //
    unsigned char bTerminalID;

    //
    //! Constant characterizing the type of Terminal. See USB Audio Terminal
    //! Types.
    //
    unsigned short wTerminalType;

    //
    //! Constant, identifying the Input Terminal to which this Output Terminal
    //! is associated.
    //
    unsigned char bAssocTerminal;

    //
    //! Number of logical output channels in the Terminal's output audio
    //! channel cluster.
    //
    unsigned char bNrChannels;

    //
    //! Describes the spatial location of the logical channels.
    //
    unsigned short wChannelConfig;

    //
    //! Index of a string descriptor, describing the name of the first logical
    //! channel.
    //
    unsigned char iChannelNames;

    //
    //! Index of a string descriptor, describing the Output Terminal.
    //
    unsigned char iTerminal;
}
PACKED tACInputTerminal;

//*****************************************************************************
//
//! This structure describes the Mixer Descriptor as defined in Universal
//! Serial Bus Device Class Definition for Audio Devices Release 1.0.
//
//*****************************************************************************
typedef struct
{
    //
    //! The length of this descriptor in bytes.
    //
    unsigned char bLength;

    //
    //! The type of the descriptor.  For an interface descriptor, this will
    //! be USB_DTYPE_CS_INTERFACE (36).
    //
    unsigned char bDescriptorType;

    //
    //! This will be USB_AI_MIXER_UNIT for the header.
    //
    unsigned char bDescriptorSubtype;

    //
    //! Constant uniquely identifying the Unit within the audio function. This
    //! value is used in all requests to address this Unit.
    //
    unsigned char bUnitID;

    //
    //! Number of Input Pins of this Unit.
    //
    unsigned char bNrInPins;

    //
    //! ID of the Unit or Terminal to which the first Input Pin of this Mixer Unit is
    //! connected.
    //
    unsigned char baSourceID;
}
PACKED tACMixer;

//*****************************************************************************
//
//! This structure describes the Selector Descriptor as defined in Universal
//! Serial Bus Device Class Definition for Audio Devices Release 1.0.
//
//*****************************************************************************
typedef struct
{
    //
    //! The length of this descriptor in bytes.
    //
    unsigned char bLength;

    //
    //! The type of the descriptor.  For an interface descriptor, this will
    //! be USB_DTYPE_CS_INTERFACE (36).
    //
    unsigned char bDescriptorType;

    //
    //! This will be USB_AI_MIXER_UNIT for the header.
    //
    unsigned char bDescriptorSubtype;

    //
    //! Constant uniquely identifying the Unit within the audio function. This
    //! value is used in all requests to address this Unit.
    //
    unsigned char bUnitID;

    //
    //! Number of Input Pins of this Unit.
    //
    unsigned char bNrInPins;

    //
    //! ID of the Unit or Terminal to which the first Input Pin of this Mixer Unit is
    //! connected.
    //
    unsigned char baSourceID;
}
PACKED tACSelector;

//*****************************************************************************
//
//! This structure describes the Output Terminal Descriptor as defined in
//! Universal Serial Bus Device Class Definition for Audio Devices Release 1.0.
//
//*****************************************************************************
typedef struct
{
    //
    //! The length of this descriptor in bytes.
    //
    unsigned char bLength;

    //
    //! The type of the descriptor.  For an interface descriptor, this will
    //! be USB_DTYPE_CS_INTERFACE (36).
    //
    unsigned char bDescriptorType;

    //
    //! This will be USB_DSUBTYPE_GENERAL for the header.
    //
    unsigned char bDescriptorSubtype;

    //
    //! The Terminal ID of the Terminal to which the endpoint of this
    //! interface is connected.
    //
    unsigned char bTerminalLink;

    //
    //! Delay introduced by the data path. Expressed in number of frames.
    //
    unsigned char bDelay;

    //
    //! The Audio Data Format that has to be used to communicate with this
    //! interface.
    //
    unsigned short wFormatTag;
}
PACKED tACGeneral;

//*****************************************************************************
//
//! This structure describes the Type I Audio format descriptors defined in
//! USB Audio Devices Release 1.0.
//
//*****************************************************************************
typedef struct
{
    //
    //! The length of this descriptor in bytes.
    //
    unsigned char bLength;

    //
    //! The type of the descriptor.  For an interface descriptor, this will
    //! be USB_DTYPE_CS_INTERFACE (36).
    //
    unsigned char bDescriptorType;

    //
    //! This will be USB_AS_FORMAT_TYPE.
    //
    unsigned char bDescriptorSubtype;

    //
    //! This will be USB_AS_FORMAT_TYPE_I.
    //
    unsigned char bFormatType;

    //
    //! Number of channels on this streaming interface.
    //
    unsigned char bNrChannels;

    //
    //! Number of bytes per audio sub-frame or channel.
    //
    unsigned char bSubFrameSize;

    //
    //! Number of bits per sample.
    //
    unsigned char bBitResolution;

    //
    //! Number of sample rates that are supported.
    //
    unsigned char bSamFreqType;

    //
    //! Number of bits per sample.
    //
    unsigned char tSamFreq;
}
PACKED tASFormat;

//*****************************************************************************
//
// Return to default packing when using the IAR Embedded Workbench compiler.
//
//*****************************************************************************
#ifdef ewarm
#pragma pack()
#endif

#endif

