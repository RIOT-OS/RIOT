//*****************************************************************************
//
// canvas.c - A drawing canvas widget.
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
#include "grlib/canvas.h"

//*****************************************************************************
//
//! \addtogroup canvas_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Draws the contents of a canvas.
//!
//! \param pWidget is a pointer to the canvas widget to be drawn.
//!
//! This function draws the contents of a canvas on the display.  This is
//! called in response to a \b #WIDGET_MSG_PAINT message.
//!
//! \return None.
//
//*****************************************************************************
static void
CanvasPaint(tWidget *pWidget)
{
    tCanvasWidget *pCanvas;
    tContext sCtx;
    long lX, lY, lSize;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a canvas widget pointer.
    //
    pCanvas = (tCanvasWidget *)pWidget;

    //
    // Initialize a drawing context.
    //
    GrContextInit(&sCtx, pWidget->pDisplay);

    //
    // Initialize the clipping region based on the extents of this canvas.
    //
    GrContextClipRegionSet(&sCtx, &(pWidget->sPosition));

    //
    // See if the canvas fill style is selected.
    //
    if(pCanvas->ulStyle & CANVAS_STYLE_FILL)
    {
        //
        // Fill the canvas with the fill color.
        //
        GrContextForegroundSet(&sCtx, pCanvas->ulFillColor);
        GrRectFill(&sCtx, &(pWidget->sPosition));
    }

    //
    // See if the canvas outline style is selected.
    //
    if(pCanvas->ulStyle & CANVAS_STYLE_OUTLINE)
    {
        //
        // Outline the canvas with the outline color.
        //
        GrContextForegroundSet(&sCtx, pCanvas->ulOutlineColor);
        GrRectDraw(&sCtx, &(pWidget->sPosition));
    }

    //
    // See if the canvas text or image style is selected.
    //
    if(pCanvas->ulStyle & (CANVAS_STYLE_TEXT | CANVAS_STYLE_IMG))
    {
        //
        // Compute the center of the canvas.
        //
        lX = (pWidget->sPosition.sXMin +
              ((pWidget->sPosition.sXMax - pWidget->sPosition.sXMin + 1) / 2));
        lY = (pWidget->sPosition.sYMin +
              ((pWidget->sPosition.sYMax - pWidget->sPosition.sYMin + 1) / 2));

        //
        // If the canvas outline style is selected then shrink the clipping
        // region by one pixel on each side so that the outline is not
        // overwritten by the text or image.
        //
        if(pCanvas->ulStyle & CANVAS_STYLE_OUTLINE)
        {
            sCtx.sClipRegion.sXMin++;
            sCtx.sClipRegion.sYMin++;
            sCtx.sClipRegion.sXMax--;
            sCtx.sClipRegion.sYMax--;
        }

        //
        // See if the canvas image style is selected.
        //
        if(pCanvas->ulStyle & CANVAS_STYLE_IMG)
        {
            //
            // Set the foreground and background colors to use for 1 BPP
            // images.
            //
            GrContextForegroundSet(&sCtx, pCanvas->ulTextColor);
            GrContextBackgroundSet(&sCtx, pCanvas->ulFillColor);

            //
            // Draw the image centered in the canvas.
            //
            GrImageDraw(&sCtx, pCanvas->pucImage,
                        lX - (GrImageWidthGet(pCanvas->pucImage) / 2),
                        lY - (GrImageHeightGet(pCanvas->pucImage) / 2));
        }

        //
        // See if the canvas text style is selected.
        //
        if(pCanvas->ulStyle & CANVAS_STYLE_TEXT)
        {
            //
            // Set the relevant font and colors.
            //
            GrContextFontSet(&sCtx, pCanvas->pFont);
            GrContextForegroundSet(&sCtx, pCanvas->ulTextColor);
            GrContextBackgroundSet(&sCtx, pCanvas->ulFillColor);

            //
            // Determine the drawing position for the string based on the
            // text alignment style.  First consider the horizontal case.  We
            // enter this section with lX set to the center of the widget.
            //

            //
            // How wide is the string?
            //
            lSize = GrStringWidthGet(&sCtx, pCanvas->pcText, -1);

            if(pCanvas->ulStyle & CANVAS_STYLE_TEXT_LEFT)
            {
                //
                // The string is to be aligned with the left edge of
                // the widget.  Use the clipping rectangle as reference
                // since this will ensure that the string doesn't
                // encroach on any border that is set.
                //
                lX = sCtx.sClipRegion.sXMin;
            }
            else
            {
                if(pCanvas->ulStyle & CANVAS_STYLE_TEXT_RIGHT)
                {
                    //
                    // The string is to be aligned with the right edge of
                    // the widget.  Use the clipping rectangle as reference
                    // since this will ensure that the string doesn't
                    // encroach on any border that is set.
                    //
                    lX = sCtx.sClipRegion.sXMax - lSize;
                }
                else
                {
                    //
                    // We are centering the string horizontally so adjust
                    // the position accordingly to take into account the
                    // width of the string.
                    //
                    lX -= (lSize / 2);
                }
            }

            //
            // Now consider the horizontal case.  We enter this section with lY
            // set to the center of the widget.
            //
            // How tall is the string?
            //
            lSize = GrStringHeightGet(&sCtx);

            if(pCanvas->ulStyle & CANVAS_STYLE_TEXT_TOP)
            {
                //
                // The string is to be aligned with the top edge of
                // the widget.  Use the clipping rectangle as reference
                // since this will ensure that the string doesn't
                // encroach on any border that is set.
                //
                lY = sCtx.sClipRegion.sYMin;
            }
            else
            {
                if(pCanvas->ulStyle & CANVAS_STYLE_TEXT_BOTTOM)
                {
                    //
                    // The string is to be aligned with the bottom edge of
                    // the widget.  Use the clipping rectangle as reference
                    // since this will ensure that the string doesn't
                    // encroach on any border that is set.
                    //
                    lY = sCtx.sClipRegion.sYMax - lSize;
                }
                else
                {
                    //
                    // We are centering the string vertically so adjust
                    // the position accordingly to take into account the
                    // height of the string.
                    //
                    lY -= (lSize / 2);
                }
            }

            //
            // Now draw the string.
            //
            GrStringDraw(&sCtx, pCanvas->pcText, -1, lX, lY,
                         pCanvas->ulStyle & CANVAS_STYLE_TEXT_OPAQUE);
        }
    }

    //
    // See if the application-drawn style is selected.
    //
    if(pCanvas->ulStyle & CANVAS_STYLE_APP_DRAWN)
    {
        //
        // Call the application-supplied function to draw the canvas.
        //
        pCanvas->pfnOnPaint(pWidget, &sCtx);
    }
}

//*****************************************************************************
//
//! Handles messages for a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget.
//! \param ulMsg is the message.
//! \param ulParam1 is the first parameter to the message.
//! \param ulParam2 is the second parameter to the message.
//!
//! This function receives messages intended for this canvas widget and
//! processes them accordingly.  The processing of the message varies based on
//! the message in question.
//!
//! Unrecognized messages are handled by calling WidgetDefaultMsgProc().
//!
//! \return Returns a value appropriate to the supplied message.
//
//*****************************************************************************
long
CanvasMsgProc(tWidget *pWidget, unsigned long ulMsg, unsigned long ulParam1,
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
            CanvasPaint(pWidget);

            //
            // Return one to indicate that the message was successfully
            // processed.
            //
            return(1);
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
//! Initializes a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to initialize.
//! \param pDisplay is a pointer to the display on which to draw the canvas.
//! \param lX is the X coordinate of the upper left corner of the canvas.
//! \param lY is the Y coordinate of the upper left corner of the canvas.
//! \param lWidth is the width of the canvas.
//! \param lHeight is the height of the canvas.
//!
//! This function initializes the provided canvas widget.
//!
//! \return None.
//
//*****************************************************************************
void
CanvasInit(tCanvasWidget *pWidget, const tDisplay *pDisplay, long lX, long lY,
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
    for(ulIdx = 0; ulIdx < sizeof(tCanvasWidget); ulIdx += 4)
    {
        ((unsigned long *)pWidget)[ulIdx / 4] = 0;
    }

    //
    // Set the size of the canvas widget structure.
    //
    pWidget->sBase.lSize = sizeof(tCanvasWidget);

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
    // Set the extents of this canvas.
    //
    pWidget->sBase.sPosition.sXMin = lX;
    pWidget->sBase.sPosition.sYMin = lY;
    pWidget->sBase.sPosition.sXMax = lX + lWidth - 1;
    pWidget->sBase.sPosition.sYMax = lY + lHeight - 1;

    //
    // Use the canvas message handler to process messages to this canvas.
    //
    pWidget->sBase.pfnMsgProc = CanvasMsgProc;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
