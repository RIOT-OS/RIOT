/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech
Description: LoRaMac classA device implementation
License: Revised BSD License, see LICENSE.TXT file include in the project
Maintainer: Miguel Luis and Gregory Cristian
*/
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "shell_commands.h"
#include "thread.h"
#include "xtimer.h"
#include "periph/rtc.h"
#include "random.h"

#include "common.h"
#include "board.h"

#include "net/gnrc/netdev2.h"
#include "net/netdev2.h"
#include "sx1276_params.h"
#include "sx1276_netdev.h"
#include "net/gnrc/lorawan/lorawan.h"
#include "net/lorawan/hdr.h"
#include "net/gnrc/netreg.h"

#define GNRC_LORA_MSG_QUEUE 16

/*!
 * LoRaWAN confirmed messages
 */
#ifndef LORAWAN_CONFIRMED_MSG_ON 
    #define LORAWAN_CONFIRMED_MSG_ON                    false
#endif

/*!
 * LoRaWAN Adaptive Data Rate
 *
 * \remark Please note that when ADR is enabled the end-device should be static
 */
#ifndef LORAWAN_ADR_ON 
    #define LORAWAN_ADR_ON                              0
#endif

/*!
 * LoRaWAN application port
 */
#ifndef LORAWAN_APP_PORT 
    #define LORAWAN_APP_PORT                            2
#endif

/*!
 * User application data buffer size
 */

#if defined( USE_BAND_915 ) || defined( USE_BAND_915_HYBRID )

#define LORAWAN_APP_DATA_SIZE                       11

#endif


extern LoRaMacStatus_t Send(netdev2_lorawan_t *loradev, lw_hdr_t *hdr, uint8_t fPort,
                             void *fBuffer, uint16_t fBufferSize );
static uint8_t DevEui[] = LORAWAN_DEVICE_EUI;
static uint8_t AppEui[] = LORAWAN_APPLICATION_EUI;
static uint8_t AppKey[] = LORAWAN_APPLICATION_KEY;

static uint8_t NwkSKey[] = LORAWAN_NWKSKEY;
static uint8_t AppSKey[] = LORAWAN_APPSKEY;

/*!
 * Device address
 */
static uint32_t DevAddr = LORAWAN_DEVICE_ADDRESS;

/*!
 * Indicates if the MAC layer has already joined a network.
 */
static bool IsNetworkJoined = false;

/*!
 * User application data buffer size
 */
#define LORAWAN_APP_DATA_MAX_SIZE                           64

/*!
 * Indicates if the node is sending confirmed or unconfirmed messages
 */
static uint8_t IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;

/*!
 * LoRaWAN compliance tests support data
 */
struct ComplianceTest_s
{
    bool Running;
    uint8_t State;
    bool IsTxConfirmed;
    uint8_t AppPort;
    uint8_t AppDataSize;
    uint8_t *AppDataBuffer;
    uint16_t DownLinkCounter;
    bool LinkCheck;
    uint8_t DemodMargin;
    uint8_t NbGateways;
}ComplianceTest;

int lorawan_setup(int argc, char **argv) {

    if (argc < 5) {
        puts("ERROR: Not enough arguments");
        return -1;
    }

    kernel_pid_t pid = atoi(argv[1]);
    if(strstr(argv[2], "ota") != NULL)
    {
        // Initialize LoRaMac device unique ID
        #ifdef NZ32_SC151
            board_get_unique_id( DevEui );
            printf("BoardID: ");
            for(uint8_t i = 0; i < 8; i++)
            {
                printf("%02x ",*(DevEui + i));
            }
            printf("\n");
        #endif
        IsNetworkJoined = false;

        uint8_t res;
        gnrc_netapi_set(pid, NETOPT_LORAWAN_DEV_EUI,0, DevEui, sizeof(uint8_t*));
        gnrc_netapi_set(pid, NETOPT_LORAWAN_APP_EUI,0, AppEui, sizeof(uint8_t*));
        gnrc_netapi_set(pid, NETOPT_LORAWAN_APP_KEY,0, AppKey, sizeof(uint8_t*));
        gnrc_netapi_get(pid, NETOPT_LORAWAN_JOIN,0, &res, sizeof(uint8_t));

        puts("Activation Type: OTA");
    }
    else if(strstr(argv[2], "abp") != NULL)
    {
        if( DevAddr == 0 )
        {
            uint32_t netid = LORAWAN_NETWORK_ID;

            // Choose a random device address
            DevAddr = random_uint32_range( 0, 0x01FFFFFF+1 );
            // Get sesion Keys
            gnrc_netapi_set(pid, NETOPT_ADDRESS, 0, &DevAddr, sizeof(DevAddr));
            gnrc_netapi_set(pid, NETOPT_LORAWAN_APP_SKEY, 0, AppSKey, sizeof(AppKey)/sizeof(AppSKey[0]));
            gnrc_netapi_set(pid, NETOPT_LORAWAN_NWK_SKEY, 0, NwkSKey, sizeof(NwkSKey)/sizeof(NwkSKey[0]));
            gnrc_netapi_set(pid, NETOPT_LORAWAN_NET_ID, 0, &(netid), sizeof(netid));
            IsNetworkJoined = true;
        }
        puts("Activation Type: ABP");
    }
    else
    {
        puts("ERROR: Invalid activation type");
        return -1;
    }

    uint8_t adr;
    if(strstr(argv[3], "on") != NULL)
    {
        adr = 1;
        gnrc_netapi_set(pid, NETOPT_LORAWAN_ADR, 0, &(adr), sizeof(uint8_t));
        puts("Adaptative Data Rate: ON");
    }
    else if(strstr(argv[3], "off") != NULL)
    {
        adr = 0;
        gnrc_netapi_set(pid, NETOPT_LORAWAN_ADR, 0, &(adr), sizeof(uint8_t));
        puts("Adaptative Data Rate: OFF");
    }
    else
    {
        puts("ERROR: Invalid Adaptative Data Rate Setting");
        return -1;
    }

    uint8_t sw;
    if(strstr(argv[4], "public") != NULL)
    {
        sw = true;
        gnrc_netapi_set(pid, NETOPT_LORAWAN_NWK_TYPE, 0, &(sw), sizeof(uint8_t));
        puts("Network: PUBLIC");
    }
    else if(strstr(argv[4], "private") != NULL)
    {
        sw = false;
        gnrc_netapi_set(pid, NETOPT_LORAWAN_NWK_TYPE, 0, &(sw), sizeof(uint8_t));
        puts("Network: PRIVATE");
    }
    else
    {
        puts("ERROR: Invalid Network Type");
        return -1;
    }

    puts("lorawan_setup: configuration is set");

    return 0;
}

int send(int argc, char **argv)
{
    
    if (argc <= 4) {
        puts("ERROR: Not enough arguments");
        return -1;
    }
    kernel_pid_t pid = atoi(argv[1]);
    uint8_t retries = 0;
    if(strstr(argv[2], "unconfirmed") != NULL)
    {
        IsTxConfirmed = false;
        retries = 1;
        puts("Message Type: Unconfirmed");
    }
    else if(strstr(argv[2], "confirmed") != NULL)
    {
        IsTxConfirmed = true;
        retries = 8;
        puts("Message Type: Confirmed");
    }
    else
    {
        puts("ERROR: Invalid Message Type");
        return -1;
    }

        gnrc_netapi_set(pid, NETOPT_LORAWAN_TX_RETRIES,0, &retries, sizeof(uint8_t));


    uint8_t port = atoi(argv[3]);
    if(port >243)
    {
        puts("ERROR: Invalid AppPort");
        return -1;
    }

    puts("Preparing frame");

    uint8_t size = strlen(argv[4]);
    if(size>254)
    {
        puts("ERROR: Input string bigger max physical payload");
        return -1;
    }

    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, NULL, size+1, GNRC_NETTYPE_UNDEF);
    memcpy(pkt->data,&port,1);
    memcpy(((uint8_t*) pkt->data)+1,argv[4],size);

    msg_t msg;
    msg.type = GNRC_NETAPI_MSG_TYPE_SND;
    msg.content.ptr = pkt;

    msg_send(&msg, pid);
    puts("Frame sent");
    
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "lorawan_setup", "<ActivationType (ota, abp)> <ADR (on, off)> <NetworkType (public, private)> - sets up LoRaWAN settings", lorawan_setup},
    { "send", "<Type (confirmed, unconfirmed)> <AppPort(1..243)> <payload> ", send },
    { NULL, NULL, NULL }
};

void *_receiver_loop(void *arg)
{
    static msg_t _msg_q[GNRC_LORA_MSG_QUEUE];
    msg_t msg;
    msg_init_queue(_msg_q, GNRC_LORA_MSG_QUEUE);

    printf("Thread began!\n");
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                printf("Received msg\n");
                gnrc_pktbuf_release((gnrc_pktsnip_t*) msg.content.ptr);
                break;
            case GNRC_LORAWAN_JOIN_REQUEST_MSG:
                printf("Join request done!\n");
                IsNetworkJoined = true;
            case GNRC_LORAWAN_LINK_CHECK_MSG:
                if( ComplianceTest.Running == true )
                {
                    ComplianceTest.LinkCheck = true;
                    /*TODO:
                    ComplianceTest.DemodMargin = dev->demod_margin;
                    ComplianceTest.NbGateways = dev->number_of_gateways;
                    */
                }
            default:
                break;
        }
    }
    return NULL;
}

static char stack[THREAD_STACKSIZE_DEFAULT];
/**
 * Main application entry point.
 */
int main( void )
{
    kernel_pid_t pid2;
    pid2 = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 6, THREAD_CREATE_STACKTEST,
                     _receiver_loop, NULL, "lora_receiver");

    gnrc_netreg_entry_t lw_reg = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL, pid2);
    gnrc_netreg_register(GNRC_NETTYPE_LORAWAN, &lw_reg);
    /* start the shell */
    puts("Initialization successful - starting the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    while(1){}

    return 0;
}
