//*****************************************************************************
//
// usbhostenum.c - Device enumeration code for the USB host library.
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
// This is part of revision 9453 of the Stellaris USB Library.
//
//*****************************************************************************

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"
#include "driverlib/usb.h"
#include "driverlib/rtos_bindings.h"
#include "usblib/usblib.h"
#include "usblib/usblibpriv.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhostpriv.h"
#include "usblib/host/usbhhub.h"

#ifdef INCLUDE_DEBUG_OUTPUT
#include "utils/uartstdio.h"
#define DEBUG_OUTPUT UARTprintf
#else
#define DEBUG_OUTPUT while(0)((int (*)(char *, ...))0)
#endif

//*****************************************************************************
//
//! \addtogroup usblib_hcd
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// External prototypes.
//
//*****************************************************************************
extern tUSBMode g_eUSBMode;

extern void OTGDeviceDisconnect(unsigned long ulIndex);

//*****************************************************************************
//
// Internal function prototypes.
//
//*****************************************************************************
static void USBHCDEP0StateTx(void);
static void USBHCDEnumHandler(void);
static void USBHCDClearFeature(unsigned long ulDevAddress,
                               unsigned long ulEndpoint,
                               unsigned long ulFeature);

//*****************************************************************************
//
// Automatic power enable.
//
//*****************************************************************************
#define USB_HOST_PWREN_AUTO     0x00000002

//*****************************************************************************
//
// Flags used to signal between the interrupt handler and USBHCDMain().
//
//*****************************************************************************
#define INT_EVENT_VBUS_ERR      0x01
#define INT_EVENT_CONNECT       0x02
#define INT_EVENT_DISCONNECT    0x04
#define INT_EVENT_POWER_FAULT   0x08
#define INT_EVENT_SOF           0x10
#define INT_EVENT_ENUM          0x20

volatile unsigned long g_ulUSBHIntEvents;

//*****************************************************************************
//
// Flags used to indicate that a uDMA transfer is pending on a pipe.
//
//*****************************************************************************
#define DMA_PEND_TRANSMIT_FLAG  0x10000
#define DMA_PEND_RECEIVE_FLAG   0x1

volatile unsigned long g_ulDMAPending = 0;

//*****************************************************************************
//
// Flag used to indicate that a workaround should be applied when using
// uDMA with USB.  The uDMA transfers must match the USB FIFO size when
// with Rev A0 silicon.
//
//*****************************************************************************
static unsigned long g_bUseDMAWA = 0;

//*****************************************************************************
//
// This holds the current power configuration that is used when USBHCDInit()
// is called.
//
//*****************************************************************************
static unsigned long g_ulPowerConfig = USBHCD_VBUS_AUTO_HIGH;

//*****************************************************************************
//
// The states for endpoint 0 during enumeration.
//
//*****************************************************************************
typedef enum
{
    //
    // The USB device is waiting on a request from the host controller on
    // endpoint 0.
    //
    EP0_STATE_IDLE,

    //
    // Setup packet is expecting data IN.
    //
    EP0_STATE_SETUP_IN,

    //
    // Setup packet is sending data OUT.
    //
    EP0_STATE_SETUP_OUT,

    //
    // The USB device is receiving data from the device due to an SETUP IN
    // request.
    //
    EP0_STATE_RX,

    //
    // The USB device has completed the IN or OUT request and is now waiting
    // for the host to acknowledge the end of the IN/OUT transaction.  This
    // is the status phase for a USB control transaction.
    //
    EP0_STATE_STATUS,

    //
    // This state is for when a response only has a status phase and no
    // data phase.
    //
    EP0_STATE_STATUS_IN,

    //
    // This endpoint has signaled a stall condition and is waiting for the
    // stall to be acknowledged by the host controller.
    //
    EP0_STATE_STALL,

    //
    // An error has occurred on endpoint 0.
    //
    EP0_STATE_ERROR
}
tEP0State;

//*****************************************************************************
//
// This structure holds the full state for the device enumeration.
//
//*****************************************************************************
typedef struct
{
    //
    // This is the pointer to the current data being sent out or received
    // on endpoint 0.
    //
    unsigned char *pData;

    //
    // This is the number of bytes that remain to be sent from or received
    // into the g_DeviceState.pEP0Data data buffer.
    //
    volatile unsigned long ulBytesRemaining;

    //
    // The amount of data being sent/received due to a request.
    //
    unsigned long ulDataSize;

    //
    // This is the current device address in use by endpoint 0.
    //
    unsigned long ulDevAddress;

    //
    // The maximum packet size for the device responding to the setup packet.
    //
    unsigned long ulMaxPacketSize;

    //
    // The host controller's state.
    //
    tEP0State eState;
}
tHostState;

//*****************************************************************************
//
// This variable holds the current state of endpoint 0.
//
//*****************************************************************************
static volatile tHostState g_sUSBHEP0State =
{
    0,                          // pData
    0,                          // ulBytesRemaining
    0,                          // ulDataSize
    0,                          // ulDevAddress
    0,                          // ulMaxPacketSize
    EP0_STATE_IDLE              // eState
};

//*****************************************************************************
//
// The global delay time for use by SysCtlDelay() function.  This is
// initialized to an appropriate value for a 50MHz clock.  The correct value
// will be set in USBHCDInit().
//
//*****************************************************************************
static unsigned long g_ulTickms = (50000000 / 3000);
static volatile unsigned long g_ulCurrentTick = 0;

//*****************************************************************************
//
// The current active drivers.
//
//*****************************************************************************
static long g_lUSBHActiveDriver[MAX_USB_DEVICES + 1];
static void *g_pvDriverInstance[MAX_USB_DEVICES + 1];

//*****************************************************************************
//
// This is the structure used to hold the information for a given USB pipe
// that is attached to a device.
//
//*****************************************************************************
typedef struct
{
    //
    // The current address for this pipe.
    //
    tUSBHostDevice *psDevice;

    //
    // The current address for this pipe.
    //
    unsigned char ucEPNumber;

    //
    // The DMA channel assigned to this endpoint.
    //
    unsigned char ucDMAChannel;

    //
    // The current type for this pipe.
    //
    unsigned long ulType;

    //
    // The millisecond interval for this pipe.
    //
    unsigned long ulInterval;

    //
    // The next tick value to trigger and event on this pipe.
    //
    unsigned long ulNextEventTick;

    //
    // The current call back for this pipe.
    //
    tHCDPipeCallback pfnCallback;

    //
    // The pointer to which IN data must be copied.
    //
    unsigned char *pucReadPtr;

    //
    // The number of bytes of read data to copy.
    //
    unsigned long ulReadSize;

    //
    // The state of a given USB pipe.
    //
    volatile enum
    {
        PIPE_READING,
        PIPE_DATA_READY,
        PIPE_DATA_SENT,
        PIPE_WRITING,
        PIPE_STALLED,
        PIPE_ERROR,
        PIPE_IDLE,
        PIPE_DISABLED
    }
    eState;

    //
    // The actual FIFO offset allocated to this endpoint.
    //
    unsigned short usFIFOAddr;

    //
    // The size of the FIFO entry based on the size parameter.  These are
    // equivalent to the USB_FIFO_SZ_* values in usb.h.
    //
    unsigned char ucFIFOSize;

    //
    // The bit offset in the allocation structure.
    //
    unsigned char ucFIFOBitOffset;
}
tUSBHCDPipe;

//*****************************************************************************
//
// The internal state of the device.
//
//*****************************************************************************
typedef enum
{
    HCD_DEV_DISCONNECTED,
    HCD_DEV_CONNECTED,
    HCD_DEV_REQUEST,
    HCD_DEV_RESET,
    HCD_DEV_ADDRESSED,
    HCD_DEV_CONFIGURED,
    HCD_DEV_GETSTRINGS,
    HCD_DEV_ERROR,
    HCD_VBUS_ERROR,
    HCD_POWER_FAULT,
    HCD_IDLE
}
tUSBHDeviceState;

static void ProcessUSBDeviceStateMachine(tUSBHDeviceState eOldState,
                                         unsigned long ulDevIndex);

//*****************************************************************************
//
// This is a fixed number as it relates to the maximum number of USB pipes
// available on any USB controller.  The actual number on a given device may
// be less than this number.
//
//*****************************************************************************
#define MAX_NUM_PIPES           15

//*****************************************************************************
//
// This is a fixed number as it relates to the number of USB pipes available
// in the USB controller.
//
//*****************************************************************************
#define MAX_NUM_DMA_CHANNELS    6

//*****************************************************************************
//
// Marker for an unused DMA channel slot.
//
//*****************************************************************************
#define USBHCD_DMA_UNUSED       0xff

//*****************************************************************************
//
// These definitions are used to manipulate the values returned as allocated
// USB pipes.
//
//*****************************************************************************
#define EP_PIPE_TYPE_LOW_SPEED  0x02000000
#define EP_PIPE_USE_UDMA        0x01000000
#define EP_PIPE_TYPE_ISOC       0x00800000
#define EP_PIPE_TYPE_INTR       0x00400000
#define EP_PIPE_TYPE_BULK       0x00200000
#define EP_PIPE_TYPE_CONTROL    0x00100000
#define EP_PIPE_TYPE_IN         0x00020000
#define EP_PIPE_TYPE_OUT        0x00010000
#define EP_PIPE_IDX_M           0x0000ffff

//*****************************************************************************
//
// This creates a USB pipe handle from an index.
//
//*****************************************************************************
#define OUT_PIPE_HANDLE(ulIdx)  (g_sUSBHCD.USBOUTPipes[ulIdx].ulType | ulIdx)
#define IN_PIPE_HANDLE(ulIdx)   (g_sUSBHCD.USBINPipes[ulIdx].ulType | ulIdx)

//*****************************************************************************
//
// Converts from an endpoint specifier to the offset of the endpoint's
// control/status registers.
//
//*****************************************************************************
#define EP_OFFSET(Endpoint)     (Endpoint - 0x10)

//*****************************************************************************
//
// This structure holds the state information for a given host controller.
//
//*****************************************************************************
typedef struct
{
    unsigned long ulUSBBase;

    tUSBHCDPipe USBControlPipe;
    tUSBHCDPipe USBOUTPipes[MAX_NUM_PIPES];
    tUSBHCDPipe USBINPipes[MAX_NUM_PIPES];
    unsigned char ucDMAChannels[MAX_NUM_DMA_CHANNELS];

    //
    // Each devices state.  We support a total of (MAX_USB_DEVICES + 1) devices
    // to allow for the use if MAX_USB_DEVICES through a single hub (which is
    // itself a device).
    //
    tUSBHostDevice USBDevice[MAX_USB_DEVICES + 1];

    //
    // Holds the current state of the device.
    //
    volatile tUSBHDeviceState eDeviceState[MAX_USB_DEVICES + 1];

    //
    // Pointer to the memory pool for this controller.
    //
    void *pvPool;

    //
    // The pool size for this controller.
    //
    unsigned long ulPoolSize;

    //
    // The number of endpoint pairs supported by the controller.
    //
    unsigned long ulNumEndpoints;

    //
    // The class drivers for this controller.
    //
    const tUSBHostClassDriver * const *pClassDrivers;

    //
    // The number of class drivers.
    //
    unsigned long ulNumClassDrivers;

    //
    // This is the index in the driver list of the event driver.
    //
    long lEventDriver;

    //
    // These are the generic event information used by the event driver.
    //
    unsigned long ulEventEnables;

    unsigned long ulClass;
}
tUSBHCD;

//*****************************************************************************
//
// The global to hold all of the state information for a given host controller.
//
//*****************************************************************************
static tUSBHCD g_sUSBHCD;

//*****************************************************************************
//
// Return the device index from a ulInstance value passed from an external
// source.
//
//*****************************************************************************
static unsigned char
HCDInstanceToDevIndex(unsigned long ulInstance)
{
    unsigned long ulDevIndex;

    //
    // Get the device instance from the instance value.
    //
    ulDevIndex = (ulInstance & 0xff);

    //
    // If the above math went negative or is too large just return 0xff.
    //
    if(ulDevIndex > MAX_USB_DEVICES)
    {
        ulDevIndex = 0xff;
    }

    return(ulDevIndex);
}

//=============================================================================
//
// This is the internal function that will map an event to a valid event flag.
//
// \param ulEvent specifies which event flag to retrieve.
//
// \return The event flag or 0 if there is no support event flag for the
// event specified by the \e ulEvent parameter.
//
//=============================================================================
static unsigned long
GetEventFlag(unsigned long ulEvent)
{
    unsigned long ulEventFlag;

    ulEventFlag = 0;

    //
    // Search for a valid event flag for the requested event.
    //
    switch(ulEvent)
    {
        case USB_EVENT_SOF:
        {
            ulEventFlag |= USBHCD_EVFLAG_SOF;
            break;
        }
        case USB_EVENT_CONNECTED:
        {
            ulEventFlag |= USBHCD_EVFLAG_CONNECT;
            break;
        }
        case USB_EVENT_DISCONNECTED:
        {
            ulEventFlag |= USBHCD_EVFLAG_DISCNCT;
            break;
        }
        case USB_EVENT_UNKNOWN_CONNECTED:
        {
            ulEventFlag |= USBHCD_EVFLAG_UNKCNCT;
            break;
        }
        case USB_EVENT_POWER_FAULT:
        {
            ulEventFlag |= USBHCD_EVFLAG_PWRFAULT;
            break;
        }
        case USB_EVENT_POWER_DISABLE:
        {
            ulEventFlag |= USBHCD_EVFLAG_PWRDIS;
            break;
        }
        case USB_EVENT_POWER_ENABLE:
        {
            ulEventFlag |= USBHCD_EVFLAG_PWREN;
            break;
        }
        default:
        {
            break;
        }
    }
    return(ulEventFlag);
}

//=============================================================================
//
//! This function is called to enable a specific USB HCD event notification.
//!
//! \param ulIndex specifies which USB controller to use.
//! \param pvEventDriver is the event driver structure that was passed into
//! the USBHCDRegisterDrivers() function as part of the array of
//! tUSBHostClassDriver structures.
//! \param ulEvent is the event to enable.
//!
//! This function is called to enable event callbacks for a specific USB HCD
//! event.  The requested event is passed in the \e ulEvent parameter.  Not
//! all events can be enables so the function will return zero if the event
//! provided cannot be enabled.  The \e pvEventDriver is a pointer to the
//! event driver structure that the caller passed into the
//! USBHCDRegisterDrivers() function.  This structure is typically declared
//! with the DECLARE_EVENT_DRIVER() macro and included as part of the array
//! of pointers to tUSBHostClassDriver structures that is passed to the
//! USBHCDRegisterDrivers() function.
//!
//! \return This function returns a non-zero number if the event was
//! successfully enabled and returns zero if the event cannot be enabled.
//
//=============================================================================
long
USBHCDEventEnable(unsigned long ulIndex, void *pvEventDriver,
                  unsigned long ulEvent)
{
    long lRet;
    unsigned long ulEventFlag;

    ASSERT(ulIndex == 0);

    //
    // Default the return to fail the call unless a valid event is found.
    //
    lRet = 0;

    //
    // Get the event flag for this event.
    //
    ulEventFlag = GetEventFlag(ulEvent);

    //
    // Check if there was an event flag for the corresponding event.
    //
    if(ulEventFlag)
    {
        //
        // Set the enable for this event.
        //
        g_sUSBHCD.ulEventEnables |= ulEventFlag;

        //
        // Indicate that the event was valid and is now enabled.
        //
        lRet = 1;
    }

    return(lRet);
}

//=============================================================================
//
//! This function is called to disable a specific USB HCD event notification.
//!
//! \param ulIndex specifies which USB controller to use.
//! \param pvEventDriver is the event driver structure that was passed into
//! the USBHCDRegisterDrivers() function as part of the array of
//! tUSBHostClassDriver structures.
//! \param ulEvent is the event to disable.
//!
//! This function is called to disable event callbacks for a specific USB HCD
//! event.  The requested event is passed in the \e ulEvent parameter.  Not
//! all events can be enables so the function will return zero if the event
//! provided cannot be enabled.  The \e pvEventDriver is a pointer to the
//! event driver structure that the caller passed into the
//! USBHCDRegisterDrivers() function.  This structure is typically declared
//! with the DECLARE_EVENT_DRIVER() macro and included as part of the array
//! of pointers to tUSBHostClassDriver structures that is passed to the
//! USBHCDRegisterDrivers() function.
//!
//! \return This function returns a non-zero number if the event was
//! successfully disabled and returns zero if the event cannot be disabled.
//
//=============================================================================
long
USBHCDEventDisable(unsigned long ulIndex, void *pvEventDriver,
                   unsigned long ulEvent)
{
    long lRet;
    unsigned long ulEventFlag;

    ASSERT(ulIndex == 0);

    //
    // Default the return to fail the call unless a valid event is found.
    //
    lRet = 0;

    //
    // Get the event flag for this event.
    //
    ulEventFlag = GetEventFlag(ulEvent);

    //
    // Check if there was an event flag for the corresponding event.
    //
    if(ulEventFlag)
    {
        //
        // Clear the enable for this event.
        //
        g_sUSBHCD.ulEventEnables &= ~ulEventFlag;

        //
        // Indicate that the event was valid and is now disabled.
        //
        lRet = 1;
    }

    return(lRet);
}

//*****************************************************************************
//
// If there is an event driver this function will send out a generic connection
// event USB_EVENT_UNKNOWN_CONNECTED indicating that an unknown connection
// event has occurred.
//
//*****************************************************************************
static void
SendUnknownConnect(unsigned long ulIndex, unsigned long ulClass)
{
    tEventInfo sEvent;

    //
    // If there is an event driver registered and it has a event handler and
    // the USBHCD_EVFLAG_UNKCNCT is enabled then call the function.
    //
    sEvent.ulEvent = USB_EVENT_UNKNOWN_CONNECTED;
    sEvent.ulInstance = ulClass;
    InternalUSBHCDSendEvent(0, &sEvent, USBHCD_EVFLAG_UNKCNCT);
}

//*****************************************************************************
//
// Internal memory allocation space is two unsigned long values where each
// bit represents a 64 byte block in the FIFO.  This requires 64 bits for
// the 4096 bytes of FIFO available.
//
//*****************************************************************************
static unsigned long g_ulAlloc[2];

//*****************************************************************************
//
// This function handles freeing FIFO memory that has been allocated using the
// FIFOAlloc() function.
//
//*****************************************************************************
static void
FIFOFree(tUSBHCDPipe *pUSBPipe)
{
    unsigned long ulMask;

    //
    // Calculate the mask value to use to clear off the allocated blocks used
    // by the USB pipe specified by pUSBPipe.
    //
    ulMask = (1 << (pUSBPipe->ucFIFOSize - 2)) - 1;
    ulMask = ulMask << pUSBPipe->ucFIFOBitOffset;

    //
    // Determine which 32 bit word to access based on the size.
    //
    if(pUSBPipe->ucFIFOSize > USB_FIFO_SZ_64)
    {
        //
        // If the FIFO size is greater than 64 then use the upper 32 bits.
        //
        g_ulAlloc[1] &= ~ulMask;
    }
    else
    {
        //
        // If the FIFO size is less than or equal to 64 then use the lower
        // 32 bits.
        //
        g_ulAlloc[0] &= ~ulMask;
    }
}

//*****************************************************************************
//
// This function is used to allocate FIFO memory to a given USB pipe.
//
// \param pUSBPipe is the USB pipe that needs FIFO memory allocated.
// \param ulSize is the minimum size in bytes of the FIFO to allocate.
//
// This function will allocate \e ulSize bytes to the USB pipe in the
// \e pUSBPipe parameter.  The function will fill the pUSBPipe structure
// members ucFIFOSize and ucFIFOAddr with values that can be used with the
// USBFIFOConfigSet() API.  This allocation uses a first fit algorithm.
//
// \return This function returns the size of the block allocated.
//
//*****************************************************************************
static unsigned long
FIFOAlloc(tUSBHCDPipe *pUSBPipe, unsigned long ulSize)
{
    unsigned long ulBlocks, ulStart, ulBlockSize;
    unsigned short usFIFOAddr;
    unsigned long ulTemp, ulIndex;

    //
    // Save which 32 bit value to access, the upper is for blocks greater
    // than 64 and the lower is for block 64 or less.
    //
    if(ulSize > 64)
    {
        ulIndex = 1;
    }
    else
    {
        ulIndex = 0;
    }

    //
    // Initial FIFO address is 0.
    //
    usFIFOAddr = 0;

    //
    // Initialize the bit pattern and bit location.
    //
    ulBlocks = 1;
    ulStart = 0;

    //
    // The initial block size is always the minimum size of 64 bytes.
    //
    ulBlockSize = 64;

    //
    // The initial size and offset are 64 and 0.
    //
    pUSBPipe->ucFIFOBitOffset = 0;
    pUSBPipe->ucFIFOSize = 3;

    //
    // Scan through 32 bits looking for a memory block large enough to fill
    // the request.
    //
    while(usFIFOAddr <= 32)
    {
        //
        // If the pattern is zero then it is a possible match.
        //
        if((g_ulAlloc[ulIndex] & ulBlocks) == 0)
        {
            //
            // If the size is large enough then save it and break out of the
            // loop.
            //
            if(ulBlockSize >= ulSize)
            {
                //
                // Mark the memory as allocated.
                //
                g_ulAlloc[ulIndex] |= ulBlocks;

                break;
            }

            //
            // Increment the size of the FIFO block.
            //
            pUSBPipe->ucFIFOSize++;

            //
            // Add in a new bit to the size of the allocation.
            //
            ulBlocks = ulBlocks | (ulBlocks << 1) ;

            //
            // Double the current size.
            //
            ulBlockSize <<= 1;

        }
        else
        {
            //
            // Need to start over looking because the last allocation match
            // failed, so reset the bit offset to the current location and the
            // size to 64 bytes.
            //
            pUSBPipe->ucFIFOBitOffset = usFIFOAddr;
            pUSBPipe->ucFIFOSize = 3;

            //
            // Reset the block size to the minimum (64 bytes).
            //
            ulBlockSize = 64;

            //
            // Store the current starting bit location and set the block mask
            // to this value.
            //
            ulStart = 1 << usFIFOAddr;
            ulBlocks = ulStart;
        }

        //
        // Increase the address of the FIFO offset.
        //
        usFIFOAddr++;
    }

    //
    // If there was no block large enough then fail this call.
    //
    if(usFIFOAddr > 32)
    {
        ulBlockSize = 0;
        pUSBPipe->usFIFOAddr = 0;
        pUSBPipe->ucFIFOBitOffset = 0;
        pUSBPipe->ucFIFOSize = 0;
    }
    else
    {
        //
        // Calculate the offset in the FIFO.
        //
        ulTemp = pUSBPipe->ucFIFOBitOffset * 64;

        //
        // Sizes greater than 64 are allocated in the second half of the FIFO
        // memory space.
        //
        if(ulSize > 64)
        {
            ulTemp += 2048;
        }

        //
        // Convert this to the value that can be set in the USB controller.
        //
        pUSBPipe->usFIFOAddr = (unsigned short)ulTemp;
    }
    return(ulBlockSize);
}

//*****************************************************************************
//
//! This function is used to allocate a USB HCD pipe.
//!
//! \param ulIndex specifies which USB controller to use.
//! \param ulEndpointType is the type of endpoint that this pipe will be
//! communicating with.
//! \param psDevice is the device instance associated with this endpoint.
//! \param ulSize is the size of the FIFO in bytes.
//! \param pfnCallback is the function that will be called when events occur on
//! this USB Pipe.
//!
//! Since there are a limited number of USB HCD pipes that can be used in the
//! host controller, this function is used to temporarily or permanently
//! acquire one of the endpoints.  Unlike the USBHCDPipeAlloc() function this
//! function allows the caller to specify the size of the FIFO allocated to
//! this endpoint in the \e ulSize parameter.  This function also provides a
//! method to register a callback for status changes on this endpoint.  If no
//! callbacks are desired then the \e pfnCallback function should be set to 0.
//! The callback should be used when using the USBHCDPipeSchedule() function
//! so that the caller is notified when the action is complete.
//!
//! \return This function returns a value indicating which pipe was reserved.
//! If the value is 0 then there were no pipes currently available.  This value
//! should be passed to any USBHCDPipe APIs to indicate which pipe is being
//! accessed.
//
//*****************************************************************************
unsigned long
USBHCDPipeAllocSize(unsigned long ulIndex, unsigned long ulEndpointType,
                    tUSBHostDevice *psDevice, unsigned long ulSize,
                    tHCDPipeCallback pfnCallback)
{
    long lIdx, lDMAIdx;
    unsigned long ulHubAddr;

    ASSERT(ulIndex == 0);

    //
    // Find a USB pipe that is free.
    //
    for(lIdx = 0; lIdx < MAX_NUM_PIPES; lIdx++)
    {
        //
        // Handle OUT Pipes.
        //
        if(ulEndpointType & EP_PIPE_TYPE_OUT)
        {
            //
            // A zero address indicates free.
            //
            if(g_sUSBHCD.USBOUTPipes[lIdx].psDevice == 0)
            {
                //
                // Set up uDMA for the pipe.
                //
                if(ulEndpointType & EP_PIPE_USE_UDMA)
                {
                    //
                    // First three endpoints have fixed channels on some
                    // parts so bias the pipes to match this as best is
                    // possible.
                    //
                    if(lIdx < 3)
                    {
                        //
                        // Check if the fixed channel is available for this
                        // USB pipe.
                        //
                        if((g_sUSBHCD.ucDMAChannels[1 + (lIdx * 2)] ==
                                      USBHCD_DMA_UNUSED))
                        {
                            //
                            // The default channel was available so use it.
                            //
                            g_sUSBHCD.ucDMAChannels[1 + (lIdx * 2)] = lIdx;
                            g_sUSBHCD.USBOUTPipes[lIdx].ucDMAChannel =
                                1 + (lIdx * 2);
                        }
                        else
                        {
                            //
                            // Go to the next USB pipe if the fixed one was not
                            // available.
                            //
                            continue;
                        }
                    }
                    else
                    {
                        //
                        // Either the fixed channel was not available or the
                        // pipe index was more than the first 3 pipes that are
                        // available on all parts.
                        //
                        for(lDMAIdx = 1; lDMAIdx < MAX_NUM_DMA_CHANNELS;
                            lDMAIdx += 2)
                        {
                            //
                            // Find any available channel.
                            //
                            if(g_sUSBHCD.ucDMAChannels[lDMAIdx] ==
                               USBHCD_DMA_UNUSED)
                            {
                                //
                                // Save the index and the DMA channel
                                // information.
                                //
                                g_sUSBHCD.ucDMAChannels[lDMAIdx] = lIdx;
                                g_sUSBHCD.USBOUTPipes[lIdx].ucDMAChannel =
                                    lDMAIdx;
                            }
                        }
                    }

                    //
                    // If no DMA channel was available then just disable DMA
                    // on this pipe.
                    //
                    if(g_sUSBHCD.USBOUTPipes[lIdx].ucDMAChannel ==
                       USBHCD_DMA_UNUSED)
                    {
                        ulEndpointType &= ~EP_PIPE_USE_UDMA;
                    }
                    else
                    {
                        //
                        // Set the DMA channel for this endpoint, this has no
                        // effect on parts without configurable DMA.
                        //
                        MAP_USBEndpointDMAChannel(
                            USB0_BASE, INDEX_TO_USB_EP(lIdx + 1),
                            g_sUSBHCD.USBOUTPipes[lIdx].ucDMAChannel);

                        //
                        // Clear all the attributes for the channel
                        //
                        MAP_uDMAChannelAttributeDisable(
                            g_sUSBHCD.USBOUTPipes[lIdx].ucDMAChannel,
                            UDMA_ATTR_ALL);

                        //
                        // Configure the uDMA channel for the pipe
                        //
                        MAP_uDMAChannelControlSet(
                            g_sUSBHCD.USBOUTPipes[lIdx].ucDMAChannel,
                            (UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE |
                             UDMA_ARB_64));
                    }
                }

                //
                // Save the endpoint type and device address and callback
                // function.
                //
                g_sUSBHCD.USBOUTPipes[lIdx].ulType = ulEndpointType;
                g_sUSBHCD.USBOUTPipes[lIdx].psDevice = psDevice;
                g_sUSBHCD.USBOUTPipes[lIdx].pfnCallback = pfnCallback;

                //
                // Clear out any pending status on this endpoint in case it
                // was in use before a allowing a new device class to use it.
                //
                MAP_USBHostEndpointStatusClear(USB0_BASE,
                                               INDEX_TO_USB_EP(lIdx + 1),
                                               USB_HOST_OUT_STATUS);

                //
                // Initialize the endpoint as idle.
                //
                g_sUSBHCD.USBOUTPipes[lIdx].eState = PIPE_IDLE;

                //
                // Allocate space in the FIFO for this endpoint.
                //
                if(FIFOAlloc(&g_sUSBHCD.USBOUTPipes[lIdx], ulSize) != 0)
                {
                    //
                    // Configure the FIFO.
                    //
                    MAP_USBFIFOConfigSet(USB0_BASE, INDEX_TO_USB_EP(lIdx + 1),
                                         g_sUSBHCD.USBOUTPipes[lIdx].usFIFOAddr,
                                         g_sUSBHCD.USBOUTPipes[lIdx].ucFIFOSize,
                                         USB_EP_HOST_OUT);
                }

                //
                // Set the function address for this endpoint.
                //
                MAP_USBHostAddrSet(USB0_BASE, INDEX_TO_USB_EP(lIdx + 1),
                                   psDevice->ulAddress, USB_EP_HOST_OUT);

                //
                // Set the hub and port address for the endpoint.
                //
                ulHubAddr = (psDevice->ucHub << 8) |
                             psDevice->ucHubPort;
                USBHostHubAddrSet(USB0_BASE, INDEX_TO_USB_EP(lIdx + 1),
                                  ulHubAddr, (USB_EP_HOST_OUT |
                               (psDevice->bLowSpeed ?
                                  USB_EP_SPEED_LOW : USB_EP_SPEED_FULL)));

                break;
            }
        }
        //
        // Handle IN Pipes.
        //
        else if(ulEndpointType & EP_PIPE_TYPE_IN)
        {
            //
            // A zero address indicates free.
            //
            if(g_sUSBHCD.USBINPipes[lIdx].psDevice == 0)
            {
                //
                // Set up uDMA for the pipe.
                //
                if(ulEndpointType & EP_PIPE_USE_UDMA)
                {
                    //
                    // First three endpoints have fixed channels on some
                    // parts so bias the pipes to match this as best is
                    // possible.
                    //
                    if(lIdx < 3)
                    {
                        //
                        // Check if the fixed channel is available for this
                        // USB pipe.
                        //
                        if(g_sUSBHCD.ucDMAChannels[lIdx * 2] ==
                           USBHCD_DMA_UNUSED)
                        {
                            //
                            // The default channel was available so use it.
                            //
                            g_sUSBHCD.ucDMAChannels[lIdx * 2] = lIdx;
                            g_sUSBHCD.USBINPipes[lIdx].ucDMAChannel = lIdx * 2;
                        }
                        else
                        {
                            //
                            // Go to the next USB pipe if the fixed one was not
                            // available.
                            //
                            continue;
                        }
                    }
                    else
                    {
                        //
                        // Either the fixed channel was not available or the
                        // pipe index was more than the first 3 pipes that are
                        // available on all parts.
                        //
                        for(lDMAIdx = 0; lDMAIdx < MAX_NUM_DMA_CHANNELS;
                            lDMAIdx += 2)
                        {
                            //
                            // Find any available channel.
                            //
                            if(g_sUSBHCD.ucDMAChannels[lDMAIdx] ==
                               USBHCD_DMA_UNUSED)
                            {
                                //
                                // Save the index and the DMA channel
                                // information.
                                //
                                g_sUSBHCD.ucDMAChannels[lDMAIdx] = lIdx;
                                g_sUSBHCD.USBINPipes[lIdx].ucDMAChannel =
                                    lDMAIdx;
                            }
                        }
                    }

                    //
                    // If no DMA channel was available then just disable DMA
                    // on this pipe.
                    //
                    if(g_sUSBHCD.USBINPipes[lIdx].ucDMAChannel ==
                       USBHCD_DMA_UNUSED)
                    {
                        ulEndpointType &= ~EP_PIPE_USE_UDMA;
                    }
                    else
                    {
                        //
                        // Set the DMA channel for this endpoint, this has no
                        // effect on parts without configurable DMA.
                        //
                        //
                        MAP_USBEndpointDMAChannel(
                            USB0_BASE, INDEX_TO_USB_EP(lIdx + 1),
                            g_sUSBHCD.USBINPipes[lIdx].ucDMAChannel);

                        //
                        // Clear all the attributes for the channel
                        //
                        MAP_uDMAChannelAttributeDisable(
                            g_sUSBHCD.USBINPipes[lIdx].ucDMAChannel,
                            UDMA_ATTR_ALL);

                        //
                        // Configure the uDMA channel for the pipe
                        //
                        MAP_uDMAChannelControlSet(
                            g_sUSBHCD.USBINPipes[lIdx].ucDMAChannel,
                            (UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 |
                             UDMA_ARB_64));
                    }
                }

                //
                // Save the endpoint type and device address and callback
                // function.
                //
                g_sUSBHCD.USBINPipes[lIdx].ulType = ulEndpointType;
                g_sUSBHCD.USBINPipes[lIdx].psDevice = psDevice;
                g_sUSBHCD.USBINPipes[lIdx].pfnCallback = pfnCallback;

                //
                // Clear out any pending status on this endpoint in case it
                // was in use before a allowing a new device class to use it.
                //
                MAP_USBHostEndpointStatusClear(USB0_BASE,
                                               INDEX_TO_USB_EP(lIdx + 1),
                                               USB_HOST_IN_STATUS);

                //
                // Allocate space in the FIFO for this endpoint.
                //
                if(FIFOAlloc(&g_sUSBHCD.USBINPipes[lIdx], ulSize) != 0)
                {
                    //
                    // Configure the FIFO.
                    //
                    MAP_USBFIFOConfigSet(USB0_BASE, INDEX_TO_USB_EP(lIdx + 1),
                                         g_sUSBHCD.USBINPipes[lIdx].usFIFOAddr,
                                         g_sUSBHCD.USBINPipes[lIdx].ucFIFOSize,
                                         USB_EP_HOST_IN);
                }

                //
                // Set the function address for this endpoint.
                //
                MAP_USBHostAddrSet(USB0_BASE, INDEX_TO_USB_EP(lIdx + 1),
                                   psDevice->ulAddress, USB_EP_HOST_IN);

                //
                // Set the hub and port address for the endpoint.
                //
                ulHubAddr = (psDevice->ucHub << 8) |
                             psDevice->ucHubPort;
                USBHostHubAddrSet(USB0_BASE, INDEX_TO_USB_EP(lIdx + 1),
                                  ulHubAddr, (USB_EP_HOST_IN |
                                (psDevice->bLowSpeed ?
                                  USB_EP_SPEED_LOW : USB_EP_SPEED_FULL)));

                //
                // Reset the state of the pipe to idle.
                //
                g_sUSBHCD.USBINPipes[lIdx].eState = PIPE_IDLE;

                break;
            }
        }
    }

    //
    // Did not find a free pipe.
    //
    if(lIdx == MAX_NUM_PIPES)
    {
        return(0);
    }

    //
    // Return the pipe index and type that was allocated.
    //
    return(ulEndpointType | lIdx);
}

//*****************************************************************************
//
//! This function is used to allocate a USB HCD pipe.
//!
//! \param ulIndex specifies which USB controller to use.
//! \param ulEndpointType is the type of endpoint that this pipe will be
//! communicating with.
//! \param psDevice is the device instance associated with this endpoint.
//! \param pfnCallback is the function that will be called when events occur on
//! this USB Pipe.
//!
//! Since there are a limited number of USB HCD pipes that can be used in the
//! host controller, this function is used to temporarily or permanently
//! acquire one of the endpoints.  It also provides a method to register a
//! callback for status changes on this endpoint.  If no callbacks are desired
//! then the \e pfnCallback function should be set to 0.  The callback should
//! be used when using the USBHCDPipeSchedule() function so that the caller is
//! notified when the action is complete.
//!
//! \return This function returns a value indicating which pipe was reserved.
//! If the value is 0 then there were no pipes currently available.  This value
//! should be passed to any USBHCDPipe APIs to indicate which pipe is being
//! accessed.
//
//*****************************************************************************
unsigned long
USBHCDPipeAlloc(unsigned long ulIndex, unsigned long ulEndpointType,
                tUSBHostDevice *psDevice, tHCDPipeCallback pfnCallback)
{
    //
    // The old API allocated only 64 bytes to each endpoint.
    //
    return(USBHCDPipeAllocSize(ulIndex, ulEndpointType, psDevice, 64,
                               pfnCallback));
}

//*****************************************************************************
//
//! This function is used to configure a USB HCD pipe.
//!
//! This should be called after allocating a USB pipe with a call to
//! USBHCDPipeAlloc().  It is used to set the configuration associated with an
//! endpoint like the max payload and target endpoint.  The \e ulMaxPayload
//! parameter is typically read directly from the devices endpoint descriptor
//! and is expressed in bytes.
//!
//! Setting the \e ulInterval parameter depends on the type of endpoint being
//! configured.  For endpoints that do not need to use the \e ulInterval
//! parameter \e ulInterval should be set to 0.  For Bulk \e ulInterval is a
//! value from 2-16 and will set the NAK timeout value as 2^(\e ulInterval-1)
//! frames.  For interrupt endpoints \e ulInterval is a value from 1-255 and
//! is the count in frames between polling the endpoint.  For isochronous
//! endpoints \e ulInterval ranges from 1-16 and is the polling interval in
//! frames represented as 2^(\e ulInterval-1) frames.
//!
//! \param ulPipe is the allocated endpoint to modify.
//! \param ulMaxPayload is maximum data that can be handled per transaction.
//! \param ulInterval is the polling interval for data transfers expressed in
//! frames.
//! \param ulTargetEndpoint is the target endpoint on the device to communicate
//! with.
//!
//! \return If the call was successful, this function returns zero any other
//! value indicates an error.
//
//*****************************************************************************
unsigned long
USBHCDPipeConfig(unsigned long ulPipe, unsigned long ulMaxPayload,
                 unsigned long ulInterval, unsigned long ulTargetEndpoint)
{
    unsigned long ulFlags;
    unsigned long ulIndex;

    //
    // Get the index number from the allocated pipe.
    //
    ulIndex = (ulPipe & EP_PIPE_IDX_M);

    //
    // Set the direction.
    //
    if(ulPipe & EP_PIPE_TYPE_OUT)
    {
        //
        // Set the mode for this endpoint.
        //
        if(g_sUSBHCD.USBOUTPipes[ulIndex].ulType & EP_PIPE_TYPE_BULK)
        {
            ulFlags = USB_EP_MODE_BULK;
        }
        else if(g_sUSBHCD.USBOUTPipes[ulIndex].ulType & EP_PIPE_TYPE_INTR)
        {
            ulFlags = USB_EP_MODE_INT;
        }
        else if(g_sUSBHCD.USBOUTPipes[ulIndex].ulType & EP_PIPE_TYPE_ISOC)
        {
            ulFlags = USB_EP_MODE_ISOC;
        }
        else
        {
            ulFlags = USB_EP_MODE_CTRL;
        }

        ulFlags |= USB_EP_HOST_OUT;

        g_sUSBHCD.USBOUTPipes[ulIndex].ucEPNumber =
            (unsigned char)ulTargetEndpoint;

        //
        // Save the interval and the next tick to trigger a scheduler event.
        //
        g_sUSBHCD.USBOUTPipes[ulIndex].ulInterval = ulInterval;
        g_sUSBHCD.USBOUTPipes[ulIndex].ulNextEventTick =
            ulInterval + g_ulCurrentTick;

        //
        // Set the device speed.
        //
        ulFlags |= (g_sUSBHCD.USBOUTPipes[ulIndex].psDevice->bLowSpeed ?
                                      USB_EP_SPEED_LOW : USB_EP_SPEED_FULL);
    }
    else
    {
        //
        // Set the mode for this endpoint.
        //
        if(g_sUSBHCD.USBINPipes[ulIndex].ulType & EP_PIPE_TYPE_BULK)
        {
            ulFlags = USB_EP_MODE_BULK;
        }
        else if(g_sUSBHCD.USBINPipes[ulIndex].ulType & EP_PIPE_TYPE_INTR)
        {
            ulFlags = USB_EP_MODE_INT;
        }
        else if(g_sUSBHCD.USBINPipes[ulIndex].ulType & EP_PIPE_TYPE_ISOC)
        {
            ulFlags = USB_EP_MODE_ISOC;
        }
        else
        {
            ulFlags = USB_EP_MODE_CTRL;
        }
        ulFlags |= USB_EP_HOST_IN;

        g_sUSBHCD.USBINPipes[ulIndex].ucEPNumber =
            (unsigned char)ulTargetEndpoint;

        //
        // Save the interval and the next tick to trigger a scheduler event.
        //
        g_sUSBHCD.USBINPipes[ulIndex].ulInterval = ulInterval;
        g_sUSBHCD.USBINPipes[ulIndex].ulNextEventTick =
            ulInterval + g_ulCurrentTick;

        //
        // Set the device speed.
        //
        ulFlags |= (g_sUSBHCD.USBINPipes[ulIndex].psDevice->bLowSpeed ?
                                      USB_EP_SPEED_LOW : USB_EP_SPEED_FULL);
    }

    //
    // Set up the appropriate flags if uDMA is used.
    //
    if(ulPipe & EP_PIPE_USE_UDMA)
    {
        ulFlags |= USB_EP_DMA_MODE_0;
    }

    //
    // Configure the endpoint according to the flags determined above.
    //
    USBHostEndpointConfig(USB0_BASE,
                          INDEX_TO_USB_EP((ulPipe & EP_PIPE_IDX_M) + 1),
                          ulMaxPayload, ulInterval, ulTargetEndpoint,
                          ulFlags);

    return(0);
}

//*****************************************************************************
//
//! This function is used to return the current status of a USB HCD pipe.
//!
//! This function will return the current status for a given USB pipe.  If
//! there is no status to report this call will simply return
//! \b USBHCD_PIPE_NO_CHANGE.
//!
//! \param ulPipe is the USB pipe for this status request.
//!
//! \return This function returns the current status for the given endpoint.
//! This will be one of the \b USBHCD_PIPE_* values.
//
//*****************************************************************************
unsigned long
USBHCDPipeStatus(unsigned long ulPipe)
{
    return(0);
}

//*****************************************************************************
//
//! This function is used to write data to a USB HCD pipe.
//!
//! \param ulPipe is the USB pipe to put data into.
//! \param pucData is a pointer to the data to send.
//! \param ulSize is the amount of data to send.
//!
//! This function will block until it has sent as much data as was
//! requested using the USB pipe's FIFO.  The caller should have registered a
//! callback with the USBHCDPipeAlloc() call in order to be informed when the
//! data has been transmitted.  The value returned by this function can be less
//! than the \e ulSize requested if the USB pipe has less space available than
//! this request is making.
//!
//! \return This function returns the number of bytes that were scheduled to
//! be sent on the given USB pipe.
//
//*****************************************************************************
unsigned long
USBHCDPipeWrite(unsigned long ulPipe, unsigned char *pucData,
                unsigned long ulSize)
{
    unsigned long ulEndpoint;
    unsigned long ulRemainingBytes;
    unsigned long ulByteToSend;
    unsigned long ulPipeIdx;
    unsigned long ulEPStatus;

    //
    // Determine which endpoint interface that this pipe is using.
    //
    ulEndpoint = INDEX_TO_USB_EP((EP_PIPE_IDX_M & ulPipe) + 1);

    //
    // Get index used for looking up pipe data
    //
    ulPipeIdx = ulPipe & EP_PIPE_IDX_M;

    //
    // Set the total number of bytes to send out.
    //
    ulRemainingBytes = ulSize;

    if(ulSize > 64)
    {
        //
        // Only send 64 bytes at a time.
        //
        ulByteToSend = 64;
    }
    else
    {
        //
        // Send the requested number of bytes.
        //
        ulByteToSend = ulSize;
    }

    //
    // Send all of the requested data.
    //
    while(ulRemainingBytes != 0)
    {
        //
        // Start a write request.
        //
        g_sUSBHCD.USBOUTPipes[ulPipeIdx].eState = PIPE_WRITING;

        //
        // If uDMA is not enabled for this pipe, or if the uDMA workaround
        // is applied, then don't use uDMA for this transfer.
        //
        if(!(ulPipe & EP_PIPE_USE_UDMA) ||
           (g_bUseDMAWA && (ulByteToSend != 64)))
        {
            //
            // Disable uDMA on the USB endpoint
            //
            MAP_USBEndpointDMADisable(USB0_BASE, ulEndpoint, USB_EP_HOST_OUT);

            //
            // Put the data in the buffer.
            //
            MAP_USBEndpointDataPut(USB0_BASE, ulEndpoint, pucData,
                                   ulByteToSend);

            //
            // Schedule the data to be sent.
            //
            MAP_USBEndpointDataSend(USB0_BASE, ulEndpoint, USB_TRANS_OUT);
        }

        //
        // Otherwise, uDMA should be used for this transfer
        //
        else
        {
            //
            // Set up the uDMA transfer.
            //
            MAP_uDMAChannelTransferSet(UDMA_CHANNEL_USBEP1TX + (ulPipeIdx * 2),
                                       UDMA_MODE_AUTO, pucData,
                                       (void *)USBFIFOAddrGet(USB0_BASE,
                                                              ulEndpoint),
                                       ulByteToSend);

            //
            // Enable uDMA on the USB endpoint
            //
            MAP_USBEndpointDMAEnable(USB0_BASE, ulEndpoint, USB_EP_HOST_OUT);

            //
            // Disable the USB interrupt.
            //
            OS_INT_DISABLE(INT_USB0);

            //
            // Set pending transmit DMA flag
            //
            g_ulDMAPending |= DMA_PEND_TRANSMIT_FLAG << ulPipeIdx;

            //
            // Enable the uDMA channel to start the transfer
            //
            MAP_uDMAChannelEnable(UDMA_CHANNEL_USBEP1TX + (ulPipeIdx * 2));

            //
            // Enable the USB interrupt.
            //
            OS_INT_ENABLE(INT_USB0);
        }

        //
        // Wait for a status change.
        //
        while(g_sUSBHCD.USBOUTPipes[ulPipeIdx].eState == PIPE_WRITING)
        {
            //
            // Read the status of the endpoint connected to this pipe.
            //
            ulEPStatus = MAP_USBEndpointStatus(USB0_BASE,
                                               INDEX_TO_USB_EP(ulPipeIdx + 1));

            //
            // Check if the device stalled the request.
            //
            if(ulEPStatus & USB_HOST_OUT_STALL)
            {
                //
                // If uDMA is being used, then disable the channel.
                //
                if(ulPipe & EP_PIPE_USE_UDMA)
                {
                    MAP_uDMAChannelDisable(UDMA_CHANNEL_USBEP1TX +
                                           (ulPipeIdx * 2));
                }
            }

            //
            // If a disconnect event occurs the exit out of the loop.
            //
            if(g_ulUSBHIntEvents & INT_EVENT_DISCONNECT)
            {
                //
                // Set the pipe state to error.
                //
                g_sUSBHCD.USBOUTPipes[ulPipeIdx].eState = PIPE_ERROR;
            }
        }

        //
        // If the data was successfully sent then decrement the count and
        // continue.
        //
        if(g_sUSBHCD.USBOUTPipes[ulPipeIdx].eState == PIPE_DATA_SENT)
        {
            //
            // Decrement the remaining data and advance the pointer.
            //
            ulRemainingBytes -= ulByteToSend;
            pucData += ulByteToSend;
        }
        else if(g_sUSBHCD.USBOUTPipes[ulPipeIdx].eState == PIPE_STALLED)
        {
            //
            // Zero out the size so that the caller knows that no data was
            // written.
            //
            ulSize = 0;

            //
            // If uDMA is being used, then disable the channel.
            //
            if(ulPipe & EP_PIPE_USE_UDMA)
            {
                //
                // Disable the DMA channel.
                //
                MAP_uDMAChannelDisable(UDMA_CHANNEL_USBEP1TX + (ulPipeIdx * 2));
            }

            //
            // This is the actual endpoint number.
            //
            USBHCDClearFeature(
                g_sUSBHCD.USBOUTPipes[ulPipeIdx].psDevice->ulAddress,
                ulPipe, USB_FEATURE_EP_HALT);

            //
            // If there was a stall, then no more data is coming so break out.
            //
            break;
        }

        //
        // If there are less than 64 bytes to send then this is the last
        // of the data to go out.
        //
        if(ulRemainingBytes < 64)
        {
            ulByteToSend = ulRemainingBytes;
        }
        else if(g_sUSBHCD.USBOUTPipes[ulPipeIdx].eState == PIPE_ERROR)
        {
            //
            // An error occurred so stop this transaction and set the number
            // of bytes to zero.
            //
            ulSize = 0;
            break;
        }
    }

    //
    // Go Idle once this state has been reached.
    //
    g_sUSBHCD.USBOUTPipes[ulPipeIdx].eState = PIPE_IDLE;

    return(ulSize);
}

//*****************************************************************************
//
//! This function is used to schedule and IN transaction on a USB HCD pipe.
//!
//! \param ulPipe is the USB pipe to read data from.
//! \param pucData is a pointer to store the data that is received.
//! \param ulSize is the size in bytes of the buffer pointed to by pucData.
//!
//! This function will not block depending on the type of pipe passed in will
//! schedule either a send of data to the device or a read of data from the
//! device.  In either case the amount of data will be limited to what will
//! fit in the FIFO for a given endpoint.
//!
//! \return This function returns the number of bytes that were sent in the case
//! of a transfer of data or it will return 0 for a request on a USB IN pipe.
//
//*****************************************************************************
unsigned long
USBHCDPipeSchedule(unsigned long ulPipe, unsigned char *pucData,
                   unsigned long ulSize)
{
    unsigned long ulEndpoint;
    unsigned long ulPipeIdx;

    //
    // Get index used for looking up pipe data
    //
    ulPipeIdx = ulPipe & EP_PIPE_IDX_M;

    //
    // Determine which endpoint interface that this pipe is using.
    //
    ulEndpoint = INDEX_TO_USB_EP((EP_PIPE_IDX_M & ulPipe) + 1);

    if(ulPipe & EP_PIPE_TYPE_OUT)
    {
        //
        // Start a write request.
        //
        g_sUSBHCD.USBOUTPipes[EP_PIPE_IDX_M & ulPipe].eState = PIPE_WRITING;

        //
        // Check if uDMA is enabled on this pipe.
        //
        if(ulPipe & EP_PIPE_USE_UDMA)
        {
            //
            // Set up the uDMA transfer.
            //
            MAP_uDMAChannelTransferSet(UDMA_CHANNEL_USBEP1TX + (ulPipeIdx * 2),
                                       UDMA_MODE_AUTO, pucData,
                                       (void *)USBFIFOAddrGet(USB0_BASE,
                                                              ulEndpoint),
                                       ulSize);

            //
            // Enable uDMA on the USB endpoint
            //
            MAP_USBEndpointDMAEnable(USB0_BASE, ulEndpoint, USB_EP_HOST_OUT);

            //
            // Disable the USB interrupt.
            //
            OS_INT_DISABLE(INT_USB0);

            //
            // Set pending transmit DMA flag
            //
            g_ulDMAPending |= DMA_PEND_TRANSMIT_FLAG << ulPipeIdx;

            //
            // Enable the uDMA channel to start the transfer
            //
            MAP_uDMAChannelEnable(UDMA_CHANNEL_USBEP1TX + (ulPipeIdx * 2));

            //
            // Enable the USB interrupt.
            //
            OS_INT_ENABLE(INT_USB0);
        }
        else
        {
            //
            // Put the data in the buffer.
            //
            MAP_USBEndpointDataPut(USB0_BASE, ulEndpoint, pucData, ulSize);

            //
            // Schedule the data to be sent.
            //
            MAP_USBEndpointDataSend(USB0_BASE, ulEndpoint, USB_TRANS_OUT);
        }
    }
    else
    {
        //
        // Start a read request.
        //
        g_sUSBHCD.USBINPipes[EP_PIPE_IDX_M & ulPipe].eState = PIPE_READING;

        //
        // If uDMA is not enabled for this pipe, or if the uDMA workaround
        // is applied, then do not use uDMA for this transfer.
        //
        if((ulPipe & EP_PIPE_USE_UDMA) == 0)
        {
            //
            // Disable uDMA on the endpoint
            //
            MAP_USBEndpointDMADisable(USB0_BASE, ulEndpoint, USB_EP_HOST_IN);
        }
        //
        // Otherwise, uDMA should be used for this transfer, so set up
        // the uDMA channel in advance of triggering the IN request.
        //
        else
        {
            //
            // Compute bytes to transfer and set up transfer
            //
            MAP_uDMAChannelTransferSet(UDMA_CHANNEL_USBEP1RX + (ulPipeIdx * 2),
                                       UDMA_MODE_AUTO,
                                       (void *)USBFIFOAddrGet(USB0_BASE,
                                                              ulEndpoint),
                                       pucData, ulSize);

            //
            // Enable uDMA on the endpoint
            //
            MAP_USBEndpointDMAEnable(USB0_BASE, ulEndpoint, USB_EP_HOST_IN);

            //
            // Disable the USB interrupt.
            //
            OS_INT_DISABLE(INT_USB0);

            //
            // Set pending DMA flag
            //
            g_ulDMAPending |= DMA_PEND_RECEIVE_FLAG << ulPipeIdx;

            //
            // Enable the uDMA channel to start the transfer
            //
            MAP_uDMAChannelEnable(UDMA_CHANNEL_USBEP1RX + (ulPipeIdx * 2));

            //
            // Enable the USB interrupt.
            //
            OS_INT_ENABLE(INT_USB0);
        }

        //
        // Remember details of the buffer into which the data will be read.
        //
        g_sUSBHCD.USBINPipes[ulPipeIdx].pucReadPtr = pucData;
        g_sUSBHCD.USBINPipes[ulPipeIdx].ulReadSize = ulSize;

        //
        // Trigger a request for data from the device.
        //
        MAP_USBHostRequestIN(USB0_BASE, ulEndpoint);

        //
        // No data was put into or read from the buffer.
        //
        ulSize = 0;
    }
    return(ulSize);
}

//*****************************************************************************
//
//! This function is used to read data from a USB HCD pipe.
//!
//! \param ulPipe is the USB pipe to read data from.
//! \param pucData is a pointer to store the data that is received.
//! \param ulSize is the size in bytes of the buffer pointed to by pucData.
//!
//! This function will not block and will only read as much data as requested
//! or as much data is currently available from the USB pipe.  The caller
//! should have registered a callback with the USBHCDPipeAlloc() call in order
//! to be informed when the data has been received.  The value returned by this
//! function can be less than the \e ulSize requested if the USB pipe has less
//! data available than was requested.
//!
//! \return This function returns the number of bytes that were returned in the
//! \e pucData buffer.
//
//*****************************************************************************
unsigned long
USBHCDPipeReadNonBlocking(unsigned long ulPipe, unsigned char *pucData,
                          unsigned long ulSize)
{
    unsigned long ulEndpoint;

    //
    // Determine which endpoint interface that this pipe is using.
    //
    ulEndpoint = INDEX_TO_USB_EP((EP_PIPE_IDX_M & ulPipe) + 1);

    //
    // Read the data out of the USB endpoint interface.
    //
    MAP_USBEndpointDataGet(USB0_BASE, ulEndpoint, pucData, &ulSize);

    //
    // Acknowledge that the data was read from the endpoint.
    //
    MAP_USBHostEndpointDataAck(USB0_BASE, ulEndpoint);

    //
    // Go Idle once this state has been reached.
    //
    g_sUSBHCD.USBINPipes[EP_PIPE_IDX_M & ulPipe].eState = PIPE_IDLE;

    return(ulSize);
}

//*****************************************************************************
//
//! This function acknowledges data received via an interrupt IN pipe.
//!
//! \param ulPipe is the USB INT pipe whose last packet is to be acknowledged.
//!
//! This function is used to acknowledge reception of data on an interrupt IN
//! pipe.  A transfer on an interrupt IN endpoint is scheduled via a call to
//! USBHCDPipeSchedule() and the application is notified when data is received
//! using a USB_EVENT_RX_AVAILABLE event.  In the handler for this event, the
//! application must call USBHCDPipeDataAck() to have the USB controller ACK
//! the data from the device and complete the transaction.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDPipeDataAck(unsigned long ulPipe)
{
    unsigned long ulEndpoint;

    //
    // Determine which endpoint interface that this pipe is using.
    //
    ulEndpoint = INDEX_TO_USB_EP((EP_PIPE_IDX_M & ulPipe) + 1);

    //
    // Acknowledge that the data was read from the endpoint.
    //
    USBHostEndpointDataAck(USB0_BASE, ulEndpoint);

    //
    // Go Idle once this state has been reached.
    //
    g_sUSBHCD.USBINPipes[EP_PIPE_IDX_M & ulPipe].eState = PIPE_IDLE;
}

//*****************************************************************************
//
//! This function is used to read data from a USB HCD pipe.
//!
//! \param ulPipe is the USB pipe to read data from.
//! \param pucData is a pointer to store the data that is received.
//! \param ulSize is the size in bytes of the buffer pointed to by pucData.
//!
//! This function will block and will only return when it has read as much data
//! as requested from the USB pipe.  The caller should have registered a
//! callback with the USBHCDPipeAlloc() call in order to be informed when the
//! data has been received.  The value returned by this function can be less
//! than the \e ulSize requested if the USB pipe has less data available than
//! was requested.
//!
//! \return This function returns the number of bytes that were returned in the
//! \e pucData buffer.
//
//*****************************************************************************
unsigned long
USBHCDPipeRead(unsigned long ulPipe, unsigned char *pucData,
               unsigned long ulSize)
{
    unsigned long ulEndpoint;
    unsigned long ulRemainingBytes;
    unsigned long ulBytesRead;
    unsigned long ulPipeIdx;
    unsigned long ulEPStatus;

    //
    // Get index used for looking up pipe data
    //
    ulPipeIdx = ulPipe & EP_PIPE_IDX_M;

    //
    // Initialized the number of bytes read.
    //
    ulBytesRead = 0;

    //
    // Determine which endpoint interface that this pipe is using.
    //
    ulEndpoint = INDEX_TO_USB_EP(ulPipeIdx + 1);

    //
    // Set the remaining bytes to received.
    //
    ulRemainingBytes = ulSize;

    //
    // Continue until all data requested has been received.
    //
    while(ulRemainingBytes != 0)
    {
        //
        // Start a read request.
        //
        g_sUSBHCD.USBINPipes[ulPipeIdx].eState = PIPE_READING;

        //
        // If uDMA is not enabled for this pipe, or if the uDMA workaround
        // is applied, then do not use uDMA for this transfer.
        //
        if(!(ulPipe & EP_PIPE_USE_UDMA) ||
            (g_bUseDMAWA && (ulRemainingBytes < 64)))
        {
            //
            // Disable uDMA on the endpoint
            //
            MAP_USBEndpointDMADisable(USB0_BASE, ulEndpoint, USB_EP_HOST_IN);
        }

        //
        // Otherwise, uDMA should be used for this transfer, so set up
        // the uDMA channel in advance of triggering the IN request.
        //
        else
        {
            //
            // Compute bytes to transfer and set up transfer
            //
            ulBytesRead = ulRemainingBytes > 64 ? 64 : ulRemainingBytes;

            MAP_uDMAChannelTransferSet(UDMA_CHANNEL_USBEP1RX + (ulPipeIdx * 2),
                                       UDMA_MODE_AUTO,
                                       (void *)USBFIFOAddrGet(USB0_BASE,
                                                              ulEndpoint),
                                       pucData, ulBytesRead);

            //
            // Enable uDMA on the endpoint
            //
            MAP_USBEndpointDMAEnable(USB0_BASE, ulEndpoint, USB_EP_HOST_IN);

            //
            // Disable the USB interrupt.
            //
            OS_INT_DISABLE(INT_USB0);

            //
            // Set pending DMA flag
            //
            g_ulDMAPending |= DMA_PEND_RECEIVE_FLAG << ulPipeIdx;

            //
            // Enable the uDMA channel to start the transfer
            //
            MAP_uDMAChannelEnable(UDMA_CHANNEL_USBEP1RX + (ulPipeIdx * 2));

            //
            // Enable the USB interrupt.
            //
            OS_INT_ENABLE(INT_USB0);
        }

        //
        // Set up for the next transaction.
        //
        g_sUSBHCD.USBINPipes[ulPipeIdx].pucReadPtr = pucData;
        g_sUSBHCD.USBINPipes[ulPipeIdx].ulReadSize = (ulRemainingBytes < 64) ?
                                                     ulRemainingBytes : 64;

        //
        // Trigger a request for data from the device.
        //
        MAP_USBHostRequestIN(USB0_BASE, ulEndpoint);

        //
        // Wait for a status change.
        //
        while(g_sUSBHCD.USBINPipes[ulPipeIdx].eState == PIPE_READING)
        {
            //
            // Read the status of the endpoint connected to this pipe.
            //
            ulEPStatus = MAP_USBEndpointStatus(USB0_BASE,
                                               INDEX_TO_USB_EP(ulPipeIdx + 1));

            //
            // Check if the device stalled the request.
            //
            if(ulEPStatus & USB_HOST_IN_STALL)
            {
                //
                // If uDMA is being used, then disable the channel.
                //
                if(ulPipe & EP_PIPE_USE_UDMA)
                {
                    //
                    // Disable the DMA channel.
                    //
                    MAP_uDMAChannelDisable(UDMA_CHANNEL_USBEP1RX +
                                           (ulPipeIdx * 2));
                }
            }

            //
            // If a disconnect event occurs the exit out of the loop.
            //
            if(g_ulUSBHIntEvents & INT_EVENT_DISCONNECT)
            {
                //
                // Set the pipe state to error.
                //
                g_sUSBHCD.USBINPipes[ulPipeIdx].eState = PIPE_ERROR;
            }
        }

        //
        // If data is ready then return it.
        //
        if(g_sUSBHCD.USBINPipes[ulPipeIdx].eState == PIPE_DATA_READY)
        {
            //
            // If not using uDMA then read the data from the USB.  Otherwise
            // the data will already be in the buffer.
            //
            if(!(ulPipe & EP_PIPE_USE_UDMA) ||
                (g_bUseDMAWA && (ulRemainingBytes < 64)))
            {
                //
                // Compute bytes to transfer and set up transfer
                //
                ulBytesRead = ulRemainingBytes > 64 ? 64 : ulRemainingBytes;

                //
                // Acknowledge that the data was read from the endpoint.
                //
                MAP_USBHostEndpointDataAck(USB0_BASE, ulEndpoint);
            }

            //
            // Subtract the number of bytes read from the bytes remaining.
            //
            ulRemainingBytes -= ulBytesRead;

            //
            // If there were less than 64 bytes read, then this was a short
            // packet and no more data will be returned.
            //
            if(ulBytesRead < 64)
            {
                //
                // Subtract off the bytes that were not received and exit the
                // loop.
                //
                ulSize = ulSize - ulRemainingBytes;
                break;
            }
            else
            {
                //
                // Move the buffer ahead to receive more data into the buffer.
                //
                pucData += 64;
            }
        }
        else if(g_sUSBHCD.USBINPipes[ulPipeIdx].eState == PIPE_STALLED)
        {
            //
            // Zero out the size so that the caller knows that no data was read.
            //
            ulSize = 0;

            //
            // If uDMA is being used, then disable the channel.
            //
            if(ulPipe & EP_PIPE_USE_UDMA)
            {
                MAP_uDMAChannelDisable(UDMA_CHANNEL_USBEP1RX + (ulPipeIdx * 2));
            }

            //
            // This is the actual endpoint number.
            //
            USBHCDClearFeature(
                g_sUSBHCD.USBINPipes[ulPipeIdx].psDevice->ulAddress,
                ulPipe, USB_FEATURE_EP_HALT);

            //
            // If there was a stall, then no more data is coming so break out.
            //
            break;
        }
        else if(g_sUSBHCD.USBINPipes[ulPipeIdx].eState == PIPE_ERROR)
        {
            //
            // An error occurred so stop this transaction and set the number
            // of bytes to zero.
            //
            ulSize = 0;
            break;
        }
    }

    //
    // Go Idle once this state has been reached.
    //
    g_sUSBHCD.USBINPipes[ulPipeIdx].eState = PIPE_IDLE;

    return(ulSize);
}

//*****************************************************************************
//
//! This function is used to release a USB pipe.
//!
//! \param ulPipe is the allocated USB pipe to release.
//!
//! This function is used to release a USB pipe that was allocated by a call to
//! USBHCDPipeAlloc() for use by some other device endpoint in the system.
//! Freeing an unallocated or invalid pipe will not generate an error and will
//! instead simply return.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDPipeFree(unsigned long ulPipe)
{
    unsigned long ulDMAIdx;
    unsigned long ulIndex;

    //
    // Get the index number from the allocated pipe.
    //
    ulIndex = (ulPipe & EP_PIPE_IDX_M);

    if(ulPipe & EP_PIPE_TYPE_OUT)
    {
        //
        // Clear the address and type for this endpoint to free it up.
        //
        g_sUSBHCD.USBOUTPipes[ulPipe & EP_PIPE_IDX_M].psDevice = 0;
        g_sUSBHCD.USBOUTPipes[ulPipe & EP_PIPE_IDX_M].ulType = 0;
        g_sUSBHCD.USBOUTPipes[ulPipe & EP_PIPE_IDX_M].pfnCallback = 0;

        //
        // Check if this pipe has allocated a DMA channel.
        //
        if(g_sUSBHCD.USBOUTPipes[ulPipe & EP_PIPE_IDX_M].ucDMAChannel !=
           USBHCD_DMA_UNUSED)
        {
            //
            // Get the DMA channel used by this pipe.
            //
            ulDMAIdx =
                g_sUSBHCD.USBOUTPipes[ulPipe & EP_PIPE_IDX_M].ucDMAChannel;

            //
            // This is a debug check that will prevent accessing beyond the
            // buffer allocated to the DMA channels.
            //
            ASSERT(ulDMAIdx < MAX_NUM_DMA_CHANNELS);

            //
            // Mark the channel as free for use.
            //
            g_sUSBHCD.ucDMAChannels[ulDMAIdx] = USBHCD_DMA_UNUSED;

            //
            // Clear out the current channel in use by this pipe.
            //
            g_sUSBHCD.USBOUTPipes[ulPipe & EP_PIPE_IDX_M].ucDMAChannel =
                USBHCD_DMA_UNUSED;
        }

        //
        // Free up the FIFO memory used by this endpoint.
        //
        if(g_sUSBHCD.USBOUTPipes[ulPipe & EP_PIPE_IDX_M].ucFIFOSize)
        {
            FIFOFree(&g_sUSBHCD.USBOUTPipes[ulPipe & EP_PIPE_IDX_M]);
        }

        //
        // Set the function address for this endpoint back to zero.
        //
        USBHostAddrSet(USB0_BASE, INDEX_TO_USB_EP(ulIndex + 1),
                       0, USB_EP_HOST_OUT);

        //
        // Set the hub and port address for the endpoint back to zero and the
        // speed back to LOW.
        //
        USBHostHubAddrSet(USB0_BASE, INDEX_TO_USB_EP(ulIndex + 1),
                          0, (USB_EP_HOST_OUT | USB_EP_SPEED_LOW));
    }
    else if(ulPipe & EP_PIPE_TYPE_IN)
    {
        //
        // Clear the address and type for this endpoint to free it up.
        //
        g_sUSBHCD.USBINPipes[ulPipe & EP_PIPE_IDX_M].psDevice = 0;
        g_sUSBHCD.USBINPipes[ulPipe & EP_PIPE_IDX_M].ulType = 0;
        g_sUSBHCD.USBINPipes[ulPipe & EP_PIPE_IDX_M].pfnCallback = 0;

        //
        // Check if this pipe has allocated a DMA channel.
        //
        if(g_sUSBHCD.USBINPipes[ulPipe & EP_PIPE_IDX_M].ucDMAChannel !=
           USBHCD_DMA_UNUSED)
        {
            //
            // Get the DMA channel used by this pipe.
            //
            ulDMAIdx = g_sUSBHCD.USBINPipes[ulPipe & EP_PIPE_IDX_M].ucDMAChannel;

            //
            // This is a debug check that will prevent accessing beyond the
            // buffer allocated to the DMA channels.
            //
            ASSERT(ulDMAIdx < MAX_NUM_DMA_CHANNELS);

            //
            // Mark the channel as free for use.
            //
            g_sUSBHCD.ucDMAChannels[ulDMAIdx] = USBHCD_DMA_UNUSED;

            //
            // Clear out the current channel in use by this pipe.
            //
            g_sUSBHCD.USBINPipes[ulPipe & EP_PIPE_IDX_M].ucDMAChannel =
                USBHCD_DMA_UNUSED;
        }

        //
        // Free up the FIFO memory used by this endpoint.
        //
        if(g_sUSBHCD.USBINPipes[ulPipe & EP_PIPE_IDX_M].ucFIFOSize)
        {
            FIFOFree(&g_sUSBHCD.USBINPipes[ulPipe & EP_PIPE_IDX_M]);
        }

        //
        // Set the function address for this endpoint back to zero.
        //
        USBHostAddrSet(USB0_BASE, INDEX_TO_USB_EP(ulIndex + 1),
                       0, USB_EP_HOST_IN);

        //
        // Set the hub and port address for the endpoint back to zero and the
        // speed back to LOW.
        //
        USBHostHubAddrSet(USB0_BASE, INDEX_TO_USB_EP(ulIndex + 1),
                          0, (USB_EP_HOST_IN | USB_EP_SPEED_LOW));

        //
        // Clear any pending IN transactions.
        //
        USBHostRequestINClear(USB0_BASE, INDEX_TO_USB_EP(ulIndex + 1));
    }
}

//*****************************************************************************
//
// This internal function initializes the HCD code.
//
// \param ulIndex specifies which USB controller to use.
// \param pvPool is a pointer to the data to use as a memory pool for this
// controller.
// \param ulPoolSize is the size in bytes of the buffer passed in as pvPool.
//
// This function will perform all the necessary operations to allow the USB
// host controller to begin enumeration and communication with a device.  This
// function should typically be called once at the start of an application
// before any other calls are made to the host controller.
//
// \return None.
//
//*****************************************************************************
static void
USBHCDInitInternal(unsigned long ulIndex, void *pvPool,
                   unsigned long ulPoolSize)
{
    long lIdx;

    ASSERT(ulIndex == 0);

    //
    // Get the number of endpoints supported by this device.
    //
    g_sUSBHCD.ulNumEndpoints = USBNumEndpointsGet(USB0_BASE);

    //
    // The first 64 Bytes are allocated to endpoint 0.
    //
    g_ulAlloc[0] = 1;
    g_ulAlloc[1] = 0;

    //
    // Save the base address for this controller.
    //
    g_sUSBHCD.ulUSBBase = USB0_BASE;

    //
    // All Pipes are unused at start.
    //
    for(lIdx = 0; lIdx < MAX_NUM_PIPES; lIdx++)
    {
        g_sUSBHCD.USBINPipes[lIdx].psDevice = 0;
        g_sUSBHCD.USBINPipes[lIdx].ulType = USBHCD_PIPE_UNUSED;
        g_sUSBHCD.USBINPipes[lIdx].ucDMAChannel = USBHCD_DMA_UNUSED;
        g_sUSBHCD.USBOUTPipes[lIdx].psDevice = 0;
        g_sUSBHCD.USBOUTPipes[lIdx].ulType = USBHCD_PIPE_UNUSED;
        g_sUSBHCD.USBOUTPipes[lIdx].ucDMAChannel = USBHCD_DMA_UNUSED;
    }

    //
    // Make sure that the hub driver is initialized since it is called even
    // if it is not present in the system.
    //
    USBHHubInit();

    //
    // All DMA channels are unused at start.
    //
    for(lIdx = 0; lIdx < MAX_NUM_DMA_CHANNELS; lIdx++)
    {
        g_sUSBHCD.ucDMAChannels[lIdx] = USBHCD_DMA_UNUSED;
    }

    //
    // Initialized the device structures.
    //
    for(lIdx = 0; lIdx <= MAX_USB_DEVICES; lIdx++)
    {
        //
        // Clear the config descriptor and state.
        //
        g_sUSBHCD.eDeviceState[lIdx] = HCD_IDLE;
        g_sUSBHCD.USBDevice[lIdx].pConfigDescriptor = 0;
        g_sUSBHCD.USBDevice[lIdx].bConfigRead = false;

        //
        // Initialize the device descriptor.
        //
        g_sUSBHCD.USBDevice[lIdx].DeviceDescriptor.bLength = 0;
        g_sUSBHCD.USBDevice[lIdx].DeviceDescriptor.bMaxPacketSize0 = 0;

        //
        // Initialize the device address.
        //
        g_sUSBHCD.USBDevice[lIdx].ulAddress = 0;

        //
        // Set the current interface to 0.
        //
        g_sUSBHCD.USBDevice[lIdx].ulInterface = 0;

        //
        // Clear the active driver for the device.
        //
        g_lUSBHActiveDriver[lIdx] = -1;
    }

    //
    // Allocate the memory needed for reading descriptors.
    //
    g_sUSBHCD.pvPool = pvPool;
    g_sUSBHCD.ulPoolSize = ulPoolSize;

    //
    // Initialize the device class.
    //
    g_sUSBHCD.ulClass = USB_CLASS_EVENTS;

    //
    // Default enable connect, disconnect, unknown device and power fault
    // event notifications.
    //
    g_sUSBHCD.ulEventEnables = USBHCD_EVFLAG_CONNECT | USBHCD_EVFLAG_UNKCNCT |
                               USBHCD_EVFLAG_DISCNCT | USBHCD_EVFLAG_PWRFAULT |
                               USBHCD_EVFLAG_PWREN   | USBHCD_EVFLAG_PWRDIS;

    //
    // Initialize the USB tick module.
    //
    InternalUSBTickInit();

    //
    // Only do hardware update if the stack is in Host mode, do not touch the
    // hardware for OTG mode operation.
    //
    if((g_eUSBMode == USB_MODE_HOST) || (g_eUSBMode == USB_MODE_FORCE_HOST))
    {
        //
        // Configure the End point 0.
        //
        USBHostEndpointConfig(USB0_BASE, USB_EP_0, 64, 0, 0,
                              (USB_EP_MODE_CTRL | USB_EP_SPEED_FULL |
                               USB_EP_HOST_OUT));

        //
        // Enable USB Interrupts.
        //
        MAP_USBIntEnableControl(USB0_BASE, USB_INTCTRL_RESET |
                                           USB_INTCTRL_DISCONNECT |
                                           USB_INTCTRL_SOF |
                                           USB_INTCTRL_SESSION |
                                           USB_INTCTRL_BABBLE |
                                           USB_INTCTRL_CONNECT |
                                           USB_INTCTRL_RESUME |
                                           USB_INTCTRL_SUSPEND |
                                           USB_INTCTRL_VBUS_ERR |
                                           USB_INTCTRL_MODE_DETECT |
                                           USB_INTCTRL_POWER_FAULT);

        MAP_USBIntEnableEndpoint(USB0_BASE, USB_INTEP_ALL);

        //
        // Enable the USB interrupt.
        //
        OS_INT_ENABLE(INT_USB0);

        //
        // There is no automatic power in pure host mode.
        //
        USBHCDPowerConfigSet(ulIndex, g_ulPowerConfig & ~USB_HOST_PWREN_AUTO);

        //
        // Force the power on as well as this point.
        //
        MAP_USBHostPwrEnable(USB0_BASE);

        //
        // This is required to get into host mode on some parts.
        //
        USBOTGSessionRequest(USB0_BASE, true);
    }
}

//*****************************************************************************
//
//! This function is used to set the power pin and power fault configuration.
//!
//! \param ulIndex specifies which USB controller to use.
//! \param ulPwrConfig is the power configuration to use for the application.
//!
//! This function must be called before HCDInit() is called so that the power
//! pin configuration can be set before power is enabled.  The \e ulPwrConfig
//! flags specify the power fault level sensitivity, the power fault action,
//! and the power enable pin level and source.
//!
//! One of the following can be selected as the power fault level sensitivity:
//!
//! - \b USBHCD_FAULT_LOW - An external power fault is indicated by the pin
//!                         being driven low.
//! - \b USBHCD_FAULT_HIGH - An external power fault is indicated by the pin
//!                          being driven high.
//!
//! One of the following can be selected as the power fault action:
//!
//! - \b USBHCD_FAULT_VBUS_NONE - No automatic action when power fault
//!   detected.
//! - \b USBHCD_FAULT_VBUS_TRI - Automatically Tri-state the USBnEPEN pin on a
//!                              power fault.
//! - \b USBHCD_FAULT_VBUS_DIS - Automatically drive the USBnEPEN pin to it's
//!                              inactive state on a power fault.
//!
//! One of the following can be selected as the power enable level and source:
//!
//! - \b USBHCD_VBUS_MANUAL - Power control is completely managed by the
//!                           application, the USB library will provide a
//!                           power callback to request power state changes.
//! - \b USBHCD_VBUS_AUTO_LOW - USBEPEN is driven low by the USB controller
//!                             automatically if USBOTGSessionRequest() has
//!                             enabled a session.
//! - \b USBHCD_VBUS_AUTO_HIGH - USBEPEN is driven high by the USB controller
//!                              automatically if USBOTGSessionRequest() has
//!                              enabled a session.
//!
//! If USBHCD_VBUS_MANUAL is used then the application must provide an
//! event driver to receive the USB_EVENT_POWER_ENABLE and
//! USB_EVENT_POWER_DISABLE events and enable and disable power to VBUS when
//! requested by the USB library.  The application should respond to a power
//! control callback by enabling or disabling VBUS as soon as possible and
//! before returning from the callback function.
//!
//! \note The following values should no longer be used with the USB library:
//! USB_HOST_PWRFLT_LOW, USB_HOST_PWRFLT_HIGH, USB_HOST_PWRFLT_EP_NONE,
//! USB_HOST_PWRFLT_EP_TRI, USB_HOST_PWRFLT_EP_LOW, USB_HOST_PWRFLT_EP_HIGH,
//! USB_HOST_PWREN_LOW, USB_HOST_PWREN_HIGH, USB_HOST_PWREN_VBLOW, and
//! USB_HOST_PWREN_VBHIGH.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDPowerConfigInit(unsigned long ulIndex, unsigned long ulPwrConfig)
{
    ASSERT(ulIndex == 0);

    //
    // Save the value as it will be used later.
    //
    g_ulPowerConfig = ulPwrConfig;
}

//*****************************************************************************
//
//! This function is used to get the power pin and power fault configuration.
//!
//! \param ulIndex specifies which USB controller to use.
//!
//! This function will return the current power control pin configuration as
//! set by the USBHCDPowerConfigInit() function or the defaults if not yet set.
//! See the USBHCDPowerConfigInit() documentation for the meaning of the bits
//! that are returned by this function.
//!
//! \return The configuration of the power control pins.
//!
//*****************************************************************************
unsigned long
USBHCDPowerConfigGet(unsigned long ulIndex)
{
    ASSERT(ulIndex == 0);

    //
    // Save the value as it will be used later.
    //
    return(g_ulPowerConfig);
}

//*****************************************************************************
//
//! This function is used to set the power pin and power fault configuration.
//!
//! \param ulIndex specifies which USB controller to use.
//! \param ulConfig specifies which USB power configuration to use.
//!
//! This function will set the current power control pin configuration as
//! set by the USBHCDPowerConfigInit() function or the defaults if not yet set.
//! See the USBHCDPowerConfigInit() documentation for the meaning of the bits
//! that are set by this function.
//!
//! \return Returns zero to indicate the power setting is now active.
//!
//*****************************************************************************
unsigned long
USBHCDPowerConfigSet(unsigned long ulIndex, unsigned long ulConfig)
{
    ASSERT(ulIndex == 0);

    //
    // Remember the current setting.
    //
    g_ulPowerConfig = ulConfig;

    //
    // Clear out the two flag bits.
    //
    ulConfig = g_ulPowerConfig & ~(USBHCD_VBUS_MANUAL | USBHCD_FAULT_VBUS_DIS);

    //
    // If there is an automatic disable power action specified then set the
    // polarity of the signal to match EPEN.
    //
    if(g_ulPowerConfig & USBHCD_FAULT_VBUS_DIS)
    {
        //
        // Insure that the assumption below is true.
        //
        ASSERT((USBHCD_VBUS_AUTO_HIGH & 1) == 1);
        ASSERT((USBHCD_VBUS_AUTO_LOW & 1) == 0);

        //
        // This is taking advantage of the difference between
        // USBHCD_VBUS_AUTO_LOW and USBHCD_VBUS_AUTO_HIGH being that bit
        // one is set when EPEN is active high.
        //
        if(g_ulPowerConfig & 1)
        {
            g_ulPowerConfig |= USB_HOST_PWRFLT_EP_LOW;
            ulConfig |= USB_HOST_PWRFLT_EP_LOW;
        }
        else
        {
            g_ulPowerConfig |= USB_HOST_PWRFLT_EP_HIGH;
            ulConfig |= USB_HOST_PWRFLT_EP_HIGH;
        }
    }

    //
    // Initialize the power configuration.
    //
    MAP_USBHostPwrConfig(USB0_BASE, ulConfig);

    //
    // If not in manual mode then just turn on power.
    //
    if((g_ulPowerConfig & USBHCD_VBUS_MANUAL) == 0)
    {
        //
        // Power the USB bus.
        //
        MAP_USBHostPwrEnable(USB0_BASE);
    }

    //
    // Return success.
    //
    return(0);
}

//*****************************************************************************
//
//! This function returns if the current power settings will automatically
//! handle enabling and disabling VBUS power.
//!
//! \param ulIndex specifies which USB controller to query.
//!
//! This function returns if the current power control pin configuration will
//! automatically apply power or whether it will be left to the application
//! to turn on power when it is notified.
//!
//! \return A non-zero value indicates that power is automatically applied and
//! a value of zero indicates that the application must manually apply power.
//!
//*****************************************************************************
unsigned long
USBHCDPowerAutomatic(unsigned long ulIndex)
{
    //
    // Check if the controller is automatically applying power or not.
    //
    if(g_ulPowerConfig & USBHCD_VBUS_MANUAL)
    {
        return(0);
    }
    return(1);
}

//*****************************************************************************
//
//! This function is used to initialize the HCD code.
//!
//! \param ulIndex specifies which USB controller to use.
//! \param pvPool is a pointer to the data to use as a memory pool for this
//! controller.
//! \param ulPoolSize is the size in bytes of the buffer passed in as pvPool.
//!
//! This function will perform all the necessary operations to allow the USB
//! host controller to begin enumeration and communication with devices.  This
//! function should typically be called once at the start of an application
//! once all of the device and class drivers are ready for normal operation.
//! This call will start up the USB host controller and any connected device
//! will immediately start the enumeration sequence.
//!
//! The USBStackModeSet() function can be called with USB_MODE_HOST in order to
//! cause the USB library to force the USB operating mode to a host controller.
//! This allows the application to used the USBVBUS and USBID pins as GPIOs on
//! devices that support forcing OTG to operate as a host only controller.  By
//! default the USB library will assume that the USBVBUS and USBID pins are
//! configured as USB pins and not GPIOs.
//!
//! \note Forcing of the USB controller mode feature is not available on all
//! Stellaris microcontrollers.  Consult the data sheet for the microcontroller
//! that the application is using to determine if this feature is available.
//!
//! The memory pool passed to this function must be at least as large as a
//! typical configuration descriptor for devices that are to be supported.  This
//! value is application-dependent however it should never be less than 32
//! bytes and, in most cases, should be at least 64 bytes.  If there is not
//! sufficient memory to load a configuration descriptor from a device, the
//! device will not be recognized by the USB library's host controller driver.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDInit(unsigned long ulIndex, void *pvPool, unsigned long ulPoolSize)
{
    long lDriver;

    //
    // Check the arguments.
    //
    ASSERT(ulIndex == 0);

    //
    // Make sure there is at least enough to read the configuration descriptor.
    //
    ASSERT(ulPoolSize >= sizeof(tConfigDescriptor));

    //
    // Should not call this if the stack is in device mode.
    //
    ASSERT(g_eUSBMode != USB_MODE_DEVICE);
    ASSERT(g_eUSBMode != USB_MODE_FORCE_DEVICE);

    //
    // If the mode was not set then default to USB_MODE_HOST.
    //
    if(g_eUSBMode == USB_MODE_NONE)
    {
        g_eUSBMode = USB_MODE_HOST;
    }

    //
    // Reset the USB controller.
    //
    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_USB0);

    //
    // Enable Clocking to the USB controller.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);

    //
    // Turn on USB Phy clock.
    //
    MAP_SysCtlUSBPLLEnable();

    //
    // If the application not requesting OTG mode then set the mode to forced
    // host mode.  If the mode is actually USB_MODE_HOST, this will be switched
    // off when ID pin detection is complete and the ID is no longer in use.
    //
    if(g_eUSBMode != USB_MODE_OTG)
    {
        //
        // Force Host mode on devices that support force host mode.
        //
        MAP_USBHostMode(USB0_BASE);
    }

    //
    // Call our internal function to perform the initialization.
    //
    USBHCDInitInternal(ulIndex, pvPool, ulPoolSize);

    //
    // No event driver is present by default.
    //
    g_sUSBHCD.lEventDriver = -1;

    //
    // Search through the Host Class driver list for the devices class.
    //
    for(lDriver = 0; lDriver < g_sUSBHCD.ulNumClassDrivers; lDriver++)
    {
        if(g_sUSBHCD.pClassDrivers[lDriver]->ulInterfaceClass ==
           USB_CLASS_EVENTS)
        {
            //
            // Event driver was found so remember it.
            //
            g_sUSBHCD.lEventDriver = lDriver;
        }
    }

    //
    // Get the number of ticks per millisecond, this is only used by blocking
    // delays using the SysCtlDelay() function.
    //
    g_ulTickms = MAP_SysCtlClockGet() / 3000;

    //
    // Check to see if uDMA workaround is needed.
    //
    if(CLASS_IS_DUSTDEVIL && REVISION_IS_A0)
    {
        g_bUseDMAWA = 1;
    }
}

//*****************************************************************************
//
//! This function is used to initialize the HCD class driver list.
//!
//! \param ulIndex specifies which USB controller to use.
//! \param ppHClassDrvs is an array of host class drivers that are
//! supported on this controller.
//! \param ulNumDrivers is the number of entries in the \e pHostClassDrivers
//! array.
//!
//! This function will set the host classes supported by the host controller
//! specified by the \e ulIndex parameter.  This function should be called
//! before enabling the host controller driver with the USBHCDInit() function.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDRegisterDrivers(unsigned long ulIndex,
                      const tUSBHostClassDriver * const *ppHClassDrvs,
                      unsigned long ulNumDrivers)
{
    ASSERT(ulIndex == 0);

    //
    // Save the class drivers.
    //
    g_sUSBHCD.pClassDrivers = ppHClassDrvs;

    //
    // Save the number of class drivers.
    //
    g_sUSBHCD.ulNumClassDrivers = ulNumDrivers;
}

//*****************************************************************************
//
//! This function is used to terminate the HCD code.
//!
//! \param ulIndex specifies which USB controller to release.
//!
//! This function will clean up the USB host controller and disable it in
//! preparation for shutdown or a switch to USB device mode.  Once this call is
//! made, \e USBHCDInit() may be called to reinitialize the controller and
//! prepare for host mode operation.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDTerm(unsigned long ulIndex)
{
    ASSERT(ulIndex == 0);

    //
    // End the session.
    //
    USBOTGSessionRequest(USB0_BASE, false);

    //
    // Remove power from the USB bus.
    //
    MAP_USBHostPwrDisable(USB0_BASE);

    //
    // Disable USB interrupts.
    //
    OS_INT_DISABLE(INT_USB0);

    MAP_USBIntDisableControl(USB0_BASE, USB_INTCTRL_ALL);

    MAP_USBIntDisableEndpoint(USB0_BASE, USB_INTEP_ALL);

    //
    // Set the host controller state back to it's initial values.
    //
    g_sUSBHCD.USBINPipes[0].ulType = USBHCD_PIPE_UNUSED;
    g_sUSBHCD.USBINPipes[1].ulType = USBHCD_PIPE_UNUSED;
    g_sUSBHCD.USBINPipes[2].ulType = USBHCD_PIPE_UNUSED;
    g_sUSBHCD.USBOUTPipes[0].ulType = USBHCD_PIPE_UNUSED;
    g_sUSBHCD.USBOUTPipes[1].ulType = USBHCD_PIPE_UNUSED;
    g_sUSBHCD.USBOUTPipes[2].ulType = USBHCD_PIPE_UNUSED;
    g_sUSBHCD.eDeviceState[0] = HCD_IDLE;
    g_sUSBHCD.USBDevice[0].pConfigDescriptor = 0;
    g_sUSBHCD.USBDevice[0].bConfigRead = false;
    g_sUSBHCD.USBDevice[0].DeviceDescriptor.bLength = 0;
    g_sUSBHCD.USBDevice[0].DeviceDescriptor.bMaxPacketSize0 = 0;
    g_sUSBHCD.USBDevice[0].ulAddress = 0;
    g_sUSBHCD.USBDevice[0].ulInterface = 0;
    g_sUSBHCD.pvPool = 0;
    g_sUSBHCD.ulPoolSize = 0;
}

//*****************************************************************************
//
//! This function generates reset signaling on the USB bus.
//!
//! \param ulIndex specifies which USB controller to use.
//!
//! This function handles sending out reset signaling on the USB bus.  After
//! returning from this function, any attached device on the USB bus should
//! have returned to it's reset state.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDReset(unsigned long ulIndex)
{
    ASSERT(ulIndex == 0);

    //
    // Start the reset signaling.
    //
    MAP_USBHostReset(USB0_BASE, 1);

    //
    // Wait 20ms
    //
    OS_DELAY(g_ulTickms * 20);

    //
    // End reset signaling on the bus.
    //
    MAP_USBHostReset(USB0_BASE, 0);

    //
    // Need to wait at least 10ms to let the device recover from
    // the reset.  This is the delay specified in the USB 2.0 spec.
    // We will hold the reset for 20ms.
    //
    OS_DELAY(g_ulTickms * 20);
}

//*****************************************************************************
//
//! This function will generate suspend signaling on the USB bus.
//!
//! \param ulIndex specifies which USB controller to use.
//!
//! This function is used to generate suspend signaling on the USB bus.  In
//! order to leave the suspended state, the application should call
//! USBHCDResume().
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDSuspend(unsigned long ulIndex)
{
    ASSERT(ulIndex == 0);

    //
    // Start the suspend signaling.
    //
    MAP_USBHostSuspend(USB0_BASE);
}

//*****************************************************************************
//
//! This function will generate resume signaling on the USB bus.
//!
//! \param ulIndex specifies which USB controller to use.
//!
//! This function is used to generate resume signaling on the USB bus in order
//! to cause  USB devices to leave their suspended state.  This call should
//! not be made unless a preceding call to USBHCDSuspend() has been made.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDResume(unsigned long ulIndex)
{
    ASSERT(ulIndex == 0);

    //
    // Start the resume signaling.
    //
    MAP_USBHostResume(USB0_BASE, 1);

    //
    // Wait 100ms
    //
    OS_DELAY(g_ulTickms * 100);

    //
    // End reset signaling on the bus.
    //
    MAP_USBHostResume(USB0_BASE, 0);
}

//*****************************************************************************
//
//! This function issues a request for the current configuration descriptor
//! from a device.
//!
//! \param ulIndex specifies which USB controller to use.
//! \param pDevice is a pointer to the device structure that holds the buffer
//! to store the configuration descriptor.
//!
//! This function will request the configuration descriptor from the device.
//! The \e pDevice->ConfigDescriptor member variable is used to hold the data
//! for this request.  This buffer will be allocated from the pool provided by
//! the HCDInit() function.  \e pDevice->DeviceDescriptor.bMaxPacketSize0
//! should be valid prior to this call in order to correctly receive the
//! configuration descriptor.  If this variable is not valid then this call
//! will not return accurate data.
//!
//! \return The number of bytes returned due to the request.  This value can be
//! zero if the device did not respond.
//
//*****************************************************************************
static unsigned long
USBHCDGetConfigDescriptor(unsigned long ulIndex, tUSBHostDevice *pDevice)
{
    tUSBRequest SetupPacket;
    unsigned long ulBytes;

    ASSERT(ulIndex == 0);

    ulBytes = 0;

    //
    // This is a Standard Device IN request.
    //
    SetupPacket.bmRequestType =
        USB_RTYPE_DIR_IN | USB_RTYPE_STANDARD | USB_RTYPE_DEVICE;

    //
    // Request a Device Descriptor.
    //
    SetupPacket.bRequest = USBREQ_GET_DESCRIPTOR;
    SetupPacket.wValue = USB_DTYPE_CONFIGURATION << 8;

    //
    // Index is always 0 for device configurations requests.
    //
    SetupPacket.wIndex = 0;

    //
    // Only ask for the configuration header first to see how big the
    // whole thing is.
    //
    if(!pDevice->bConfigRead)
    {
        //
        // Only request the space available.
        //
        SetupPacket.wLength = sizeof(tConfigDescriptor);

        //
        // Put the setup packet in the buffer.
        //
        ulBytes =
            USBHCDControlTransfer(0, &SetupPacket, pDevice,
                                  (unsigned char *)pDevice->pConfigDescriptor,
                                  sizeof(tConfigDescriptor),
                                  pDevice->DeviceDescriptor.bMaxPacketSize0);
    }

    //
    // If the Configuration header was successfully returned then get the
    // full configuration descriptor.
    //
    if(ulBytes == sizeof(tConfigDescriptor))
    {
        //
        // Save the total size and request the full configuration descriptor.
        //
        SetupPacket.wLength =
            pDevice->pConfigDescriptor->wTotalLength;

        //
        // Don't allow the buffer to be larger than was allocated.
        //
        if(SetupPacket.wLength > g_sUSBHCD.ulPoolSize)
        {
            SetupPacket.wLength = g_sUSBHCD.ulPoolSize;
        }

        //
        // Put the setup packet in the buffer.
        //
        ulBytes =
            USBHCDControlTransfer(0, &SetupPacket, pDevice,
                                  (unsigned char *)pDevice->pConfigDescriptor,
                                  SetupPacket.wLength,
                                  pDevice->DeviceDescriptor.bMaxPacketSize0);

        //
        // If we read the descriptor, remember the fact.
        //
        if(ulBytes)
        {
            pDevice->bConfigRead = true;
        }
    }

    return(ulBytes);
}

//*****************************************************************************
//
//! This function issues a request for a device descriptor from a device.
//!
//! \param ulIndex specifies which USB controller to use.
//! \param pDevice is a pointer to the device structure that holds the buffer
//! to store the device descriptor into.
//!
//! This function will request the device descriptor from the device.  The
//! \e pDevice->DeviceDescriptor descriptor is used to hold the data for this
//! request.  \e pDevice->DeviceDescriptor.bMaxPacketSize0 should be
//! initialized to zero or to the valid maximum packet size if it is known.  If
//! this variable is not set to zero, then this call will determine the maximum
//! packet size for endpoint 0 and save it in the structure member
//! bMaxPacketSize0.
//!
//! \return The number of bytes returned due to the request.  This value can be
//! zero if the device did not respond.
//
//*****************************************************************************
static unsigned long
USBHCDGetDeviceDescriptor(unsigned long ulIndex, tUSBHostDevice *pDevice)
{
    tUSBRequest SetupPacket;
    unsigned long ulBytes;

    ASSERT(ulIndex == 0);

    //
    // This is a Standard Device IN request.
    //
    SetupPacket.bmRequestType =
        USB_RTYPE_DIR_IN | USB_RTYPE_STANDARD | USB_RTYPE_DEVICE;

    //
    // Request a Device Descriptor.
    //
    SetupPacket.bRequest = USBREQ_GET_DESCRIPTOR;
    SetupPacket.wValue = USB_DTYPE_DEVICE << 8;

    //
    // Index is always 0 for device requests.
    //
    SetupPacket.wIndex = 0;

    //
    // All devices must have at least an 8 byte max packet size so just ask
    // for 8 bytes to start with.
    //
    SetupPacket.wLength = sizeof(tDeviceDescriptor);

    ulBytes = 0;

    //
    // Discover the max packet size for endpoint 0.
    //
    if(pDevice->DeviceDescriptor.bMaxPacketSize0 == 0)
    {
        //
        // Put the setup packet in the buffer.
        //
        ulBytes =
            USBHCDControlTransfer(ulIndex, &SetupPacket, pDevice,
                                  (unsigned char *)&(pDevice->DeviceDescriptor),
                                  sizeof(tDeviceDescriptor),
                                  8);
    }

    //
    // Now get the full descriptor now that the actual maximum packet size
    // is known.
    //
    if(ulBytes < sizeof(tDeviceDescriptor))
    {
        SetupPacket.wLength = (unsigned short)sizeof(tDeviceDescriptor);

        ulBytes =
            USBHCDControlTransfer(ulIndex, &SetupPacket, pDevice,
                                  (unsigned char *)&(pDevice->DeviceDescriptor),
                                  sizeof(tDeviceDescriptor),
                                  pDevice->DeviceDescriptor.bMaxPacketSize0);
    }

    return(ulBytes);
}

//*****************************************************************************
//
//! This function is used to send the set address command to a device.
//!
//! \param ulDevIndex is the index of the device whose address is to be
//! set.  This value must be 0 to indicate that the device is connected
//! directly to the host controller.  Higher values indicate devices connected
//!  via a hub.
//! \param ulDevAddress is the new device address to use for a device.
//!
//! The USBHCDSetAddress() function is used to set the USB device address, once
//! a device has been discovered on the bus.  This call is typically issued
//! following a USB reset triggered by a call the USBHCDReset().  The
//! address passed into this function via the \e ulDevAddress parameter is used
//! for all further communications with the device after this function
//! returns.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDSetAddress(unsigned long ulDevIndex, unsigned long ulDevAddress)
{
    tUSBRequest SetupPacket;

    //
    // This is a Standard Device OUT request.
    //
    SetupPacket.bmRequestType =
        USB_RTYPE_DIR_OUT | USB_RTYPE_STANDARD | USB_RTYPE_DEVICE;

    //
    // Request a Device Descriptor.
    //
    SetupPacket.bRequest = USBREQ_SET_ADDRESS;
    SetupPacket.wValue = ulDevAddress;

    //
    // Index is always 0 for device requests.
    //
    SetupPacket.wIndex = 0;

    //
    // Only request the space available.
    //
    SetupPacket.wLength = 0;

    //
    // Put the setup packet in the buffer.
    //
    USBHCDControlTransfer(0, &SetupPacket, &g_sUSBHCD.USBDevice[ulDevIndex], 0,
                          0, MAX_PACKET_SIZE_EP0);

    //
    // Must delay 2ms after setting the address.
    //
    OS_DELAY(g_ulTickms * 2);
}

//*****************************************************************************
//
//! This function is used to send a Clear Feature request to a device.
//!
//! \param ulDevAddress is the USB bus address of the device that will receive
//! this request.
//! \param ulPipe is the pipe that will be used to send the request.
//! \param ulFeature is one of the USB_FEATURE_* definitions.
//!
//! This function will issue a Clear Feature request to the device indicated
//! by the \e ulDevAddress parameter.  The \e ulPipe parameter is the USB pipe
//! that should be used to send this request.  The \e ulFeature parameter
//! should be one of the following values:
//!
//! * \b USB_FEATURE_EP_HALT is used to end a HALT condition on a devices
//!   endpoint.
//! * \b USB_FEATURE_REMOTE_WAKE is used to disable a device's remote wake
//!   feature.
//! * \b USB_FEATURE_TEST_MODE is used take the USB device out of test mode.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDClearFeature(unsigned long ulDevAddress, unsigned long ulPipe,
                   unsigned long ulFeature)
{
    tUSBRequest SetupPacket;
    unsigned long ulIndex;

    //
    // Get the index number from the allocated pipe.
    //
    ulIndex = (ulPipe & EP_PIPE_IDX_M);

    //
    // This is a Standard Device OUT request.
    //
    SetupPacket.bmRequestType =
        USB_RTYPE_DIR_OUT | USB_RTYPE_STANDARD | USB_RTYPE_ENDPOINT;

    //
    // Request a Device Descriptor.
    //
    SetupPacket.bRequest = USBREQ_CLEAR_FEATURE;
    SetupPacket.wValue = ulFeature;

    //
    // Set the endpoint to access.
    //
    if(ulPipe & EP_PIPE_TYPE_IN)
    {
        SetupPacket.wIndex = g_sUSBHCD.USBINPipes[ulIndex].ucEPNumber | 0x80;
    }
    else
    {
        SetupPacket.wIndex = g_sUSBHCD.USBOUTPipes[ulIndex].ucEPNumber;
    }

    //
    // This is always 0.
    //
    SetupPacket.wLength = 0;

    //
    // Put the setup packet in the buffer.
    //
    USBHCDControlTransfer(0, &SetupPacket,
                          &g_sUSBHCD.USBDevice[ulDevAddress - 1], 0, 0,
                          MAX_PACKET_SIZE_EP0);

    //
    // Set the endpoint to access.
    //
    if(ulPipe & EP_PIPE_TYPE_IN)
    {
        MAP_USBEndpointDataToggleClear(USB0_BASE, INDEX_TO_USB_EP(ulIndex + 1),
                                       USB_EP_HOST_IN);
    }
    else
    {
        MAP_USBEndpointDataToggleClear(USB0_BASE, INDEX_TO_USB_EP(ulIndex + 1),
                                       USB_EP_HOST_OUT);
    }

    //
    // Must delay 2ms after clearing the feature.
    //
    OS_DELAY(g_ulTickms * 2);
}

//*****************************************************************************
//
//! This function is used to set the current configuration for a device.
//!
//! \param ulIndex specifies which USB controller to use.
//! \param ulDevice is the USB device for this function.
//! \param ulConfiguration is one of the devices valid configurations.
//!
//! This function is used to set the current device configuration for a USB
//! device.  The \e ulConfiguration value must be one of the configuration
//! indexes that was returned in the configuration descriptor from the device,
//! or a value of 0.  If 0 is passed in, the device will return to it's
//! addressed state and no longer be in a configured state.  If the value is
//! non-zero then the device will change to the requested configuration.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDSetConfig(unsigned long ulIndex, unsigned long ulDevice,
                unsigned long ulConfiguration)
{
    tUSBRequest SetupPacket;
    tUSBHostDevice *pDevice;

    ASSERT(ulIndex == 0);

    pDevice = (tUSBHostDevice *)ulDevice;

    //
    // This is a Standard Device OUT request.
    //
    SetupPacket.bmRequestType =
        USB_RTYPE_DIR_OUT | USB_RTYPE_STANDARD | USB_RTYPE_DEVICE;

    //
    // Request a Device Descriptor.
    //
    SetupPacket.bRequest = USBREQ_SET_CONFIG;
    SetupPacket.wValue = ulConfiguration;

    //
    // Index is always 0 for device requests.
    //
    SetupPacket.wIndex = 0;

    //
    // Only request the space available.
    //
    SetupPacket.wLength = 0;

    //
    // Put the setup packet in the buffer.
    //
    USBHCDControlTransfer(0, &SetupPacket, pDevice, 0, 0,
                          MAX_PACKET_SIZE_EP0);
}

//*****************************************************************************
//
//! This function is used to set the current interface and alternate setting
//! for an interface on a device.
//!
//! \param ulIndex specifies which USB controller to use.
//! \param ulDevice is the USB device for this function.
//! \param ulInterface is one of the valid interface numbers for a device.
//! \param ulAltSetting is one of the valid alternate interfaces for the
//! ulInterface number.
//!
//! This function is used to change the alternate setting for one of the valid
//! interfaces on a USB device.  The \e ulDevice specifies the device instance
//! that was returned when the device was connected.  This call will set the
//! USB device's interface based on the \e ulInterface and \e ulAltSetting.
//!
//! \b Example: Set the USB device interface 2 to alternate setting 1.
//!
//! \verbatim
//! USBHCDSetInterface(0, ulDevice, 2, 1);
//! \endverbatim
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDSetInterface(unsigned long ulIndex, unsigned long ulDevice,
                   unsigned long ulInterface, unsigned ulAltSetting)
{
    tUSBRequest SetupPacket;
    tUSBHostDevice *pDevice;

    ASSERT(ulIndex == 0);

    pDevice = (tUSBHostDevice *)ulDevice;

    //
    // This is a Standard Device OUT request.
    //
    SetupPacket.bmRequestType =
        USB_RTYPE_DIR_OUT | USB_RTYPE_STANDARD | USB_RTYPE_INTERFACE;

    //
    // Request a Device Descriptor.
    //
    SetupPacket.bRequest = USBREQ_SET_INTERFACE;

    //
    // Index is the interface to access.
    //
    SetupPacket.wIndex = ulInterface;

    //
    // wValue is the alternate setting.
    //
    SetupPacket.wValue = ulAltSetting;


    //
    // Only request the space available.
    //
    SetupPacket.wLength = 0;

    //
    // Put the setup packet in the buffer.
    //
    USBHCDControlTransfer(0, &SetupPacket, pDevice, 0, 0,
                          MAX_PACKET_SIZE_EP0);
}

//*****************************************************************************
//
// The internal function to see if a new schedule event should occur.
//
// This function is called by the main interrupt handler due to start of frame
// interrupts to determine if a new scheduler event should be sent to the USB
// pipe.
//
// \return None.
//
//*****************************************************************************
void
USBHostCheckPipes(void)
{
    long lIdx;

    g_ulCurrentTick++;

    for(lIdx = 0; lIdx < g_sUSBHCD.ulNumEndpoints; lIdx++)
    {
        //
        // Skip unused pipes.
        //
        if(g_sUSBHCD.USBINPipes[lIdx].ulType == USBHCD_PIPE_UNUSED)
        {
            continue;
        }

        //
        // If the tick has expired and it has an interval then update it.
        //
        if((g_sUSBHCD.USBINPipes[lIdx].ulInterval != 0) &&
           (g_sUSBHCD.USBINPipes[lIdx].ulNextEventTick == g_ulCurrentTick))
        {
            //
            // Schedule the next event.
            //
            g_sUSBHCD.USBINPipes[lIdx].ulNextEventTick +=
                g_sUSBHCD.USBINPipes[lIdx].ulInterval;

            //
            // If the pipe is IDLE and there is a callback, let the higher
            // level drivers know that a new transfer can be scheduled.
            //
            if((g_sUSBHCD.USBINPipes[lIdx].eState == PIPE_IDLE) &&
               (g_sUSBHCD.USBINPipes[lIdx].pfnCallback))
            {
                g_sUSBHCD.USBINPipes[lIdx].pfnCallback(IN_PIPE_HANDLE(lIdx),
                                                       USB_EVENT_SCHEDULER);
            }
        }
    }
}

//*****************************************************************************
//
// The internal USB host mode interrupt handler.
//
// \param ulIndex is the USB controller associated with this interrupt.
// \param ulStatus is the current interrupt status as read via a call to
// \e USBIntStatusControl().
//
// This the main USB interrupt handler called when operating in host mode.
// This handler will branch the interrupt off to the appropriate handlers
// depending on the current status of the USB controller.
//
// The two-tiered structure for the interrupt handler ensures that it is
// possible to use the same handler code in both host and OTG modes and
// means that device code can be excluded from applications that only require
// support for USB host mode operation.
//
// \return None.
//
//*****************************************************************************
void
USBHostIntHandlerInternal(unsigned long ulIndex, unsigned long ulStatus)
{
    unsigned long ulEPStatus;
    static unsigned long ulSOFDivide = 0;
    unsigned long ulEvent;
    unsigned long ulIdx;
    unsigned long ulDevIndex;
    long lClassDrvr;

    //
    // By default, assume we are dealing with the device directly connected
    // to the host controller and that we need to notify its class driver of
    // this interrupt.
    //
    g_sUSBHCD.USBDevice[0].bNotifyInt = true;

    if(ulStatus & USB_INTCTRL_SOF)
    {
        //
        // Indicate that a start of frame has occurred.
        //
        g_ulUSBHIntEvents |= INT_EVENT_SOF;
    }

    //
    // A power fault has occurred so notify the application.
    //
    if(ulStatus & USB_INTCTRL_POWER_FAULT)
    {
        //
        // Indicate that a power fault has occurred.
        //
        g_ulUSBHIntEvents |= INT_EVENT_POWER_FAULT;

        //
        // Turn off power to the bus.
        //
        MAP_USBHostPwrDisable(USB0_BASE);

        //
        // Disable USB interrupts.
        //
        OS_INT_DISABLE(INT_USB0);

        return;
    }

    //
    // In the event of a USB VBUS error, end the session and remove power to
    // the device.
    //
    if(ulStatus & USB_INTCTRL_VBUS_ERR)
    {
        //
        // Set the VBUS error event.  We deliberately clear all other events
        // since this one means anything else that is outstanding is
        // irrelevant.
        //
        g_ulUSBHIntEvents = INT_EVENT_VBUS_ERR;
        return;
    }

    //
    // Received a reset from the host.
    //
    if(ulStatus & USB_INTCTRL_BABBLE)
    {
    }

    //
    // Suspend was signaled on the bus.
    //
    if(ulStatus & USB_INTCTRL_SUSPEND)
    {
    }

    //
    // Start the session.
    //
    if(ulStatus & USB_INTCTRL_SESSION)
    {
        //
        // Power the USB bus.
        //
        MAP_USBHostPwrEnable(USB0_BASE);

        USBOTGSessionRequest(USB0_BASE, true);
    }

    //
    // Resume was signaled on the bus.
    //
    if(ulStatus & USB_INTCTRL_RESUME)
    {
    }

    //
    // Device connected so tell the main routine to issue a reset.
    //
    if(ulStatus & USB_INTCTRL_CONNECT)
    {
        //
        // Set the connect flag and clear disconnect if it happens to be set.
        //
        g_ulUSBHIntEvents |= INT_EVENT_CONNECT;
        g_ulUSBHIntEvents &= ~INT_EVENT_DISCONNECT;

        //
        // Power the USB bus.
        //
        MAP_USBHostPwrEnable(USB0_BASE);
    }

    //
    // Handle the ID detection so that the ID pin can be used as a
    // GPIO in USB_MODE_HOST.
    //
    if(ulStatus & USB_INTCTRL_MODE_DETECT)
    {
        //
        // If in USB_MODE_HOST mode then switch back to OTG detection
        // so that VBUS can be monitored but free up the ID pin.
        //
        if(g_eUSBMode == USB_MODE_HOST)
        {
            USBOTGMode(USB0_BASE);
        }
    }

    //
    // Device was unplugged.
    //
    if(ulStatus & USB_INTCTRL_DISCONNECT)
    {
        //
        // Set the disconnect flag and clear connect if it happens to be set.
        //
        g_ulUSBHIntEvents |= INT_EVENT_DISCONNECT;
        g_ulUSBHIntEvents &= ~INT_EVENT_CONNECT;
    }

    //
    // Start of Frame was received.
    //
    if(ulStatus & USB_INTCTRL_SOF)
    {
        //
        // Increment the global Start of Frame counter.
        //
        g_ulUSBSOFCount++;

        //
        // Increment our SOF divider.
        //
        ulSOFDivide++;

        //
        // Have we counted enough SOFs to allow us to call the tick function?
        //
        if(ulSOFDivide == USB_SOF_TICK_DIVIDE)
        {
            //
            // Yes - reset the divider and call the SOF tick handler.
            //
            ulSOFDivide = 0;
            InternalUSBStartOfFrameTick(USB_SOF_TICK_DIVIDE);
        }
    }

    //
    // Get the current endpoint interrupt status.
    //
    ulStatus = MAP_USBIntStatusEndpoint(USB0_BASE);

    //
    // Handle end point 0 interrupts.
    //
    if(ulStatus & USB_INTEP_0)
    {
        //
        // Indicate that a start of frame has occurred.
        //
        g_ulUSBHIntEvents |= INT_EVENT_ENUM;
    }

    //
    // Check to see if any uDMA transfers are pending
    //
    for(ulIdx = 0; ulIdx < MAX_NUM_PIPES; ulIdx++)
    {
        if((g_ulDMAPending == 0) && (ulStatus == 0))
        {
            break;
        }

        //
        // Check each pipe to see if uDMA is pending
        //
        if(g_ulDMAPending & (DMA_PEND_RECEIVE_FLAG << ulIdx))
        {
            //
            // Handle the case where the pipe is reading
            //
            if(g_sUSBHCD.USBINPipes[ulIdx].eState == PIPE_READING)
            {
                //
                // If the DMA channel transfer is complete, send an ack.
                //
                if(uDMAChannelModeGet(UDMA_CHANNEL_USBEP1RX + (ulIdx * 2))
                        == UDMA_MODE_STOP)
                {
                    MAP_USBHostEndpointDataAck(USB0_BASE,
                                               INDEX_TO_USB_EP(ulIdx + 1));
                    g_ulDMAPending &= ~(DMA_PEND_RECEIVE_FLAG << ulIdx);

                    //
                    // If using uDMA then the endpoint status int will not
                    // occur.  So process the data ready event here.
                    //
                    g_sUSBHCD.USBINPipes[ulIdx].eState = PIPE_DATA_READY;
                    ulEvent = USB_EVENT_RX_AVAILABLE;

                    //
                    // Only call a handler if one is present.
                    //
                    if(g_sUSBHCD.USBINPipes[ulIdx].pfnCallback)
                    {
                        g_sUSBHCD.USBINPipes[ulIdx].pfnCallback(
                            IN_PIPE_HANDLE(ulIdx), ulEvent);
                    }

                    //
                    // Remember that we need to notify this device's class
                    // driver that an interrupt occurred.
                    //
                    g_sUSBHCD.USBINPipes[ulIdx].psDevice->bNotifyInt = true;
                }
            }
        }

        //
        // Check for a pending DMA transmit transaction.
        //
        if(g_ulDMAPending & (DMA_PEND_TRANSMIT_FLAG << ulIdx))
        {
            //
            // Handle the case where the pipe is writing
            //
            if(g_sUSBHCD.USBOUTPipes[ulIdx].eState == PIPE_WRITING)
            {
                //
                // If the uDMA channel transfer is complete, then tell
                // the USB controller to go ahead and send the data
                //
                if(uDMAChannelModeGet(UDMA_CHANNEL_USBEP1TX + (ulIdx * 2))
                        == UDMA_MODE_STOP)
                    {
                    MAP_USBEndpointDataSend(USB0_BASE,
                                            INDEX_TO_USB_EP(ulIdx + 1),
                                            USB_TRANS_OUT);
                    g_ulDMAPending &= ~(DMA_PEND_TRANSMIT_FLAG << ulIdx);
                }
            }
            }

        //
        // Check the next pipe, the first time through this will clear out
        // any interrupts dealing with endpoint zero since it was handled above.
        //
        ulStatus >>= 1;

        //
        // Check the status of the transmit(OUT) pipes.
        //
        if(ulStatus & 1)
        {
            //
            // Read the status of the endpoint connected to this pipe.
            //
            ulEPStatus = MAP_USBEndpointStatus(USB0_BASE,
                                               INDEX_TO_USB_EP(ulIdx + 1));

            //
            // Check if the device stalled the request.
            //
            if(ulEPStatus & USB_HOST_OUT_STALL)
            {
                //
                // Clear the stall condition on this endpoint pipe.
                //
                MAP_USBHostEndpointStatusClear(USB0_BASE,
                                               INDEX_TO_USB_EP(ulIdx + 1),
                                               USB_HOST_OUT_STALL);

                //
                // Save the STALLED state.
                //
                g_sUSBHCD.USBOUTPipes[ulIdx].eState = PIPE_STALLED;

                //
                // Notify the pipe that it was stalled.
                //
                ulEvent = USB_EVENT_STALL;
            }
            else if(ulEPStatus & USB_HOST_OUT_ERROR)
            {
                //
                // Clear the error condition on this endpoint pipe.
                //
                MAP_USBHostEndpointStatusClear(USB0_BASE,
                                               INDEX_TO_USB_EP(ulIdx + 1),
                                               USB_HOST_OUT_ERROR);

                //
                // Save the Pipes error state.
                //
                g_sUSBHCD.USBOUTPipes[ulIdx].eState = PIPE_ERROR;

                //
                // Notify the pipe that had an error.
                //
                ulEvent = USB_EVENT_ERROR;
            }
            else
            {
                //
                // Data was transmitted successfully.
                //
                g_sUSBHCD.USBOUTPipes[ulIdx].eState = PIPE_DATA_SENT;

                //
                // Notify the pipe that its last transaction was completed.
                //
                ulEvent = USB_EVENT_TX_COMPLETE;
            }

            //
            // Clear the stall condition on this endpoint pipe.
            //
            MAP_USBHostEndpointStatusClear(USB0_BASE,
                                           INDEX_TO_USB_EP(ulIdx + 1),
                                           ulEPStatus);

            //
            // Only call a handler if one is present.
            //
            if(g_sUSBHCD.USBOUTPipes[ulIdx].pfnCallback)
            {
                g_sUSBHCD.USBOUTPipes[ulIdx].pfnCallback(OUT_PIPE_HANDLE(ulIdx),
                                                         ulEvent);
            }

            //
            // Remember that we need to notify this device's class
            // driver that an interrupt occurred.
            //
            g_sUSBHCD.USBOUTPipes[ulIdx].psDevice->bNotifyInt = true;
        }

        //
        // Check the status of the receive(IN) pipes.
        //
        if(ulStatus & 0x10000)
        {
            //
            // Clear the status flag for the IN Pipe.
            //
            ulStatus &= ~0x10000;

            //
            // Read the status of the endpoint connected to this pipe.
            //
            ulEPStatus = MAP_USBEndpointStatus(USB0_BASE,
                                               INDEX_TO_USB_EP(ulIdx + 1));

            //
            // Check if the device stalled the request.
            //
            if(ulEPStatus & USB_HOST_IN_STALL)
            {
                //
                // Clear the stall condition on this endpoint pipe.
                //
                MAP_USBHostEndpointStatusClear(USB0_BASE,
                                               INDEX_TO_USB_EP(ulIdx + 1),
                                               USB_HOST_IN_STALL);

                //
                // Save the STALLED state.
                //
                g_sUSBHCD.USBINPipes[ulIdx].eState = PIPE_STALLED;

                //
                // Notify the pipe that it was stalled.
                //
                ulEvent = USB_EVENT_STALL;
            }
            else if(ulEPStatus & USB_HOST_IN_ERROR)
            {
                //
                // We can no longer communicate with this device for some
                // reason.  It may have been disconnected from a hub, for
                // example.  Merely clear the status and continue.
                //
                USBHostEndpointStatusClear(USB0_BASE,
                                           INDEX_TO_USB_EP(ulIdx + 1),
                                           USB_HOST_IN_ERROR);

                //
                // Save the STALLED state.
                //
                g_sUSBHCD.USBINPipes[ulIdx].eState = PIPE_ERROR;

                //
                // Notify the pipe that it was stalled.
                //
                ulEvent = USB_EVENT_ERROR;
            }
            else
            {
                //
                // Data is available.
                //
                g_sUSBHCD.USBINPipes[ulIdx].eState = PIPE_DATA_READY;

                //
                // Read the data out of the USB endpoint interface into the
                // buffer provided by the caller to USBHCDPipeRead() or
                // USBHCDPipeSchedule() if a buffer was provided already.
                //
                if(g_sUSBHCD.USBINPipes[ulIdx].pucReadPtr)
                {
                    USBEndpointDataGet(USB0_BASE, INDEX_TO_USB_EP(ulIdx + 1),
                                       g_sUSBHCD.USBINPipes[ulIdx].pucReadPtr,
                                       &g_sUSBHCD.USBINPipes[ulIdx].ulReadSize);
                }

                //
                // Notify the pipe that its last transaction was completed.
                //
                ulEvent = USB_EVENT_RX_AVAILABLE;
            }

            //
            // Only call a handler if one is present.
            //
            if(g_sUSBHCD.USBINPipes[ulIdx].pfnCallback)
            {
                g_sUSBHCD.USBINPipes[ulIdx].pfnCallback(IN_PIPE_HANDLE(ulIdx),
                                                        ulEvent);
            }

            //
            // Remember that we need to notify this device's class
            // driver that an interrupt occurred.
            //
            g_sUSBHCD.USBINPipes[ulIdx].psDevice->bNotifyInt = true;
        }
    }

    //
    // Send back notifications to any class driver whose endpoint required
    // service during the handler.
    //
    for(ulDevIndex = 0; ulDevIndex <= MAX_USB_DEVICES; ulDevIndex++)
    {
        //
        // Which class driver does this device use?
        //
        lClassDrvr = g_lUSBHActiveDriver[ulDevIndex];

        //
        // If a class driver is in use, and one of its endpoints was serviced
        // and the class driver has an interrupt callback...
        //
        if((lClassDrvr >= 0) && g_sUSBHCD.USBDevice[ulDevIndex].bNotifyInt &&
           (g_sUSBHCD.pClassDrivers[lClassDrvr]->pfnIntHandler))
        {
            //
            // ...call the class driver's interrupt notification callback.
            //
            g_sUSBHCD.pClassDrivers[lClassDrvr]->pfnIntHandler(
                                               g_pvDriverInstance[ulDevIndex]);
        }
    }
}

//*****************************************************************************
//
//! The USB host mode interrupt handler for controller index 0.
//!
//! This the main USB interrupt handler entry point.  This handler will branch
//! the interrupt off to the appropriate handlers depending on the current
//! status of the USB controller.   This function must be placed in the
//! interrupt table in order for the USB Library host stack to function.
//!
//! \return None.
//
//*****************************************************************************
void
USB0HostIntHandler(void)
{
    unsigned long ulStatus;

    //
    // Get the control interrupt status.
    //
    ulStatus = MAP_USBIntStatusControl(USB0_BASE);

    //
    // Call the internal handler to process the interrupts.
    //
    USBHostIntHandlerInternal(0, ulStatus);
}

//*****************************************************************************
//
//! This function opens the class driver.
//!
//! \param ulIndex specifies which USB controller to use.
//! \param ulDeviceNum is the device number for the driver to load.
//!
//! This function opens the driver needed based on the class value found in
//! the device's interface descriptor.
//!
//! \return This function returns -1 if no driver is found, or it returns the
//! index of the driver found in the list of host class drivers.
//
//*****************************************************************************
static long
USBHCDOpenDriver(unsigned long ulIndex, unsigned long ulDeviceNum)
{
    long lDriver;
    unsigned long ulClass;
    tInterfaceDescriptor *pInterface;
    tEventInfo sEvent;

    ASSERT(ulIndex == 0);

    //
    // Get the interface descriptor.
    //
    pInterface = USBDescGetInterface(
                            g_sUSBHCD.USBDevice[ulDeviceNum].pConfigDescriptor,
                            g_sUSBHCD.USBDevice[ulDeviceNum].ulInterface,
                            USB_DESC_ANY);

    //
    // Read the interface class.
    //
    ulClass = pInterface->bInterfaceClass;

    //
    // Search through the Host Class driver list for the devices class.
    //
    for(lDriver = 0; lDriver < g_sUSBHCD.ulNumClassDrivers; lDriver++)
    {
        //
        // If a driver was found call the open for this driver and save which
        // driver is in use.
        //
        if(g_sUSBHCD.pClassDrivers[lDriver]->ulInterfaceClass == ulClass)
        {
            //
            // Call the open function for the class driver.
            //
            g_pvDriverInstance[ulDeviceNum] =
                    g_sUSBHCD.pClassDrivers[lDriver]->pfnOpen(
                            &g_sUSBHCD.USBDevice[ulDeviceNum]);

            //
            // If the driver was successfully loaded then break out of the
            // loop.
            //
            if(g_pvDriverInstance[ulDeviceNum] != 0)
            {
                break;
            }
        }
    }

    //
    // If no drivers were found then return -1 to indicate an invalid
    // driver instance.
    //
    if(lDriver == g_sUSBHCD.ulNumClassDrivers)
    {
        //
        // Send an unknown connection event.
        //
        SendUnknownConnect(ulIndex, ulClass);

        //
        // Indicate that no driver was found.
        //
        lDriver = -1;
    }

    //
    // If the connect event is enabled then send the event.
    //
    sEvent.ulEvent = USB_EVENT_CONNECTED;
    sEvent.ulInstance = (ulIndex << 16) | ulDeviceNum;
    InternalUSBHCDSendEvent(0, &sEvent, USBHCD_EVFLAG_CONNECT);

    return(lDriver);
}

//*****************************************************************************
//
// This function will send an event to a registered event driver.
//
// \param ulIndex specifies which USB controller to use.
// \param psEvent is a pointer to the event structure to send.
//
// This function is only used internally to the USB library and will check
// if an event driver is registered and send on the event.
//
// Note: This function should not be called outside of the USB library.
//
// \return None.
//
//*****************************************************************************
void
InternalUSBHCDSendEvent(unsigned long ulIndex, tEventInfo *psEvent,
                        unsigned long ulEvFlag)
{
    //
    // Make sure that an event driver has been registered.
    //
    if((g_sUSBHCD.lEventDriver != -1) &&
       (g_sUSBHCD.pClassDrivers[g_sUSBHCD.lEventDriver]->pfnIntHandler) &&
       (g_sUSBHCD.ulEventEnables & ulEvFlag))
    {
        g_sUSBHCD.pClassDrivers[g_sUSBHCD.lEventDriver]->pfnIntHandler(psEvent);
    }
}

//*****************************************************************************
//
// This function handles the necessary clean up for device disconnect.
//
// \param ulIndex is the device number for the device that was disconnected.
//
// This function handles all of the necessary clean up after a device
// disconnect has been detected by the stack.  This includes calling back the
// appropriate driver if necessary.
//
// \return None.
//
//*****************************************************************************
static void
USBHCDDeviceDisconnected(unsigned long ulIndex, unsigned long ulDevIndex)
{
    tEventInfo sEvent;

    ASSERT(ulIndex == 0);
    ASSERT(ulDevIndex <= MAX_USB_DEVICES);

    //
    // If there is an event driver with a valid event handler and the
    // USBHCD_EVFLAG_DISCNCT is enabled, then call the registered event handler.
    //
    sEvent.ulEvent = USB_EVENT_DISCONNECTED;
    sEvent.ulInstance = (ulIndex << 16) | ulDevIndex;
    InternalUSBHCDSendEvent(0, &sEvent, USBHCD_EVFLAG_DISCNCT);

    //
    // Reset the class.
    //
    g_sUSBHCD.ulClass = USB_CLASS_EVENTS;

    if(g_sUSBHCD.USBDevice[ulDevIndex].pConfigDescriptor)
    {
        //
        // Invalidate the configuration descriptor.
        //
        g_sUSBHCD.USBDevice[ulDevIndex].pConfigDescriptor = 0;
        g_sUSBHCD.USBDevice[ulDevIndex].bConfigRead = false;

    }

    //
    // Reset the max packet size so that this will be re-read from new devices.
    //
    g_sUSBHCD.USBDevice[ulDevIndex].DeviceDescriptor.bMaxPacketSize0 = 0;

    //
    // No longer have a device descriptor.
    //
    g_sUSBHCD.USBDevice[ulDevIndex].DeviceDescriptor.bLength = 0;

    //
    // No longer addressed.
    //
    g_sUSBHCD.USBDevice[ulDevIndex].ulAddress = 0;

    //
    // If this was an active driver then close it out.
    //
    if(g_lUSBHActiveDriver[ulDevIndex] >= 0)
    {
        //
        // Call the driver Close entry point.
        //
        g_sUSBHCD.pClassDrivers[g_lUSBHActiveDriver[ulDevIndex]]->
            pfnClose(g_pvDriverInstance[ulDevIndex]);

        //
        // No active driver now present.
        //
        g_lUSBHActiveDriver[ulDevIndex] = -1;
        g_pvDriverInstance[ulDevIndex] = 0;
    }

    //
    // This call is necessary for OTG controllers to know that the host
    // stack has completed handling the disconnect of the device before
    // removing power and returning to a state that can allow OTG
    // negotiations once again.
    // We only do this if the disconnected device
    // was attached directly to us (device index 0).
    //
    if((ulDevIndex == 0) && (g_eUSBMode == USB_MODE_OTG))
    {
        OTGDeviceDisconnect(0);
    }
}

//*****************************************************************************
//
//! This function is the main routine for the Host Controller Driver.
//!
//! This function is the main routine for the host controller driver, and must
//! be called periodically by the main application outside of a callback
//! context.  This allows for a simple cooperative system to access the the
//! host controller driver interface without the need for an RTOS.  All time
//! critical operations are handled in interrupt context but all blocking
//! operations are run from the this function to allow them to block and wait
//! for completion without holding off other interrupts.
//!
//! \return None.
//
//*****************************************************************************
void
USBHCDMain(void)
{
    tUSBHDeviceState eOldState;
    unsigned long ulLoop;
    tEventInfo sEvent;

    //
    // Save the old state to detect changes properly.
    //
    eOldState = g_sUSBHCD.eDeviceState[0];

    //
    // Fix up the state if any important interrupt events occurred.
    //
    if(g_ulUSBHIntEvents)
    {
        //
        // Disable the USB interrupt.
        //
        OS_INT_DISABLE(INT_USB0);

        if(g_ulUSBHIntEvents & INT_EVENT_POWER_FAULT)
        {
            //
            // A power fault has occurred so notify the application if there
            // is an event handler and the event has been enabled.
            //
            sEvent.ulEvent = USB_EVENT_POWER_FAULT;
            sEvent.ulInstance = 0;
            InternalUSBHCDSendEvent(0, &sEvent, USBHCD_EVFLAG_PWRFAULT);

            g_sUSBHCD.eDeviceState[0] = HCD_POWER_FAULT;
        }
        else if(g_ulUSBHIntEvents & INT_EVENT_VBUS_ERR)
        {
            //
            // A VBUS error has occurred.  This event trumps connect and
            // disconnect since it will cause a controller reset.
            //
            g_sUSBHCD.eDeviceState[0] = HCD_VBUS_ERROR;
        }
        else
        {
            //
            // Has a device connected?
            //
            if(g_ulUSBHIntEvents & INT_EVENT_CONNECT)
            {
                g_sUSBHCD.eDeviceState[0] = HCD_DEV_RESET;
                g_sUSBHCD.USBDevice[0].ucHub = 0;
                g_sUSBHCD.USBDevice[0].ucHubPort = 0;
            }
            else
            {
                //
                // Has a device disconnected?
                //
                if(g_ulUSBHIntEvents & INT_EVENT_DISCONNECT)
                {
                    g_sUSBHCD.eDeviceState[0] = HCD_DEV_DISCONNECTED;
                }
            }

            //
            // Handle the start of frame event
            //
            if(g_ulUSBHIntEvents & INT_EVENT_SOF)
            {
                //
                // If the connect event is enabled then send the event.
                //
                sEvent.ulEvent = USB_EVENT_SOF;
                sEvent.ulInstance = 0;
                InternalUSBHCDSendEvent(0, &sEvent, USBHCD_EVFLAG_SOF);

                USBHostCheckPipes();

                //
                // Call the hub driver to have it perform any necessary processing to
                // handle downstream devices.
                //
                USBHHubMain();
            }
        }

        //
        // Clear the flags.
        //
        g_ulUSBHIntEvents = 0;

        //
        // Enable the USB interrupt.
        //
        OS_INT_ENABLE(INT_USB0);
    }

    //
    // Process the state machine for each connected device.  Yes, the exit
    // condition for this loop is correct since we support (MAX_USB_DEVICES+1)
    // devices (the hub counts as one).
    //
    for(ulLoop = 0; ulLoop <= MAX_USB_DEVICES; ulLoop++)
    {
        //
        // If this is not the first device (i.e. the one directly connected to
        // the host controller) then set the old state to the current state
        // since we won't have mucked with it in any of the previous code.
        //
        if(ulLoop != 0)
        {
            eOldState = g_sUSBHCD.eDeviceState[ulLoop];
        }

        //
        // Process the state machine for this device.
        //
        ProcessUSBDeviceStateMachine(eOldState, ulLoop);
    }
}

static void
ProcessUSBDeviceStateMachine(tUSBHDeviceState eOldState,
                             unsigned long ulDevIndex)
{
    switch(g_sUSBHCD.eDeviceState[ulDevIndex])
    {
        //
        // There was a power fault condition so shut down and wait for the
        // application to re-initialized the system.
        //
        case HCD_POWER_FAULT:
        {
            break;
        }

        //
        // There was a VBUS error so handle it.
        //
        case HCD_VBUS_ERROR:
        {
            //
            // Disable USB interrupts.
            //
            OS_INT_DISABLE(INT_USB0);

            //
            // If there was a device in any state of connection then indicate
            // that it has been disconnected.
            //
            if((eOldState != HCD_IDLE) && (eOldState != HCD_POWER_FAULT))
            {
                //
                // Handle device disconnect.
                //
                USBHCDDeviceDisconnected(0, ulDevIndex);
            }

            //
            // Reset the controller.
            //
            MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_USB0);

            //
            // Wait for 100ms before trying to re-power the device.
            //
            OS_DELAY(g_ulTickms * 100);

            //
            // Re-initialize the HCD.
            //
            USBHCDInitInternal(0, g_sUSBHCD.pvPool, g_sUSBHCD.ulPoolSize);

            break;
        }
        //
        // Trigger a reset to the connected device.
        //
        case HCD_DEV_RESET:
        {
            if(!ulDevIndex)
            {
                //
                // Trigger a Reset.  This is only ever done for devices attached
                // directly to the controller.
                //
                DEBUG_OUTPUT("USB reset.\n");
                USBHCDReset(0);
            }

            //
            // The state moves to connected but not configured.
            //
            g_sUSBHCD.eDeviceState[0] = HCD_DEV_CONNECTED;

            //
            // Set the memory to use for the config descriptor and save the
            // size.
            //
            g_sUSBHCD.USBDevice[0].pConfigDescriptor = g_sUSBHCD.pvPool;
            g_sUSBHCD.USBDevice[0].ulConfigDescriptorSize =
                                                       g_sUSBHCD.ulPoolSize;

            //
            // Remember that we don't have a valid configuration descriptor
            // yet.
            //
            g_sUSBHCD.USBDevice[0].bConfigRead = false;

            break;
        }
        //
        // Device connection has been established now start enumerating
        // the device.
        //
        case HCD_DEV_CONNECTED:
        {
            //
            // First check if we have read the device descriptor at all
            // before proceeding.
            //
            if(g_sUSBHCD.USBDevice[ulDevIndex].DeviceDescriptor.bLength == 0)
            {
                //
                // Initialize a request for the device descriptor.
                //
                DEBUG_OUTPUT("Connection %d - getting device descriptor\n",
                             ulDevIndex);

                if(USBHCDGetDeviceDescriptor(0,
                        &g_sUSBHCD.USBDevice[ulDevIndex]) == 0)
                {
                    //
                    // If the device descriptor cannot be read then the device
                    // will be treated as unknown.
                    //
                    g_sUSBHCD.eDeviceState[ulDevIndex] = HCD_DEV_ERROR;

                    DEBUG_OUTPUT("Connection %d - failed to get descriptor\n",
                                 ulDevIndex);

                    //
                    // Send an unknown connection event.
                    //
                    SendUnknownConnect(0, 1);

                    //
                    // If the device is connected via a hub, tell the hub
                    // driver that we experienced an error enumerating the
                    // device.
                    //
                    if(g_sUSBHCD.USBDevice[ulDevIndex].ucHub)
                    {
                        USBHHubEnumerationError(
                                g_sUSBHCD.USBDevice[ulDevIndex].ucHub,
                                g_sUSBHCD.USBDevice[ulDevIndex].ucHubPort);
                    }
                }
            }
            //
            // If we have the device descriptor then move on to setting
            // the address of the device.
            //
            else if(g_sUSBHCD.USBDevice[ulDevIndex].ulAddress == 0)
            {
                DEBUG_OUTPUT("Connection %d - setting address %d\n",
                             ulDevIndex, ulDevIndex + 1);

                //
                // Send the set address command.
                //
                USBHCDSetAddress(ulDevIndex, (ulDevIndex + 1));

                //
                // Save the address.
                //
                g_sUSBHCD.USBDevice[ulDevIndex].ulAddress = (ulDevIndex + 1);

                //
                // Move on to the addressed state.
                //
                g_sUSBHCD.eDeviceState[ulDevIndex] = HCD_DEV_ADDRESSED;
            }
            break;
        }
        case HCD_DEV_ADDRESSED:
        {
            //
            // First check if we have read the configuration descriptor.
            //
            if(!g_sUSBHCD.USBDevice[ulDevIndex].bConfigRead)
            {
                DEBUG_OUTPUT("Connection %d - getting config descriptor\n",
                             ulDevIndex);

                //
                // Initialize a request for the config descriptor.
                //
                if(USBHCDGetConfigDescriptor(0,
                                        &g_sUSBHCD.USBDevice[ulDevIndex]) == 0)
                {
                    //
                    // If the device descriptor cannot be read then the device
                    // will be treated as unknown.
                    //
                    g_sUSBHCD.eDeviceState[ulDevIndex] = HCD_DEV_ERROR;

                    DEBUG_OUTPUT("Connection %d - failed to get descriptor\n",
                                 ulDevIndex);

                    //
                    // Send an unknown connection event.
                    //
                    SendUnknownConnect(0, 0);

                    //
                    // If the device is connected via a hub, tell the hub
                    // driver that we experienced an error enumerating the
                    // device.
                    //
                    if(g_sUSBHCD.USBDevice[ulDevIndex].ucHub)
                    {
                        USBHHubEnumerationError(
                                g_sUSBHCD.USBDevice[ulDevIndex].ucHub,
                                g_sUSBHCD.USBDevice[ulDevIndex].ucHubPort);
                    }
                }
            }
            //
            // Now have addressed and received the device configuration,
            // so get ready to set the device configuration.
            //
            else
            {
                DEBUG_OUTPUT("Connection %d - setting configuration.\n",
                             ulDevIndex);

                //
                // Use the first configuration to set the device
                // configuration.
                //
                USBHCDSetConfig(0,
                            (unsigned long)&g_sUSBHCD.USBDevice[ulDevIndex], 1);

                //
                // Move on to the configured state.
                //
                g_sUSBHCD.eDeviceState[ulDevIndex] = HCD_DEV_CONFIGURED;

                //
                // Open the driver for the device.
                //
                g_lUSBHActiveDriver[ulDevIndex] = USBHCDOpenDriver(0,
                                                                   ulDevIndex);

                //
                // If the device is connected via a hub, tell the hub
                // driver that enumeration is complete.
                //
                if(g_sUSBHCD.USBDevice[ulDevIndex].ucHub)
                {
                    USBHHubEnumerationComplete(
                            g_sUSBHCD.USBDevice[ulDevIndex].ucHub,
                            g_sUSBHCD.USBDevice[ulDevIndex].ucHubPort);
                }
            }
            break;
        }
        //
        // The device was making a request and is now complete.
        //
        case HCD_DEV_REQUEST:
        {
            g_sUSBHCD.eDeviceState[ulDevIndex] = HCD_DEV_CONNECTED;
            break;
        }
        //
        // The strings are currently not accessed.
        //
        case HCD_DEV_GETSTRINGS:
        {
            break;
        }
        //
        // Basically Idle at this point.
        //
        case HCD_DEV_DISCONNECTED:
        {
            DEBUG_OUTPUT("Connection %d - disconnected.\n",
                         ulDevIndex);

            //
            // Handle device disconnect.
            //
            USBHCDDeviceDisconnected(0, ulDevIndex);

            //
            // Return to the Idle state.
            //
            g_sUSBHCD.eDeviceState[ulDevIndex] = HCD_IDLE;
            break;
        }

        //
        // Connection and enumeration is complete so allow this function
        // to exit.
        //
        case HCD_DEV_CONFIGURED:
        {
            break;
        }

        //
        // Poorly behaving device are in limbo in this state until removed.
        //
        case HCD_DEV_ERROR:
        {
            DEBUG_OUTPUT("Connection %d - Error!\n", ulDevIndex);

            //
            // If this device is connected directly to us, tidy up and ignore
            // it until it is removed.  If the device is connected to a hub,
            // we just leave it in the error state until it is removed.
            //
            if(ulDevIndex == 0)
            {
                g_ulUSBHIntEvents |= INT_EVENT_DISCONNECT;
                g_sUSBHCD.eDeviceState[ulDevIndex] = HCD_IDLE;
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
//! This function completes a control transaction to a device.
//!
//! \param ulIndex is the controller index to use for this transfer.
//! \param pSetupPacket is the setup request to be sent.
//! \param pDevice is the device instance pointer for this request.
//! \param pData is the data to send for OUT requests or the receive buffer
//! for IN requests.
//! \param ulSize is the size of the buffer in pData.
//! \param ulMaxPacketSize is the maximum packet size for the device for this
//! request.
//!
//! This function handles the state changes necessary to send a control
//! transaction to a device.  This function should not be called from within
//! an interrupt callback as it is a blocking function.
//!
//! \return The number of bytes of data that were sent or received as a result
//! of this request.
//
//*****************************************************************************
unsigned long
USBHCDControlTransfer(unsigned long ulIndex, tUSBRequest *pSetupPacket,
                      tUSBHostDevice *pDevice, unsigned char *pData,
                      unsigned long ulSize, unsigned long ulMaxPacketSize)
{
    unsigned long ulRemaining;
    unsigned long ulDataSize;

    //
    // Debug sanity check.
    //
    ASSERT(g_sUSBHEP0State.eState == EP0_STATE_IDLE);
    ASSERT(ulIndex == 0);

    //
    // Initialize the state of the data for this request.
    //
    g_sUSBHEP0State.pData = pData;
    g_sUSBHEP0State.ulBytesRemaining = ulSize;
    g_sUSBHEP0State.ulDataSize = ulSize;

    //
    // Set the maximum packet size.
    //
    g_sUSBHEP0State.ulMaxPacketSize = ulMaxPacketSize;

    //
    // Save the current address.
    //
    g_sUSBHEP0State.ulDevAddress = pDevice->ulAddress;

    //
    // Set the address the host will used to communicate with the device.
    //
    MAP_USBHostAddrSet(USB0_BASE, USB_EP_0, g_sUSBHEP0State.ulDevAddress,
                       USB_EP_HOST_OUT);

    //
    // Put the data in the correct FIFO.
    //
    MAP_USBEndpointDataPut(USB0_BASE, USB_EP_0, (unsigned char *)pSetupPacket,
                           sizeof(tUSBRequest));

    //
    // If this is an IN request, change to that state.
    //
    if(pSetupPacket->bmRequestType & USB_RTYPE_DIR_IN)
    {
        g_sUSBHEP0State.eState = EP0_STATE_SETUP_IN;
    }
    else
    {
        //
        // If there is no data then this is not an OUT request.
        //
        if(ulSize != 0)
        {
            //
            // Since there is data, this is an OUT request.
            //
            g_sUSBHEP0State.eState = EP0_STATE_SETUP_OUT;
        }
        else
        {
            //
            // Otherwise this request has no data and just a status phase.
            //
            g_sUSBHEP0State.eState = EP0_STATE_STATUS_IN;
        }
    }

    //
    // Make sure we are talking to the correct device.
    //
    USBHostHubAddrSet(USB0_BASE, USB_EP_0,
                      ((pDevice->ucHub << 8) | (pDevice->ucHubPort)),
                      USB_EP_HOST_OUT | (pDevice->bLowSpeed ?
                      USB_EP_SPEED_LOW : USB_EP_SPEED_FULL));

    //
    // Send the Setup packet.
    //
    MAP_USBEndpointDataSend(USB0_BASE, USB_EP_0, USB_TRANS_SETUP);

    //
    // Block until endpoint 0 returns to the IDLE state.
    //
    while(g_sUSBHEP0State.eState != EP0_STATE_IDLE)
    {
        OS_INT_DISABLE(INT_USB0);

        if((g_ulUSBHIntEvents & (INT_EVENT_ENUM | INT_EVENT_SOF)) == (INT_EVENT_ENUM | INT_EVENT_SOF))
        {
            g_ulUSBHIntEvents &= ~(INT_EVENT_ENUM | INT_EVENT_SOF);

            USBHCDEnumHandler();
        }

        OS_INT_ENABLE(INT_USB0);

        if(g_sUSBHEP0State.eState == EP0_STATE_ERROR)
        {
            return(0xffffffff);
        }

        //
        // If we aborted the transfer due to an error, tell the caller
        // that no bytes were transferred.
        //
        if(g_ulUSBHIntEvents & (INT_EVENT_VBUS_ERR | INT_EVENT_DISCONNECT))
        {
            return(0xffffffff);
        }
    }

    //
    // Calculate and return the number of bytes that were sent or received.
    // The extra copy into local variables is required to prevent some
    // compilers from warning about undefined order of volatile access.
    //
    ulDataSize = g_sUSBHEP0State.ulDataSize;
    ulRemaining = g_sUSBHEP0State.ulBytesRemaining;

    return(ulDataSize - ulRemaining);
}

//*****************************************************************************
//
// Starts enumerating a new device connected via the hub.
//
// \param ulIndex is the index of the USB controller to use.
// \param ulHub is the hub address from which the connection is being made.
// \param ulPort is the hub port number that the new device is connected to.
// \param pucConfigPool is memory to be used to store the device's config
//        descriptor.
// \param ulConfigSize is the number of bytes available in the buffer pointed
//        to by pucConfigPool.
//
// This function is called by the hub class driver after it has detected a new
// device connection and reset the device.
//
// \return Returns the index of the device allocated or 0 if no resources are
//         available. Device index 0 is the hub itself.
//
//*****************************************************************************
unsigned long
USBHCDHubDeviceConnected(unsigned long ulIndex, unsigned char ucHub,
                         unsigned char ucPort, tBoolean bLowSpeed,
                         unsigned char *pucConfigPool,
                         unsigned long ulConfigSize)
{
    unsigned long ulDevIndex;

    //
    // Debug sanity checks.
    //
    ASSERT(ulIndex == 0);
    ASSERT(pucConfigPool);
    ASSERT(ulConfigSize);
    ASSERT(ucPort);

    DEBUG_OUTPUT("Connection from hub %d, port %d.\n", ucHub, ucPort);

    //
    // Look for a free slot in the device table.
    //
    for(ulDevIndex = 1; ulDevIndex <= MAX_USB_DEVICES; ulDevIndex++)
    {
        if(g_sUSBHCD.USBDevice[ulDevIndex].pConfigDescriptor == 0)
        {
            //
            // We found one. Set the state to ensure that it gets enumerated.
            //
            g_sUSBHCD.USBDevice[ulDevIndex].pConfigDescriptor =
                                           (tConfigDescriptor *)pucConfigPool;
            g_sUSBHCD.USBDevice[ulDevIndex].ulConfigDescriptorSize =
                                                                ulConfigSize;
            g_sUSBHCD.USBDevice[ulDevIndex].pConfigDescriptor->bLength = 0;
            g_sUSBHCD.USBDevice[ulDevIndex].ucHub = ucHub;
            g_sUSBHCD.USBDevice[ulDevIndex].ucHubPort = ucPort;
            g_sUSBHCD.USBDevice[ulDevIndex].bLowSpeed = bLowSpeed;
            g_sUSBHCD.USBDevice[ulDevIndex].DeviceDescriptor.bLength = 0;

            //
            // Set the state to ensure enumeration begins.
            //
            g_sUSBHCD.eDeviceState[ulDevIndex] = HCD_DEV_CONNECTED;

            DEBUG_OUTPUT("Allocating device %d\n", ulDevIndex);

            //
            // Pass the device index back to the hub driver.
            //
            return(ulDevIndex);
        }
    }

    //
    // If we get here, there are device slots available so send back an invalid
    // device index to tell the caller to ignore this device.
    //
    return(0);
}

//*****************************************************************************
//
// TODO: Documentation
//
//*****************************************************************************
void
USBHCDHubDeviceDisconnected(unsigned long ulIndex, unsigned long ulDevIndex)
{
    //
    // Debug sanity checks.
    //
    ASSERT(ulIndex == 0);
    ASSERT(ulDevIndex && (ulDevIndex <= MAX_USB_DEVICES));

    DEBUG_OUTPUT("Disconnection from hub %d, port %d, device %d\n",
            g_sUSBHCD.USBDevice[ulDevIndex].ucHub,
            g_sUSBHCD.USBDevice[ulDevIndex].ucHubPort, ulDevIndex);

    //
    // Set the device state to ensure that USBHCDMain cleans it up.
    //
    g_sUSBHCD.eDeviceState[ulDevIndex] = HCD_DEV_DISCONNECTED;
}

//*****************************************************************************
//
// This is the endpoint 0 interrupt handler.
//
// \return None.
//
//*****************************************************************************
static void
USBHCDEnumHandler(void)
{
    unsigned long ulEPStatus;
    unsigned long ulDataSize;

    //
    // Get the end point 0 status.
    //
    ulEPStatus = MAP_USBEndpointStatus(USB0_BASE, USB_EP_0);

    //
    // If there was an error then go to the error state.
    //
    if(ulEPStatus == USB_HOST_EP0_ERROR)
    {
        //
        // Clear this status indicating that the status packet was
        // received.
        //
        MAP_USBHostEndpointStatusClear(USB0_BASE, USB_EP_0, USB_HOST_EP0_ERROR);
        MAP_USBFIFOFlush(USB0_BASE, USB_EP_0, 0);

        //
        // Just go back to the idle state.
        //
        g_sUSBHEP0State.eState = EP0_STATE_ERROR;

        return;
    }

    switch(g_sUSBHEP0State.eState)
    {
        //
        // Handle the status state, this is a transitory state from
        // USB_STATE_TX or USB_STATE_RX back to USB_STATE_IDLE.
        //
        case EP0_STATE_STATUS:
        {
            //
            // Handle the case of a received status packet.
            //
            if(ulEPStatus & (USB_HOST_EP0_RXPKTRDY | USB_HOST_EP0_STATUS))
            {
                //
                // Clear this status indicating that the status packet was
                // received.
                //
                MAP_USBHostEndpointStatusClear(USB0_BASE, USB_EP_0,
                                               (USB_HOST_EP0_RXPKTRDY |
                                                USB_HOST_EP0_STATUS));
            }

            //
            // Just go back to the idle state.
            //
            g_sUSBHEP0State.eState = EP0_STATE_IDLE;

            break;
        }

        //
        // This state triggers a STATUS IN request from the device.
        //
        case EP0_STATE_STATUS_IN:
        {
            //
            // Generate an IN request from the device.
            //
            MAP_USBHostRequestStatus(USB0_BASE);

            //
            // Change to the status phase and wait for the response.
            //
            g_sUSBHEP0State.eState =  EP0_STATE_STATUS;

            break;
        }

        //
        // In the IDLE state the code is waiting to receive data from the host.
        //
        case EP0_STATE_IDLE:
        {
            break;
        }

        //
        // Data is still being sent to the host so handle this in the
        // EP0StateTx() function.
        //
        case EP0_STATE_SETUP_OUT:
        {
            //
            // Send remaining data if necessary.
            //
            USBHCDEP0StateTx();

            break;
        }

        //
        // Handle the receive state for commands that are receiving data on
        // endpoint 0.
        //
        case EP0_STATE_SETUP_IN:
        {
            //
            // Generate a new IN request to the device.
            //
            MAP_USBHostRequestIN(USB0_BASE, USB_EP_0);

            //
            // Proceed to the RX state to receive the requested data.
            //
            g_sUSBHEP0State.eState =  EP0_STATE_RX;

            break;
        }

        //
        // The endpoint remains in this state until all requested data has
        // been received.
        //
        case EP0_STATE_RX:
        {
            //
            // There was a stall on endpoint 0 so go back to the idle state
            // as this command has been terminated.
            //
            if(ulEPStatus & USB_HOST_EP0_RX_STALL)
            {
                g_sUSBHEP0State.eState = EP0_STATE_IDLE;

                //
                // Clear the stalled state on endpoint 0.
                //
                MAP_USBHostEndpointStatusClear(USB0_BASE, USB_EP_0, ulEPStatus);
                break;
            }

            //
            // Set the number of bytes to get out of this next packet.
            //
            if(g_sUSBHEP0State.ulBytesRemaining >
               g_sUSBHEP0State.ulMaxPacketSize)
            {
                //
                // Don't send more than EP0_MAX_PACKET_SIZE bytes.
                //
                ulDataSize = MAX_PACKET_SIZE_EP0;
            }
            else
            {
                //
                // There was space so send the remaining bytes.
                //
                ulDataSize = g_sUSBHEP0State.ulBytesRemaining;
            }

            if(ulDataSize != 0)
            {
                //
                // Get the data from the USB controller end point 0.
                //
                MAP_USBEndpointDataGet(USB0_BASE, USB_EP_0,
                                       g_sUSBHEP0State.pData,
                                       &ulDataSize);
            }

            //
            // Advance the pointer.
            //
            g_sUSBHEP0State.pData += ulDataSize;

            //
            // Decrement the number of bytes that are being waited on.
            //
            g_sUSBHEP0State.ulBytesRemaining -= ulDataSize;

            //
            // Need to ack the data on end point 0 in this case
            // without setting data end.
            //
            MAP_USBHostEndpointDataAck(USB0_BASE, USB_EP_0);

            //
            // If there was not more than the maximum packet size bytes of data
            // the this was a short packet and indicates that this transfer is
            // complete.  If there were exactly g_sUSBHEP0State.ulMaxPacketSize
            // remaining then there still needs to be null packet sent before
            // this transfer is complete.
            //
            if((ulDataSize < g_sUSBHEP0State.ulMaxPacketSize) ||
               (g_sUSBHEP0State.ulBytesRemaining == 0))
            {
                //
                // Return to the idle state.
                //
                g_sUSBHEP0State.eState =  EP0_STATE_STATUS;

                //
                // No more data.
                //
                g_sUSBHEP0State.pData = 0;

                //
                // Send a null packet to acknowledge that all data was received.
                //
                MAP_USBEndpointDataSend(USB0_BASE, USB_EP_0, USB_TRANS_STATUS);
            }
            else
            {
                //
                // Request more data.
                //
                MAP_USBHostRequestIN(USB0_BASE, USB_EP_0);
            }
            break;
        }

        //
        // The device stalled endpoint zero so check if the stall needs to be
        // cleared once it has been successfully sent.
        //
        case EP0_STATE_STALL:
        {
            //
            // Reset the global end point 0 state to IDLE.
            //
            g_sUSBHEP0State.eState = EP0_STATE_IDLE;

            break;
        }

        //
        // Halt on an unknown state, but only in DEBUG builds.
        //
        default:
        {
            ASSERT(0);
            break;
        }
    }
}

//*****************************************************************************
//
// This internal function handles sending data on endpoint 0.
//
// \return None.
//
//*****************************************************************************
static void
USBHCDEP0StateTx(void)
{
    unsigned long ulNumBytes;
    unsigned char *pData;

    //
    // In the TX state on endpoint 0.
    //
    g_sUSBHEP0State.eState = EP0_STATE_SETUP_OUT;

    //
    // Set the number of bytes to send this iteration.
    //
    ulNumBytes = g_sUSBHEP0State.ulBytesRemaining;

    //
    // Limit individual transfers to 64 bytes.
    //
    if(ulNumBytes > 64)
    {
        ulNumBytes = 64;
    }

    //
    // Save the pointer so that it can be passed to the USBEndpointDataPut()
    // function.
    //
    pData = (unsigned char *)g_sUSBHEP0State.pData;

    //
    // Advance the data pointer and counter to the next data to be sent.
    //
    g_sUSBHEP0State.ulBytesRemaining -= ulNumBytes;
    g_sUSBHEP0State.pData += ulNumBytes;

    //
    // Put the data in the correct FIFO.
    //
    MAP_USBEndpointDataPut(USB0_BASE, USB_EP_0, pData, ulNumBytes);

    //
    // If this is exactly 64 then don't set the last packet yet.
    //
    if(ulNumBytes == 64)
    {
        //
        // There is more data to send or exactly 64 bytes were sent, this
        // means that there is either more data coming or a null packet needs
        // to be sent to complete the transaction.
        //
        MAP_USBEndpointDataSend(USB0_BASE, USB_EP_0, USB_TRANS_OUT);
    }
    else
    {
        //
        // Send the last bit of data.
        //
        MAP_USBEndpointDataSend(USB0_BASE, USB_EP_0, USB_TRANS_OUT);

        //
        // Now go to the status state and wait for the transmit to complete.
        //
        g_sUSBHEP0State.eState = EP0_STATE_STATUS_IN;
    }
}

//*****************************************************************************
//
//! This function returns the USB hub port for the requested device instance.
//!
//! \param ulInstance is a unique value indicating which device to query.
//!
//! This function returns the USB hub port for the device that is associated
//! with the \e ulInstance parameter.  The caller must use the value for
//! \e ulInstance was passed to the application when it receives a
//! USB_EVENT_CONNECTED event.  The function returns the USB hub port for
//! the interface number specified by the \e ulInterface parameter.
//!
//! \return The USB hub port for the requested interface.
//
//*****************************************************************************
unsigned char
USBHCDDevHubPort(unsigned long ulInstance)
{
    unsigned long ulDevIndex;

    ulDevIndex = HCDInstanceToDevIndex(ulInstance);

    if(ulDevIndex == 0xff)
    {
        return(ulDevIndex);
    }

    return(g_sUSBHCD.USBDevice[ulDevIndex].ucHubPort);
}

//*****************************************************************************
//
//! This function will return the USB address for the requested device
//! instance.
//!
//! \param ulInstance is a unique value indicating which device to query.
//!
//! This function returns the USB address for the device that is associated
//! with the \e ulInstance parameter.  The caller must use a value for
//! \e ulInstance have been passed to the application when it receives a
//! USB_EVENT_CONNECTED event.  The function will return the USB address for
//! the interface number specified by the \e ulInterface parameter.
//!
//! \return The USB address for the requested interface.
//
//*****************************************************************************
unsigned char
USBHCDDevAddress(unsigned long ulInstance)
{
    unsigned long ulDevIndex;

    ulDevIndex = HCDInstanceToDevIndex(ulInstance);

    if(ulDevIndex == 0xff)
    {
        return(ulDevIndex);
    }

    return(g_sUSBHCD.USBDevice[ulDevIndex].ulAddress);
}

//*****************************************************************************
//
//! This function will return the USB class for the requested device
//! instance.
//!
//! \param ulInstance is a unique value indicating which device to query.
//! \param ulInterface is the interface number to query for the USB class.
//!
//! This function returns the USB class for the device that is associated
//! with the \e ulInstance parameter.  The caller must use a value for
//! \e ulInstance have been passed to the application when it receives a
//! USB_EVENT_CONNECTED event.  The function will return the USB class for
//! the interface number specified by the \e ulInterface parameter.  If
//! \e ulInterface is set to 0xFFFFFFFF then the function will return the USB
//! class for the first interface that is found in the device's USB
//! descriptors.
//!
//! \return The USB class for the requested interface.
//
//*****************************************************************************
unsigned char
USBHCDDevClass(unsigned long ulInstance, unsigned long ulInterface)
{
    unsigned long ulDevIndex;
    tInterfaceDescriptor *pInterface;

    ulDevIndex = HCDInstanceToDevIndex(ulInstance);

    //
    // If the instance was not valid return an undefined class.
    //
    if(ulDevIndex == 0xff)
    {
        return(USB_CLASS_DEVICE);
    }

    //
    // Get the interface descriptor.
    //
    pInterface = USBDescGetInterface(
                            g_sUSBHCD.USBDevice[ulDevIndex].pConfigDescriptor,
                            g_sUSBHCD.USBDevice[ulDevIndex].ulInterface,
                            ulInterface);

    //
    // Make sure that the interface requested actually exists.
    //
    if(pInterface)
    {
        //
        // Return the interface class.
        //
        return(pInterface->bInterfaceClass);
    }

    //
    // No valid interface so return an undefined class.
    //
    return(USB_CLASS_DEVICE);
}

//*****************************************************************************
//
//! This function will return the USB subclass for the requested device
//! instance.
//!
//! \param ulInstance is a unique value indicating which device to query.
//! \param ulInterface is the interface number to query for the USB subclass.
//!
//! This function returns the USB subclass for the device that is associated
//! with the \e ulInstance parameter.  The caller must use a value for
//! \e ulInstance have been passed to the application when it receives a
//! USB_EVENT_CONNECTED event.  The function will return the USB subclass for
//! the interface number specified by the \e ulInterface parameter.  If
//! \e ulInterface is set to 0xFFFFFFFF then the function will return the USB
//! subclass for the first interface that is found in the device's USB
//! descriptors.
//!
//! \return The USB subclass for the requested interface.
//
//*****************************************************************************
unsigned char
USBHCDDevSubClass(unsigned long ulInstance, unsigned long ulInterface)
{
    unsigned long ulDevIndex;
    tInterfaceDescriptor *pInterface;

    ulDevIndex = HCDInstanceToDevIndex(ulInstance);

    //
    // If the instance was not valid return an undefined subclass.
    //
    if(ulDevIndex == 0xff)
    {
        return(USB_SUBCLASS_UNDEFINED);
    }

    //
    // Get the interface descriptor.
    //
    pInterface = USBDescGetInterface(
                            g_sUSBHCD.USBDevice[ulDevIndex].pConfigDescriptor,
                            g_sUSBHCD.USBDevice[ulDevIndex].ulInterface,
                            ulInterface);

    //
    // Make sure that the interface requested actually exists.
    //
    if(pInterface)
    {
        //
        // Return the interface subclass.
        //
        return(pInterface->bInterfaceSubClass);
    }

    //
    // No valid interface so return an undefined subclass.
    //
    return(USB_SUBCLASS_UNDEFINED);
}

//*****************************************************************************
//
//! This function will return the USB protocol for the requested device
//! instance.
//!
//! \param ulInstance is a unique value indicating which device to query.
//! \param ulInterface is the interface number to query for the USB protocol.
//!
//! This function returns the USB protocol for the device that is associated
//! with the \e ulInstance parameter.  The caller must use a value for
//! \e ulInstance have been passed to the application when it receives a
//! USB_EVENT_CONNECTED event.  The function will return the USB protocol for
//! the interface number specified by the \e ulInterface parameter.  If
//! \e ulInterface is set to 0xFFFFFFFF then the function will return the USB
//! protocol for the first interface that is found in the device's USB
//! descriptors.
//!
//! \return The USB protocol for the requested interface.
//
//*****************************************************************************
unsigned char
USBHCDDevProtocol(unsigned long ulInstance, unsigned long ulInterface)
{
    unsigned long ulDevIndex;
    tInterfaceDescriptor *pInterface;

    ulDevIndex = HCDInstanceToDevIndex(ulInstance);

    //
    // If the instance was not valid return an undefined protocol.
    //
    if(ulDevIndex == 0xff)
    {
        return(USB_PROTOCOL_UNDEFINED);
    }

    //
    // Get the interface descriptor.
    //
    pInterface = USBDescGetInterface(
                            g_sUSBHCD.USBDevice[ulDevIndex].pConfigDescriptor,
                            g_sUSBHCD.USBDevice[ulDevIndex].ulInterface,
                            ulInterface);

    //
    // Make sure that the interface requested actually exists.
    //
    if(pInterface)
    {
        //
        // Return the interface protocol.
        //
        return(pInterface->bInterfaceProtocol);
    }

    //
    // No valid interface so return an undefined protocol.
    //
    return(USB_PROTOCOL_UNDEFINED);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
