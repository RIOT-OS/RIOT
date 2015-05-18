//*****************************************************************************
//
// radiobutton.c - Radio button widget.
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
#include "grlib/radiobutton.h"

//*****************************************************************************
//
//! \addtogroup radiobutton_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Draws a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to be drawn.
//! \param bClick is a boolean that is \b true if the paint request is a result
//! of a pointer click and \b false if not.
//!
//! This function draws a radio button widget on the display.  This is called
//! in response to a \b #WIDGET_MSG_PAINT message.
//!
//! \return None.
//
//*****************************************************************************
static void
RadioButtonPaint(tWidget *pWidget, unsigned long bClick)
{
    tRadioButtonWidget *pRadio;
    tContext sCtx;
    long lX, lY;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a radio button widget pointer.
    //
    pRadio = (tRadioButtonWidget *)pWidget;

    //
    // Initialize a drawing context.
    //
    GrContextInit(&sCtx, pWidget->pDisplay);

    //
    // Initialize the clipping region based on the extents of this radio
    // button.
    //
    GrContextClipRegionSet(&sCtx, &(pWidget->sPosition));

    //
    // See if the radio button fill style is selected.
    //
    if((pRadio->usStyle & RB_STYLE_FILL) && !bClick)
    {
        //
        // Fill the radio button with the fill color.
        //
        GrContextForegroundSet(&sCtx, pRadio->ulFillColor);
        GrRectFill(&sCtx, &(pWidget->sPosition));
    }

    //
    // See if the radio button outline style is selected.
    //
    if((pRadio->usStyle & RB_STYLE_OUTLINE) && !bClick)
    {
        //
        // Outline the radio button with the outline color.
        //
        GrContextForegroundSet(&sCtx, pRadio->ulOutlineColor);
        GrRectDraw(&sCtx, &(pWidget->sPosition));
    }

    //
    // Draw the radio button.
    //
    lX = pWidget->sPosition.sXMin + (pRadio->usCircleSize / 2) + 2;
    lY = (pWidget->sPosition.sYMin +
          ((pWidget->sPosition.sYMax - pWidget->sPosition.sYMin) / 2));
    if(!bClick)
    {
        GrContextForegroundSet(&sCtx, pRadio->ulOutlineColor);
        GrCircleDraw(&sCtx, lX, lY, pRadio->usCircleSize / 2);
    }

    //
    // Select the foreground color based on whether or not the radio button is
    // selected.
    //
    if(pRadio->usStyle & RB_STYLE_SELECTED)
    {
        GrContextForegroundSet(&sCtx, pRadio->ulOutlineColor);
    }
    else
    {
        GrContextForegroundSet(&sCtx, pRadio->ulFillColor);
    }

    //
    // Fill in the radio button.
    //
    GrCircleFill(&sCtx, lX, lY, (pRadio->usCircleSize / 2) - 2);

    //
    // See if the radio button text or image style is selected.
    //
    if((pRadio->usStyle & (RB_STYLE_TEXT | RB_STYLE_IMG)) && !bClick)
    {
        //
        // Shrink the clipping region by the size of the radio button so that
        // it is not overwritten by further "decorative" portions of the
        // widget.
        //
        sCtx.sClipRegion.sXMin += pRadio->usCircleSize + 4;

        //
        // If the radio button outline style is selected then shrink the
        // clipping region by one pixel on each side so that the outline is not
        // overwritten by the text or image.
        //
        if(pRadio->usStyle & RB_STYLE_OUTLINE)
        {
            sCtx.sClipRegion.sYMin++;
            sCtx.sClipRegion.sXMax--;
            sCtx.sClipRegion.sYMax--;
        }

        //
        // See if the radio button image style is selected.
        //
        if(pRadio->usStyle & RB_STYLE_IMG)
        {
            //
            // Determine where along the Y extent of the widget to draw the
            // image.  It is drawn at the top if it takes all (or more than
            // all) of the Y extent of the widget, and it is drawn centered if
            // it takes less than the Y extent.
            //
            if(GrImageHeightGet(pRadio->pucImage) >
               (sCtx.sClipRegion.sYMax - sCtx.sClipRegion.sYMin))
            {
                lY = sCtx.sClipRegion.sYMin;
            }
            else
            {
                lY = (sCtx.sClipRegion.sYMin +
                      ((sCtx.sClipRegion.sYMax - sCtx.sClipRegion.sYMin -
                        GrImageHeightGet(pRadio->pucImage) + 1) / 2));
            }

            //
            // Set the foreground and background colors to use for 1 BPP
            // images.
            //
            GrContextForegroundSet(&sCtx, pRadio->ulTextColor);
            GrContextBackgroundSet(&sCtx, pRadio->ulFillColor);

            //
            // Draw the image next to the radio button.
            //
            GrImageDraw(&sCtx, pRadio->pucImage, sCtx.sClipRegion.sXMin, lY);
        }

        //
        // See if the radio button text style is selected.
        //
        if(pRadio->usStyle & RB_STYLE_TEXT)
        {
            //
            // Determine where along the Y extent of the widget to draw the
            // string.  It is drawn at the top if it takes all (or more than
            // all) of the Y extent of the widget, and it is drawn centered if
            // it takes less than the Y extent.
            //
            if(GrFontHeightGet(pRadio->pFont) >
               (sCtx.sClipRegion.sYMax - sCtx.sClipRegion.sYMin))
            {
                lY = sCtx.sClipRegion.sYMin;
            }
            else
            {
                lY = (sCtx.sClipRegion.sYMin +
                      ((sCtx.sClipRegion.sYMax - sCtx.sClipRegion.sYMin -
                        GrFontHeightGet(pRadio->pFont) + 1) / 2));
            }

            //
            // Draw the text next to the radio button.
            //
            GrContextFontSet(&sCtx, pRadio->pFont);
            GrContextForegroundSet(&sCtx, pRadio->ulTextColor);
            GrContextBackgroundSet(&sCtx, pRadio->ulFillColor);
            GrStringDraw(&sCtx, pRadio->pcText, -1, sCtx.sClipRegion.sXMin, lY,
                         pRadio->usStyle & RB_STYLE_TEXT_OPAQUE);
        }
    }
}

//*****************************************************************************
//
//! Handles pointer events for a radio button.
//!
//! \param pWidget is a pointer to the radio button widget.
//! \param ulMsg is the pointer event message.
//! \param lX is the X coordinate of the pointer event.
//! \param lY is the Y coordiante of the pointer event.
//!
//! This function processes pointer event messages for a radio button.  This is
//! called in response to a \b #WIDGET_MSG_PTR_DOWN, \b #WIDGET_MSG_PTR_MOVE,
//! and \b #WIDGET_MSG_PTR_UP messages.
//!
//! If the \b #WIDGET_MSG_PTR_UP message is received with a position within the
//! extents of the radio button, the radio button's selected state will be
//! unchanged if it is already selected.  If it is not selected, it will be
//! selected, its OnChange function will be called, and the peer radio button
//! widget that is selected will be unselected, causing its OnChange to be
//! called as well.
//!
//! \return Returns 1 if the coordinates are within the extents of the radio
//! button and 0 otherwise.
//
//*****************************************************************************
static long
RadioButtonClick(tWidget *pWidget, unsigned long ulMsg, long lX, long lY)
{
    tRadioButtonWidget *pRadio, *pRadio2;
    tWidget *pSibling;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a radio button widget pointer.
    //
    pRadio = (tRadioButtonWidget *)pWidget;

    //
    // See if the given coordinates are within the extents of the radio button.
    //
    if((lX >= pWidget->sPosition.sXMin) &&
       (lX <= pWidget->sPosition.sXMax) &&
       (lY >= pWidget->sPosition.sYMin) &&
       (lY <= pWidget->sPosition.sYMax))
    {
        //
        // See if the pointer was just raised and this radio button is not
        // selected.
        //
        if((ulMsg == WIDGET_MSG_PTR_UP) &&
           !(pRadio->usStyle & RB_STYLE_SELECTED))
        {
            //
            // Loop through the siblings of this radio button widget.
            //
            for(pSibling = pWidget->pParent->pChild; pSibling;
                pSibling = pSibling->pNext)
            {
                //
                // Skip this widget if it is not a radio button widget, or if
                // it is the original radio button widget.
                //
                if((pSibling == pWidget) ||
                   (pSibling->pfnMsgProc != RadioButtonMsgProc))
                {
                    continue;
                }

                //
                // Convert the generic widget pointer into a radio button
                // widget pointer.
                //
                pRadio2 = (tRadioButtonWidget *)pSibling;

                //
                // See if the sibling radio button is selected.
                //
                if(pRadio2->usStyle & RB_STYLE_SELECTED)
                {
                    //
                    // Clear the selected state of the sibling radio button.
                    //
                    pRadio2->usStyle &= ~(RB_STYLE_SELECTED);

                    //
                    // Redraw the sibling radio button.
                    //
                    RadioButtonPaint(pSibling, 1);

                    //
                    // If there is an OnChange callback for the sibling radio
                    // button then call the callback.
                    //
                    if(pRadio2->pfnOnChange)
                    {
                        pRadio2->pfnOnChange(pSibling, 0);
                    }
                }
            }

            //
            // Set the selected state of this radio button.
            //
            pRadio->usStyle |= RB_STYLE_SELECTED;

            //
            // Redraw the radio button.
            //
            RadioButtonPaint(pWidget, 1);

            //
            // If there is an OnChange callback for this widget then call the
            // callback.
            //
            if(pRadio->pfnOnChange)
            {
                pRadio->pfnOnChange(pWidget, 1);
            }
        }

        //
        // These coordinates are within the extents of the radio button widget.
        //
        return(1);
    }

    //
    // These coordinates are not within the extents of the radio button widget.
    //
    return(0);
}

//*****************************************************************************
//
//! Handles messages for a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget.
//! \param ulMsg is the message.
//! \param ulParam1 is the first parameter to the message.
//! \param ulParam2 is the second parameter to the message.
//!
//! This function receives messages intended for this radio button widget and
//! processes them accordingly.  The processing of the message varies based on
//! the message in question.
//!
//! Unrecognized messages are handled by calling WidgetDefaultMsgProc().
//!
//! \return Returns a value appropriate to the supplied message.
//
//*****************************************************************************
long
RadioButtonMsgProc(tWidget *pWidget, unsigned long ulMsg,
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
            RadioButtonPaint(pWidget, 0);

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
            return(RadioButtonClick(pWidget, ulMsg, ulParam1, ulParam2));
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
//! Initializes a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to initialize.
//! \param pDisplay is a pointer to the display on which to draw the push
//! button.
//! \param lX is the X coordinate of the upper left corner of the radio button.
//! \param lY is the Y coordinate of the upper left corner of the radio button.
//! \param lWidth is the width of the radio button.
//! \param lHeight is the height of the radio button.
//!
//! This function initializes the provided radio button widget.
//!
//! \return None.
//
//*****************************************************************************
void
RadioButtonInit(tRadioButtonWidget *pWidget, const tDisplay *pDisplay, long lX,
                long lY, long lWidth, long lHeight)
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
    for(ulIdx = 0; ulIdx < sizeof(tRadioButtonWidget); ulIdx += 4)
    {
        ((unsigned long *)pWidget)[ulIdx / 4] = 0;
    }

    //
    // Set the size of the radio button widget structure.
    //
    pWidget->sBase.lSize = sizeof(tRadioButtonWidget);

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
    // Set the extents of this radio button.
    //
    pWidget->sBase.sPosition.sXMin = lX;
    pWidget->sBase.sPosition.sYMin = lY;
    pWidget->sBase.sPosition.sXMax = lX + lWidth - 1;
    pWidget->sBase.sPosition.sYMax = lY + lHeight - 1;

    //
    // Use the radio button message handler to processage messages to this
    // radio button.
    //
    pWidget->sBase.pfnMsgProc = RadioButtonMsgProc;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
