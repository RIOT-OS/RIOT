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

#include "cc31xx_sl_net.h"

#ifdef DEBUG_NET_DEV
extern _i32 (*debug_printf)(const char *fmt, ...);
#define PRINTF(x,...)    debug_printf(x,##__VA_ARGS__)
#else
#define PRINTF(x,...)
#endif
/*
 3200 Devices specific Network Services Implementation
 */

#define LISTEN_QUE_SIZE 2

//*****************************************************************************
//                      GLOBAL VARIABLES
//*****************************************************************************

//*****************************************************************************
//                      STATIC FUNCTIONS
//*****************************************************************************

#ifdef DEBUG_NET_DEV
static _i32 buf_printf(const _u8 *buf, _u32 len, _u32 idt)
{
    _i32 i = 0;
    for(i = 0; i < len; i++)
    {
        PRINTF("%02x ", *buf++);

        if(0x03 == (i & 0x03))
        PRINTF(" ");

        if(0x0F == (i & 0x0F))
        {
            _i32 j = 0;
            PRINTF("\n\r");

            for(j = 0; j < idt; j++)
            PRINTF(" ");
        }
    }

    PRINTF("\n\r");

    return len;
}
#endif

/*-----------------------------------------------------------------------------
 Open a TCP socket and modify its properties i.e security options if req.
 Socket properties modified in this function are based on the options set
 outside the scope of this function.
 Returns a valid handle on success, otherwise a negative number.
 -----------------------------------------------------------------------------*/

static _i32 create_socket(_u32 nwconn_opts, struct secure_conn *nw_security_opts)
{
    _i32 MqttSocketFd, Status;

    //local variables for creating secure socket
    _u8 SecurityMethod;
    _u32 SecurityCypher;
    _i8 i;

    //If TLS is required
    if ((nwconn_opts & DEV_NETCONN_OPT_SEC) != 0) // bit was set to 1
            {
        MqttSocketFd = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, SL_SEC_SOCKET);
        if (MqttSocketFd < 0) {
            return (MqttSocketFd);
        }

        SecurityMethod = *((_u8 *) (nw_security_opts->method));
        SecurityCypher = *((_u32 *) (nw_security_opts->cipher));

        if (nw_security_opts->n_file < 1  || nw_security_opts->n_file > 4 ) {
            PRINTF("\n\r ERROR: security files missing or wrong number of\
                   security files\n\r");
            PRINTF("\n\r ERROR: Did not create socket\n\r");
            return (-1);
        }

        //Set Socket Options that were just defined
        Status = sl_SetSockOpt(MqttSocketFd, SL_SOL_SOCKET, SL_SO_SECMETHOD,
                &SecurityMethod, sizeof(SecurityMethod));
        if (Status < 0) {
            sl_Close(MqttSocketFd);
            return (Status);
        }

        Status = sl_SetSockOpt(MqttSocketFd, SL_SOL_SOCKET, SL_SO_SECURE_MASK,
                &SecurityCypher, sizeof(SecurityCypher));
        if (Status < 0) {
            sl_Close(MqttSocketFd);
            return (Status);
        }

        if(nw_security_opts->n_file == 1){
        	Status = sl_SetSockOpt(MqttSocketFd, SL_SOL_SOCKET, SL_SO_SECURE_FILES_CA_FILE_NAME,
        			 nw_security_opts->files[0], strlen(nw_security_opts->files[0]));
        	if (Status < 0) {
				sl_Close(MqttSocketFd);
				return (Status);
			}
        }else{
        	for(i=0; i<nw_security_opts->n_file;i++){
        		if(NULL != nw_security_opts->files[i]){
        			Status = sl_SetSockOpt(MqttSocketFd, SL_SOL_SOCKET,
        					(SL_SO_SECURE_FILES_PRIVATE_KEY_FILE_NAME+i),
        			        nw_security_opts->files[i],
        			        strlen(nw_security_opts->files[i]));
        			if (Status < 0) {
						sl_Close(MqttSocketFd);
						return (Status);
					}
        		}
        	}
        }

    }
    // If no TLS required
    else {
        // check to create a udp or tcp socket
        if ((nwconn_opts & DEV_NETCONN_OPT_UDP) != 0) // bit is set ; create a udp socket
                {
            MqttSocketFd = sl_Socket(SL_AF_INET, SL_SOCK_DGRAM, SL_IPPROTO_UDP);
        } else // socket for tcp
        {
            MqttSocketFd = sl_Socket(SL_AF_INET, SL_SOCK_STREAM,
            SL_IPPROTO_TCP); // consider putting 0 in place of SL_IPPROTO_TCP
        }
    }

    return (MqttSocketFd);

} // end of function

/*-----------------------------------------------------------------------------
 This function takes an ipv4 address in dot format i.e "a.b.c.d" and returns the
 ip address in Network byte Order, which can be used in connect call
 It returns 0, if a valid ip address is not detected.
 -----------------------------------------------------------------------------*/

static _u32 svr_addr_NB_order_IPV4(char *svr_addr_str)
{
    _u8 addr[4];
    _i8 i = 0;
    char *token;
    _u32 svr_addr;
    _i32 temp;

    /*take a temporary copy of the string. strtok modifies the input string*/
    _i8 svr_addr_size = strlen(svr_addr_str);
    char *svr_addr_cpy = malloc(svr_addr_size + 1); //1 for null
	if(NULL == svr_addr_cpy) return 0;
    strcpy(svr_addr_cpy, svr_addr_str);

    PRINTF("\n\r server address = %s\n\r", svr_addr_cpy);
    PRINTF("\n\r server address string length = %d\n\r", strlen(svr_addr_cpy));

    /* get the first token */
    token = strtok((char*)svr_addr_cpy, ".");

    /* walk through other tokens */
    while (token != NULL) {
        temp = atoi((const char*)token);

        //check for invalid tokens or if already 4 tokens were obtained
        if ((temp < 0) || (temp > 255) || (i >= 4)) {
            free(svr_addr_cpy);
            return (0);
        }

        addr[i++] = (_u8) temp;
        token = strtok(NULL, ".");
    }

    // check if exactly 4 valid tokens are available or not
    if (i != 4) {
        free(svr_addr_cpy);
        return (0);
    }

    //form address if above test passed
    svr_addr = *((_u32 *) &addr);
    free(svr_addr_cpy);

    return (svr_addr);

} // end of function

//*****************************************************************************
//                      Network Services functions
//*****************************************************************************

/*-----------------------------------------------------------------------------
 Open a TCP socket with required properties
 Also connect to the server.
 Returns a valid handle on success, NULL on failure.
 -----------------------------------------------------------------------------*/

_i32 comm_open(_u32 nwconn_opts, const char *server_addr, _u16 port_number,
        const struct secure_conn *nw_security)
{
    _i32 Status, MqttSocketFd;

    SlSockAddrIn_t LocalAddr; //address of the server to connect to
    _i32 LocalAddrSize;

    _u32 uiIP;

    // create socket
    MqttSocketFd = create_socket(nwconn_opts,
            (struct secure_conn*) nw_security);

    if (MqttSocketFd < 0) {
        PRINTF("\n\r ERROR: Could not create a socket.\n\r");
        return -1;
    }

    if ((nwconn_opts & DEV_NETCONN_OPT_UDP) != 0) // bit is set ; create a udp socket
            {
        //filling the UDP server socket address
        LocalAddr.sin_family = SL_AF_INET;
        LocalAddr.sin_port = sl_Htons((unsigned short) port_number);
        LocalAddr.sin_addr.s_addr = 0;
        LocalAddrSize = sizeof(SlSockAddrIn_t);

        Status = sl_Bind(MqttSocketFd, (SlSockAddr_t *) &LocalAddr,
                LocalAddrSize);
        if (Status < 0) {
            // error
            sl_Close(MqttSocketFd);
            return -1;
        }
    } else // do tcp connection
    {
        // get the ip address of server to do tcp connect
        if ((nwconn_opts & DEV_NETCONN_OPT_URL) != 0) // server address is a URL
                {
            Status = sl_NetAppDnsGetHostByName((_i8*) server_addr,
                    strlen(server_addr), (_u32*) &uiIP, SL_AF_INET);

            if (Status < 0) {
                PRINTF("\n\r ERROR: Could not resolve the ip address of the server \n\r");
                return (-1);
            }
            // convert the address to network byte order as the function returns in host byte order
            uiIP = sl_Htonl(uiIP);
        } else // server address is a string in dot notation
        {
            if ((nwconn_opts & DEV_NETCONN_OPT_IP6) != 0) // server address is an IPV6 address string
                    {
                PRINTF("\n\r ERROR: Currently do not support IPV6 addresses \n\r");
                return (-1);
            } else // address is an IPv4 string
            {
                // get the server ip address in Network Byte order
                uiIP = svr_addr_NB_order_IPV4((char*) server_addr);
                if (0 == uiIP) {
                    PRINTF("\n\r ERROR: Could not resolve the ip address of the server \n\r");
                    return (-1);
                }
            }

        }

        LocalAddr.sin_family = SL_AF_INET;
        LocalAddr.sin_addr.s_addr = uiIP;
        LocalAddr.sin_port = sl_Htons(port_number);
        LocalAddrSize = sizeof(SlSockAddrIn_t);

        // do tcp connect
        Status = sl_Connect(MqttSocketFd, (SlSockAddr_t *) &LocalAddr,
                LocalAddrSize);

        if (Status < 0) {
            if (SL_ESECSNOVERIFY != Status) {
                PRINTF(" \n\r ERROR: Could not establish connection to server.\n\r");PRINTF(" \n\r ERROR: Closing the socket.\n\r");

                sl_Close(MqttSocketFd);
                return (-1);
            } else // SL_ESECSNOVERIFY == Status
            {
                PRINTF(" \n\r ERROR: Could not establish secure connection to server.\n\r");PRINTF(" \n\r Continuing with unsecure connection to server...\n\r");
            }
        }

        // Success
        PRINTF("\n\r Connected to server ....\n\r");

    } // end of doing binding port to udp socket or doing tcp connect

    // set Timer for host processor
    platform_timer_init();

    return (MqttSocketFd);

} // end of function

_i32 tcp_send(_i32 comm, const _u8 *buf, _u32 len, void *ctx)
{
    _i32 Status;

    PRINTF("\n\r TCP send invoked for data with len %d\n\r", len);PRINTF("\n\r Sent Data : ");

#ifdef DEBUG_NET_DEV
    buf_printf(buf, len, 0);
#endif

    Status = sl_Send(comm, buf, len, 0);

    return (Status);

} // end of function

_i32 tcp_recv(_i32 comm, _u8 *buf, _u32 len, _u32 wait_secs, bool *timed_out,
        void *ctx)
{
    _i32 Status;
    _i32 MqttSocketFd = comm;

#ifdef SOC_RCV_TIMEOUT_OPT

    // socket receive time out options
    SlTimeval_t timeVal;

    // recv time out options
    timeVal.tv_sec = wait_secs;      // Seconds
    timeVal.tv_usec = 0;          // Microseconds. 10000 microseconds resoultion

    /*------------------- setting receive timeout option on socket ---------------------*/

    Status = sl_SetSockOpt(MqttSocketFd, SOL_SOCKET, SL_SO_RCVTIMEO, &timeVal,
            sizeof(timeVal));

    if (Status == 0) {
        PRINTF("\n\r successfully set socket recv_timeout_option %d\n\r",timeVal.tv_sec);
    } else if (Status < 0) {
        PRINTF("\n\r ERROR: setting socket recv_timeout_option unsuccessfull! \n\r");

    }
    /*--------------end of setting receive timeout option on socket ---------------------*/

#endif

    PRINTF("\n\r TCP recv invoked ...\n\r");
    *timed_out = 0;

    Status = sl_Recv(MqttSocketFd, buf, len, 0);

    PRINTF("\n\r Received a message with len %d\n\r", Status);

    //PRINTF("\n\r Data : ");
    if (Status > 0) {
#ifdef DEBUG_NET_DEV
        buf_printf(buf, Status, 0);
#endif
    }

    if (0 == Status) {
        PRINTF("\n\r Connection Closed by peer....\n\r");
    }

    if (SL_EAGAIN == Status) {
        PRINTF("\n\r ERROR: Recv Time out error on server socket \n\r");
        *timed_out = 1;
    }

    return (Status);

}             // end of function

_i32 comm_close(_i32 comm) {
    _i32 Status;

    Status = sl_Close(comm);
    return (Status);

}             // end of function

_u32 rtc_secs(void) {
    return(platform_get_time_in_secs());   
}             // end of function

//--------------------------- adding functions for server functionalities ---------------------------

_i32 tcp_listen(_u32 nwconn_info, _u16 port_number,
        const struct secure_conn *nw_security)
{
    SlSockAddrIn_t sLocalAddr;
    _i32 iSockID, iAddrSize;
    _i32 iStatus;

    //filling the TCP server socket address
    sLocalAddr.sin_family = SL_AF_INET;
    sLocalAddr.sin_port = sl_Htons(port_number);
    sLocalAddr.sin_addr.s_addr = 0;
    iAddrSize = sizeof(SlSockAddrIn_t);

    // creating a TCP socket
    iSockID = sl_Socket(SL_AF_INET, SL_SOCK_STREAM, 0);
    if (iSockID < 0) {
        // error
        return (-1);
    }

    // binding the TCP socket to the TCP server address
    iStatus = sl_Bind(iSockID, (SlSockAddr_t *) &sLocalAddr, iAddrSize);
    if (iStatus < 0) {
        // error
        sl_Close(iSockID);
        return (-1);
    }

    // putting the socket for listening to the incoming TCP connection
    iStatus = sl_Listen(iSockID, LISTEN_QUE_SIZE);
    if (iStatus < 0) {
        sl_Close(iSockID);
        return (-1);
    }

    PRINTF("\n\r\t Server Socket created and listening on port number: %d! \n\r", port_number);

    return (iSockID);

}             // end of function

_i32 tcp_select(_i32 *recv_cvec, _i32 *send_cvec, _i32 *rsvd_cvec, _u32 wait_secs)
{
    struct SlTimeval_t tv, *p_tv;
    SlFdSet_t rdfds;
    _i32 rd_idx = 0, wr_idx = 0, max_fd = 0;
    _i32 rv = 0;

    tv.tv_sec = wait_secs;
    tv.tv_usec = 0;

    p_tv = (0xFFFFFFFF != wait_secs) ? &tv : NULL;

    SL_FD_ZERO(&rdfds);

    while (-1 != recv_cvec[rd_idx]) {
        _i32 fd = recv_cvec[rd_idx++];

        SL_FD_SET(fd, &rdfds);

        if (max_fd < fd)
            max_fd = fd;
    }

    PRINTF("Blocking an network for (%s) %u secs to monitor %d fd(s)\n\r",
            p_tv? "finite" : "forever", wait_secs, rd_idx);

    rv = sl_Select(max_fd + 1, &rdfds, NULL, NULL, p_tv);

    if (rv <= 0) {
        PRINTF("Select Failed \n\r");
        return rv;
    }

    rd_idx = 0;
    while (-1 != recv_cvec[rd_idx]) {
        _i32 fd = recv_cvec[rd_idx++];
        if (SL_FD_ISSET(fd, &rdfds))
            recv_cvec[wr_idx++] = fd;
    }

    recv_cvec[wr_idx] = NULL;

    PRINTF("Number of sockets on which activity is observed = %d \n\r", wr_idx);

    return (wr_idx);
}             // end of function

_i32 tcp_accept(_i32 listen_hnd, _u8 *client_ip, _u32 *ip_length)
{
    _i32 new_fd;
    SlSockAddrIn_t client_addr = {0}; // client address
    SlSocklen_t cl_addr_size;

    cl_addr_size = sizeof(client_addr);

    new_fd = sl_Accept(listen_hnd, (struct SlSockAddr_t *) &client_addr,
                       &cl_addr_size);

    if (new_fd < 0) {
        PRINTF("\n\r ERROR: in accept \n\r");
        return (NULL);
    }

    client_ip[0] = (client_addr.sin_addr.s_addr & 0xFF000000) >> 24;
    client_ip[1] = (client_addr.sin_addr.s_addr & 0x00FF0000) >> 16;
    client_ip[2] = (client_addr.sin_addr.s_addr & 0x0000FF00) >> 8;
    client_ip[3] = (client_addr.sin_addr.s_addr & 0x000000FF);

    *ip_length = 4;

    return new_fd;

} // end of function

//--------------------------- adding functions for udp functionalities ---------------------------

_i32 send_dest(_i32 comm, const _u8 *buf, _u32 len, _u16 dest_port,
              const _u8 *dest_ip, _u32 ip_len)
{
    _i32 iSockID = (_i32) comm;
    _i32 iStatus, iAddrSize;
    SlSockAddrIn_t sAddr;
    _u32 uiDestIp;

    //get destination ip
#if 0
    uiDestIp = svr_addr_NB_order_IPV4((_u8*)dest_ip); //assuming a IPV4 address is passed in dot notation.
    if( 0 == uiDestIp )
    {
        PRINTF("\n\r ERROR: Could not resolve the ip address of the destination \n\r");
        return(-1);
    }
#else
    uiDestIp = (((_u32) dest_ip[0] << 24) | ((_u32) dest_ip[1] << 16)
            | (dest_ip[2] << 8) | (dest_ip[3]));

    PRINTF("Writing to %d, %08x\n", (_i32)comm, uiDestIp);

#endif

    //filling the UDP server socket address
    sAddr.sin_family = SL_AF_INET;
    sAddr.sin_port = sl_Htons((unsigned short) dest_port);
    sAddr.sin_addr.s_addr = sl_Htonl(uiDestIp);

    iAddrSize = sizeof(SlSockAddrIn_t);

    // sending packet
    iStatus = sl_SendTo(iSockID, buf, len, 0, (SlSockAddr_t *) &sAddr,
            iAddrSize);

    if (iStatus <= 0) {
        // error
        sl_Close(iSockID);
        PRINTF("Error: Closed the UDP socket\n\r");
    }

    return (iStatus);

} // end of function

_i32 recv_from(_i32 comm, _u8 *buf, _u32 len, _u16 *from_port, _u8 *from_ip,
               _u32 *ip_len)
{

    _i32 iSockID = (_i32) comm;
    _i32 iStatus, iAddrSize;
    SlSockAddrIn_t fromAddr = {0};

    iAddrSize = sizeof(SlSockAddrIn_t);

    iStatus = sl_RecvFrom(iSockID, buf, len, 0, (SlSockAddr_t *) &fromAddr,
                          (SlSocklen_t*) &iAddrSize);

    if (iStatus < 0) {
        // error
        sl_Close(iSockID);
        PRINTF("Error: Closed the UDP socket\n\r");
        return (iStatus);
    }

    //else populate from ip, from_port and ip_len parameters
    // refer to comments in .h
    if (from_port)
        *from_port = fromAddr.sin_port;

    if (from_ip) {
        from_ip[0] = (fromAddr.sin_addr.s_addr & 0xFF000000) >> 24;
        from_ip[1] = (fromAddr.sin_addr.s_addr & 0x00FF0000) >> 16;
        from_ip[2] = (fromAddr.sin_addr.s_addr & 0x0000FF00) >> 8;
        from_ip[3] = (fromAddr.sin_addr.s_addr & 0x000000FF);
        *ip_len = 4;
    }

    return (iStatus);

}    // end of function

