//*****************************************************************************
//
// widget.c - Generic widget tree handling code.
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

//*****************************************************************************
//
//! \addtogroup widget_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Flags that indicate how messages from the message queue are processed.  They
// can be sent via either a pre-order or post-order search, and can optionally
// be sent to no other widgets once one accepts the message.
//
//*****************************************************************************
#define MQ_FLAG_POST_ORDER      1
#define MQ_FLAG_STOP_ON_SUCCESS 2

//*****************************************************************************
//
// The size of the message queue.  In order to make the queue pointer
// arithmetic more efficient, this should be a power of two.
//
//*****************************************************************************
#define QUEUE_SIZE              16

#ifdef DEBUG_MSGQ
//*****************************************************************************
//
// In debug builds, keep track of the number of cases where a message was
// lost due to the queue being full.  We count the following occurrences:
//
// 1. All messages discarded due to queue overflow (g_ulMQOverflow)
// 2. Messages other than WIDGET_MSG_PTR_MOVE discarded due to queue
//    overflow (g_ulMQNonMouseOverflow).  In this case, we also remember the
//    last message that was discarded (g_ulMQLastLostMsg).
// 3. The number of calls to WidgetMessageQueueAdd that fail due to the queue
//    mutex already being held.
// 4. The number of cases where WidgetMessageQueueAdd reused an unread
//    WIDGET_MSG_PTR_MOVE message when a second one arrived before the previous
//    one had been processed.
//
//*****************************************************************************
unsigned long g_ulMQOverflow = 0;
unsigned long g_ulMQNonMouseOverflow = 0;
unsigned long g_ulMQLastLostMsg = 0;
unsigned long g_ulMQMutexClash = 0;
unsigned long g_ulMQMoveOverwrite = 0;
#endif

//*****************************************************************************
//
// This structure describes the message queue used to hold widget messages.
//
//*****************************************************************************
typedef struct
{
    //
    // The flags that describe how this message should be processed; this is
    // defined by the MQ_FLAG_xxx flags.
    //
    unsigned long ulFlags;

    //
    // The widget (or widget tree) to which the message should be sent.
    //
    tWidget *pWidget;

    //
    // The message to be sent.
    //
    unsigned long ulMessage;

    //
    // The first parameter to the message.
    //
    unsigned long ulParam1;

    //
    // The second parameter to the message.
    //
    unsigned long ulParam2;
}
tWidgetMessageQueue;

//*****************************************************************************
//
// The root of the widget tree.  This is the widget used when no parent is
// specified when adding a widget, or when no widget is specified when sending
// a message.  The parent and sibling of this widget are always zero.  This
// should not be directly referenced by applications; WIDGET_ROOT should be
// used instead.
//
//*****************************************************************************
tWidget g_sRoot =
{
    sizeof(tWidget),
    0,
    0,
    0,
    0,
    {
        0,
        0,
        0,
        0,
    },
    WidgetDefaultMsgProc
};

//*****************************************************************************
//
// The widget that has captured pointer messages.  When a pointer down message
// is accepted by a widget, that widget is saved in this variable and all
// subsequent pointer move and pointer up messages are sent directly to this
// widget.
//
//*****************************************************************************
static tWidget *g_pPointerWidget = 0;

//*****************************************************************************
//
// The message queue that holds messages that are waiting to be processed.
//
//*****************************************************************************
static volatile tWidgetMessageQueue g_pMQ[QUEUE_SIZE];

//*****************************************************************************
//
// The offset to the next message to be read from the message queue.  The
// message queue is empty when this has the same value as g_ulMQWrite.
//
//*****************************************************************************
static unsigned long g_ulMQRead = 0;

//*****************************************************************************
//
// The offset to the next message to be written to the message queue.  The
// message queue is full when this value is one less than g_ulMQRead (modulo
// the queue size).
//
//*****************************************************************************
static volatile unsigned long g_ulMQWrite = 0;

//*****************************************************************************
//
// The mutex used to protect access to the message queue.
//
//*****************************************************************************
static unsigned char g_ucMQMutex = 0;

//*****************************************************************************
//
//! Initializes a mutex to the unowned state.
//!
//! \param pcMutex is a pointer to mutex that is to be initialized.
//!
//! This function initializes a mutual exclusion semaphore (mutex) to its
//! unowned state in preparation for use with WidgetMutexGet() and
//! WidgetMutexPut().  A mutex is a two state object typically used to
//! serialize access to a shared resource.  An application will call
//! WidgetMutexGet() to request ownership of the mutex.  If ownership is
//! granted, the caller may safely access the resource then release the mutex
//! using WidgetMutexPut() once it is finished.  If ownership is not granted,
//! the caller knows that some other context is currently modifying the shared
//! resource and it must not access the resource at that time.
//!
//! Note that this function must not be called if the mutex passed in \e pcMutex
//! is already in use since this will have the effect of releasing the lock even
//! if some caller currently owns it.
//!
//! \return None.
//
//*****************************************************************************
void
WidgetMutexInit(unsigned char *pcMutex)
{
    //
    // Catch NULL pointers in a debug build.
    //
    ASSERT(pcMutex);

    //
    // Clear the mutex location to set it to the unowned state.
    //
    *pcMutex = 0;
}

//*****************************************************************************
//
//! Attempts to acquire a mutex.
//!
//! \param pcMutex is a pointer to mutex that is to be acquired.
//!
//! This function attempts to acquire a mutual exclusion semaphore (mutex) on
//! behalf of the caller.  If the mutex is not already held, 0 is returned to
//! indicate that the caller may safely access whichever resource the mutex is
//! protecting.  If the mutex is already held, 1 is returned and the caller
//! must not access the shared resource.
//!
//! When access to the shared resource is complete, the mutex owner should call
//! WidgetMutexPut() to release the mutex and relinquish ownership of the
//! shared resource.
//!
//! \return Returns 0 if the mutex is acquired successfully or 1 if it is
//! already held by another caller.
//
//*****************************************************************************
#if defined(ewarm) || defined(DOXYGEN)
unsigned long
WidgetMutexGet(unsigned char *pcMutex)
{
    //
    // Acquire the mutex if possible.
    //
    __asm("    mov     r1, #1\n"
          "    ldrexb  r2, [r0]\n"
          "    cmp     r2, #0\n"
          "    it      eq\n"
          "    strexb  r2, r1, [r0]\n"
          "    mov     r0, r2\n");

    //
    // "Warning[Pe940]: missing return statement at end of non-void function"
    // is suppressed here to avoid putting a "bx lr" in the inline assembly
    // above and a superfluous return statement here.
    //
#pragma diag_suppress=Pe940
}
#pragma diag_default=Pe940
#endif
#if defined(codered) || defined(gcc) || defined(sourcerygxx)
unsigned long __attribute__((naked))
WidgetMutexGet(unsigned char *pcMutex)
{
    unsigned long ulRet;

    //
    // Acquire the mutex if possible.
    //
    __asm("    mov      r1, #1\n"
          "    ldrexb   r2, [r0]\n"
          "    cmp      r2, #0\n"
          "    it       eq\n"
          "    strexbeq r2, r1, [r0]\n"
          "    mov      r0, r2\n"
          "    bx       lr\n"
          : "=r" (ulRet));

    //
    // The return is handled in the inline assembly, but the compiler will
    // still complain if there is not an explicit return here (despite the fact
    // that this does not result in any code being produced because of the
    // naked attribute).
    //
    return(ulRet);
}
#endif
#if defined(rvmdk) || defined(__ARMCC_VERSION)
__asm unsigned long
WidgetMutexGet(unsigned char *pcMutex)
{
    mov         r1, #1
    ldrexb      r2, [r0]
    cmp         r2, #0
    it          eq
    strexbeq    r2, r1, [r0]
    mov         r0, r2
    bx          lr
}
#endif
//
// For CCS implement this function in pure assembly.  This prevents the TI
// compiler from doing funny things with the optimizer.
//
#if defined(ccs)
    __asm("    .sect \".text:WidgetMutexGet\"\n"
          "    .clink\n"
          "    .thumbfunc WidgetMutexGet\n"
          "    .thumb\n"
          "    .global WidgetMutexGet\n"
          "WidgetMutexGet:\n"
          "    mov         r1, #1\n"
          "    ldrexb      r2, [r0]\n"
          "    cmp         r2, #0\n"
          "    it          EQ\n" // TI assembler requires upper case cond
          "    strexbeq    r2, r1, [r0]\n"
          "    mov         r0, r2\n"
          "    bx          lr\n");
#endif


//*****************************************************************************
//
//! Release a mutex.
//!
//! \param pcMutex is a pointer to mutex that is to be released.
//!
//! This function releases a mutual exclusion semaphore (mutex), leaving it in
//! the unowned state.
//!
//! \return None.
//
//*****************************************************************************
void
WidgetMutexPut(unsigned char *pcMutex)
{
    //
    // Release the mutex.
    //
    *pcMutex = 0;
}

//*****************************************************************************
//
// Determines if a widget exists in the tree below a given point.
//
// \param pWidget is a pointer to the widget tree.
// \param pFind is a pointer to the widget that is being searched for.
//
// This function searches the widget tree below pWidget to determine whether
// or not the widget pointed to by \e pFind exists in the subtree.
//
// \return Returns 1 if \e pFind exists in the subtree or 0 if it does not.
//
//*****************************************************************************
static long
WidgetIsInTree(tWidget *pWidget, tWidget *pFind)
{
    tWidget *pTemp;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);
    ASSERT(pFind);

    //
    // Loop through the tree under the widget until every widget is searched.
    //
    for(pTemp = pWidget; pTemp != pWidget->pParent; )
    {
        //
        // See if this widget has a child.
        //
        if(pTemp->pChild)
        {
            //
            // Go to this widget's child first.
            //
            pTemp = pTemp->pChild;
        }

        //
        // This widget does not have a child, so either a sibling or a parent
        // must be checked.  When moving back to the parent, another move must
        // be performed as well to avoid getting stuck in a loop (since the
        // parent's children have already been searched.
        //
        else
        {
            //
            // Loop until returning to the parent of the starting widget.  This
            // loop will be explicitly broken out of if an intervening widget
            // is encountered that has not been searched.
            //
            while(pTemp != pWidget->pParent)
            {
                if(pTemp == pFind)
                {
                    return(1);
                }

                //
                // See if this widget has a sibling.
                //
                if(pTemp->pNext)
                {
                    //
                    // Visit the sibling of this widget.
                    //
                    pTemp = pTemp->pNext;

                    //
                    // Since this widget has not been searched yet, break out
                    // of the controlling loop.
                    //
                    break;
                }
                else
                {
                    //
                    // This widget has no siblings, so go to its parent.  Since
                    // the parent has already been searched, the same sibling
                    // vs. parent decision must be made on this widget as well.
                    //
                    pTemp = pTemp->pParent;
                }
            }
        }
    }

    //
    // The widget could not be found.
    //
    return(0);
}

//*****************************************************************************
//
//! Handles widget messages.
//!
//! \param pWidget is a pointer to the widget.
//! \param ulMessage is the message to be processed.
//! \param ulParam1 is the first parameter to the message.
//! \param ulParam2 is the second parameter to the message.
//!
//! This function is a default handler for widget messages; it simply ignores
//! all messages sent to it.  This is used as the message handler for the root
//! widget, and should be called by the message handler for other widgets when
//! they do not explicitly handle the provided message (in case new messages
//! are added that require some default but override-able processing).
//!
//! \return Always returns 0.
//
//*****************************************************************************
long
WidgetDefaultMsgProc(tWidget *pWidget, unsigned long ulMessage,
                     unsigned long ulParam1, unsigned long ulParam2)
{
    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Return zero for all messages.
    //
    return(0);
}

//*****************************************************************************
//
//! Adds a widget to the widget tree.
//!
//! \param pParent is the parent for the widget. To add to the root of the tree
//! set this parameter to \b WIDGET_ROOT.
//! \param pWidget is the widget to add.
//!
//! This function adds a widget to the widget tree at the given position within
//! the tree.  The widget will become the last child of its parent, and will
//! therefore be searched after the existing children.
//!
//! The added widget can be a full widget tree, allowing addition of an entire
//! heirarchy all at once (for example, adding an entire screen to the widget
//! tree all at once).  In this case, it is the responsibility of the caller to
//! ensure that the pParent field of each widget in the added tree is correctly
//! set (in other words, only the widget pointed to by \e pWidget is updated to
//! properly reside in the tree).
//!
//! It is the responsibility of the caller to initialize the pNext and pChild
//! field of the added widget; either of these fields being non-zero results in
//! a pre-defined tree of widgets being added instead of a single one.
//!
//! \return None.
//
//*****************************************************************************
void
WidgetAdd(tWidget *pParent, tWidget *pWidget)
{
    //
    // Check the arguments.
    //
    ASSERT(pParent);
    ASSERT(pWidget);

    //
    // Make this widget be a child of its parent.
    //
    pWidget->pParent = pParent;

    //
    // See if this parent already has children.
    //
    if(pParent->pChild)
    {
        //
        // Find the last child of this parent and also check that widget is not
        // already present at this level of the tree.
        //
        for(pParent = pParent->pChild; pParent->pNext;
            pParent = pParent->pNext)
        {
            //
            // If we find this widget here already, just return.  If we don't
            // do this, we allow errant programs to add the same child twice
            // resulting in looping on message processing.
            //
            if(pParent == pWidget)
            {
                return;
            }
        }

        //
        // We perform one final check to see if we are about to add the widget
        // twice.  We need this to catch the case of a single child which
        // causes the previous loop to exit before performing the widget check.
        //
        if(pParent == pWidget)
        {
            return;
        }

        //
        // Add this widget to the end of the list of children of this parent.
        //
        pParent->pNext = pWidget;
    }
    else
    {
        //
        // Make this widget be the first (and only) child of this parent.
        //
        pParent->pChild = pWidget;
    }
}

//*****************************************************************************
//
//! Removes a widget from the widget tree.
//!
//! \param pWidget is the widget to be removed.
//!
//! This function removes a widget from the widget tree.  The removed widget
//! can be a full widget tree, allowing removal of an entire heirarchy all at
//! once (for example, removing an entire screen from the widget tree).
//!
//! \return None.
//
//*****************************************************************************
void
WidgetRemove(tWidget *pWidget)
{
    tWidget *pTemp;

    //
    // Check the argument.
    //
    ASSERT(pWidget);

    //
    // Make sure that the supplied widget is actually in the tree section
    // owned by its parent and, hence, removeable.
    //
    if(!pWidget->pParent || !WidgetIsInTree(pWidget->pParent, pWidget))
    {
        return;
    }

    //
    // See if this widget is the first child of its parent.
    //
    if(pWidget->pParent->pChild == pWidget)
    {
        //
        // Make the first child of this widgets parent be this widget's
        // sibling.
        //
        pWidget->pParent->pChild = pWidget->pNext;
    }
    else
    {
        //
        // Find the sibling directly before this widget.
        //
        for(pTemp = pWidget->pParent->pChild; pTemp->pNext != pWidget;
            pTemp = pTemp->pNext)
        {
        }

        //
        // Make the previous sibling point to the next sibling, removing this
        // widget from the sibling chain.
        //
        pTemp->pNext = pWidget->pNext;
    }

    //
    // Check to see if the widget which currently owns the pointer has just
    // been removed and, if so, clear the pointer focus.
    //
    if(g_pPointerWidget && !WidgetIsInTree(&g_sRoot, g_pPointerWidget))
    {
        g_pPointerWidget = 0;
    }

    //
    // Clear the next pointer of the widget.
    //
    pWidget->pNext = 0;
}

//*****************************************************************************
//
//! Sends a message to a widget tree via a pre-order, depth-first search.
//!
//! \param pWidget is a pointer to the widget tree.
//! \param ulMessage is the message to send.
//! \param ulParam1 is the first parameter to the message.
//! \param ulParam2 is the second parameter to the message.
//! \param bStopOnSuccess is true if the search should be stopped when the
//! first widget is found that returns success in response to the message.
//!
//! This function performs a pre-order, depth-first search of the widget tree,
//! sending a message to each widget encountered.  In a depth-first search, the
//! children of a widget are searched before its siblings (preferring to go
//! deeper into the tree, hence the name depth-first).  A pre-order search
//! means that the message is sent to a widget before any of its children are
//! searched.
//!
//! An example use of the pre-order search is for paint messages; the larger
//! enclosing widgets should be drawn on the screen before the smaller widgets
//! that reside within the parent widget (otherwise, the children would be
//! overwritten by the parent).
//!
//! \return Returns 0 if \e bStopOnSuccess is false or no widget returned
//! success in response to the message, or the value returned by the first
//! widget to successfully process the message.
//
//*****************************************************************************
unsigned long
WidgetMessageSendPreOrder(tWidget *pWidget, unsigned long ulMessage,
                          unsigned long ulParam1, unsigned long ulParam2,
                          unsigned long bStopOnSuccess)
{
    unsigned long ulRet;
    tWidget *pTemp;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Send the message to the initial widget and return if it succeeded and
    // the search should stop on success.
    //
    ulRet = pWidget->pfnMsgProc(pWidget, ulMessage, ulParam1, ulParam2);
    if((ulRet != 0) && bStopOnSuccess)
    {
        return(ulRet);
    }

    //
    // Return if the widget does not have any children.
    //
    if(!pWidget->pChild)
    {
        return(0);
    }

    //
    // Loop through the tree under the widget until every widget is searched.
    //
    for(pTemp = pWidget->pChild; pTemp != pWidget; )
    {
        //
        // Send the message to this widget and return if it succeeded and the
        // search should stop on success.
        //
        ulRet = pTemp->pfnMsgProc(pTemp, ulMessage, ulParam1, ulParam2);
        if((ulRet != 0) && bStopOnSuccess)
        {
            return(ulRet);
        }

        //
        // Find the next widget to examine.  If this widget has a child, then
        // that is the next widget to examine.
        //
        if(pTemp->pChild)
        {
            pTemp = pTemp->pChild;
        }

        //
        // This widget does not have a child, so either a sibling or a parent
        // must be checked.  When moving back to the parent, another move must
        // be performed as well to avoid getting stuck in a loop (since the
        // parent's children have already been searched).
        //
        else
        {
            //
            // Loop until returning to the starting widget.  This loop will be
            // explicitly broken out of if an intervening widget is encountered
            // that has not be searched.
            //
            while(pTemp != pWidget)
            {
                //
                // See if this widget has a sibling.
                //
                if(pTemp->pNext)
                {
                    //
                    // Visit the sibling of this widget.
                    //
                    pTemp = pTemp->pNext;

                    //
                    // Since this widget has not been searched yet, break out
                    // of the controlling loop.
                    //
                    break;
                }
                else
                {
                    //
                    // This widget has no siblings, so go to its parent.  Since
                    // the parent has already been searched, the same sibling
                    // vs. parent decision must be made on this widget as well.
                    //
                    pTemp = pTemp->pParent;
                }
            }
        }
    }

    //
    // No widget returned success for the message, or bStopOnSuccess was zero,
    // so return zero.
    //
    return(0);
}

//*****************************************************************************
//
//! Sends a message to a widget tree via a post-order, depth-first search.
//!
//! \param pWidget is a pointer to the widget tree; if this is zero then the
//! root of the widget tree willb e used.
//! \param ulMessage is the message to send.
//! \param ulParam1 is the first parameter to the message.
//! \param ulParam2 is the second parameter to the message.
//! \param bStopOnSuccess is true if the search should be stopped when the
//! first widget is found that returns success in response to the message.
//!
//! This function performs a post-order, depth-first search of the widget tree,
//! sending a message to each widget encountered.  In a depth-first search, the
//! children of a widget are searched before its sibling (preferring to go
//! deeper into the tree, hence the name depth-first).  A post-order search
//! means that the message is sent to a widget after all of its children are
//! searched.
//!
//! An example use of the post-order search is for pointer-related messages;
//! those messages should be delivered to the lowest widget in the tree before
//! its parents (in other words, the widget deepest in the tree that has a hit
//! should get the message, not the higher up widgets that also include the hit
//! location).
//!
//! Special handling is performed for pointer-related messages.  The widget
//! that accepts \b #WIDGET_MSG_PTR_DOWN is remembered and subsequent
//! \b #WIDGET_MSG_PTR_MOVE and \b #WIDGET_MSG_PTR_UP messages are sent
//! directly to that widget.
//!
//! \return Returns 0 if \e bStopOnSuccess is false or no widget returned
//! success in response to the message, or the value returned by the first
//! widget to successfully process the message.
//
//*****************************************************************************
unsigned long
WidgetMessageSendPostOrder(tWidget *pWidget, unsigned long ulMessage,
                           unsigned long ulParam1, unsigned long ulParam2,
                           unsigned long bStopOnSuccess)
{
    unsigned long ulRet;
    tWidget *pTemp;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // See if this is a pointer move or up message.
    //
    if((ulMessage == WIDGET_MSG_PTR_MOVE) || (ulMessage == WIDGET_MSG_PTR_UP))
    {
        //
        // If there is not a widget that has captured pointer messages, then
        // simply drop this message.
        //
        if(!g_pPointerWidget)
        {
            return(0);
        }

        //
        // Send the message directly to the widget that has captured pointer
        // messages.
        //
        ulRet = g_pPointerWidget->pfnMsgProc(g_pPointerWidget, ulMessage,
                                             ulParam1, ulParam2);

        //
        // See if this is a pointer up message.
        //
        if(ulMessage == WIDGET_MSG_PTR_UP)
        {
            //
            // Since this was a pointer up, the widget no longer has pointer
            // messages captured.
            //
            g_pPointerWidget = 0;
        }

        //
        // Return the value returned by the pointer capture widget.
        //
        return(ulRet);
    }

    //
    // Loop through the tree under the widget until every widget is searched.
    //
    for(pTemp = pWidget; pTemp != pWidget->pParent; )
    {
        //
        // See if this widget has a child.
        //
        if(pTemp->pChild)
        {
            //
            // Go to this widget's child first.
            //
            pTemp = pTemp->pChild;
        }

        //
        // This widget does not have a child, so either a sibling or a parent
        // must be checked.  When moving back to the parent, another move must
        // be performed as well to avoid getting stuck in a loop (since the
        // parent's children have already been searched.
        //
        else
        {
            //
            // Loop until returning to the parent of the starting widget.  This
            // loop will be explicitly broken out of if an intervening widget
            // is encountered that has not been searched.
            //
            while(pTemp != pWidget->pParent)
            {
                //
                // Send the message to this widget.
                //
                ulRet = pTemp->pfnMsgProc(pTemp, ulMessage, ulParam1,
                                          ulParam2);

                //
                // If this is a pointer down message, the widget accepted the
                // message and the handler didn't modify the tree such that
                // this widget is no longer present, then save a pointer to the
                // widget for subsequent pointer move or pointer up messages.
                //
                if((ulMessage == WIDGET_MSG_PTR_DOWN) && (ulRet != 0))
                {
                    //
                    // Is the current widget still in the tree?
                    //
                    if(WidgetIsInTree(&g_sRoot, pTemp))
                    {
                        //
                        // The widget is still in the tree so save it for later
                        // use.
                        //
                        g_pPointerWidget = pTemp;
                    }
                    else
                    {
                        //
                        // Although this widget handled the PTR_DOWN message,
                        // it's message handler rearranged the widget tree and
                        // removed itself so we don't want to send any more
                        // messages directly to it after all.
                        //
                        g_pPointerWidget = 0;
                    }
                }

                //
                // If the widget returned success and the search should stop on
                // success then return immediately.
                //
                if((ulRet != 0) && bStopOnSuccess)
                {
                    return(ulRet);
                }

                //
                // See if this widget has a sibling.
                //
                if(pTemp->pNext)
                {
                    //
                    // Visit the sibling of this widget.
                    //
                    pTemp = pTemp->pNext;

                    //
                    // Since this widget has not been searched yet, break out
                    // of the controlling loop.
                    //
                    break;
                }
                else
                {
                    //
                    // This widget has no siblings, so go to its parent.  Since
                    // the parent has already been searched, the same sibling
                    // vs. parent decision must be made on this widget as well.
                    //
                    pTemp = pTemp->pParent;
                }
            }
        }
    }

    //
    // No widget returned success for the message, or bStopOnSuccess was zero,
    // so return zero.
    //
    return(0);
}

//*****************************************************************************
//
//! Adds a message to the widget message queue.
//!
//! \param pWidget is the widget to which the message should be sent.
//! \param ulMessage is the message to be sent.
//! \param ulParam1 is the first parameter to the message.
//! \param ulParam2 is the second parameter to the message.
//! \param bPostOrder is \b true if the message should be sent via a post-order
//! search, and \b false if it should be sent via a pre-order search.
//! \param bStopOnSuccess is \b true if the message should be sent to widgets
//! until one returns success, and \b false if it should be sent to all
//! widgets.
//!
//! This function places a widget message into the message queue for later
//! processing.  The messages are removed from the queue by
//! WidgetMessageQueueProcess() and sent to the appropriate place.
//!
//! It is safe for code which interrupts WidgetMessageQueueProcess() (or called
//! by it) to call this function to send a message.  It is not safe for code
//! which interrupts this function to call this function as well; it is up to
//! the caller to guarantee that the later sequence never occurs.
//!
//! \return Returns 1 if the message was added to the queue, and 0 if it could
//! not be added since either the queue is full or another context is currently
//! adding a message to the queue.
//
//*****************************************************************************
long
WidgetMessageQueueAdd(tWidget *pWidget, unsigned long ulMessage,
                      unsigned long ulParam1, unsigned long ulParam2,
                      unsigned long bPostOrder, unsigned long bStopOnSuccess)
{
    unsigned long ulNext;
    unsigned long ulOwned;

    //
    // Check the arguments.
    //
    ASSERT(pWidget);

    //
    // Get the mutex we use to protect access to the message queue.
    //
    ulOwned = WidgetMutexGet(&g_ucMQMutex);
    if(ulOwned)
    {
        //
        // The mutex is already being held by some other caller so return a
        // failure.
        //
#ifdef DEBUG_MSGQ
        g_ulMQMutexClash++;
#endif
        return(0);
    }

    //
    // Compute the next value for the write pointer.
    //
    ulNext = (g_ulMQWrite + 1) % QUEUE_SIZE;

    //
    // If the queue is not empty, and this is a pointer move message, see if
    // the previous message was also a move and, if so, replace the
    // coordinates.  Without this, the message queue can very quickly overflow
    // if the application is busy doing something while the user keeps pressing
    // the display.
    //
    if(ulMessage == WIDGET_MSG_PTR_MOVE)
    {
        //
        // Is the message queue empty?
        //
        if(g_ulMQRead != g_ulMQWrite)
        {
            //
            // No - what is the index of the previous message?
            //
            ulOwned = (g_ulMQWrite == 0) ? (QUEUE_SIZE - 1) : (g_ulMQWrite - 1);

            //
            // Was this a pointer move message?
            //
            if(g_pMQ[g_ulMQWrite].ulMessage == WIDGET_MSG_PTR_MOVE)
            {
                //
                // Yes - overwrite this message with the new
                // coordinate information.
                //
                g_pMQ[ulOwned].ulParam1 = ulParam1;
                g_pMQ[ulOwned].ulParam2 = ulParam2;
#ifdef DEBUG_MSGQ
                g_ulMQMoveOverwrite++;
#endif

                //
                // Release the message queue mutex.
                //
                WidgetMutexPut(&g_ucMQMutex);

                //
                // Success.
                //
                return(1);
            }
        }
    }

    //
    // Return a failure if the message queue is full.
    //
    if(ulNext == g_ulMQRead)
    {
#ifdef DEBUG_MSGQ
        g_ulMQOverflow++;
        if(ulMessage != WIDGET_MSG_PTR_MOVE)
        {
            g_ulMQNonMouseOverflow++;
            g_ulMQLastLostMsg = ulMessage;
        }
#endif
        //
        // Release the message queue mutex.
        //
        WidgetMutexPut(&g_ucMQMutex);

        return(0);
    }

    //
    // Write this message into the next location in the message queue.
    //
    g_pMQ[g_ulMQWrite].ulFlags = ((bPostOrder ? MQ_FLAG_POST_ORDER : 0) |
                                  (bStopOnSuccess ? MQ_FLAG_STOP_ON_SUCCESS :
                                   0));
    g_pMQ[g_ulMQWrite].pWidget = pWidget;
    g_pMQ[g_ulMQWrite].ulMessage = ulMessage;
    g_pMQ[g_ulMQWrite].ulParam1 = ulParam1;
    g_pMQ[g_ulMQWrite].ulParam2 = ulParam2;

    //
    // Update the message queue write pointer.
    //
    g_ulMQWrite = ulNext;

    //
    // Release the message queue mutex.
    //
    WidgetMutexPut(&g_ucMQMutex);

    //
    // Success.
    //
    return(1);
}

//*****************************************************************************
//
//! Processes the messages in the widget message queue.
//!
//! This function extracts messages from the widget message queue one at a time
//! and processes them.  If the processing of a widget message requires that a
//! new message be sent, it is acceptable to call WidgetMessageQueueAdd().  It
//! is also acceptable for code which interrupts this function to call
//! WidgetMessageQueueAdd() to send more messages.  In both cases, the newly
//! added message will also be processed before this function returns.
//!
//! \return None.
//
//*****************************************************************************
void
WidgetMessageQueueProcess(void)
{
    tWidget *pWidget;
    unsigned long ulFlags, ulMessage, ulParam1, ulParam2;

    //
    // Loop while there are more messages in the message queue.
    //
    while(g_ulMQRead != g_ulMQWrite)
    {
        //
        // Copy the contents of this message into local variables.
        //
        pWidget = g_pMQ[g_ulMQRead].pWidget;
        ulFlags = g_pMQ[g_ulMQRead].ulFlags;
        ulMessage = g_pMQ[g_ulMQRead].ulMessage;
        ulParam1 = g_pMQ[g_ulMQRead].ulParam1;
        ulParam2 = g_pMQ[g_ulMQRead].ulParam2;

        //
        // Remove this message from the queue.
        //
        g_ulMQRead = (g_ulMQRead + 1) % QUEUE_SIZE;

        //
        // See if this message should be sent via a post-order or pre-order
        // search.
        //
        if(ulFlags & MQ_FLAG_POST_ORDER)
        {
            //
            // Send this message with a post-order search of the widget tree.
            //
            WidgetMessageSendPostOrder(pWidget, ulMessage, ulParam1, ulParam2,
                                       ((ulFlags & MQ_FLAG_STOP_ON_SUCCESS) ?
                                        1 : 0));
        }
        else
        {
            //
            // Send this message with a pre-order search of the widget tree.
            //
            WidgetMessageSendPreOrder(pWidget, ulMessage, ulParam1, ulParam2,
                                      ((ulFlags & MQ_FLAG_STOP_ON_SUCCESS) ?
                                       1 : 0));
        }
    }
}

//*****************************************************************************
//
//! Sends a pointer message.
//!
//! \param ulMessage is the pointer message to be sent.
//! \param lX is the X coordinate associated with the message.
//! \param lY is the Y coordinate associated with the message.
//!
//! This function sends a pointer message to the root widget.  A pointer driver
//! (such as a touch screen driver) can use this function to deliver pointer
//! activity to the widget tree without having to have direct knowledge of the
//! structure of the widget framework.
//!
//! \return Returns 1 if the message was added to the queue, and 0 if it could
//! not be added since the queue is full.
//
//*****************************************************************************
long
WidgetPointerMessage(unsigned long ulMessage, long lX, long lY)
{
    //
    // Add the message to the widget message queue.
    //
    return(WidgetMessageQueueAdd(WIDGET_ROOT, ulMessage, lX, lY, 1, 1));
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
