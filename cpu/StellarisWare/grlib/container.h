//*****************************************************************************
//
// container.h - Prototypes for the container widget.
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

#ifndef __CONTAINER_H__
#define __CONTAINER_H__

//*****************************************************************************
//
//! \addtogroup container_api
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
//! The structure that describes a container widget.
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
    //! CTR_STYLE_xxx.
    //
    unsigned long ulStyle;

    //
    //! The 24-bit RGB color used to fill this container widget, if
    //! CTR_STYLE_FILL is selected, and to use as the background color if
    //! CTR_STYLE_TEXT_OPAQUE is selected.
    //
    unsigned long ulFillColor;

    //
    //! The 24-bit RGB color used to outline this container widget, if
    //! CTR_STYLE_OUTLINE is selected.
    //
    unsigned long ulOutlineColor;

    //
    //! The 24-bit RGB color used to draw text on this container widget, if
    //! CTR_STYLE_TEXT is selected.
    //
    unsigned long ulTextColor;

    //
    //! A pointer to the font used to render the container text, if
    //! CTR_STYLE_TEXT is selected.
    //
    const tFont *pFont;

    //
    //! A pointer to the text to draw on this container widget, if
    //! CTR_STYLE_TEXT is selected.
    //
    const char *pcText;
}
tContainerWidget;

//*****************************************************************************
//
//! This flag indicates that the container widget should be outlined.
//
//*****************************************************************************
#define CTR_STYLE_OUTLINE       0x00000001

//*****************************************************************************
//
//! This flag indicates that the container widget should be filled.
//
//*****************************************************************************
#define CTR_STYLE_FILL          0x00000002

//*****************************************************************************
//
//! This flag indicates that the container widget should have text drawn on it.
//
//*****************************************************************************
#define CTR_STYLE_TEXT          0x00000004

//*****************************************************************************
//
//! This flag indicates that the container text should be drawn opaque (in
//! other words, drawing the background pixels as well as the foreground
//! pixels).
//
//*****************************************************************************
#define CTR_STYLE_TEXT_OPAQUE   0x00000008

//*****************************************************************************
//
//! This flag indicates that the container text should be drawn centered
//! within the width of the container.
//
//*****************************************************************************
#define CTR_STYLE_TEXT_CENTER   0x00000010

//*****************************************************************************
//
//! Declares an initialized container widget data structure.
//!
//! \param pParent is a pointer to the parent widget.
//! \param pNext is a pointer to the sibling widget.
//! \param pChild is a pointer to the first child widget.
//! \param pDisplay is a pointer to the display on which to draw the container
//! widget.
//! \param lX is the X coordinate of the upper left corner of the container
//! widget.
//! \param lY is the Y coordinate of the upper left corner of the container
//! widget.
//! \param lWidth is the width of the container widget.
//! \param lHeight is the height of the container widget.
//! \param ulStyle is the style to be applied to the container widget.
//! \param ulFillColor is the color used to fill in the container widget.
//! \param ulOutlineColor is the color used to outline the container widget.
//! \param ulTextColor is the color used to draw text on the container widget.
//! \param pFont is a pointer to the font to be used to draw text on the
//! container widget.
//! \param pcText is a pointer to the text to draw on the container widget.
//!
//! This macro provides an initialized container widget data structure, which
//! can be used to construct the widget tree at compile time in global
//! variables (as opposed to run-time via function calls).  This must be
//! assigned to a variable, such as:
//!
//! \verbatim
//!     tContainerWidget g_sContainer = ContainerStruct(...);
//! \endverbatim
//!
//! Or, in an array of variables:
//!
//! \verbatim
//!     tContainerWidget g_psContainers[] =
//!     {
//!         ContainerStruct(...),
//!         ContainerStruct(...)
//!     };
//! \endverbatim
//!
//! \e ulStyle is the logical OR of the following:
//!
//! - \b #CTR_STYLE_OUTLINE to indicate that the container widget should be
//!   outlined.
//! - \b #CTR_STYLE_FILL to indicate that the container widget should be
//!   filled.
//! - \b #CTR_STYLE_TEXT to indicate that the container widget should have text
//!   drawn on it (using \e pFont and \e pcText).
//! - \b #CTR_STYLE_TEXT_OPAQUE to indicate that the container widget text
//!   should be drawn opaque (in other words, drawing the background pixels).
//! - \b #CTR_STYLE_TEXT_CENTER to indicate that the container widget text
//!   should be drawn centered horizontally.
//!
//! \return Nothing; this is not a function.
//
//*****************************************************************************
#define ContainerStruct(pParent, pNext, pChild, pDisplay, lX, lY, lWidth, \
                        lHeight, ulStyle, ulFillColor, ulOutlineColor,    \
                        ulTextColor, pFont, pcText)                       \
        {                                                                 \
            {                                                             \
                sizeof(tContainerWidget),                                 \
                (tWidget *)(pParent),                                     \
                (tWidget *)(pNext),                                       \
                (tWidget *)(pChild),                                      \
                pDisplay,                                                 \
                {                                                         \
                    lX,                                                   \
                    lY,                                                   \
                    (lX) + (lWidth) - 1,                                  \
                    (lY) + (lHeight) - 1                                  \
                },                                                        \
                ContainerMsgProc                                          \
            },                                                            \
            ulStyle,                                                      \
            ulFillColor,                                                  \
            ulOutlineColor,                                               \
            ulTextColor,                                                  \
            pFont,                                                        \
            pcText                                                        \
        }

//*****************************************************************************
//
//! Declares an initialized variable containing a container widget data
//! structure.
//!
//! \param sName is the name of the variable to be declared.
//! \param pParent is a pointer to the parent widget.
//! \param pNext is a pointer to the sibling widget.
//! \param pChild is a pointer to the first child widget.
//! \param pDisplay is a pointer to the display on which to draw the container
//! widget.
//! \param lX is the X coordinate of the upper left corner of the container
//! widget.
//! \param lY is the Y coordinate of the upper left corner of the container
//! widget.
//! \param lWidth is the width of the container widget.
//! \param lHeight is the height of the container widget.
//! \param ulStyle is the style to be applied to the container widget.
//! \param ulFillColor is the color used to fill in the container widget.
//! \param ulOutlineColor is the color used to outline the container widget.
//! \param ulTextColor is the color used to draw text on the container widget.
//! \param pFont is a pointer to the font to be used to draw text on the
//! container widget.
//! \param pcText is a poitner to the text to draw on the container widget.
//!
//! This macro provides an initialized container widget data structure, which
//! can be used to construct the widget tree at compile time in global
//! variables (as opposed to run-time via function calls).
//!
//! \e ulStyle is the logical OR of the following:
//!
//! - \b #CTR_STYLE_OUTLINE to indicate that the container widget should be
//!   outlined.
//! - \b #CTR_STYLE_FILL to indicate that the container widget should be
//!   filled.
//! - \b #CTR_STYLE_TEXT to indicate that the container widget should have text
//!   drawn on it (using \e pFont and \e pcText).
//! - \b #CTR_STYLE_TEXT_OPAQUE to indicate that the container widget text
//!   should be drawn opaque (in other words, drawing the background pixels).
//! - \b #CTR_STYLE_TEXT_CENTER to indicate that the container widget text
//!   should be drawn centered horizontally.
//!
//! \return Nothing; this is not a function.
//
//*****************************************************************************
#define Container(sName, pParent, pNext, pChild, pDisplay, lX, lY, lWidth,    \
                  lHeight, ulStyle, ulFillColor, ulOutlineColor, ulTextColor, \
                  pFont, pcText)                                              \
        tContainerWidget sName =                                              \
            ContainerStruct(pParent, pNext, pChild, pDisplay, lX, lY, lWidth, \
                            lHeight, ulStyle, ulFillColor, ulOutlineColor,    \
                            ulTextColor, pFont, pcText)

//*****************************************************************************
//
//! Sets the fill color of a container widget.
//!
//! \param pWidget is a pointer to the container widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to fill the container widget.
//!
//! This function changes the color used to fill the container widget on the
//! display.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerFillColorSet(pWidget, ulColor) \
        do                                      \
        {                                       \
            tContainerWidget *pW = pWidget;     \
            pW->ulFillColor = ulColor;          \
        }                                       \
        while(0)

//*****************************************************************************
//
//! Disables filling of a container widget.
//!
//! \param pWidget is a pointer to the container widget to modify.
//!
//! This function disables the filling of a container widget.  The display is
//! not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerFillOff(pWidget)             \
        do                                    \
        {                                     \
            tContainerWidget *pW = pWidget;   \
            pW->ulStyle &= ~(CTR_STYLE_FILL); \
        }                                     \
        while(0)

//*****************************************************************************
//
//! Enables filling of a container widget.
//!
//! \param pWidget is a pointer to the container widget to modify.
//!
//! This function enables the filling of a container widget.  The display is
//! not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerFillOn(pWidget)            \
        do                                  \
        {                                   \
            tContainerWidget *pW = pWidget; \
            pW->ulStyle |= CTR_STYLE_FILL;  \
        }                                   \
        while(0)

//*****************************************************************************
//
//! Sets the font for a container widget.
//!
//! \param pWidget is a pointer to the container widget to modify.
//! \param pFnt is a pointer to the font to use to draw text on the container
//! widget.
//!
//! This function changes the font used to draw text on the container widget.
//! The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerFontSet(pWidget, pFnt)     \
        do                                  \
        {                                   \
            tContainerWidget *pW = pWidget; \
            const tFont *pF = pFnt;         \
            pW->pFont = pF;                 \
        }                                   \
        while(0)

//*****************************************************************************
//
//! Sets the outline color of a container widget.
//!
//! \param pWidget is a pointer to the container widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to outline the container
//! widget.
//!
//! This function changes the color used to outline the container widget on the
//! display.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerOutlineColorSet(pWidget, ulColor) \
        do                                         \
        {                                          \
            tContainerWidget *pW = pWidget;        \
            pW->ulOutlineColor = ulColor;          \
        }                                          \
        while(0)

//*****************************************************************************
//
//! Disables outlining of a container widget.
//!
//! \param pWidget is a pointer to the container widget to modify.
//!
//! This function disables the outlining of a container widget.  The display is
//! not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerOutlineOff(pWidget)             \
        do                                       \
        {                                        \
            tContainerWidget *pW = pWidget;      \
            pW->ulStyle &= ~(CTR_STYLE_OUTLINE); \
        }                                        \
        while(0)

//*****************************************************************************
//
//! Enables outlining of a container widget.
//!
//! \param pWidget is a pointer to the container widget to modify.
//!
//! This function enables the outlining of a container widget.  The display is
//! not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerOutlineOn(pWidget)           \
        do                                    \
        {                                     \
            tContainerWidget *pW = pWidget;   \
            pW->ulStyle |= CTR_STYLE_OUTLINE; \
        }                                     \
        while(0)

//*****************************************************************************
//
//! Disables the centering of text on a container widget.
//!
//! \param pWidget is a pointer to the container widget to modify.
//!
//! This function disables the centering of text on a container widget.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerTextCenterOff(pWidget)              \
        do                                           \
        {                                            \
            tContainerWidget *pW = pWidget;          \
            pW->ulStyle &= ~(CTR_STYLE_TEXT_CENTER); \
        }                                            \
        while(0)

//*****************************************************************************
//
//! Enables the centering of text on a container widget.
//!
//! \param pWidget is a pointer to the container widget to modify.
//!
//! This function enables the centering of text on a container widget.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerTextCenterOn(pWidget)            \
        do                                        \
        {                                         \
            tContainerWidget *pW = pWidget;       \
            pW->ulStyle |= CTR_STYLE_TEXT_CENTER; \
        }                                         \
        while(0)

//*****************************************************************************
//
//! Sets the text color of a container widget.
//!
//! \param pWidget is a pointer to the container widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to draw text on the
//! container widget.
//!
//! This function changes the color used to draw text on the container widget
//! on the display.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerTextColorSet(pWidget, ulColor) \
        do                                      \
        {                                       \
            tContainerWidget *pW = pWidget;     \
            pW->ulTextColor = ulColor;          \
        }                                       \
        while(0)

//*****************************************************************************
//
//! Disables the text on a container widget.
//!
//! \param pWidget is a pointer to the container widget to modify.
//!
//! This function disables the drawing of text on a container widget.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerTextOff(pWidget)             \
        do                                    \
        {                                     \
            tContainerWidget *pW = pWidget;   \
            pW->ulStyle &= ~(CTR_STYLE_TEXT); \
        }                                     \
        while(0)

//*****************************************************************************
//
//! Enables the text on a container widget.
//!
//! \param pWidget is a pointer to the container widget to modify.
//!
//! This function enables the drawing of text on a container widget.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerTextOn(pWidget)            \
        do                                  \
        {                                   \
            tContainerWidget *pW = pWidget; \
            pW->ulStyle |= CTR_STYLE_TEXT;  \
        }                                   \
        while(0)

//*****************************************************************************
//
//! Disables opaque text on a container widget.
//!
//! \param pWidget is a pointer to the container widget to modify.
//!
//! This function disables the use of opaque text on this container widget.
//! When not using opaque text, only the foreground pixels of the text are
//! drawn on the screen, allowing the previously drawn pixels (such as the
//! background) to show through the text.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerTextOpaqueOff(pWidget)              \
        do                                           \
        {                                            \
            tContainerWidget *pW = pWidget;          \
            pW->ulStyle &= ~(CTR_STYLE_TEXT_OPAQUE); \
        }                                            \
        while(0)

//*****************************************************************************
//
//! Enables opaque text on a container widget.
//!
//! \param pWidget is a pointer to the container widget to modify.
//!
//! This function enables the use of opaque text on this container widget.
//! When using opaque text, both the foreground and background pixels of the
//! text are drawn on the screen, blocking out the previously drawn pixels.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerTextOpaqueOn(pWidget)            \
        do                                        \
        {                                         \
            tContainerWidget *pW = pWidget;       \
            pW->ulStyle |= CTR_STYLE_TEXT_OPAQUE; \
        }                                         \
        while(0)

//*****************************************************************************
//
//! Changes the text drawn on a container widget.
//!
//! \param pWidget is a pointer to the container widget to be modified.
//! \param pcTxt is a pointer to the text to draw onto the container widget.
//!
//! This function changes the text that is drawn onto the container widget.
//! The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define ContainerTextSet(pWidget, pcTxt)    \
        do                                  \
        {                                   \
            tContainerWidget *pW = pWidget; \
            const char *pcT = pcTxt;        \
            pW->pcText = pcT;               \
        }                                   \
        while(0)

//*****************************************************************************
//
// Prototypes for the container widget APIs.
//
//*****************************************************************************
extern long ContainerMsgProc(tWidget *pWidget, unsigned long ulMsg,
                             unsigned long ulParam1, unsigned long ulParam2);
extern void ContainerInit(tContainerWidget *pWidget, const tDisplay *pDisplay,
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

#endif // __CONTAINER_H__
