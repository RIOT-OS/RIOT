//*****************************************************************************
//
// container.c - Generic container widget.
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
#include "grlib/container.h"

//*****************************************************************************
//
//! \addtogroup container_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Draws a container widget.
//!
//! \param pWidget is a pointer to the container widget to be drawn.
//!
//! This function draws a container widget on the display.  This is called in
//! response to a \b #WIDGET_MSG_PAINT message.
//!
//! \return None.
//
//*****************************************************************************
static void
ContainerPaint(tWidget *pWidget)
{
    tContainerWidget *pContainer;
    long lX1, lX2, lY;
    tContext sCtx;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a container widget pointer.
    //
    pContainer = (tContainerWidget *)pWidget;

    //
    // Initialize a drawing context.
    //
    GrContextInit(&sCtx, pWidget->pDisplay);

    //
    // Initialize the clipping region based on the extents of this container.
    //
    GrContextClipRegionSet(&sCtx, &(pWidget->sPosition));

    //
    // See if the container fill style is selected.
    //
    if(pContainer->ulStyle & CTR_STYLE_FILL)
    {
        //
        // Fill the container with the fill color.
        //
        GrContextForegroundSet(&sCtx, pContainer->ulFillColor);
        GrRectFill(&sCtx, &(pWidget->sPosition));
    }

    //
    // See if the container text style is selected.
    //
    if(pContainer->ulStyle & CTR_STYLE_TEXT)
    {
        //
        // Set the font and colors used to draw the container text.
        //
        GrContextFontSet(&sCtx, pContainer->pFont);
        GrContextForegroundSet(&sCtx, pContainer->ulTextColor);
        GrContextBackgroundSet(&sCtx, pContainer->ulFillColor);

        //
        // Get the width of the container text.
        //
        lX2 = GrStringWidthGet(&sCtx, pContainer->pcText, -1);

        //
        // Determine the position of the text.  The position depends on the
        // the width of the string and if centering is enabled.
        //
        if(pContainer->ulStyle & CTR_STYLE_TEXT_CENTER)
        {
            lX1 = (pWidget->sPosition.sXMin +
                   ((pWidget->sPosition.sXMax - pWidget->sPosition.sXMin + 1 -
                     lX2 - 8) / 2));
        }
        else
        {
            lX1 = pWidget->sPosition.sXMin + 4;
        }

        //
        // Draw the container text.
        //
        GrStringDraw(&sCtx, pContainer->pcText, -1, lX1 + 4,
                     pWidget->sPosition.sYMin,
                     pContainer->ulStyle & CTR_STYLE_TEXT_OPAQUE);

        //
        // See if the container outline style is selected.
        //
        if(pContainer->ulStyle & CTR_STYLE_OUTLINE)
        {
            //
            // Get the position of the right side of the string.
            //
            lX2 = lX1 + lX2 + 8;

            //
            // Get the position of the vertical center of the text.
            //
            lY = (pWidget->sPosition.sYMin +
                  (GrFontBaselineGet(pContainer->pFont) / 2));

            //
            // Set the color to draw the outline.
            //
            GrContextForegroundSet(&sCtx, pContainer->ulOutlineColor);

            //
            // Draw the outline around the container widget, leaving a gap
            // where the text reside across the top of the widget.
            //
            GrLineDraw(&sCtx, lX1, lY, pWidget->sPosition.sXMin, lY);
            GrLineDraw(&sCtx, pWidget->sPosition.sXMin, lY,
                       pWidget->sPosition.sXMin, pWidget->sPosition.sYMax);
            GrLineDraw(&sCtx, pWidget->sPosition.sXMin,
                       pWidget->sPosition.sYMax, pWidget->sPosition.sXMax,
                       pWidget->sPosition.sYMax);
            GrLineDraw(&sCtx, pWidget->sPosition.sXMax,
                       pWidget->sPosition.sYMax, pWidget->sPosition.sXMax, lY);
            GrLineDraw(&sCtx, pWidget->sPosition.sXMax, lY, lX2, lY);
        }
    }

    //
    // Otherwise, see if the container outline style is selected.
    //
    else if(pContainer->ulStyle & CTR_STYLE_OUTLINE)
    {
        //
        // Outline the container with the outline color.
        //
        GrContextForegroundSet(&sCtx, pContainer->ulOutlineColor);
        GrRectDraw(&sCtx, &(pWidget->sPosition));
    }
}

//*****************************************************************************
//
//! Handles messages for a container widget.
//!
//! \param pWidget is a pointer to the container widget.
//! \param ulMsg is the message.
//! \param ulParam1 is the first parameter to the message.
//! \param ulParam2 is the second parameter to the message.
//!
//! This function receives messages intended for this container widget and
//! processes them accordingly.  The processing of the message varies based on
//! the message in question.
//!
//! Unrecognized messages are handled by calling WidgetDefaultMsgProc().
//!
//! \return Returns a value appropriate to the supplied message.
//
//*****************************************************************************
long
ContainerMsgProc(tWidget *pWidget, unsigned long ulMsg, unsigned long ulParam1,
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
            ContainerPaint(pWidget);

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
//! Initializes a container widget.
//!
//! \param pWidget is a pointer to the container widget to initialize.
//! \param pDisplay is a pointer to the display on which to draw the container
//! widget.
//! \param lX is the X coordinate of the upper left corner of the container
//! widget.
//! \param lY is the Y coordinate of the upper left corner of the container
//! widget.
//! \param lWidth is the width of the container widget.
//! \param lHeight is the height of the container widget.
//!
//! This function initializes a container widget, preparing it for placement
//! into the widget tree.
//!
//! \return none.
//
//*****************************************************************************
void
ContainerInit(tContainerWidget *pWidget, const tDisplay *pDisplay, long lX,
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
    for(ulIdx = 0; ulIdx < sizeof(tContainerWidget); ulIdx += 4)
    {
        ((unsigned long *)pWidget)[ulIdx / 4] = 0;
    }

    //
    // Set the size of the container widget structure.
    //
    pWidget->sBase.lSize = sizeof(tContainerWidget);

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
    // Set the extents of this container widget.
    //
    pWidget->sBase.sPosition.sXMin = lX;
    pWidget->sBase.sPosition.sYMin = lY;
    pWidget->sBase.sPosition.sXMax = lX + lWidth - 1;
    pWidget->sBase.sPosition.sYMax = lY + lHeight - 1;

    //
    // Use the container widget message handler to process messages to this
    // container widget.
    //
    pWidget->sBase.pfnMsgProc = ContainerMsgProc;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
