//*****************************************************************************
//
// offscr1bpp.c - 1 BPP off-screen display buffer driver.
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
// Translates a 24-bit RGB color to a display driver-specific color.
//
// \param c is the 24-bit RGB color.  The least-significant byte is the blue
// channel, the next byte is the green channel, and the third byte is the red
// channel.
//
// This macro translates a 24-bit RGB color into a value that can be written
// into the display's frame buffer in order to reproduce that color, or the
// closest possible approximation of that color.
//
// \return Returns the display-driver specific color.
//
//*****************************************************************************
#define DPYCOLORTRANSLATE(c)    ((((((c) & 0x00ff0000) >> 16) * 19661) + \
                                  ((((c) & 0x0000ff00) >> 8) * 38666) +  \
                                  (((c) & 0x000000ff) * 7209)) /         \
                                 (65536 * 128))

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
GrOffScreen1BPPPixelDraw(void *pvDisplayData, long lX, long lY,
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
    lBytesPerRow = (*(unsigned short *)(pucData + 1) + 7) / 8;

    //
    // Get the offset to the byte of the image buffer that contains the pixel
    // in question.
    //
    pucData += (lBytesPerRow * lY) + (lX / 8) + 5;

    //
    // Determine how much to shift to get to the bit that contains this pixel.
    //
    lX = 7 - (lX & 7);

    //
    // Write this pixel into the image buffer.
    //
    *pucData = (*pucData & ~(1 << lX)) | (ulValue << lX);
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
GrOffScreen1BPPPixelDrawMultiple(void *pvDisplayData, long lX, long lY,
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
    lBytesPerRow = (*(unsigned short *)(pucPtr + 1) + 7) / 8;

    //
    // Get the offset to the byte of the image buffer that contains the
    // starting pixel.
    //
    pucPtr += (lBytesPerRow * lY) + (lX / 8) + 5;

    //
    // Determine the bit position of the starting pixel.
    //
    lX = 7 - (lX & 7);

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
                    *pucPtr = ((*pucPtr & ~(1 << lX)) |
                               ((((unsigned long *)pucPalette)[(ulByte >>
                                                                (7 - lX0)) &
                                                               1]) << lX));
                    if(lX-- == 0)
                    {
                        lX = 7;
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
                        // Translate this palette entry and write it to the
                        // screen.
                        //
                        *pucPtr = ((*pucPtr & ~(1 << lX)) |
                                   (DPYCOLORTRANSLATE(ulByte) << lX));
                        if(lX-- == 0)
                        {
                            lX = 7;
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
                            // Translate this palette entry and write it to the
                            // screen.
                            //
                            *pucPtr = ((*pucPtr & ~(1 << lX)) |
                                       (DPYCOLORTRANSLATE(ulByte) << lX));
                            if(lX-- == 0)
                            {
                                lX = 7;
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
                // Translate this palette entry and write it to the screen.
                //
                *pucPtr = ((*pucPtr & ~(1 << lX)) |
                           (DPYCOLORTRANSLATE(ulByte) << lX));
                if(lX-- == 0)
                {
                    lX = 7;
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
GrOffScreen1BPPLineDrawH(void *pvDisplayData, long lX1, long lX2, long lY,
                         unsigned long ulValue)
{
    long lBytesPerRow, lMask;
    unsigned char *pucData;

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
    lBytesPerRow = (*(unsigned short *)(pucData + 1) + 7) / 8;

    //
    // Get the offset to the byte of the image buffer that contains the
    // starting pixel.
    //
    pucData += (lBytesPerRow * lY) + (lX1 / 8) + 5;

    //
    // Copy the pixel value into all 32 pixels of the unsigned long.  This will
    // be used later to write multiple pixels into memory (as opposed to one at
    // a time).
    //
    if(ulValue)
    {
        ulValue = 0xffffffff;
    }

    //
    // See if the current buffer byte contains pixels that should be left
    // unmodified.
    //
    if(lX1 & 7)
    {
        //
        // Compute the mask to access only the appropriate pixels within this
        // byte.  The line may start and stop within this byte, so the mask may
        // need to be shortened to account for this situation.
        //
        lMask = 8 - (lX1 & 7);
        if(lMask > (lX2 - lX1 + 1))
        {
            lMask = lX2 - lX1 + 1;
        }
        lMask = ((1 << lMask) - 1) << (8 - (lX1 & 7) - lMask);

        //
        // Draw the appropriate pixels within this byte.
        //
        *pucData = (*pucData & ~lMask) | (ulValue & lMask);
        pucData++;
        lX1 = (lX1 + 7) & ~7;
    }

    //
    // See if the buffer pointer is not half-word aligned and there are at
    // least eight pixels left to draw.
    //
    if(((unsigned long)pucData & 1) && ((lX2 - lX1) > 6))
    {
        //
        // Draw eight pixels to half-word align the buffer pointer.
        //
        *pucData++ = ulValue & 0xff;
        lX1 += 8;
    }

    //
    // See if the buffer pointer is not word aligned and there are at least
    // sixteen pixels left to draw.
    //
    if(((unsigned long)pucData & 2) && ((lX2 - lX1) > 14))
    {
        //
        // Draw sixteen pixels to word align the buffer pointer.
        //
        *(unsigned short *)pucData = ulValue & 0xffff;
        pucData += 2;
        lX1 += 16;
    }

    //
    // Loop while there are at least thirty two pixels left to draw.
    //
    while((lX1 + 31) <= lX2)
    {
        //
        // Draw thirty two pixels.
        //
        *(unsigned long *)pucData = ulValue;
        pucData += 4;
        lX1 += 32;
    }

    //
    // See if there are at least sixteen pixels left to draw.
    //
    if((lX1 + 15) <= lX2)
    {
        //
        // Draw sixteen pixels, leaving the buffer pointer half-word aligned.
        //
        *(unsigned short *)pucData = ulValue & 0xffff;
        pucData += 2;
        lX1 += 16;
    }

    //
    // See if there are at least eight pixels left to draw.
    //
    if((lX1 + 7) <= lX2)
    {
        //
        // Draw eight pixels, leaving the buffer pointer byte aligned.
        //
        *pucData++ = ulValue & 0xff;
        lX1 += 8;
    }

    //
    // See if there are any pixels left to draw.
    //
    if(lX1 <= lX2)
    {
        //
        // Draw the remaining pixels.
        //
        lMask = 0xff >> (lX2 - lX1 + 1);
        *pucData = (*pucData & lMask) | (ulValue & ~lMask);
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
GrOffScreen1BPPLineDrawV(void *pvDisplayData, long lX, long lY1, long lY2,
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
    lBytesPerRow = (*(unsigned short *)(pucData + 1) + 7) / 8;

    //
    // Get the offset to the byte of the image buffer that contains the
    // starting pixel.
    //
    pucData += (lBytesPerRow * lY1) + (lX / 8) + 5;

    //
    // Determine how much to shift to get to the bit that contains this pixel.
    //
    lX = 7 - (lX & 7);

    //
    // Shift the pixel value up to the correct bit position, and create a mask
    // to preserve the value of the remaining pixels.
    //
    ulValue <<= lX;
    lX = ~(1 << lX);

    //
    // Loop over the rows of the line.
    //
    for(; lY1 <= lY2; lY1++)
    {
        //
        // Draw this pixel of the line.
        //
        *pucData = (*pucData & lX) | ulValue;
        pucData += lBytesPerRow;
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
GrOffScreen1BPPRectFill(void *pvDisplayData, const tRectangle *pRect,
                        unsigned long ulValue)
{
    unsigned char *pucData, *pucColumn;
    long lBytesPerRow, lMask, lX, lY;

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
    lBytesPerRow = (*(unsigned short *)(pucData + 1) + 7) / 8;

    //
    // Get the offset to the byte of the image buffer that contains the
    // starting pixel.
    //
    pucData += (lBytesPerRow * pRect->sYMin) + (pRect->sXMin / 8) + 5;

    //
    // Copy the pixel value into all 32 pixels of the unsigned long.  This will
    // be used later to write multiple pixels into memory (as opposed to one at
    // a time).
    //
    if(ulValue)
    {
        ulValue = 0xffffffff;
    }

    //
    // Get the starting X coordinate of the rectangle.
    //
    lX = pRect->sXMin;

    //
    // See if the current buffer byte contains pixel columns that should be
    // left unmodified.
    //
    if(lX & 7)
    {
        //
        // Compute the mask to access only the appropriate pixels within this
        // byte column.  The rectangle may start and stop within this byte
        // column, so the mask may need to be shortened to account for this
        // situation.
        //
        lMask = 8 - (lX & 7);
        if(lMask > (pRect->sXMax - lX + 1))
        {
            lMask = pRect->sXMax - lX + 1;
        }
        lMask = ((1 << lMask) - 1) << (8 - (lX & 7) - lMask);

        //
        // Draw the appropriate pixels within this column.
        //
        for(lY = pRect->sYMin, pucColumn = pucData; lY <= pRect->sYMax;
            lY++, pucColumn += lBytesPerRow)
        {
            *pucColumn = (*pucColumn & ~lMask) | (ulValue & lMask);
        }
        pucData++;
        lX = (lX + 7) & ~7;
    }

    //
    // See if the buffer pointer is not half-word aligned and there are at
    // least eight pixel columns left to draw.
    //
    if(((unsigned long)pucData & 1) && ((pRect->sXMax - lX) > 6))
    {
        //
        // Draw eight pixel columns to half-word align the buffer pointer.
        //
        for(lY = pRect->sYMin, pucColumn = pucData; lY <= pRect->sYMax;
            lY++, pucColumn += lBytesPerRow)
        {
            *pucColumn = ulValue & 0xff;
        }
        pucData++;
        lX += 8;
    }

    //
    // See if the buffer pointer is not word aligned and there are at least
    // sixteen pixel columns left to draw.
    //
    if(((unsigned long)pucData & 2) && ((pRect->sXMax - lX) > 14))
    {
        //
        // Draw sixteen pixel columns to word align the buffer pointer.
        //
        for(lY = pRect->sYMin, pucColumn = pucData; lY <= pRect->sYMax;
            lY++, pucColumn += lBytesPerRow)
        {
            *(unsigned short *)pucColumn = ulValue & 0xffff;
        }
        pucData += 2;
        lX += 16;
    }

    //
    // Loop while there are at least thirty two pixel columnss left to draw.
    //
    while((lX + 31) <= pRect->sXMax)
    {
        //
        // Draw thirty two pixel columnss.
        //
        for(lY = pRect->sYMin, pucColumn = pucData; lY <= pRect->sYMax;
            lY++, pucColumn += lBytesPerRow)
        {
            *(unsigned long *)pucColumn = ulValue;
        }
        pucData += 4;
        lX += 32;
    }

    //
    // See if there are at least sixteen pixel columnss left to draw.
    //
    if((lX + 15) <= pRect->sXMax)
    {
        //
        // Draw sixteen pixel columns, leaving the buffer pointer half-word
        // aligned.
        //
        ulValue &= 0xffff;
        for(lY = pRect->sYMin, pucColumn = pucData; lY <= pRect->sYMax;
            lY++, pucColumn += lBytesPerRow)
        {
            *(unsigned short *)pucColumn = ulValue;
        }
        pucData += 2;
        lX += 16;
    }

    //
    // See if there are at least eight pixel columns left to draw.
    //
    if((lX + 7) <= pRect->sXMax)
    {
        //
        // Draw eight pixel columns, leaving the buffer pointer byte aligned.
        //
        ulValue &= 0xff;
        for(lY = pRect->sYMin, pucColumn = pucData; lY <= pRect->sYMax;
            lY++, pucColumn += lBytesPerRow)
        {
            *pucColumn = ulValue;
        }
        pucData++;
        lX += 8;
    }

    //
    // See if there are any pixel columns left to draw.
    //
    if(lX <= pRect->sXMax)
    {
        //
        // Draw the remaining pixel columns.
        //
        lMask = 0xff >> (pRect->sXMax - lX + 1);
        ulValue &= ~lMask;
        for(lY = pRect->sYMin; lY <= pRect->sYMax;
            lY++, pucData += lBytesPerRow)
        {
            *pucData = (*pucData & lMask) | ulValue;
        }
    }
}

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
GrOffScreen1BPPColorTranslate(void *pvDisplayData, unsigned long ulValue)
{
    //
    // Check the arguments.
    //
    ASSERT(pvDisplayData);

    //
    // Translate from a 24-bit RGB color to black or white.
    //
    return(DPYCOLORTRANSLATE(ulValue));
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
GrOffScreen1BPPFlush(void *pvDisplayData)
{
    //
    // Check the arguments.
    //
    ASSERT(pvDisplayData);
}

//*****************************************************************************
//
//! Initializes a 1 BPP off-screen buffer.
//!
//! \param pDisplay is a pointer to the display structure to be configured for
//! the 1 BPP off-screen buffer.
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
GrOffScreen1BPPInit(tDisplay *pDisplay, unsigned char *pucImage, long lWidth,
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
    pDisplay->pfnPixelDraw = GrOffScreen1BPPPixelDraw;
    pDisplay->pfnPixelDrawMultiple = GrOffScreen1BPPPixelDrawMultiple;
    pDisplay->pfnLineDrawH = GrOffScreen1BPPLineDrawH;
    pDisplay->pfnLineDrawV = GrOffScreen1BPPLineDrawV;
    pDisplay->pfnRectFill = GrOffScreen1BPPRectFill;
    pDisplay->pfnColorTranslate = GrOffScreen1BPPColorTranslate;
    pDisplay->pfnFlush = GrOffScreen1BPPFlush;

    //
    // Initialize the image buffer.
    //
    pucImage[0] = IMAGE_FMT_1BPP_UNCOMP;
    *(unsigned short *)(pucImage + 1) = lWidth;
    *(unsigned short *)(pucImage + 3) = lHeight;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
