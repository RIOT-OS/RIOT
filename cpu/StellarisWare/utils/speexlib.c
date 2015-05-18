//*****************************************************************************
//
// speexlib.c - Stellaris interface to the speex coder/encoder library.
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
// This is part of revision 9453 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "third_party/speex-1.2rc1/include/speex/speex.h"
#include "third_party/speex-1.2rc1/include/speex/speex_header.h"
#include "utils/speexlib.h"
#include "driverlib/debug.h"

//*****************************************************************************
//
// The private structure that is used by the speex encoder or decoder for
// holding all state information for an encoder or decoder.
//
//*****************************************************************************
typedef struct
{
    //
    // Holds the state of the decoder.
    //
    void *State;

    //
    // Holds bits so they can be read and written to by the Speex routines
    //
    SpeexBits sBits;

    //
    // Holds the header information for the current file.
    //
    SpeexHeader sHeader;

    //
    // The current Segment table for the stream.
    //
    unsigned char pucSegTable[256];

    //
    // The size of the current Segment table.
    //
    unsigned char ucSegTableSize;

    //
    // The current active page in a segment.
    //
    unsigned char ucPageCurrent;

    //
    // Current state flags.
    //
    unsigned long ulFlags;
} tSpeexInstance;

//*****************************************************************************
//
// The decoder and encoder instance data.
//
//*****************************************************************************
tSpeexInstance g_sSpeexDecoder, g_sSpeexEncoder;

//*****************************************************************************
//
//! Initialize the decoder's state to prepare for decoding new frames.
//!
//! This function will initializes the decoder so that it is prepared to start
//! receiving frames to decode.
//!
//! /return This function returns 0.
//
//*****************************************************************************
int
SpeexDecodeInit(void)
{
    int iTemp;

    //
    // Clear out the flags for this instance.
    //
    g_sSpeexDecoder.ulFlags = 0;

    //
    // Create a new decoder state in narrow band mode.
    //
    g_sSpeexDecoder.State = speex_decoder_init(&speex_nb_mode);

    //
    // Disable enhanced decoding to reduce processing requirements.
    //
    iTemp = 0;
    speex_decoder_ctl(g_sSpeexDecoder.State, SPEEX_SET_ENH, &iTemp);

    //
    // Initialization of the structure that holds the bits.
    //
    speex_bits_init(&g_sSpeexDecoder.sBits);

    return(0);
}

//*****************************************************************************
//
//! This function returns the current frame size from the decoder.
//!
//! This function queries the decoder for the current decode frame size in byte
//! and returns it to the caller.
//!
//! /return The current decoder frame size.
//
//*****************************************************************************
int
SpeexDecodeFrameSizeGet(void)
{
    int iFrameSize;

    //
    // Return 0 if the wrong request is made.
    //
    iFrameSize = 0;

    //
    // Query the decoder for the current frame size.
    //
    speex_decoder_ctl(g_sSpeexDecoder.State, SPEEX_GET_FRAME_SIZE,
                      &iFrameSize);

    return(iFrameSize);
}

//*****************************************************************************
//
//! This function decodes a single frame of Speex encoded audio.
//!
//! /param pucInBuffer is the buffer that contains the Speex encoded audio.
//! /param ulInSize is the number of valid bytes in the pucInBuffer buffer.
//! /param pucOutBuffer is a pointer to the buffer to store decoded audio.
//! /param ulOutSize is the size of the buffer pointed to by the pucOutBuffer
//! pointer.
//!
//! This function will take a buffer of Speex encoded audio and decode it into
//! raw PCM audio.  The /e pusInBuffer parameter should contain a single frame
//! encoded Speex audio.  The /e pucOutBuffer will contain the decoded
//! audio after returning from this function.
//!
//! /return This function returns the number of decoded bytes in the
//! /e pucOutBuffer buffer.
//
//*****************************************************************************
int
SpeexDecode(unsigned char *pucInBuffer, unsigned long ulInSize,
            unsigned char *pucOutBuffer, unsigned long ulOutSize)
{
    int iBytes;

    //
    // Read in the bit stream to the Speex library.
    //
    speex_bits_read_from(&g_sSpeexDecoder.sBits, (char *)pucInBuffer, ulInSize);

    //
    // Decode one frame of data.
    //
    iBytes = speex_decode_int(g_sSpeexDecoder.State, &g_sSpeexDecoder.sBits,
                              (short *)pucOutBuffer);

    return(iBytes);
}

//*****************************************************************************
//
//! This function sets the current quality setting for the Speex encoder.
//!
//! /param iQuality is the new Quality setting to use for the Speex encoder.
//!
//! This function will use the /e iQuality setting as the new quality setting
//! for the Speex encoder.
//!
//! /return This function returns 0.
//
//*****************************************************************************
int
SpeexEncodeQualitySet(int iQuality)
{
    //
    // Set the current encoder quality setting.
    //
    speex_encoder_ctl(g_sSpeexEncoder.State, SPEEX_SET_QUALITY, &iQuality);

    return(0);
}

//*****************************************************************************
//
//! This function returns the current frame size from the encoder.
//!
//! This function queries the encoder for the current encode frame size in byte
//! and returns it to the caller.
//!
//! /return The current encoder frame size.
//
//*****************************************************************************
int
SpeexEncodeFrameSizeGet(void)
{
    int iFrameSize;

    //
    // Return 0 if the wrong request is made.
    //
    iFrameSize = 0;

    //
    // Query the encoder for the current frame size.
    //
    speex_encoder_ctl(g_sSpeexEncoder.State, SPEEX_GET_FRAME_SIZE, &iFrameSize);

    return(iFrameSize);
}

//*****************************************************************************
//
//! Initialize the encoder's state to prepare for encoding new frames.
//!
//! /param iSampleRate is the sample rate of the incoming audio.
//! /param iComplexity is the complexity setting for the encoder.
//! /param iQuality is the quality setting for the encoder.
//!
//! This function will initializes the encoder by setting the sample rate,
//! complexity and quality settings.  The /e iComplexity and /i Quality
//! settings are explained further in the Speex documentation.
//!
//! /return This function returns 0.
//
//*****************************************************************************
int
SpeexEncodeInit(int iSampleRate, int iComplexity, int iQuality)
{
    const SpeexMode *psMode;

    //
    // Clear out the flags for this instance.
    //
    g_sSpeexEncoder.ulFlags = 0;

    //
    // Read out the current encoder mode.
    //
    psMode = speex_lib_get_mode(SPEEX_MODEID_NB);

    //
    // Create a new decoder state in narrow band mode.
    //
    g_sSpeexEncoder.State = speex_encoder_init(psMode);

    //
    // Initialize the bit stream.
    //
    speex_bits_init(&g_sSpeexEncoder.sBits);

    //
    // Set the quality.
    //
    SpeexEncodeQualitySet(iQuality);

    //
    // Set the complexity and sample rate for the encoder.
    //
    speex_encoder_ctl(g_sSpeexEncoder.State, SPEEX_SET_COMPLEXITY,
                      &iComplexity);
    speex_encoder_ctl(g_sSpeexEncoder.State, SPEEX_SET_SAMPLING_RATE,
                      &iSampleRate);

    return(0);
}

//*****************************************************************************
//
//! Encode a single frame of speex encoded audio.
//!
//! /param pusInBuffer is the buffer that contains the raw PCM audio.
//! /param ulInSize is the number of valid bytes in the pusInBuffer buffer.
//! /param pucOutBuffer is a pointer to the buffer to store the encoded audio.
//! /param ulOutSize is the size of the buffer pointed to by the pucOutBuffer
//! pointer.
//!
//! This function will take a buffer of PCM audio and encode it into a frame
//! of speex compressed audio.  The /e pusInBuffer parameter should contain
//! a single frame of PCM audio.  The /e pucOutBuffer will contain the encoded
//! audio after returning from this function.
//!
//! /return This function returns the number of encoded bytes in the
//! /e pucOutBuffer parameter.
//
//*****************************************************************************
int
SpeexEncode(short *pusInBuffer, unsigned long ulInSize,
            unsigned char *pucOutBuffer, unsigned long ulOutSize)
{
    int iBytes;

    //
    // Reset the bit stream before encoding a new frame.
    //
    speex_bits_reset(&g_sSpeexEncoder.sBits);

    //
    // Encode a single frame.
    //
    speex_encode_int(g_sSpeexEncoder.State, pusInBuffer,
                     &g_sSpeexEncoder.sBits);

    //
    // Read the PCM data from the encoded bit stream.
    //
    iBytes = speex_bits_write(&g_sSpeexEncoder.sBits, (char *)pucOutBuffer,
                              ulOutSize);

    //
    // return the number of bytes in the PCM data.
    //
    return(iBytes);
}

//*****************************************************************************
//
// This is called by speex in the event of a fatal error.
//
//*****************************************************************************
void
_speex_fatal(const char *str, const char *file, int line)
{
    ASSERT(0);
    while(1);
}

//*****************************************************************************
//
// Speex wrapper for putc so that it does not use any file writing library
// functions.  The speex library uses some file access for debug printing, this
// will disable this feature.
//
//*****************************************************************************
void
_speex_putc(int ch, void *file)
{
}

