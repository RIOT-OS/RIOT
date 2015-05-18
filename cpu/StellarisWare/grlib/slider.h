//*****************************************************************************
//
// slider.h - Prototypes for the slider widget class.
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

#ifndef __SLIDER_H__
#define __SLIDER_H__

//*****************************************************************************
//
//! \addtogroup slider_api
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
//! The structure that describes a slider widget.
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
    //! SL_STYLE_xxx.
    //
    unsigned long ulStyle;

    //
    //! The 24-bit RGB color used to fill this slider, if SL_STYLE_FILL is
    //! selected, and to use as the background color if SL_STYLE_TEXT_OPAQUE is
    //! selected.
    //
    unsigned long ulFillColor;

    //
    //! The 24-bit RGB color used to fill the background portion of the slider
    //! if SL_STYLE_FILL is selected, and to use as the background color if
    //! SL_STYLE_TEXT_OPAQUE is selected.
    //
    unsigned long ulBackgroundFillColor;

    //
    //! The 24-bit RGB color used to outline this slider, if
    //! SL_STYLE_OUTLINE is selected.
    //
    unsigned long ulOutlineColor;

    //
    //! The 24-bit RGB color used to draw text on the "active" portion of
    //! this slider, if SL_STYLE_TEXT is selected.
    //
    unsigned long ulTextColor;

    //
    //! The 24-bit RGB color used to draw text on the background portion of
    //! this slider, if SL_STYLE_TEXT is selected.
    //
    unsigned long ulBackgroundTextColor;

    //
    //! A pointer to the font used to render the slider text, if
    //! SL_STYLE_TEXT is selected.
    //
    const tFont *pFont;

    //
    //! A pointer to the text to draw on this slider, if SL_STYLE_TEXT is
    //! selected.
    //
    const char *pcText;

    //
    //! A pointer to the image to be drawn onto this slider, if
    //! SL_STYLE_IMG is selected.
    //
    const unsigned char *pucImage;

    //
    //! A pointer to the image to be drawn onto this slider background if
    //! SL_STYLE_BACKG_IMG is selected.
    //
    const unsigned char *pucBackgroundImage;

    //
    //! A pointer to the function to be called when the state of the slider
    //! changes.
    //
    void (*pfnOnChange)(tWidget *pWidget, long lValue);

    //
    //! The value represented by the slider at its zero position.  This
    //! value is returned if a horizontal slider is pulled to the far left or
    //! a vertical slider is pulled to the bottom of widget's bounding
    //! rectangle.
    //
    long lMin;

    //
    //! The value represented by the slider at its maximum position.  This value
    //! is returned if a horizontal slider is pulled to the far right or a
    //! vertical slider is pulled to the top of the widget's bounding
    //! rectangle.
    //
    long lMax;

    //
    //! The current slider value scaled according to the minimum and maximum
    //! values for the control.
    //
    long lValue;

    //
    //! This internal work variable stores the pixel position representing the
    //! current slider value.
    //
    short sPos;
}
tSliderWidget;

//*****************************************************************************
//
//! This flag indicates that the slider should be outlined.
//
//*****************************************************************************
#define SL_STYLE_OUTLINE            0x00000001

//*****************************************************************************
//
//! This flag indicates that the active portion of the slider should be filled.
//
//*****************************************************************************
#define SL_STYLE_FILL               0x00000002

//*****************************************************************************
//
//! This flag indicates that the background portion of the slider should be
//! filled.
//
//*****************************************************************************
#define SL_STYLE_BACKG_FILL         0x00000004

//*****************************************************************************
//
//! This flag indicates that the slider should have text drawn on top of the
//! active portion.
//
//*****************************************************************************
#define SL_STYLE_TEXT               0x00000008

//*****************************************************************************
//
//! This flag indicates that the slider should have text drawn on top of the
//! background portion.
//
//*****************************************************************************
#define SL_STYLE_BACKG_TEXT         0x00000010

//*****************************************************************************
//
//! This flag indicates that the slider should have an image drawn on it.
//
//*****************************************************************************
#define SL_STYLE_IMG                0x00000020

//*****************************************************************************
//
//! This flag indicates that the slider should have an image drawn on its
//! background.
//
//*****************************************************************************
#define SL_STYLE_BACKG_IMG          0x00000040

//*****************************************************************************
//
//! This flag indicates that the slider text should be drawn opaque (in
//! other words, drawing the background pixels as well as the foreground
//! pixels) in the active portion of the slider.
//
//*****************************************************************************
#define SL_STYLE_TEXT_OPAQUE        0x00000080

//*****************************************************************************
//
//! This flag indicates that the slider text should be drawn opaque (in
//! other words, drawing the background pixels as well as the foreground
//! pixels) in the background portion of the slider.
//
//*****************************************************************************
#define SL_STYLE_BACKG_TEXT_OPAQUE  0x00000100

//*****************************************************************************
//
//! This flag indicates that the slider is vertical rather than horizontal.  If
//! the flag is absent, the slider is assumed to operate horizontally with the
//! reported value increasing from left to right.  If set, the reported value
//! increases from the bottom of the widget towards the top.
//
//*****************************************************************************
#define SL_STYLE_VERTICAL           0x00000200

//*****************************************************************************
//
//! This flag causes the slider to ignore pointer input and act as a passive
//! indicator.  An application may set its value and repaint it as normal but
//! its value will not be changed in response to any touchscreen activity.
//
//*****************************************************************************
#define SL_STYLE_LOCKED             0x00000400

//*****************************************************************************
//
//! Declares an initialized slider widget data structure.
//!
//! \param pParent is a pointer to the parent widget.
//! \param pNext is a pointer to the sibling widget.
//! \param pChild is a pointer to the first child widget.
//! \param pDisplay is a pointer to the display on which to draw the slider.
//! \param lX is the X coordinate of the upper left corner of the slider.
//! \param lY is the Y coordinate of the upper left corner of the slider.
//! \param lWidth is the width of the slider.
//! \param lHeight is the height of the slider.
//! \param lMin is the minimum value for the slider (corresponding to the left
//!  or bottom position).
//! \param lMax is the maximum value for the slider (corresponding to the right
//!  or top position).
//! \param lValue is the initial value of the slider.  This must lie in the
//!  range defined by \e lMin and \e lMax.
//! \param ulStyle is the style to be applied to the slider.
//! \param ulFillColor is the color used to fill in the slider.
//! \param ulBackgroundFillColor is the color used to fill the background area
//! of the slider.
//! \param ulOutlineColor is the color used to outline the slider.
//! \param ulTextColor is the color used to draw text on the slider.
//! \param ulBackgroundTextColor is the color used to draw text on the
//! background portion of the slider.
//! \param pFont is a pointer to the font to be used to draw text on the
//! slider.
//! \param pcText is a pointer to the text to draw on this slider.
//! \param pucImage is a pointer to the image to draw on this slider.
//! \param pucBackgroundImage is a pointer to the image to draw on the slider
//! background.
//! \param pfnOnChange is a pointer to the function that is called to notify
//! the application of slider value changes.
//!
//! This macro provides an initialized slider widget data structure, which can
//! be used to construct the widget tree at compile time in global variables
//! (as opposed to run-time via function calls).  This must be assigned to a
//! variable, such as:
//!
//! \verbatim
//!     tSliderWidget g_sSlider = SliderStruct(...);
//! \endverbatim
//!
//! Or, in an array of variables:
//!
//! \verbatim
//!     tSliderWidget g_psSliders[] =
//!     {
//!         SliderStruct(...),
//!         SliderStruct(...)
//!     };
//! \endverbatim
//!
//! \e ulStyle is the logical OR of the following:
//!
//! - \b #SL_STYLE_OUTLINE to indicate that the slider should be outlined.
//! - \b #SL_STYLE_FILL to indicate that the slider should be filled.
//! - \b #SL_STYLE_BACKG_FILL to indicate that the background portion of the
//!   slider should be filled.
//! - \b #SL_STYLE_TEXT to indicate that the slider should have text drawn
//!   on its active portion (using \e pFont and \e pcText).
//! - \b #SL_STYLE_BACKG_TEXT to indicate that the slider should have text drawn
//!   on its background portion (using \e pFont and \e pcText).
//! - \b #SL_STYLE_IMG to indicate that the slider should have an image
//!   drawn on it (using \e pucImage).
//! - \b #SL_STYLE_BACKG_IMG to indicate that the slider should have an image
//!   drawn on its background (using \e pucBackgroundImage).
//! - \b #SL_STYLE_TEXT_OPAQUE to indicate that the slider text should be
//!   drawn opaque (in other words, drawing the background pixels).
//! - \b #SL_STYLE_BACKG_TEXT_OPAQUE to indicate that the slider text should be
//!   drawn opaque in the background portion of the widget. (in other words,
//!   drawing the background pixels).
//! - \b #SL_STYLE_VERTICAL to indicate that this is a vertical slider
//!   rather than a horizontal one (the default if this style flag is not set).
//! - \b #SL_STYLE_LOCKED to indicate that the slider is being used as an
//!   indicator and should ignore user input.
//!
//! \return Nothing; this is not a function.
//
//*****************************************************************************
#define SliderStruct(pParent, pNext, pChild, pDisplay, lX, lY, lWidth,    \
                     lHeight, lMin, lMax, lValue, ulStyle, ulFillColor,   \
                     ulBackgroundFillColor, ulOutlineColor, ulTextColor,  \
                     ulBackgroundTextColor, pFont, pcText, pucImage,      \
                     pucBackgroundImage, pfnOnChange)                     \
        {                                                                 \
            {                                                             \
                sizeof(tSliderWidget),                                    \
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
                SliderMsgProc                                             \
            },                                                            \
            ulStyle,                                                      \
            ulFillColor,                                                  \
            ulBackgroundFillColor,                                        \
            ulOutlineColor,                                               \
            ulTextColor,                                                  \
            ulBackgroundTextColor,                                        \
            pFont,                                                        \
            pcText,                                                       \
            pucImage,                                                     \
            pucBackgroundImage,                                           \
            pfnOnChange,                                                  \
            lMin,                                                         \
            lMax,                                                         \
            lValue,                                                       \
            0                                                             \
        }

//*****************************************************************************
//
//! Declares an initialized variable containing a slider widget data structure.
//!
//! \param sName is the name of the variable to be declared.
//! \param pParent is a pointer to the parent widget.
//! \param pNext is a pointer to the sibling widget.
//! \param pChild is a pointer to the first child widget.
//! \param pDisplay is a pointer to the display on which to draw the slider.
//! \param lX is the X coordinate of the upper left corner of the slider.
//! \param lY is the Y coordinate of the upper left corner of the slider.
//! \param lWidth is the width of the slider.
//! \param lHeight is the height of the slider.
//! \param lMin is the minimum value for the slider (corresponding to the left
//!  or bottom position).
//! \param lMax is the maximum value for the slider (corresponding to the right
//!  or top position).
//! \param lValue is the initial value of the slider.  This must lie in the
//!  range defined by \e lMin and \e lMax.
//! \param ulStyle is the style to be applied to the slider.
//! \param ulFillColor is the color used to fill in the slider.
//! \param ulBackgroundFillColor is the color used to fill in the background
//!  area of the slider.
//! \param ulOutlineColor is the color used to outline the slider.
//! \param ulTextColor is the color used to draw text on the slider.
//! \param ulBackgroundTextColor is the color used to draw text on the
//! background portion of the slider.
//! \param pFont is a pointer to the font to be used to draw text on the slider.
//! \param pcText is a pointer to the text to draw on this slider.
//! \param pucImage is a pointer to the image to draw on this slider.
//! \param pucBackgroundImage is a pointer to the image to draw on the slider
//! background.
//! \param pfnOnChange is a pointer to the function that is called to notify
//! the application of slider value changes.
//!
//! This macro provides an initialized slider widget data structure, which can
//! be used to construct the widget tree at compile time in global variables
//! (as opposed to run-time via function calls).
//!
//! \e ulStyle is the logical OR of the following:
//!
//! - \b #SL_STYLE_OUTLINE to indicate that the slider should be outlined.
//! - \b #SL_STYLE_FILL to indicate that the slider should be filled.
//! - \b #SL_STYLE_BACKG_FILL to indicate that the background portion of the
//!   slider should be filled.
//! - \b #SL_STYLE_TEXT to indicate that the slider should have text drawn
//!   on its active portion (using \e pFont and \e pcText).
//! - \b #SL_STYLE_BACKG_TEXT to indicate that the slider should have text drawn
//!   on its background portion (using \e pFont and \e pcText).
//! - \b #SL_STYLE_IMG to indicate that the slider should have an image
//!   drawn on it (using \e pucImage).
//! - \b #SL_STYLE_BACKG_IMG to indicate that the slider should have an image
//!   drawn on its background (using \e pucBackgroundImage).
//! - \b #SL_STYLE_TEXT_OPAQUE to indicate that the slider text should be
//!   drawn opaque (in other words, drawing the background pixels).
//! - \b #SL_STYLE_BACKG_TEXT_OPAQUE to indicate that the slider text should be
//!   drawn opaque in the background portion of the widget. (in other words,
//!   drawing the background pixels).
//! - \b #SL_STYLE_VERTICAL to indicate that this is a vertical slider
//!   rather than a horizontal one (the default if this style flag is not set).
//! - \b #SL_STYLE_LOCKED to indicate that the slider is being used as an
//!   indicator and should ignore user input.
//!
//! \return Nothing; this is not a function.
//
//*****************************************************************************
#define Slider(sName, pParent, pNext, pChild, pDisplay, lX, lY,      \
               lWidth, lHeight, lMin, lMax, lValue, ulStyle,         \
               ulFillColor, ulBackgroundFillColor, ulOutlineColor,   \
               ulTextColor, ulBackgroundTextColor, pFont, pcText,    \
               pucImage, pucBackgroundImage, pfnOnChange)            \
        tSliderWidget sName =                                        \
            SliderStruct(pParent, pNext, pChild, pDisplay, lX, lY,   \
                         lWidth, lHeight, lMin, lMax, lValue,        \
                         ulStyle, ulFillColor, ulBackgroundFillColor,\
                         ulOutlineColor, ulTextColor,                \
                         ulBackgroundTextColor, pFont, pcText,       \
                         pucImage, pucBackgroundImage, pfnOnChange)

//*****************************************************************************
//
//! Sets the function to call when this slider widget's value changes.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//! \param pfnCallback is a pointer to the function to call.
//!
//! This function sets the function to be called when the value represented by
//! the slider changes.
//!
//! \return None.
//
//*****************************************************************************
#define SliderCallbackSet(pWidget, pfnCallback)   \
        do                                        \
        {                                         \
            tSliderWidget *pW = pWidget;          \
            pW->pfnOnChange = pfnCallback;        \
        }                                         \
        while(0)

//*****************************************************************************
//
//! Sets the fill color for the active area of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to fill the slider.
//!
//! This function changes the color used to fill the active are of the slider
//! on the display.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderFillColorSet(pWidget, ulColor)     \
        do                                       \
        {                                        \
            tSliderWidget *pW = pWidget;         \
            pW->ulFillColor = ulColor;           \
        }                                        \
        while(0)

//*****************************************************************************
//
//! Sets the fill color for the background area of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to fill the background area
//! of the slider.
//!
//! This function changes the color used to fill the background area of the
//! slider on the display.  The display is not updated until the next paint
//! request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderFillColorBackgroundedSet(pWidget, ulColor) \
        do                                               \
        {                                                \
            tSliderWidget *pW = pWidget;                 \
            pW->ulBackgroundFillColor = ulColor;         \
        }                                                \
        while(0)

//*****************************************************************************
//
//! Disables filling of the active area of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function disables the filling of the active area of a slider widget.
//! The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderFillOff(pWidget)               \
        do                                   \
        {                                    \
            tSliderWidget *pW = pWidget;     \
            pW->ulStyle &= ~(SL_STYLE_FILL); \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Enables filling of the active area of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function enables the filling of the active area of a slider widget.
//! The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderFillOn(pWidget)                \
        do                                   \
        {                                    \
            tSliderWidget *pW = pWidget;     \
            pW->ulStyle |= SL_STYLE_FILL;    \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Disables filling of the background area of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function disables the filling of the background area of a slider
//! widget.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderBackgroundFillOff(pWidget)            \
        do                                          \
        {                                           \
            tSliderWidget *pW = pWidget;            \
            pW->ulStyle &= ~(SL_STYLE_BACKG_FILL ); \
        }                                           \
        while(0)

//*****************************************************************************
//
//! Enables filling of the background area of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function enables the filling of the background area of a slider widget.
//! The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderBackgroundFillOn(pWidget)             \
        do                                          \
        {                                           \
            tSliderWidget *pW = pWidget;            \
            pW->ulStyle |= SL_STYLE_BACKG_FILL ;    \
        }                                           \
        while(0)

//*****************************************************************************
//
//! Sets the font for a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//! \param pFnt is a pointer to the font to use to draw text on the slider.
//!
//! This function changes the font used to draw text on the slider.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderFontSet(pWidget, pFnt)         \
        do                                   \
        {                                    \
            tSliderWidget *pW = pWidget;     \
            const tFont *pF = pFnt;          \
            pW->pFont = pF;                  \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Changes the image drawn on the active area of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to be modified.
//! \param pImg is a pointer to the image to draw onto the slider.
//!
//! This function changes the image that is drawn on the active area of the
//! slider.  This image will be centered within the widget rectangle and the
//! portion represented by the current slider value will be visible.  The
//! display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderImageSet(pWidget, pImg)        \
        do                                   \
        {                                    \
            tSliderWidget *pW = pWidget;     \
            const unsigned char *pI = pImg;  \
            pW->pucImage = pI;               \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Changes the image drawn on the background area of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to be modified.
//! \param pImg is a pointer to the image to draw onto the background area of
//! the slider.
//!
//! This function changes the image that is drawn onto the background area of
//! the slider.  This image will be centered within the widget rectangle and
//! the portion in the area not represented by the current slider value will
//! be visible.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderBackgroundImageSet(pWidget, pImg)  \
        do                                       \
        {                                        \
            tSliderWidget *pW = pWidget;         \
            const unsigned char *pI = pImg;      \
            pW->pucBackgroundImage = pI;         \
        }                                        \
        while(0)

//*****************************************************************************
//
//! Disables the image on the active area of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function disables the drawing of an image on the active area of a
//! slider widget.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderImageOff(pWidget)              \
        do                                   \
        {                                    \
            tSliderWidget *pW = pWidget;     \
            pW->ulStyle &= ~(SL_STYLE_IMG);  \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Enables the image on the active area of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function enables the drawing of an image on the active area of a
//! slider widget.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderImageOn(pWidget)               \
        do                                   \
        {                                    \
            tSliderWidget *pW = pWidget;     \
            pW->ulStyle |= SL_STYLE_IMG;     \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Disables the image on the background area of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function disables the drawing of an image on the background area of a
//! slider widget.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderBackgroundImageOff(pWidget)           \
        do                                          \
        {                                           \
            tSliderWidget *pW = pWidget;            \
            pW->ulStyle &= ~(SL_STYLE_BACKG_IMG);   \
        }                                           \
        while(0)

//*****************************************************************************
//
//! Enables the image on the background area of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function enables the drawing of an image on the background area of a
//! slider widget.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderBackgroundImageOn(pWidget)        \
        do                                      \
        {                                       \
            tSliderWidget *pW = pWidget;        \
            pW->ulStyle |= SL_STYLE_BACKG_IMG;  \
        }                                       \
        while(0)

//*****************************************************************************
//
//! Sets the outline color of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to outline the slider.
//!
//! This function changes the color used to outline the slider on the
//! display.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderOutlineColorSet(pWidget, ulColor)     \
        do                                          \
        {                                           \
            tSliderWidget *pW = pWidget;            \
            pW->ulOutlineColor = ulColor;           \
        }                                           \
        while(0)

//*****************************************************************************
//
//! Disables outlining of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function disables the outlining of a slider widget.  The display
//! is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderOutlineOff(pWidget)               \
        do                                      \
        {                                       \
            tSliderWidget *pW = pWidget;        \
            pW->ulStyle &= ~(SL_STYLE_OUTLINE); \
        }                                       \
        while(0)

//*****************************************************************************
//
//! Enables outlining of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function enables the outlining of a slider widget.  The display
//! is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderOutlineOn(pWidget)             \
        do                                   \
        {                                    \
            tSliderWidget *pW = pWidget;     \
            pW->ulStyle |= SL_STYLE_OUTLINE; \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Sets the text color of the active portion of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to draw text on the slider.
//!
//! This function changes the color used to draw text on the active portion of
//! the slider on the display.  The display is not updated until the next paint
//! request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderTextColorSet(pWidget, ulColor)     \
        do                                       \
        {                                        \
            tSliderWidget *pW = pWidget;         \
            pW->ulTextColor = ulColor;           \
        }                                        \
        while(0)

//*****************************************************************************
//
//! Sets the background text color of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to be modified.
//! \param ulColor is the 24-bit RGB color to use to draw background text on
//! the slider.
//!
//! This function changes the color used to draw text on the slider's
//! background portion on the display.  The display is not updated until the
//! next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderBackgroundTextColorSet(pWidget, ulColor)  \
        do                                              \
        {                                               \
            tSliderWidget *pW = pWidget;                \
            pW->ulBackgroundTextColor = ulColor;        \
        }                                               \
        while(0)

//*****************************************************************************
//
//! Disables the text on the active portion of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function disables the drawing of text on the active portion of a
//! slider widget.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderTextOff(pWidget)               \
        do                                   \
        {                                    \
            tSliderWidget *pW = pWidget;     \
            pW->ulStyle &= ~(SL_STYLE_TEXT); \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Enables the text on the active portion of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function enables the drawing of text on the active portion of a slider
//! widget.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderTextOn(pWidget)                \
        do                                   \
        {                                    \
            tSliderWidget *pW = pWidget;     \
            pW->ulStyle |= SL_STYLE_TEXT;    \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Disables opaque text on the active portion of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function disables the use of opaque text on the active portion of this
//! slider.  When not using opaque text, only the foreground pixels of the text
//! are drawn on the screen, allowing the previously drawn pixels (such as the
//! slider image) to show through the text.  Note that SL_STYLE_TEXT must also
//! be cleared to disable text rendering on the slider active area.
//!
//! \return None.
//
//*****************************************************************************
#define SliderTextOpaqueOff(pWidget)                \
        do                                          \
        {                                           \
            tSliderWidget *pW = pWidget;            \
            pW->ulStyle &= ~(SL_STYLE_TEXT_OPAQUE); \
        }                                           \
        while(0)

//*****************************************************************************
//
//! Enables opaque text on the active portion of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function enables the use of opaque text on  the active portion of this
//! slider.  When using opaque text, both the foreground and background pixels
//! of the text are drawn on the screen, blocking out the previously drawn
//! pixels.    Note that SL_STYLE_TEXT must also be set to enable text
//! rendering on the slider active area.
//!
//! \return None.
//
//*****************************************************************************
#define SliderTextOpaqueOn(pWidget)              \
        do                                       \
        {                                        \
            tSliderWidget *pW = pWidget;         \
            pW->ulStyle |= SL_STYLE_TEXT_OPAQUE; \
        }                                        \
        while(0)

//*****************************************************************************
//
//! Disables the text on the background portion of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function disables the drawing of text on the background portion of
//! a slider widget.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderBackgroundTextOff(pWidget)                \
        do                                              \
        {                                               \
            tSliderWidget *pW = pWidget;                \
            pW->ulStyle &= ~(SL_STYLE_BACKG_TEXT);      \
        }                                               \
        while(0)

//*****************************************************************************
//
//! Enables the text on the background portion of a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function enables the drawing of text on the background portion of a
//! slider widget.  The display is not updated until the next paint request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderBackgroundTextOn(pWidget)                 \
        do                                              \
        {                                               \
            tSliderWidget *pW = pWidget;                \
            pW->ulStyle |= SL_STYLE_BACKG_TEXT;         \
        }                                               \
        while(0)

//*****************************************************************************
//
//! Disables opaque background text on a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function disables the use of opaque text on the background portion of
//! this slider.  When not using opaque text, only the foreground pixels of the
//! text are drawn on the screen, allowing the previously drawn pixels (such as
//! the slider image) to show through the text.  Note that SL_STYLE_BACKG_TEXT
//! must also be cleared to disable text rendering on the slider background
//! area.
//!
//! \return None.
//
//*****************************************************************************
#define SliderBackgroundTextOpaqueOff(pWidget)              \
        do                                                  \
        {                                                   \
            tSliderWidget *pW = pWidget;                    \
            pW->ulStyle &= ~(SL_STYLE_BACKG_TEXT_OPAQUE);   \
        }                                                   \
        while(0)

//*****************************************************************************
//
//! Enables opaque background text on a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function enables the use of opaque text on the background portion of
//! this slider.  When using opaque text, both the foreground and background
//! pixels of the text are drawn on the screen, blocking out the previously
//! drawn pixels.  Note that SL_STYLE_BACKG_TEXT must also be set to enable
//! text rendering on the slider background area.
//!
//! \return None.
//
//*****************************************************************************
#define SliderBackgroundTextOpaqueOn(pWidget)               \
        do                                                  \
        {                                                   \
            tSliderWidget *pW = pWidget;                    \
            pW->ulStyle |= SL_STYLE_BACKG_TEXT_OPAQUE;      \
        }                                                   \
        while(0)

//*****************************************************************************
//
//! Locks a slider making it ignore pointer input.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function locks a slider widget and makes it ignore all pointer input.
//! When locked, a slider acts as a passive indicator.  Its value may be
//! changed using SliderValueSet() and the value display updated using
//! WidgetPaint() but no user interaction via the pointer will change the
//! widget value.
//!
//! \return None.
//
//*****************************************************************************
#define SliderLock(pWidget)                     \
        do                                      \
        {                                       \
            tSliderWidget *pW = pWidget;        \
            pW->ulStyle |= SL_STYLE_LOCKED;     \
        }                                       \
        while(0)

//*****************************************************************************
//
//! Unlocks a slider making it pay attention to pointer input.
//!
//! \param pWidget is a pointer to the slider widget to modify.
//!
//! This function unlocks a slider widget.  When unlocked, a slider will
//! respond to pointer input by setting its value appropriately and informing
//! the application via callbacks.
//!
//! \return None.
//
//*****************************************************************************
#define SliderUnlock(pWidget)                   \
        do                                      \
        {                                       \
            tSliderWidget *pW = pWidget;        \
            pW->ulStyle &= ~(SL_STYLE_LOCKED);  \
        }                                       \
        while(0)

//*****************************************************************************
//
//! Changes the text drawn on a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to be modified.
//! \param pcTxt is a pointer to the text to draw onto the slider.
//!
//! This function changes the text that is drawn onto the slider.  The string
//! is centered across the slider and straddles the active and background
//! portions of the widget.  The display is not updated until the next paint
//! request.
//!
//! \return None.
//
//*****************************************************************************
#define SliderTextSet(pWidget, pcTxt)        \
        do                                   \
        {                                    \
            tSliderWidget *pW = pWidget;     \
            const char *pcT = pcTxt;         \
            pW->pcText = pcT;                \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Changes the value range for a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to be modified.
//! \param lMinimum is the minimum value that the slider will report.
//! \param lMaximum is the maximum value that the slider will report.
//!
//! This function changes the range of a slider.  Slider positions are reported
//! in terms of this range with the current position of the slider on the
//! display being scaled and translated into this range such that the minimum
//! value represents the left position of a horizontal slider or the bottom
//! position of a vertical slider and the maximum value represents the other
//! end of the slider range.  Note that this function does not cause the slider
//! to be redrawn.  The caller must call WidgetPaint() explicitly after this
//! call to ensure that the widget is redrawn.
//!
//! \return None.
//
//*****************************************************************************
#define SliderRangeSet(pWidget, lMinimum, lMaximum)  \
        do                                           \
        {                                            \
            tSliderWidget *pW = pWidget;             \
            pW->lMin = (lMinimum);                   \
            pW->lMax = (lMaximum);                   \
        }                                            \
        while(0)

//*****************************************************************************
//
//! Changes the minimum value for a slider widget.
//!
//! \param pWidget is a pointer to the slider widget to be modified.
//! \param lVal is the new value to set for the slider.  This is in terms of
//! the value range currently set for the slider.
//!
//! This function changes the value that the slider will display the next time
//! the widget is painted.  The caller is responsible for ensuring that the
//! value passed is within the range specified for the target widget.  The
//! caller must call WidgetPaint() explicitly after this call to ensure that
//! the widget is redrawn.
//!
//! \return None.
//
//*****************************************************************************
#define SliderValueSet(pWidget, lVal)        \
        do                                   \
        {                                    \
            tSliderWidget *pW = pWidget;     \
            pW->lValue = (lVal);             \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Sets the vertical or horizontal style for a slider widget
//!
//! \param pWidget is a pointer to the slider widget to be modified.
//! \param bVertical is \b true to set the vertical slider style or \b false
//! to set the horizontal slider style.
//!
//! This function allows the vertical or horizontal style to be set when
//! creating slider widgets dynamically.  The function will typically be called
//! before the slider is first attached to the active widget tree.  Since the
//! vertical or horizontal style is intimately linked with the slider size
//! and position on the display, it seldom makes sense to change this style for
//! a widget which is already on the display.
//!
//! \return None.
//
//*****************************************************************************
#define SliderVerticalSet(pWidget, bVertical)           \
        do                                              \
        {                                               \
            tSliderWidget *pW = pWidget;                \
            if(bVertical)                               \
            {                                           \
                pW->ulStyle |= (SL_STYLE_VERTICAL);     \
            }                                           \
            else                                        \
            {                                           \
                pW->ulStyle &= ~(SL_STYLE_VERTICAL);    \
            }                                           \
        }                                               \
        while(0)

//*****************************************************************************
//
// Prototypes for the slider widget APIs.
//
//*****************************************************************************
extern long SliderMsgProc(tWidget *pWidget, unsigned long ulMsg,
                          unsigned long ulParam1, unsigned long ulParam2);
extern void SliderInit(tSliderWidget *pWidget,
                       const tDisplay *pDisplay, long lX, long lY, long lWidth,
                       long lHeight);

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

#endif // __SLIDER_H__
