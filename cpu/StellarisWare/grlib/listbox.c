//*****************************************************************************
//
// listbox.c - A listbox widget.
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
#include "grlib/listbox.h"

//*****************************************************************************
//
//! \addtogroup listbox_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Make sure that the abs() macro is defined.
//
//*****************************************************************************
#ifndef abs
#define abs(a) (((a) >= 0) ? (a) : (-(a)))
#endif

//*****************************************************************************
//
// Make sure min and max are defined.
//
//*****************************************************************************
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) (((a) < (b)) ? (b) : (a))
#endif

//*****************************************************************************
//
//! Draws the contents of a listbox.
//!
//! \param pWidget is a pointer to the listbox widget to be drawn.
//!
//! This function draws the contents of a listbox on the display.  This is
//! called in response to a \b #WIDGET_MSG_PAINT message.
//!
//! \return None.
//
//*****************************************************************************
static void
ListBoxPaint(tWidget *pWidget)
{
    tListBoxWidget *pListBox;
    tContext sCtx;
    tRectangle sWidgetRect, sLineRect;
    short sHeight;
    long lWidth;
    unsigned short usString;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic widget pointer into a listbox widget pointer.
    //
    pListBox = (tListBoxWidget *)pWidget;

    //
    // Initialize a drawing context.
    //
    GrContextInit(&sCtx, pWidget->pDisplay);
    GrContextFontSet(&sCtx, pListBox->pFont);

    //
    // Initialize the clipping region based on the extents of this listbox.
    //
    sWidgetRect = pWidget->sPosition;
    GrContextClipRegionSet(&sCtx, &sWidgetRect);

    //
    // See if the listbox outline style is selected.
    //
    if(pListBox->ulStyle & LISTBOX_STYLE_OUTLINE)
    {
        //
        // Outline the listbox with the outline color.
        //
        GrContextForegroundSet(&sCtx, pListBox->ulOutlineColor);
        GrRectDraw(&sCtx, &(pWidget->sPosition));

        //
        // Shrink the widget region by one pixel on each side and draw another
        // rectangle, this time in the background color.  This ensures that the
        // text will not interfere with the colored border.
        //
        sWidgetRect.sXMin++;
        sWidgetRect.sYMin++;
        sWidgetRect.sXMax--;
        sWidgetRect.sYMax--;
        GrContextForegroundSet(&sCtx, pListBox->ulBackgroundColor);
        GrRectDraw(&sCtx, &sWidgetRect);

        //
        // Reduce the size of the rectangle by another pixel to get the final
        // area into which we will put the text.
        //
        sWidgetRect.sXMin++;
        sWidgetRect.sYMin++;
        sWidgetRect.sXMax--;
        sWidgetRect.sYMax--;
        GrContextClipRegionSet(&sCtx, &sWidgetRect);
    }

    //
    // Start drawing at the top of the widget.
    //
    sLineRect = sWidgetRect;
    usString = pListBox->usStartEntry;
    sHeight = GrFontHeightGet(pListBox->pFont);

    //
    // Keep drawing until we reach the bottom of the listbox or run out of
    // strings to draw.
    //
    while((sLineRect.sYMin < sWidgetRect.sYMax) &&
          (usString < pListBox->usPopulated))
    {
        //
        // Calculate the rectangle that will enclose this line of text.
        //
        sLineRect.sYMax = sLineRect.sYMin + sHeight - 1;

        //
        // Set foreground and background colors appropriately.
        //
        GrContextBackgroundSet(&sCtx, ((usString == pListBox->sSelected) ?
                               pListBox->ulSelectedBackgroundColor :
                               pListBox->ulBackgroundColor));
        GrContextForegroundSet(&sCtx, ((usString == pListBox->sSelected) ?
                               pListBox->ulSelectedTextColor :
                               pListBox->ulTextColor));

        //
        // Draw the text.
        //
        GrStringDraw(&sCtx, pListBox->ppcText[usString], -1, sLineRect.sXMin,
                     sLineRect.sYMin, 1);

        //
        // Determine the width of the string we just rendered.
        //
        lWidth = GrStringWidthGet(&sCtx, pListBox->ppcText[usString], -1);

        //
        // Do we need to clear the area to the right of the string?
        //
        if(lWidth < (sLineRect.sXMax - sLineRect.sXMin + 1))
        {
            //
            // Yes - we need to fill the right side of this string with
            // background color.
            //
            GrContextForegroundSet(&sCtx, ((usString == pListBox->sSelected) ?
                                   pListBox->ulSelectedBackgroundColor :
                                   pListBox->ulBackgroundColor));
            sLineRect.sXMin += lWidth;
            GrRectFill(&sCtx, &sLineRect);
            sLineRect.sXMin = sWidgetRect.sXMin;
        }

        //
        // Move on to the next string, wrapping if necessary.
        //
        usString++;
        if(usString == pListBox->usMaxEntries)
        {
            usString = 0;
        }
        sLineRect.sYMin += sHeight;

        //
        // If we are wrapping and got back at the oldest entry, we drop out.
        //
        if(usString == pListBox->usOldestEntry)
        {
            break;
        }
    }

    //
    // Fill the remainder of the listbox area with the background color.
    //
    if(sLineRect.sYMin < sWidgetRect.sYMax)
    {
        //
        // Determine the rectangle to be filled.
        //
        sLineRect.sYMax = sWidgetRect.sYMax;

        //
        // Fill the rectangle with the background color.
        //
        GrContextForegroundSet(&sCtx, pListBox->ulBackgroundColor);
        GrRectFill(&sCtx, &sLineRect);
    }
}

//*****************************************************************************
//
// Handles pointer messages for a listbox widget.
//
// \param pListBox is a pointer to the listbox widget.
// \param ulMsg is the message.
// \param lX is the X coordinate of the pointer.
// \param lY is the Y coordinate of the pointer.
//
// This function receives pointer messages intended for this listbox widget
// and processes them accordingly.
//
// \return Returns a value appropriate to the supplied message.
//
//*****************************************************************************
static long
ListBoxPointer(tListBoxWidget *pListBox, unsigned long ulMsg, long lX, long lY)
{
    long lLineNum, lEntry, lVisible, lMaxUp, lMaxDown, lScroll;

    switch(ulMsg)
    {
        //
        // The touchscreen has been pressed.
        //
        case WIDGET_MSG_PTR_DOWN:
        {
            //
            // Is the pointer press within the bounds of this widget?
            //
            if(!GrRectContainsPoint(&(pListBox->sBase.sPosition), lX, lY))
            {
                //
                // This is not a message for us so return 0 to indicate that
                // we did not process it.
                //
                return(0);
            }
            else
            {
                //
                // The pointer was pressed within this control.  Remember the Y
                // coordinate and reset or scrolling flag.
                //
                pListBox->usScrolled = 0;
                pListBox->lPointerY = lY;

                //
                // Return 1 to indicate to the widget manager that we processed
                // the message.  This widget will now receive all pointer move
                // messages until the pointer is released.
                //
                return(1);
            }
        }

        //
        // The touchscreen has been released.
        //
        case WIDGET_MSG_PTR_UP:
        {
            //
            // If the pointer is still within the bounds of the control and
            // we have not scrolled the contents since the last time the
            // pointer was pressed, we assume that this is a tap rather than
            // a drag and select the element that falls beneath the current
            // pointer position.  If the pointer is outside our control, if
            // we have scrolled already or if the control is locked, don't
            // change the selection.
            //
            if((pListBox->usScrolled == 0) &&
               !(pListBox->ulStyle & LISTBOX_STYLE_LOCKED) &&
                GrRectContainsPoint(&(pListBox->sBase.sPosition), lX, lY))
            {
                //
                // It seems we need to change the selected element. What is
                // the display line number that has been clicked on?
                //
                lLineNum = (lY - (long)pListBox->sBase.sPosition.sYMin) /
                           GrFontHeightGet(pListBox->pFont);

                //
                // We now know the location of the click as a number of text
                // lines from the top of the list box.  Now determine what
                // entry is shown there, remembering that the index may wrap.
                //
                lEntry = ((long)pListBox->usStartEntry + lLineNum) %
                         pListBox->usMaxEntries;

                //
                // If this is an unpopulated entry or the current selection,
                // clear the selection.
                //
                if((lEntry >= (long)pListBox->usPopulated) ||
                   (lEntry == (long)pListBox->sSelected))
                {
                    //
                    // Yes - update the selection and force a repaint.
                    //
                    pListBox->sSelected = (short)0xFFFF;
                }
                else
                {
                    //
                    // The pointer was tapped on a valid entry other than the
                    // current selection so change the selection.
                    //
                    pListBox->sSelected = (short)lEntry;
                }

                //
                // Force a repaint of the widget.
                //
                WidgetPaint((tWidget *)pListBox);

                //
                // Tell the client that the selection changed.
                //
                if(pListBox->pfnOnChange)
                {
                    (pListBox->pfnOnChange)((tWidget *)pListBox,
                                            pListBox->sSelected);
                }
            }

            //
            // We process all pointer up messages so return 1 to tell the
            // widget manager this.
            //
            return(1);
        }

        //
        // The pointer is moving while pressed.
        //
        case WIDGET_MSG_PTR_MOVE:
        {
            //
            // How far has the pointer moved vertically from the point where it
            // was pressed or where we last registered a scroll?  lLineNum will
            // be negative for downward scrolling.
            //
            lLineNum = pListBox->lPointerY - lY;

            //
            // If this distance is greater than or equal to the height of a
            // line of text, we need to check to see if we need to scroll the
            // list box contents.
            //
            if(abs(lLineNum) >= GrFontHeightGet(pListBox->pFont))
            {
                //
                // We have to scroll if this is possible.  How many lines can
                // be visible on the display?
                //
                lVisible = (pListBox->sBase.sPosition.sYMax -
                            pListBox->sBase.sPosition.sYMin) /
                            (long)GrFontHeightGet(pListBox->pFont);

                //
                // If we have fewer strings in the listbox than there are lines
                // on the display, scrolling is not possible so give up now.
                //
                if(lVisible > (long)pListBox->usPopulated)
                {
                    return(1);
                }

                //
                // How many lines of scrolling does the latest pointer position
                // indicate?  A negative value implies downward scrolling (i.e.
                // showing earlier strings).
                //
                lScroll = lLineNum / (long)GrFontHeightGet(pListBox->pFont);

                //
                // What is the farthest we could scroll downwards (i.e. moving
                // the pointer towards the bottom of the screen)?  Note - this
                // will be negative or 0.
                //
                lMaxDown = (pListBox->usStartEntry >= pListBox->usOldestEntry) ?
                           (pListBox->usOldestEntry - pListBox->usStartEntry ) :
                           ((pListBox->usOldestEntry - pListBox->usStartEntry) -
                             pListBox->usMaxEntries);

                //
                // What is the farthest we could scroll upwards?  Note - this
                // will be a positive number.
                //
                lMaxUp = ((long)pListBox->usPopulated - lVisible) + lMaxDown;

                //
                // Determine the actual scroll distance given the maximum
                // distances calculated.
                //
                lScroll = min(lScroll, lMaxUp);
                lScroll = max(lScroll, lMaxDown);

                if(lScroll)
                {
                    long lTemp;

                    //
                    // Adjust the start entry appropriately, taking care to handle
                    // the wrap case.  The use of a temporary variable here is
                    // required to work around a compiler bug which resulted in an
                    // invalid value of pListBox->usStartEntry following the
                    // calculation.
                    //
                    lTemp = pListBox->usStartEntry;
                    lTemp += lScroll;
                    lTemp %= (long)pListBox->usMaxEntries;
                    pListBox->usStartEntry = (unsigned short)lTemp;

                    //
                    // Remember that we scrolled.
                    //
                    pListBox->usScrolled = 1;

                    //
                    // Adjust the pointer position we record to take into account
                    // the amount we just scrolled.
                    //
                    pListBox->lPointerY -= (lScroll *
                                            GrFontHeightGet(pListBox->pFont));

                    //
                    // Repaint the contents of the widget.
                    //
                    WidgetPaint((tWidget *)pListBox);
                }
            }

            return(1);
        }
    }

    //
    // We don't handle any other messages so return 0 if we get these.
    //
    return(0);
}

//*****************************************************************************
//
//! Handles messages for a listbox widget.
//!
//! \param pWidget is a pointer to the listbox widget.
//! \param ulMsg is the message.
//! \param ulParam1 is the first parameter to the message.
//! \param ulParam2 is the second parameter to the message.
//!
//! This function receives messages intended for this listbox widget and
//! processes them accordingly.  The processing of the message varies based on
//! the message in question.
//!
//! Unrecognized messages are handled by calling WidgetDefaultMsgProc().
//!
//! \return Returns a value appropriate to the supplied message.
//
//*****************************************************************************
long
ListBoxMsgProc(tWidget *pWidget, unsigned long ulMsg, unsigned long ulParam1,
              unsigned long ulParam2)
{
    tListBoxWidget *pListBox;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Convert the generic pointer to a list box pointer.
    //
    pListBox = (tListBoxWidget *)pWidget;

    //
    // Determine which message is being sent.
    //
    switch(ulMsg)
    {
        //
        // A pointer message has been received.
        //
        case WIDGET_MSG_PTR_DOWN:
        case WIDGET_MSG_PTR_UP:
        case WIDGET_MSG_PTR_MOVE:
            return(ListBoxPointer(pListBox, ulMsg, (long)ulParam1,
                                  (long)ulParam2));

        //
        // The widget paint request has been sent.
        //
        case WIDGET_MSG_PAINT:
        {
            //
            // Handle the widget paint request.
            //
            ListBoxPaint(pWidget);

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
//! Initializes a listbox widget.
//!
//! \param pWidget is a pointer to the listbox widget to initialize.
//! \param pDisplay is a pointer to the display on which to draw the listbox.
//! \param ppcText is a pointer to an array of character pointers which will
//! hold the strings that the listbox displays.
//! \param usMaxEntries provides the total number of entries in the \e ppcText
//! array.
//! \param usPopulatedEntries provides the number of entries in the \e ppcText
//! array which are populated.
//! \param lX is the X coordinate of the upper left corner of the listbox.
//! \param lY is the Y coordinate of the upper left corner of the listbox.
//! \param lWidth is the width of the listbox.
//! \param lHeight is the height of the listbox.
//!
//! This function initializes the provided listbox widget.
//!
//! \return None.
//
//*****************************************************************************
void
ListBoxInit(tListBoxWidget *pWidget, const tDisplay *pDisplay,
            const char **ppcText, unsigned short usMaxEntries,
            unsigned short usPopulatedEntries, long lX, long lY, long lWidth,
            long lHeight)
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
    for(ulIdx = 0; ulIdx < sizeof(tListBoxWidget); ulIdx += 4)
    {
        ((unsigned long *)pWidget)[ulIdx / 4] = 0;
    }

    //
    // Set the size of the listbox widget structure.
    //
    pWidget->sBase.lSize = sizeof(tListBoxWidget);

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
    // Set the extents of this listbox.
    //
    pWidget->sBase.sPosition.sXMin = lX;
    pWidget->sBase.sPosition.sYMin = lY;
    pWidget->sBase.sPosition.sXMax = lX + lWidth - 1;
    pWidget->sBase.sPosition.sYMax = lY + lHeight - 1;

    //
    // Use the listbox message handler to process messages to this listbox.
    //
    pWidget->sBase.pfnMsgProc = ListBoxMsgProc;

    //
    // Initialize some of the widget fields that are not accessible via
    // macros.
    //
    pWidget->ppcText = ppcText;
    pWidget->usMaxEntries = usMaxEntries;
    pWidget->usPopulated = usPopulatedEntries;
    pWidget->sSelected = (short)0xFFFF;
}

//*****************************************************************************
//
//! Adds a line of text to a listbox.
//!
//! \param pListBox is a pointer to the listbox widget that is to receive the
//! new text string.
//! \param pcTxt is a pointer to the string that is to be added to the listbox.
//!
//! This function adds a new string to the listbox.  If the listbox has
//! style \b #LISTBOX_STYLE_WRAP and the current string table is full, this
//! function will discard the oldest string and replace it with the one passed
//! here.  If this style flag is absent, the function will return -1 if no
//! empty entries exist in the string table for the widget.
//!
//! The display is not automatically updated as a result of this function call.
//! An application must call WidgetPaint() to update the display after adding
//! a new string to the listbox.
//!
//! \note To replace the string associated with a particular, existing element
//! in the listbox, use ListBoxTextSet().
//!
//! \return Returns the string table index into which the new string has been
//! placed if successful or -1 if the string table is full and
//! \b #LISTBOX_STYLE_WRAP is not set.
//
//*****************************************************************************
long ListBoxTextAdd(tListBoxWidget *pListBox, const char *pcTxt)
{
    unsigned long ulIndex;

    //
    // Is the list box full?
    //
    if(pListBox->usPopulated == pListBox->usMaxEntries)
    {
        //
        // The box is already full.  If the wrap style is not set, fail
        // the call.
        //
        if(!(pListBox->ulStyle & LISTBOX_STYLE_WRAP))
        {
            //
            // The listbox is full and it is not configured to wrap so we can't
            // add another string to it.
            //
            return(-1);
        }
        else
        {
            //
            // We are wrapping so replace the oldest entry in the box.
            //
            ulIndex = pListBox->usOldestEntry;

            //
            // Check to see if we are displaying the oldest entry and, if so,
            // move the start entry on by one to keep the display order
            // correct.
            //
            if(pListBox->usOldestEntry == pListBox->usStartEntry)
            {
                pListBox->usStartEntry++;
                if(pListBox->usStartEntry == pListBox->usMaxEntries)
                {
                    pListBox->usStartEntry = 0;
                }
            }

            //
            // The new oldest entry is the next one.  Update the index and
            // take care to wrap if we reach the end of the string table.
            //
            pListBox->usOldestEntry++;
            if(pListBox->usOldestEntry == pListBox->usMaxEntries)
            {
                pListBox->usOldestEntry = 0;
            }
        }
    }
    else
    {
        //
        // The listbox is not full so add the new string to the first free
        // slot in the string table.
        //
        ulIndex = pListBox->usPopulated;
        pListBox->usPopulated++;
    }

    //
    // Save the new string in the appropriate string table entry.
    //
    pListBox->ppcText[ulIndex] = pcTxt;

    //
    // Tell the caller which string table entry was added.
    //
    return((long)ulIndex);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
