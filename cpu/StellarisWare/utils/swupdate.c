//*****************************************************************************
//
// swupdate.c - A module wrapping the Ethernet bootloader software update
//              functionality.
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
// This is part of revision 9453 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/flash.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/lwiplib.h"
#include "utils/swupdate.h"

//*****************************************************************************
//
//! \addtogroup swupdate_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// The UDP port used to send the remote firmware update request signal.  This
// is the well-known port associated with "discard" function and is also used
// by some Wake-On-LAN implementations.
//
//*****************************************************************************
#define MPACKET_PORT             9

//*****************************************************************************
//
// The length of the various parts of the remote firmware update request magic
// packet and its total length.  This contains a 6 byte header followed by 4
// copies of the target MAC address.
//
//*****************************************************************************
#define MPACKET_HEADER_LEN 6
#define MPACKET_MAC_REP    4
#define MPACKET_MAC_LEN    6
#define MPACKET_LEN (MPACKET_HEADER_LEN + (MPACKET_MAC_REP * MPACKET_MAC_LEN))

//*****************************************************************************
//
// The marker byte used at the start of the magic packet.  This is repeated
// MPACKET_HEADER_LEN times.
//
//*****************************************************************************
#define MPACKET_MARKER     0xAA

//*****************************************************************************
//
// The callback function which is used to determine whether or not the
// application wants to allow a remotely-requested firmware update.
//
//*****************************************************************************
tSoftwareUpdateRequested g_pfnUpdateCallback = NULL;

//*****************************************************************************
//
// A pointer to the remote firmware update signal PCB data structure.
//
//*****************************************************************************
static struct udp_pcb *g_pMagicPacketPCB = NULL;

//*****************************************************************************
//
// The MAC address for this board.
//
//*****************************************************************************
static unsigned char g_pucMACAddr[6];

//*****************************************************************************
//
// Receives a UDP port 9 packet from lwIP.
//
// \param arg is not used in this implementation.
// \param pcb is the pointer to the UDB control structure.
// \param p is the pointer to the PBUF structure containing the packet data.
// \param addr is the source (remote) IP address for this packet.
// \param port is the source (remote) port for this packet.
//
// This function is called when the lwIP TCP/IP stack has an incoming
// UDP packet to be processed on the remote firmware update signal port.
//
// \return None.
//
//*****************************************************************************
static void
SoftwareUpdateUDPReceive(void *arg, struct udp_pcb *pcb, struct pbuf *p,
              struct ip_addr *addr, u16_t port)
{
    char *pcData = p->payload;
    unsigned long ulLoop, ulMACLoop;

    //
    // Check that the packet length is what we expect.  If not, ignore the
    // packet.
    //
    if(p->len == MPACKET_LEN)
    {
        //
        // The length matches so now look for the 6 byte header
        //
        for(ulLoop = 0; ulLoop < MPACKET_HEADER_LEN; ulLoop++)
        {
            //
            // Does this header byte match the expected marker?
            //
            if(pcData[ulLoop] != MPACKET_MARKER)
            {
                //
                // No - free the buffer and return - this is not a packet
                // we are interested in.
                //
                pbuf_free(p);
                return;
            }
        }
    }

    //
    // If we get here, the packet length and header markers indicate
    // that this is a remote firmware update request.  Now check that it
    // is for us and that it contains the required number of copies of
    // the MAC address.
    //

    //
    // Skip the header.
    //
    pcData += MPACKET_HEADER_LEN;

    //
    // Loop through each of the expected MAC address copies.
    //
    for(ulLoop = 0; ulLoop < MPACKET_MAC_REP; ulLoop++)
    {
        //
        // Loop through each byte of the MAC address in this
        // copy.
        //
        for(ulMACLoop = 0; ulMACLoop < MPACKET_MAC_LEN; ulMACLoop++)
        {
            //
            // Does the payload MAC address byte match what we expect?
            //
            if(*pcData != g_pucMACAddr[ulMACLoop])
            {
                //
                // No match - free the packet and return.
                //
                pbuf_free(p);
                return;
            }
            else
            {
                //
                // Byte matched so move on to the next one.
                //
                pcData++;
            }
        }
    }

    //
    // Free the pbuf since we are finished with it now.
    //
    pbuf_free(p);

    //
    // If we get this far, we've received a valid remote firmare update
    // request targetted at this board.  Signal this to the application
    // if we have a valid callback pointer.
    //
    if(g_pfnUpdateCallback)
    {
        g_pfnUpdateCallback();
    }
}

//*****************************************************************************
//
//! Initializes the remote Ethernet software update notification feature.
//!
//! \param pfnCallback is a pointer to a function which will be called whenever
//! a remote firmware update request is received.  If the application wishes
//! to allow the update to go ahead, it must call SoftwareUpdateBegin() from
//! non-interrupt context after the callback is received.  Note that the
//! callback will most likely be made in interrupt context so it is not safe
//! to call SoftwareUpdateBegin() from within the callback itself.
//!
//! This function may be used on Ethernet-enabled parts to support
//! remotely-signaled firmware updates over Ethernet.  The LM Flash Programmer
//! (LMFlash.exe) application sends a magic packet to UDP port 9 whenever the
//! user requests an Ethernet-based firmware update.  This packet consists of
//! 6 bytes of 0xAA followed by the target MAC address repeated 4 times.
//! This function starts listening on UDP port 9 and, if a magic packet
//! matching the MAC address of this board is received, makes a call to the
//! provided callback function to indicate that an update has been requested.
//!
//! The callback function provided here will typically be called in the context
//! of the lwIP Ethernet interrupt handler.  It is not safe to call
//! SoftwareUpdateBegin() in this context so the application should use the
//! callback to signal code running in a non-interrupt context to perform the
//! update if it is to be allowed.
//!
//! UDP port 9 is chosen for this function since this is the well-known port
//! associated with ``discard'' operation.  In other words, any other system
//! receiving the magic packet will simply ignore it.  The actual magic packet
//! used is modeled on Wake-On-LAN which uses a similar structure (6 bytes of
//! 0xFF followed by 16 repetitions of the target MAC address).  Some
//! Wake-On-LAN implementations also use UDP port 9 for their signaling.
//!
//! \note Applications using this function must initialize the lwIP stack prior
//! to making this call and must ensure that the lwIPTimer() function is called
//! periodically.  lwIP UDP must be enabled in lwipopts.h to ensure that the
//! magic packets can be received.
//!
//! \return None.
//
//*****************************************************************************
void
SoftwareUpdateInit(tSoftwareUpdateRequested pfnCallback)
{
    unsigned long ulUser0, ulUser1;

    //
    // Remember the callback function pointer we have been given.
    //
    g_pfnUpdateCallback = pfnCallback;

    //
    // Get the MAC address from the user registers in NV ram.
    //
    FlashUserGet(&ulUser0, &ulUser1);

    //
    // Convert the 24/24 split MAC address from NV ram into a MAC address
    // array.
    //
    g_pucMACAddr[0] = ulUser0 & 0xff;
    g_pucMACAddr[1] = (ulUser0 >> 8) & 0xff;
    g_pucMACAddr[2] = (ulUser0 >> 16) & 0xff;
    g_pucMACAddr[3] = ulUser1 & 0xff;
    g_pucMACAddr[4] = (ulUser1 >> 8) & 0xff;
    g_pucMACAddr[5] = (ulUser1 >> 16) & 0xff;

    //
    // Set up a UDP PCB to allow us to receive the magic packets sent from
    // LMFlash. These may be sent to port 9 from any port on the source
    // machine so we do not call udp_connect here (since this causes lwIP to
    // filter any packet that did not originate from port 9 too).
    //
    g_pMagicPacketPCB = udp_new();
    udp_recv(g_pMagicPacketPCB, SoftwareUpdateUDPReceive, NULL);
    udp_bind(g_pMagicPacketPCB, IP_ADDR_ANY, MPACKET_PORT);
}

//*****************************************************************************
//
//! Passes control to the bootloader and initiates a remote software update
//! over Ethernet.
//!
//! This function passes control to the bootloader and initiates an update of
//! the main application firmware image via BOOTP across Ethernet.  This
//! function may only be used on parts supporting Ethernet and in cases where
//! the Ethernet boot loader is in use alongside the main application image.
//! It must not be called in interrupt context.
//!
//! Applications wishing to make use of this function must be built to
//! operate with the bootloader.  If this function is called on a system
//! which does not include the bootloader, the results are unpredictable.
//!
//! \note It is not safe to call this function from within the callback
//! provided on the initial call to SoftwareUpdateInit().  The application
//! must use the callback to signal a pending update (assuming the update is to
//! be permitted) to some other code running in a non-interrupt context.
//!
//! \return Never returns.
//
//*****************************************************************************
void
SoftwareUpdateBegin(void)
{
    //
    // Disable all processor interrupts.  Instead of disabling them
    // one at a time (and possibly missing an interrupt if new sources
    // are added), a direct write to NVIC is done to disable all
    // peripheral interrupts.
    //
    HWREG(NVIC_DIS0) = 0xffffffff;
    HWREG(NVIC_DIS1) = 0xffffffff;

    //
    // Also disable the SysTick interrupt.
    //
    SysTickIntDisable();

    //
    // Return control to the boot loader.  This is a call to the SVC
    // handler in the boot loader, or to the ROM if available.
    //
#if ((defined ROM_UpdateEthernet) && !(defined USE_FLASH_BOOT_LOADER))
    ROM_UpdateEthernet(ROM_SysCtlClockGet());
#else
    (*((void (*)(void))(*(unsigned long *)0x2c)))();
#endif
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
