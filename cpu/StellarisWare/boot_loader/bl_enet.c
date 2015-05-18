//*****************************************************************************
//
// bl_enet.c - Functions to update via Ethernet.
//
// Copyright (c) 2007-2012 Texas Instruments Incorporated.  All rights reserved.
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

#include <string.h>
#include "inc/hw_ethernet.h"
#include "inc/hw_flash.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "bl_config.h"
#include "boot_loader/bl_crystal.h"
#include "boot_loader/bl_decrypt.h"
#include "boot_loader/bl_flash.h"
#include "boot_loader/bl_hooks.h"

//*****************************************************************************
//
//! \addtogroup bl_enet_api
//! @{
//
//*****************************************************************************
#if defined(ENET_ENABLE_UPDATE) || defined(DOXYGEN)

//*****************************************************************************
//
// Make sure that the crystal frequency is defined.
//
//*****************************************************************************
#if !defined(CRYSTAL_FREQ)
#error ERROR: CRYSTAL_FREQ must be defined for Ethernet update!
#endif

//*****************************************************************************
//
// Make sure that boot loader update is not enabled (it is not supported via
// BOOTP given that there is no way to distinguish between a normal firmware
// image and a boot loader update image).
//
//*****************************************************************************
#if defined(ENABLE_BL_UPDATE)
#error ERROR: Updating the boot loader is not supported over Ethernet!
#endif

//*****************************************************************************
//
// TFTP packets contain 512 bytes of data and a packet shorter than this
// indicates the end of the transfer.
//
//*****************************************************************************
#define TFTP_BLOCK_SIZE         512

//*****************************************************************************
//
// uIP uses memset, so a simple one is provided here.  This is not as efficient
// as the one in the C library (from an execution time perspective), but it is
// much smaller.
//
//*****************************************************************************
void *
my_memset(void *pvDest, int iChar, size_t lLength)
{
    char *pcBuf = (char *)pvDest;

    //
    // Fill the buffer with the given character.
    //
    while(lLength--)
    {
        *pcBuf++ = iChar;
    }

    //
    // Return a pointer to the beginning of the buffer.
    //
    return(pvDest);
}

//*****************************************************************************
//
// uIP uses memcpy, so a simple one is provided here.  This is not as efficient
// as the one in the C library (from an execution time perspective), but it is
// much smaller.
//
//*****************************************************************************
void *
my_memcpy(void *pvDest, const void *pvSrc, size_t lLength)
{
    const char *pcSrc = (const char *)pvSrc;
    char *pcDest = (char *)pvDest;

    //
    // Copy bytes from the source buffer to the destination buffer.
    //
    while(lLength--)
    {
        *pcDest++ = *pcSrc++;
    }

    //
    // Return a pointer to the beginning of the destination buffer.
    //
    return(pvDest);
}

//*****************************************************************************
//
// Directly include the uIP code if using Ethernet for the update.  This allows
// non-Ethernet boot loader builds to not have to supply the uip-conf.h file
// that would otherwise be required.
//
//*****************************************************************************
#define memcpy my_memcpy
#define memset my_memset
#include "third_party/uip-1.0/uip/pt.h"
#include "third_party/uip-1.0/uip/uip_arp.c"
#undef BUF
#include "third_party/uip-1.0/uip/uip.c"

//*****************************************************************************
//
// A prototype for the function (in the startup code) for a predictable length
// delay.
//
//*****************************************************************************
extern void Delay(unsigned long ulCount);

//*****************************************************************************
//
// Defines for setting up the system clock.
//
//*****************************************************************************
#define SYSTICKHZ               100
#define SYSTICKMS               (1000 / SYSTICKHZ)

//*****************************************************************************
//
// UIP Timers (in ms)
//
//*****************************************************************************
#define UIP_PERIODIC_TIMER_MS   50
#define UIP_ARP_TIMER_MS        10000

//*****************************************************************************
//
// This structure defines the fields in a BOOTP request/reply packet.
//
//*****************************************************************************
typedef struct
{
    //
    // The operation; 1 is a request, 2 is a reply.
    //
    unsigned char ucOp;

    //
    // The hardware type; 1 is Ethernet.
    //
    unsigned char ucHType;

    //
    // The hardware address length; for Ethernet this will be 6, the length of
    // the MAC address.
    //
    unsigned char ucHLen;

    //
    // Hop count, used by gateways for cross-gateway booting.
    //
    unsigned char ucHops;

    //
    // The transaction ID.
    //
    unsigned long ulXID;

    //
    // The number of seconds elapsed since the client started trying to boot.
    //
    unsigned short usSecs;

    //
    // The BOOTP flags.
    //
    unsigned short usFlags;

    //
    // The client's IP address, if it knows it.
    //
    unsigned long ulCIAddr;

    //
    // The client's IP address, as assigned by the BOOTP server.
    //
    unsigned long ulYIAddr;

    //
    // The TFTP server's IP address.
    //
    unsigned long ulSIAddr;

    //
    // The gateway IP address, if booting cross-gateway.
    //
    unsigned long ulGIAddr;

    //
    // The hardware address; for Ethernet this is the MAC address.
    //
    unsigned char pucCHAddr[16];

    //
    // The name, or nickname, of the server that should handle this BOOTP
    // request.
    //
    char pcSName[64];

    //
    // The name of the boot file to be loaded via TFTP.
    //
    char pcFile[128];

    //
    // Optional vendor-specific area; not used for BOOTP.
    //
    unsigned char pucVend[64];
}
tBOOTPPacket;

//*****************************************************************************
//
// The BOOTP commands.
//
//*****************************************************************************
#define BOOTP_REQUEST           1
#define BOOTP_REPLY             2

//*****************************************************************************
//
// The TFTP commands.
//
//*****************************************************************************
#define TFTP_RRQ                1
#define TFTP_WRQ                2
#define TFTP_DATA               3
#define TFTP_ACK                4
#define TFTP_ERROR              5

//*****************************************************************************
//
// The UDP ports used by the BOOTP protocol.
//
//*****************************************************************************
#define BOOTP_SERVER_PORT       67
#define BOOTP_CLIENT_PORT       68

//*****************************************************************************
//
// The UDP port for the TFTP server.
//
//*****************************************************************************
#define TFTP_PORT               69

//*****************************************************************************
//
// The MAC address of the Ethernet interface.
//
//*****************************************************************************
#ifdef ENET_MAC_ADDR0
static struct uip_eth_addr g_sMACAddr =
{
    {
        ENET_MAC_ADDR0,
        ENET_MAC_ADDR1,
        ENET_MAC_ADDR2,
        ENET_MAC_ADDR3,
        ENET_MAC_ADDR4,
        ENET_MAC_ADDR5
    }
};
#else
static struct uip_eth_addr g_sMACAddr;
#endif

//*****************************************************************************
//
// The number of SysTick interrupts since the start of the boot loader.
//
//*****************************************************************************
static unsigned long g_ulTicks;

//*****************************************************************************
//
// The seed for the random number generator.
//
//*****************************************************************************
static unsigned long g_ulRandomSeed;

//*****************************************************************************
//
// The number of milliseconds since the last call to uip_udp_periodic().
//
//*****************************************************************************
static volatile unsigned long g_ulPeriodicTimer;

//*****************************************************************************
//
// The number of milliseconds since the last call to uip_arp_timer().
//
//*****************************************************************************
static volatile unsigned long g_ulARPTimer;

//*****************************************************************************
//
// The transaction ID of the most recently sent out BOOTP request.
//
//*****************************************************************************
static unsigned long g_ulXID;

//*****************************************************************************
//
// The state for the proto-thread that handles the BOOTP process.
//
//*****************************************************************************
static struct pt g_sThread;

//*****************************************************************************
//
// The amount of time to wait for a BOOTP reply before sending out a new BOOTP
// request.
//
//*****************************************************************************
static unsigned long g_ulDelay;

//*****************************************************************************
//
// The target time (relative to g_ulTicks) when the next timeout occurs.
//
//*****************************************************************************
static unsigned long g_ulTarget;

//*****************************************************************************
//
// The IP address of the TFTP server.
//
//*****************************************************************************
static uip_ipaddr_t g_sServerAddr;

//*****************************************************************************
//
// The name of the file to be read from the TFTP server.
//
//*****************************************************************************
static char g_pcFilename[128];

//*****************************************************************************
//
// The end of flash.  If there is not a reserved block at the end of flash,
// this is the real end of flash.  If there is a reserved block, this is the
// start of the reserved block (i.e. the virtual end of flash).
//
//*****************************************************************************
static unsigned long g_ulFlashEnd;

//*****************************************************************************
//
// The current block being read from the TFTP server.
//
//*****************************************************************************
static unsigned long g_ulTFTPBlock;

//*****************************************************************************
//
// The UDP socket used to communicate with the BOOTP and TFTP servers (in
// sequence).
//
//*****************************************************************************
struct uip_udp_conn *g_pConn;

//*****************************************************************************
//
//! Handles the SysTick interrupt.
//!
//! This function is called when the SysTick interrupt occurs.  It simply
//! keeps a running count of interrupts, used as a time basis for the BOOTP and
//! TFTP protocols.
//!
//! \return None.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    //
    // Increment the tick count.
    //
    g_ulTicks++;
    g_ulPeriodicTimer += SYSTICKMS;
    g_ulARPTimer += SYSTICKMS;
}

//*****************************************************************************
//
//! Computes a new random number.
//!
//! This function computes a new pseudo-random number, using a linear
//! congruence random number generator.  Note that if the entire 32-bits of the
//! produced random number are not being used, the upper N bits should be used
//! instead of the lower N bits as they are much more random (for example, use
//! ``RandomNumber() >> 28'' instead of ``RandomNumber() & 15'').
//!
//! \return Returns a 32-bit pseudo-random number.
//
//*****************************************************************************
static unsigned long
RandomNumber(void)
{
    //
    // Generate a new pseudo-random number with a linear congruence random
    // number generator.  This new random number becomes the seed for the next
    // random number.
    //
    g_ulRandomSeed = (g_ulRandomSeed * 1664525) + 1013904223;

    //
    // Return the new random number.
    //
    return(g_ulRandomSeed);
}

//*****************************************************************************
//
//! Reads a packet from the Ethernet controller.
//!
//! This function reads a packet from the Ethernet controller into the uIP
//! packet buffer.  It assumes that a packet is available to be read.
//!
//! \return None.
//
//*****************************************************************************
static void
EnetReadPacket(void)
{
    unsigned long ulTemp;
    long lIdx, lCount;

    //
    // Read the first word from the FIFO.
    //
    ulTemp = HWREG(ETH_BASE + MAC_O_DATA);

    //
    // Extract the size of the remainder of the packet.
    //
    lCount = (ulTemp & 0xffff) - 4;

    //
    // See if the packet will fit into the data buffer.
    //
    if(lCount > (sizeof(uip_buf) + 2))
    {
        //
        // The packet will not fit into the data buffer, so read the rest of
        // the data and throw it away.
        //
        while(lCount > 0)
        {
            HWREG(ETH_BASE + MAC_O_DATA);
            lCount -= 4;
        }

        //
        // There is no packet for uIP to process.
        //
        uip_len = 0;

        //
        // Return without any further processing.
        //
        return;
    }

    //
    // Copy the first two bytes into the packet buffer.
    //
    *(unsigned short *)uip_buf = ulTemp >> 16;

    //
    // Save the packet length for uIP's use.
    //
    uip_len = lCount - 2;

    //
    // Read the remainder of the packet data into uIP's buffer.
    //
    for(lIdx = 2; lIdx < uip_len; lIdx += 4)
    {
        *(unsigned long *)(uip_buf + lIdx) = HWREG(ETH_BASE + MAC_O_DATA);
    }

    //
    // Read the final word from the FIFO, which contains part or all of the
    // frame check sequence (which is ignored).
    //
    HWREG(ETH_BASE + MAC_O_DATA);
}

//*****************************************************************************
//
//! Writes a packet to the Ethernet controller.
//!
//! This function writes a packet from the uIP packet buffer into the Ethernet
//! controller and requests that the packet be transmitted.  It will busy wait
//! until there is space in the transmit FIFO.
//!
//! \return None.
//
//*****************************************************************************
static void
EnetWritePacket(void)
{
    long lIdx, lCount;

    //
    // Wait until there is space in the transmit FIFO.
    //
    while(HWREG(ETH_BASE + MAC_O_TR) & MAC_TR_NEWTX)
    {
    }

    //
    // Get the length of the packet.
    //
    lCount = uip_len;

    //
    // Write the first two bytes of the packet to the FIFO, along with the
    // packet size.
    //
    HWREG(ETH_BASE + MAC_O_DATA) = ((*(unsigned short *)uip_buf << 16) |
                                    (lCount - 14));

    //
    // Write the remaining bytes of the packet to the FIFO.
    //
    for(lIdx = 2; lIdx < lCount; lIdx += 4)
    {
        HWREG(ETH_BASE + MAC_O_DATA) = *(unsigned long *)(uip_buf + lIdx);
    }

    //
    // Send the packet.
    //
    HWREG(ETH_BASE + MAC_O_TR) = MAC_TR_NEWTX;
}

//*****************************************************************************
//
//! Constructs and sends a BOOTP request packet.
//!
//! This function constructs a BOOTP request packet and sends it as a broadcast
//! message to the network.
//!
//! \return None.
//
//*****************************************************************************
static void
SendBOOTPRequest(void)
{
    unsigned char *pucPacket = (unsigned char *)uip_appdata;
    tBOOTPPacket *pBOOTP = (tBOOTPPacket *)uip_appdata;
    unsigned long ulIdx;

    //
    // Zero fill the BOOTP request packet.
    //
    for(ulIdx = 0; ulIdx < sizeof(tBOOTPPacket); ulIdx++)
    {
        pucPacket[ulIdx] = 0;
    }

    //
    // Construct a BOOTP request.
    //
    pBOOTP->ucOp = BOOTP_REQUEST;

    //
    // Set the hardware type to Ethernet.
    //
    pBOOTP->ucHType = 0x01;

    //
    // Set the hardware address length to 6.
    //
    pBOOTP->ucHLen = 0x06;

    //
    // Choose a random number for the transaction ID.
    //
    pBOOTP->ulXID = g_ulXID = RandomNumber();

    //
    // Set the number of seconds since we started.
    //
    pBOOTP->usSecs = HTONS(g_ulTicks / SYSTICKHZ);

    //
    // Fill in the Ethernet MAC address.
    //
    for(ulIdx = 0; ulIdx < 6; ulIdx++)
    {
        pBOOTP->pucCHAddr[ulIdx] = g_sMACAddr.addr[ulIdx];
    }

    //
    // Set the server name if defined.
    //
#ifdef ENET_BOOTP_SERVER
    for(ulIdx = 0; (pBOOTP->pcSName[ulIdx] = ENET_BOOTP_SERVER[ulIdx]) != 0;
        ulIdx++)
    {
    }
#endif

    //
    // Send the BOOTP request packet.
    //
    uip_udp_send(sizeof(tBOOTPPacket));
}

//*****************************************************************************
//
//! Parses a packet checking for a BOOTP reply message.
//!
//! This function parses a packet to determine if it is a BOOTP reply to our
//! currently outstanding BOOTP request.  If a valid reply is found, the
//! appropriate information from the packet is extracted and saved.
//!
//! \return Returns 1 if a valid BOOTP reply message was found and 0 otherwise.
//
//*****************************************************************************
static unsigned long
ParseBOOTPReply(void)
{
    tBOOTPPacket *pBOOTP = (tBOOTPPacket *)uip_appdata;
    unsigned long ulIdx;

    //
    // See if this is a reply for our current BOOTP request.
    //
    if((pBOOTP->ucOp != BOOTP_REPLY) ||
       (pBOOTP->ulXID != g_ulXID) ||
       (*(unsigned long *)pBOOTP->pucCHAddr !=
        *(unsigned long *)g_sMACAddr.addr) ||
       (*(unsigned short *)(pBOOTP->pucCHAddr + 4) !=
        *(unsigned short *)(g_sMACAddr.addr + 4)))
    {
        return(0);
    }

    //
    // Extract our IP address from the response.
    //
    *((unsigned long *)(void *)(&uip_hostaddr)) = pBOOTP->ulYIAddr;

    //
    // Extract the server address from the response.
    //
    *((unsigned long *)(void *)(&g_sServerAddr)) = pBOOTP->ulSIAddr;

    //
    // Save the boot file name.
    //
    for(ulIdx = 0;
        ((g_pcFilename[ulIdx] = pBOOTP->pcFile[ulIdx]) != 0) &&
            (ulIdx < (sizeof(g_pcFilename) - 1));
        ulIdx++)
    {
    }
    g_pcFilename[ulIdx] = 0;

    //
    // A valid BOOTP reply was found and decoded.
    //
    return(1);
}


//*****************************************************************************
//
//! Constructs and sends a TFTP error packet.
//!
//! This function constructs a TFTP read request packet (RRQ) and sends it to
//! the server.
//!
//! \return None.
//
//*****************************************************************************
static void
SendTFTPError(unsigned short usError, char *pcString)
{
    unsigned char *pucPacket = (unsigned char *)uip_appdata;
    long lLen;

    pucPacket[0] = (TFTP_ERROR >> 8) & 0xff;
    pucPacket[1] = TFTP_ERROR & 0xff;
    pucPacket[2] = (usError >> 8) & 0xFF;
    pucPacket[3] = usError & 0xFF;

    //
    // Get ready to copy the error string.
    //
    lLen = 4;
    pucPacket += 4;

    //
    // Copy as much of the string as we can fit.
    //
    while((lLen < (UIP_APPDATA_SIZE - 1)) && *pcString)
    {
        *pucPacket++ = *pcString++;
        lLen++;
    }

    //
    // Write the terminating 0.
    //
    *pucPacket = (unsigned char)0;

    //
    // Send the error packet.
    //
    uip_udp_send(lLen + 1);
}

//*****************************************************************************
//
//! Constructs and sends a TFTP read packet.
//!
//! This function constructs a TFTP read request packet (RRQ) and sends it to
//! the server.
//!
//! \return None.
//
//*****************************************************************************
static void
SendTFTPGet(void)
{
    unsigned char *pucPacket = (unsigned char *)uip_appdata;
    unsigned long ulIdx;
    char *pcFilename;

    //
    // The TFTP RRQ packet should be sent to the TFTP server port.
    //
    g_pConn->rport = HTONS(TFTP_PORT);

    //
    // Set the TFTP packet opcode to RRQ.
    //
    pucPacket[0] = (TFTP_RRQ >> 8) & 0xff;
    pucPacket[1] = TFTP_RRQ & 0xff;

    //
    // Copy the file name into the RRQ packet.
    //
    for(ulIdx = 2, pcFilename = g_pcFilename;
        (pucPacket[ulIdx++] = *pcFilename++) != 0; )
    {
    }

    //
    // Set the transfer mode to binary.
    //
    for(pcFilename = "octet"; (pucPacket[ulIdx++] = *pcFilename++) != 0; )
    {
    }

    //
    // Send the TFTP read packet.
    //
    uip_udp_send(ulIdx);
}

//*****************************************************************************
//
//! Parses a packet checking for a TFTP data packet.
//!
//! This function parses a packet to determine if it is a TFTP data packet for
//! out current TFTP transfer.  If a valid packet is found, the contents of the
//! packet are programmed into flash.
//!
//! \return Returns 1 if this packet was the last packet of the TFTP data
//! transfer and 0 otherwise.
//
//*****************************************************************************
static unsigned long
ParseTFTPData(void)
{
    unsigned char *pucPacket = (unsigned char *)uip_appdata;
    unsigned long ulFlashAddr;
    unsigned long ulIdx;

    //
    // See if this is a TFTP data packet.
    //
    if((pucPacket[0] != ((TFTP_DATA >> 8) && 0xff)) ||
       (pucPacket[1] != (TFTP_DATA & 0xff)))
    {
        return(0);
    }

    //
    // If the remote port on our connection is still the TFTP server port (i.e.
    // this is the first data packet), then copy the transaction ID for the
    // TFTP data connection into our connection.  This will ensure that our
    // response will be sent to the correct port.
    //
    if(g_pConn->rport == HTONS(TFTP_PORT))
    {
        g_pConn->rport =
            ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])->srcport;
    }

    //
    // See if this is the correct data packet.
    //
    if((pucPacket[2] != ((g_ulTFTPBlock >> 8) & 0xff)) ||
       (pucPacket[3] != (g_ulTFTPBlock & 0xff)))
    {
        //
        // Since the wrong data packet was sent, resend the ACK for it since
        // we've already processed it.
        //
        pucPacket[0] = (TFTP_ACK >> 8) & 0xff;
        pucPacket[1] = TFTP_ACK & 0xff;
        uip_udp_send(4);

        //
        // Ignore this packet.
        //
        return(0);
    }

    //
    // What address are we about to program to?
    //
    ulFlashAddr = ((g_ulTFTPBlock - 1) * TFTP_BLOCK_SIZE) + APP_START_ADDRESS;

    //
    // Do not program this data into flash if it is beyond the end of flash.
    //
    if(ulFlashAddr < g_ulFlashEnd)
    {
        //
        // If this is the first block and we have been provided with a start
        // hook function, call it here to indicate that we are about to begin
        // flashing a new image.
        //
#ifdef BL_START_FN_HOOK
        if(g_ulTFTPBlock == 1)
        {
            BL_START_FN_HOOK();
        }
#endif

        //
        // Clear any flash error indicator.
        //
        BL_FLASH_CL_ERR_FN_HOOK();

        //
        // If this is the first data packet and code protection is enabled,
        // then erase the entire flash.
        //
#ifdef FLASH_CODE_PROTECTION
        if(g_ulTFTPBlock == 1)
        {
            //
            // Loop through the pages in the flash, excluding the pages that
            // contain the boot loader and the optional reserved space.
            //
            for(ulIdx = APP_START_ADDRESS; ulIdx < g_ulFlashEnd;
                ulIdx += FLASH_PAGE_SIZE)
            {
                //
                // Erase this block of the flash.
                //
                BL_FLASH_ERASE_FN_HOOK((ulIdx);
            }
        }
#else
        //
        // Flash code protection is not enabled, so see if the data in this
        // packet will be programmed to the beginning of a flash block.  We
        // assume that the flash block size is always a multiple of 1KB so,
        // since each TFTP packet is 512 bytes and that the start must always
        // be on a flash page boundary, we can be sure that we will hit the
        // start of each page as we receive packets.
        //
        if(!(ulFlashAddr & (FLASH_PAGE_SIZE - 1)))
        {
            //
            // Erase this block of the flash.
            //
            BL_FLASH_ERASE_FN_HOOK(ulFlashAddr);
        }
#endif

        //
        // Decrypt the data if required.
        //
#ifdef BL_DECRYPT_FN_HOOK
        BL_DECRYPT_FN_HOOK(pucPacket + 4, uip_len - 4);
#endif

        //
        // Program this block of data into flash.
        //
        BL_FLASH_PROGRAM_FN_HOOK(ulFlashAddr, (pucPacket + 4), (uip_len - 4));

        //
        // If a progress reporting hook function has been provided, call it
        // here.  The TFTP protocol doesn't let us know how large the image is
        // before it starts the transfer so we pass 0 as the ulTotal parameter
        // to indicate this.
        //
#ifdef BL_PROGRESS_FN_HOOK
        BL_PROGRESS_FN_HOOK(((ulFlashAddr - APP_START_ADDRESS) + (uip_len - 4)),
                            0);
#endif
    }

    //
    // Increment to the next block.
    //
    g_ulTFTPBlock++;

    //
    // Save the packet length.
    //
    ulIdx = uip_len;

    //
    // Did we see any error?
    //
    if(BL_FLASH_ERROR_FN_HOOK())
    {
        //
        // Yes - send back an error packet.
        //
        SendTFTPError(2, "Error programming flash.");
    }
    else
    {
        //
        // No errors reported so construct an ACK packet.  The block number
        // field is already correct, so it does not need to be set.
        //
        pucPacket[0] = (TFTP_ACK >> 8) & 0xff;
        pucPacket[1] = TFTP_ACK & 0xff;

        //
        // Send the ACK packet to the TFTP server.
        //
        uip_udp_send(4);
    }

    //
    // If the packet was shorter than TFTP_BLOCK_SIZE bytes then this was the
    // last packet in the file.
    //
    if(ulIdx != (TFTP_BLOCK_SIZE + 4))
    {
        //
        // If an end signal hook function has been provided, call it here.
        //
#ifdef BL_END_FN_HOOK
        BL_END_FN_HOOK();
#endif
        return(1);
    }

    //
    // There is more data to be read.
    //
    return(0);
}

//*****************************************************************************
//
//! Handles the BOOTP process.
//!
//! This function contains the proto-thread for handling the BOOTP process.  It
//! first communicates with the BOOTP server to get its boot parameters (IP
//! address, server address, and file name), then it communicates with the TFTP
//! server on the specified server to read the firmware image file.
//!
//! \return None.
//
//*****************************************************************************
#ifdef DOXYGEN
char
BOOTPThread(void)
#else
PT_THREAD(BOOTPThread(void))
#endif
{
    //
    // Begin the proto-thread.
    //
    PT_BEGIN(&g_sThread);

    //
    // Set the initial delay between BOOTP requests to 1 second.
    //
    g_ulDelay = SYSTICKHZ;

    //
    // Loop forever.  This loop is explicitly exited when a valid BOOTP reply
    // is received.
    //
    while(1)
    {
        //
        // Send a BOOTP request.
        //
        SendBOOTPRequest();

        //
        // Set the amount of time to wait for the BOOTP reply message.
        //
        g_ulTarget = g_ulTicks + g_ulDelay;

        //
        // Wait until a packet is received or the timeout has occurred.
        //
        PT_WAIT_UNTIL(&g_sThread, uip_newdata() || (g_ulTicks > g_ulTarget));

        //
        // See if a packet has been received.
        //
        if(uip_newdata())
        {
            //
            // Clear the new data flag so that this packet will only be
            // examined one time.
            //
            uip_flags &= ~(UIP_NEWDATA);

            //
            // See if this is a BOOTP reply.
            //
            if(ParseBOOTPReply() == 1)
            {
                break;
            }
        }

        //
        // If the delay between BOOTP requests is less than 60 seconds, double
        // the delay time.  This avoids constantly slamming the network with
        // requests.
        //
        if(g_ulDelay < (60 * SYSTICKHZ))
        {
            g_ulDelay *= 2;
        }
    }

    //
    // Reconfigure the UDP socket to target the TFTP port on the server.
    //
    uip_ipaddr_copy(&g_pConn->ripaddr, g_sServerAddr);
    uip_udp_bind(g_pConn, HTONS(13633));

    //
    // Send a TFTP read request.
    //
    SendTFTPGet();

    //
    // Since the first TFTP read request will result in an ARP request, delay
    // for just a bit and then re-issue the TFTP read request.
    //
    PT_YIELD(&g_sThread);

    //
    // Resend the TFTP read request.  If the ARP request has already been
    // answered, this will go out as is and avoid the two second timeout below.
    //
    SendTFTPGet();

    //
    // Start the TFTP transfer from block one.
    //
    g_ulTFTPBlock = 1;

    //
    // Loop forever.  This loop is explicitly exited when the TFTP transfer has
    // completed.
    //
    while(1)
    {
        //
        // Set the amount of time to wait for the TFTP data packet.
        //
        g_ulTarget = g_ulTicks + (SYSTICKHZ * 4);

        //
        // Wait until a packet is received or the timeout has occurred.
        //
        PT_WAIT_UNTIL(&g_sThread, uip_newdata() || (g_ulTicks > g_ulTarget));

        //
        // See if a packet has been received.
        //
        if(uip_newdata())
        {
            //
            // Clear the new data flag so that this packet will only be
            // examined one time.
            //
            uip_flags &= ~(UIP_NEWDATA);

            //
            // See if this is a TFTP data packet.
            //
            if(ParseTFTPData() == 1)
            {
                break;
            }
        }
        else
        {
            //
            // The transfer timed out, so send a new TFTP read request.
            //
            SendTFTPGet();

            //
            // Start the TFTP transfer from block one.
            //
            g_ulTFTPBlock = 1;
        }
    }

    //
    // Wait for a bit to make sure that the final ACK packet is transmitted.
    //
    g_ulTarget = g_ulTicks + (SYSTICKHZ / 4);
    while(g_ulTicks < g_ulTarget)
    {
        PT_YIELD(&g_sThread);
    }

    //
    // Perform a software reset request.  This will cause the microcontroller
    // to reset; no further code will be executed.
    //
    // Use the reset function in the flash patch if appropriate.
    //
#ifdef FLASH_PATCH_COMPATIBLE
    ((void (*)(void))0x801)();
#else
    HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
#endif

    //
    // The microcontroller should have reset, so this should never be reached.
    // Just in case, loop forever.
    //
    while(1)
    {
    }

    //
    // End the proto-thread.
    //
    PT_END(&g_sThread);
}

//*****************************************************************************
//
//! Configures the Ethernet controller.
//!
//! This function configures the Ethernet controller, preparing it for use by
//! the boot loader.
//!
//! \return None.
//
//*****************************************************************************
void
ConfigureEnet(void)
{
    //
    // Turn the LDO voltage up to 2.75V if configured to do so.  This is a
    // workaround for errata in revision A2 of Fury-class devices that allows
    // the PLL to operate reliably.
    //
#ifdef BOOST_LDO_VOLTAGE
    HWREG(SYSCTL_LDOPCTL) = SYSCTL_LDOPCTL_2_75V;
#endif

    //
    // Enable the main oscillator.
    //
    HWREG(SYSCTL_RCC) &= ~(SYSCTL_RCC_MOSCDIS);

    //
    // Delay while the main oscillator starts up.
    //
    Delay(524288);

    //
    // Set the crystal frequency, switch to the main oscillator, and enable the
    // PLL.
    //
    HWREG(SYSCTL_RCC) = ((HWREG(SYSCTL_RCC) &
                          ~(SYSCTL_RCC_PWRDN | SYSCTL_RCC_XTAL_M |
                            SYSCTL_RCC_OSCSRC_M)) |
                         XTAL_VALUE | SYSCTL_RCC_OSCSRC_MAIN);

    //
    // Delay while the PLL locks.
    //
    Delay(524288);

    //
    // Disable the PLL bypass so that the part is clocked from the PLL, and set
    // sysdiv to 16.  This yields a system clock of 12.5MHz.
    //
    HWREG(SYSCTL_RCC) = ((HWREG(SYSCTL_RCC) &
                          ~(SYSCTL_RCC_BYPASS | SYSCTL_RCC_SYSDIV_M)) |
                         SYSCTL_RCC_SYSDIV_16 | SYSCTL_RCC_USESYSDIV);

    //
    // Enable the Ethernet controller, along with the LEDs if requested.
    //
#ifdef ENET_ENABLE_LEDS
    HWREG(SYSCTL_RCGC2) |= (SYSCTL_RCGC2_EPHY0 | SYSCTL_RCGC2_EMAC0 |
                           SYSCTL_RCGC2_GPIOF);
#else
    HWREG(SYSCTL_RCGC2) |= SYSCTL_RCGC2_EPHY0 | SYSCTL_RCGC2_EMAC0;
#endif
    Delay(1);

    //
    // Enable the Ethernet LED outputs if requested.
    //
#ifdef ENET_ENABLE_LEDS
    HWREG(GPIO_PORTF_BASE + GPIO_O_DEN) |= (1 << 2) | (1 << 3);
    HWREG(GPIO_PORTF_BASE + GPIO_O_AFSEL) |= (1 << 2) | (1 << 3);
#endif

    //
    // Set the Ethernet management clock divider based on the 12.5MHz system
    // clock.
    //
    HWREG(ETH_BASE + MAC_O_MDV) = (200000000 / 16) / 2 / 2500000;

    //
    // Setup the Ethernet transmit and receive control registers.
    //
    HWREG(ETH_BASE + MAC_O_TCTL) = (MAC_TCTL_DUPLEX | MAC_TCTL_CRC |
                                    MAC_TCTL_PADEN | MAC_TCTL_TXEN);
    HWREG(ETH_BASE + MAC_O_RCTL) = (MAC_RCTL_RSTFIFO | MAC_RCTL_AMUL |
                                    MAC_RCTL_RXEN);
}

//*****************************************************************************
//
//! Starts the update process via BOOTP.
//!
//! This function starts the Ethernet firmware update process.  The BOOTP
//! (as defined by RFC951 at http://tools.ietf.org/html/rfc951) and TFTP (as
//! defined by RFC1350 at http://tools.ietf.org/html/rfc1350) protocols are
//! used to transfer the firmware image over Ethernet.
//!
//! \return Never returns.
//
//*****************************************************************************
void
UpdateBOOTP(void)
{
    unsigned long ulTemp;
    uip_ipaddr_t sAddr;

    //
    // Set sysdiv to 16.
    //
    HWREG(SYSCTL_RCC) |= SYSCTL_RCC_SYSDIV_16;

    //
    // Set the flash programming based on the 12.5 MHz system clock.
    //
    HWREG(FLASH_USECRL) = 12;

    //
    // Get the size of flash.
    //
    g_ulFlashEnd = BL_FLASH_END_FN_HOOK();
#ifdef FLASH_RSVD_SPACE
    g_ulFlashEnd -= FLASH_RSVD_SPACE;
#endif

    //
    // Setup SysTick.
    //
    HWREG(NVIC_ST_RELOAD) = ((200000000 / 16) / SYSTICKHZ) - 1;
    HWREG(NVIC_ST_CTRL) = (NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_INTEN |
                           NVIC_ST_CTRL_ENABLE);

    //
    // If a static MAC address is not being used, then get the MAC address from
    // the flash user registers.
    //
#ifndef ENET_MAC_ADDR0
    ulTemp = HWREG(FLASH_USERREG0);
    g_sMACAddr.addr[0] = ulTemp & 0xff;
    g_sMACAddr.addr[1] = (ulTemp >> 8) & 0xff;
    g_sMACAddr.addr[2] = (ulTemp >> 16) & 0xff;
    ulTemp = HWREG(FLASH_USERREG1);
    g_sMACAddr.addr[3] = ulTemp & 0xff;
    g_sMACAddr.addr[4] = (ulTemp >> 8) & 0xff;
    g_sMACAddr.addr[5] = (ulTemp >> 16) & 0xff;
#endif

    //
    // Seed the random number generator from the MAC address.
    //
    g_ulRandomSeed = *(unsigned long *)(g_sMACAddr.addr + 2);

    //
    // Initialize the uIP stack.
    //
    uip_init();
    uip_arp_init();

    //
    // Program the MAC address.
    //
    HWREG(ETH_BASE + MAC_O_IA0) = *((unsigned long *)g_sMACAddr.addr);
    HWREG(ETH_BASE + MAC_O_IA1) = *((unsigned short *)(g_sMACAddr.addr + 4));
    uip_setethaddr(g_sMACAddr);

    //
    // Initialize the proto-thread used by the BOOTP protocol handler.
    //
    PT_INIT(&g_sThread);

    //
    // Create a UDP socket for sending requests to the BOOTP server.  After the
    // BOOTP portion of the protocol has been handled, this socket will be
    // reused to communicate with the TFTP server.
    //
    *((unsigned long *)(void *)(&sAddr)) = 0xffffffff;
    g_pConn = uip_udp_new(&sAddr, HTONS(BOOTP_SERVER_PORT));
    uip_udp_bind(g_pConn, HTONS(BOOTP_CLIENT_PORT));

    //
    // Reset the counters that are incremented by SysTick.
    //
    g_ulTicks = 0;
    g_ulPeriodicTimer = 0;
    g_ulARPTimer = 0;

    //
    // Main Application Loop.
    //
    while(1)
    {
        //
        // See if there is a packet waiting to be read.
        //
        if((HWREG(ETH_BASE + MAC_O_NP) & MAC_NP_NPR_M) != 0)
        {
            //
            // Read the packet from the Ethernet controller.
            //
            EnetReadPacket();

            //
            // See if this is an IP packet.
            //
            if((uip_len != 0) &&
               (((struct uip_eth_hdr *)&uip_buf[0])->type ==
                HTONS(UIP_ETHTYPE_IP)))
            {
                //
                // Update the ARP tables based on this packet.
                //
                uip_arp_ipin();

                //
                // Process this packet.
                //
                uip_input();

                //
                // See if the processing of this packet resulted in a packet to
                // be sent.
                //
                if(uip_len > 0)
                {
                    //
                    // Update the ARP tables based on the packet to be sent.
                    //
                    uip_arp_out();

                    //
                    // Send the packet.
                    //
                    EnetWritePacket();

                    //
                    // Indicate that the packet has been sent.
                    //
                    uip_len = 0;
                }
            }

            //
            // See if this is an ARP packet.
            //
            else if((uip_len != 0) &&
                    (((struct uip_eth_hdr *)&uip_buf[0])->type ==
                     HTONS(UIP_ETHTYPE_ARP)))
            {
                //
                // Process this packet.
                //
                uip_arp_arpin();

                //
                // See if the processing of this packet resulted in a packet to
                // be sent.
                //
                if(uip_len > 0)
                {
                    //
                    // Send the packet.
                    //
                    EnetWritePacket();

                    //
                    // Indicate that the packet has been sent.
                    //
                    uip_len = 0;
                }
            }
        }

        //
        // See if the periodic timer has expired.
        //
        if(g_ulPeriodicTimer > UIP_PERIODIC_TIMER_MS)
        {
            //
            // Reset the periodic timer.
            //
            g_ulPeriodicTimer = 0;

            //
            // Loop through the UDP connections.
            //
            for(ulTemp = 0; ulTemp < UIP_UDP_CONNS; ulTemp++)
            {
                //
                // Perform the periodic processing on this UDP connection.
                //
                uip_udp_periodic(ulTemp);

                //
                // See if the periodic processing of this connection resulted
                // in a packet to be sent.
                //
                if(uip_len > 0)
                {
                    //
                    // Update the ARP tables based on the packet to be sent.
                    //
                    uip_arp_out();

                    //
                    // Send the packet.
                    //
                    EnetWritePacket();

                    //
                    // Indicate that the packet has been sent.
                    //
                    uip_len = 0;
                }
            }
        }

        //
        // See if the ARP timer has expired.
        //
        if(g_ulARPTimer > UIP_ARP_TIMER_MS)
        {
            //
            // Reset the ARP timer.
            //
            g_ulARPTimer = 0;

            //
            // Perform periodic processing on the ARP table.
            //
            uip_arp_timer();
        }
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
#endif
