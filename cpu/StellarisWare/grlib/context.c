//*****************************************************************************
//
// context.c - Routines for handling drawing contexts.
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

#ifndef GRLIB_REMOVE_WIDE_FONT_SUPPORT
//*****************************************************************************
//
// A pointer to the application-passed structure containing default values for
// various text rendering properties.
//
//*****************************************************************************
static tGrLibDefaults const *g_psGrLibDefaults;

//
// Default text mapping functions.  This table allows the use of 8 bit encoded
// source text with either Unicode encoded wide fonts or legacy fonts
// which are assumed to contain ISO8859-1 text (ASCII + western European).
//
const tCodePointMap g_psDefaultCodePointMapTable[] =
{
    {CODEPAGE_ISO8859_1, CODEPAGE_UNICODE, GrMapISO8859_1_Unicode},
    {CODEPAGE_ISO8859_1, CODEPAGE_ISO8859_1, GrMapISO8859_1_Unicode}
};

#define NUM_DEFAULT_CODEPOINT_MAPS (sizeof(g_psDefaultCodePointMapTable) /    \
                                    sizeof(tCodePointMap))

//*****************************************************************************
//
//! Initializes graphics library default text rendering values.
//!
//! \param pDefaults points to a structure containing default values to use.
//!
//! This function allows an application to set global default values that the
//! graphics library will use when initializing any graphics context.  These
//! values set the source text codepage, the rendering function to use for
//! strings and mapping functions used to allow extraction of the correct
//! glyphs from fonts.
//!
//! If this function is not called by an application, the graphics library
//! assumes that text strings are ISO8859-1 encoded and that the default string
//! renderer is used.
//!
//! \return None.
//
//*****************************************************************************
void
GrLibInit(const tGrLibDefaults *pDefaults)
{
    ASSERT(pDefaults);

    //
    // Remember the pointer to the defaults structure.
    //
    g_psGrLibDefaults = pDefaults;
}

#endif

//*****************************************************************************
//
//! Initializes a drawing context.
//!
//! \param pContext is a pointer to the drawing context to initialize.
//! \param pDisplay is a pointer to the tDisplayInfo structure that describes
//! the display driver to use.
//!
//! This function initializes a drawing context, preparing it for use.  The
//! provided display driver will be used for all subsequent graphics
//! operations, and the default clipping region will be set to the extent of
//! the screen.
//!
//! \return None.
//
//*****************************************************************************
void
GrContextInit(tContext *pContext, const tDisplay *pDisplay)
{
    //
    // Check the arguments.
    //
    ASSERT(pContext);
    ASSERT(pDisplay);

    //
    // Set the size of the context.
    //
    pContext->lSize = sizeof(tContext);

    //
    // Save the pointer to the display.
    //
    pContext->pDisplay = pDisplay;

    //
    // Initialize the extent of the clipping region to the extents of the
    // screen.
    //
    pContext->sClipRegion.sXMin = 0;
    pContext->sClipRegion.sYMin = 0;
    pContext->sClipRegion.sXMax = pDisplay->usWidth - 1;
    pContext->sClipRegion.sYMax = pDisplay->usHeight - 1;

    //
    // Provide a default color and font.
    //
    pContext->ulForeground = 0;
    pContext->ulBackground = 0;
    pContext->pFont = 0;

#ifndef GRLIB_REMOVE_WIDE_FONT_SUPPORT
    //
    // Set defaults for all text rendering options.
    //
    if(g_psGrLibDefaults)
    {
        pContext->pfnStringRenderer = g_psGrLibDefaults->pfnStringRenderer;
        pContext->pCodePointMapTable = g_psGrLibDefaults->pCodePointMapTable;
        pContext->usCodepage = g_psGrLibDefaults->usCodepage;
        pContext->ucNumCodePointMaps = g_psGrLibDefaults->ucNumCodePointMaps;
        pContext->ucReserved = g_psGrLibDefaults->ucReserved;
    }
    else
    {
        pContext->pfnStringRenderer = GrDefaultStringRenderer;
        pContext->pCodePointMapTable = g_psDefaultCodePointMapTable;
        pContext->usCodepage = CODEPAGE_ISO8859_1;
        pContext->ucNumCodePointMaps = NUM_DEFAULT_CODEPOINT_MAPS;
        pContext->ucReserved = 0;
    }
    pContext->ucCodePointMap = 0;
#endif
}

//*****************************************************************************
//
//! Sets the extents of the clipping region.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param pRect is a pointer to the structure containing the extents of the
//! clipping region.
//!
//! This function sets the extents of the clipping region.  The clipping region
//! is not allowed to exceed the extents of the screen, but may be a portion of
//! the screen.
//!
//! The supplied coordinate are inclusive; \e sXMin of 1 and \e sXMax of 1 will
//! define a clipping region that will display only the pixels in the X = 1
//! column.  A consequence of this is that the clipping region must contain
//! at least one row and one column.
//!
//! \return None.
//
//*****************************************************************************
void
GrContextClipRegionSet(tContext *pContext, tRectangle *pRect)
{
    unsigned long ulW, ulH;

    //
    // Check the arguments.
    //
    ASSERT(pContext);
    ASSERT(pRect);

    //
    // Get the width and height of the display.
    //
    ulW = DpyWidthGet(pContext->pDisplay);
    ulH = DpyHeightGet(pContext->pDisplay);

    //
    // Set the extents of the clipping region, forcing them to reside within
    // the extents of the screen.
    //
    pContext->sClipRegion.sXMin = ((pRect->sXMin < 0) ? 0 :
                                   ((pRect->sXMin >= ulW) ? (ulW - 1) :
                                    pRect->sXMin));
    pContext->sClipRegion.sYMin = ((pRect->sYMin < 0) ? 0 :
                                   ((pRect->sYMin >= ulH) ? (ulH - 1) :
                                    pRect->sYMin));
    pContext->sClipRegion.sXMax = ((pRect->sXMax < 0) ? 0 :
                                   ((pRect->sXMax >= ulW) ? (ulW - 1) :
                                    pRect->sXMax));
    pContext->sClipRegion.sYMax = ((pRect->sYMax < 0) ? 0 :
                                   ((pRect->sYMax >= ulH) ? (ulH - 1) :
                                    pRect->sYMax));
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
