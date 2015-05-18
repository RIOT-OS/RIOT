//*****************************************************************************
//
// offscr4bpp.c - 4 BPP off-screen display buffer driver.
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
//! Translates a 24-bit RGB color to a display driver-specific color.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param ulValue is the 24-bit RGB color.  The least-significant byte is the
//! blue channel, the next byte is the green channel, and the third byte is the
//! red channel.
//!
//! This function translates a 24-bit RGB color into a value that can be
//! written into the display's frame buffer in order to reproduce that color,
//! or the closest possible approximation of that color.
//!
//! \return Returns the display-driver specific color.
//
//*****************************************************************************
static unsigned long
GrOffScreen4BPPColorTranslate(void *pvDisplayData, unsigned long ulValue)
{
    unsigned long ulIdx, ulDiff, ulMatchIdx, ulMatchDiff, ulR, ulG, ulB;
    unsigned char *pucPalette;

    //
    // Check the arguments.
    //
    ASSERT(pvDisplayData);

    //
    // Get a pointer to the palette for the off-screen buffer.
    //
    pucPalette = (unsigned char *)pvDisplayData + 6;

    //
    // Extract the red, green, and blue component from the input color.
    //
    ulR = (ulValue >> ClrRedShift) & 0xff;
    ulG = (ulValue >> ClrGreenShift) & 0xff;
    ulB = (ulValue >> ClrBlueShift) & 0xff;

    //
    // Set the match such that the first palette entry will be a better match.
    //
    ulMatchIdx = 0;
    ulMatchDiff = 0xffffffff;

    //
    // Loop through the colors in the palette.
    //
    for(ulIdx = 0; ulIdx < 16; ulIdx++, pucPalette += 3)
    {
        //
        // Compute the Cartesian distance between these two colors.
        //
        ulDiff = (((pucPalette[2] - ulR) * (pucPalette[2] - ulR)) +
                  ((pucPalette[1] - ulG) * (pucPalette[1] - ulG)) +
                  ((pucPalette[0] - ulB) * (pucPalette[0] - ulB)));

        //
        // See if this color is a closer match than any of the previous colors.
        //
        if(ulDiff < ulMatchDiff)
        {
            //
            // Save this color as the new best match.
            //
            ulMatchDiff = ulDiff;
            ulMatchIdx = ulIdx;
        }

        //
        // Stop looking if an exact match was found.
        //
        if(ulDiff == 0)
        {
            break;
        }
    }

    //
    // Return the index of the best match.
    //
    return(ulMatchIdx);
}

//*****************************************************************************
//
//! Draws a pixel on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the pixel.
//! \param lY is the Y coordinate of the pixel.
//! \param ulValue is the color of the pixel.
//!
//! This function sets the given pixel to a particular color.  The coordinates
//! of the pixel are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
GrOffScreen4BPPPixelDraw(void *pvDisplayData, long lX, long lY,
                         unsigned long ulValue)
{
    unsigned char *pucData;
    long lBytesPerRow;

    //
    // Check the arguments.
    //
    ASSERT(pvDisplayData);

    //
    // Create a character pointer for the display-specific data (which points
    // to the image buffer).
    //
    pucData = (unsigned char *)pvDisplayData;

    //
    // Compute the number of bytes per row in the image buffer.
    //
    lBytesPerRow = (*(unsigned short *)(pucData + 1) + 1) / 2;

    //
    // Get the offset to the byte of the image buffer that contains the pixel
    // in question.
    //
    pucData += (lBytesPerRow * lY) + (lX / 2) + 6 + (16 * 3);

    //
    // Determine how much to shift to get to the nibble that contains this
    // pixel.
    //
    lX = (1 - (lX & 1)) * 4;

    //
    // Write this pixel into the image buffer.
    //
    *pucData = (*pucData & ~(0xf << lX)) | (ulValue << lX);
}

//*****************************************************************************
//
//! Draws a horizontal sequence of pixels on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the first pixel.
//! \param lY is the Y coordinate of the first pixel.
//! \param lX0 is sub-pixel offset within the pixel data, which is valid for 1
//! or 4 bit per pixel formats.
//! \param lCount is the number of pixels to draw.
//! \param lBPP is the number of bits per pixel; must be 1, 4, or 8.
//! \param pucData is a pointer to the pixel data.  For 1 and 4 bit per pixel
//! formats, the most significant bit(s) represent the left-most pixel.
//! \param pucPalette is a pointer to the palette used to draw the pixels.
//!
//! This function draws a horizontal sequence of pixels on the screen, using
//! the supplied palette.  For 1 bit per pixel format, the palette contains
//! pre-translated colors; for 4 and 8 bit per pixel formats, the palette
//! contains 24-bit RGB values that must be translated before being written to
//! the display.
//!
//! \return None.
//
//*****************************************************************************
static void
GrOffScreen4BPPPixelDrawMultiple(void *pvDisplayData, long lX, long lY,
                                 long lX0, long lCount, long lBPP,
                                 const unsigned char *pucData,
                                 const unsigned char *pucPalette)
{
    unsigned char *pucPtr;
    unsigned long ulByte;
    long lBytesPerRow;

    //
    // Check the arguments.
    //
    ASSERT(pvDisplayData);
    ASSERT(pucData);
    ASSERT(pucPalette);

    //
    // Create a character pointer for the display-specific data (which points
    // to the image buffer).
    //
    pucPtr = (unsigned char *)pvDisplayData;

    //
    // Compute the number of bytes per row in the image buffer.
    //
    lBytesPerRow = (*(unsigned short *)(pucPtr + 1) + 1) / 2;

    //
    // Get the offset to the byte of the image buffer that contains the
    // starting pixel.
    //
    pucPtr += (lBytesPerRow * lY) + (lX / 2) + 6 + (16 * 3);

    //
    // Determine how much to shift to get to the nibble that contains this
    // pixel.
    //
    lX = (1 - (lX & 1)) * 4;

    //
    // Determine how to interpret the pixel data based on the number of bits
    // per pixel.
    //
    switch(lBPP)
    {
        //
        // The pixel data is in 1 bit per pixel format.
        //
        case 1:
        {
            //
            // Loop while there are more pixels to draw.
            //
            while(lCount)
            {
                //
                // Get the next byte of image data.
                //
                ulByte = *pucData++;

                //
                // Loop through the pixels in this byte of image data.
                //
                for(; (lX0 < 8) && lCount; lX0++, lCount--)
                {
                    //
                    // Draw this pixel in the appropriate color.
                    //
                    *pucPtr = ((*pucPtr & ~(0xf << lX)) |
                               (((unsigned long *)pucPalette)[(ulByte >>
                                                               (7 - lX0)) &
                                                              1] << lX));
                    if(lX ^= 4)
                    {
                        pucPtr++;
                    }
                }

                //
                // Start at the beginning of the next byte of image data.
                //
                lX0 = 0;
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // The pixel data is in 4 bit per pixel format.
        //
        case 4:
        {
            //
            // Loop while there are more pixels to draw.  "Duff's device" is
            // used to jump into the middle of the loop if the first nibble of
            // the pixel data should not be used.  Duff's device makes use of
            // the fact that a case statement is legal anywhere within a
            // sub-block of a switch statement.  See
            // http://en.wikipedia.org/wiki/Duff's_device for detailed
            // information about Duff's device.
            //
            switch(lX0 & 1)
            {
                case 0:
                    while(lCount)
                    {
                        //
                        // Get the upper nibble of the next byte of pixel data
                        // and extract the corresponding entry from the
                        // palette.
                        //
                        ulByte = (*pucData >> 4) * 3;
                        ulByte = (*(unsigned long *)(pucPalette + ulByte) &
                                  0x00ffffff);

                        //
                        // Translate this palette entry.
                        //
                        ulByte = GrOffScreen4BPPColorTranslate(pvDisplayData,
                                                               ulByte);

                        //
                        // Write this pixel to the screen.
                        //
                        *pucPtr = (*pucPtr & ~(0xf << lX)) | (ulByte << lX);
                        if(lX ^= 4)
                        {
                            pucPtr++;
                        }

                        //
                        // Decrement the count of pixels to draw.
                        //
                        lCount--;

                        //
                        // See if there is another pixel to draw.
                        //
                        if(lCount)
                        {
                case 1:
                            //
                            // Get the lower nibble of the next byte of pixel
                            // data and extract the corresponding entry from
                            // the palette.
                            //
                            ulByte = (*pucData++ & 15) * 3;
                            ulByte = (*(unsigned long *)(pucPalette + ulByte) &
                                      0x00ffffff);

                            //
                            // Translate this palette entry.
                            //
                            ulByte =
                                GrOffScreen4BPPColorTranslate(pvDisplayData,
                                                              ulByte);

                            //
                            // Write this pixel to the screen.
                            //
                            *pucPtr = ((*pucPtr & ~(0xf << lX)) |
                                       (ulByte << lX));
                            if(lX ^= 4)
                            {
                                pucPtr++;
                            }

                            //
                            // Decrement the count of pixels to draw.
                            //
                            lCount--;
                        }
                    }
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // The pixel data is in 8 bit per pixel format.
        //
        case 8:
        {
            //
            // Loop while there are more pixels to draw.
            //
            while(lCount--)
            {
                //
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette.
                //
                ulByte = *pucData++ * 3;
                ulByte = *(unsigned long *)(pucPalette + ulByte) & 0x00ffffff;

                //
                // Translate this palette entry.
                //
                ulByte = GrOffScreen4BPPColorTranslate(pvDisplayData, ulByte);

                //
                // Write this pixel to the screen.
                //
                *pucPtr = (*pucPtr & ~(0xf << lX)) | (ulByte << lX);
                if(lX ^= 4)
                {
                    pucPtr++;
                }
            }

            //
            // The image data has been drawn.
            //
            break;
        }
    }
}

//*****************************************************************************
//
//! Draws a horizontal line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX1 is the X coordinate of the start of the line.
//! \param lX2 is the X coordinate of the end of the line.
//! \param lY is the Y coordinate of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a horizontal line on the display.  The coordinates of
//! the line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
GrOffScreen4BPPLineDrawH(void *pvDisplayData, long lX1, long lX2, long lY,
                         unsigned long ulValue)
{
    unsigned char *pucData;
    long lBytesPerRow;

    //
    // Check the arguments.
    //
    ASSERT(pvDisplayData);

    //
    // Create a character pointer for the display-specific data (which points
    // to the image buffer).
    //
    pucData = (unsigned char *)pvDisplayData;

    //
    // Compute the number of bytes per row in the image buffer.
    //
    lBytesPerRow = (*(unsigned short *)(pucData + 1) + 1) / 2;

    //
    // Get the offset to the byte of the image buffer that contains the
    // starting pixel.
    //
    pucData += (lBytesPerRow * lY) + (lX1 / 2) + 6 + (16 * 3);

    //
    // Copy the pixel value into all 8 pixels of the unsigned long.  This will
    // be used later to write multiple pixels into memory (as opposed to one at
    // a time).
    //
    ulValue = ((ulValue << 28) | (ulValue << 24) | (ulValue << 20) |
               (ulValue << 16) | (ulValue << 12) | (ulValue << 8) |
               (ulValue << 4) | ulValue);

    //
    // See if the second pixel in a byte is part of the line.
    //
    if(lX1 & 1)
    {
        //
        // Draw the second pixel in the byte.
        //
        *pucData = (*pucData & 0xf0) | (ulValue & 0x0f);
        pucData++;
        lX1++;
    }

    //
    // See if the buffer pointer is not half-word aligned and there are at
    // least two pixels left to draw.
    //
    if(((unsigned long)pucData & 1) && ((lX2 - lX1) > 0))
    {
        //
        // Draw two pixels to half-word align the buffer pointer.
        //
        *pucData++ = ulValue & 0xff;
        lX1 += 2;
    }

    //
    // See if the buffer pointer is not word aligned and there are at least
    // four pixels left to draw.
    //
    if(((unsigned long)pucData & 2) && ((lX2 - lX1) > 2))
    {
        //
        // Draw four pixels to word align the buffer pointer.
        //
        *(unsigned short *)pucData = ulValue & 0xffff;
        pucData += 2;
        lX1 += 4;
    }

    //
    // Loop while there are at least eight pixels left to draw.
    //
    while((lX1 + 7) <= lX2)
    {
        //
        // Draw eight pixels.
        //
        *(unsigned long *)pucData = ulValue;
        pucData += 4;
        lX1 += 8;
    }

    //
    // See if there are at least four pixels left to draw.
    //
    if((lX1 + 3) <= lX2)
    {
        //
        // Draw four pixels, leaving the buffer pointer half-word aligned.
        //
        *(unsigned short *)pucData = ulValue & 0xffff;
        pucData += 2;
        lX1 += 4;
    }

    //
    // See if there are at least two pixels left to draw.
    //
    if((lX1 + 1) <= lX2)
    {
        //
        // Draw two pixels, leaving the buffer pointer byte aligned.
        //
        *pucData++ = ulValue & 0xff;
        lX1 += 2;
    }

    //
    // See if there is one pixel left to draw.
    //
    if(lX1 == lX2)
    {
        //
        // Draw the final pixel.
        //
        *pucData = (*pucData & 0x0f) | (ulValue & 0xf0);
    }
}

//*****************************************************************************
//
//! Draws a vertical line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the line.
//! \param lY1 is the Y coordinate of the start of the line.
//! \param lY2 is the Y coordinate of the end of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a vertical line on the display.  The coordinates of the
//! line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
GrOffScreen4BPPLineDrawV(void *pvDisplayData, long lX, long lY1, long lY2,
                         unsigned long ulValue)
{
    unsigned char *pucData;
    long lBytesPerRow;

    //
    // Check the arguments.
    //
    ASSERT(pvDisplayData);

    //
    // Create a character pointer for the display-specific data (which points
    // to the image buffer).
    //
    pucData = (unsigned char *)pvDisplayData;

    //
    // Compute the number of bytes per row in the image buffer.
    //
    lBytesPerRow = (*(unsigned short *)(pucData + 1) + 1) / 2;

    //
    // Get the offset to the byte of the image buffer that contains the
    // starting pixel.
    //
    pucData += (lBytesPerRow * lY1) + (lX / 2) + 6 + (16 * 3);

    //
    // See if the vertical line resides in the first or second nibble.
    //
    if(lX & 1)
    {
        //
        // The line resides in the second nibble.  Loop over the rows of the
        // line.
        //
        while(lY1 <= lY2)
        {
            //
            // Draw this pixel of the line.
            //
            *pucData = (*pucData & 0xf0) | ulValue;
            pucData += lBytesPerRow;
            lY1++;
        }
    }
    else
    {
        //
        // The line resides in the first nibble.  Loop over the rows of the
        // line.
        //
        ulValue <<= 4;
        while(lY1 <= lY2)
        {
            //
            // Draw this pixel of the line.
            //
            *pucData = (*pucData & 0x0f) | ulValue;
            pucData += lBytesPerRow;
            lY1++;
        }
    }
}

//*****************************************************************************
//
//! Fills a rectangle.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param pRect is a pointer to the structure describing the rectangle.
//! \param ulValue is the color of the rectangle.
//!
//! This function fills a rectangle on the display.  The coordinates of the
//! rectangle are assumed to be within the extents of the display, and the
//! rectangle specification is fully inclusive (in other words, both sXMin and
//! sXMax are drawn, along with sYMin and sYMax).
//!
//! \return None.
//
//*****************************************************************************
static void
GrOffScreen4BPPRectFill(void *pvDisplayData, const tRectangle *pRect,
                        unsigned long ulValue)
{
    unsigned char *pucData, *pucColumn;
    long lBytesPerRow, lX, lY;

    //
    // Check the arguments.
    //
    ASSERT(pvDisplayData);
    ASSERT(pRect);

    //
    // Create a character pointer for the display-specific data (which points
    // to the image buffer).
    //
    pucData = (unsigned char *)pvDisplayData;

    //
    // Compute the number of bytes per row in the image buffer.
    //
    lBytesPerRow = (*(unsigned short *)(pucData + 1) + 1) / 2;

    //
    // Get the offset to the byte of the image buffer that contains the
    // starting pixel.
    //
    pucData += ((lBytesPerRow * pRect->sYMin) + (pRect->sXMin / 2) + 6 +
                (16 * 3));

    //
    // Copy the pixel value into all 8 pixels of the unsigned long.  This will
    // be used later to write multiple pixels into memory (as opposed to one at
    // a time).
    //
    ulValue = ((ulValue << 28) | (ulValue << 24) | (ulValue << 20) |
               (ulValue << 16) | (ulValue << 12) | (ulValue << 8) |
               (ulValue << 4) | ulValue);

    //
    // Get the starting X coordinate of the rectangle.
    //
    lX = pRect->sXMin;

    //
    // See if the second pixel in a byte is part of the rectangle.
    //
    if(lX & 1)
    {
        //
        // Draw the second pixel of this column of the rectangle.
        //
        for(lY = pRect->sYMin, pucColumn = pucData; lY <= pRect->sYMax;
            lY++, pucColumn += lBytesPerRow)
        {
            *pucColumn = (*pucColumn & 0xf0) | (ulValue & 0x0f);
        }
        pucData++;
        lX++;
    }

    //
    // See if the buffer pointer is not half-word aligned and there are at
    // least two pixel columns left to draw.
    //
    if(((unsigned long)pucData & 1) && (pRect->sXMax - lX) > 0)
    {
        //
        // Draw two pixels in this column of the rectangle.
        //
        for(lY = pRect->sYMin, pucColumn = pucData; lY <= pRect->sYMax;
            lY++, pucColumn += lBytesPerRow)
        {
            *pucColumn = ulValue & 0xff;
        }
        pucData++;
        lX += 2;
    }

    //
    // See if the buffer pointer is not word aligned and there are at least
    // four pixel columns left to draw.
    //
    if(((unsigned long)pucData & 2) && ((pRect->sXMax - lX) > 2))
    {
        //
        // Draw four pixels in this column of the rectangle.
        //
        for(lY = pRect->sYMin, pucColumn = pucData; lY <= pRect->sYMax;
            lY++, pucColumn += lBytesPerRow)
        {
            *(unsigned short *)pucColumn = ulValue & 0xffff;
        }
        pucData += 2;
        lX += 4;
    }

    //
    // Loop while there are at least eight pixel columns left to draw.
    //
    while((lX + 7) <= pRect->sXMax)
    {
        //
        // Draw eight pixels in this column of the rectangle.
        //
        for(lY = pRect->sYMin, pucColumn = pucData; lY <= pRect->sYMax;
            lY++, pucColumn += lBytesPerRow)
        {
            *(unsigned long *)pucColumn = ulValue;
        }
        pucData += 4;
        lX += 8;
    }

    //
    // See if there are at least four pixel columns left to draw.
    //
    if((lX + 3) <= pRect->sXMax)
    {
        //
        // Draw four pixel columns, leaving the buffer pointer half-word
        // aligned.
        //
        ulValue &= 0xffff;
        for(lY = pRect->sYMin, pucColumn = pucData; lY <= pRect->sYMax;
            lY++, pucColumn += lBytesPerRow)
        {
            *(unsigned short *)pucColumn = ulValue;
        }
        pucData += 2;
        lX += 4;
    }

    //
    // See if there are at least two pixel columns left to draw.
    //
    if((lX + 1) <= pRect->sXMax)
    {
        //
        // Draw two pixel columns, leaving the buffer pointer byte aligned.
        //
        ulValue &= 0xff;
        for(lY = pRect->sYMin, pucColumn = pucData; lY <= pRect->sYMax;
            lY++, pucColumn += lBytesPerRow)
        {
            *pucColumn = ulValue;
        }
        pucData++;
        lX += 2;
    }

    //
    // See if there is one pixel column left to draw.
    //
    if(lX == pRect->sXMax)
    {
        //
        // Draw the final pixel column.
        //
        ulValue = ulValue & 0xf0;
        for(lY = pRect->sYMin; lY <= pRect->sYMax;
            lY++, pucData += lBytesPerRow)
        {
            *pucData = (*pucData & 0x0f) | ulValue;
        }
    }
}

//*****************************************************************************
//
//! Flushes any cached drawing operations.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//!
//! This functions flushes any cached drawing operations to the display.  This
//! is useful when a local frame buffer is used for drawing operations, and the
//! flush would copy the local frame buffer to the display.  For the off-screen
//! display buffer driver, the flush is a no operation.
//!
//! \return None.
//
//*****************************************************************************
static void
GrOffScreen4BPPFlush(void *pvDisplayData)
{
    //
    // Check the arguments.
    //
    ASSERT(pvDisplayData);
}

//*****************************************************************************
//
//! Initializes a 4 BPP off-screen buffer.
//!
//! \param pDisplay is a pointer to the display structure to be configured for
//! the 4 BPP off-screen buffer.
//! \param pucImage is a pointer to the image buffer to be used for the
//! off-screen buffer.
//! \param lWidth is the width of the image buffer in pixels.
//! \param lHeight is the height of the image buffer in pixels.
//!
//! This function initializes a display structure, preparing it to draw into
//! the supplied image buffer.  The image buffer is assumed to be large enough
//! to hold an image of the specified geometry.
//!
//! \return None.
//
//*****************************************************************************
void
GrOffScreen4BPPInit(tDisplay *pDisplay, unsigned char *pucImage, long lWidth,
                    long lHeight)
{
    //
    // Check the arguments.
    //
    ASSERT(pDisplay);
    ASSERT(pucImage);

    //
    // Initialize the display structure.
    //
    pDisplay->lSize = sizeof(tDisplay);
    pDisplay->pvDisplayData = pucImage;
    pDisplay->usWidth = lWidth;
    pDisplay->usHeight = lHeight;
    pDisplay->pfnPixelDraw = GrOffScreen4BPPPixelDraw;
    pDisplay->pfnPixelDrawMultiple = GrOffScreen4BPPPixelDrawMultiple;
    pDisplay->pfnLineDrawH = GrOffScreen4BPPLineDrawH;
    pDisplay->pfnLineDrawV = GrOffScreen4BPPLineDrawV;
    pDisplay->pfnRectFill = GrOffScreen4BPPRectFill;
    pDisplay->pfnColorTranslate = GrOffScreen4BPPColorTranslate;
    pDisplay->pfnFlush = GrOffScreen4BPPFlush;

    //
    // Initialize the image buffer.
    //
    pucImage[0] = IMAGE_FMT_4BPP_UNCOMP;
    *(unsigned short *)(pucImage + 1) = lWidth;
    *(unsigned short *)(pucImage + 3) = lHeight;
    pucImage[5] = 15;
}

//*****************************************************************************
//
//! Sets the palette of a 4 BPP off-screen buffer.
//!
//! \param pDisplay is a pointer to the display structure for the 4 BPP
//! off-screen buffer.
//! \param pulPalette is a pointer to the array of 24-bit RGB values to be
//! placed into the palette.
//! \param ulOffset is the starting offset into the image palette.
//! \param ulCount is the number of palette entries to set.
//!
//! This function sets the entries of the palette used by the 4 BPP off-screen
//! buffer.  The palette is used to select colors for drawing via
//! GrOffScreen4BPPColorTranslate(), and for the final rendering of the image
//! to a real display via GrImageDraw().
//!
//! \return None.
//
//*****************************************************************************
void
GrOffScreen4BPPPaletteSet(tDisplay *pDisplay, unsigned long *pulPalette,
                          unsigned long ulOffset, unsigned long ulCount)
{
    unsigned char *pucData;

    //
    // Check the arguments.
    //
    ASSERT(pDisplay);
    ASSERT(pulPalette);
    ASSERT(ulOffset < 16);
    ASSERT((ulOffset + ulCount) <= 16);

    //
    // Get a pointer to the start of the image buffer's palette.
    //
    pucData = (unsigned char *)pDisplay->pvDisplayData + 6;

    //
    // Skip to the specified offset in the palette.
    //
    pucData += ulOffset * 3;

    //
    // Loop while there are more palette entries to add.
    //
    while(ulCount--)
    {
        //
        // Copy this palette entry to the image buffer's palette.
        //
        *pucData++ = (*pulPalette >> ClrBlueShift) & 0xff;
        *pucData++ = (*pulPalette >> ClrGreenShift) & 0xff;
        *pucData++ = (*pulPalette++ >> ClrRedShift) & 0xff;
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
