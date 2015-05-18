//*****************************************************************************
//
// widget.h - Prototypes for the widget base "class".
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

#ifndef __WIDGET_H__
#define __WIDGET_H__

//*****************************************************************************
//
//! \addtogroup widget_api
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
//! The structure that describes a generic widget.  This structure is the base
//! ``class'' for all other widgets.
//
//*****************************************************************************
typedef struct __Widget
{
    //
    //! The size of this structure.  This will be the size of the full
    //! structure, not just the generic widget subset.
    //
    long lSize;

    //
    //! A pointer to this widget's parent widget.
    //
    struct __Widget *pParent;

    //
    //! A pointer to this widget's first sibling widget.
    //
    struct __Widget *pNext;

    //
    //! A pointer to this widget's first child widget.
    //
    struct __Widget *pChild;

    //
    //! A pointer to the display on which this widget resides.
    //
    const tDisplay *pDisplay;

    //
    //! The rectangle that encloses this widget.
    //
    tRectangle sPosition;

    //
    //! The procedure that handles messages sent to this widget.
    //
    long (*pfnMsgProc)(struct __Widget *pWidget, unsigned long ulMessage,
                       unsigned long ulParam1, unsigned long ulParam2);
}
tWidget;

//*****************************************************************************
//
//! The widget at the root of the widget tree.  This can be used when
//! constructing a widget tree at compile time (used as the pParent argument to
//! a widget declaration) or as the pWidget argument to an API (such as
//! WidgetPaint() to paint the entire widget tree).
//
//*****************************************************************************
#define WIDGET_ROOT             &g_sRoot

//*****************************************************************************
//
//! This message is sent to indicate that the widget should draw itself on the
//! display.  Neither \e ulParam1 nor \e ulParam2 are used by this message.
//! This message is delivered in top-down order.
//
//*****************************************************************************
#define WIDGET_MSG_PAINT        0x00000001

//*****************************************************************************
//
//! This message is sent to indicate that the pointer is now down.  \e ulParam1
//! is the X coordinate of the location where the pointer down event occurred,
//! and \e ulParam2 is the Y coordinate.  This message is delivered in
//! bottom-up order.
//
//*****************************************************************************
#define WIDGET_MSG_PTR_DOWN     0x00000002

//*****************************************************************************
//
//! This message is sent to indicate that the pointer has moved while being
//! down.  \e ulParam1 is the X coordinate of the new pointer location, and
//! \e ulParam2 is the Y coordinate.  This message is delivered in bottom-up
//! order.
//
//*****************************************************************************
#define WIDGET_MSG_PTR_MOVE     0x00000003

//*****************************************************************************
//
//! This message is sent to indicate that the pointer is now up.  \e ulParam1
//! is the X coordinate of the location where the pointer up event occurred,
//! and \e ulParam2 is the Y coordinate.  This message is delivered in
//! bottom-up order.
//
//*****************************************************************************
#define WIDGET_MSG_PTR_UP       0x00000004

//*****************************************************************************
//
//! This message is sent by the application to indicate that there has been a
//! key press or button press meaning "up".  \e ulParam1 by convention is a
//! pointer to the widget that is the intended recipient of the key press.
//! This is controlled by the application.
//
//*****************************************************************************
#define WIDGET_MSG_KEY_UP       0x00000005

//*****************************************************************************
//
//! This message is sent by the application to indicate that there has been a
//! key press or button press meaning "down".  \e ulParam1 by convention is a
//! pointer to the widget that is the intended recipient of the key press.
//! This is controlled by the application.
//
//*****************************************************************************
#define WIDGET_MSG_KEY_DOWN     0x00000006

//*****************************************************************************
//
//! This message is sent by the application to indicate that there has been a
//! key press or button press meaning "left".  \e ulParam1 by convention is a
//! pointer to the widget that is the intended recipient of the key press.
//! This is controlled by the application.
//
//*****************************************************************************
#define WIDGET_MSG_KEY_LEFT     0x00000007

//*****************************************************************************
//
//! This message is sent by the application to indicate that there has been a
//! key press or button press meaning "right".  \e ulParam1 by convention is a
//! pointer to the widget that is the intended recipient of the key press.
//! This is controlled by the application.
//
//*****************************************************************************
#define WIDGET_MSG_KEY_RIGHT    0x00000008

//*****************************************************************************
//
//! This message is sent by the application to indicate that there has been a
//! key press or button press meaning "select".  \e ulParam1 by convention is a
//! pointer to the widget that is the intended recipient of the key press.
//! This is controlled by the application.
//
//*****************************************************************************
#define WIDGET_MSG_KEY_SELECT   0x00000009

//*****************************************************************************
//
//! Requests a redraw of the widget tree.
//!
//! \param pWidget is a pointer to the widget tree to paint.
//!
//! This function sends a \b #WIDGET_MSG_PAINT message to the given widgets,
//! and all of the widget beneath it, so that they will draw or redraw
//! themselves on the display.  The actual drawing will occur when this message
//! is retrieved from the message queue and processed.
//!
//! \return Returns 1 if the message was added to the message queue and 0 if it
//! cound not be added (due to the queue being full).
//
//*****************************************************************************
#define WidgetPaint(pWidget)                                         \
        WidgetMessageQueueAdd(pWidget, WIDGET_MSG_PAINT, 0, 0, 0, 0)

//*****************************************************************************
//
// Prototypes for the generic widget handling functions.
//
//*****************************************************************************
extern tWidget g_sRoot;
extern long WidgetDefaultMsgProc(tWidget *pWidget, unsigned long ulMessage,
                                 unsigned long ulParam1,
                                 unsigned long ulParam2);
extern void WidgetAdd(tWidget *pParent, tWidget *pWidget);
extern void WidgetRemove(tWidget *pWidget);
extern unsigned long WidgetMessageSendPreOrder(tWidget *pWidget,
                                               unsigned long ulMessage,
                                               unsigned long ulParam1,
                                               unsigned long ulParam2,
                                               unsigned long bStopOnSuccess);
extern unsigned long WidgetMessageSendPostOrder(tWidget *pWidget,
                                                unsigned long ulMessage,
                                                unsigned long ulParam1,
                                                unsigned long ulParam2,
                                                unsigned long bStopOnSuccess);
extern long WidgetMessageQueueAdd(tWidget *pWidget, unsigned long ulMessage,
                                  unsigned long ulParam1,
                                  unsigned long ulParam2,
                                  unsigned long bPostOrder,
                                  unsigned long bStopOnSuccess);
extern void WidgetMessageQueueProcess(void);
extern long WidgetPointerMessage(unsigned long ulMessage, long lX, long lY);
extern void WidgetMutexInit(unsigned char *pcMutex);
extern unsigned long WidgetMutexGet(unsigned char *pcMutex);
extern void WidgetMutexPut(unsigned char *pcMutex);

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

#endif // __WIDGET_H__
