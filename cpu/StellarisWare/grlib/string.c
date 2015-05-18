//*****************************************************************************
//
// string.c - Routines for drawing text.
//
// Copyright (c) 2007-2012 Texas Instruments Incorporated.  All rights reserved.
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
// The character printed by GrStringDraw in place of any character in the
// string which does not appear in the font.  When using a font which does not
// include this character, a space is left instead.
//
//*****************************************************************************
#define ABSENT_CHAR_REPLACEMENT '.'

//*****************************************************************************
//
//! Determines the width of a string.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param pcString is the string in question.
//! \param lLength is the length of the string.
//!
//! This function determines the width of a string (or portion of the string)
//! when drawn with a particular font.  The \e lLength parameter allows a
//! portion of the string to be examined without having to insert a NULL
//! character at the stopping point (would not be possible if the string was
//! located in flash); specifying a length of -1 will cause the width of the
//! entire string to be computed.
//!
//! \return Returns the width of the string in pixels.
//
//*****************************************************************************
#ifdef GRLIB_REMOVE_WIDE_FONT_SUPPORT
long
GrStringWidthGet(const tContext *pContext, const char *pcString, long lLength)
{
    const unsigned short *pusOffset;
    const unsigned char *pucData;
    unsigned char ucFirst, ucLast, ucAbsent;
    long lWidth;

    //
    // Check the arguments.
    //
    ASSERT(pContext);
    ASSERT(pcString);

    //
    // This function doesn't support wide character fonts or wrapped fonts.
    //
    ASSERT(!(pContext->pFont->ucFormat &&
            (FONT_FMT_WRAPPED | FONT_WIDE_MARKER)));

    //
    // Get some pointers to relevant information in the font to make things
    // easier, and give the compiler a hint about extraneous loads that it can
    // avoid.
    //
    if(pContext->pFont->ucFormat & FONT_EX_MARKER)
    {
        tFontEx *pFont;

        pFont = (tFontEx *)(pContext->pFont);

        pucData = pFont->pucData;
        pusOffset = pFont->pusOffset;
        ucFirst = pFont->ucFirst;
        ucLast = pFont->ucLast;

        //
        // Does the default absent character replacement exist in the font?
        //
        if((ABSENT_CHAR_REPLACEMENT >= ucFirst) &&
           (ABSENT_CHAR_REPLACEMENT <= ucLast))
        {
            //
            // Yes - use the standard character when an absent character is
            // found.
            //
            ucAbsent = ABSENT_CHAR_REPLACEMENT;
        }
        else
        {
            //
            // The default absent character is not present in the font so use
            // the first character (we only use its width here) instead.
            //
            ucAbsent = pFont->ucFirst;
        }
    }
    else
    {
        pucData = pContext->pFont->pucData;
        pusOffset = pContext->pFont->pusOffset;
        ucFirst = 32;
        ucLast = 126;
        ucAbsent = ABSENT_CHAR_REPLACEMENT;
    }

    //
    // Loop through the characters in the string.
    //
    for(lWidth = 0; *pcString && lLength; pcString++, lLength--)
    {
        //
        // Get a pointer to the font data for the next character from the
        // string.  If there is not a glyph for the next character, replace it
        // with a ".".
        //
        if((*pcString >= ucFirst) && (*pcString <= ucLast))
        {
            //
            // Add the width of this character as drawn with the given font.
            //
            lWidth += pucData[pusOffset[*pcString - ucFirst] + 1];
        }
        else
        {
            //
            // This character does not exist in the font so replace it with
            // a '.' instead.  This matches the approach taken in GrStringDraw
            // and ensures that the width returned here represents the
            // rendered dimension of the string.
            //
            lWidth += pucData[pusOffset[ucAbsent - ucFirst] + 1];
        }
    }

    //
    // Return the width of the string.
    //
    return(lWidth);
}
#else
long
GrStringWidthGet(const tContext *pContext, const char *pcString, long lLength)
{
    const unsigned char *pucData;
    unsigned char ucWidth, ucHeight, ucBaseline, ucFormat;
    unsigned long ulCount, ulChar, ulSkip;
    long lWidth;

    //
    // Check the arguments.
    //
    ASSERT(pContext);
    ASSERT(pcString);

    //
    // Initialize our string length.
    //
    lWidth = 0;

    //
    // Set the maximum number of characters we should render.  Note that the
    // value -1 is used to indicate that the function should render until it
    // hits the end of the string so casting it to an unsigned long here is
    // fine since this says keep rendering for 2^32 characters.  We are very
    // unlikely to ever be passed a string this long.
    //
    ulCount = (unsigned long)lLength;

    //
    // Loop through each character in the string.
    //
    while(ulCount)
    {
        //
        // Get the next character to render.
        //
        ulChar = GrStringNextCharGet(pContext, pcString, ulCount, &ulSkip);

        //
        // If we ran out of characters to render, drop out of the loop.
        //
        if(!ulChar)
        {
            break;
        }

        //
        // Get information on this glyph.
        //
        pucData = GrFontGlyphDataGet(pContext->pFont, ulChar, &ucWidth);

        //
        // Does the glyph exist?
        //
        if(!pucData)
        {
            //
            // No - get the absent character replacement information.
            //
            pucData = GrFontGlyphDataGet(pContext->pFont,
                                         ABSENT_CHAR_REPLACEMENT, &ucWidth);

            //
            // Does this character exist in the font?
            //
            if(!pucData)
            {
                //
                // No - look for the ASCII/Unicode space character.
                //
                pucData = GrFontGlyphDataGet(pContext->pFont, 0x20,
                                             &ucWidth);

                //
                // Does this exist?
                //
                if(!pucData)
                {
                    //
                    // No - give up and just pad with a character cell of space.
                    //
                    GrFontInfoGet(pContext->pFont, &ucFormat, &ucWidth,
                                  &ucHeight, &ucBaseline);
                }
            }
        }

        //
        // Increment our string length.
        //
        lWidth += (long)ucWidth;

        //
        // Move on to the next character.
        //
        pcString += ulSkip;
        ulCount -= ulSkip;
    }

    //
    // Return the width of the string.
    //
    return(lWidth);
}
#endif

//*****************************************************************************
//
//! Draws a string.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param pcString is a pointer to the string to be drawn.
//! \param lLength is the number of characters from the string that should be
//! drawn on the screen.
//! \param lX is the X coordinate of the upper left corner of the string
//! position on the screen.
//! \param lY is the Y coordinate of the upper left corner of the string
//! position on the screen.
//! \param bOpaque is true of the background of each character should be drawn
//! and false if it should not (leaving the background as is).
//!
//! This function draws a string of text on the screen.  The \e lLength
//! parameter allows a portion of the string to be examined without having to
//! insert a NULL character at the stopping point (which would not be possible
//! if the string was located in flash); specifying a length of -1 will cause
//! the entire string to be rendered (subject to clipping).
//!
//! \return None.
//
//*****************************************************************************
#ifdef GRLIB_REMOVE_WIDE_FONT_SUPPORT
//
// This version of GrStringDraw supports the original tFont and tFontEx ASCII
// and ISO8859 (8 bit) fonts only.
//
void
GrStringDraw(const tContext *pContext, const char *pcString, long lLength,
             long lX, long lY, unsigned long bOpaque)
{
    long lIdx, lX0, lY0, lCount, lOff, lOn, lBit;
    const unsigned char *pucData;
    const unsigned char *pucGlyphs;
    const unsigned short *pusOffset;
    unsigned char ucFirst, ucLast, ucAbsent;
    tContext sCon;

    //
    // Check the arguments.
    //
    ASSERT(pContext);
    ASSERT(pcString);

    //
    // This function doesn't support wide character fonts or wrapped fonts.
    //
    ASSERT(!(pContext->pFont->ucFormat &&
            (FONT_FMT_WRAPPED | FONT_WIDE_MARKER)));

    //
    // Copy the drawing context into a local structure that can be modified.
    //
    sCon = *pContext;

    //
    // Extract various parameters from the font depending upon whether it's
    // in the tFont or tFontEx format.
    //
    if(pContext->pFont->ucFormat & FONT_EX_MARKER)
    {
        tFontEx *pFont;

        pFont = (tFontEx *)(pContext->pFont);

        pucGlyphs = pFont->pucData;
        pusOffset = pFont->pusOffset;
        ucFirst = pFont->ucFirst;
        ucLast = pFont->ucLast;

        //
        // Does the default absent character replacement exist in the font?
        //
        if((ABSENT_CHAR_REPLACEMENT >= ucFirst) &&
           (ABSENT_CHAR_REPLACEMENT <= ucLast))
        {
            //
            // Yes - use the standard character when an absent character is
            // found.
            //
            ucAbsent = ABSENT_CHAR_REPLACEMENT;
        }
        else
        {
            //
            // The default absent character is not present in the font so use
            // the first character instead.
            //
            ucAbsent = pFont->ucFirst;
        }
    }
    else
    {
        pucGlyphs = pContext->pFont->pucData;
        pusOffset = pContext->pFont->pusOffset;
        ucFirst = 32;
        ucLast = 126;
        ucAbsent = ABSENT_CHAR_REPLACEMENT;
    }

    //
    // Loop through the characters in the string.
    //
    while(*pcString && lLength--)
    {
        //
        // Stop drawing the string if the right edge of the clipping region has
        // been exceeded.
        //
        if(lX > sCon.sClipRegion.sXMax)
        {
            break;
        }

        //
        // Get a pointer to the font data for the next character from the
        // string.  If there is not a glyph for the next character, replace it
        // with the "absent" character (usually '.').
        //
        if((*pcString >= ucFirst) && (*pcString <= ucLast))
        {
            pucData = (pucGlyphs + pusOffset[*pcString - ucFirst]);
        }
        else
        {
            pucData = (pucGlyphs + pusOffset[ucAbsent - ucFirst]);
        }
        pcString++;

        //
        // See if the entire character is to the left of the clipping region.
        //
        if((lX + pucData[1]) < sCon.sClipRegion.sXMin)
        {
            //
            // Increment the X coordinate by the width of the character.
            //
            lX += pucData[1];

            //
            // Go to the next character in the string.
            //
            continue;
        }

        //
        // Loop through the bytes in the encoded data for this glyph.
        //
        for(lIdx = 2, lX0 = 0, lBit = 0, lY0 = 0; lIdx < pucData[0]; )
        {
            //
            // See if the bottom of the clipping region has been exceeded.
            //
            if((lY + lY0) > sCon.sClipRegion.sYMax)
            {
                //
                // Stop drawing this character.
                //
                break;
            }

            //
            // See if the font is uncompressed.
            //
            if((sCon.pFont->ucFormat & ~FONT_EX_MARKER) ==
                FONT_FMT_UNCOMPRESSED)
            {
                //
                // Count the number of off pixels from this position in the
                // glyph image.
                //
                for(lOff = 0; lIdx < pucData[0]; )
                {
                    //
                    // Get the number of zero pixels at this position.
                    //
                    lCount = NumLeadingZeros(pucData[lIdx] << (24 + lBit));

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
                        lIdx++;
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
                for(lOn = 0; lIdx < pucData[0]; )
                {
                    //
                    // Get the number of one pixels at this location (by
                    // inverting the data and counting the number of zeros).
                    //
                    lCount = NumLeadingZeros(~(pucData[lIdx] << (24 + lBit)));

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
                        lIdx++;
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
            }

            //
            // Otherwise, the font is compressed with a pixel RLE scheme.
            //
            else
            {
                //
                // See if this is a byte that encodes some on and off pixels.
                //
                if(pucData[lIdx])
                {
                    //
                    // Extract the number of off pixels.
                    //
                    lOff = (pucData[lIdx] >> 4) & 15;

                    //
                    // Extract the number of on pixels.
                    //
                    lOn = pucData[lIdx] & 15;

                    //
                    // Skip past this encoded byte.
                    //
                    lIdx++;
                }

                //
                // Otherwise, see if this is a repeated on pixel byte.
                //
                else if(pucData[lIdx + 1] & 0x80)
                {
                    //
                    // There are no off pixels in this encoding.
                    //
                    lOff = 0;

                    //
                    // Extract the number of on pixels.
                    //
                    lOn = (pucData[lIdx + 1] & 0x7f) * 8;

                    //
                    // Skip past these two encoded bytes.
                    //
                    lIdx += 2;
                }

                //
                // Otherwise, this is a repeated off pixel byte.
                //
                else
                {
                    //
                    // Extract the number of off pixels.
                    //
                    lOff = pucData[lIdx + 1] * 8;

                    //
                    // There are no on pixels in this encoding.
                    //
                    lOn = 0;

                    //
                    // Skip past these two encoded bytes.
                    //
                    lIdx += 2;
                }
            }

            //
            // Loop while there are any off pixels.
            //
            while(lOff)
            {
                //
                // See if the bottom of the clipping region has been exceeded.
                //
                if((lY + lY0) > sCon.sClipRegion.sYMax)
                {
                    //
                    // Ignore the remainder of the on pixels.
                    //
                    break;
                }

                //
                // See if there is more than one on pixel that will fit onto
                // the current row.
                //
                if((lOff > 1) && ((lX0 + 1) < pucData[1]))
                {
                    //
                    // Determine the number of on pixels that will fit on this
                    // row.
                    //
                    lCount = (((lX0 + lOff) > pucData[1]) ? pucData[1] - lX0 :
                              lOff);

                    //
                    // If this row is within the clipping region, draw a
                    // horizontal line that corresponds to the sequence of on
                    // pixels.
                    //
                    if(((lY + lY0) >= sCon.sClipRegion.sYMin) && bOpaque)
                    {
                        sCon.ulForeground = pContext->ulBackground;
                        GrLineDrawH(&sCon, lX + lX0, lX + lX0 + lCount - 1,
                                    lY + lY0);
                    }

                    //
                    // Decrement the count of on pixels by the number on this
                    // row.
                    //
                    lOff -= lCount;

                    //
                    // Increment the X offset by the number of on pixels.
                    //
                    lX0 += lCount;
                }

                //
                // Otherwise, there is only a single on pixel that can be
                // drawn.
                //
                else
                {
                    //
                    // If this pixel is within the clipping region, then draw
                    // it.
                    //
                    if(((lX + lX0) >= sCon.sClipRegion.sXMin) &&
                       ((lX + lX0) <= sCon.sClipRegion.sXMax) &&
                       ((lY + lY0) >= sCon.sClipRegion.sYMin) && bOpaque)
                    {
                        DpyPixelDraw(pContext->pDisplay, lX + lX0, lY + lY0,
                                     pContext->ulBackground);
                    }

                    //
                    // Decrement the count of on pixels.
                    //
                    lOff--;

                    //
                    // Increment the X offset.
                    //
                    lX0++;
                }

                //
                // See if the X offset has reached the right side of the
                // character glyph.
                //
                if(lX0 == pucData[1])
                {
                    //
                    // Increment the Y offset.
                    //
                    lY0++;

                    //
                    // Reset the X offset to the left side of the character
                    // glyph.
                    //
                    lX0 = 0;
                }
            }

            //
            // Loop while there are any on pixels.
            //
            while(lOn)
            {
                //
                // See if the bottom of the clipping region has been exceeded.
                //
                if((lY + lY0) > sCon.sClipRegion.sYMax)
                {
                    //
                    // Ignore the remainder of the on pixels.
                    //
                    break;
                }

                //
                // See if there is more than one on pixel that will fit onto
                // the current row.
                //
                if((lOn > 1) && ((lX0 + 1) < pucData[1]))
                {
                    //
                    // Determine the number of on pixels that will fit on this
                    // row.
                    //
                    lCount = (((lX0 + lOn) > pucData[1]) ? pucData[1] - lX0 :
                              lOn);

                    //
                    // If this row is within the clipping region, draw a
                    // horizontal line that corresponds to the sequence of on
                    // pixels.
                    //
                    if((lY + lY0) >= sCon.sClipRegion.sYMin)
                    {
                        sCon.ulForeground = pContext->ulForeground;
                        GrLineDrawH(&sCon, lX + lX0, lX + lX0 + lCount - 1,
                                    lY + lY0);
                    }

                    //
                    // Decrement the count of on pixels by the number on this
                    // row.
                    //
                    lOn -= lCount;

                    //
                    // Increment the X offset by the number of on pixels.
                    //
                    lX0 += lCount;
                }

                //
                // Otherwise, there is only a single on pixel that can be
                // drawn.
                //
                else
                {
                    //
                    // If this pixel is within the clipping region, then draw
                    // it.
                    //
                    if(((lX + lX0) >= sCon.sClipRegion.sXMin) &&
                       ((lX + lX0) <= sCon.sClipRegion.sXMax) &&
                       ((lY + lY0) >= sCon.sClipRegion.sYMin))
                    {
                        DpyPixelDraw(pContext->pDisplay, lX + lX0, lY + lY0,
                                     pContext->ulForeground);
                    }

                    //
                    // Decrement the count of on pixels.
                    //
                    lOn--;

                    //
                    // Increment the X offset.
                    //
                    lX0++;
                }

                //
                // See if the X offset has reached the right side of the
                // character glyph.
                //
                if(lX0 == pucData[1])
                {
                    //
                    // Increment the Y offset.
                    //
                    lY0++;

                    //
                    // Reset the X offset to the left side of the character
                    // glyph.
                    //
                    lX0 = 0;
                }
            }
        }

        //
        // Increment the X coordinate by the width of the character.
        //
        lX += pucData[1];
    }
}
#else
//
// This version of GrStringDraw supports the original tFont and tFontEx ASCII
// and ISO8859 (8 bit) fonts along with new wide character set, relocatable
// fonts.  Support for source and font codepages is also included.
//
void
GrStringDraw(const tContext *pContext, const char *pcString, long lLength,
             long lX, long lY, unsigned long bOpaque)
{
    ASSERT(pContext);
    ASSERT(pContext->pfnStringRenderer);

    //
    // Call the currently registered string rendering function.
    //
    pContext->pfnStringRenderer(pContext, pcString, lLength, lX, lY, bOpaque);
}

//*****************************************************************************
//
//! The default text string rendering function.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param pcString is a pointer to the string to be drawn.
//! \param lLength is the number of characters from the string that should be
//! drawn on the screen.
//! \param lX is the X coordinate of the upper left corner of the string
//! position on the screen.
//! \param lY is the Y coordinate of the upper left corner of the string
//! position on the screen.
//! \param bOpaque is true of the background of each character should be drawn
//! and false if it should not (leaving the background as is).
//!
//! This function acts as the default string rendering function called by
//! GrStringDraw() if no language-specific renderer is registered. It draws a
//! string of text on the screen using the text orientation currently set in
//! the graphics context.  The \e lLength parameter allows a portion of the
//! string to be examined without having to insert a NULL character at the
//! stopping point (which would not be possible if the string was located in
//! flash); specifying a length of -1 will cause the entire string to be
//! rendered (subject to clipping).
//!
//! Applications are not expected to call this function directly but should
//! call GrStringDraw() instead.  This function is provided as an aid to
//! language-specific renders which may call it to render parts of a string
//! at particular positions after dealing with any language-specific layout
//! issues such as, for example, inserting left-to-right numbers within a
//! right-to-left Arabic string.
//!
//! \return None.
//
//*****************************************************************************
void
GrDefaultStringRenderer(const tContext *pContext, const char *pcString,
                        long lLength, long lX, long lY, unsigned long bOpaque)
{
    unsigned char ucFormat, ucWidth, ucMaxWidth, ucHeight, ucBaseline;
    unsigned long ulChar, ulCount, ulSkip;
    const unsigned char *pucData;

    //
    // Check the arguments.
    //
    ASSERT(pContext);
    ASSERT(pcString);

    //
    // Get information on the font we are rendering the text in.
    //
    GrFontInfoGet(pContext->pFont, &ucFormat, &ucMaxWidth, &ucHeight,
                  &ucBaseline);

    //
    // If the string is completely outside the clipping region, don't even
    // start rendering it.
    //
    if((lY > pContext->sClipRegion.sYMax) ||
       ((lY + ucHeight) < pContext->sClipRegion.sYMin))
    {
        return;
    }

    //
    // Set the maximum number of characters we should render.  Note that the
    // value -1 is used to indicate that the function should render until it
    // hits the end of the string so casting it to an unsigned long here is
    // fine since this says keep rendering for 2^32 characters.  We are very
    // unlikely to ever be passed a string this long.
    //
    ulCount = (unsigned long)lLength;

    //
    // Loop through each character in the string.
    //
    while(ulCount)
    {
        //
        // Get the next character to render.
        //
        ulChar = GrStringNextCharGet(pContext, pcString, ulCount, &ulSkip);

        //
        // If we ran out of characters to render, return immediately.
        //
        if(!ulChar)
        {
            return;
        }

        //
        // If we are already outside the clipping region, exit early.
        //
        if(lX >= pContext->sClipRegion.sXMax)
        {
            return;
        }

        //
        // Get the glyph data pointer for this character.
        //
        pucData = GrFontGlyphDataGet(pContext->pFont, ulChar, &ucWidth);

        //
        // Does this glyph exist in the font?
        //
        if(!pucData)
        {
            //
            // Look for the character we are supposed to use in place of absent
            // glyphs.
            //
            pucData = GrFontGlyphDataGet(pContext->pFont,
                                         ABSENT_CHAR_REPLACEMENT, &ucWidth);

            //
            // Does this glyph exist in the font?
            //
            if(!pucData)
            {
                //
                // Last chance - look for the space character.
                //
                pucData = GrFontGlyphDataGet(pContext->pFont, ' ', &ucWidth);
            }
        }

        //
        // Did we find something to render?
        //
        if(pucData)
        {
            GrFontGlyphRender(pContext, pucData, lX, lY,
                              (ucFormat & FONT_FMT_PIXEL_RLE) ? true : false,
                              bOpaque);
            lX += ucWidth;
        }
        else
        {
            //
            // Leave a space in place of the undefined glyph.
            //
            lX += ucMaxWidth;
        }

        //
        // Move on to the next character.
        //
        pcString += ulSkip;
        ulCount -= ulSkip;
    }
}

//*****************************************************************************
//
//! Returns the codepoint of the first character in a string.
//!
//! \param pContext points to the graphics context in use.
//! \param pcString points to the first byte of the string from which the
//!        next character is to be parsed.
//! \param ulCount provides the number of bytes in the pcString buffer.
//! \param pulSkip points to storage which will be written with the number of
//!        bytes that must be skipped in the string buffer to move past the
//!        current character.
//!
//! This function is used to walk through a string extracting one character at
//! a time.  The input string is assumed to be encoded using the currently-
//! selected string codepage (as set via a call to the GrStringCodepageSet()
//! function).  The value returned is the codepoint of the first character in
//! the string as mapped into the current font's codepage.  This may be passed
//! to the GrFontGlyphDataGet() function to retrieve the glyph data for the
//! character.
//!
//! Since variable length encoding schemes such as UTF-8 are supported, this
//! function also returns information on the size of the character that has
//! been parsed, allowing the caller to increment the string pointer by the
//! relevant amount before querying the next character in the string.
//!
//! \return Returns the font codepoint representing the first character in the
//! string or 0 if no valid character was found.
//!
//*****************************************************************************
unsigned long
GrStringNextCharGet(const tContext *pContext, const char *pcString,
                    unsigned long ulCount, unsigned long *pulSkip)
{
    ASSERT(pContext);
    ASSERT(pcString);
    ASSERT(pulSkip);

    //
    // If the string is empty, return immediately.
    //
    if(!ulCount)
    {
        return(0);
    }

    //
    // Has a codepage mapping table been registered for this context?
    //
    if(pContext->pCodePointMapTable)
    {
        //
        // Yes - use the relevant mapping function
        //
        return(pContext->pCodePointMapTable[
              pContext->ucCodePointMap].pfnMapChar(pcString, ulCount, pulSkip));
    }
    else
    {
        //
        // No codepage mapping table has been registered so fall back on the
        // assumption that we are using ASCII or ISO8859-1 for both the
        // string and font codepages (i.e. the legacy case).
        //
        *pulSkip = 1;
        return((unsigned long)*pcString);
    }
}

//*****************************************************************************
//
//! Renders a single character glyph on the display at a given position.
//!
//! \param pContext points to the graphics context in use.
//! \param pucData points to the first byte of data for the glyph to be
//!        rendered.
//! \param lX is the X coordinate of the top left pixel of the glyph.
//! \param lY is the Y coordinate of the top left pixel of the glyph.
//! \param bCompressed is \b true if the data pointed to by \b pucData is in
//!        compressed format or \b false if uncompressed.
//! \param bOpaque is \b true of background pixels are to be written or \b
//!        false if only foreground pixels are drawn.
//!
//! This function is included as an aid to language-specific string rendering
//! functions.  Applications are expected to render strings and characters
//! using calls to GrStringDraw or GrStringDrawCentered and should not call
//! this function directly.
//!
//! A string rendering function may call this low level API to place a single
//! character glyph on the display at a particular position.  The rendered
//! glyph is subject to the clipping rectangle currently set in the passed
//! graphics context.  Rendering colors are also taken from the context
//! structure.  Glyph data pointed to by \b pucData should be retrieved using
//! a call to GrFontGlyphDataGet().
//!
//! \return None.
//
//*****************************************************************************
void
GrFontGlyphRender(const tContext *pContext, const unsigned char *pucData,
                  long lX, long lY, unsigned long bCompressed,
                  unsigned long bOpaque)
{
    long lIdx, lX0, lY0, lCount, lOff, lOn, lBit, lClipX1, lClipX2;

    //
    // Check the arguments.
    //
    ASSERT(pContext);
    ASSERT(pucData);

    //
    // Stop drawing the string if the right edge of the clipping region has
    // been exceeded.
    //
    if(lX > pContext->sClipRegion.sXMax)
    {
        return;
    }

    //
    // See if the entire character is to the left of the clipping region.
    //
    if((lX + pucData[1]) < pContext->sClipRegion.sXMin)
    {
        return;
    }

    //
    // Loop through the bytes in the encoded data for this glyph.
    //
    for(lIdx = 2, lX0 = 0, lBit = 0, lY0 = 0; lIdx < pucData[0]; )
    {
        //
        // See if the bottom of the clipping region has been exceeded.
        //
        if((lY + lY0) > pContext->sClipRegion.sYMax)
        {
            //
            // Stop drawing this character.
            //
            break;
        }

        //
        // See if the font is uncompressed.
        //
        if(!bCompressed)
        {
            //
            // Count the number of off pixels from this position in the
            // glyph image.
            //
            for(lOff = 0; lIdx < pucData[0]; )
            {
                //
                // Get the number of zero pixels at this position.
                //
                lCount = NumLeadingZeros(pucData[lIdx] << (24 + lBit));

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
                    lIdx++;
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
            for(lOn = 0; lIdx < pucData[0]; )
            {
                //
                // Get the number of one pixels at this location (by
                // inverting the data and counting the number of zeros).
                //
                lCount = NumLeadingZeros(~(pucData[lIdx] << (24 + lBit)));

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
                    lIdx++;
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
        }

        //
        // Otherwise, the font is compressed with a pixel RLE scheme.
        //
        else
        {
            //
            // See if this is a byte that encodes some on and off pixels.
            //
            if(pucData[lIdx])
            {
                //
                // Extract the number of off pixels.
                //
                lOff = (pucData[lIdx] >> 4) & 15;

                //
                // Extract the number of on pixels.
                //
                lOn = pucData[lIdx] & 15;

                //
                // Skip past this encoded byte.
                //
                lIdx++;
            }

            //
            // Otherwise, see if this is a repeated on pixel byte.
            //
            else if(pucData[lIdx + 1] & 0x80)
            {
                //
                // There are no off pixels in this encoding.
                //
                lOff = 0;

                //
                // Extract the number of on pixels.
                //
                lOn = (pucData[lIdx + 1] & 0x7f) * 8;

                //
                // Skip past these two encoded bytes.
                //
                lIdx += 2;
            }

            //
            // Otherwise, this is a repeated off pixel byte.
            //
            else
            {
                //
                // Extract the number of off pixels.
                //
                lOff = pucData[lIdx + 1] * 8;

                //
                // There are no on pixels in this encoding.
                //
                lOn = 0;

                //
                // Skip past these two encoded bytes.
                //
                lIdx += 2;
            }
        }

        //
        // Loop while there are any off pixels.
        //
        while(lOff)
        {
            //
            // See if the bottom of the clipping region has been exceeded.
            //
            if((lY + lY0) > pContext->sClipRegion.sYMax)
            {
                //
                // Ignore the remainder of the on pixels.
                //
                break;
            }

            //
            // See if there is more than one on pixel that will fit onto
            // the current row.
            //
            if((lOff > 1) && ((lX0 + 1) < pucData[1]))
            {
                //
                // Determine the number of on pixels that will fit on this
                // row.
                //
                lCount = (((lX0 + lOff) > pucData[1]) ? pucData[1] - lX0 :
                          lOff);

                //
                // If this row is within the clipping region, draw a
                // horizontal line that corresponds to the sequence of on
                // pixels.
                //
                if(((lY + lY0) >= pContext->sClipRegion.sYMin) && bOpaque)
                {
                    if((lX + lX0) < pContext->sClipRegion.sXMin)
                    {
                        lClipX1 = pContext->sClipRegion.sXMin;
                    }
                    else
                    {
                        lClipX1 = lX + lX0;
                    }

                    if((lX + lX0 + lCount - 1) > pContext->sClipRegion.sXMax)
                    {
                        lClipX2 = pContext->sClipRegion.sXMax;
                    }
                    else
                    {
                        lClipX2 = lX + lX0 + lCount - 1;
                    }

                    DpyLineDrawH(pContext->pDisplay, lClipX1, lClipX2,
                                 lY + lY0, pContext->ulBackground);
                }

                //
                // Decrement the count of on pixels by the number on this
                // row.
                //
                lOff -= lCount;

                //
                // Increment the X offset by the number of on pixels.
                //
                lX0 += lCount;
            }

            //
            // Otherwise, there is only a single on pixel that can be
            // drawn.
            //
            else
            {
                //
                // If this pixel is within the clipping region, then draw
                // it.
                //
                if(((lX + lX0) >= pContext->sClipRegion.sXMin) &&
                   ((lX + lX0) <= pContext->sClipRegion.sXMax) &&
                   ((lY + lY0) >= pContext->sClipRegion.sYMin) && bOpaque)
                {
                    DpyPixelDraw(pContext->pDisplay, lX + lX0, lY + lY0,
                                 pContext->ulBackground);
                }

                //
                // Decrement the count of on pixels.
                //
                lOff--;

                //
                // Increment the X offset.
                //
                lX0++;
            }

            //
            // See if the X offset has reached the right side of the
            // character glyph.
            //
            if(lX0 == pucData[1])
            {
                //
                // Increment the Y offset.
                //
                lY0++;

                //
                // Reset the X offset to the left side of the character
                // glyph.
                //
                lX0 = 0;
            }
        }

        //
        // Loop while there are any on pixels.
        //
        while(lOn)
        {
            //
            // See if the bottom of the clipping region has been exceeded.
            //
            if((lY + lY0) > pContext->sClipRegion.sYMax)
            {
                //
                // Ignore the remainder of the on pixels.
                //
                break;
            }

            //
            // See if there is more than one on pixel that will fit onto
            // the current row.
            //
            if((lOn > 1) && ((lX0 + 1) < pucData[1]))
            {
                //
                // Determine the number of on pixels that will fit on this
                // row.
                //
                lCount = (((lX0 + lOn) > pucData[1]) ? pucData[1] - lX0 :
                          lOn);

                //
                // If this row is within the clipping region, draw a
                // horizontal line that corresponds to the sequence of on
                // pixels.
                //
                if((lY + lY0) >= pContext->sClipRegion.sYMin)
                {
                    if((lX + lX0) >= pContext->sClipRegion.sXMin)
                    {
                        lClipX1 = lX + lX0;
                    }
                    else
                    {
                        lClipX1 = pContext->sClipRegion.sXMin;
                    }

                    if((lX + lX0 + lCount - 1) > pContext->sClipRegion.sXMax)
                    {
                        lClipX2 = pContext->sClipRegion.sXMax;
                    }
                    else
                    {
                        lClipX2 = lX + lX0 + lCount - 1;
                    }

                    DpyLineDrawH(pContext->pDisplay, lClipX1, lClipX2,
                                 lY + lY0, pContext->ulForeground);
                }

                //
                // Decrement the count of on pixels by the number on this
                // row.
                //
                lOn -= lCount;

                //
                // Increment the X offset by the number of on pixels.
                //
                lX0 += lCount;
            }

            //
            // Otherwise, there is only a single on pixel that can be
            // drawn.
            //
            else
            {
                //
                // If this pixel is within the clipping region, then draw
                // it.
                //
                if(((lX + lX0) >= pContext->sClipRegion.sXMin) &&
                   ((lX + lX0) <= pContext->sClipRegion.sXMax) &&
                   ((lY + lY0) >= pContext->sClipRegion.sYMin))
                {
                    DpyPixelDraw(pContext->pDisplay, lX + lX0, lY + lY0,
                                 pContext->ulForeground);
                }

                //
                // Decrement the count of on pixels.
                //
                lOn--;

                //
                // Increment the X offset.
                //
                lX0++;
            }

            //
            // See if the X offset has reached the right side of the
            // character glyph.
            //
            if(lX0 == pucData[1])
            {
                //
                // Increment the Y offset.
                //
                lY0++;

                //
                // Reset the X offset to the left side of the character
                // glyph.
                //
                lX0 = 0;
            }
        }
    }
}

//*****************************************************************************
//
//! Retrieves header information from a font.
//!
//! \param pFont points to the font whose information is to be queried.
//! \param pucFormat points to storage which will be written with the font
//!        format.
//! \param pucMaxWidth points to storage which will be written with the
//!        maximum character width for the font in pixels.
//! \param pucHeight points to storage which will be written with the height
//!        of the font character cell in pixels.
//! \param pucBaseline points to storage which will be written with the font
//!        baseline offset in pixels.
//!
//! This function may be used to retrieve information about a given font.  The
//! \e pFont parameter may point to any supported font format including wrapped
//! fonts described using a \e tFontWrapper structure (with the pointer cast
//! to a tFont pointer).
//!
//! \return None.
//
//*****************************************************************************
void
GrFontInfoGet(const tFont *pFont, unsigned char *pucFormat,
              unsigned char *pucMaxWidth, unsigned char *pucHeight,
              unsigned char *pucBaseline)
{
    //
    // Parameter sanity checks.
    //
    ASSERT(pFont);
    ASSERT(pucFormat);
    ASSERT(pucMaxWidth);
    ASSERT(pucHeight);
    ASSERT(pucBaseline);

    //
    // Is this a wrapped font?
    //
    if(pFont->ucFormat & FONT_FMT_WRAPPED)
    {
        tFontWrapper *pFontWrapper;

        //
        // Yes - get a pointer to the relevant header type and call the
        // access function to retrieve the font information.
        //
        pFontWrapper = (tFontWrapper *)pFont;
        pFontWrapper->pFuncs->pfnFontInfoGet(pFontWrapper->pucFontId, pucFormat,
                                            pucMaxWidth, pucHeight,
                                            pucBaseline);
    }
    else
    {
        //
        // This is not a wrapped font so we can read the information directly
        // from the font structure passed.
        //
        *pucFormat = pFont->ucFormat;
        *pucMaxWidth = pFont->ucMaxWidth;
        *pucHeight = pFont->ucHeight;
        *pucBaseline = pFont->ucBaseline;
    }
}

//*****************************************************************************
//
//! Gets the baseline of a font.
//!
//! \param pFont is a pointer to the font to query.
//!
//! This function determines the baseline position of a font.  The baseline is
//! the offset between the top of the font and the bottom of the capital
//! letters.  The only font data that exists below the baseline are the
//! descenders on some lower-case letters (such as ``y'').
//!
//! \return Returns the baseline of the font, in pixels.
//
//*****************************************************************************
unsigned long
GrFontBaselineGet(const tFont *pFont)
{
    unsigned char ucFormat, ucWidth, ucHeight, ucBaseline;

    ASSERT(pFont);

    if(pFont->ucFormat != FONT_FMT_WRAPPED)
    {
        return((unsigned long)(pFont->ucBaseline));
    }
    else
    {
        tFontWrapper *pWrap;

        pWrap = (tFontWrapper *)pFont;
        pWrap->pFuncs->pfnFontInfoGet(pWrap->pucFontId, &ucFormat, &ucWidth,
                                      &ucHeight, &ucBaseline);
        return((unsigned long)ucBaseline);
    }

}

//*****************************************************************************
//
//! Gets the height of a font.
//!
//! \param pFont is a pointer to the font to query.
//!
//! This function determines the height of a font.  The height is the offset
//! between the top of the font and the bottom of the font, including any
//! ascenders and descenders.
//!
//! \return Returns the height of the font, in pixels.
//
//*****************************************************************************
unsigned long
GrFontHeightGet(const tFont *pFont)
{
    unsigned char ucFormat, ucWidth, ucHeight, ucBaseline;

    ASSERT(pFont);

    if(pFont->ucFormat != FONT_FMT_WRAPPED)
    {
        return(pFont->ucHeight);
    }
    else
    {
        tFontWrapper *pWrap;

        pWrap = (tFontWrapper *)pFont;
        pWrap->pFuncs->pfnFontInfoGet(pWrap->pucFontId, &ucFormat, &ucWidth,
                                      &ucHeight, &ucBaseline);
        return((unsigned long)ucHeight);
    }
}

//*****************************************************************************
//
//! Gets the maximum width of a font.
//!
//! \param pFont is a pointer to the font to query.
//!
//! This function determines the maximum width of a font.  The maximum width is
//! the width of the widest individual character in the font.
//!
//! \return Returns the maximum width of the font, in pixels.
//
//*****************************************************************************
unsigned long
GrFontMaxWidthGet(const tFont *pFont)
{
    unsigned char ucFormat, ucWidth, ucHeight, ucBaseline;

    ASSERT(pFont);

    if(pFont->ucFormat != FONT_FMT_WRAPPED)
    {
        return(pFont->ucMaxWidth);
    }
    else
    {
        tFontWrapper *pWrap;

        pWrap = (tFontWrapper *)pFont;
        pWrap->pFuncs->pfnFontInfoGet(pWrap->pucFontId, &ucFormat, &ucWidth,
                                      &ucHeight, &ucBaseline);
        return((unsigned long)ucWidth);
    }
}

//*****************************************************************************
//
// Retrieves a pointer to the data for a specific glyph in a tFont or tFontEx
// font.
//
// \param pFont points to the font whose glyph is to be queried.
// \param ulCodePoint idenfities the specific glyph whose data is being
//        queried.
// \param pucWidth points to storage which will be written with the
//        width of the requested glyph in pixels.
//
// This function may be used to retrieve the pixel data for a particular glyph
// in a font described using a tFont or tFontEx type.
//
// \return Returns a pointer to the data for the requested glyph or NULL if
// the glyph does not exist in the font.
//
//*****************************************************************************
static const unsigned char *
FontGlyphDataGet(const tFont *pFont, unsigned long ulCodePoint,
                 unsigned char *pucWidth)
{
    const unsigned char *pucGlyphs;
    const unsigned short *pusOffset;
    unsigned char ucFirst, ucLast;
    const tFontEx *pFontEx;
    const unsigned char *pucData;

    //
    // Extract various parameters from the font depending upon whether it's
    // in the tFont or tFontEx format.
    //
    if(pFont->ucFormat & FONT_EX_MARKER)
    {
        pFontEx = (const tFontEx *)pFont;

        pucGlyphs = pFontEx->pucData;
        pusOffset = pFontEx->pusOffset;
        ucFirst = pFontEx->ucFirst;
        ucLast = pFontEx->ucLast;
    }
    else
    {
        pucGlyphs = pFont->pucData;
        pusOffset = pFont->pusOffset;
        ucFirst = 32;
        ucLast = 126;
    }

    //
    // Does the codepoint passed exist in the font?
    //
    if((ulCodePoint >= ucFirst) && (ulCodePoint <= ucLast))
    {
        //
        // Yes - return a pointer to the glyph data for the character.
        //
        pucData = pucGlyphs + pusOffset[ulCodePoint - ucFirst];
        *pucWidth = pucData[1];
        return(pucData);
    }
    else
    {
        //
        // No - the glyph doesn't exist so return NULL to indicate this.
        //
        return((unsigned char)0);
    }
}

//*****************************************************************************
//
// Retrieves a pointer to the data for a specific glypn in a tFontWide font.
//
// \param pFont points to the font whose glyph is to be queried.
// \param ulCodePoint idenfities the specific glyph whose data is being
//        queried.
// \param pucWidth points to storage which will be written with the
//        width of the requested glyph in pixels.
//
// This function may be used to retrieve the pixel data for a particular glyph
// in a font described using a tFontWide type.
//
// \return Returns a pointer to the data for the requested glyph or NULL if
// the glyph does not exist in the font.
//
//*****************************************************************************
static const unsigned char *
FontWideGlyphDataGet(const tFontWide *pFont, unsigned long ulCodePoint,
                     unsigned char *pucWidth)
{
    const unsigned char *pucData;
    tFontBlock *pBlock;
    unsigned long *pulOffsetTable;
    unsigned long ulLoop;
    unsigned long ulOffset;

    //
    // Get a pointer to the first block description in the font.
    //
    pBlock = (tFontBlock *)(pFont + 1);

    //
    // Run through the blocks of the font looking for the one that contains
    // our codepoint.
    //
    for(ulLoop = 0; ulLoop < pFont->usNumBlocks; ulLoop++)
    {
        //
        // Does the codepoint lie within this block?
        //
        if((ulCodePoint >= pBlock[ulLoop].ulStartCodepoint) &&
           (ulCodePoint < (pBlock[ulLoop].ulStartCodepoint +
                           pBlock[ulLoop].ulNumCodepoints)))
        {
            //
            // Yes - drop out of the loop early.
            //
            break;
        }
    }

    //
    // Did we find the block?
    //
    if(ulLoop == pFont->usNumBlocks)
    {
        //
        // No - return NULL to indicate that the character wasn't found.
        //
        return(0);
    }

    //
    // Get the offset to the glyph data via the block's offset table.
    //
    pulOffsetTable = (unsigned long *)((unsigned char *)pFont +
            pBlock[ulLoop].ulGlyphTableOffset);
    ulOffset = pulOffsetTable[ulCodePoint - pBlock[ulLoop].ulStartCodepoint];

    //
    // Is the offset non-zero? Zero offset indicates that the glyph is not
    // encoded in the font.
    //
    if(ulOffset)
    {
        //
        // The offset is not 0 so this glyph does exist. Return a pointer to
        // its data.
        //
        pucData = (const unsigned char *)pulOffsetTable + ulOffset;
        *pucWidth = pucData[1];
        return(pucData);
    }
    else
    {
        //
        // The glyph offset was 0 so this implies that the glyph does not
        // exist.  Return NULL to indicate this.
        //
        return(0);
    }
}

//*****************************************************************************
//
//! Retrieves a pointer to the data for a specific font glyph.
//!
//! \param pFont points to the font whose glyph is to be queried.
//! \param ulCodePoint idenfities the specific glyph whose data is being
//!        queried.
//! \param pucWidth points to storage which will be written with the
//!        width of the requested glyph in pixels.
//!
//! This function may be used to retrieve the pixel data for a particular glyph
//! in a font.  The pointer returned may be passed to GrFontGlyphRender to
//! draw the glyph on the display.  The format of the data may be determined
//! from the font format returned via a call to GrFontInfoGet().
//!
//! \return Returns a pointer to the data for the requested glyph or NULL if
//! the glyph does not exist in the font.
//
//*****************************************************************************
const unsigned char *
GrFontGlyphDataGet(const tFont *pFont, unsigned long ulCodePoint,
                   unsigned char *pucWidth)
{
    ASSERT(pFont);
    ASSERT(pucWidth);

    //
    // What type of font are we dealing with here?
    //
    if(pFont->ucFormat == FONT_FMT_WRAPPED)
    {
        tFontWrapper *pFontWrapper;

        //
        // This is a wrapped font so call the access function required to get
        // the required information.
        //
        pFontWrapper = (tFontWrapper *)pFont;
        return(pFontWrapper->pFuncs->pfnFontGlyphDataGet(pFontWrapper->pucFontId,
                                                 ulCodePoint, pucWidth));
    }
    else if (pFont->ucFormat & FONT_WIDE_MARKER)
    {
        //
        // This is a wide character set font so call the relevant function to
        // retrieve the glyph data pointer.
        //
        return(FontWideGlyphDataGet((const tFontWide *)pFont, ulCodePoint,
                                    pucWidth));
    }
    else
    {
        //
        // This is an 8 bit font so call the relevant function to retrieve the
        // glyph data pointer.
        //
        return(FontGlyphDataGet(pFont, ulCodePoint, pucWidth));
    }
}

//*****************************************************************************
//
//! Returns the codepage supported by the given font.
//!
//! \param pFont points to the font whose codepage is to be returned.
//!
//! This function returns the codepage supported by the font whose pointer is
//! passed.  The codepage defines the mapping between a given character code
//! and the glyph that represents it.  Standard codepages are identified by
//! labels of the form \b CODEPAGE_xxxx.  Fonts may also be encoded using
//! application specific codepages with values of 0x8000 or higher.
//!
//! \return Returns the font codepage identifier.
//!
//*****************************************************************************
unsigned short
GrFontCodepageGet(const tFont *pFont)
{
    ASSERT(pFont);

    //
    // Is this a wide character set font?
    //
    if(pFont->ucFormat & FONT_WIDE_MARKER)
    {
        //
        // Yes - read the font codepage from the header.
        //
        return(((tFontWide *)pFont)->usCodepage);
    }
    else if(pFont->ucFormat & FONT_FMT_WRAPPED)
    {
        //
        // This is a wrapper-based font so call the access function to get
        // its codepage.
        //
        ASSERT(((tFontWrapper *)pFont)->pFuncs->pfnFontCodepageGet);

        return(((tFontWrapper *)pFont)->pFuncs->pfnFontCodepageGet(
                    ((tFontWrapper *)pFont)->pucFontId));
    }
    else
    {
        //
        // No - this is an old format font so just return ISO8859-1.  This is
        // compatible with ASCII so should be benign.
        //
        return(CODEPAGE_ISO8859_1);
    }
}

//*****************************************************************************
//
// Determines which codepoint mapping function to use based on the current
// source codepage and font selection in the context.
//
//*****************************************************************************
static long
UpdateContextCharMapping(tContext *pContext)
{
    unsigned long ulLoop;
    unsigned short usFontCodepage;

    //
    // Make sure we have a font selected.
    //
    if(!pContext->pFont)
    {
        //
        // No font is yet selected so we can't determine the codepage map to
        // use.
        //
        return(-1);
    }

    //
    // Get the current font's codepage.
    //
    usFontCodepage = GrFontCodepageGet(pContext->pFont);

    //
    // Look through the codepage mapping functions we have been given and
    // find an appropriate one.
    //
    for(ulLoop = 0; ulLoop < pContext->ucNumCodePointMaps; ulLoop++)
    {
        if((pContext->pCodePointMapTable[ulLoop].usSrcCodepage ==
            pContext->usCodepage) &&
           (pContext->pCodePointMapTable[ulLoop].usFontCodepage ==
            usFontCodepage))
        {
            //
            // We found a suitable mapping function so remember it.
            //
            pContext->ucCodePointMap = (unsigned char)ulLoop;
            return((long)ulLoop);
        }
    }

    //
    // If we get here, no suitable mapping function could be found.  Set things
    // up to use the first mapping function (even though it's not right).
    //
    pContext->ucCodePointMap = 0;
    return(-1);
}
//*****************************************************************************
//
//! Returns the number of blocks of character encoded by a font.
//!
//! \param pFont is a pointer to the font which is to be queried.
//!
//! This function may be used to query the number of contiguous blocks of
//! codepoints (characters) encoded by a given font.  This is primarily of use
//! to applications which wish to parse fonts directly to, for example, display
//! all glyphs in the font.  It is unlikely that applications which wish to
//! display text strings would need to call this function.
//!
//! The \e pFont parameter may point to any supported font format including
//! wrapped fonts described using the \e tFontWrapper structure (assuming, of
//! course, that the structure pointer is cast to a \e tFont pointer).
//!
//! \return Returns the number of blocks of codepoints within the font.
//
//*****************************************************************************
unsigned short
GrFontNumBlocksGet(const tFont *pFont)
{
    ASSERT(pFont);

    //
    // Is this a wide character set font?
    //
    if(pFont->ucFormat & FONT_WIDE_MARKER)
    {
        //
        // Yes - read the number of blocks from the header.
        //
        return(((tFontWide *)pFont)->usNumBlocks);
    }
    else if(pFont->ucFormat & FONT_FMT_WRAPPED)
    {
        //
        // This is a wrapper-based font so call the access function to get
        // the information.
        //
        ASSERT(((tFontWrapper *)pFont)->pFuncs->pfnFontNumBlocksGet);

        return(((tFontWrapper *)pFont)->pFuncs->pfnFontNumBlocksGet(
                    ((tFontWrapper *)pFont)->pucFontId));
    }
    else
    {
        //
        // No - this is an old format font so it only supports a single block
        // of characters.
        //
        return(1);
    }
}

unsigned long
GrFontBlockCodepointsGet(const tFont *pFont, unsigned short usBlockIndex,
                         unsigned long *pulStart)
{
    ASSERT(pFont);
    ASSERT(pulStart);

    //
    // Is this a wide character set font?
    //
    if(pFont->ucFormat & FONT_WIDE_MARKER)
    {
        tFontWide *pFontWide;
        tFontBlock *pBlock;

        //
        // This is a wide character set font.  Is the block index valid?
        //
        pFontWide = (tFontWide *)pFont;
        if(usBlockIndex >= pFontWide->usNumBlocks)
        {
            //
            // The block number is invalid so return 0 to indicate the error.
            //
               return(0);
        }

        //
        // Yes - find the relevant block table and extract the required
        // information from it.
        //
        pBlock = (tFontBlock *)(pFontWide + 1);
        *pulStart = pBlock[usBlockIndex].ulStartCodepoint;
        return(pBlock[usBlockIndex].ulNumCodepoints);
    }
    else if(pFont->ucFormat & FONT_FMT_WRAPPED)
    {
        //
        // This is a wrapper-based font so call the access function to get
        // its codepage.
        //
        ASSERT(((tFontWrapper *)pFont)->pFuncs->pfnFontBlockCodepointsGet);

        return(((tFontWrapper *)pFont)->pFuncs->pfnFontBlockCodepointsGet(
                    ((tFontWrapper *)pFont)->pucFontId, usBlockIndex,
                    pulStart));
    }
    else
    {
        //
        // No - this is an old format font so it only supports a single block
        // of
        //
        if(usBlockIndex != 0)
        {
            //
            // An invalid block number was passed so return 0 to indicate an
            // error.
            //
            return(0);
        }

        //
        // We were passed a valid block number (0) so return the start
        // codepoint and number of characters.  Is this an extended font or
        // the original ASCII-only flavor?
        //
         if(pFont->ucFormat & FONT_EX_MARKER)
         {
             tFontEx *pFontEx;

             //
             // It's an extended font so read the character range from the
             // header.
             //
             pFontEx = (tFontEx *)pFont;

             *pulStart = (unsigned long)pFontEx->ucFirst;
             return((unsigned long)(pFontEx->ucLast - pFontEx->ucFirst + 1));
         }
         else
         {
             //
             // This is an ASCII font so it supports a fixed set of characters.
             //
             *pulStart = 0x20;
             return(96);
         }
    }
}

//*****************************************************************************
//
//! Provides GrLib with a table of source/font codepage mapping functions.
//!
//! \param pContext is a pointer to the context to modify.
//! \param pCodePointMapTable points to an array of structures each defining
//!   the mapping from a source text codepage to a destination font codepage.
//! \param ucNumMaps provides the number of entries in the \e pCodePointMapTable
//!   array.
//!
//! This function provides GrLib with a set of functions that can be used to
//! map text encoded in a particular codepage to one other codepage.  These
//! functions are used to allow GrLib to parse text strings and display the
//! correct glyphs from the font.  The mapping function used by the library
//! will depend upon the source text codepage set using a call to
//! GrStringCodepageSet() and the context's font, set using GrContextFontSet().
//!
//! If no conversion function is available to map from the selected source
//! codepage to the font's codepage, GrLib use the first conversion function
//! provided in the codepoint map table and the displayed text will likely be
//! incorrect.
//!
//! If this call is not made, GrLib assumes ISO8859-1 encoding for both the
//! source text and font to maintain backwards compatibility for applications
//! which were developed prior to the introduction of international character
//! set support.
//!
//! \return None.
//
//*****************************************************************************
void
GrCodepageMapTableSet(tContext *pContext,
                      tCodePointMap *pCodePointMapTable,
                      unsigned char ucNumMaps)
{
    ASSERT(pContext);
    ASSERT(pCodePointMapTable);
    ASSERT(ucNumMaps);

    //
    // Remember the table details.
    //
    pContext->pCodePointMapTable = pCodePointMapTable;
    pContext->ucNumCodePointMaps = ucNumMaps;

    //
    // Update the character mapping to ensure that we show the right glyphs.
    //
    UpdateContextCharMapping(pContext);
}

//*****************************************************************************
//
//! Sets the source text codepage to be used.
//!
//! \param pContext is a pointer to the context to modify.
//! \param usCodepage is the identifier of the codepage for the text that the
//!        application will pass on future calls to the GrStringDraw() and
//!        GrStringDrawCentered() functions.
//!
//! This function sets the codepage that will be used when rendering text
//! via future calls to GrStringDraw() or GrStringDrawCentered().  The codepage
//! defines the mapping between specific numbers used to define characters and
//! the actual character glyphs displayed.  By default, GrLib assumes text
//! passed is encoded using the ISO8859-1 which supports ASCII and western
//! European character sets.  Applications wishing to use multi-byte character
//! sets or alphabets other than those supported by ISO8859-1 should set an
//! appropriate codepage such as UTF-8.
//!
//! It is important to ensure that your application makes use of fonts which
//! support the required codepage or that you have supplied GrLib with a
//! codepage mapping function that allows translation of your chosen text
//! codepage into the codepage supported by the fonts in use.  Several
//! mapping functions for commonly-used codepages are provided and others can
//! be written easily to support different text and font codepage combinations.
//! Codepage mapping functions are provided to GrLib in a table passed as a
//! parameter to the function GrCodepageMapTableSet().
//!
//! \return None.
//
//*****************************************************************************
long
GrStringCodepageSet(tContext *pContext, unsigned short usCodepage)
{
    ASSERT(pContext);

    //
    // Remember the codepage to be used.
    //
    pContext->usCodepage = usCodepage;

    //
    // Update the character mapping to ensure that we show the right glyphs.
    //
    return(UpdateContextCharMapping(pContext));
}

//*****************************************************************************
//
//! Sets the font to be used.
//!
//! \param pContext is a pointer to the drawing context to modify.
//! \param pFont is a pointer to the font to be used.
//!
//! This function sets the font to be used for string drawing operations in the
//! specified drawing context.
//!
//! \return None.
//
//*****************************************************************************
void
GrContextFontSet(tContext *pContext, const tFont *pFont)
{
    ASSERT(pContext);
    ASSERT(pFont);

    //
    // Remember the font to be used.
    //
    pContext->pFont = pFont;

    //
    // Update the character mapping to ensure that we show the right glyphs.
    //
    UpdateContextCharMapping(pContext);
}
#endif

//*****************************************************************************
//
// Definitions and variables used by the decompression routine for the string
// table.
//
//*****************************************************************************
#define SC_MAX_INDEX            2047
#define SC_IS_NULL              0x0000ffff
#define SC_GET_LEN(v)           ((v) >> (32 - 5))
#define SC_GET_INDEX(v)         (((v) >> 16) & SC_MAX_INDEX)
#define SC_GET_OFF(v)           ((v) & SC_IS_NULL)

#define SC_FLAG_COMPRESSED      0x00008000
#define SC_OFFSET_M             0x00007fff

//*****************************************************************************
//
// The globals that hold the shortcuts to various locations and values in the
// table.
//
//*****************************************************************************
static const unsigned long *g_pulStringTable;
static const unsigned short *g_pusLanguageTable;
static const unsigned char *g_pucStringData;

static unsigned short g_usLanguage;
static unsigned short g_usNumLanguages;
static unsigned short g_usNumStrings;

//*****************************************************************************
//
//! This function sets the location of the current string table.
//!
//! \param pvTable is a pointer to a string table that was generated by the
//! string compression utility.
//!
//! This function is used to set the string table to use for strings in an
//! application.  This string table is created by the string compression
//! utility.  This function is used to swap out multiple string tables if the
//! application requires more than one table.  It does not allow using more
//! than one string table at a time.
//!
//! \return None.
//
//*****************************************************************************
void
GrStringTableSet(const void *pvTable)
{
    //
    // Save the number of languages and number of strings.
    //
    g_usNumStrings = ((unsigned short *)pvTable)[0];
    g_usNumLanguages = ((unsigned short *)pvTable)[1];

    //
    // Save a pointer to the Language Identifier table.
    //
    g_pusLanguageTable = (unsigned short *)pvTable + 2;

    //
    // Save a pointer to the String Index table.
    //
    g_pulStringTable = (unsigned long *)(g_pusLanguageTable +
                                         g_usNumLanguages);

    //
    // Save a pointer to the String Data.
    //
    g_pucStringData = (unsigned char *)(g_pulStringTable +
                                        (g_usNumStrings * g_usNumLanguages));
}

//*****************************************************************************
//
//! This function sets the current language for strings returned by the
//! GrStringGet() function.
//!
//! \param usLangID is one of the language identifiers provided in the string
//! table.
//!
//! This function is used to set the language identifier for the strings
//! returned by the GrStringGet() function.  The \e usLangID parameter should
//! match one of the identifiers that was included in the string table.  These
//! are provided in a header file in the graphics library and must match the
//! values that were passed through the sting compression utility.
//!
//! \return This function returns 0 if the language was not found and a
//! non-zero value if the laguage was found.
//
//*****************************************************************************
unsigned long
GrStringLanguageSet(unsigned short usLangID)
{
    long lLang;

    //
    // Search for the requested language.
    //
    for(lLang = 0; lLang < g_usNumLanguages; lLang++)
    {
        //
        // Once found, break out and save the new language.
        //
        if(g_pusLanguageTable[lLang] == usLangID)
        {
            break;
        }
    }

    //
    // Only accept the language if it was found, otherwise continue using
    // previous language.
    //
    if(lLang != g_usNumLanguages)
    {
        g_usLanguage = lLang;
        return(1);
    }

    return(0);
}

//*****************************************************************************
//
//! This function returns a string from the current string table.
//!
//! \param lIndex is the index of the string to retrieve.
//! \param pcData is the pointer to the buffer to store the string into.
//! \param ulSize is the size of the buffer provided by pcData.
//!
//! This function will return a string from the string table in the language
//! set by the GrStringLanguageSet() function.  The value passed in \e iIndex
//! parameter is the string that is being requested and will be returned in
//! the buffer provided in the \e pcData parameter.  The amount of data
//! returned will be limited by the ulSize parameter.
//!
//! \return Returns the number of valid bytes returned in the \e pcData buffer.
//
//*****************************************************************************
unsigned long
GrStringGet(long lIndex, char *pcData, unsigned long ulSize)
{
    unsigned long ulLen, ulOffset, ulSubCode[16];
    long lPos, lIdx, lBit, lSkip, lBuf;
    unsigned char *pucBufferOut;
    const unsigned char *pucString;

    ASSERT(lIndex < g_usNumStrings);
    ASSERT(pcData != 0);

    //
    // Initialize the output buffer state.
    //
    lPos = 0;
    pucBufferOut = 0;

    //
    // if built up from another string, we need to process that
    // this could nest multiple layers, so we follow in
    //
    ulSubCode[lPos] = g_pulStringTable[(g_usLanguage * g_usNumStrings) +
                                       lIndex];

    if(SC_GET_LEN(ulSubCode[lPos]))
    {
        //
        // recurse down
        //
        while(lPos < 16)
        {
            //
            // Copy over the partial (if any) from a previous string.
            //
            lIdx = SC_GET_INDEX(ulSubCode[lPos++]);

            ulSubCode[lPos] = g_pulStringTable[(g_usLanguage *
                                                g_usNumStrings) + lIdx];

            if(!SC_GET_LEN(ulSubCode[lPos]))
            {
                //
                // not linked, just string
                //
                break;
            }
        }
    }

    //
    // Now work backwards out.
    //
    lIdx = 0;

    //
    // Build up the string in pieces.
    //
    while(lPos >= 0)
    {
        //
        // Get the offset in string table.
        //
        ulOffset = SC_GET_OFF(ulSubCode[lPos]);

        if(ulOffset == SC_IS_NULL)
        {
            //
            // An empty string.
            //
            pcData[lIdx] = 0;
        }
        else if(ulOffset & SC_FLAG_COMPRESSED)
        {
            //
            // This is a compressed string so initialize the pointer to the
            // compressed data.
            //
            pucString = g_pucStringData + (ulOffset & SC_OFFSET_M);

            //
            // Initialize the bit variables.
            //
            lBit = 0;
            lSkip = 0;

            //
            // Make a pointer to the current buffer out location.
            //
            pucBufferOut = (unsigned char *)pcData + lIdx;

            //
            // If the out buffer is beyond the maximum size then just break
            // out and return what we have so far.
            //
            if((char *)pucBufferOut > (pcData + ulSize))
            {
                break;
            }

            //
            // Now build up real string by decompressing bits.
            //
            if(!SC_GET_LEN(ulSubCode[lPos]) && SC_GET_INDEX(ulSubCode[lPos]))
            {
                lSkip = SC_GET_INDEX(ulSubCode[lPos]);

                if(lPos)
                {
                    ulLen = SC_GET_LEN(ulSubCode[lPos - 1]);
                }
                else
                {
                    ulLen = (lSkip & 0x3f);
                }

                lSkip >>= 6;
                lIdx += ulLen;
                ulLen += lSkip;
            }
            else if(lPos)
            {
                //
                // Get the length of the partial string.
                //
                ulLen = SC_GET_LEN(ulSubCode[lPos - 1]) - lIdx;
                lIdx += ulLen;
            }
            else if(!SC_GET_LEN(ulSubCode[0]) && SC_GET_INDEX(ulSubCode[0]))
            {
                ulLen = SC_GET_INDEX(ulSubCode[0]);
                lSkip = ulLen >> 6;
                ulLen = (ulLen & 0x3f) + lSkip;
            }
            else
            {
                //
                // Arbitrary as null character ends the string.
                //
                ulLen = 1024;
            }

            for(; ulLen; ulLen--)
            {
                //
                // Packed 6 bits for each char
                //
                *pucBufferOut = (*pucString >> lBit) & 0x3f;

                if(lBit >= 2)
                {
                    *pucBufferOut |= (*++pucString << (8 - lBit)) & 0x3f;
                }

                lBit = (lBit + 6) & 0x7;

                if(!*pucBufferOut)
                {
                    //
                    // end of string
                    //
                    break;
                }

                if(lSkip)
                {
                    lSkip--;
                    continue;
                }

                //
                // Put back removed bit
                //
                *pucBufferOut |= 0x40;

                //
                // Now look for a few special chars we mapped up into other
                // characters.
                //
                if(*pucBufferOut == '`')
                {
                    *pucBufferOut = ' ';
                }
                else if(*pucBufferOut == '~')
                {
                    *pucBufferOut = '-';
                }
                else if(*pucBufferOut == 0x7f)
                {
                    *pucBufferOut = '.';
                }
                else if(*pucBufferOut == '\\')
                {
                    *pucBufferOut = ':';
                }

                //
                // Increment the pointer and break out if the pointer is now
                // beyond the end of the buffer provided.
                //
                pucBufferOut++;

                if((char *)pucBufferOut >= (pcData + ulSize))
                {
                    break;
                }
            }
        }
        else if(lPos)
        {
            //
            // Part of another string
            //
            ulLen = SC_GET_LEN(ulSubCode[lPos - 1]) - lIdx;

            //
            // Prevent this copy from going beyond the end of the buffer
            // provided.
            //
            if((lIdx + ulLen) > ulSize)
            {
                ulLen = ulSize - lIdx;
            }

            //
            // Copy this portion of the string to the output buffer.
            //
            for(lBuf = 0; lBuf < ulLen; lBuf++)
            {
                pcData[lIdx + lBuf] = g_pucStringData[ulOffset + lBuf];
            }

            lIdx += ulLen;
        }
        else if(SC_GET_INDEX(ulSubCode[0]) && !SC_GET_LEN(ulSubCode[0]))
        {
            //
            // Copy this portion of the string to the output buffer.
            //
            for(lBuf = 0; lBuf < SC_GET_INDEX(ulSubCode[0]); lBuf++)
            {
                if((lIdx + lBuf) < ulSize)
                {
                    pcData[lIdx + lBuf] = g_pucStringData[ulOffset + lBuf];
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            //
            // Now copy the last piece of the string.
            //
            for(lBuf = 0; lBuf < (ulSize - lIdx); lBuf++)
            {
                //
                // Copy the string to the output buffer.
                //
                pcData[lIdx + lBuf] = g_pucStringData[ulOffset + lBuf];

                //
                // If a null is hit then terminate the copy.
                //
                if(pcData[lIdx + lBuf] == 0)
                {
                    break;
                }
            }

            //
            // If we had not copied any characters before hitting this case,
            // initialize the output pointer (this keeps the code at the end of
            // the function that returns the length happy).  This will be the
            // case if we are using an uncompressed string table.
            //
            if(!pucBufferOut)
            {
                pucBufferOut = (unsigned char *)pcData + (lIdx + lBuf);
            }
        }
        lPos--;
    }

    //
    // Return the number of bytes copied into the output buffer.
    //
    if(pucBufferOut)
    {
        ulLen = ((unsigned long)pucBufferOut - (unsigned long)pcData);

        //
        // Null terminate the string if there is room.
        //
        if(ulLen < ulSize)
        {
            pcData[ulLen] = 0;
        }
    }
    else
    {
        ulLen = 0;
    }

    return(ulLen);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
