//*****************************************************************************
//
// line.c - Routines for drawing lines.
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
//! Draws a horizontal line.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param lX1 is the X coordinate of one end of the line.
//! \param lX2 is the X coordinate of the other end of the line.
//! \param lY is the Y coordinate of the line.
//!
//! This function draws a horizontal line, taking advantage of the fact that
//! the line is horizontal to draw it more efficiently.  The clipping of the
//! horizontal line to the clipping rectangle is performed within this routine;
//! the display driver's horizontal line routine is used to perform the actual
//! line drawing.
//!
//! \return None.
//
//*****************************************************************************
void
GrLineDrawH(const tContext *pContext, long lX1, long lX2, long lY)
{
    long lTemp;

    //
    // Check the arguments.
    //
    ASSERT(pContext);

    //
    // If the Y coordinate of this line is not in the clipping region, then
    // there is nothing to be done.
    //
    if((lY < pContext->sClipRegion.sYMin) ||
       (lY > pContext->sClipRegion.sYMax))
    {
        return;
    }

    //
    // Swap the X coordinates if the first is larger than the second.
    //
    if(lX1 > lX2)
    {
        lTemp = lX1;
        lX1 = lX2;
        lX2 = lTemp;
    }

    //
    // If the entire line is outside the clipping region, then there is nothing
    // to be done.
    //
    if((lX1 > pContext->sClipRegion.sXMax) ||
       (lX2 < pContext->sClipRegion.sXMin))
    {
        return;
    }

    //
    // Clip the starting coordinate to the left side of the clipping region if
    // required.
    //
    if(lX1 < pContext->sClipRegion.sXMin)
    {
        lX1 = pContext->sClipRegion.sXMin;
    }

    //
    // Clip the ending coordinate to the right side of the clipping region if
    // required.
    //
    if(lX2 > pContext->sClipRegion.sXMax)
    {
        lX2 = pContext->sClipRegion.sXMax;
    }

    //
    // Call the low level horizontal line drawing routine.
    //
    DpyLineDrawH(pContext->pDisplay, lX1, lX2, lY, pContext->ulForeground);
}

//*****************************************************************************
//
//! Draws a vertical line.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param lX is the X coordinate of the line.
//! \param lY1 is the Y coordinate of one end of the line.
//! \param lY2 is the Y coordinate of the other end of the line.
//!
//! This function draws a vertical line, taking advantage of the fact that the
//! line is vertical to draw it more efficiently.  The clipping of the vertical
//! line to the clipping rectangle is performed within this routine; the
//! display driver's vertical line routine is used to perform the actual line
//! drawing.
//!
//! \return None.
//
//*****************************************************************************
void
GrLineDrawV(const tContext *pContext, long lX, long lY1, long lY2)
{
    long lTemp;

    //
    // Check the arguments.
    //
    ASSERT(pContext);

    //
    // If the X coordinate of this line is not within the clipping region, then
    // there is nothing to be done.
    //
    if((lX < pContext->sClipRegion.sXMin) ||
       (lX > pContext->sClipRegion.sXMax))
    {
        return;
    }

    //
    // Swap the Y coordinates if the first is larger than the second.
    //
    if(lY1 > lY2)
    {
        lTemp = lY1;
        lY1 = lY2;
        lY2 = lTemp;
    }

    //
    // If the entire line is out of the clipping region, then there is nothing
    // to be done.
    //
    if((lY1 > pContext->sClipRegion.sYMax) ||
       (lY2 < pContext->sClipRegion.sYMin))
    {
        return;
    }

    //
    // Clip the starting coordinate to the top side of the clipping region if
    // required.
    //
    if(lY1 < pContext->sClipRegion.sYMin)
    {
        lY1 = pContext->sClipRegion.sYMin;
    }

    //
    // Clip the ending coordinate to the bottom side of the clipping region if
    // required.
    //
    if(lY2 > pContext->sClipRegion.sYMax)
    {
        lY2 = pContext->sClipRegion.sYMax;
    }

    //
    // Call the low level vertical line drawing routine.
    //
    DpyLineDrawV(pContext->pDisplay, lX, lY1, lY2, pContext->ulForeground);
}

//*****************************************************************************
//
//! Computes the clipping code used by the Cohen-Sutherland clipping algorithm.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param lX is the X coordinate of the point.
//! \param lY is the Y coordinate of the point.
//!
//! This function computes the clipping code used by the Cohen-Sutherland
//! clipping algorithm.  Clipping is performed by classifying the endpoints of
//! the line based on their relation to the clipping region; this determines
//! those relationships.
//!
//! \return Returns the clipping code.
//
//*****************************************************************************
static long
GrClipCodeGet(const tContext *pContext, long lX, long lY)
{
    long lCode;

    //
    // Initialize the clipping code to zero.
    //
    lCode = 0;

    //
    // Set bit zero of the clipping code if the Y coordinate is above the
    // clipping region.
    //
    if(lY < pContext->sClipRegion.sYMin)
    {
        lCode |= 1;
    }

    //
    // Set bit one of the clipping code if the Y coordinate is below the
    // clipping region.
    //
    if(lY > pContext->sClipRegion.sYMax)
    {
        lCode |= 2;
    }

    //
    // Set bit two of the clipping code if the X coordinate is to the left of
    // the clipping region.
    //
    if(lX < pContext->sClipRegion.sXMin)
    {
        lCode |= 4;
    }

    //
    // Set bit three of the clipping code if the X coordinate is to the right
    // of the clipping region.
    //
    if(lX > pContext->sClipRegion.sXMax)
    {
        lCode |= 8;
    }

    //
    // Return the clipping code.
    //
    return(lCode);
}

//*****************************************************************************
//
//! Clips a line to the clipping region.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param plX1 is the X coordinate of the start of the line.
//! \param plY1 is the Y coordinate of the start of the line.
//! \param plX2 is the X coordinate of the end of the line.
//! \param plY2 is the Y coordinate of the end of the line.
//!
//! This function clips a line to the extents of the clipping region using the
//! Cohen-Sutherland clipping algorithm.  The ends of the line are classified
//! based on their relation to the clipping region, and the codes are used to
//! either trivially accept a line (both end points within the clipping
//! region), trivially reject a line (both end points to one side of the
//! clipping region), or to adjust an endpoint one axis at a time to the edge
//! of the clipping region until the line can either be trivially accepted or
//! trivially rejected.
//!
//! The provided coordinates are modified such that they reside within the
//! extents of the clipping region if the line is not rejected.  If it is
//! rejected, the coordinates may be modified during the process of attempting
//! to clip them.
//!
//! \return Returns one if the clipped line lies within the extent of the
//! clipping region and zero if it does not.
//
//*****************************************************************************
static long
GrLineClip(const tContext *pContext, long *plX1, long *plY1, long *plX2,
           long *plY2)
{
    long lCode, lCode1, lCode2, lX, lY;

    //
    // Compute the clipping codes for the two endpoints of the line.
    //
    lCode1 = GrClipCodeGet(pContext, *plX1, *plY1);
    lCode2 = GrClipCodeGet(pContext, *plX2, *plY2);

    //
    // Loop forever.  This loop will be explicitly broken out of when the line
    // is either trivially accepted or trivially rejected.
    //
    while(1)
    {
        //
        // If both codes are zero, then both points lie within the extent of
        // the clipping region.  In this case, trivally accept the line.
        //
        if((lCode1 == 0) && (lCode2 == 0))
        {
            return(1);
        }

        //
        // If the intersection of the codes is non-zero, then the line lies
        // entirely off one edge of the clipping region.  In this case,
        // trivally reject the line.
        //
        if((lCode1 & lCode2) != 0)
        {
            return(0);
        }

        //
        // Determine the end of the line to move.  The first end of the line is
        // moved until it is within the clipping region, and then the second
        // end of the line is moved until it is also within the clipping
        // region.
        //
        if(lCode1)
        {
            lCode = lCode1;
        }
        else
        {
            lCode = lCode2;
        }

        //
        // See if this end of the line lies above the clipping region.
        //
        if(lCode & 1)
        {
            //
            // Move this end of the line to the intersection of the line and
            // the top of the clipping region.
            //
            lX = (*plX1 + (((*plX2 - *plX1) *
                            (pContext->sClipRegion.sYMin - *plY1)) /
                           (*plY2 - *plY1)));
            lY = pContext->sClipRegion.sYMin;
        }

        //
        // Otherwise, see if this end of the line lies below the clipping
        // region.
        //
        else if(lCode & 2)
        {
            //
            // Move this end of the line to the intersection of the line and
            // the bottom of the clipping region.
            //
            lX = (*plX1 + (((*plX2 - *plX1) *
                            (pContext->sClipRegion.sYMax - *plY1)) /
                           (*plY2 - *plY1)));
            lY = pContext->sClipRegion.sYMax;
        }

        //
        // Otherwise, see if this end of the line lies to the left of the
        // clipping region.
        //
        else if(lCode & 4)
        {
            //
            // Move this end of the line to the intersection of the line and
            // the left side of the clipping region.
            //
            lX = pContext->sClipRegion.sXMin;
            lY = (*plY1 + (((*plY2 - *plY1) *
                            (pContext->sClipRegion.sXMin - *plX1)) /
                           (*plX2 - *plX1)));
        }

        //
        // Otherwise, this end of the line lies to the right of the clipping
        // region.
        //
        else
        {
            //
            // Move this end of the line to the intersection of the line and
            // the right side of the clipping region.
            //
            lX = pContext->sClipRegion.sXMax;
            lY = (*plY1 + (((*plY2 - *plY1) *
                            (pContext->sClipRegion.sXMax - *plX1)) /
                           (*plX2 - *plX1)));
        }

        //
        // See which end of the line just moved.
        //
        if(lCode1)
        {
            //
            // Save the new coordinates for the start of the line.
            //
            *plX1 = lX;
            *plY1 = lY;

            //
            // Recompute the clipping code for the start of the line.
            //
            lCode1 = GrClipCodeGet(pContext, lX, lY);
        }
        else
        {
            //
            // Save the new coordinates for the end of the line.
            //
            *plX2 = lX;
            *plY2 = lY;

            //
            // Recompute the clipping code for the end of the line.
            //
            lCode2 = GrClipCodeGet(pContext, lX, lY);
        }
    }
}

//*****************************************************************************
//
//! Draws a line.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param lX1 is the X coordinate of the start of the line.
//! \param lY1 is the Y coordinate of the start of the line.
//! \param lX2 is the X coordinate of the end of the line.
//! \param lY2 is the Y coordinate of the end of the line.
//!
//! This function draws a line, utilizing GrLineDrawH() and GrLineDrawV() to
//! draw the line as efficiently as possible.  The line is clipped to the
//! clippping rectangle using the Cohen-Sutherland clipping algorithm, and then
//! scan converted using Bresenham's line drawing algorithm.
//!
//! \return None.
//
//*****************************************************************************
void
GrLineDraw(const tContext *pContext, long lX1, long lY1, long lX2, long lY2)
{
    long lError, lDeltaX, lDeltaY, lYStep, bSteep;

    //
    // Check the arguments.
    //
    ASSERT(pContext);

    //
    // See if this is a vertical line.
    //
    if(lX1 == lX2)
    {
        //
        // It is more efficient to avoid Bresenham's algorithm when drawing a
        // vertical line, so use the vertical line routine to draw this line.
        //
        GrLineDrawV(pContext, lX1, lY1, lY2);

        //
        // The line has ben drawn, so return.
        //
        return;
    }

    //
    // See if this is a horizontal line.
    //
    if(lY1 == lY2)
    {
        //
        // It is more efficient to avoid Bresenham's algorithm when drawing a
        // horizontal line, so use the horizontal line routien to draw this
        // line.
        //
        GrLineDrawH(pContext, lX1, lX2, lY1);

        //
        // The line has ben drawn, so return.
        //
        return;
    }

    //
    // Clip this line if necessary, and return without drawing anything if the
    // line does not cross the clipping region.
    //
    if(GrLineClip(pContext, &lX1, &lY1, &lX2, &lY2) == 0)
    {
        return;
    }

    //
    // Determine if the line is steep.  A steep line has more motion in the Y
    // direction than the X direction.
    //
    if(((lY2 > lY1) ? (lY2 - lY1) : (lY1 - lY2)) >
       ((lX2 > lX1) ? (lX2 - lX1) : (lX1 - lX2)))
    {
        bSteep = 1;
    }
    else
    {
        bSteep = 0;
    }

    //
    // If the line is steep, then swap the X and Y coordinates.
    //
    if(bSteep)
    {
        lError = lX1;
        lX1 = lY1;
        lY1 = lError;
        lError = lX2;
        lX2 = lY2;
        lY2 = lError;
    }

    //
    // If the starting X coordinate is larger than the ending X coordinate,
    // then swap the start and end coordinates.
    //
    if(lX1 > lX2)
    {
        lError = lX1;
        lX1 = lX2;
        lX2 = lError;
        lError = lY1;
        lY1 = lY2;
        lY2 = lError;
    }

    //
    // Compute the difference between the start and end coordinates in each
    // axis.
    //
    lDeltaX = lX2 - lX1;
    lDeltaY = (lY2 > lY1) ? (lY2 - lY1) : (lY1 - lY2);

    //
    // Initialize the error term to negative half the X delta.
    //
    lError = -lDeltaX / 2;

    //
    // Determine the direction to step in the Y axis when required.
    //
    if(lY1 < lY2)
    {
        lYStep = 1;
    }
    else
    {
        lYStep = -1;
    }

    //
    // Loop through all the points along the X axis of the line.
    //
    for(; lX1 <= lX2; lX1++)
    {
        //
        // See if this is a steep line.
        //
        if(bSteep)
        {
            //
            // Plot this point of the line, swapping the X and Y coordinates.
            //
            DpyPixelDraw(pContext->pDisplay, lY1, lX1, pContext->ulForeground);
        }
        else
        {
            //
            // Plot this point of the line, using the coordinates as is.
            //
            DpyPixelDraw(pContext->pDisplay, lX1, lY1, pContext->ulForeground);
        }

        //
        // Increment the error term by the Y delta.
        //
        lError += lDeltaY;

        //
        // See if the error term is now greater than zero.
        //
        if(lError > 0)
        {
            //
            // Take a step in the Y axis.
            //
            lY1 += lYStep;

            //
            // Decrement the error term by the X delta.
            //
            lError -= lDeltaX;
        }
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
