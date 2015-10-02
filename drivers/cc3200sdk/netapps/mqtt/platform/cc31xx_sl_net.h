/******************************************************************************
*
*   Copyright (C) 2014 Texas Instruments Incorporated
*
*   All rights reserved. Property of Texas Instruments Incorporated.
*   Restricted rights to use, duplicate or disclose this code are
*   granted through contract.
*
*   The program may not be used without the written permission of
*   Texas Instruments Incorporated or against the terms and conditions
*   stipulated in the agreement under which this program has been supplied,
*   and under no circumstances can it be used with non-TI connectivity device.
*
******************************************************************************/

/*
  Network Services for General Purpose Linux environment
*/

#ifndef __CC31XX_SL_NET_H__
#define __CC31XX_SL_NET_H__

#include "simplelink.h"
#include "mqtt_client.h" /* conn options */



//*****************************************************************************
//                      MACROS
//*****************************************************************************

// MACRO to include receive time out feature
#define SOC_RCV_TIMEOUT_OPT 	1


/*-----------------------------------------------------------------------------
definitions needed for the functions in this file
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
prototypes of functions
-----------------------------------------------------------------------------*/

_i32 comm_open(_u32 nwconn_opts, const char *server_addr, _u16 port_number,
        		const struct secure_conn *nw_security);
_i32 tcp_send(_i32 comm, const u8 *buf, _u32 len, void *ctx);
_i32 tcp_recv(_i32 comm, u8 *buf, _u32 len, _u32 wait_secs, bool *timed_out,
		     void *ctx);
_i32 comm_close(_i32 comm);
_u32 rtc_secs(void);

/*-----------------functions added for server -----------------------------*/

_i32 tcp_listen(_u32 nwconn_info,  _u16 port_number,
                 const struct secure_conn *nw_security);

_i32 tcp_select(_i32 *recv_cvec, _i32 *send_cvec, _i32 *rsvd_cvec,
               _u32 wait_secs);


_i32 tcp_accept(_i32 listen_hnd, u8 *client_ip,
                 _u32 *ip_length);

/*----------------- adding functions for udp functionalities -------------------*/

/** Send a UDP packet 
    
    @param[in] comm communication entity handle; socket handle in this case
    @param[in] buf buf to be sent in the udp packet
    @param[in] len length of the buffer buf
    @param[in] dest_port destination port number
    @param[in] dest_ip ip address of the destination in dot notation, interpretted as a string;
                Only IPV4 is supported currently.
    @param[in] ip_len length of string dest_ip; currently not used
    @return number of bytes sent or error returned by sl_SendTo call
*/
_i32 send_dest(_i32, const u8 *buf, _u32 len, _u16 dest_port, const u8 *dest_ip, _u32 ip_len);

/** Recieve a UDP packet
    
    @param[in] comm communication entity handle; socket handle in this case
    @param[out] buf buf into which received UDP packet is written
    @param[in] maximum len length of the buffer buf
    @param[out] from_port port number of UDP packet source
    @param[out] from_ip ip address of the UDP packet source. 
                The ip address is to be interpreted as a _u32 number in network byte ordering
                Only IPV4 is supported currently.
    @param[out] ip_len length of string from_ip; 
               currently always populated with 4 as the address is a IPV4 address
    @return number of bytes received or error returned by sl_RecvFrom call
*/
_i32 recv_from(_i32 comm, u8 *buf, _u32 len, _u16 *from_port, u8 *from_ip, _u32 *ip_len);

#endif
