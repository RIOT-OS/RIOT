//*****************************************************************************
//
// checkbox.h - Prototypes for the check box widget.
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

#ifndef __CHECKBOX_H__
#define __CHECKBOX_H__

//*****************************************************************************
//
//! \addtogroup checkbox_api
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
//! The structure that describes a check box widget.
//
//*****************************************************************************
typedef struct
{
    //
    //! The generic widget information.
    //
    tWidget sBase;

    //
    //! The style for this check box.  This is a set of flags defined by
    //! CB_STYLE_xxx.
    //
    unsigned short usStyle;

    //
    //! The size of the check box itself, not including the text and/or image
    //! that accompanies it (in other words, the size of the actual box that is
    //! checked or unchecked).
    //
    unsigned short usBoxSize;

    //
    //! The 24-bit RGB color used to fill this check box, if CB_STYLE_FILL is
    //! selected, and to use as the background color if CB_STYLE_TEXT_OPAQUE is
    //! selected.
    //
    unsigned long ulFillColor;

    //
    //! The 24-bit RGB color used to outline this check box, if
    //! CB_STYLE_OUTLINE is selected.
    //
    unsigned long ulOutlineColor;

    //
    //! The 24-bit RGB color used to draw text on this check box, if
    //! CB_STYLE_TEXT is selected.
    //
    unsigned long ulTextColor;

    //
    //! The font used to draw the check box text, if CB_STYLE_TEXT is selected.
    //
    const tFont *pFont;

    //
    //! A pointer to the text to draw on this check box, if CB_STYLE_TEXT is
    //! selected.
    //
    const char *pcText;

    //
    //! A pointer to the image to be drawn onto this check box, if CB_STYLE_IMG
    //! is selected.
    //
    const unsigned char *pucImage;

    //
    //! A pointer to the function to be called when the check box is pressed.
    //! This function is called when the state of the check box is changed.
    //
    void (*pfnOnChange)(tWidget *pWidget, unsigned long bSelected);
}
tCheckBoxWidget;

//*****************************************************************************
//
//! This flag indicates that the check box should be outlined.
//
//*****************************************************************************
#define CB_STYLE_OUTLINE        0x0001

//*****************************************************************************
//
//! This flag indicates that the check box should be filled.
//
//*****************************************************************************
#define CB_STYLE_FILL           0x0002

//*****************************************************************************
//
//! This flag indicates that the check box should have text drawn on it.
//
//*****************************************************************************
#define CB_STYLE_TEXT           0x0004

//*****************************************************************************
//
//! This flag indicates that the check box should have an image drawn on it.
//
//*****************************************************************************
#define CB_STYLE_IMG            0x0008

//*****************************************************************************
//
//! This flag indicates that the check box text should be drawn opaque (in
//! other words, drawing the background pixels as well as the foreground
//! pixels).
//
//*****************************************************************************
#define CB_STYLE_TEXT_OPAQUE    0x0010

//*****************************************************************************
//
//! This flag indicates that the check box is selected.
//
//*****************************************************************************
#define CB_STYLE_SELECTED       0x0020

//*****************************************************************************
//
//! Declares an initialized check box widget data structure.
//!
//! \param pParent is a pointer to the parent widget.
//! \param pNext is a pointer to the sibling widget.
//! \param pChild is a pointer to the first child widget.
//! \param pDisplay is a pointer to the display on which to draw the check box.
//! \param lX is the X coordinate of the upper left corner of the check box.
//! \param lY is the Y coordinate of the upper left corner of the check box.
//! \param lWidth is the width of the check box.
//! \param lHeight is the height of the check box.
//! \param usStyle is the style to be applied to this check box.
//! \param usBoxSize is the size of the box that is checked.
//! \param ulFillColor is the color used to fill in the check box.
//! \param ulOutlineColor is the color used to outline the check box.
//! \param ulTextColor is the color used to draw text on the check box.
//! \param pFont is a pointer to the font to be used to draw text on the check
//! box.
//! \param pcText is a pointer to the text to draw on this check box.
//! \param pucImage is a pointer to the image to draw on this check box.
//! \param pfnOnChange is a pointer to the function that is called when the
//! check box is pressed.
//!
//! This macro provides an initialized check box widget data structure, which
//! can be used to construct the widget tree at compile time in global
//! variables (as opposed to run-time via function calls).  This must be
//! assigned to a variable, such as:
//!
//! \verbatim
//!     tCheckBoxWidget g_sCheckBox = CheckBoxStruct(...);
//! \endverbatim
//!
//! Or, in an array of variables:
//!
//! \verbatim
//!     tCheckBoxWidget g_psCheckBoxes[] =
//!     {
//!         CheckBoxStruct(...),
//!         CheckBoxStruct(...)
//!     };
//! \endverbatim
//!
//! \e usStyle is the logical OR of the following:
//!
//! - \b #CB_STYLE_OUTLINE to indicate that the check box should be outlined.
//! - \b #CB_STYLE_FILL to indicate that the check box should be filled.
//! - \b #CB_STYLE_TEXT to indicate that the check box should have text drawn
//!   on it (using \e pFont and \e pcText).
//! - \b #CB_STYLE_IMG to indicate that the check box should have an image
//!   drawn on it (using \e pucImage).
//! - \b #CB_STYLE_TEXT_OPAQUE to indicate that the check box text should be
//!   drawn opaque (in other words, drawing the background pixels).
//! - \b #CB_STYLE_SELECTED to indicate that the check box is selected.
//!
//! \return Nothing; this is not a function.
//
//*****************************************************************************
#define CheckBoxStruct(pParent, pNext, pChild, pDisplay, lX, lY, lWidth,     \
                       lHeight, usStyle, usBoxSize, ulFillColor,             \
                       ulOutlineColor, ulTextColor, pFont, pcText, pucImage, \
                       pfnOnChange)                                          \
        {                                                                    \
            {                                                                \
                sizeof(tCheckBoxWidget),                                     \
                (tWidget *)(pParent),                                        \
                (tWidget *)(pNext),                                          \
                (tWidget *)(pChild),                                         \
                pDisplay,                                                    \
                {                                                            \
                    lX,                                                      \
                    lY,                                                      \
                    (lX) + (lWidth) - 1,                                     \
                    (lY) + (lHeight) - 1                                     \
                },                                                           \
                CheckBoxMsgProc                                              \
            },                                                               \
            usStyle,                                                         \
            usBoxSize,                                                       \
            ulFillColor,                                                     \
            ulOutlineColor,                                                  \
            ulTextColor,                                                     \
            pFont,                                                           \
            pcText,                                                          \
            pucImage,                                                        \
            pfnOnChange                                                      \
        }

//*****************************************************************************
//
//! Declares an initialized variable containing a check box widget data
//! structure.
//!
//! \param sName is the name of the variable to be declared.
//! \param pParent is a pointer to the parent widget.
//! \param pNext is a pointer to the sibling widget.
//! \param pChild is a pointer to the first child widget.
//! \param pDisplay is a pointer to the display on which to draw the check box.
//! \param lX is the X coordinate of the upper left corner of the check box.
//! \param lY is the Y coordinate of the upper left corner of the check box.
//! \param lWidth is the width of the check box.
//! \param lHeight is the height of the check box.
//! \param usStyle is the style to be applied to this check box.
//! \param usBoxSize is the size of the box that is checked.
//! \param ulFillColor is the color used to fill in the check box.
//! \param ulOutlineColor is the color used to outline the check box.
//! \param ulTextColor is the color used to draw text on the check box.
//! \param pFont is a pointer to the font to be used to draw text on the check
//! box.
//! \param pcText is a pointer to the text to draw on this check box.
//! \param pucImage is a pointer to the image to draw on this check box.
//! \param pfnOnChange is a pointer to the function that is called when the
//! check box is pressed.
//!
//! This macro provides an initialized check box widget data structure, which
//! can be used to construct the widget tree at compile time in global
//! variables (as opposed to run-time via function calls).
//!
//! \e usStyle is the logical OR of the following:
//!
//! - \b #CB_STYLE_OUTLINE to indicate that the check box should be outlined.
//! - \b #CB_STYLE_FILL to indicate that the check box should be filled.
//! - \b #CB_STYLE_TEXT to indicate that the check box should have text drawn
//!   on it (using \e pFont and \e pcText).
//! - \b #CB_STYLE_IMG to indicate that the check box should have an image
//!   drawn on it (using \e pucImage).
//! - \b #CB_STYLE_TEXT_OPAQUE to indicate that the check box text should be
//!   drawn opaque (in other words, drawing the background pixels).
//! - \b #CB_STYLE_SELECTED to indicate that the check box is selected.
//!
//! \return Nothing; this is not a function.
//
//*****************************************************************************
#define CheckBox(sName, pParent, pNext, pChild, pDisplay, lX, lY, lWidth,    \
                 lHeight, usStyle, usBoxSize, ulFillColor, ulOutlineColor,   \
                 ulTextColor, pFont, pcText, pucImage, pfnOnChange)          \
        tCheckBoxWidget sName =                                              \
            CheckBoxStruct(pParent, pNext, pChild, pDisplay, lX, lY, lWidth, \
                           lHeight, usStyle, usBoxSize, ulFillColor,         \
                           ulOutlineColor, ulTextColor, pFont, pcText,       \
                           pucImage, pfnOnChange)

//*****************************************************************************
//
//! Sets size of the box to be checked.
//!
//! \param pWidget is a pointer to the check box widget to modify.
//! \param usSize is the size of the box, in pixels.
//!
//! This function sets the size of the box that is drawn as part of the check
//! box.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxBoxSizeSet(pWidget, usSize) \
        do                                  \
        {                                   \
            tCheckBoxWidget *pW = pWidget;  \
            pW->usBoxSize = usSize;         \
        }                                   \
        while(0)

//*****************************************************************************
//
//! Sets the function to call when this check box widget is toggled.
//!
//! \param pWidget is a pointer to the check box widget to modify.
//! \param pfnOnChg is a pointer to the function to call.
//!
//! This function sets the function to be called when this check box is
//! toggled.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxCallbackSet(pWidget, pfnOnChg) \
        do                                     \
        {                                      \
            tCheckBoxWidget *pW = pWidget;     \
            pW->pfnOnChange = pfnOnChg;        \
        }                                      \
        while(0)

//*****************************************************************************
//
//! Sets the fill color of a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to fill the check box.
//!
//! This function changes the color used to fill the check box on the display.
//! The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxFillColorSet(pWidget, ulColor) \
        do                                     \
        {                                      \
            tCheckBoxWidget *pW = pWidget;     \
            pW->ulFillColor = ulColor;         \
        }                                      \
        while(0)

//*****************************************************************************
//
//! Disables filling of a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to modify.
//!
//! This function disables the filling of a check box widget.  The display is
//! not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxFillOff(pWidget)             \
        do                                   \
        {                                    \
            tCheckBoxWidget *pW = pWidget;   \
            pW->usStyle &= ~(CB_STYLE_FILL); \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Enables filling of a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to modify.
//!
//! This function enables the filling of a check box widget.  The display is
//! not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxFillOn(pWidget)            \
        do                                 \
        {                                  \
            tCheckBoxWidget *pW = pWidget; \
            pW->usStyle |= CB_STYLE_FILL;  \
        }                                  \
        while(0)

//*****************************************************************************
//
//! Sets the font for a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to modify.
//! \param pFnt is a pointer to the font to use to draw text on the check box.
//!
//! This function changes the font used to draw text on the check box.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxFontSet(pWidget, pFnt)     \
        do                                 \
        {                                  \
            tCheckBoxWidget *pW = pWidget; \
            const tFont *pF = pFnt;        \
            pW->pFont = pF;                \
        }                                  \
        while(0)

//*****************************************************************************
//
//! Changes the image drawn on a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to be modified.
//! \param pImg is a pointer to the image to draw onto the check box.
//!
//! This function changes the image that is drawn onto the check box.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxImageSet(pWidget, pImg)     \
        do                                  \
        {                                   \
            tCheckBoxWidget *pW = pWidget;  \
            const unsigned char *pI = pImg; \
            pW->pucImage = pI;              \
        }                                   \
        while(0)

//*****************************************************************************
//
//! Disables the image on a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to modify.
//!
//! This function disables the drawing of an image on a check box widget.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxImageOff(pWidget)           \
        do                                  \
        {                                   \
            tCheckBoxWidget *pW = pWidget;  \
            pW->usStyle &= ~(CB_STYLE_IMG); \
        }                                   \
        while(0)

//*****************************************************************************
//
//! Enables the image on a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to modify.
//!
//! This function enables the drawing of an image on a check box widget.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxImageOn(pWidget)           \
        do                                 \
        {                                  \
            tCheckBoxWidget *pW = pWidget; \
            pW->usStyle |= CB_STYLE_IMG;   \
        }                                  \
        while(0)

//*****************************************************************************
//
//! Sets the outline color of a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to outline the check box.
//!
//! This function changes the color used to outline the check box on the
//! display.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxOutlineColorSet(pWidget, ulColor) \
        do                                        \
        {                                         \
            tCheckBoxWidget *pW = pWidget;        \
            pW->ulOutlineColor = ulColor;         \
        }                                         \
        while(0)

//*****************************************************************************
//
//! Disables outlining of a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to modify.
//!
//! This function disables the outlining of a check box widget.  The display is
//! not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxOutlineOff(pWidget)             \
        do                                      \
        {                                       \
            tCheckBoxWidget *pW = pWidget;      \
            pW->usStyle &= ~(CB_STYLE_OUTLINE); \
        }                                       \
        while(0)

//*****************************************************************************
//
//! Enables outlining of a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to modify.
//!
//! This function enables the outlining of a check box widget.  The display is
//! not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxOutlineOn(pWidget)           \
        do                                   \
        {                                    \
            tCheckBoxWidget *pW = pWidget;   \
            pW->usStyle |= CB_STYLE_OUTLINE; \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Sets the text color of a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to draw text on the check
//! box.
//!
//! This function changes the color used to draw text on the check box on the
//! display.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxTextColorSet(pWidget, ulColor) \
        do                                     \
        {                                      \
            tCheckBoxWidget *pW = pWidget;     \
            pW->ulTextColor = ulColor;         \
        }                                      \
        while(0)

//*****************************************************************************
//
//! Disables the text on a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to modify.
//!
//! This function disables the drawing of text on a check box widget.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxTextOff(pWidget)             \
        do                                   \
        {                                    \
            tCheckBoxWidget *pW = pWidget;   \
            pW->usStyle &= ~(CB_STYLE_TEXT); \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Enables the text on a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to modify.
//!
//! This function enables the drawing of text on a check box widget.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxTextOn(pWidget)            \
        do                                 \
        {                                  \
            tCheckBoxWidget *pW = pWidget; \
            pW->usStyle |= CB_STYLE_TEXT;  \
        }                                  \
        while(0)

//*****************************************************************************
//
//! Disables opaque text on a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to modify.
//!
//! This function disables the use of opaque text on this check box.  When not
//! using opaque text, only the foreground pixels of the text are drawn on the
//! screen, allowing the previously drawn pixels (such as the check box image)
//! to show through the text.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxTextOpaqueOff(pWidget)              \
        do                                          \
        {                                           \
            tCheckBoxWidget *pW = pWidget;          \
            pW->usStyle &= ~(CB_STYLE_TEXT_OPAQUE); \
        }                                           \
        while(0)

//*****************************************************************************
//
//! Enables opaque text on a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to modify.
//!
//! This function enables the use of opaque text on this check box.  When using
//! opaque text, both the foreground and background pixels of the text are
//! drawn on the screen, blocking out the previously drawn pixels.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxTextOpaqueOn(pWidget)            \
        do                                       \
        {                                        \
            tCheckBoxWidget *pW = pWidget;       \
            pW->usStyle |= CB_STYLE_TEXT_OPAQUE; \
        }                                        \
        while(0)

//*****************************************************************************
//
//! Changes the text drawn on a check box widget.
//!
//! \param pWidget is a pointer to the check box widget to be modified.
//! \param pcTxt is a pointer to the text to draw onto the check box.
//!
//! This function changes the text that is drawn onto the check box.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define CheckBoxTextSet(pWidget, pcTxt)    \
        do                                 \
        {                                  \
            tCheckBoxWidget *pW = pWidget; \
            const char *pcT = pcTxt;       \
            pW->pcText = pcT;              \
        }                                  \
        while(0)

//*****************************************************************************
//
// Prototypes for the check box widget APIs.
//
//*****************************************************************************
extern long CheckBoxMsgProc(tWidget *pWidget, unsigned long ulMsg,
                            unsigned long ulParam1, unsigned long ulParam2);
extern void CheckBoxInit(tCheckBoxWidget *pWidget, const tDisplay *pDisplay,
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

#endif // __CHECKBOX_H__
