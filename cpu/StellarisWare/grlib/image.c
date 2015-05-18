//*****************************************************************************
//
// image.c - Routines for drawing bitmap images.
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
// This is part of revision 9453 of the Stellaris Graphics Library.
//
//*****************************************************************************

#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "grlib/grlib.h"

//*****************************************************************************
//
//! \addtogroup primitives_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// The buffer that holds the dictionary used by the Lempel-Ziv-Storer-Szymanski
// compression algorithm.  This is simply the last 32 bytes decoded from the
// stream, and is initially filled with zeros.
//
//*****************************************************************************
static unsigned char g_pucDictionary[32];

//*****************************************************************************
//
// Draws a run of pixels, dropping out any in a given transparent color.
//
//*****************************************************************************
static void
PixelTransparentDraw(const tContext *pContext, long lX, long lY,
                     long lX0, long lCount, long lBPP,
                     const unsigned char *pucData,
                     const unsigned char *pucPalette,
                     unsigned long ulTransparent)
{
    long lStart, lLen, lIndex, lStartX0, lOn, lOff, lNumBytes, lBit, lDraw;
    unsigned long ulMask;
    unsigned char ucPixel;
    tBoolean bSkip;

    //
    // What format are we dealing with?
    //
    switch(lBPP)
    {
        //
        // Two color bitmap.
        //
        case 1:
        {
            //
            // How many bytes do we need to read to cover the line of data
            // we've been passed.
            //
            lNumBytes = (lCount + lX0 + 7) / 8;

            //
            // Where must we end the line of pixels?
            //
            lLen = lCount;

            //
            // Set our mask to allow us to make either foreground or background
            // pixels transparent.
            //
            ulMask = ulTransparent ? 0xFF : 0;

            //
            // Loop through the bytes in the pixel data.
            //
            lBit = lX0;
            lX0 = 0;
            for(lIndex = 0; lIndex < lNumBytes; )
            {
                //
                // Count the number of off pixels from this position in the
                // glyph image.
                //
                for(lOff = 0; lIndex < lNumBytes; )
                {
                    //
                    // Get the number of zero pixels at this position.
                    //
                    lCount = NumLeadingZeros(((pucData[lIndex] ^ ulMask) <<
                                              (24 + lBit)));

                    //
                    // If there were more than 8, then it is a "false" result
                    // since it counted beyond the end of the current byte.
                    // Therefore, simply limit it to the number of pixels
                    // remaining in this byte.
                    //
                    if(lCount > 8)
                    {
                        lCount = 8 - lBit;
                    }

                    //
                    // Increment the number of off pixels.
                    //
                    lOff += lCount;

                    //
                    // Increment the bit position within the byte.
                    //
                    lBit += lCount;

                    //
                    // See if the end of the byte has been reached.
                    //
                    if(lBit == 8)
                    {
                        //
                        // Advance to the next byte and continue counting off
                        // pixels.
                        //
                        lBit = 0;
                        lIndex++;
                    }
                    else
                    {
                        //
                        // Since the end of the byte was not reached, there
                        // must be an on pixel.  Therefore, stop counting off
                        // pixels.
                        //
                        break;
                    }
                }

                //
                // Count the number of on pixels from this position in the
                // glyph image.
                //
                for(lOn = 0; lIndex < lNumBytes; )
                {
                    //
                    // Get the number of one pixels at this location (by
                    // inverting the data and counting the number of zeros).
                    //
                    lCount = NumLeadingZeros(~(((pucData[lIndex] ^ ulMask) <<
                                             (24 + lBit))));

                    //
                    // If there were more than 8, then it is a "false" result
                    // since it counted beyond the end of the current byte.
                    // Therefore, simply limit it to the number of pixels
                    // remaining in this byte.
                    //
                    if(lCount > 8)
                    {
                        lCount = 8 - lBit;
                    }

                    //
                    // Increment the number of on pixels.
                    //
                    lOn += lCount;

                    //
                    // Increment the bit position within the byte.
                    //
                    lBit += lCount;

                    //
                    // See if the end of the byte has been reached.
                    //
                    if(lBit == 8)
                    {
                        //
                        // Advance to the next byte and continue counting on
                        // pixels.
                        //
                        lBit = 0;
                        lIndex++;
                    }
                    else
                    {
                        //
                        // Since the end of the byte was not reached, there
                        // must be an off pixel.  Therefore, stop counting on
                        // pixels.
                        //
                        break;
                    }
                }

                //
                // At this point, we have the next off and on run lengths
                // determined so draw the on run if it is non-zero length
                // and falls within the range we need to draw.
                //
                if(lOn && (lOff < lLen))
                {
                    lDraw = ((lOff + lOn) > lLen) ? (lX + lLen) :
                            (lX + lOff + lOn);
                    DpyLineDrawH(pContext->pDisplay, lX + lOff, lDraw - 1, lY,
                                 *(unsigned long *)(pucPalette +
                                 (ulTransparent ? 0 : 4)));
                }

                //
                // Move right past these two runs.
                //
                lX += (lOff + lOn);
                lLen -= (lOff + lOn);
            }
        }
        break;

        //
        // 4 bits per pixel (16 color) bitmap.
        //
        case 4:
        {
            //
            // Are we starting by drawing or skipping pixels?
            //
            ucPixel = (pucData[0] >> (lX0 ? 0 : 4)) & 0x0F;
            bSkip = (ucPixel == (unsigned char)ulTransparent) ? true : false;
            lStart = 0;
            lStartX0 = lX0;
            lBit = lX0;
            lLen = bSkip ? 0 : 1;

            //
            // Scan all pixels in the line of data provided.
            //
            for(lIndex = 1; lIndex < lCount; lIndex++)
            {
                //
                // Toggle the sub-byte pixel indicator;
                //
                lX0 = 1 - lX0;

                //
                // Read the next pixel.
                //
                ucPixel = (pucData[(lIndex + lBit) / 2] >> (lX0 ? 0 : 4)) & 0x0F;

                //
                // Is this pixel a transparent one?
                //
                if(ucPixel != (unsigned char)ulTransparent)
                {
                    //
                    // It's not transparent.  Have we just ended a run of
                    // transparent pixels?
                    //
                    if(bSkip)
                    {
                        //
                        // We are currently skipping pixels so this starts a
                        // new run.
                        //
                        lStart = lIndex;
                        lStartX0 = lX0;
                        lLen = 1;
                        bSkip = false;
                    }
                    else
                    {
                        //
                        // We were already in the middle of a run of non-
                        // transparent pixels so increment the run length.
                        //
                        lLen++;
                    }
                }
                else
                {
                    //
                    // Pixel is transparent.  Do we have a run to draw?
                    //
                    if(!bSkip)
                    {
                        //
                        // Yes - draw what we have.
                        //
                        DpyPixelDrawMultiple(pContext->pDisplay, lX + lStart,
                                             lY, lStartX0, lLen, 4,
                                             &pucData[(lStart + lBit) / 2],
                                             pucPalette);

                        //
                        // Reset for the transparent run.
                        //
                        lLen = 0;
                        bSkip = true;
                    }
                }
            }

            //
            // If we drop out of the pixel loop with a run not drawn, draw it
            // here.
            //
            if(!bSkip && lLen)
            {
                DpyPixelDrawMultiple(pContext->pDisplay, lX + lStart,
                                     lY, lStartX0, lLen, 4,
                                     &pucData[(lStart + lBit) / 2],
                                     pucPalette);
            }
        }
        break;

        //
        // 8 bit per pixel (256 color) bitmap.
        //
        case 8:
        {
            //
            // Are we starting by drawing or skipping pixels?
            //
            bSkip = (pucData[0] == (unsigned char)ulTransparent) ? true : false;
            lStart = 0;
            lLen = bSkip ? 0 : 1;

            //
            // Scan all pixels in the line of data provided.
            //
            for(lIndex = 1; lIndex < lCount; lIndex++)
            {
                //
                // Is this pixel a transparent one?
                //
                if(pucData[lIndex] != (unsigned char)ulTransparent)
                {
                    //
                    // It's not transparent.  Have we just ended a run of
                    // transparent pixels?
                    //
                    if(bSkip)
                    {
                        //
                        // We are currently skipping pixels so this starts a
                        // new run.
                        //
                        lStart = lIndex;
                        lLen = 1;
                        bSkip = false;
                    }
                    else
                    {
                        //
                        // We were already in the middle of a run of non-
                        // transparent pixels so increment the run length.
                        //
                        lLen++;
                    }
                }
                else
                {
                    //
                    // Pixel is transparent.  Do we have a run to draw?
                    //
                    if(!bSkip)
                    {
                        //
                        // Yes - draw what we have.
                        //
                        DpyPixelDrawMultiple(pContext->pDisplay, lX + lStart,
                                             lY, 0, lLen, 8,
                                             &pucData[lStart], pucPalette);

                        //
                        // Reset for the transparent run.
                        //
                        lLen = 0;
                        bSkip = true;
                    }
                }
            }

            //
            // If we drop out of the pixel loop with a run not drawn, draw it
            // here.
            //
            if(!bSkip && lLen)
            {
                DpyPixelDrawMultiple(pContext->pDisplay, lX + lStart,
                                     lY, lX0, lLen, 8,
                                     &pucData[lStart], pucPalette);
            }
        }
        break;
    }
}

//*****************************************************************************
//
// Internal function implementing both normal and transparent image drawing.
//
//*****************************************************************************
static void
InternalImageDraw(const tContext *pContext, const unsigned char *pucImage,
                  long lX, long lY, unsigned long ulTransparent,
                  tBoolean bTransparent)
{
    unsigned long ulByte, ulBits, ulMatch, ulSize, ulIdx, ulCount, ulNum;
    long lBPP, lWidth, lHeight, lX0, lX1, lX2, lXMask;
    const unsigned char *pucPalette;
    unsigned long pulBWPalette[2];

    //
    // Check the arguments.
    //
    ASSERT(pContext);
    ASSERT(pucImage);

    //
    // Get the image format from the image data.
    //
    lBPP = *pucImage++;

    //
    // Get the image width from the image data.
    //
    lWidth = *(unsigned short *)pucImage;
    pucImage += 2;

    //
    // Get the image height from the image data.
    //
    lHeight = *(unsigned short *)pucImage;
    pucImage += 2;

    //
    // Return without doing anything if the entire image lies outside the
    // current clipping region.
    //
    if((lX > pContext->sClipRegion.sXMax) ||
       ((lX + lWidth - 1) < pContext->sClipRegion.sXMin) ||
       (lY > pContext->sClipRegion.sYMax) ||
       ((lY + lHeight - 1) < pContext->sClipRegion.sYMin))
    {
        return;
    }

    //
    // Get the starting X offset within the image based on the current clipping
    // region.
    //
    if(lX < pContext->sClipRegion.sXMin)
    {
        lX0 = pContext->sClipRegion.sXMin - lX;
    }
    else
    {
        lX0 = 0;
    }

    //
    // Get the ending X offset within the image based on the current clipping
    // region.
    //
    if((lX + lWidth - 1) > pContext->sClipRegion.sXMax)
    {
        lX2 = pContext->sClipRegion.sXMax - lX;
    }
    else
    {
        lX2 = lWidth - 1;
    }

    //
    // Reduce the height of the image, if required, based on the current
    // clipping region.
    //
    if((lY + lHeight - 1) > pContext->sClipRegion.sYMax)
    {
        lHeight = pContext->sClipRegion.sYMax - lY + 1;
    }

    //
    // Determine the color palette for the image based on the image format.
    //
    if((lBPP & 0x7f) == IMAGE_FMT_1BPP_UNCOMP)
    {
        //
        // Construct a local "black & white" palette based on the foreground
        // and background colors of the drawing context.
        //
        pulBWPalette[0] = pContext->ulBackground;
        pulBWPalette[1] = pContext->ulForeground;

        //
        // Set the palette pointer to the local "black & white" palette.
        //
        pucPalette = (unsigned char *)pulBWPalette;
    }
    else
    {
        //
        // For 4 and 8 BPP images, the palette is contained at the start of the
        // image data.
        //
        pucPalette = pucImage + 1;
        pucImage += (pucImage[0] * 3) + 4;
    }

    //
    // See if the image is compressed.
    //
    if(!(lBPP & 0x80))
    {
        //
        // The image is not compressed.  See if the top portion of the image
        // lies above the clipping region.
        //
        if(lY < pContext->sClipRegion.sYMin)
        {
            //
            // Determine the number of rows that lie above the clipping region.
            //
            lX1 = pContext->sClipRegion.sYMin - lY;

            //
            // Skip past the data for the rows that lie above the clipping
            // region.
            //
            pucImage += (((lWidth * lBPP) + 7) / 8) * lX1;

            //
            // Decrement the image height by the number of skipped rows.
            //
            lHeight -= lX1;

            //
            // Increment the starting Y coordinate by the number of skipped
            // rows.
            //
            lY += lX1;
        }

        //
        // Determine the starting offset for the first source pixel within
        // the byte.
        //
        switch(lBPP)
        {
            case 1:
            {
                lXMask = lX0 & 7;
                break;
            }

            case 4:
            {
                lXMask = lX0 & 1;
                break;
            }

            default:
            {
                lXMask = 0;
                break;
            }
        }

        //
        // Loop while there are more rows to draw.
        //
        while(lHeight--)
        {
            //
            // Draw this row of image pixels.
            //
            if(bTransparent)
            {
                PixelTransparentDraw(pContext, lX + lX0, lY, lXMask,
                                     lX2 - lX0 + 1, lBPP,
                                     pucImage + ((lX0 * lBPP) / 8),
                                     pucPalette, ulTransparent);
            }
            else
            {
                DpyPixelDrawMultiple(pContext->pDisplay, lX + lX0, lY, lXMask,
                                     lX2 - lX0 + 1, lBPP,
                                     pucImage + ((lX0 * lBPP) / 8), pucPalette);
            }

            //
            // Skip past the data for this row.
            //
            pucImage += ((lWidth * lBPP) + 7) / 8;

            //
            // Increment the Y coordinate.
            //
            lY++;
        }
    }
    else
    {
        //
        // The image is compressed.  Clear the compressed flag in the format
        // specifier so that the bits per pixel remains.
        //
        lBPP &= 0x7f;

        //
        // Reset the dictionary used to uncompress the image.
        //
        for(ulBits = 0; ulBits < sizeof(g_pucDictionary); ulBits += 4)
        {
            *(unsigned long *)(g_pucDictionary + ulBits) = 0;
        }

        //
        // Determine the number of bytes of data to decompress.
        //
        ulCount = (((lWidth * lBPP) + 7) / 8) * lHeight;

        //
        // Initialize the pointer into the dictionary.
        //
        ulIdx = 0;

        //
        // Start off with no encoding byte.
        //
        ulBits = 0;
        ulByte = 0;

        //
        // Start from the upper left corner of the image.
        //
        lX1 = 0;

        //
        // Loop while there are more rows or more data in the image.
        //
        while(lHeight && ulCount)
        {
            //
            // See if an encoding byte needs to be read.
            //
            if(ulBits == 0)
            {
                //
                // Read the encoding byte, which indicates if each of the
                // following eight bytes are encoded or literal.
                //
                ulByte = *pucImage++;
                ulBits = 8;
            }

            //
            // See if the next byte is encoded or literal.
            //
            if(ulByte & (1 << (ulBits - 1)))
            {
                //
                // This byte is encoded, so extract the location and size of
                // the encoded data within the dictionary.
                //
                ulMatch = *pucImage >> 3;
                ulSize = (*pucImage++ & 7) + 2;

                //
                // Decrement the count of bytes to decode by the number of
                // copied bytes.
                //
                ulCount -= ulSize;
            }
            else
            {
                //
                // This byte is a literal, so copy it into the dictionary.
                //
                g_pucDictionary[ulIdx++] = *pucImage++;

                //
                // Decrement the count of bytes to decode.
                //
                ulCount--;

                //
                // Clear any previous encoded data information.
                //
                ulMatch = 0;
                ulSize = 0;
            }

            //
            // Loop while there are bytes to copy for the encoded data, or
            // once for literal data.
            //
            while(ulSize || !(ulByte & (1 << (ulBits - 1))))
            {
                //
                // Set the encoded data bit for this data so that this loop
                // will only be executed once for literal data.
                //
                ulByte |= 1 << (ulBits - 1);

                //
                // Loop while there is more encoded data to copy and there is
                // additional space in the dictionary (before the buffer
                // wraps).
                //
                while(ulSize && (ulIdx != sizeof(g_pucDictionary)))
                {
                    //
                    // Copy this byte.
                    //
                    g_pucDictionary[ulIdx] =
                        g_pucDictionary[(ulIdx + ulMatch) %
                                        sizeof(g_pucDictionary)];

                    //
                    // Increment the dictionary pointer.
                    //
                    ulIdx++;

                    //
                    // Decrement the encoded data size.
                    //
                    ulSize--;
                }

                //
                // See if the dictionary pointer is about to wrap, or if there
                // is no more data to decompress.
                //
                if((ulIdx == sizeof(g_pucDictionary)) || !ulCount)
                {
                    //
                    // Loop through the data in the dictionary buffer.
                    //
                    for(ulIdx = 0;
                        (ulIdx < sizeof(g_pucDictionary)) && lHeight; )
                    {
                        //
                        // Compute the number of pixels that remain in the
                        // dictionary buffer.
                        //
                        ulNum = ((sizeof(g_pucDictionary) - ulIdx) * 8) / lBPP;

                        //
                        // See if any of the pixels in the dictionary buffer
                        // are within the clipping region.
                        //
                        if((lY >= pContext->sClipRegion.sYMin) &&
                           ((lX1 + ulNum) >= lX0) && (lX1 <= lX2))
                        {
                            //
                            // Skip some pixels at the start of the scan line
                            // if required to stay within the clipping region.
                            //
                            if(lX1 < lX0)
                            {
                                ulIdx += ((lX0 - lX1) * lBPP) / 8;
                                lX1 = lX0;
                            }

                            //
                            // Shorten the scan line if required to stay within
                            // the clipping region.
                            //
                            if(ulNum > (lX2 - lX1 + 1))
                            {
                                ulNum = lX2 - lX1 + 1;
                            }

                            //
                            // Determine the starting offset for the first
                            // source pixel within the byte.
                            //
                            switch(lBPP)
                            {

                                case 1:
                                {
                                    lXMask = lX1 & 7;
                                    break;
                                }

                                case 4:
                                {
                                    lXMask = lX1 & 1;
                                    break;
                                }

                                default:
                                {
                                    lXMask = 0;
                                    break;
                                }
                            }

                            //
                            // Draw this row of image pixels.
                            //
                            if(bTransparent)
                            {
                                PixelTransparentDraw(pContext, lX + lX1,
                                                     lY, lXMask, ulNum, lBPP,
                                                     g_pucDictionary + ulIdx,
                                                     pucPalette, ulTransparent);
                            }
                            else
                            {
                                DpyPixelDrawMultiple(pContext->pDisplay,
                                                     lX + lX1, lY, lXMask,
                                                     ulNum, lBPP,
                                                     g_pucDictionary + ulIdx,
                                                     pucPalette);
                            }
                        }

                        //
                        // Move the X coordinate back to the start of the first
                        // data byte in this portion of the dictionary buffer.
                        //
                        lX1 = ((lX1 * lBPP) & ~7) / lBPP;

                        //
                        // See if the remainder of this scan line resides
                        // within the dictionary buffer.
                        //
                        if(((((lWidth - lX1) * lBPP) + 7) / 8) >
                           (sizeof(g_pucDictionary) - ulIdx))
                        {
                            //
                            // There is more to this scan line than is in the
                            // dictionary buffer at this point, so move the
                            // X coordinate by by the number of pixels in the
                            // dictionary buffer.
                            //
                            lX1 += (((sizeof(g_pucDictionary) - ulIdx) * 8) /
                                    lBPP);

                            //
                            // The entire dictionary buffer has been scanned.
                            //
                            ulIdx = sizeof(g_pucDictionary);
                        }
                        else
                        {
                            //
                            // The remainder of this scan line resides in the
                            // dictionary buffer, so skip past it.
                            //
                            ulIdx += (((lWidth - lX1) * lBPP) + 7) / 8;

                            //
                            // Move to the start of the next scan line.
                            //
                            lX1 = 0;
                            lY++;

                            //
                            // There is one less scan line to process.
                            //
                            lHeight--;
                        }
                    }

                    //
                    // Start over from the beginning of the dictionary buffer.
                    //
                    ulIdx = 0;
                }
            }

            //
            // Advance to the next bit in the encoding byte.
            //
            ulBits--;
        }
    }
}

//*****************************************************************************
//
//! Draws a bitmap image, dropping out a single transparent color.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param pucImage is a pointer to the image to draw.
//! \param lX is the X coordinate of the upper left corner of the image.
//! \param lY is the Y coordinate of the upper left corner of the image.
//! \param ulTransparent is the image color which is to be considered
//! transparent.
//!
//! This function draws a bitmap image but, unlike GrImageDraw, will drop out
//! any pixel of a particular color allowing the previous background to ``shine
//! through''.  The image may be 1 bit per pixel (using the foreground and
//! background color from the drawing context), 4 bits per pixel (using a
//! palette supplied in the image data), or 8 bits per pixel (using a palette
//! supplied in the image data).  It can be uncompressed data, or it can be
//! compressed using the Lempel-Ziv-Storer-Szymanski algorithm (as published in
//! the Journal of the ACM, 29(4):928-951, October 1982).  For 4bpp and 8bpp
//! images, the \b ulTransparent parameter contains the palette index of the
//! colour which is to be considered transparent.  For 1bpp images, the
//! \b ulTransparent parameter should be set to 0 to draw only foreground
//! pixels or 1 to draw only background pixels.
//!
//! \return None.
//
//*****************************************************************************
void
GrTransparentImageDraw(const tContext *pContext, const unsigned char *pucImage,
                       long lX, long lY, unsigned long ulTransparent)
{
    InternalImageDraw(pContext, pucImage, lX, lY, ulTransparent, true);
}

//*****************************************************************************
//
//! Draws a bitmap image.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param pucImage is a pointer to the image to draw.
//! \param lX is the X coordinate of the upper left corner of the image.
//! \param lY is the Y coordinate of the upper left corner of the image.
//!
//! This function draws a bitmap image.  The image may be 1 bit per pixel
//! (using the foreground and background color from the drawing context), 4
//! bits per pixel (using a palette supplied in the image data), or 8 bits per
//! pixel (using a palette supplied in the image data).  It can be uncompressed
//! data, or it can be compressed using the Lempel-Ziv-Storer-Szymanski
//! algorithm (as published in the Journal of the ACM, 29(4):928-951, October
//! 1982).
//!
//! \return None.
//
//*****************************************************************************
void
GrImageDraw(const tContext *pContext, const unsigned char *pucImage,
            long lX, long lY)
{
    InternalImageDraw(pContext, pucImage, lX, lY, 0, false);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
