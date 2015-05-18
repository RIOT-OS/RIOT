//*****************************************************************************
//
// locator.c - A device locator server using UDP in lwIP.
//
// Copyright (c) 2009-2012 Texas Instruments Incorporated.  All rights reserved.
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

#include "utils/locator.h"
#include "utils/lwiplib.h"

//*****************************************************************************
//
//! \addtogroup locator_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// These defines are used to describe the device locator protocol.
//
//*****************************************************************************
#define TAG_CMD                 0xff
#define TAG_STATUS              0xfe
#define CMD_DISCOVER_TARGET     0x02

//*****************************************************************************
//
// An array that contains the device locator response data.  The format of the
// data is as follows:
//
//     Byte        Description
//     --------    ------------------------
//      0          TAG_STATUS
//      1          packet length
//      2          CMD_DISCOVER_TARGET
//      3          board type
//      4          board ID
//      5..8       client IP address
//      9..14      MAC address
//     15..18      firmware version
//     19..82      application title
//     83          checksum
//
//*****************************************************************************
static unsigned char g_pucLocatorData[84];

//*****************************************************************************
//
// This function is called by the lwIP TCP/IP stack when it receives a UDP
// packet from the discovery port.  It produces the response packet, which is
// sent back to the querying client.
//
//*****************************************************************************
static void
LocatorReceive(void *arg, struct udp_pcb *pcb, struct pbuf *p,
               struct ip_addr *addr, u16_t port)
{
    unsigned char *pucData;
    unsigned long ulIdx;

    //
    // Validate the contents of the datagram.
    //
    pucData = p->payload;
    if((p->len != 4) || (pucData[0] != TAG_CMD) || (pucData[1] != 4) ||
       (pucData[2] != CMD_DISCOVER_TARGET) ||
       (pucData[3] != ((0 - TAG_CMD - 4 - CMD_DISCOVER_TARGET) & 0xff)))
    {
        pbuf_free(p);
        return;
    }

    //
    // The incoming pbuf is no longer needed, so free it.
    //
    pbuf_free(p);

    //
    // Allocate a new pbuf for sending the response.
    //
    p = pbuf_alloc(PBUF_TRANSPORT, sizeof(g_pucLocatorData), PBUF_RAM);
    if(p == NULL)
    {
        return;
    }

    //
    // Calcuate and fill in the checksum on the response packet.
    //
    for(ulIdx = 0, g_pucLocatorData[sizeof(g_pucLocatorData) - 1] = 0;
        ulIdx < (sizeof(g_pucLocatorData) - 1); ulIdx++)
    {
        g_pucLocatorData[sizeof(g_pucLocatorData) - 1] -=
            g_pucLocatorData[ulIdx];
    }

    //
    // Copy the response packet data into the pbuf.
    //
    pucData = p->payload;
    for(ulIdx = 0; ulIdx < sizeof(g_pucLocatorData); ulIdx++)
    {
        pucData[ulIdx] = g_pucLocatorData[ulIdx];
    }

    //
    // Send the response.
    //
    udp_sendto(pcb, p, addr, port);

    //
    // Free the pbuf.
    //
    pbuf_free(p);
}

//*****************************************************************************
//
//! Initializes the locator service.
//!
//! This function prepares the locator service to handle device discovery
//! requests.  A UDP server is created and the locator response data is
//! initialized to all empty.
//!
//! \return None.
//
//*****************************************************************************
void
LocatorInit(void)
{
    unsigned long ulIdx;
    void *pcb;

    //
    // Clear out the response data.
    //
    for(ulIdx = 0; ulIdx < 84; ulIdx++)
    {
        g_pucLocatorData[ulIdx] = 0;
    }

    //
    // Fill in the header for the response data.
    //
    g_pucLocatorData[0] = TAG_STATUS;
    g_pucLocatorData[1] = sizeof(g_pucLocatorData);
    g_pucLocatorData[2] = CMD_DISCOVER_TARGET;

    //
    // Fill in the MAC address for the response data.
    //
    g_pucLocatorData[9] = 0;
    g_pucLocatorData[10] = 0;
    g_pucLocatorData[11] = 0;
    g_pucLocatorData[12] = 0;
    g_pucLocatorData[13] = 0;
    g_pucLocatorData[14] = 0;

    //
    // Create a new UDP port for listening to device locator requests.
    //
    pcb = udp_new();
    udp_recv(pcb, LocatorReceive, NULL);
    udp_bind(pcb, IP_ADDR_ANY, 23);
}

//*****************************************************************************
//
//! Sets the board type in the locator response packet.
//!
//! \param ulType is the type of the board.
//!
//! This function sets the board type field in the locator response packet.
//!
//! \return None.
//
//*****************************************************************************
void
LocatorBoardTypeSet(unsigned long ulType)
{
    //
    // Save the board type in the response data.
    //
    g_pucLocatorData[3] = ulType & 0xff;
}

//*****************************************************************************
//
//! Sets the board ID in the locator response packet.
//!
//! \param ulID is the ID of the board.
//!
//! This function sets the board ID field in the locator response packet.
//!
//! \return None.
//
//*****************************************************************************
void
LocatorBoardIDSet(unsigned long ulID)
{
    //
    // Save the board ID in the response data.
    //
    g_pucLocatorData[4] = ulID & 0xff;
}

//*****************************************************************************
//
//! Sets the client IP address in the locator response packet.
//!
//! \param ulIP is the IP address of the currently connected client.
//!
//! This function sets the IP address of the currently connected client in the
//! locator response packet.  The IP should be set to 0.0.0.0 if there is no
//! client connected.  It should never be set for devices that do not have a
//! strict one-to-one mapping of client to server (for example, a web server).
//!
//! \return None.
//
//*****************************************************************************
void
LocatorClientIPSet(unsigned long ulIP)
{
    //
    // Save the client IP address in the response data.
    //
    g_pucLocatorData[5] = ulIP & 0xff;
    g_pucLocatorData[6] = (ulIP >> 8) & 0xff;
    g_pucLocatorData[7] = (ulIP >> 16) & 0xff;
    g_pucLocatorData[8] = (ulIP >> 24) & 0xff;
}

//*****************************************************************************
//
//! Sets the MAC address in the locator response packet.
//!
//! \param pucMACArray is the MAC address of the network interface.
//!
//! This function sets the MAC address of the network interface in the locator
//! response packet.
//!
//! \return None.
//
//*****************************************************************************
void
LocatorMACAddrSet(unsigned char *pucMACArray)
{
    //
    // Save the MAC address.
    //
    g_pucLocatorData[9] = pucMACArray[0];
    g_pucLocatorData[10] = pucMACArray[1];
    g_pucLocatorData[11] = pucMACArray[2];
    g_pucLocatorData[12] = pucMACArray[3];
    g_pucLocatorData[13] = pucMACArray[4];
    g_pucLocatorData[14] = pucMACArray[5];
}

//*****************************************************************************
//
//! Sets the firmware version in the locator response packet.
//!
//! \param ulVersion is the version number of the device firmware.
//!
//! This function sets the version number of the device firmware in the locator
//! response packet.
//!
//! \return None.
//
//*****************************************************************************
void
LocatorVersionSet(unsigned long ulVersion)
{
    //
    // Save the firmware version number in the response data.
    //
    g_pucLocatorData[15] = ulVersion & 0xff;
    g_pucLocatorData[16] = (ulVersion >> 8) & 0xff;
    g_pucLocatorData[17] = (ulVersion >> 16) & 0xff;
    g_pucLocatorData[18] = (ulVersion >> 24) & 0xff;
}

//*****************************************************************************
//
//! Sets the application title in the locator response packet.
//!
//! \param pcAppTitle is a pointer to the application title string.
//!
//! This function sets the application title in the locator response packet.
//! The string is truncated at 64 characters if it is longer (without a
//! terminating 0), and is zero-filled to 64 characters if it is shorter.
//!
//! \return None.
//
//*****************************************************************************
void
LocatorAppTitleSet(const char *pcAppTitle)
{
    unsigned long ulCount;

    //
    // Copy the application title string into the response data.
    //
    for(ulCount = 0; (ulCount < 64) && *pcAppTitle; ulCount++)
    {
        g_pucLocatorData[ulCount + 19] = *pcAppTitle++;
    }

    //
    // Zero-fill the remainder of the space in the response data (if any).
    //
    for(; ulCount < 64; ulCount++)
    {
        g_pucLocatorData[ulCount + 19] = 0;
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
