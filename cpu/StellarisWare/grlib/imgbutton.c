//*****************************************************************************
//
// imgbutton.c - An image-based button widget.
//
// Copyright (c) 2009-2012 Texas Instruments Incorporated.  All rights reserved.
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
#include "grlib/imgbutton.h"

//*****************************************************************************
//
//! \addtogroup imgbutton_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
//! Draws an image button.
//!
//! \param pWidget is a pointer to the image button widget to be drawn.
//!
//! This function draws a rectangular image button on the display.  This is
//! called in response to a \b #WIDGET_MSG_PAINT message.
//!
//! \return None.
//
//*****************************************************************************
static void
ImageButtonPaint(tWidget *pWidget)
{
    const unsigned char *pucImage;
    tImageButtonWidget *pPush;
    tContext sCtx;
    long lX, lY;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a image button widget pointer.
    //
    pPush = (tImageButtonWidget *)pWidget;

    //
    // Initialize a drawing context.
    //
    GrContextInit(&sCtx, pWidget->pDisplay);

    //
    // Initialize the clipping region based on the extents of this rectangular
    // image button.
    //
    GrContextClipRegionSet(&sCtx, &(pWidget->sPosition));

    //
    // Compute the center of the image button.
    //
    lX = (pWidget->sPosition.sXMin +
          ((pWidget->sPosition.sXMax - pWidget->sPosition.sXMin + 1) / 2));
    lY = (pWidget->sPosition.sYMin +
          ((pWidget->sPosition.sYMax - pWidget->sPosition.sYMin + 1) / 2));

    //
    // Do we need to fill the widget background with a color?
    //
    if(pPush->ulStyle & IB_STYLE_FILL)
    {
        //
        // Yes. Set the appropriate color depending upon whether or not
        // the widget is currently pressed.
        //
        GrContextForegroundSet(&sCtx,
                               ((pPush->ulStyle & IB_STYLE_PRESSED) ?
                                pPush->ulPressedColor :
                                pPush->ulBackgroundColor));
        GrRectFill(&sCtx, &(pWidget->sPosition));
    }

    //
    // Set the foreground and background colors to use for 1 BPP
    // images and text
    //
    GrContextForegroundSet(&sCtx, pPush->ulForegroundColor);
    GrContextBackgroundSet(&sCtx,
                           ((pPush->ulStyle & IB_STYLE_PRESSED) ?
                            pPush->ulPressedColor :
                            pPush->ulBackgroundColor));

    //
    // Do we need to draw the background image?
    //
    if(!(pPush->ulStyle & IB_STYLE_IMAGE_OFF))
    {
        //
        // Get the background image to be drawn.
        //
        pucImage = ((pPush->ulStyle & IB_STYLE_PRESSED) ?
                    pPush->pucPressImage : pPush->pucImage);

        //
        // Draw the image centered in the image button.
        //
        GrImageDraw(&sCtx, pucImage, lX - (GrImageWidthGet(pucImage) / 2),
                    lY - (GrImageHeightGet(pucImage) / 2));
    }

    //
    // Adjust the drawing position if the button is pressed.
    //
    lX += ((pPush->ulStyle & IB_STYLE_PRESSED) ? pPush->sXOffset : 0);
    lY += ((pPush->ulStyle & IB_STYLE_PRESSED) ? pPush->sYOffset : 0);

    //
    // If there is a keycap image and it is not disabled, center this on the
    // top of the button, applying any offset defined if the button is
    // currently pressed.
    //
    if(pPush->pucKeycapImage && !(pPush->ulStyle & IB_STYLE_KEYCAP_OFF))
    {
        //
        // Draw the keycap image.
        //
        GrImageDraw(&sCtx, pPush->pucKeycapImage,
                    lX - (GrImageWidthGet(pPush->pucKeycapImage) / 2),
                    lY - (GrImageHeightGet(pPush->pucKeycapImage) / 2));
    }

    //
    // See if the button text style is selected.
    //
    if(pPush->ulStyle & IB_STYLE_TEXT)
    {
        //
        // Draw the text centered in the middle of the button with offset
        // applied if the button is currently pressed.
        //
        GrContextFontSet(&sCtx, pPush->pFont);
        GrStringDrawCentered(&sCtx, pPush->pcText, -1, lX, lY, 0);
    }
}

//*****************************************************************************
//
//! Handles pointer events for a rectangular image button.
//!
//! \param pWidget is a pointer to the image button widget.
//! \param ulMsg is the pointer event message.
//! \param lX is the X coordinate of the pointer event.
//! \param lY is the Y coordinate of the pointer event.
//!
//! This function processes pointer event messages for a rectangular push
//! button.  This is called in response to a \b #WIDGET_MSG_PTR_DOWN,
//! \b #WIDGET_MSG_PTR_MOVE, and \b #WIDGET_MSG_PTR_UP messages.
//!
//! If the \b #WIDGET_MSG_PTR_UP message is received with a position within the
//! extents of the image button, the image button's OnClick callback function is
//! called.
//!
//! \return Returns 1 if the coordinates are within the extents of the push
//! button and 0 otherwise.
//
//*****************************************************************************
static long
ImageButtonClick(tWidget *pWidget, unsigned long ulMsg, long lX, long lY)
{
    tImageButtonWidget *pPush;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a image button widget pointer.
    //
    pPush = (tImageButtonWidget *)pWidget;

    //
    // See if this is a pointer up message.
    //
    if(ulMsg == WIDGET_MSG_PTR_UP)
    {
        //
        // Indicate that this image button is no longer pressed.
        //
        pPush->ulStyle &= ~(IB_STYLE_PRESSED);

        //
        // Redraw the button in the released state.
        //
        ImageButtonPaint(pWidget);

        //
        // If the pointer is still within the button bounds, and it is a
        // release notify button, call the notification function here.
        //
        if(GrRectContainsPoint(&pWidget->sPosition, lX, lY) &&
           (pPush->ulStyle & IB_STYLE_RELEASE_NOTIFY) && pPush->pfnOnClick)
        {
            pPush->pfnOnClick(pWidget);
        }
    }

    //
    // See if the given coordinates are within the extents of the image button.
    //
    if(GrRectContainsPoint(&pWidget->sPosition, lX, lY))
    {
        //
        // See if this is a pointer down message.
        //
        if(ulMsg == WIDGET_MSG_PTR_DOWN)
        {
            //
            // Indicate that this image button is pressed.
            //
            pPush->ulStyle |= IB_STYLE_PRESSED;

            //
            // Draw the button in the pressed state.
            //
            ImageButtonPaint(pWidget);
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
               !(pPush->ulStyle & IB_STYLE_RELEASE_NOTIFY))
            {
                pPush->pfnOnClick(pWidget);
            }

            //
            // See if auto-repeat is enabled for this widget.
            //
            if(pPush->ulStyle & IB_STYLE_AUTO_REPEAT)
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
        // These coordinates are within the extents of the image button widget.
        //
        return(1);
    }

    //
    // These coordinates are not within the extents of the image button widget.
    //
    return(0);
}

//*****************************************************************************
//
//! Handles messages for an image button widget.
//!
//! \param pWidget is a pointer to the image button widget.
//! \param ulMsg is the message.
//! \param ulParam1 is the first parameter to the message.
//! \param ulParam2 is the second parameter to the message.
//!
//! This function receives messages intended for this image button widget and
//! processes them accordingly.  The processing of the message varies based on
//! the message in question.
//!
//! Unrecognized messages are handled by calling WidgetDefaultMsgProc().
//!
//! \return Returns a value appropriate to the supplied message.
//
//*****************************************************************************
long
ImageButtonMsgProc(tWidget *pWidget, unsigned long ulMsg,
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
            ImageButtonPaint(pWidget);

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
            return(ImageButtonClick(pWidget, ulMsg, ulParam1, ulParam2));
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
//! Initializes an image button widget.
//!
//! \param pWidget is a pointer to the image button widget to initialize.
//! \param pDisplay is a pointer to the display on which to draw the push
//! button.
//! \param lX is the X coordinate of the upper left corner of the image button.
//! \param lY is the Y coordinate of the upper left corner of the image button.
//! \param lWidth is the width of the image button.
//! \param lHeight is the height of the image button.
//!
//! This function initializes the provided image button widget.
//!
//! \return None.
//
//*****************************************************************************
void
ImageButtonInit(tImageButtonWidget *pWidget, const tDisplay *pDisplay,
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
    for(ulIdx = 0; ulIdx < sizeof(tImageButtonWidget); ulIdx += 4)
    {
        ((unsigned long *)pWidget)[ulIdx / 4] = 0;
    }

    //
    // Set the size of the image button widget structure.
    //
    pWidget->sBase.lSize = sizeof(tImageButtonWidget);

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
    // Set the extents of this rectangular image button.
    //
    pWidget->sBase.sPosition.sXMin = lX;
    pWidget->sBase.sPosition.sYMin = lY;
    pWidget->sBase.sPosition.sXMax = lX + lWidth - 1;
    pWidget->sBase.sPosition.sYMax = lY + lHeight - 1;

    //
    // Use the rectangular image button message handler to process messages to
    // this image button.
    //
    pWidget->sBase.pfnMsgProc = ImageButtonMsgProc;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
