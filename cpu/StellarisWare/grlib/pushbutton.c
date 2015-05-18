//*****************************************************************************
//
// pushbutton.c - Various types of push buttons.
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
#include "grlib/pushbutton.h"

//*****************************************************************************
//
//! \addtogroup pushbutton_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Draws a rectangular push button.
//!
//! \param pWidget is a pointer to the push button widget to be drawn.
//!
//! This function draws a rectangular push button on the display.  This is
//! called in response to a \b #WIDGET_MSG_PAINT message.
//!
//! \return None.
//
//*****************************************************************************
static void
RectangularButtonPaint(tWidget *pWidget)
{
    const unsigned char *pucImage;
    tPushButtonWidget *pPush;
    tContext sCtx;
    long lX, lY;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a push button widget pointer.
    //
    pPush = (tPushButtonWidget *)pWidget;

    //
    // Initialize a drawing context.
    //
    GrContextInit(&sCtx, pWidget->pDisplay);

    //
    // Initialize the clipping region based on the extents of this rectangular
    // push button.
    //
    GrContextClipRegionSet(&sCtx, &(pWidget->sPosition));

    //
    // See if the push button fill style is selected.
    //
    if(pPush->ulStyle & PB_STYLE_FILL)
    {
        //
        // Fill the push button with the fill color.
        //
        GrContextForegroundSet(&sCtx, ((pPush->ulStyle & PB_STYLE_PRESSED) ?
                                       pPush->ulPressFillColor :
                                       pPush->ulFillColor));
        GrRectFill(&sCtx, &(pWidget->sPosition));
    }

    //
    // See if the push button outline style is selected.
    //
    if(pPush->ulStyle & PB_STYLE_OUTLINE)
    {
        //
        // Outline the push button with the outline color.
        //
        GrContextForegroundSet(&sCtx, pPush->ulOutlineColor);
        GrRectDraw(&sCtx, &(pWidget->sPosition));
    }

    //
    // See if the push button text or image style is selected.
    //
    if(pPush->ulStyle & (PB_STYLE_TEXT | PB_STYLE_IMG))
    {
        //
        // Compute the center of the push button.
        //
        lX = (pWidget->sPosition.sXMin +
              ((pWidget->sPosition.sXMax - pWidget->sPosition.sXMin + 1) / 2));
        lY = (pWidget->sPosition.sYMin +
              ((pWidget->sPosition.sYMax - pWidget->sPosition.sYMin + 1) / 2));

        //
        // If the push button outline style is selected then shrink the
        // clipping region by one pixel on each side so that the outline is not
        // overwritten by the text or image.
        //
        if(pPush->ulStyle & PB_STYLE_OUTLINE)
        {
            sCtx.sClipRegion.sXMin++;
            sCtx.sClipRegion.sYMin++;
            sCtx.sClipRegion.sXMax--;
            sCtx.sClipRegion.sYMax--;
        }

        //
        // See if the push button image style is selected.
        //
        if(pPush->ulStyle & PB_STYLE_IMG)
        {
            //
            // Set the foreground and background colors to use for 1 BPP
            // images.
            //
            GrContextForegroundSet(&sCtx, pPush->ulTextColor);
            GrContextBackgroundSet(&sCtx,
                                   ((pPush->ulStyle & PB_STYLE_PRESSED) ?
                                    pPush->ulPressFillColor :
                                    pPush->ulFillColor));

            //
            // Get the image to be drawn.
            //
            pucImage = (((pPush->ulStyle & PB_STYLE_PRESSED) &&
                         pPush->pucPressImage) ?
                        pPush->pucPressImage : pPush->pucImage);

            //
            // Draw the image centered in the push button.
            //
            GrImageDraw(&sCtx, pucImage, lX - (GrImageWidthGet(pucImage) / 2),
                        lY - (GrImageHeightGet(pucImage) / 2));
        }

        //
        // See if the push button text style is selected.
        //
        if(pPush->ulStyle & PB_STYLE_TEXT)
        {
            //
            // Draw the text centered in the middle of the push button.
            //
            GrContextFontSet(&sCtx, pPush->pFont);
            GrContextForegroundSet(&sCtx, pPush->ulTextColor);
            GrContextBackgroundSet(&sCtx,
                                   ((pPush->ulStyle & PB_STYLE_PRESSED) ?
                                    pPush->ulPressFillColor :
                                    pPush->ulFillColor));
            GrStringDrawCentered(&sCtx, pPush->pcText, -1, lX, lY,
                                 pPush->ulStyle & PB_STYLE_TEXT_OPAQUE);
        }
    }
}

//*****************************************************************************
//
//! Handles pointer events for a rectangular push button.
//!
//! \param pWidget is a pointer to the push button widget.
//! \param ulMsg is the pointer event message.
//! \param lX is the X coordinate of the pointer event.
//! \param lY is the Y coordinate of the pointer event.
//!
//! This function processes pointer event messages for a rectangular push
//! button.  This is called in response to a \b #WIDGET_MSG_PTR_DOWN,
//! \b #WIDGET_MSG_PTR_MOVE, and \b #WIDGET_MSG_PTR_UP messages.
//!
//! If the \b #WIDGET_MSG_PTR_UP message is received with a position within the
//! extents of the push button, the push button's OnClick callback function is
//! called.
//!
//! \return Returns 1 if the coordinates are within the extents of the push
//! button and 0 otherwise.
//
//*****************************************************************************
static long
RectangularButtonClick(tWidget *pWidget, unsigned long ulMsg, long lX, long lY)
{
    tPushButtonWidget *pPush;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a push button widget pointer.
    //
    pPush = (tPushButtonWidget *)pWidget;

    //
    // See if this is a pointer up message.
    //
    if(ulMsg == WIDGET_MSG_PTR_UP)
    {
        //
        // Indicate that this push button is no longer pressed.
        //
        pPush->ulStyle &= ~(PB_STYLE_PRESSED);

        //
        // If filling is enabled for this push button, or if an image is being
        // used and a pressed button image is provided, then redraw the push
        // button to show it in its non-pressed state.
        //
        if((pPush->ulStyle & PB_STYLE_FILL) ||
           ((pPush->ulStyle & PB_STYLE_IMG) && pPush->pucPressImage))
        {
            RectangularButtonPaint(pWidget);
        }

        //
        // If the pointer is still within the button bounds, and it is a
        // release notify button, call the notification function here.
        //
        if(GrRectContainsPoint(&pWidget->sPosition, lX, lY) &&
           (pPush->ulStyle & PB_STYLE_RELEASE_NOTIFY) && pPush->pfnOnClick)
        {
            pPush->pfnOnClick(pWidget);
        }
    }

    //
    // See if the given coordinates are within the extents of the push button.
    //
    if(GrRectContainsPoint(&pWidget->sPosition, lX, lY))
    {
        //
        // See if this is a pointer down message.
        //
        if(ulMsg == WIDGET_MSG_PTR_DOWN)
        {
            //
            // Indicate that this push button is pressed.
            //
            pPush->ulStyle |= PB_STYLE_PRESSED;

            //
            // If filling is enabled for this push button, or if an image is
            // being used and a pressed button image is provided, then redraw
            // the push button to show it in its pressed state.
            //
            if((pPush->ulStyle & PB_STYLE_FILL) ||
               ((pPush->ulStyle & PB_STYLE_IMG) && pPush->pucPressImage))
            {
                RectangularButtonPaint(pWidget);
            }
        }

        //
        // See if there is an OnClick callback for this widget.
        //
        if(pPush->pfnOnClick)
        {
            //
            // If the pointer was just pressed then call the callback.
            //
            if((ulMsg == WIDGET_MSG_PTR_DOWN) &&
               !(pPush->ulStyle & PB_STYLE_RELEASE_NOTIFY))
            {
                pPush->pfnOnClick(pWidget);
            }

            //
            // See if auto-repeat is enabled for this widget.
            //
            if(pPush->ulStyle & PB_STYLE_AUTO_REPEAT)
            {
                //
                // If the pointer was just pressed, reset the auto-repeat
                // count.
                //
                if(ulMsg == WIDGET_MSG_PTR_DOWN)
                {
                    pPush->ulAutoRepeatCount = 0;
                }

                //
                // See if the pointer was moved.
                //
                else if(ulMsg == WIDGET_MSG_PTR_MOVE)
                {
                    //
                    // Increment the auto-repeat count.
                    //
                    pPush->ulAutoRepeatCount++;

                    //
                    // If the auto-repeat count exceeds the auto-repeat delay,
                    // and it is a multiple of the auto-repeat rate, then
                    // call the callback.
                    //
                    if((pPush->ulAutoRepeatCount >=
                        pPush->usAutoRepeatDelay) &&
                       (((pPush->ulAutoRepeatCount -
                          pPush->usAutoRepeatDelay) %
                         pPush->usAutoRepeatRate) == 0))
                    {
                        pPush->pfnOnClick(pWidget);
                    }
                }
            }
        }

        //
        // These coordinates are within the extents of the push button widget.
        //
        return(1);
    }

    //
    // These coordinates are not within the extents of the push button widget.
    //
    return(0);
}

//*****************************************************************************
//
//! Handles messages for a rectangular push button widget.
//!
//! \param pWidget is a pointer to the push button widget.
//! \param ulMsg is the message.
//! \param ulParam1 is the first parameter to the message.
//! \param ulParam2 is the second parameter to the message.
//!
//! This function receives messages intended for this push button widget and
//! processes them accordingly.  The processing of the message varies based on
//! the message in question.
//!
//! Unrecognized messages are handled by calling WidgetDefaultMsgProc().
//!
//! \return Returns a value appropriate to the supplied message.
//
//*****************************************************************************
long
RectangularButtonMsgProc(tWidget *pWidget, unsigned long ulMsg,
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
            RectangularButtonPaint(pWidget);

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
            return(RectangularButtonClick(pWidget, ulMsg, ulParam1, ulParam2));
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
//! Initializes a rectangular push button widget.
//!
//! \param pWidget is a pointer to the push button widget to initialize.
//! \param pDisplay is a pointer to the display on which to draw the push
//! button.
//! \param lX is the X coordinate of the upper left corner of the push button.
//! \param lY is the Y coordinate of the upper left corner of the push button.
//! \param lWidth is the width of the push button.
//! \param lHeight is the height of the push button.
//!
//! This function initializes the provided push button widget so that it will
//! be a rectangular push button.
//!
//! \return None.
//
//*****************************************************************************
void
RectangularButtonInit(tPushButtonWidget *pWidget, const tDisplay *pDisplay,
                      long lX, long lY, long lWidth, long lHeight)
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
    for(ulIdx = 0; ulIdx < sizeof(tPushButtonWidget); ulIdx += 4)
    {
        ((unsigned long *)pWidget)[ulIdx / 4] = 0;
    }

    //
    // Set the size of the push button widget structure.
    //
    pWidget->sBase.lSize = sizeof(tPushButtonWidget);

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
    // Set the extents of this rectangular push button.
    //
    pWidget->sBase.sPosition.sXMin = lX;
    pWidget->sBase.sPosition.sYMin = lY;
    pWidget->sBase.sPosition.sXMax = lX + lWidth - 1;
    pWidget->sBase.sPosition.sYMax = lY + lHeight - 1;

    //
    // Use the rectangular push button message handler to process messages to
    // this push button.
    //
    pWidget->sBase.pfnMsgProc = RectangularButtonMsgProc;
}

//*****************************************************************************
//
//! Draws a circular push button.
//!
//! \param pWidget is a pointer to the push button widget to be drawn.
//!
//! This function draws a circular push button on the display.  This is called
//! in response to a \b #WIDGET_MSG_PAINT message.
//!
//! \return None.
//
//*****************************************************************************
static void
CircularButtonPaint(tWidget *pWidget)
{
    const unsigned char *pucImage;
    tPushButtonWidget *pPush;
    tContext sCtx;
    long lX, lY, lR;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a push button widget pointer.
    //
    pPush = (tPushButtonWidget *)pWidget;

    //
    // Initialize a drawing context.
    //
    GrContextInit(&sCtx, pWidget->pDisplay);

    //
    // Initialize the clipping region based on the extents of this circular
    // push button.
    //
    GrContextClipRegionSet(&sCtx, &(pWidget->sPosition));

    //
    // Get the radius of the circular push button, along with the X and Y
    // coordinates for its center.
    //
    lR = (pWidget->sPosition.sXMax - pWidget->sPosition.sXMin + 1) / 2;
    lX = pWidget->sPosition.sXMin + lR;
    lY = pWidget->sPosition.sYMin + lR;

    //
    // See if the push button fill style is selected.
    //
    if(pPush->ulStyle & PB_STYLE_FILL)
    {
        //
        // Fill the push button with the fill color.
        //
        GrContextForegroundSet(&sCtx, ((pPush->ulStyle & PB_STYLE_PRESSED) ?
                                       pPush->ulPressFillColor :
                                       pPush->ulFillColor));
        GrCircleFill(&sCtx, lX, lY, lR);
    }

    //
    // See if the push button outline style is selected.
    //
    if(pPush->ulStyle & PB_STYLE_OUTLINE)
    {
        //
        // Outline the push button with the outline color.
        //
        GrContextForegroundSet(&sCtx, pPush->ulOutlineColor);
        GrCircleDraw(&sCtx, lX, lY, lR);
    }

    //
    // See if the push button text or image style is selected.
    //
    if(pPush->ulStyle & (PB_STYLE_TEXT | PB_STYLE_IMG))
    {
        //
        // If the push button outline style is selected then shrink the
        // clipping region by one pixel on each side so that the outline is not
        // overwritten by the text or image.
        //
        if(pPush->ulStyle & PB_STYLE_OUTLINE)
        {
            sCtx.sClipRegion.sXMin++;
            sCtx.sClipRegion.sYMin++;
            sCtx.sClipRegion.sXMax--;
            sCtx.sClipRegion.sYMax--;
        }

        //
        // See if the push button image style is selected.
        //
        if(pPush->ulStyle & PB_STYLE_IMG)
        {
            //
            // Set the foreground and background colors to use for 1 BPP
            // images.
            //
            GrContextForegroundSet(&sCtx, pPush->ulTextColor);
            GrContextBackgroundSet(&sCtx,
                                   ((pPush->ulStyle & PB_STYLE_PRESSED) ?
                                    pPush->ulPressFillColor :
                                    pPush->ulFillColor));

            //
            // Get the image to be drawn.
            //
            pucImage = (((pPush->ulStyle & PB_STYLE_PRESSED) &&
                         pPush->pucPressImage) ?
                        pPush->pucPressImage : pPush->pucImage);

            //
            // Draw the image centered in the push button.
            //
            GrImageDraw(&sCtx, pucImage, lX - (GrImageWidthGet(pucImage) / 2),
                        lY - (GrImageHeightGet(pucImage) / 2));
        }

        //
        // See if the push button text style is selected.
        //
        if(pPush->ulStyle & PB_STYLE_TEXT)
        {
            //
            // Draw the text centered in the middle of the push button.
            //
            GrContextFontSet(&sCtx, pPush->pFont);
            GrContextForegroundSet(&sCtx, pPush->ulTextColor);
            GrContextBackgroundSet(&sCtx,
                                   ((pPush->ulStyle & PB_STYLE_PRESSED) ?
                                    pPush->ulPressFillColor :
                                    pPush->ulFillColor));
            GrStringDrawCentered(&sCtx, pPush->pcText, -1, lX, lY,
                                 pPush->ulStyle & PB_STYLE_TEXT_OPAQUE);
        }
    }
}

//*****************************************************************************
//
//! Handles pointer events for a circular push button.
//!
//! \param pWidget is a pointer to the push button widget.
//! \param ulMsg is the pointer event message.
//! \param lX is the X coordinate of the pointer event.
//! \param lY is the Y coordinate of the pointer event.
//!
//! This function processes pointer event messages for a circular push button.
//! This is called in response to a \b #WIDGET_MSG_PTR_DOWN,
//! \b #WIDGET_MSG_PTR_MOVE, and \b #WIDGET_MSG_PTR_UP messages.
//!
//! If the \b #WIDGET_MSG_PTR_UP message is received with a position within the
//! extents of the push button, the push button's OnClick callback function is
//! called.
//!
//! \return Returns 1 if the coordinates are within the extents of the push
//! button and 0 otherwise.
//
//*****************************************************************************
static long
CircularButtonClick(tWidget *pWidget, unsigned long ulMsg, long lX, long lY)
{
    tPushButtonWidget *pPush;
    long lXc, lYc, lR;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a push button widget pointer.
    //
    pPush = (tPushButtonWidget *)pWidget;

    //
    // See if this is a pointer up message.
    //
    if(ulMsg == WIDGET_MSG_PTR_UP)
    {
        //
        // Indicate that this push button is no longer pressed.
        //
        pPush->ulStyle &= ~(PB_STYLE_PRESSED);

        //
        // If filling is enabled for this push button, or if an image is being
        // used and a pressed button image is provided, then redraw the push
        // button to show it in its non-pressed state.
        //
        if((pPush->ulStyle & PB_STYLE_FILL) ||
           ((pPush->ulStyle & PB_STYLE_IMG) && pPush->pucPressImage))
        {
            CircularButtonPaint(pWidget);
        }
    }

    //
    // Get the radius of the circular push button, along with the X and Y
    // coordinates for its center.
    //
    lR = (pWidget->sPosition.sXMax - pWidget->sPosition.sXMin + 1) / 2;
    lXc = pWidget->sPosition.sXMin + lR;
    lYc = pWidget->sPosition.sYMin + lR;

    //
    // See if the given coordinates are within the radius of the push button.
    //
    if((((lX - lXc) * (lX - lXc)) + ((lY - lYc) * (lY - lYc))) <= (lR * lR))
    {
        //
        // See if this is a pointer down message.
        //
        if(ulMsg == WIDGET_MSG_PTR_DOWN)
        {
            //
            // Indicate that this push button is pressed.
            //
            pPush->ulStyle |= PB_STYLE_PRESSED;

            //
            // If filling is enabled for this push button, or if an image is
            // being used and a pressed button image is provided, then redraw
            // the push button to show it in its pressed state.
            //
            if((pPush->ulStyle & PB_STYLE_FILL) ||
               ((pPush->ulStyle & PB_STYLE_IMG) && pPush->pucPressImage))
            {
                CircularButtonPaint(pWidget);
            }
        }

        //
        // See if there is an OnClick callback for this widget.
        //
        if(pPush->pfnOnClick)
        {
            //
            // If the pointer was just pressed or if the pointer was released
            // and this button is set for release notification then call the
            // callback.
            //
            if(((ulMsg == WIDGET_MSG_PTR_DOWN) &&
               !(pPush->ulStyle & PB_STYLE_RELEASE_NOTIFY)) ||
               ((ulMsg == WIDGET_MSG_PTR_UP) &&
                (pPush->ulStyle & PB_STYLE_RELEASE_NOTIFY)))
            {
                pPush->pfnOnClick(pWidget);
            }

            //
            // See if auto-repeat is enabled for this widget.
            //
            if(pPush->ulStyle & PB_STYLE_AUTO_REPEAT)
            {
                //
                // If the pointer was just pressed, reset the auto-repeat
                // count.
                //
                if(ulMsg == WIDGET_MSG_PTR_DOWN)
                {
                    pPush->ulAutoRepeatCount = 0;
                }

                //
                // See if the pointer was moved.
                //
                else if(ulMsg == WIDGET_MSG_PTR_MOVE)
                {
                    //
                    // Increment the auto-repeat count.
                    //
                    pPush->ulAutoRepeatCount++;

                    //
                    // If the auto-repeat count exceeds the auto-repeat delay,
                    // and it is a multiple of the auto-repeat rate, then
                    // call the callback.
                    //
                    if((pPush->ulAutoRepeatCount >=
                        pPush->usAutoRepeatDelay) &&
                       (((pPush->ulAutoRepeatCount -
                          pPush->usAutoRepeatDelay) %
                         pPush->usAutoRepeatRate) == 0))
                    {
                        pPush->pfnOnClick(pWidget);
                    }
                }
            }
        }

        //
        // These coordinates are within the extents of the push button widget.
        //
        return(1);
    }

    //
    // These coordinates are not within the extents of the push button widget.
    //
    return(0);
}

//*****************************************************************************
//
//! Handles messages for a circular push button widget.
//!
//! \param pWidget is a pointer to the push button widget.
//! \param ulMsg is the message.
//! \param ulParam1 is the first parameter to the message.
//! \param ulParam2 is the second parameter to the message.
//!
//! This function receives messages intended for this push button widget and
//! processes them accordingly.  The processing of the message varies based on
//! the message in question.
//!
//! Unrecognized messages are handled by calling WidgetDefaultMsgProc().
//!
//! \return Returns a value appropriate to the supplied message.
//
//*****************************************************************************
long
CircularButtonMsgProc(tWidget *pWidget, unsigned long ulMsg,
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
            CircularButtonPaint(pWidget);

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
            return(CircularButtonClick(pWidget, ulMsg, ulParam1, ulParam2));
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
//! Initializes a circular push button widget.
//!
//! \param pWidget is a pointer to the push button widget to initialize.
//! \param pDisplay is a pointer to the display on which to draw the push
//! button.
//! \param lX is the X coordinate of the upper left corner of the push button.
//! \param lY is the Y coordinate of the upper left corner of the push button.
//! \param lR is the radius of the push button.
//!
//! This function initializes the provided push button widget so that it will
//! be a circular push button.
//!
//! \return None.
//
//*****************************************************************************
void
CircularButtonInit(tPushButtonWidget *pWidget, const tDisplay *pDisplay,
                   long lX, long lY, long lR)
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
    for(ulIdx = 0; ulIdx < sizeof(tPushButtonWidget); ulIdx += 4)
    {
        ((unsigned long *)pWidget)[ulIdx / 4] = 0;
    }

    //
    // Set the size of the push button widget structure.
    //
    pWidget->sBase.lSize = sizeof(tPushButtonWidget);

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
    // Set the extents of this circular push button.
    //
    pWidget->sBase.sPosition.sXMin = lX - lR;
    pWidget->sBase.sPosition.sYMin = lY - lR;
    pWidget->sBase.sPosition.sXMax = lX + lR;
    pWidget->sBase.sPosition.sYMax = lY + lR;

    //
    // Use the circular push button message handler to processes messages to
    // this push button.
    //
    pWidget->sBase.pfnMsgProc = CircularButtonMsgProc;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
