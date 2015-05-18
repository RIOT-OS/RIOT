//*****************************************************************************
//
// lwiplib.c - lwIP TCP/IP Library Abstraction Layer.
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

//*****************************************************************************
//
// Ensure that the lwIP Compile Time Options are included first.
//
//*****************************************************************************
#include "utils/lwiplib.h"

//*****************************************************************************
//
// Include lwIP high-level API code.
//
//*****************************************************************************
#include "third_party/lwip-1.3.2/src/api/api_lib.c"
#include "third_party/lwip-1.3.2/src/api/api_msg.c"
#include "third_party/lwip-1.3.2/src/api/err.c"
#include "third_party/lwip-1.3.2/src/api/netbuf.c"
#include "third_party/lwip-1.3.2/src/api/netdb.c"
#include "third_party/lwip-1.3.2/src/api/netifapi.c"
#include "third_party/lwip-1.3.2/src/api/sockets.c"
#include "third_party/lwip-1.3.2/src/api/tcpip.c"

//*****************************************************************************
//
// Include the core lwIP TCP/IP stack code.
//
//*****************************************************************************
#include "third_party/lwip-1.3.2/src/core/dhcp.c"
#include "third_party/lwip-1.3.2/src/core/dns.c"
#include "third_party/lwip-1.3.2/src/core/init.c"
#include "third_party/lwip-1.3.2/src/core/mem.c"
#include "third_party/lwip-1.3.2/src/core/memp.c"
#include "third_party/lwip-1.3.2/src/core/netif.c"
#include "third_party/lwip-1.3.2/src/core/pbuf.c"
#include "third_party/lwip-1.3.2/src/core/raw.c"
#include "third_party/lwip-1.3.2/src/core/stats.c"
#include "third_party/lwip-1.3.2/src/core/sys.c"
#include "third_party/lwip-1.3.2/src/core/tcp.c"
#include "third_party/lwip-1.3.2/src/core/tcp_in.c"
#include "third_party/lwip-1.3.2/src/core/tcp_out.c"
#include "third_party/lwip-1.3.2/src/core/udp.c"

//*****************************************************************************
//
// Include the IPV4 code.
//
//*****************************************************************************
#include "third_party/lwip-1.3.2/src/core/ipv4/autoip.c"
#include "third_party/lwip-1.3.2/src/core/ipv4/icmp.c"
#include "third_party/lwip-1.3.2/src/core/ipv4/igmp.c"
#include "third_party/lwip-1.3.2/src/core/ipv4/inet.c"
#include "third_party/lwip-1.3.2/src/core/ipv4/inet_chksum.c"
#include "third_party/lwip-1.3.2/src/core/ipv4/ip.c"
#include "third_party/lwip-1.3.2/src/core/ipv4/ip_addr.c"
#include "third_party/lwip-1.3.2/src/core/ipv4/ip_frag.c"

//*****************************************************************************
//
// Include the lwIP SNMP code.
//
//*****************************************************************************
#include "third_party/lwip-1.3.2/src/core/snmp/asn1_dec.c"
#include "third_party/lwip-1.3.2/src/core/snmp/asn1_enc.c"
#include "third_party/lwip-1.3.2/src/core/snmp/mib2.c"
#include "third_party/lwip-1.3.2/src/core/snmp/mib_structs.c"
#include "third_party/lwip-1.3.2/src/core/snmp/msg_in.c"
#include "third_party/lwip-1.3.2/src/core/snmp/msg_out.c"

//*****************************************************************************
//
// Include the Network Interface code.
//
//*****************************************************************************
#include "third_party/lwip-1.3.2/src/netif/etharp.c"
#include "third_party/lwip-1.3.2/src/netif/loopif.c"

//*****************************************************************************
//
// Include the Network Interface PPP code.
//
//*****************************************************************************
#include "third_party/lwip-1.3.2/src/netif/ppp/auth.c"
#include "third_party/lwip-1.3.2/src/netif/ppp/chap.c"
#include "third_party/lwip-1.3.2/src/netif/ppp/chpms.c"
#include "third_party/lwip-1.3.2/src/netif/ppp/fsm.c"
#include "third_party/lwip-1.3.2/src/netif/ppp/ipcp.c"
#include "third_party/lwip-1.3.2/src/netif/ppp/lcp.c"
#include "third_party/lwip-1.3.2/src/netif/ppp/magic.c"
#include "third_party/lwip-1.3.2/src/netif/ppp/md5.c"
#include "third_party/lwip-1.3.2/src/netif/ppp/pap.c"
#include "third_party/lwip-1.3.2/src/netif/ppp/ppp.c"
#include "third_party/lwip-1.3.2/src/netif/ppp/ppp_oe.c"
#include "third_party/lwip-1.3.2/src/netif/ppp/randm.c"
#include "third_party/lwip-1.3.2/src/netif/ppp/vj.c"

//*****************************************************************************
//
// Include Stellaris-specific lwIP interface/porting layer code.
//
//*****************************************************************************
#include "third_party/lwip-1.3.2/ports/stellaris/perf.c"
#include "third_party/lwip-1.3.2/ports/stellaris/sys_arch.c"
#include "third_party/lwip-1.3.2/ports/stellaris/netif/stellarisif.c"

//*****************************************************************************
//
//! \addtogroup lwiplib_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// The lwIP Library abstration layer provides for a host callback function
// to be called periodically in the lwIP context.  This is the timer
// interval, in ms, for this periodic callback.  If the timer interval is
// defined to 0 (the default value), then no periodic host callback is
// performed.
//
//*****************************************************************************
#ifndef HOST_TMR_INTERVAL
#define HOST_TMR_INTERVAL       0
#else
extern void lwIPHostTimerHandler(void);
#endif

//*****************************************************************************
//
// The number of milliseconds between calls to the soft-MDIX function when
// using a RTOS.
//
//*****************************************************************************
#define SOFT_MDIX_INTERVAL      10

//*****************************************************************************
//
// Driverlib headers needed for this library module.
//
//*****************************************************************************
#include "inc/hw_ints.h"
#include "inc/hw_ethernet.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/ethernet.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#if !NO_SYS
#if RTOS_SAFERTOS
#include "SafeRTOS/SafeRTOS_API.h"
#elif RTOS_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#endif
#if ((RTOS_SAFERTOS + RTOS_FREERTOS) < 1)
    #error No RTOS is defined. Please define an RTOS.
#endif
#if ((RTOS_SAFERTOS + RTOS_FREERTOS) > 1)
    #error Two RTOS defined. Please define only one RTOS at a time.
#endif
#endif

//*****************************************************************************
//
// The lwIP network interface structure for the Stellaris Ethernet MAC.
//
//*****************************************************************************
static struct netif g_sNetIF;

//*****************************************************************************
//
// The local time for the lwIP Library Abstraction layer, used to support the
// Host and lwIP periodic callback functions.
//
//*****************************************************************************
#if NO_SYS
static unsigned long g_ulLocalTimer = 0;
#endif

//*****************************************************************************
//
// The local time when the soft MDI/MDIX switch was last switched.
//
//*****************************************************************************
static unsigned long g_ulMDIXTimer = 0;

//*****************************************************************************
//
// The local time when the TCP timer was last serviced.
//
//*****************************************************************************
#if NO_SYS
static unsigned long g_ulTCPTimer = 0;
#endif

//*****************************************************************************
//
// The local time when the HOST timer was last serviced.
//
//*****************************************************************************
#if NO_SYS && HOST_TMR_INTERVAL
static unsigned long g_ulHostTimer = 0;
#endif

//*****************************************************************************
//
// The local time when the ARP timer was last serviced.
//
//*****************************************************************************
#if NO_SYS && LWIP_ARP
static unsigned long g_ulARPTimer = 0;
#endif

//*****************************************************************************
//
// The local time when the AutoIP timer was last serviced.
//
//*****************************************************************************
#if NO_SYS && LWIP_AUTOIP
static unsigned long g_ulAutoIPTimer = 0;
#endif

//*****************************************************************************
//
// The local time when the DHCP Coarse timer was last serviced.
//
//*****************************************************************************
#if NO_SYS && LWIP_DHCP
static unsigned long g_ulDHCPCoarseTimer = 0;
#endif

//*****************************************************************************
//
// The local time when the DHCP Fine timer was last serviced.
//
//*****************************************************************************
#if NO_SYS && LWIP_DHCP
static unsigned long g_ulDHCPFineTimer = 0;
#endif

//*****************************************************************************
//
// The local time when the IP Reassembly timer was last serviced.
//
//*****************************************************************************
#if NO_SYS && IP_REASSEMBLY
static unsigned long g_ulIPReassemblyTimer = 0;
#endif

//*****************************************************************************
//
// The local time when the IGMP timer was last serviced.
//
//*****************************************************************************
#if NO_SYS && LWIP_IGMP
static unsigned long g_ulIGMPTimer = 0;
#endif

//*****************************************************************************
//
// The local time when the DNS timer was last serviced.
//
//*****************************************************************************
#if NO_SYS && LWIP_DNS
static unsigned long g_ulDNSTimer = 0;
#endif

//*****************************************************************************
//
// The default IP address aquisition mode.
//
//*****************************************************************************
static unsigned long g_ulIPMode = IPADDR_USE_STATIC;

//*****************************************************************************
//
// The IP address to be used.  This is used during the initialization of the
// stack and when the interface configuration is changed.
//
//*****************************************************************************
static unsigned long g_ulIPAddr;

//*****************************************************************************
//
// The netmask to be used.  This is used during the initialization of the stack
// and when the interface configuration is changed.
//
//*****************************************************************************
static unsigned long g_ulNetMask;

//*****************************************************************************
//
// The gateway address to be used.  This is used during the initialization of
// the stack and when the interface configuration is changed.
//
//*****************************************************************************
static unsigned long g_ulGWAddr;

//*****************************************************************************
//
// The stack to used for the interrupt task.
//
//*****************************************************************************
#if !NO_SYS
static unsigned long g_pulStack[128];
#endif

//*****************************************************************************
//
// Memory for the "queue" used to signal the interrupt task from the interrupt
// handler.  This is a single entry in size and is really just a semaphore.
//
//*****************************************************************************
#if !NO_SYS
static signed char g_pcQueueMem[sizeof(void *) + portQUEUE_OVERHEAD_BYTES];
#endif

//*****************************************************************************
//
// The handle for the "queue" (semaphore) used to signal the interrupt task
// from the interrupt handler.
//
//*****************************************************************************
#if !NO_SYS
static xQueueHandle g_pInterrupt;
#endif

//*****************************************************************************
//
// This task handles reading packets from the Ethernet controller and supplying
// them to the TCP/IP thread.
//
//*****************************************************************************
#if !NO_SYS
static void
lwIPInterruptTask(void *pvArg)
{
    //
    // Loop forever.
    //
    while(1)
    {
        //
        // Wait until the semaphore has been signalled.
        //
        while(xQueueReceive(g_pInterrupt, &pvArg, portMAX_DELAY) != pdPASS)
        {
        }

        //
        // Processes any packets waiting to be sent or received.
        //
        stellarisif_interrupt(&g_sNetIF);

        //
        // Re-enable the Ethernet interrupts.
        //
        EthernetIntEnable(ETH_BASE, ETH_INT_RX | ETH_INT_TX);
    }
}
#endif

//*****************************************************************************
//
// This function services all of the lwIP periodic timers, including TCP and
// Host timers.  This should be called from the lwIP context, which may be
// the Ethernet interrupt (in the case of a non-RTOS system) or the lwIP
// thread, in the event that an RTOS is used.
//
//*****************************************************************************
#if NO_SYS
static void
lwIPServiceTimers(void)
{
    //
    // Service the MDIX timer.
    //
    if((EthernetPHYRead(ETH_BASE, PHY_MR1) & PHY_MR1_LINK) == 0)
    {
        //
        // See if there has not been a link for 2 seconds.
        //
        if((g_ulLocalTimer - g_ulMDIXTimer) >= 2000)
        {
            //
            // There has not been a link for 2 seconds, so flip the MDI/MDIX
            // switch.  This is handled automatically by Fury rev A2, but is
            // harmless.
            //
            HWREG(ETH_BASE + MAC_O_MDIX) ^= MAC_MDIX_EN;

            //
            // Reset the MDIX timer.
            //
            g_ulMDIXTimer = g_ulLocalTimer;
        }
    }
    else
    {
        //
        // There is a link, so reset the MDIX timer.
        //
        g_ulMDIXTimer = g_ulLocalTimer;
    }

    //
    // Service the host timer.
    //
#if HOST_TMR_INTERVAL
    if((g_ulLocalTimer - g_ulHostTimer) >= HOST_TMR_INTERVAL)
    {
        g_ulHostTimer = g_ulLocalTimer;
        lwIPHostTimerHandler();
    }
#endif

    //
    // Service the ARP timer.
    //
#if LWIP_ARP
    if((g_ulLocalTimer - g_ulARPTimer) >= ARP_TMR_INTERVAL)
    {
        g_ulARPTimer = g_ulLocalTimer;
        etharp_tmr();
    }
#endif

    //
    // Service the TCP timer.
    //
#if LWIP_TCP
    if((g_ulLocalTimer - g_ulTCPTimer) >= TCP_TMR_INTERVAL)
    {
        g_ulTCPTimer = g_ulLocalTimer;
        tcp_tmr();
    }
#endif

    //
    // Service the AutoIP timer.
    //
#if LWIP_AUTOIP
    if((g_ulLocalTimer - g_ulAutoIPTimer) >= AUTOIP_TMR_INTERVAL)
    {
        g_ulAutoIPTimer = g_ulLocalTimer;
        autoip_tmr();
    }
#endif

    //
    // Service the DCHP Coarse Timer.
    //
#if LWIP_DHCP
    if((g_ulLocalTimer - g_ulDHCPCoarseTimer) >= DHCP_COARSE_TIMER_MSECS)
    {
        g_ulDHCPCoarseTimer = g_ulLocalTimer;
        dhcp_coarse_tmr();
    }
#endif

    //
    // Service the DCHP Fine Timer.
    //
#if LWIP_DHCP
    if((g_ulLocalTimer - g_ulDHCPFineTimer) >= DHCP_FINE_TIMER_MSECS)
    {
        g_ulDHCPFineTimer = g_ulLocalTimer;
        dhcp_fine_tmr();
    }
#endif

    //
    // Service the IP Reassembly Timer
    //
#if IP_REASSEMBLY
    if((g_ulLocalTimer - g_ulIPReassemblyTimer) >= IP_TMR_INTERVAL)
    {
        g_ulIPReassemblyTimer = g_ulLocalTimer;
        ip_reass_tmr();
    }
#endif

    //
    // Service the IGMP Timer
    //
#if LWIP_IGMP
    if((g_ulLocalTimer - g_ulIGMPTimer) >= IGMP_TMR_INTERVAL)
    {
        g_ulIGMPTimer = g_ulLocalTimer;
        igmp_tmr();
    }
#endif

    //
    // Service the DNS Timer
    //
#if LWIP_DNS
    if((g_ulLocalTimer - g_ulDNSTimer) >= DNS_TMR_INTERVAL)
    {
        g_ulDNSTimer = g_ulLocalTimer;
        dns_tmr();
    }
#endif
}
#endif

//*****************************************************************************
//
// Handles the timeout for the soft-MDIX timer when using a RTOS.
//
//*****************************************************************************
#if !NO_SYS
static void
lwIPSoftMDIXTimer(void *pvArg)
{
    //
    // Service the MDIX timer.
    //
    if((EthernetPHYRead(ETH_BASE, PHY_MR1) & PHY_MR1_LINK) == 0)
    {
        g_ulMDIXTimer += SOFT_MDIX_INTERVAL;

        //
        // See if there has not been a link for 2 seconds.
        //
        if(g_ulMDIXTimer >= 2000)
        {
            //
            // There has not been a link for 2 seconds, so flip the MDI/MDIX
            // switch.  This is handled automatically by Fury rev A2, but is
            // harmless.
            //
            HWREG(ETH_BASE + MAC_O_MDIX) ^= MAC_MDIX_EN;

            //
            // Reset the MDIX timer.
            //
            g_ulMDIXTimer = 0;
        }
    }
    else
    {
        //
        // There is a link, so reset the MDIX timer.
        //
        g_ulMDIXTimer = 0;
    }

    //
    // Re-schedule the soft-MDIX timer callback function timeout.
    //
    sys_timeout(SOFT_MDIX_INTERVAL, lwIPSoftMDIXTimer, NULL);
}
#endif

//*****************************************************************************
//
// Handles the timeout for the host callback function timer when using a RTOS.
//
//*****************************************************************************
#if !NO_SYS && HOST_TMR_INTERVAL
static void
lwIPPrivateHostTimer(void *pvArg)
{
    //
    // Call the application-supplied host timer callback function.
    //
    lwIPHostTimerHandler();

    //
    // Re-schedule the host timer callback function timeout.
    //
    sys_timeout(HOST_TMR_INTERVAL, lwIPPrivateHostTimer, NULL);
}
#endif

//*****************************************************************************
//
// Completes the initialization of lwIP.  This is directly called when not
// using a RTOS and provided as a callback to the TCP/IP thread when using a
// RTOS.
//
//*****************************************************************************
static void
lwIPPrivateInit(void *pvArg)
{
    struct ip_addr ip_addr;
    struct ip_addr net_mask;
    struct ip_addr gw_addr;

    //
    // If not using a RTOS, initialize the lwIP stack.
    //
#if NO_SYS
    lwip_init();
#endif

    //
    // If using a RTOS, create a queue (to be used as a semaphore) to signal
    // the Ethernet interrupt task from the Ethernet interrupt handler.
    //
#if !NO_SYS
#if RTOS_SAFERTOS
    xQueueCreate(g_pcQueueMem, sizeof(g_pcQueueMem), 1, sizeof(void *),
                 &g_pInterrupt);
#elif RTOS_FREERTOS
    g_pInterrupt = xQueueCreate(1,sizeof(void *));
#endif
#endif

    //
    // If using a RTOS, create the Ethernet interrupt task.
    //
#if !NO_SYS
#if RTOS_SAFERTOS
    xTaskCreate(lwIPInterruptTask, (signed portCHAR *)"eth_int",
                (signed portCHAR *)g_pulStack, sizeof(g_pulStack), 0, 1, 0);
#elif RTOS_FREERTOS
    xTaskCreate(lwIPInterruptTask, (signed portCHAR *)"eth_int",
                 sizeof(g_pulStack)/sizeof(int), 0, tskIDLE_PRIORITY+1, 0);
#endif
#endif

    //
    // Setup the network address values.
    //
    if(g_ulIPMode == IPADDR_USE_STATIC)
    {
        ip_addr.addr = htonl(g_ulIPAddr);
        net_mask.addr = htonl(g_ulNetMask);
        gw_addr.addr = htonl(g_ulGWAddr);
    }
    else
    {
        ip_addr.addr = 0;
        net_mask.addr = 0;
        gw_addr.addr = 0;
    }

    //
    // Create, configure and add the Ethernet controller interface with
    // default settings.  ip_input should be used to send packets directly to
    // the stack when not using a RTOS and tcpip_input should be used to send
    // packets to the TCP/IP thread's queue when using a RTOS.
    //
#if NO_SYS
    netif_add(&g_sNetIF, &ip_addr, &net_mask, &gw_addr, NULL, stellarisif_init,
              ip_input);
#else
    netif_add(&g_sNetIF, &ip_addr, &net_mask, &gw_addr, NULL, stellarisif_init,
              tcpip_input);
#endif
    netif_set_default(&g_sNetIF);

    //
    // Start DHCP, if enabled.
    //
#if LWIP_DHCP
    if(g_ulIPMode == IPADDR_USE_DHCP)
    {
        dhcp_start(&g_sNetIF);
    }
#endif

    //
    // Start AutoIP, if enabled and DHCP is not.
    //
#if LWIP_AUTOIP
    if(g_ulIPMode == IPADDR_USE_AUTOIP)
    {
        autoip_start(&g_sNetIF);
    }
#endif

    //
    // Bring the interface up.
    //
    netif_set_up(&g_sNetIF);

    //
    // Setup a timeout for the host timer callback function if using a RTOS.
    //
#if !NO_SYS && HOST_TMR_INTERVAL
    sys_timeout(HOST_TMR_INTERVAL, lwIPPrivateHostTimer, NULL);
#endif

    //
    // If not running on a Fury-class device, then MDIX is handled in software.
    // In this case, when using a RTOS, setup a timeout for the soft-MDIX
    // handler.
    //
#if !NO_SYS
    if(!CLASS_IS_FURY)
    {
        sys_timeout(SOFT_MDIX_INTERVAL, lwIPSoftMDIXTimer, NULL);
    }
#endif
}

//*****************************************************************************
//
//! Initializes the lwIP TCP/IP stack.
//!
//! \param pucMAC is a pointer to a six byte array containing the MAC
//! address to be used for the interface.
//! \param ulIPAddr is the IP address to be used (static).
//! \param ulNetMask is the network mask to be used (static).
//! \param ulGWAddr is the Gateway address to be used (static).
//! \param ulIPMode is the IP Address Mode.  \b IPADDR_USE_STATIC will force
//! static IP addressing to be used, \b IPADDR_USE_DHCP will force DHCP with
//! fallback to Link Local (Auto IP), while \b IPADDR_USE_AUTOIP will force
//! Link Local only.
//!
//! This function performs initialization of the lwIP TCP/IP stack for the
//! Stellaris Ethernet MAC, including DHCP and/or AutoIP, as configured.
//!
//! \return None.
//
//*****************************************************************************
void
lwIPInit(const unsigned char *pucMAC, unsigned long ulIPAddr,
         unsigned long ulNetMask, unsigned long ulGWAddr,
         unsigned long ulIPMode)
{
    //
    // Check the parameters.
    //
#if LWIP_DHCP && LWIP_AUTOIP
    ASSERT((ulIPMode == IPADDR_USE_STATIC) ||
           (ulIPMode == IPADDR_USE_DHCP) ||
           (ulIPMode == IPADDR_USE_AUTOIP));
#elif LWIP_DHCP
    ASSERT((ulIPMode == IPADDR_USE_STATIC) ||
           (ulIPMode == IPADDR_USE_DHCP));
#elif LWIP_AUTOIP
    ASSERT((ulIPMode == IPADDR_USE_STATIC) ||
           (ulIPMode == IPADDR_USE_AUTOIP));
#else
    ASSERT(ulIPMode == IPADDR_USE_STATIC);
#endif

    //
    // Enable the ethernet peripheral.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);

    //
    // Program the MAC address into the Ethernet controller.
    //
    EthernetMACAddrSet(ETH_BASE, (unsigned char *)pucMAC);

    //
    // Save the network configuration for later use by the private
    // initialization.
    //
    g_ulIPMode = ulIPMode;
    g_ulIPAddr = ulIPAddr;
    g_ulNetMask = ulNetMask;
    g_ulGWAddr = ulGWAddr;

    //
    // Initialize lwIP.  The remainder of initialization is done immediately if
    // not using a RTOS and it is deferred to the TCP/IP thread's context if
    // using a RTOS.
    //
#if NO_SYS
    lwIPPrivateInit(0);
#else
    tcpip_init(lwIPPrivateInit, 0);
#endif
}

//*****************************************************************************
//
//! Handles periodic timer events for the lwIP TCP/IP stack.
//!
//! \param ulTimeMS is the incremental time for this periodic interrupt.
//!
//! This function will update the local timer by the value in \e ulTimeMS.
//! If the system is configured for use without an RTOS, an Ethernet interrupt
//! will be triggered to allow the lwIP periodic timers to be serviced in the
//! Ethernet interrupt.
//!
//! \return None.
//
//*****************************************************************************
#if NO_SYS
void
lwIPTimer(unsigned long ulTimeMS)
{
    //
    // Increment the lwIP Ethernet timer.
    //
    g_ulLocalTimer += ulTimeMS;

    //
    // Generate an Ethernet interrupt.  This will perform the actual work
    // of checking the lwIP timers and taking the appropriate actions.  This is
    // needed since lwIP is not re-entrant, and this allows all lwIP calls to
    // be placed inside the Ethernet interrupt handler ensuring that all calls
    // into lwIP are coming from the same context, preventing any reentrancy
    // issues.  Putting all the lwIP calls in the Ethernet interrupt handler
    // avoids the use of mutexes to avoid re-entering lwIP.
    //
    HWREG(NVIC_SW_TRIG) |= INT_ETH - 16;
}
#endif

//*****************************************************************************
//
//! Handles Ethernet interrupts for the lwIP TCP/IP stack.
//!
//! This function handles Ethernet interrupts for the lwIP TCP/IP stack.  At
//! the lowest level, all receive packets are placed into a packet queue for
//! processing at a higher level.  Also, the transmit packet queue is checked
//! and packets are drained and transmitted through the Ethernet MAC as needed.
//! If the system is configured without an RTOS, additional processing is
//! performed at the interrupt level.  The packet queues are processed by the
//! lwIP TCP/IP code, and lwIP periodic timers are serviced (as needed).
//!
//! \return None.
//
//*****************************************************************************
void
lwIPEthernetIntHandler(void)
{
    unsigned long ulStatus;
#if !NO_SYS
    portBASE_TYPE xWake;
#endif

    //
    // Read and Clear the interrupt.
    //
    ulStatus = EthernetIntStatus(ETH_BASE, false);
    EthernetIntClear(ETH_BASE, ulStatus);

    //
    // The handling of the interrupt is different based on the use of a RTOS.
    //
#if NO_SYS
    //
    // No RTOS is being used.  If a transmit/receive interrupt was active,
    // run the low-level interrupt handler.
    //
    if(ulStatus)
    {
        stellarisif_interrupt(&g_sNetIF);
    }

    //
    // Service the lwIP timers.
    //
    lwIPServiceTimers();
#else
    //
    // A RTOS is being used.  Signal the Ethernet interrupt task.
    //
    xQueueSendFromISR(g_pInterrupt, (void *)&ulStatus, &xWake);

    //
    // Disable the Ethernet interrupts.  Since the interrupts have not been
    // handled, they are not asserted.  Once they are handled by the Ethernet
    // interrupt task, it will re-enable the interrupts.
    //
    EthernetIntDisable(ETH_BASE, ETH_INT_RX | ETH_INT_TX);

    //
    // Potentially task switch as a result of the above queue write.
    //
#if RTOS_SAFERTOS
    taskYIELD_FROM_ISR(xWake);
#elif RTOS_FREERTOS
    if(xWake == pdTRUE)
    {
        vPortYieldFromISR();
    }
#endif
#endif
}

//*****************************************************************************
//
//! Returns the IP address for this interface.
//!
//! This function will read and return the currently assigned IP address for
//! the Stellaris Ethernet interface.
//!
//! \return Returns the assigned IP address for this interface.
//
//*****************************************************************************
unsigned long
lwIPLocalIPAddrGet(void)
{
    return((unsigned long)g_sNetIF.ip_addr.addr);
}

//*****************************************************************************
//
//! Returns the network mask for this interface.
//!
//! This function will read and return the currently assigned network mask for
//! the Stellaris Ethernet interface.
//!
//! \return the assigned network mask for this interface.
//
//*****************************************************************************
unsigned long
lwIPLocalNetMaskGet(void)
{
    return((unsigned long)g_sNetIF.netmask.addr);
}

//*****************************************************************************
//
//! Returns the gateway address for this interface.
//!
//! This function will read and return the currently assigned gateway address
//! for the Stellaris Ethernet interface.
//!
//! \return the assigned gateway address for this interface.
//
//*****************************************************************************
unsigned long
lwIPLocalGWAddrGet(void)
{
    return((unsigned long)g_sNetIF.gw.addr);
}

//*****************************************************************************
//
//! Returns the local MAC/HW address for this interface.
//!
//! \param pucMAC is a pointer to an array of bytes used to store the MAC
//! address.
//!
//! This function will read the currently assigned MAC address into the array
//! passed in \e pucMAC.
//!
//! \return None.
//
//*****************************************************************************
void
lwIPLocalMACGet(unsigned char *pucMAC)
{
    EthernetMACAddrGet(ETH_BASE, pucMAC);
}

//*****************************************************************************
//
// Completes the network configuration change.  This is directly called when
// not using a RTOS and provided as a callback to the TCP/IP thread when using
// a RTOS.
//
//*****************************************************************************
static void
lwIPPrivateNetworkConfigChange(void *pvArg)
{
    unsigned long ulIPMode;
    struct ip_addr ip_addr;
    struct ip_addr net_mask;
    struct ip_addr gw_addr;

    //
    // Get the new address mode.
    //
    ulIPMode = (unsigned long)pvArg;

    //
    // Setup the network address values.
    //
    if(ulIPMode == IPADDR_USE_STATIC)
    {
        ip_addr.addr = htonl(g_ulIPAddr);
        net_mask.addr = htonl(g_ulNetMask);
        gw_addr.addr = htonl(g_ulGWAddr);
    }
#if LWIP_DHCP || LWIP_AUTOIP
    else
    {
        ip_addr.addr = 0;
        net_mask.addr = 0;
        gw_addr.addr = 0;
    }
#endif

    //
    // Switch on the current IP Address Aquisition mode.
    //
    switch(g_ulIPMode)
    {
        //
        // Static IP
        //
        case IPADDR_USE_STATIC:
        {
            //
            // Set the new address parameters.  This will change the address
            // configuration in lwIP, and if necessary, will reset any links
            // that are active.  This is valid for all three modes.
            //
            netif_set_addr(&g_sNetIF, &ip_addr, &net_mask, &gw_addr);

            //
            // If we are going to DHCP mode, then start the DHCP server now.
            //
#if LWIP_DHCP
            if(ulIPMode == IPADDR_USE_DHCP)
            {
                dhcp_start(&g_sNetIF);
            }
#endif

            //
            // If we are going to AutoIP mode, then start the AutoIP process
            // now.
            //
#if LWIP_AUTOIP
            if(ulIPMode == IPADDR_USE_AUTOIP)
            {
                autoip_start(&g_sNetIF);
            }
#endif

            //
            // And we're done.
            //
            break;
        }

        //
        // DHCP (with AutoIP fallback).
        //
#if LWIP_DHCP
        case IPADDR_USE_DHCP:
        {
            //
            // If we are going to static IP addressing, then disable DHCP and
            // force the new static IP address.
            //
            if(ulIPMode == IPADDR_USE_STATIC)
            {
                dhcp_stop(&g_sNetIF);
                netif_set_addr(&g_sNetIF, &ip_addr, &net_mask, &gw_addr);
            }

            //
            // If we are going to AUTO IP addressing, then disable DHCP, set
            // the default addresses, and start AutoIP.
            //
#if LWIP_AUTOIP
            else if(ulIPMode == IPADDR_USE_AUTOIP)
            {
                dhcp_stop(&g_sNetIF);
                netif_set_addr(&g_sNetIF, &ip_addr, &net_mask, &gw_addr);
                autoip_start(&g_sNetIF);
            }
#endif
            break;
        }
#endif

        //
        // AUTOIP
        //
#if LWIP_AUTOIP
        case IPADDR_USE_AUTOIP:
        {
            //
            // If we are going to static IP addressing, then disable AutoIP and
            // force the new static IP address.
            //
            if(ulIPMode == IPADDR_USE_STATIC)
            {
                autoip_stop(&g_sNetIF);
                netif_set_addr(&g_sNetIF, &ip_addr, &net_mask, &gw_addr);
            }

            //
            // If we are going to DHCP addressing, then disable AutoIP, set the
            // default addresses, and start dhcp.
            //
#if LWIP_DHCP
            else if(ulIPMode == IPADDR_USE_DHCP)
            {
                autoip_stop(&g_sNetIF);
                netif_set_addr(&g_sNetIF, &ip_addr, &net_mask, &gw_addr);
                dhcp_start(&g_sNetIF);
            }
#endif
            break;
        }
#endif
    }

    //
    // Bring the interface up.
    //
    netif_set_up(&g_sNetIF);

    //
    // Save the new mode.
    //
    g_ulIPMode = ulIPMode;
}

//*****************************************************************************
//
//! Change the configuration of the lwIP network interface.
//!
//! \param ulIPAddr is the new IP address to be used (static).
//! \param ulNetMask is the new network mask to be used (static).
//! \param ulGWAddr is the new Gateway address to be used (static).
//! \param ulIPMode is the IP Address Mode.  \b IPADDR_USE_STATIC 0 will force
//! static IP addressing to be used, \b IPADDR_USE_DHCP will force DHCP with
//! fallback to Link Local (Auto IP), while \b IPADDR_USE_AUTOIP will force
//! Link Local only.
//!
//! This function will evaluate the new configuration data.  If necessary, the
//! interface will be brought down, reconfigured, and then brought back up
//! with the new configuration.
//!
//! \return None.
//
//*****************************************************************************
void
lwIPNetworkConfigChange(unsigned long ulIPAddr, unsigned long ulNetMask,
                        unsigned long ulGWAddr, unsigned long ulIPMode)
{
    //
    // Check the parameters.
    //
#if LWIP_DHCP && LWIP_AUTOIP
    ASSERT((ulIPMode == IPADDR_USE_STATIC) ||
           (ulIPMode == IPADDR_USE_DHCP) ||
           (ulIPMode == IPADDR_USE_AUTOIP));
#elif LWIP_DHCP
    ASSERT((ulIPMode == IPADDR_USE_STATIC) ||
           (ulIPMode == IPADDR_USE_DHCP));
#elif LWIP_AUTOIP
    ASSERT((ulIPMode == IPADDR_USE_STATIC) ||
           (ulIPMode == IPADDR_USE_AUTOIP));
#else
    ASSERT(ulIPMode == IPADDR_USE_STATIC);
#endif

    //
    // Save the network configuration for later use by the private network
    // configuration change.
    //
    g_ulIPAddr = ulIPAddr;
    g_ulNetMask = ulNetMask;
    g_ulGWAddr = ulGWAddr;

    //
    // Complete the network configuration change.  The remainder is done
    // immediately if not using a RTOS and it is deferred to the TCP/IP
    // thread's context if using a RTOS.
    //
#if NO_SYS
    lwIPPrivateNetworkConfigChange((void *)ulIPMode);
#else
    tcpip_callback(lwIPPrivateNetworkConfigChange, (void *)ulIPMode);
#endif
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
