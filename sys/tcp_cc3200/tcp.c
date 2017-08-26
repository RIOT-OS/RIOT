/*
 * Copyright (C) 2017 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */


/**
 *
 * @file
 *
 *
 * @defgroup     <id> <visible title>
 * @{
 *
 * @brief
 *
 * @author      Attilio Dona' <attilio.dona>
 *
 */
#include <stdio.h>
#include <inttypes.h>
#include "tcp_cc3200.h"

int16_t tcp_connect(const char *hostname, uint16_t server_port) {

    SlSockAddrIn_t addr;
    int iAddrSize;
    int16_t fd;
    int sts;
    uint32_t server_ip;

    // Resolve HOST NAME/IP
    sts = sl_NetAppDnsGetHostByName((signed char *)hostname,
                                          strlen((const char *)hostname),
                                          &server_ip,SL_AF_INET);
    addr.sin_family = SL_AF_INET;
    addr.sin_port = sl_Htons(server_port);
    addr.sin_addr.s_addr = sl_Htonl(server_ip);;

    // create the TCP socket
    fd = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, 0);
    if (fd < 0) {
        return fd;
    }

    iAddrSize = sizeof(SlSockAddrIn_t);

    // connect to the TCP server address
    sts = connect(fd, (SlSockAddr_t *) &addr, iAddrSize);
    if (sts < 0) {
        sl_Close(fd);
        return sts;
    }

    return fd;
}


/* @author      Ameya Joshi        <Ameya.Joshi> */

/*int16_t udp_connect(const char *hostname, uint16_t server_port) {

    SlSockAddrIn_t addr;
    int iAddrSize;
    int16_t fd;
    int sts;
    uint32_t server_ip;

    // Resolve HOST NAME/IP 
    sts = sl_NetAppDnsGetHostByName((signed char *)hostname,
                                          strlen((const char *)hostname),
                                          &server_ip,SL_AF_INET);
    addr.sin_family = SL_AF_INET;
    addr.sin_port = sl_Htons(server_port);
    //addr.sin_addr.s_addr = 0;
    addr.sin_addr.s_addr = sl_Htonl((unsigned int)g_ulDestinationIp);

    iAddrSize = sizeof(SlSockAddrIn_t);

    // create the UDP socket
    fd = sl_Socket(SL_AF_INET, SL_SOCK_DGRAM, 0);
    if (fd < 0) {
	printf("\n Error in socket creation...\n");
        return fd;
    }
    // binding the UDP socket to the UDP server address
    sts = sl_Bind(fd, (SlSockAddr_t *)&addr, iAddrSize);
    if( sts < 0 )
    {
        // error
        sl_Close(fd);
        printf("\n Bind Error...\n");
	return sts;
    }
    
    return fd;
}
*/
