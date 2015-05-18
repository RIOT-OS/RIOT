//*****************************************************************************
//
// circle.c - Routines for drawing circles.
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
//! Draws a circle.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param lX is the X coordinate of the center of the circle.
//! \param lY is the Y coordinate of the center of the circle.
//! \param lRadius is the radius of the circle.
//!
//! This function draws a circle, utilizing the Bresenham circle drawing
//! algorithm.  The extent of the circle is from \e lX - \e lRadius to \e lX +
//! \e lRadius and \e lY - \e lRadius to \e lY + \e lRadius, inclusive.
//!
//! \return None.
//
//*****************************************************************************
void
GrCircleDraw(const tContext *pContext, long lX, long lY, long lRadius)
{
    long lA, lB, lD, lX1, lY1;

    //
    // Check the arguments.
    //
    ASSERT(pContext);

    //
    // Initialize the variables that control the Bresenham circle drawing
    // algorithm.
    //
    lA = 0;
    lB = lRadius;
    lD = 3 - (2 * lRadius);

    //
    // Loop until the A delta is greater than the B delta, meaning that the
    // entire circle has been drawn.
    //
    while(lA <= lB)
    {
        //
        // Determine the row when subtracting the A delta.
        //
        lY1 = lY - lA;

        //
        // See if this row is within the clipping region.
        //
        if((lY1 >= pContext->sClipRegion.sYMin) &&
           (lY1 <= pContext->sClipRegion.sYMax))
        {
            //
            // Determine the column when subtracting the B delta.
            //
            lX1 = lX - lB;

            //
            // If this column is within the clipping region, then draw a pixel
            // at that position.
            //
            if((lX1 >= pContext->sClipRegion.sXMin) &&
               (lX1 <= pContext->sClipRegion.sXMax))
            {
                GrPixelDraw(pContext, lX1, lY1);
            }

            //
            // Determine the column when adding the B delta.
            //
            lX1 = lX + lB;

            //
            // If this column is within the clipping region, then draw a pixel
            // at that position.
            //
            if((lX1 >= pContext->sClipRegion.sXMin) &&
               (lX1 <= pContext->sClipRegion.sXMax))
            {
                GrPixelDraw(pContext, lX1, lY1);
            }
        }

        //
        // Determine the row when adding the A delta.
        //
        lY1 = lY + lA;

        //
        // See if this row is within the clipping region, and the A delta is
        // not zero (otherwise, it will be the same row as when the A delta was
        // subtracted).
        //
        if((lY1 >= pContext->sClipRegion.sYMin) &&
           (lY1 <= pContext->sClipRegion.sYMax) &&
           (lA != 0))
        {
            //
            // Determine the column when subtracting the B delta.
            //
            lX1 = lX - lB;

            //
            // If this column is within the clipping region, then draw a pixel
            // at that position.
            //
            if((lX1 >= pContext->sClipRegion.sXMin) &&
               (lX1 <= pContext->sClipRegion.sXMax))
            {
                GrPixelDraw(pContext, lX1, lY1);
            }

            //
            // Determine the column when adding the B delta.
            //
            lX1 = lX + lB;

            //
            // If this column is within the clipping region, then draw a pixel
            // at that position.
            //
            if((lX1 >= pContext->sClipRegion.sXMin) &&
               (lX1 <= pContext->sClipRegion.sXMax))
            {
                GrPixelDraw(pContext, lX1, lY1);
            }
        }

        //
        // Only draw the complementary pixels if the A and B deltas are
        // different (otherwise, they describe the same set of pixels).
        //
        if(lA != lB)
        {
            //
            // Determine the row when subtracting the B delta.
            //
            lY1 = lY - lB;

            //
            // See if this row is within the clipping region.
            //
            if((lY1 >= pContext->sClipRegion.sYMin) &&
               (lY1 <= pContext->sClipRegion.sYMax))
            {
                //
                // Determine the column when subtracting the a delta.
                //
                lX1 = lX - lA;

                //
                // If this column is within the clipping region, then draw a
                // pixel at that position.
                //
                if((lX1 >= pContext->sClipRegion.sXMin) &&
                   (lX1 <= pContext->sClipRegion.sXMax))
                {
                    GrPixelDraw(pContext, lX1, lY1);
                }

                //
                // Only draw the mirrored pixel if the A delta is non-zero
                // (otherwise, it will be the same pixel).
                //
                if(lA != 0)
                {
                    //
                    // Determine the column when adding the A delta.
                    //
                    lX1 = lX + lA;

                    //
                    // If this column is within the clipping region, then draw
                    // a pixel at that position.
                    //
                    if((lX1 >= pContext->sClipRegion.sXMin) &&
                       (lX1 <= pContext->sClipRegion.sXMax))
                    {
                        GrPixelDraw(pContext, lX1, lY1);
                    }
                }
            }

            //
            // Determine the row when adding the B delta.
            //
            lY1 = lY + lB;

            //
            // See if this row is within the clipping region.
            //
            if((lY1 >= pContext->sClipRegion.sYMin) &&
               (lY1 <= pContext->sClipRegion.sYMax))
            {
                //
                // Determine the column when subtracting the A delta.
                //
                lX1 = lX - lA;

                //
                // If this column is within the clipping region, then draw a
                // pixel at that position.
                //
                if((lX1 >= pContext->sClipRegion.sXMin) &&
                   (lX1 <= pContext->sClipRegion.sXMax))
                {
                    GrPixelDraw(pContext, lX1, lY1);
                }

                //
                // Only draw the mirrored pixel if the A delta is non-zero
                // (otherwise, it will be the same pixel).
                //
                if(lA != 0)
                {
                    //
                    // Determine the column when adding the A delta.
                    //
                    lX1 = lX + lA;

                    //
                    // If this column is within the clipping region, then draw
                    // a pixel at that position.
                    //
                    if((lX1 >= pContext->sClipRegion.sXMin) &&
                       (lX1 <= pContext->sClipRegion.sXMax))
                    {
                        GrPixelDraw(pContext, lX1, lY1);
                    }
                }
            }
        }

        //
        // See if the error term is negative.
        //
        if(lD < 0)
        {
            //
            // Since the error term is negative, adjust it based on a move in
            // only the A delta.
            //
            lD += (4 * lA) + 6;
        }
        else
        {
            //
            // Since the error term is non-negative, adjust it based on a move
            // in both the A and B deltas.
            //
            lD += (4 * (lA - lB)) + 10;

            //
            // Decrement the B delta.
            //
            lB -= 1;
        }

        //
        // Increment the A delta.
        //
        lA++;
    }
}

//*****************************************************************************
//
//! Draws a filled circle.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param lX is the X coordinate of the center of the circle.
//! \param lY is the Y coordinate of the center of the circle.
//! \param lRadius is the radius of the circle.
//!
//! This function draws a filled circle, utilizing the Bresenham circle drawing
//! algorithm.  The extent of the circle is from \e lX - \e lRadius to \e lX +
//! \e lRadius and \e lY - \e lRadius to \e lY + \e lRadius, inclusive.
//!
//! \return None.
//
//*****************************************************************************
void
GrCircleFill(const tContext *pContext, long lX, long lY, long lRadius)
{
    long lA, lB, lD, lX1, lX2, lY1;

    //
    // Check the arguments.
    //
    ASSERT(pContext);

    //
    // Initialize the variables that control the Bresenham circle drawing
    // algorithm.
    //
    lA = 0;
    lB = lRadius;
    lD = 3 - (2 * lRadius);

    //
    // Loop until the A delta is greater than the B delta, meaning that the
    // entire circle has been filled.
    //
    while(lA <= lB)
    {
        //
        // Determine the row when subtracting the A delta.
        //
        lY1 = lY - lA;

        //
        // See if this row is within the clipping region.
        //
        if((lY1 >= pContext->sClipRegion.sYMin) &&
           (lY1 <= pContext->sClipRegion.sYMax))
        {
            //
            // Determine the column when subtracting the B delta, and move it
            // to the left edge of the clipping region if it is to the left of
            // the clipping region.
            //
            lX1 = lX - lB;
            if(lX1 < pContext->sClipRegion.sXMin)
            {
                lX1 = pContext->sClipRegion.sXMin;
            }

            //
            // Determine the column when adding the B delta, and move it to the
            // right edge of the clipping region if it is to the right of the
            // clipping region.
            //
            lX2 = lX + lB;
            if(lX2 > pContext->sClipRegion.sXMax)
            {
                lX2 = pContext->sClipRegion.sXMax;
            }

            //
            // Draw a horizontal line if this portion of the circle is within
            // the clipping region.
            //
            if(lX1 <= lX2)
            {
                GrLineDrawH(pContext, lX1, lX2, lY1);
            }
        }

        //
        // Determine the row when adding the A delta.
        //
        lY1 = lY + lA;

        //
        // See if this row is within the clipping region, and the A delta is
        // not zero (otherwise, this describes the same row of the circle).
        //
        if((lY1 >= pContext->sClipRegion.sYMin) &&
           (lY1 <= pContext->sClipRegion.sYMax) &&
           (lA != 0))
        {
            //
            // Determine the column when subtracting the B delta, and move it
            // to the left edge of the clipping region if it is to the left of
            // the clipping region.
            //
            lX1 = lX - lB;
            if(lX1 < pContext->sClipRegion.sXMin)
            {
                lX1 = pContext->sClipRegion.sXMin;
            }

            //
            // Determine the column when adding the B delta, and move it to the
            // right edge of the clipping region if it is to the right of the
            // clipping region.
            //
            lX2 = lX + lB;
            if(lX2 > pContext->sClipRegion.sXMax)
            {
                lX2 = pContext->sClipRegion.sXMax;
            }

            //
            // Draw a horizontal line if this portion of the circle is within
            // the clipping region.
            //
            if(lX1 <= lX2)
            {
                GrLineDrawH(pContext, lX1, lX2, lY1);
            }
        }

        //
        // Only draw the complementary lines if the B delta is about to change
        // and the A and B delta are different (otherwise, they describe the
        // same set of pixels).
        //
        if((lD >= 0) && (lA != lB))
        {
            //
            // Determine the row when subtracting the B delta.
            //
            lY1 = lY - lB;

            //
            // See if this row is within the clipping region.
            //
            if((lY1 >= pContext->sClipRegion.sYMin) &&
               (lY1 <= pContext->sClipRegion.sYMax))
            {
                //
                // Determine the column when subtracting the A delta, and move
                // it to the left edge of the clipping regino if it is to the
                // left of the clipping region.
                //
                lX1 = lX - lA;
                if(lX1 < pContext->sClipRegion.sXMin)
                {
                    lX1 = pContext->sClipRegion.sXMin;
                }

                //
                // Determine the column when adding the A delta, and move it to
                // the right edge of the clipping region if it is to the right
                // of the clipping region.
                //
                lX2 = lX + lA;
                if(lX2 > pContext->sClipRegion.sXMax)
                {
                    lX2 = pContext->sClipRegion.sXMax;
                }

                //
                // Draw a horizontal line if this portion of the circle is
                // within the clipping region.
                //
                if(lX1 <= lX2)
                {
                    GrLineDrawH(pContext, lX1, lX2, lY1);
                }
            }

            //
            // Determine the row when adding the B delta.
            //
            lY1 = lY + lB;

            //
            // See if this row is within the clipping region.
            //
            if((lY1 >= pContext->sClipRegion.sYMin) &&
               (lY1 <= pContext->sClipRegion.sYMax))
            {
                //
                // Determine the column when subtracting the A delta, and move
                // it to the left edge of the clipping region if it is to the
                // left of the clipping region.
                //
                lX1 = lX - lA;
                if(lX1 < pContext->sClipRegion.sXMin)
                {
                    lX1 = pContext->sClipRegion.sXMin;
                }

                //
                // Determine the column when adding the A delta, and move it to
                // the right edge of the clipping region if it is to the right
                // of the clipping region.
                //
                lX2 = lX + lA;
                if(lX2 > pContext->sClipRegion.sXMax)
                {
                    lX2 = pContext->sClipRegion.sXMax;
                }

                //
                // Draw a horizontal line if this portion of the circle is
                // within the clipping region.
                //
                if(lX1 <= lX2)
                {
                    GrLineDrawH(pContext, lX1, lX2, lY1);
                }
            }
        }

        //
        // See if the error term is negative.
        //
        if(lD < 0)
        {
            //
            // Since the error term is negative, adjust it based on a move in
            // only the A delta.
            //
            lD += (4 * lA) + 6;
        }
        else
        {
            //
            // Since the error term is non-negative, adjust it based on a move
            // in both the A and B deltas.
            //
            lD += (4 * (lA - lB)) + 10;

            //
            // Decrement the B delta.
            //
            lB -= 1;
        }

        //
        // Increment the A delta.
        //
        lA++;
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
