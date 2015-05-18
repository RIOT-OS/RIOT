//*****************************************************************************
//
// canvas.h - Prototypes for the canvas widget.
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

#ifndef __CANVAS_H__
#define __CANVAS_H__

//*****************************************************************************
//
//! \addtogroup canvas_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! The structure that describes a canvas widget.
//
//*****************************************************************************
typedef struct
{
    //
    //! The generic widget information.
    //
    tWidget sBase;

    //
    //! The style for this widget.  This is a set of flags defined by
    //! CANVAS_STYLE_xxx.
    //
    unsigned long ulStyle;

    //
    //! The 24-bit RGB color used to fill this canvas, if CANVAS_STYLE_FILL is
    //! selected, and to use as the background color if
    //! CANVAS_STYLE_TEXT_OPAQUE is selected.
    //
    unsigned long ulFillColor;

    //
    //! The 24-bit RGB color used to outline this canvas, if
    //! CANVAS_STYLE_OUTLINE is selected.
    //
    unsigned long ulOutlineColor;

    //
    //! The 24-bit RGB color used to draw text on this canvas, if
    //! CANVAS_STYLE_TEXT is selected.
    //
    unsigned long ulTextColor;

    //
    //! A pointer to the font used to render the canvas text, if
    //! CANVAS_STYLE_TEXT is selected.
    //
    const tFont *pFont;

    //
    //! A pointer to the text to draw on this canvas, if CANVAS_STYLE_TEXT is
    //! selected.
    //
    const char *pcText;

    //
    //! A pointer to the image to be drawn onto this canvas, if
    //! CANVAS_STYLE_IMG is selected.
    //
    const unsigned char *pucImage;

    //
    //! A pointer to the application-supplied drawing function used to draw
    //! onto this canvas, if CANVAS_STYLE_APP_DRAWN is selected.
    //
    void (*pfnOnPaint)(tWidget *pWidget, tContext *pContext);
}
tCanvasWidget;

//*****************************************************************************
//
//! This flag indicates that the canvas should be outlined.
//
//*****************************************************************************
#define CANVAS_STYLE_OUTLINE    0x00000001

//*****************************************************************************
//
//! This flag indicates that the canvas should be filled.
//
//*****************************************************************************
#define CANVAS_STYLE_FILL       0x00000002

//*****************************************************************************
//
//! This flag indicates that the canvas should have text drawn on it.
//
//*****************************************************************************
#define CANVAS_STYLE_TEXT       0x00000004

//*****************************************************************************
//
//! This flag indicates that the canvas should have an image drawn on it.
//
//*****************************************************************************
#define CANVAS_STYLE_IMG        0x00000008

//*****************************************************************************
//
//! This flag indicates that the canvas is drawn using the application-supplied
//! drawing function.
//
//*****************************************************************************
#define CANVAS_STYLE_APP_DRAWN  0x00000010

//*****************************************************************************
//
//! This flag indicates that the canvas text should be drawn opaque (in other
//! words, drawing the background pixels as well as the foreground pixels).
//
//*****************************************************************************
#define CANVAS_STYLE_TEXT_OPAQUE \
                                0x00000020

//*****************************************************************************
//
//! This flag indicates that canvas text should be left-aligned. By default,
//! text is centered in both X and Y within the canvas bounding rectangle.
//
//*****************************************************************************
#define CANVAS_STYLE_TEXT_LEFT \
                                0x00000040

//*****************************************************************************
//
//! This flag indicates that canvas text should be right-aligned. By default,
//! text is centered in both X and Y within the canvas bounding rectangle.
//
//*****************************************************************************
#define CANVAS_STYLE_TEXT_RIGHT \
                                0x00000080

//*****************************************************************************
//
//! This flag indicates that canvas text should be top-aligned. By default,
//! text is centered in both X and Y within the canvas bounding rectangle.
//
//*****************************************************************************
#define CANVAS_STYLE_TEXT_TOP \
                                0x00000100

//*****************************************************************************
//
//! This flag indicates that canvas text should be bottom-aligned. By default,
//! text is centered in both X and Y within the canvas bounding rectangle.
//
//*****************************************************************************
#define CANVAS_STYLE_TEXT_BOTTOM \
                                0x00000200

//*****************************************************************************
//
//! This flag indicates that canvas text should be centered horizontally. By
//! default, text is centered in both X and Y within the canvas bounding
//! rectangle.
//
//*****************************************************************************
#define CANVAS_STYLE_TEXT_HCENTER \
                                0x00000000

//*****************************************************************************
//
//! This flag indicates that canvas text should be centered vertically. By
//! default, text is centered in both X and Y within the canvas bounding
//! rectangle.
//
//*****************************************************************************
#define CANVAS_STYLE_TEXT_VCENTER \
                                0x00000000

//*****************************************************************************
//
// Masks used to extract the text alignment flags from the widget style.
//
//*****************************************************************************
#define CANVAS_STYLE_ALIGN_MASK (CANVAS_STYLE_TEXT_LEFT |   \
                                 CANVAS_STYLE_TEXT_RIGHT |  \
                                 CANVAS_STYLE_TEXT_TOP |    \
                                 CANVAS_STYLE_TEXT_BOTTOM)
#define CANVAS_STYLE_ALIGN_HMASK \
                                (CANVAS_STYLE_TEXT_LEFT |   \
                                 CANVAS_STYLE_TEXT_RIGHT)
#define CANVAS_STYLE_ALIGN_VMASK \
                                (CANVAS_STYLE_TEXT_TOP |    \
                                 CANVAS_STYLE_TEXT_BOTTOM)

//*****************************************************************************
//
//! Declares an initialized canvas widget data structure.
//!
//! \param pParent is a pointer to the parent widget.
//! \param pNext is a pointer to the sibling widget.
//! \param pChild is a pointer to the first child widget.
//! \param pDisplay is a pointer to the display on which to draw the canvas.
//! \param lX is the X coordinate of the upper left corner of the canvas.
//! \param lY is the Y coordinate of the upper left corner of the canvas.
//! \param lWidth is the width of the canvas.
//! \param lHeight is the height of the canvas.
//! \param ulStyle is the style to be applied to the canvas.
//! \param ulFillColor is the color used to fill in the canvas.
//! \param ulOutlineColor is the color used to outline the canvas.
//! \param ulTextColor is the color used to draw text on the canvas.
//! \param pFont is a pointer to the font to be used to draw text on the
//! canvas.
//! \param pcText is a pointer to the text to draw on this canvas.
//! \param pucImage is a pointer to the image to draw on this canvas.
//! \param pfnOnPaint is a pointer to the application function to draw onto
//! this canvas.
//!
//! This macro provides an initialized canvas widget data structure, which can
//! be used to construct the widget tree at compile time in global variables
//! (as opposed to run-time via function calls).  This must be assigned to a
//! variable, such as:
//!
//! \verbatim
//!     tCanvasWidget g_sCanvas = CanvasStruct(...);
//! \endverbatim
//!
//! Or, in an array of variables:
//!
//! \verbatim
//!     tCanvasWidget g_psCanvas[] =
//!     {
//!         CanvasStruct(...),
//!         CanvasStruct(...)
//!     };
//! \endverbatim
//!
//! \e ulStyle is the logical OR of the following:
//!
//! - \b #CANVAS_STYLE_OUTLINE to indicate that the canvas should be outlined.
//! - \b #CANVAS_STYLE_FILL to indicate that the canvas should be filled.
//! - \b #CANVAS_STYLE_TEXT to indicate that the canvas should have text drawn
//!   on it (using \e pFont and \e pcText).
//! - \b #CANVAS_STYLE_IMG to indicate that the canvas should have an image
//!   drawn on it (using \e pucImage).
//! - \b #CANVAS_STYLE_APP_DRAWN to indicate that the canvas should be drawn
//!   with the application-supplied drawing function (using \e pfnOnPaint).
//! - \b #CANVAS_STYLE_TEXT_OPAQUE to indicate that the canvas text should be
//!   drawn opaque (in other words, drawing the background pixels).
//! - \b #CANVAS_STYLE_TEXT_LEFT to indicate that the canvas text should be
//!   left aligned within the widget bounding rectangle.
//! - \b #CANVAS_STYLE_TEXT_HCENTER to indicate that the canvas text should be
//!   horizontally centered within the widget bounding rectangle.
//! - \b #CANVAS_STYLE_TEXT_RIGHT to indicate that the canvas text should be
//!   right aligned within the widget bounding rectangle.
//! - \b #CANVAS_STYLE_TEXT_TOP to indicate that the canvas text should be
//!   top aligned within the widget bounding rectangle.
//! - \b #CANVAS_STYLE_TEXT_VCENTER to indicate that the canvas text should be
//!   vertically centered within the widget bounding rectangle.
//! - \b #CANVAS_STYLE_TEXT_BOTTOM to indicate that the canvas text should be
//!   bottom aligned within the widget bounding rectangle.
//!
//! \return Nothing; this is not a function.
//
//*****************************************************************************
#define CanvasStruct(pParent, pNext, pChild, pDisplay, lX, lY, lWidth, \
                     lHeight, ulStyle, ulFillColor, ulOutlineColor,    \
                     ulTextColor, pFont, pcText, pucImage, pfnOnPaint) \
        {                                                              \
            {                                                          \
                sizeof(tCanvasWidget),                                 \
                (tWidget *)(pParent),                                  \
                (tWidget *)(pNext),                                    \
                (tWidget *)(pChild),                                   \
                pDisplay,                                              \
                {                                                      \
                    lX,                                                \
                    lY,                                                \
                    (lX) + (lWidth) - 1,                               \
                    (lY) + (lHeight) - 1                               \
                },                                                     \
                CanvasMsgProc                                          \
            },                                                         \
            ulStyle,                                                   \
            ulFillColor,                                               \
            ulOutlineColor,                                            \
            ulTextColor,                                               \
            pFont,                                                     \
            pcText,                                                    \
            pucImage,                                                  \
            pfnOnPaint                                                 \
        }

//*****************************************************************************
//
//! Declares an initialized variable containing a canvas widget data structure.
//!
//! \param sName is the name of the variable to be declared.
//! \param pParent is a pointer to the parent widget.
//! \param pNext is a pointer to the sibling widget.
//! \param pChild is a pointer to the first child widget.
//! \param pDisplay is a pointer to the display on which to draw the canvas.
//! \param lX is the X coordinate of the upper left corner of the canvas.
//! \param lY is the Y coordinate of the upper left corner of the canvas.
//! \param lWidth is the width of the canvas.
//! \param lHeight is the height of the canvas.
//! \param ulStyle is the style to be applied to the canvas.
//! \param ulFillColor is the color used to fill in the canvas.
//! \param ulOutlineColor is the color used to outline the canvas.
//! \param ulTextColor is the color used to draw text on the canvas.
//! \param pFont is a pointer to the font to be used to draw text on the
//! canvas.
//! \param pcText is a pointer to the text to draw on this canvas.
//! \param pucImage is a pointer to the image to draw on this canvas.
//! \param pfnOnPaint is a pointer to the application function to draw onto
//! this canvas.
//!
//! This macro declares a variable containing an initialized canvas widget data
//! structure, which can be used to construct the widget tree at compile time
//! in global variables (as opposed to run-time via function calls).
//!
//! \e ulStyle is the logical OR of the following:
//!
//! - \b #CANVAS_STYLE_OUTLINE to indicate that the canvas should be outlined.
//! - \b #CANVAS_STYLE_FILL to indicate that the canvas should be filled.
//! - \b #CANVAS_STYLE_TEXT to indicate that the canvas should have text drawn
//!   on it (using \e pFont and \e pcText).
//! - \b #CANVAS_STYLE_IMG to indicate that the canvas should have an image
//!   drawn on it (using \e pucImage).
//! - \b #CANVAS_STYLE_APP_DRAWN to indicate that the canvas should be drawn
//!   with the application-supplied drawing function (using \e pfnOnPaint).
//! - \b #CANVAS_STYLE_TEXT_OPAQUE to indicate that the canvas text should be
//!   drawn opaque (in other words, drawing the background pixels).
//! - \b #CANVAS_STYLE_TEXT_LEFT to indicate that the canvas text should be
//!   left aligned within the widget bounding rectangle.
//! - \b #CANVAS_STYLE_TEXT_HCENTER to indicate that the canvas text should be
//!   horizontally centered within the widget bounding rectangle.
//! - \b #CANVAS_STYLE_TEXT_RIGHT to indicate that the canvas text should be
//!   right aligned within the widget bounding rectangle.
//! - \b #CANVAS_STYLE_TEXT_TOP to indicate that the canvas text should be
//!   top aligned within the widget bounding rectangle.
//! - \b #CANVAS_STYLE_TEXT_VCENTER to indicate that the canvas text should be
//!   vertically centered within the widget bounding rectangle.
//! - \b #CANVAS_STYLE_TEXT_BOTTOM to indicate that the canvas text should be
//!   bottom aligned within the widget bounding rectangle.
//!
//! \return Nothing; this is not a function.
//
//*****************************************************************************
#define Canvas(sName, pParent, pNext, pChild, pDisplay, lX, lY, lWidth,    \
               lHeight, ulStyle, ulFillColor, ulOutlineColor, ulTextColor, \
               pFont, pcText, pucImage, pfnOnPaint)                        \
        tCanvasWidget sName =                                              \
            CanvasStruct(pParent, pNext, pChild, pDisplay, lX, lY, lWidth, \
                         lHeight, ulStyle, ulFillColor, ulOutlineColor,    \
                         ulTextColor, pFont, pcText, pucImage, pfnOnPaint)

//*****************************************************************************
//
//! Disables application drawing of a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//!
//! This function disables the use of the application callback to draw on a
//! canvas widget.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasAppDrawnOff(pWidget)                    \
        do                                            \
        {                                             \
            tCanvasWidget *pW = pWidget;              \
            pW->ulStyle &= ~(CANVAS_STYLE_APP_DRAWN); \
        }                                             \
        while(0)

//*****************************************************************************
//
//! Enables application drawing of a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//!
//! This function enables the use of the application callback to draw on a
//! canvas widget.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasAppDrawnOn(pWidget)                  \
        do                                         \
        {                                          \
            tCanvasWidget *pW = pWidget;           \
            pW->ulStyle |= CANVAS_STYLE_APP_DRAWN; \
        }                                          \
        while(0)

//*****************************************************************************
//
//! Sets the function to call when this canvas widget is drawn.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//! \param pfnOnPnt is a pointer to the function to call.
//!
//! This function sets the function to be called when this canvas is drawn and
//! \b CANVAS_STYLE_APP_DRAWN is selected.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasCallbackSet(pWidget, pfnOnPnt) \
        do                                   \
        {                                    \
            tCanvasWidget *pW = pWidget;     \
            pW->pfnOnPaint = pfnOnPnt;       \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Sets the fill color of a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to fill the canvas.
//!
//! This function changes the color used to fill the canvas on the display.
//! The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasFillColorSet(pWidget, ulColor) \
        do                                   \
        {                                    \
            tCanvasWidget *pW = pWidget;     \
            pW->ulFillColor = ulColor;       \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Disables filling of a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//!
//! This function disables the filling of a canvas widget.  The display is not
//! updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasFillOff(pWidget)                   \
        do                                       \
        {                                        \
            tCanvasWidget *pW = pWidget;         \
            pW->ulStyle &= ~(CANVAS_STYLE_FILL); \
        }                                        \
        while(0)

//*****************************************************************************
//
//! Enables filling of a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//!
//! This function enables the filling of a canvas widget.  The display is not
//! updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasFillOn(pWidget)                 \
        do                                    \
        {                                     \
            tCanvasWidget *pW = pWidget;      \
            pW->ulStyle |= CANVAS_STYLE_FILL; \
        }                                     \
        while(0)

//*****************************************************************************
//
//! Sets the font for a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//! \param pFnt is a pointer to the font to use to draw text on the canvas.
//!
//! This function changes the font used to draw text on the canvas.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasFontSet(pWidget, pFnt)     \
        do                               \
        {                                \
            tCanvasWidget *pW = pWidget; \
            const tFont *pF = pFnt;      \
            pW->pFont = pF;              \
        }                                \
        while(0)

//*****************************************************************************
//
//! Changes the image drawn on a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to be modified.
//! \param pImg is a pointer to the image to draw onto the canvas.
//!
//! This function changes the image that is drawn onto the canvas.  The display
//! is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasImageSet(pWidget, pImg)       \
        do                                  \
        {                                   \
            tCanvasWidget *pW = pWidget;    \
            const unsigned char *pI = pImg; \
            pW->pucImage = pI;              \
        }                                   \
        while(0)

//*****************************************************************************
//
//! Disables the image on a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//!
//! This function disables the drawing of an image on a canvas widget.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasImageOff(pWidget)                 \
        do                                      \
        {                                       \
            tCanvasWidget *pW = pWidget;        \
            pW->ulStyle &= ~(CANVAS_STYLE_IMG); \
        }                                       \
        while(0)

//*****************************************************************************
//
//! Enables the image on a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//!
//! This function enables the drawing of an image on a canvas widget.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasImageOn(pWidget)               \
        do                                   \
        {                                    \
            tCanvasWidget *pW = pWidget;     \
            pW->ulStyle |= CANVAS_STYLE_IMG; \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Sets the outline color of a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to outline the canvas.
//!
//! This function changes the color used to outline the canvas on the display.
//! The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasOutlineColorSet(pWidget, ulColor) \
        do                                      \
        {                                       \
            tCanvasWidget *pW = pWidget;        \
            pW->ulOutlineColor = ulColor;       \
        }                                       \
        while(0)

//*****************************************************************************
//
//! Disables outlining of a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//!
//! This function disables the outlining of a canvas widget.  The display is
//! not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasOutlineOff(pWidget)                   \
        do                                          \
        {                                           \
            tCanvasWidget *pW = pWidget;            \
            pW->ulStyle &= ~(CANVAS_STYLE_OUTLINE); \
        }                                           \
        while(0)

//*****************************************************************************
//
//! Enables outlining of a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//!
//! This function enables the outlining of a canvas widget.  The display is not
//! updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasOutlineOn(pWidget)                 \
        do                                       \
        {                                        \
            tCanvasWidget *pW = pWidget;         \
            pW->ulStyle |= CANVAS_STYLE_OUTLINE; \
        }                                        \
        while(0)

//*****************************************************************************
//
//! Sets the text color of a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to draw text on the canvas.
//!
//! This function changes the color used to draw text on the canvas on the
//! display.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasTextColorSet(pWidget, ulColor) \
        do                                   \
        {                                    \
            tCanvasWidget *pW = pWidget;     \
            pW->ulTextColor = ulColor;       \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Disables the text on a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//!
//! This function disables the drawing of text on a canvas widget.  The display
//! is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasTextOff(pWidget)                   \
        do                                       \
        {                                        \
            tCanvasWidget *pW = pWidget;         \
            pW->ulStyle &= ~(CANVAS_STYLE_TEXT); \
        }                                        \
        while(0)

//*****************************************************************************
//
//! Enables the text on a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//!
//! This function enables the drawing of text on a canvas widget.  The display
//! is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasTextOn(pWidget)                 \
        do                                    \
        {                                     \
            tCanvasWidget *pW = pWidget;      \
            pW->ulStyle |= CANVAS_STYLE_TEXT; \
        }                                     \
        while(0)

//*****************************************************************************
//
//! Disables opaque text on a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//!
//! This function disables the use of opaque text on this canvas.  When not
//! using opaque text, only the foreground pixels of the text are drawn on the
//! screen, allowing the previously drawn pixels (such as the canvas image) to
//! show through the text.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasTextOpaqueOff(pWidget)                    \
        do                                              \
        {                                               \
            tCanvasWidget *pW = pWidget;                \
            pW->ulStyle &= ~(CANVAS_STYLE_TEXT_OPAQUE); \
        }                                               \
        while(0)

//*****************************************************************************
//
//! Enables opaque text on a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//!
//! This function enables the use of opaque text on this canvas.  When using
//! opaque text, both the foreground and background pixels of the text
//! are drawn on the screen, blocking out the previously drawn pixels.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasTextOpaqueOn(pWidget)                  \
        do                                           \
        {                                            \
            tCanvasWidget *pW = pWidget;             \
            pW->ulStyle |= CANVAS_STYLE_TEXT_OPAQUE; \
        }                                            \
        while(0)

//*****************************************************************************
//
//! Sets the text alignment for a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to modify.
//! \param ulAlign contains the required text alignment setting. This is a
//! logical OR of style values \b #CANVAS_STYLE_TEXT_LEFT, \b
//! #CANVAS_STYLE_TEXT_RIGHT, \b #CANVAS_STYLE_TEXT_HCENTER,
//! \b #CANVAS_STYLE_TEXT_VCENTER, \b #CANVAS_STYLE_TEXT_TOP and
//! \b #CANVAS_STYLE_TEXT_BOTTOM.
//!
//! This function sets the alignment of the text drawn inside the widget.
//! Independent alignment options for horizontal and vertical placement allow
//! the text to be positioned in one of 9 positions within the boinding box
//! of the widget.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasTextAlignment(pWidget, ulAlign)                       \
        do                                                          \
        {                                                           \
            tCanvasWidget *pW = pWidget;                            \
            pW->ulStyle &= ~CANVAS_STYLE_ALIGN_MASK;                \
            pW->ulStyle |= ((ulAlign) & CANVAS_STYLE_ALIGN_MASK);   \
        }                                                           \
        while(0)

//*****************************************************************************
//
//! Changes the text drawn on a canvas widget.
//!
//! \param pWidget is a pointer to the canvas widget to be modified.
//! \param pcTxt is a pointer to the text to draw onto the canvas.
//!
//! This function changes the text that is drawn onto the canvas.  The display
//! is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CanvasTextSet(pWidget, pcTxt)    \
        do                               \
        {                                \
            tCanvasWidget *pW = pWidget; \
            const char *pcT = pcTxt;     \
            pW->pcText = pcT;            \
        }                                \
        while(0)

//*****************************************************************************
//
// Prototypes for the canvas widget APIs.
//
//*****************************************************************************
extern long CanvasMsgProc(tWidget *pWidget, unsigned long ulMsg,
                          unsigned long ulParam1, unsigned long ulParam2);
extern void CanvasInit(tCanvasWidget *pWidget, const tDisplay *pDisplay,
                       long lX, long lY, long lWidth, long lHeight);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

#endif // __CANVAS_H__
