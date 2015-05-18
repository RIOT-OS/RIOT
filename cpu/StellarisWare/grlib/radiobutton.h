//*****************************************************************************
//
// radiobutton.h - Prototypes for the radio button widget.
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

#ifndef __RADIOBUTTON_H__
#define __RADIOBUTTON_H__

//*****************************************************************************
//
//! \addtogroup radiobutton_api
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
//! The structure that describes a radio button widget.
//
//*****************************************************************************
typedef struct
{
    //
    //! The generic widget information.
    //
    tWidget sBase;

    //
    //! The style for this radio button.  This is a set of flags defined by
    //! RB_STYLE_xxx.
    //
    unsigned short usStyle;

    //
    //! The size of the radio button itself, not including the text and/or
    //! image that accompanies it (in other words, the size of the actual
    //! circle that is filled or unfilled).
    //
    unsigned short usCircleSize;

    //
    //! The 24-bit RGB color used to fill this radio button, if RB_STYLE_FILL
    //! is selected, and to use as the background color if RB_STYLE_TEXT_OPAQUE
    //! is selected.
    //
    unsigned long ulFillColor;

    //
    //! The 24-bit RGB color used to outline this radio button, if
    //! RB_STYLE_OUTLINE is selected.
    //
    unsigned long ulOutlineColor;

    //
    //! The 24-bit RGB color used to draw text on this radio button, if
    //! RB_STYLE_TEXT is selected.
    //
    unsigned long ulTextColor;

    //
    //! The font used to draw the radio button text, if RB_STYLE_TEXT is
    //! selected.
    //
    const tFont *pFont;

    //
    //! A pointer to the text to draw on this radio button, if RB_STYLE_TEXT is
    //! selected.
    //
    const char *pcText;

    //
    //! A pointer to the image to be drawn onto this radio button, if
    //! RB_STYLE_IMG is selected.
    //
    const unsigned char *pucImage;

    //
    //! A pointer to the function to be called when the radio button is
    //! pressed.  This function is called when the state of the radio button is
    //! changed.
    //
    void (*pfnOnChange)(tWidget *pWidget, unsigned long bSelected);
}
tRadioButtonWidget;

//*****************************************************************************
//
//! This flag indicates that the radio button should be outlined.
//
//*****************************************************************************
#define RB_STYLE_OUTLINE        0x0001

//*****************************************************************************
//
//! This flag indicates that the radio button should be filled.
//
//*****************************************************************************
#define RB_STYLE_FILL           0x0002

//*****************************************************************************
//
//! This flag indicates that the radio button should have text drawn on it.
//
//*****************************************************************************
#define RB_STYLE_TEXT           0x0004

//*****************************************************************************
//
//! This flag indicates that the radio button should have an image drawn on it.
//
//*****************************************************************************
#define RB_STYLE_IMG            0x0008

//*****************************************************************************
//
//! This flag indicates that the radio button text should be drawn opaque (in
//! other words, drawing the background pixels as well as the foreground
//! pixels).
//
//*****************************************************************************
#define RB_STYLE_TEXT_OPAQUE    0x0010

//*****************************************************************************
//
//! This flag indicates that the radio button is selected.
//
//*****************************************************************************
#define RB_STYLE_SELECTED       0x0020

//*****************************************************************************
//
//! Declares an initialized radio button widget data structure.
//!
//! \param pParent is a pointer to the parent widget.
//! \param pNext is a pointer to the sibling widget.
//! \param pChild is a pointer to the first child widget.
//! \param pDisplay is a pointer to the display on which to draw the radio
//! button.
//! \param lX is the X coordinate of the upper left corner of the radio button.
//! \param lY is the Y coordinate of the upper left corner of the radio button.
//! \param lWidth is the width of the radio button.
//! \param lHeight is the height of the radio button.
//! \param usStyle is the style to be applied to this radio button.
//! \param usCircleSize is the size of the circle that is filled.
//! \param ulFillColor is the color used to fill in the radio button.
//! \param ulOutlineColor is the color used to outline the radio button.
//! \param ulTextColor is the color used to draw text on the radio button.
//! \param pFont is a pointer to the font to be used to draw text on the radio
//! button.
//! \param pcText is a pointer to the text to draw on this radio button.
//! \param pucImage is a pointer to the image to draw on this radio button.
//! \param pfnOnChange is a pointer to the function that is called when the
//! radio button is pressed.
//!
//! This macro provides an initialized radio button widget data structure,
//! which can be used to construct the widget tree at compile time in global
//! variables (as opposed to run-time via function calls).  This must be
//! assigned to a variable, such as:
//!
//! \verbatim
//!     tRadioButtonWidget g_sRadioButton = RadioButtonStruct(...);
//! \endverbatim
//!
//! Or, in an array of variables:
//!
//! \verbatim
//!     tRadioButtonWidget g_psRadioButtons[] =
//!     {
//!         RadioButtonStruct(...),
//!         RadioButtonStruct(...)
//!     };
//! \endverbatim
//!
//! \e usStyle is the logical OR of the following:
//!
//! - \b #RB_STYLE_OUTLINE to indicate that the radio button should be
//!   outlined.
//! - \b #RB_STYLE_FILL to indicate that the radio button should be filled.
//! - \b #RB_STYLE_TEXT to indicate that the radio button should have text
//!   drawn on it (using \e pFont and \e pcText).
//! - \b #RB_STYLE_IMG to indicate that the radio button should have an image
//!   drawn on it (using \e pucImage).
//! - \b #RB_STYLE_TEXT_OPAQUE to indicate that the radio button text should be
//!   drawn opaque (in other words, drawing the background pixels).
//! - \b #RB_STYLE_SELECTED to indicate that the radio button is selected.
//!
//! \return Nothing; this is not a function.
//
//*****************************************************************************
#define RadioButtonStruct(pParent, pNext, pChild, pDisplay, lX, lY, lWidth, \
                          lHeight, usStyle, usCircleSize, ulFillColor,      \
                          ulOutlineColor, ulTextColor, pFont, pcText,       \
                          pucImage, pfnOnChange)                            \
        {                                                                   \
            {                                                               \
                sizeof(tRadioButtonWidget),                                 \
                (tWidget *)(pParent),                                       \
                (tWidget *)(pNext),                                         \
                (tWidget *)(pChild),                                        \
                pDisplay,                                                   \
                {                                                           \
                    lX,                                                     \
                    lY,                                                     \
                    (lX) + (lWidth) - 1,                                    \
                    (lY) + (lHeight) - 1                                    \
                },                                                          \
                RadioButtonMsgProc                                          \
            },                                                              \
            usStyle,                                                        \
            usCircleSize,                                                   \
            ulFillColor,                                                    \
            ulOutlineColor,                                                 \
            ulTextColor,                                                    \
            pFont,                                                          \
            pcText,                                                         \
            pucImage,                                                       \
            pfnOnChange                                                     \
        }

//*****************************************************************************
//
//! Declares an initialized variable containing a radio button widget data
//! structure.
//!
//! \param sName is the name of the variable to be declared.
//! \param pParent is a pointer to the parent widget.
//! \param pNext is a pointer to the sibling widget.
//! \param pChild is a pointer to the first child widget.
//! \param pDisplay is a pointer to the display on which to draw the radio
//! button.
//! \param lX is the X coordinate of the upper left corner of the radio button.
//! \param lY is the Y coordinate of the upper left corner of the radio button.
//! \param lWidth is the width of the radio button.
//! \param lHeight is the height of the radio button.
//! \param usStyle is the style to be applied to this radio button.
//! \param usCircleSize is the size of the circle that is filled.
//! \param ulFillColor is the color used to fill in the radio button.
//! \param ulOutlineColor is the color used to outline the radio button.
//! \param ulTextColor is the color used to draw text on the radio button.
//! \param pFont is a pointer to the font to be used to draw text on the radio
//! button.
//! \param pcText is a pointer to the text to draw on this radio button.
//! \param pucImage is a pointer to the image to draw on this radio button.
//! \param pfnOnChange is a pointer to the function that is called when the
//! radio button is pressed.
//!
//! This macro provides an initialized radio button widget data structure,
//! which can be used to construct the widget tree at compile time in global
//! variables (as opposed to run-time via function calls).
//!
//! \e usStyle is the logical OR of the following:
//!
//! - \b #RB_STYLE_OUTLINE to indicate that the radio button should be
//!   outlined.
//! - \b #RB_STYLE_FILL to indicate that the radio button should be filled.
//! - \b #RB_STYLE_TEXT to indicate that the radio button should have text
//!   drawn on it (using \e pFont and \e pcText).
//! - \b #RB_STYLE_IMG to indicate that the radio button should have an image
//!   drawn on it (using \e pucImage).
//! - \b #RB_STYLE_TEXT_OPAQUE to indicate that the radio button text should be
//!   drawn opaque (in other words, drawing the background pixels).
//! - \b #RB_STYLE_SELECTED to indicate that the radio button is selected.
//!
//! \return Nothing; this is not a function.
//
//*****************************************************************************
#define RadioButton(sName, pParent, pNext, pChild, pDisplay, lX, lY, lWidth, \
                    lHeight, usStyle, usCircleSize, ulFillColor,             \
                    ulOutlineColor, ulTextColor, pFont, pcText, pucImage,    \
                    pfnOnChange)                                             \
        tRadioButtonWidget sName =                                           \
            RadioButtonStruct(pParent, pNext, pChild, pDisplay, lX, lY,      \
                              lWidth, lHeight, usStyle, usCircleSize,        \
                              ulFillColor, ulOutlineColor, ulTextColor,      \
                              pFont, pcText, pucImage, pfnOnChange)

//*****************************************************************************
//
//! Sets size of the circle to be filled.
//!
//! \param pWidget is a pointer to the radio button widget to modify.
//! \param usSize is the size of the circle, in pixels.
//!
//! This function sets the size of the circle that is drawn as part of the
//! radio button.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonCircleSizeSet(pWidget, usSize) \
        do                                        \
        {                                         \
            tRadioButtonWidget *pW = pWidget;     \
            pW->usCircleSize = usSize;            \
        }                                         \
        while(0)

//*****************************************************************************
//
//! Sets the function to call when this radio button widget is toggled.
//!
//! \param pWidget is a pointer to the radio button widget to modify.
//! \param pfnOnChg is a pointer to the function to call.
//!
//! This function sets the function to be called when this radio button is
//! toggled.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonCallbackSet(pWidget, pfnOnChg) \
        do                                        \
        {                                         \
            tRadioButtonWidget *pW = pWidget;     \
            pW->pfnOnChange = pfnOnChg;           \
        }                                         \
        while(0)

//*****************************************************************************
//
//! Sets the fill color of a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to fill the radio button.
//!
//! This function changes the color used to fill the radio button on the
//! display.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonFillColorSet(pWidget, ulColor) \
        do                                        \
        {                                         \
            tRadioButtonWidget *pW = pWidget;     \
            pW->ulFillColor = ulColor;            \
        }                                         \
        while(0)

//*****************************************************************************
//
//! Disables filling of a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to modify.
//!
//! This function disables the filling of a radio button widget.  The display
//! is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonFillOff(pWidget)           \
        do                                    \
        {                                     \
            tRadioButtonWidget *pW = pWidget; \
            pW->usStyle &= ~(RB_STYLE_FILL);  \
        }                                     \
        while(0)

//*****************************************************************************
//
//! Enables filling of a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to modify.
//!
//! This function enables the filling of a radio button widget.  The display is
//! not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonFillOn(pWidget)            \
        do                                    \
        {                                     \
            tRadioButtonWidget *pW = pWidget; \
            pW->usStyle |= RB_STYLE_FILL;     \
        }                                     \
        while(0)

//*****************************************************************************
//
//! Sets the font for a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to modify.
//! \param pFnt is a pointer to the font to use to draw text on the radio
//! button.
//!
//! This function changes the font used to draw text on the radio button.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonFontSet(pWidget, pFnt)     \
        do                                    \
        {                                     \
            tRadioButtonWidget *pW = pWidget; \
            const tFont *pF = pFnt;           \
            pW->pFont = pF;                   \
        }                                     \
        while(0)

//*****************************************************************************
//
//! Changes the image drawn on a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to be modified.
//! \param pImg is a pointer to the image to draw onto the radio button.
//!
//! This function changes the image that is drawn onto the radio button.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonImageSet(pWidget, pImg)    \
        do                                    \
        {                                     \
            tRadioButtonWidget *pW = pWidget; \
            const unsigned char *pI = pImg;   \
            pW->pucImage = pI;                \
        }                                     \
        while(0)

//*****************************************************************************
//
//! Disables the image on a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to modify.
//!
//! This function disables the drawing of an image on a radio button widget.
//! The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonImageOff(pWidget)          \
        do                                    \
        {                                     \
            tRadioButtonWidget *pW = pWidget; \
            pW->usStyle &= ~(RB_STYLE_IMG);   \
        }                                     \
        while(0)

//*****************************************************************************
//
//! Enables the image on a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to modify.
//!
//! This function enables the drawing of an image on a radio button widget.
//! The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonImageOn(pWidget)           \
        do                                    \
        {                                     \
            tRadioButtonWidget *pW = pWidget; \
            pW->usStyle |= RB_STYLE_IMG;      \
        }                                     \
        while(0)

//*****************************************************************************
//
//! Sets the outline color of a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to outline the radio button.
//!
//! This function changes the color used to outline the radio button on the
//! display.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonOutlineColorSet(pWidget, ulColor) \
        do                                           \
        {                                            \
            tRadioButtonWidget *pW = pWidget;        \
            pW->ulOutlineColor = ulColor;            \
        }                                            \
        while(0)

//*****************************************************************************
//
//! Disables outlining of a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to modify.
//!
//! This function disables the outlining of a radio button widget.  The display
//! is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonOutlineOff(pWidget)          \
        do                                      \
        {                                       \
            tRadioButtonWidget *pW = pWidget;   \
            pW->usStyle &= ~(RB_STYLE_OUTLINE); \
        }                                       \
        while(0)

//*****************************************************************************
//
//! Enables outlining of a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to modify.
//!
//! This function enables the outlining of a radio button widget.  The display
//! is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonOutlineOn(pWidget)         \
        do                                    \
        {                                     \
            tRadioButtonWidget *pW = pWidget; \
            pW->usStyle |= RB_STYLE_OUTLINE;  \
        }                                     \
        while(0)

//*****************************************************************************
//
//! Sets the text color of a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to draw text on the radio
//! button.
//!
//! This function changes the color used to draw text on the radio button on
//! the display.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonTextColorSet(pWidget, ulColor) \
        do                                        \
        {                                         \
            tRadioButtonWidget *pW = pWidget;     \
            pW->ulTextColor = ulColor;            \
        }                                         \
        while(0)

//*****************************************************************************
//
//! Disables the text on a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to modify.
//!
//! This function disables the drawing of text on a radio button widget.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonTextOff(pWidget)           \
        do                                    \
        {                                     \
            tRadioButtonWidget *pW = pWidget; \
            pW->usStyle &= ~(RB_STYLE_TEXT);  \
        }                                     \
        while(0)

//*****************************************************************************
//
//! Enables the text on a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to modify.
//!
//! This function enables the drawing of text on a radio button widget.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonTextOn(pWidget)            \
        do                                    \
        {                                     \
            tRadioButtonWidget *pW = pWidget; \
            pW->usStyle |= RB_STYLE_TEXT;     \
        }                                     \
        while(0)

//*****************************************************************************
//
//! Disables opaque text on a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to modify.
//!
//! This function disables the use of opaque text on this radio button.  When
//! not using opaque text, only the foreground pixels of the text are drawn on
//! the screen, allowing the previously drawn pixels (such as the radio button
//! image) to show through the text.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonTextOpaqueOff(pWidget)           \
        do                                          \
        {                                           \
            tRadioButtonWidget *pW = pWidget;       \
            pW->usStyle &= ~(RB_STYLE_TEXT_OPAQUE); \
        }                                           \
        while(0)

//*****************************************************************************
//
//! Enables opaque text on a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to modify.
//!
//! This function enables the use of opaque text on this radio button.  When
//! using opaque text, both the foreground and background pixels of the text
//! are drawn on the screen, blocking out the previously drawn pixels.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonTextOpaqueOn(pWidget)         \
        do                                       \
        {                                        \
            tRadioButtonWidget *pW = pWidget;    \
            pW->usStyle |= RB_STYLE_TEXT_OPAQUE; \
        }                                        \
        while(0)

//*****************************************************************************
//
//! Changes the text drawn on a radio button widget.
//!
//! \param pWidget is a pointer to the radio button widget to be modified.
//! \param pcTxt is a pointer to the text to draw onto the radio button.
//!
//! This function changes the text that is drawn onto the radio button.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define RadioButtonTextSet(pWidget, pcTxt)    \
        do                                    \
        {                                     \
            tRadioButtonWidget *pW = pWidget; \
            const char *pcT = pcTxt;          \
            pW->pcText = pcT;                 \
        }                                     \
        while(0)

//*****************************************************************************
//
// Prototypes for the radio button widget APIs.
//
//*****************************************************************************
extern long RadioButtonMsgProc(tWidget *pWidget, unsigned long ulMsg,
                               unsigned long ulParam1, unsigned long ulParam2);
extern void RadioButtonInit(tRadioButtonWidget *pWidget,
                            const tDisplay *pDisplay, long lX, long lY,
                            long lWidth, long lHeight);

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

#endif // __RADIOBUTTON_H__
