//*****************************************************************************
//
// slider.c - A simple slider widget class.
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
#include "grlib/widget.h"
#include "grlib/slider.h"

//*****************************************************************************
//
//! \addtogroup slider_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Make sure min and max are defined.
//
//*****************************************************************************
#ifndef min
#define min(a, b)               (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a, b)               (((a) < (b)) ? (b) : (a))
#endif

//*****************************************************************************
//
// Converts a slider value to a position on the display.
//
// \param pSlider is a pointer to the slider widget for which the conversion is
// being requested.
// \param lValue is the slider value that is to be converted to a position.
//
// Converts a value within the range represented by a slider into a position
// along the slider control.  The function converts taking into account the
// slider position and style as well as its range.
//
// \return Returns the screen position (x coordinate for horizontal sliders or
// y coordinate for vertical ones) that represents the value passed.
//
//*****************************************************************************
static short
SliderValueToPosition(tSliderWidget *pSlider, long lValue)
{
    unsigned short usSize;
    long lRange;
    long lPos;

    //
    // First look for the trivial cases.  To ensure correct display and remove
    // artifacts caused by rounding errors, we specifically catch the cases
    // where the value provided is at either end of the slider range.  In these
    // cases we return values that are outside the actual widget rectangle.
    // This is detected while drawing so that the relevant bars fill the full
    // area and don't leave single pixel lines at either end even when the
    // slider is at full scale.  These cases also catch out-of-range values
    // and peg them at one end of the range or the other.
    //
    // First check for values at the top of the range.
    //
    if(lValue >= pSlider->lMax)
    {
        //
        // Is this a vertical slider?
        //
        if(pSlider->ulStyle & SL_STYLE_VERTICAL)
        {
            //
            // Vertical slider case.  Return the top position.
            //
            lPos = pSlider->sBase.sPosition.sYMin - 1;

            //
            // Adjust by 1 to move past the border if this widget has one.
            //
            if(pSlider->ulStyle & SL_STYLE_OUTLINE)
            {
                lPos++;
            }
        }
        else
        {
            //
            // Horizontal slider case.  Return the rightmost position.
            //
            lPos = pSlider->sBase.sPosition.sXMax + 1;

            //
            // Adjust by 1 to move past the border if this widget has one.
            //
            if(pSlider->ulStyle & SL_STYLE_OUTLINE)
            {
                lPos--;
            }
        }
        return((short)lPos);
    }

    //
    // Now look at the bottom end of the range.
    //
    if(lValue <= pSlider->lMin)
    {
        //
        // Is this a vertical slider?
        //
        if(pSlider->ulStyle & SL_STYLE_VERTICAL)
        {
            //
            // Vertical slider case.  Return the bottom position.
            //
            lPos = pSlider->sBase.sPosition.sYMax + 1;

            //
            // Adjust by 1 to move past the border if this widget has one.
            //
            if(pSlider->ulStyle & SL_STYLE_OUTLINE)
            {
                lPos--;
            }
        }
        else
        {
            //
            // Horizontal slider case.  Return the leftmost position.
            //
            lPos = pSlider->sBase.sPosition.sXMin - 1;

            //
            // Adjust by 1 to move past the border if this widget has one.
            //
            if(pSlider->ulStyle & SL_STYLE_OUTLINE)
            {
                lPos++;
            }
        }
        return((short)lPos);
    }

    //
    // What is the length of the whole slider?
    //
    if(pSlider->ulStyle & SL_STYLE_VERTICAL)
    {
        //
        // Vertical slider case.
        //
        usSize = (pSlider->sBase.sPosition.sYMax -
                  pSlider->sBase.sPosition.sYMin) + 1;
    }
    else
    {
        //
        // Horizontal slider case.
        //
        usSize = (pSlider->sBase.sPosition.sXMax -
                  pSlider->sBase.sPosition.sXMin) + 1;
    }

    //
    // Adjust the range if the slider has an outline (which removes 2 pixels).
    //
    if(pSlider->ulStyle & SL_STYLE_OUTLINE)
    {
        usSize -= 2;
    }

    //
    // Determine the range of the slider (the number of individual integers
    // represented by the widget).
    //
    lRange = (pSlider->lMax - pSlider->lMin) + 1;

    //
    // Now we can determine the relevant position relative to the start of the
    // slider.
    //
    lPos = (((lValue - pSlider->lMin) * (long)usSize) / lRange);

    //
    // Clip the calculated position to the valid range based on the slider
    // size.
    //
    lPos = max(lPos, 0);
    lPos = min(lPos, (long)usSize - 1);

    //
    // Adjust for the position of the widget relative to the screen origin.
    //
    if(pSlider->ulStyle & SL_STYLE_VERTICAL)
    {
        //
        // Vertical case - adjust the Y coordinate.
        //
        lPos = pSlider->sBase.sPosition.sYMax - lPos;
    }
    else
    {
        //
        // Horizontal case - adjust the X coordinate.
        //
        lPos += pSlider->sBase.sPosition.sXMin;
    }

    //
    // If the widget has an outline, make sure to adjust for this too.
    //
    lPos += ((pSlider->ulStyle & SL_STYLE_OUTLINE) ? 1 : 0);

    //
    // Convert to the expected return type and hand the caller the final
    // value.
    //
    return((short)lPos);
}

//*****************************************************************************
//
// Converts a slider position to a value within its range.
//
// \param pSlider is a pointer to the slider widget for which the conversion is
// being requested.
// \param usPos is a position within the slider.  This is an x coordinate for
// a horizontal slider or a y coordinate for a vertical one.  In both cases,
// the position is relative to the display origin.
//
// Converts a screen position into a value within the current range of the
// slider.  The function converts taking into account the slider position and
// style as well as its range.
//
// \return Returns the slider value represented by the position passed.
//
//*****************************************************************************
static long
SliderPositionToValue(tSliderWidget *pSlider, short sPos)
{
    short sMax;
    short sMin;
    long lValue;

    //
    // Determine the bounds of the control on the display.
    //
    if(pSlider->ulStyle & SL_STYLE_VERTICAL)
    {
        sMax = pSlider->sBase.sPosition.sYMax;
        sMin = pSlider->sBase.sPosition.sYMin;
    }
    else
    {
        sMax = pSlider->sBase.sPosition.sXMax;
        sMin = pSlider->sBase.sPosition.sXMin;
    }

    //
    // Adjust for the outline if present.
    //
    if(pSlider->ulStyle & SL_STYLE_OUTLINE)
    {
        sMax--;
        sMin--;
    }

    //
    // If the control is too narrow, this is a bug but handle it gracefully
    // rather than throwing a divide by zero later.
    //
    ASSERT(sMax > sMin);
    if(sMax <= sMin)
    {
        return(pSlider->lMin);
    }

    //
    // Clip the supplied position to the extent of the widget.
    //
    sPos = min(sMax, sPos);
    sPos = max(sMin, sPos);

    //
    // Adjust the position to make it relative to the start of the slider.
    //
    if(pSlider->ulStyle & SL_STYLE_VERTICAL)
    {
        sPos = sMax - sPos;
    }

    else
    {
        sPos -= sMin;
    }

    //
    // Calculate the value represented by this position.
    //
    lValue = ((long)sPos * ((pSlider->lMax - pSlider->lMin) + 1)) /
             (long)((sMax - sMin) + 1);

    //
    // Adjust for the bottom of the value range.
    //
    lValue += pSlider->lMin;

    //
    // Hand the conversion result back to the caller.
    //
    return(lValue);
}

//*****************************************************************************
//
//! Draws a slider.
//!
//! \param pWidget is a pointer to the slider widget to be drawn.
//! \param pDirty is the subrectangle of the widget which is to be redrawn.
//! This is expressed in screen coordinates.
//!
//! This function draws a slider on the display.  This is called in response to
//! a \b #WIDGET_MSG_PAINT message or when the slider position changes.
//!
//! \return None.
//
//*****************************************************************************
static void
SliderPaint(tWidget *pWidget, tRectangle *pDirty)
{
    tRectangle sClipRect, sValueRect, sEmptyRect, sActiveClip;
    tSliderWidget *pSlider;
    tContext sCtx;
    long lX, lY, bIntersect;
    short sPos;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a slider widget pointer.
    //
    pSlider = (tSliderWidget *)pWidget;

    //
    // Initialize a drawing context.
    //
    GrContextInit(&sCtx, pWidget->pDisplay);

    //
    // Initialize the clipping region based on the update rectangle passed.
    //
    bIntersect = GrRectIntersectGet(pDirty, &(pSlider->sBase.sPosition),
                                    &sClipRect);
    GrContextClipRegionSet(&sCtx, &sClipRect);

    //
    // Draw the control outline if necessary.
    //
    if(pSlider->ulStyle & SL_STYLE_OUTLINE)
    {
        //
        // Outline the slider with the outline color.
        //
        GrContextForegroundSet(&sCtx, pSlider->ulOutlineColor);
        GrRectDraw(&sCtx, &(pWidget->sPosition));

        //
        // Adjust the clipping rectangle to prevent the outline from being
        // corrupted later.
        //
        if(sClipRect.sXMin == pWidget->sPosition.sXMin)
        {
            sClipRect.sXMin++;
        }

        if(sClipRect.sYMin == pWidget->sPosition.sYMin)
        {
            sClipRect.sYMin++;
        }

        if(sClipRect.sXMax == pWidget->sPosition.sXMax)
        {
            sClipRect.sXMax--;
        }

        if(sClipRect.sYMax == pWidget->sPosition.sYMax)
        {
            sClipRect.sYMax--;
        }
    }

    //
    // Determine the position associated with the current slider value.
    //
    sPos = SliderValueToPosition(pSlider, pSlider->lValue);

    //
    // Remember this so that the dirty rectangle code in the click handler
    // draws the correct thing the first time it is called.
    //
    pSlider->sPos = sPos;

    //
    // Determine the rectangles for the active and empty portions of the
    // widget.
    //
    if(pSlider->ulStyle & SL_STYLE_VERTICAL)
    {
        //
        // Determine the rectangle corresponding to the bottom (value) portion
        // of the slider.
        //
        sValueRect.sXMin = pWidget->sPosition.sXMin;
        sValueRect.sXMax = pWidget->sPosition.sXMax;
        sValueRect.sYMin = sPos;
        sValueRect.sYMax = pWidget->sPosition.sYMax;

        //
        // Determine the rectangle corresponding to the top (empty) portion
        // of the slider.
        //
        sEmptyRect.sXMin = pWidget->sPosition.sXMin;
        sEmptyRect.sXMax = pWidget->sPosition.sXMax;
        sEmptyRect.sYMin = pWidget->sPosition.sYMin;
        sEmptyRect.sYMax = max(sEmptyRect.sYMin, sValueRect.sYMin - 1);
    }
    else
    {
        //
        // Determine the rectangle corresponding to the bottom (value) portion
        // of the slider.
        //
        sValueRect.sYMin = pWidget->sPosition.sYMin;
        sValueRect.sYMax = pWidget->sPosition.sYMax;
        sValueRect.sXMin = pWidget->sPosition.sXMin;
        sValueRect.sXMax = sPos;

        //
        // Determine the rectangle corresponding to the top (empty) portion
        // of the slider.
        //
        sEmptyRect.sYMin = pWidget->sPosition.sYMin;
        sEmptyRect.sYMax = pWidget->sPosition.sYMax;
        sEmptyRect.sXMax = pWidget->sPosition.sXMax;
        sEmptyRect.sXMin = min(sEmptyRect.sXMax, sValueRect.sXMax + 1);
    }

    //
    // Compute the center of the slider.  This will be needed later if drawing
    // text or an image.
    //
    lX = (pWidget->sPosition.sXMin +
          ((pWidget->sPosition.sXMax - pWidget->sPosition.sXMin + 1) / 2));
    lY = (pWidget->sPosition.sYMin +
          ((pWidget->sPosition.sYMax - pWidget->sPosition.sYMin + 1) / 2));

    //
    // Get the required clipping rectangle for the active/value part of
    // the slider.
    //
    bIntersect = GrRectIntersectGet(&sClipRect, &sValueRect, &sActiveClip);

    //
    // Does any part of the value rectangle intersect with the region we are
    // supposed to be redrawing?
    //
    if(bIntersect)
    {
        //
        // Yes - we have something to draw.
        //

        //
        // Set the new clipping rectangle.
        //
        GrContextClipRegionSet(&sCtx, &sActiveClip);

        //
        // Do we need to fill the active area with a color?
        //
        if(pSlider->ulStyle & SL_STYLE_FILL)
        {
            GrContextForegroundSet(&sCtx, pSlider->ulFillColor);
            GrRectFill(&sCtx, &sValueRect);
        }

        //
        // Do we need to draw an image in the active area?
        //
        if(pSlider->ulStyle & SL_STYLE_IMG)
        {
            GrContextForegroundSet(&sCtx, pSlider->ulTextColor);
            GrContextBackgroundSet(&sCtx, pSlider->ulFillColor);
            GrImageDraw(&sCtx, pSlider->pucImage,
                        lX - (GrImageWidthGet(pSlider->pucImage) / 2),
                        lY - (GrImageHeightGet(pSlider->pucImage) / 2));
        }

        //
        // Do we need to render a text string over the top of the active area?
        //
        if(pSlider->ulStyle & SL_STYLE_TEXT)
        {
            GrContextFontSet(&sCtx, pSlider->pFont);
            GrContextForegroundSet(&sCtx, pSlider->ulTextColor);
            GrContextBackgroundSet(&sCtx, pSlider->ulFillColor);
            GrStringDrawCentered(&sCtx, pSlider->pcText, -1, lX, lY,
                                 pSlider->ulStyle & SL_STYLE_TEXT_OPAQUE);
        }
    }

    //
    // Now get the required clipping rectangle for the background portion of
    // the slider.
    //
    bIntersect = GrRectIntersectGet(&sClipRect, &sEmptyRect, &sActiveClip);

    //
    // Does any part of the background rectangle intersect with the region we
    // are supposed to be redrawing?
    //
    if(bIntersect)
    {
        //
        // Yes - we have something to draw.
        //

        //
        // Set the new clipping rectangle.
        //
        GrContextClipRegionSet(&sCtx, &sActiveClip);

        //
        // Do we need to fill the active area with a color?
        //
        if(pSlider->ulStyle & SL_STYLE_BACKG_FILL)
        {
            GrContextForegroundSet(&sCtx, pSlider->ulBackgroundFillColor);
            GrRectFill(&sCtx, &sEmptyRect);
        }

        //
        // Do we need to draw an image in the active area?
        //
        if(pSlider->ulStyle & SL_STYLE_BACKG_IMG)
        {
            GrContextForegroundSet(&sCtx, pSlider->ulBackgroundTextColor);
            GrContextBackgroundSet(&sCtx, pSlider->ulBackgroundFillColor);
            GrImageDraw(&sCtx, pSlider->pucBackgroundImage,
                    lX - (GrImageWidthGet(pSlider->pucBackgroundImage) / 2),
                    lY - (GrImageHeightGet(pSlider->pucBackgroundImage) / 2));
        }

        //
        // Do we need to render a text string over the top of the active area?
        //
        if(pSlider->ulStyle & SL_STYLE_BACKG_TEXT)
        {
            GrContextFontSet(&sCtx, pSlider->pFont);
            GrContextForegroundSet(&sCtx, pSlider->ulBackgroundTextColor);
            GrContextBackgroundSet(&sCtx, pSlider->ulBackgroundFillColor);
            GrStringDrawCentered(&sCtx, pSlider->pcText, -1, lX, lY,
                                 pSlider->ulStyle & SL_STYLE_BACKG_TEXT_OPAQUE);
        }
    }
}

//*****************************************************************************
//
//! Handles pointer events for slider.
//!
//! \param pWidget is a pointer to the slider widget.
//! \param ulMsg is the pointer event message.
//! \param lX is the X coordinate of the pointer event.
//! \param lY is the Y coordinate of the pointer event.
//!
//! This function processes pointer event messages for a slider.  This is
//! called in response to a \b #WIDGET_MSG_PTR_DOWN, \b #WIDGET_MSG_PTR_MOVE,
//! and \b #WIDGET_MSG_PTR_UP messages.
//!
//! If the message is \b #WIDGET_MSG_PTR_MOVE or is \b #WIDGET_MSG_PTR_DOWN and
//! the coordinates are within the bounds of the slider, the slider value is
//! updated and, if changed, the slider's OnChange callback function is called.
//!
//! \return Returns 1 if the message was consumed by the slider and 0
//! otherwise.
//
//*****************************************************************************
static long
SliderClick(tWidget *pWidget, unsigned long ulMsg, long lX, long lY)
{
    tSliderWidget *pSlider;
    tRectangle sRedrawRect;
    short sPos;
    long lNewVal;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a slider widget pointer.
    //
    pSlider = (tSliderWidget *)pWidget;

    //
    // If the slider is locked, ignore all pointer messages.
    //
    if(pSlider->ulStyle & SL_STYLE_LOCKED)
    {
        return(0);
    }

    //
    // See if the given coordinates are within the extents of the slider.
    //
    if((ulMsg == WIDGET_MSG_PTR_MOVE) ||
       ((ulMsg == WIDGET_MSG_PTR_DOWN) &&
        (lX >= pWidget->sPosition.sXMin) &&
        (lX <= pWidget->sPosition.sXMax) &&
        (lY >= pWidget->sPosition.sYMin) &&
        (lY <= pWidget->sPosition.sYMax)))
    {
        //
        // Map the pointer position to a slider value.
        //
        lNewVal = SliderPositionToValue(pSlider,
                             (pSlider->ulStyle & SL_STYLE_VERTICAL) ? lY : lX);

        //
        // Convert back to ensure that the dirty rectangle we calculate here
        // uses the same values as will be used when the widget is next
        // painted.
        //
        sPos = SliderValueToPosition(pSlider, lNewVal);

        //
        // Did the value change?
        //
        if(lNewVal != pSlider->lValue)
        {
            //
            // Yes - the value changed so report it to the app and redraw the
            // slider.
            //
            if(pSlider->pfnOnChange)
            {
                (pSlider->pfnOnChange)(pWidget, lNewVal);
            }

            //
            // Determine the rectangle that we need to redraw to update the
            // slider to the new position.
            //
            if(pSlider->ulStyle & SL_STYLE_VERTICAL)
            {
                //
                // Vertical slider case.
                //
                sRedrawRect.sYMin = min(pSlider->sPos, sPos);
                sRedrawRect.sYMax = max(pSlider->sPos, sPos);
                sRedrawRect.sXMin = pWidget->sPosition.sXMin;
                sRedrawRect.sXMax = pWidget->sPosition.sXMax;
            }
            else
            {
                //
                // Horizontal slider case.
                //
                sRedrawRect.sXMin = min(pSlider->sPos, sPos);
                sRedrawRect.sXMax = max(pSlider->sPos, sPos);
                sRedrawRect.sYMin = pWidget->sPosition.sYMin;
                sRedrawRect.sYMax = pWidget->sPosition.sYMax;
            }

            //
            // Update the widget value and position.
            //
            pSlider->lValue = lNewVal;
            pSlider->sPos = sPos;

            //
            // Redraw the area of the control that has changed.
            //
            SliderPaint(pWidget, &sRedrawRect);
        }

        //
        // These coordinates are within the extents of the slider widget.
        //
        return(1);
    }

    //
    // These coordinates are not within the extents of the slider widget.
    //
    return(0);
}

//*****************************************************************************
//
//! Handles messages for a slider widget.
//!
//! \param pWidget is a pointer to the slider widget.
//! \param ulMsg is the message.
//! \param ulParam1 is the first parameter to the message.
//! \param ulParam2 is the second parameter to the message.
//!
//! This function receives messages intended for this slider widget and
//! processes them accordingly.  The processing of the message varies based on
//! the message in question.
//!
//! Unrecognized messages are handled by calling WidgetDefaultMsgProc().
//!
//! \return Returns a value appropriate to the supplied message.
//
//*****************************************************************************
long
SliderMsgProc(tWidget *pWidget, unsigned long ulMsg,
                         unsigned long ulParam1, unsigned long ulParam2)
{
    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Determine which message is being sent.
    //
    switch(ulMsg)
    {
        //
        // The widget paint request has been sent.
        //
        case WIDGET_MSG_PAINT:
        {
            //
            // Handle the widget paint request.
            //
            SliderPaint(pWidget, &(pWidget->sPosition));

            //
            // Return one to indicate that the message was successfully
            // processed.
            //
            return(1);
        }

        //
        // One of the pointer requests has been sent.
        //
        case WIDGET_MSG_PTR_DOWN:
        case WIDGET_MSG_PTR_MOVE:
        case WIDGET_MSG_PTR_UP:
        {
            //
            // Handle the pointer request, returning the appropriate value.
            //
            return(SliderClick(pWidget, ulMsg, ulParam1, ulParam2));
        }

        //
        // An unknown request has been sent.
        //
        default:
        {
            //
            // Let the default message handler process this message.
            //
            return(WidgetDefaultMsgProc(pWidget, ulMsg, ulParam1, ulParam2));
        }
    }
}

//*****************************************************************************
//
//! Initializes a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to initialize.
//! \param pDisplay is a pointer to the display on which to draw the slider.
//! \param lX is the X coordinate of the upper left corner of the slider.
//! \param lY is the Y coordinate of the upper left corner of the slider.
//! \param lWidth is the width of the slider.
//! \param lHeight is the height of the slider.
//!
//! This function initializes the provided slider widget.
//!
//! \return None.
//
//*****************************************************************************
void
SliderInit(tSliderWidget *pWidget, const tDisplay *pDisplay, long lX, long lY,
           long lWidth, long lHeight)
{
    unsigned long ulIdx;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);
    ASSERT(pDisplay);

    //
    // Clear out the widget structure.
    //
    for(ulIdx = 0; ulIdx < sizeof(tSliderWidget); ulIdx += 4)
    {
        ((unsigned long *)pWidget)[ulIdx / 4] = 0;
    }

    //
    // Set the size of the slider widget structure.
    //
    pWidget->sBase.lSize = sizeof(tSliderWidget);

    //
    // Mark this widget as fully disconnected.
    //
    pWidget->sBase.pParent = 0;
    pWidget->sBase.pNext = 0;
    pWidget->sBase.pChild = 0;

    //
    // Save the display pointer.
    //
    pWidget->sBase.pDisplay = pDisplay;

    //
    // Set the extents of this slider.
    //
    pWidget->sBase.sPosition.sXMin = lX;
    pWidget->sBase.sPosition.sYMin = lY;
    pWidget->sBase.sPosition.sXMax = lX + lWidth - 1;
    pWidget->sBase.sPosition.sYMax = lY + lHeight - 1;

    //
    // Use the slider message handler to process messages to this widget.
    //
    pWidget->sBase.pfnMsgProc = SliderMsgProc;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
