//*****************************************************************************
//
// checkbox.c - Check box widget.
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
#include "grlib/checkbox.h"

//*****************************************************************************
//
//! \addtogroup checkbox_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Draws a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to be drawn.
//! \param bClick is a boolean that is \b true if the paint request is a result
//! of a pointer click and \b false if not.
//!
//! This function draws a check box widget on the display.  This is called in
//! response to a \b #WIDGET_MSG_PAINT message.
//!
//! \return None.
//
//*****************************************************************************
static void
CheckBoxPaint(tWidget *pWidget, unsigned long bClick)
{
    tCheckBoxWidget *pCheck;
    tRectangle sRect;
    tContext sCtx;
    long lY;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a check box widget pointer.
    //
    pCheck = (tCheckBoxWidget *)pWidget;

    //
    // Initialize a drawing context.
    //
    GrContextInit(&sCtx, pWidget->pDisplay);

    //
    // Initialize the clipping region based on the extents of this check box.
    //
    GrContextClipRegionSet(&sCtx, &(pWidget->sPosition));

    //
    // See if the check box fill style is selected.
    //
    if((pCheck->usStyle & CB_STYLE_FILL) && !bClick)
    {
        //
        // Fill the check box with the fill color.
        //
        GrContextForegroundSet(&sCtx, pCheck->ulFillColor);
        GrRectFill(&sCtx, &(pWidget->sPosition));
    }

    //
    // See if the check box outline style is selected.
    //
    if((pCheck->usStyle & CB_STYLE_OUTLINE) && !bClick)
    {
        //
        // Outline the check box with the outline color.
        //
        GrContextForegroundSet(&sCtx, pCheck->ulOutlineColor);
        GrRectDraw(&sCtx, &(pWidget->sPosition));
    }

    //
    // Draw the check box.
    //
    sRect.sXMin = pWidget->sPosition.sXMin + 2;
    sRect.sYMin = (pWidget->sPosition.sYMin +
                   ((pWidget->sPosition.sYMax - pWidget->sPosition.sYMin -
                     pCheck->usBoxSize + 1) / 2));
    sRect.sXMax = sRect.sXMin + pCheck->usBoxSize - 1;
    sRect.sYMax = sRect.sYMin + pCheck->usBoxSize - 1;
    if(!bClick)
    {
        GrContextForegroundSet(&sCtx, pCheck->ulOutlineColor);
        GrRectDraw(&sCtx, &sRect);
    }

    //
    // Select the foreground color based on whether or not the check box is
    // selected.
    //
    if(pCheck->usStyle & CB_STYLE_SELECTED)
    {
        GrContextForegroundSet(&sCtx, pCheck->ulOutlineColor);
    }
    else
    {
        GrContextForegroundSet(&sCtx, pCheck->ulFillColor);
    }

    //
    // Draw an "X" in the check box.
    //
    GrLineDraw(&sCtx, sRect.sXMin + 1, sRect.sYMin + 1, sRect.sXMax - 1,
               sRect.sYMax - 1);
    GrLineDraw(&sCtx, sRect.sXMin + 1, sRect.sYMax - 1, sRect.sXMax - 1,
               sRect.sYMin + 1);

    //
    // See if the check box text or image style is selected.
    //
    if((pCheck->usStyle & (CB_STYLE_TEXT | CB_STYLE_IMG)) && !bClick)
    {
        //
        // Shrink the clipping region by the size of the check box so that it
        // is not overwritten by further "decorative" portions of the widget.
        //
        sCtx.sClipRegion.sXMin += pCheck->usBoxSize + 4;

        //
        // If the check box outline style is selected then shrink the clipping
        // region by one pixel on each side so that the outline is not
        // overwritten by the text or image.
        //
        if(pCheck->usStyle & CB_STYLE_OUTLINE)
        {
            sCtx.sClipRegion.sYMin++;
            sCtx.sClipRegion.sXMax--;
            sCtx.sClipRegion.sYMax--;
        }

        //
        // See if the check box image style is selected.
        //
        if(pCheck->usStyle & CB_STYLE_IMG)
        {
            //
            // Determine where along the Y extent of the widget to draw the
            // image.  It is drawn at the top if it takes all (or more than
            // all) of the Y extent of the widget, and it is drawn centered if
            // it takes less than the Y extent.
            //
            if(GrImageHeightGet(pCheck->pucImage) >
               (sCtx.sClipRegion.sYMax - sCtx.sClipRegion.sYMin))
            {
                lY = sCtx.sClipRegion.sYMin;
            }
            else
            {
                lY = (sCtx.sClipRegion.sYMin +
                      ((sCtx.sClipRegion.sYMax - sCtx.sClipRegion.sYMin -
                        GrImageHeightGet(pCheck->pucImage) + 1) / 2));
            }

            //
            // Set the foreground and background colors to use for 1 BPP
            // images.
            //
            GrContextForegroundSet(&sCtx, pCheck->ulTextColor);
            GrContextBackgroundSet(&sCtx, pCheck->ulFillColor);

            //
            // Draw the image next to the check box.
            //
            GrImageDraw(&sCtx, pCheck->pucImage, sCtx.sClipRegion.sXMin, lY);
        }

        //
        // See if the check box text style is selected.
        //
        if(pCheck->usStyle & CB_STYLE_TEXT)
        {
            //
            // Determine where along the Y extent of the widget to draw the
            // string.  It is drawn at the top if it takes all (or more than
            // all) of the Y extent of the widget, and it is drawn centered if
            // it takes less than the Y extent.
            //
            if(GrFontHeightGet(pCheck->pFont) >
               (sCtx.sClipRegion.sYMax - sCtx.sClipRegion.sYMin))
            {
                lY = sCtx.sClipRegion.sYMin;
            }
            else
            {
                lY = (sCtx.sClipRegion.sYMin +
                      ((sCtx.sClipRegion.sYMax - sCtx.sClipRegion.sYMin -
                        GrFontHeightGet(pCheck->pFont) + 1) / 2));
            }

            //
            // Draw the text next to the check box.
            //
            GrContextFontSet(&sCtx, pCheck->pFont);
            GrContextForegroundSet(&sCtx, pCheck->ulTextColor);
            GrContextBackgroundSet(&sCtx, pCheck->ulFillColor);
            GrStringDraw(&sCtx, pCheck->pcText, -1, sCtx.sClipRegion.sXMin,
                         lY, pCheck->usStyle & CB_STYLE_TEXT_OPAQUE);
        }
    }
}

//*****************************************************************************
//
//! Handles pointer events for a check box.
//!
//! \param pWidget is a pointer to the check box widget.
//! \param ulMsg is the pointer event message.
//! \param lX is the X coordinate of the pointer event.
//! \param lY is the Y coordiante of the pointer event.
//!
//! This function processes pointer event messages for a check box.  This is
//! called in response to a \b #WIDGET_MSG_PTR_DOWN, \b #WIDGET_MSG_PTR_MOVE,
//! and \b #WIDGET_MSG_PTR_UP messages.
//!
//! If the \b #WIDGET_MSG_PTR_UP message is received with a position within the
//! extents of the check box, the check box's selected state will be toggled
//! and its OnChange function is called.
//!
//! \return Returns 1 if the coordinates are within the extents of the check
//! box and 0 otherwise.
//
//*****************************************************************************
static long
CheckBoxClick(tWidget *pWidget, unsigned long ulMsg, long lX, long lY)
{
    tCheckBoxWidget *pCheck;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a check box widget pointer.
    //
    pCheck = (tCheckBoxWidget *)pWidget;

    //
    // See if the given coordinates are within the extents of the check box.
    //
    if((lX >= pWidget->sPosition.sXMin) &&
       (lX <= pWidget->sPosition.sXMax) &&
       (lY >= pWidget->sPosition.sYMin) &&
       (lY <= pWidget->sPosition.sYMax))
    {
        //
        // See if the pointer was just raised.
        //
        if(ulMsg == WIDGET_MSG_PTR_UP)
        {
            //
            // Toggle the selected state of this check box.
            //
            pCheck->usStyle ^= CB_STYLE_SELECTED;

            //
            // Redraw the check box based on the new selected state.
            //
            CheckBoxPaint(pWidget, 1);

            //
            // If there is an OnChange callback for this widget then call the
            // callback.
            //
            if(pCheck->pfnOnChange)
            {
                pCheck->pfnOnChange(pWidget,
                                    pCheck->usStyle & CB_STYLE_SELECTED);
            }
        }

        //
        // These coordinates are within the extents of the check box widget.
        //
        return(1);
    }

    //
    // These coordinates are not within the extents of the check box widget.
    //
    return(0);
}

//*****************************************************************************
//
//! Handles messages for a check box widget.
//!
//! \param pWidget is a pointer to the check box widget.
//! \param ulMsg is the message.
//! \param ulParam1 is the first parameter to the message.
//! \param ulParam2 is the second parameter to the message.
//!
//! This function receives messages intended for this check box widget and
//! processes them accordingly.  The processing of the message varies based on
//! the message in question.
//!
//! Unrecognized messages are handled by calling WidgetDefaultMsgProc().
//!
//! \return Returns a value appropriate to the supplied message.
//
//*****************************************************************************
long
CheckBoxMsgProc(tWidget *pWidget, unsigned long ulMsg, unsigned long ulParam1,
                unsigned long ulParam2)
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
            CheckBoxPaint(pWidget, 0);

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
            return(CheckBoxClick(pWidget, ulMsg, ulParam1, ulParam2));
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
//! Initializes a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to initialize.
//! \param pDisplay is a pointer to the display on which to draw the check box.
//! \param lX is the X coordinate of the upper left corner of the check box.
//! \param lY is the Y coordinate of the upper left corner of the check box.
//! \param lWidth is the width of the check box.
//! \param lHeight is the height of the check box.
//!
//! This function initializes the provided check box widget.
//!
//! \return None.
//
//*****************************************************************************
void
CheckBoxInit(tCheckBoxWidget *pWidget, const tDisplay *pDisplay, long lX,
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
    for(ulIdx = 0; ulIdx < sizeof(tCheckBoxWidget); ulIdx += 4)
    {
        ((unsigned long *)pWidget)[ulIdx / 4] = 0;
    }

    //
    // Set the size of the check box widget structure.
    //
    pWidget->sBase.lSize = sizeof(tCheckBoxWidget);

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
    // Set the extents of this check box.
    //
    pWidget->sBase.sPosition.sXMin = lX;
    pWidget->sBase.sPosition.sYMin = lY;
    pWidget->sBase.sPosition.sXMax = lX + lWidth - 1;
    pWidget->sBase.sPosition.sYMax = lY + lHeight - 1;

    //
    // Use the check box message handler to processage messages to this check
    // box.
    //
    pWidget->sBase.pfnMsgProc = CheckBoxMsgProc;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
