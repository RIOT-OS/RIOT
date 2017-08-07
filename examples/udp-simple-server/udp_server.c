/*
 * Copyright (C) 2017 Ameya Joshi'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#include <stdio.h>
#include <string.h>
#include "periph/gpio.h"
#include "tcp_cc3200.h"

static char* ssid = "....";
static char* password = "....";

//#define BUF_SIZE 20

char Buf[20];

void server_hand(void) {
    SlSockAddrIn_t  sAddr;
    int iAddrSize;
    int iSockID;
    int iStatus;
    //long lLoopCount = 0;

    //filling the UDP server socket address
    sAddr.sin_family = SL_AF_INET;
    sAddr.sin_port = sl_Htons(8347);
    sAddr.sin_addr.s_addr = sl_Htonl(INADDR_ANY);

    iAddrSize = sizeof(SlSockAddrIn_t);

    // creating a UDP socket
    iSockID = sl_Socket(SL_AF_INET,SL_SOCK_DGRAM, 0);
    if( iSockID < 0 )
    {
        // error
        printf("\nError in Socket creation...\n");
    }
    //printf("Socket creation successful..\n");
    // binding the UDP socket to the UDP server address
    iStatus = bind(iSockID, (SlSockAddr_t *)&sAddr, iAddrSize);
    if( iStatus < 0 )
    {
        // error
        printf("\nError in Binding...\n");
        sl_Close(iSockID);
    }
    //printf("Binding successful..\n");
    // no listen or accept is required as UDP is connectionless protocol
    while(1)
    {
        iStatus = recv(iSockID, Buf, 20, 0);
        //iStatus = sl_RecvFrom(iSockID, Buf, strlen(Buf), 0, (SlSockAddr_t *)&sAddr, (SlSocklen_t*)&iAddrSize );
        if( iStatus < 0 )
       {
        // error
        printf("\nERROR in recv.., istatus=%d\n",iStatus);
        //sl_Close(iSockID);
       }
      //lLoopCount++;
      printf("server received data from client: %s\n",Buf);
    }
    printf("closing the connection\n");
    close(iSockID);
}

int main(void) {

    msg_t msg;

    sbapp_init(SBAPI_DEFAULT_CFG);

    sbapp_add_profile(ssid, password);

    sbapp_set_mode(ROLE_STA);

    while(1) {
           msg_receive(&msg);
           if(msg.type==SBAPP_IP_ACQUIRED) {
                printf("IP up, starting udp server ...\n");
	 	server_hand();
            }
	   else 
                printf("unexpected message %d\n", msg.type);
        }
    return 0;
}
