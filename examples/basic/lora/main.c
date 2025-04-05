/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       LoRa application example
 *
 * @author      20100dbg
 *
 * @}
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net/lora.h"
#include "net/netdev/lora.h"
#include "sx126x_params.h"
#include "sx126x_netdev.h"

#include "ztimer.h"
//using ZTIMER_MSEC constant needs to add following in Makefile :
//USEMODULE += ztimer_msec

#define SX126X_MSG_QUEUE        (8U)
#define SX126X_STACKSIZE        (THREAD_STACKSIZE_DEFAULT)
#define SX126X_MSG_TYPE_ISR     (0x3456)
#define SX126X_MAX_PAYLOAD_LEN  (128U)

static char stack[SX126X_STACKSIZE];
static kernel_pid_t _recv_pid;

static char message[SX126X_MAX_PAYLOAD_LEN];

static sx126x_t sx126x;


struct s_config {
    uint32_t frequency;
    uint16_t bandwith;
    uint8_t spreading_factor;
    uint8_t coding_rate;
    bool integrity_check;
    uint16_t preamble;
    uint8_t payload_length;
    int8_t tx_power;
};

//Called every time we receive a LoRa packet
static void receive_callback(char *message, unsigned int len, int rssi, int snr,
                             long unsigned int toa)
{
    printf("Received: \"%s\" (%" PRIuSIZE " bytes) - [RSSI: %i, SNR: %i, TOA: %" PRIu32 "ms]\n",
           message, len, rssi, snr, toa);
}

//Called for every network event
static void _event_cb(netdev_t *dev, netdev_event_t event)
{

    if (event == NETDEV_EVENT_ISR) {
        msg_t msg;
        msg.type = SX126X_MSG_TYPE_ISR;
        if (msg_send(&msg, _recv_pid) <= 0) {
            puts("sx126x_netdev: possibly lost interrupt.");
        }
    }
    else {
        switch (event) {
        case NETDEV_EVENT_RX_STARTED:
            puts("Data reception started");
            break;

        //A packet is received
        case NETDEV_EVENT_RX_COMPLETE:
        {
            size_t len = dev->driver->recv(dev, NULL, 0, 0);
            netdev_lora_rx_info_t packet_info;
            dev->driver->recv(dev, message, len, &packet_info);

            receive_callback(message, len, packet_info.rssi, packet_info.snr,
                             sx126x_get_lora_time_on_air_in_ms(&sx126x.pkt_params,
                                                               &sx126x.mod_params));

            netopt_state_t state = NETOPT_STATE_RX;
            dev->driver->set(dev, NETOPT_STATE, &state, sizeof(state));
        }
        break;

        case NETDEV_EVENT_TX_COMPLETE:
            puts("Transmission completed");
            break;

        case NETDEV_EVENT_TX_TIMEOUT:
            puts("Transmission timeout");
            break;

        case NETDEV_EVENT_TX_STARTED: puts("NETDEV_EVENT_TX_STARTED"); break;
        case NETDEV_EVENT_TX_COMPLETE_DATA_PENDING: puts("NETDEV_EVENT_TX_COMPLETE_DATA_PENDING");
            break;
        case NETDEV_EVENT_TX_NOACK: puts("NETDEV_EVENT_TX_NOACK"); break;
        case NETDEV_EVENT_TX_MEDIUM_BUSY: puts("NETDEV_EVENT_TX_MEDIUM_BUSY"); break;
        case NETDEV_EVENT_LINK_UP: puts("NETDEV_EVENT_LINK_UP"); break;
        case NETDEV_EVENT_LINK_DOWN: puts("NETDEV_EVENT_LINK_DOWN"); break;
        case NETDEV_EVENT_RX_TIMEOUT: puts("NETDEV_EVENT_RX_TIMEOUT"); break;
        case NETDEV_EVENT_CRC_ERROR: puts("NETDEV_EVENT_CRC_ERROR"); break;
        case NETDEV_EVENT_FHSS_CHANGE_CHANNEL: puts("NETDEV_EVENT_FHSS_CHANGE_CHANNEL"); break;
        case NETDEV_EVENT_CAD_DONE: puts("NETDEV_EVENT_CAD_DONE"); break;

        default:
            printf("Unexpected netdev event received: %d\n", event);
            break;
        }
    }
}

//Receive loop
void *_recv_thread(void *arg)
{
    netdev_t *netdev = arg;

    static msg_t _msg_queue[SX126X_MSG_QUEUE];

    msg_init_queue(_msg_queue, SX126X_MSG_QUEUE);

    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == SX126X_MSG_TYPE_ISR) {
            netdev->driver->isr(netdev);
        }
        else {
            puts("Unexpected msg type");
        }
    }
}

//Read current LoRa settings into a s_config struct
static int read_config(netdev_t *netdev, struct s_config *config)
{
    //frequency
    netdev->driver->get(netdev, NETOPT_CHANNEL_FREQUENCY, &config->frequency, sizeof(uint32_t));

    //bandwith
    uint8_t bw;
    netdev->driver->get(netdev, NETOPT_BANDWIDTH, &bw, sizeof(uint8_t));

    switch (bw) {
    case LORA_BW_125_KHZ: config->bandwith = 125; break;
    case LORA_BW_250_KHZ: config->bandwith = 250; break;
    case LORA_BW_500_KHZ: config->bandwith = 500; break;
    default: config->bandwith = 0; break;
    }

    //spreading factor
    netdev->driver->get(netdev, NETOPT_SPREADING_FACTOR, &config->spreading_factor,
                        sizeof(uint8_t));

    //coding rate
    netdev->driver->get(netdev, NETOPT_CODING_RATE, &config->coding_rate, sizeof(uint8_t));

    //CRC
    netopt_enable_t crc;
    netdev->driver->get(netdev, NETOPT_INTEGRITY_CHECK, &crc, sizeof(netopt_enable_t));
    config->integrity_check = (crc == NETOPT_ENABLE);

    //preamble size
    netdev->driver->get(netdev, NETOPT_PREAMBLE_LENGTH, &config->preamble, sizeof(uint16_t));

    //payload length
    netdev->driver->get(netdev, NETOPT_PDU_SIZE, &config->payload_length, sizeof(uint8_t));

    //tx power
    netdev->driver->get(netdev, NETOPT_TX_POWER, &config->tx_power, sizeof(int8_t));


    return 0;
}

//Write settings from a s_config struct (no value check)
static int write_config(netdev_t *netdev, struct s_config config)
{
    int ret = 0;

    ret =
        netdev->driver->set(netdev, NETOPT_CHANNEL_FREQUENCY, &config.frequency, sizeof(uint32_t));
    if (ret < 0) {
        printf("cannot set frequency\n");
    }

    uint8_t bw_val = 0;
    switch (config.bandwith) {
    case 125: bw_val = LORA_BW_125_KHZ; break;
    case 250: bw_val = LORA_BW_250_KHZ; break;
    case 500: bw_val = LORA_BW_500_KHZ; break;
    default: break;
    }

    ret = netdev->driver->set(netdev, NETOPT_BANDWIDTH, &bw_val, sizeof(uint8_t));
    if (ret < 0) {
        printf("cannot set bandwith\n");
    }

    //7-12
    ret =
        netdev->driver->set(netdev, NETOPT_SPREADING_FACTOR, &config.spreading_factor,
                            sizeof(uint8_t));
    if (ret < 0) {
        printf("cannot set soreading factor\n");
    }

    //1-4
    ret = netdev->driver->set(netdev, NETOPT_CODING_RATE, &config.coding_rate, sizeof(uint8_t));
    if (ret < 0) {
        printf("cannot set coding rate\n");
    }

    netopt_enable_t enable = config.integrity_check ? NETOPT_ENABLE : NETOPT_DISABLE;
    ret = netdev->driver->set(netdev, NETOPT_INTEGRITY_CHECK, &enable, sizeof(netopt_enable_t));
    if (ret < 0) {
        printf("cannot set crc\n");
    }

    ret = netdev->driver->set(netdev, NETOPT_PREAMBLE_LENGTH, &config.preamble, sizeof(uint16_t));
    if (ret < 0) {
        printf("cannot set preamble length\n");
    }


    ret = netdev->driver->set(netdev, NETOPT_PDU_SIZE, &config.payload_length, sizeof(uint8_t));
    if (ret < 0) {
        printf("cannot set payload length\n");
    }

    ret = netdev->driver->set(netdev, NETOPT_TX_POWER, &config.tx_power, sizeof(int8_t));
    if (ret < 0) {
        printf("cannot set tx power\n");
    }


    return 0;
}

//Print settings inside a s_config struct
static void print_config(struct s_config config)
{
    printf("---------------\n");
    printf("frequency=%ld\n", config.frequency);
    printf("bandwith=%d\n", config.bandwith);
    printf("spreading_factor=%d\n", config.spreading_factor);
    printf("coding_rate=%d\n", config.coding_rate);
    printf("integrity_check=%s\n", config.integrity_check ? "enabled":"disabled");
    printf("preamble=%d\n", config.preamble);
    printf("payload_length=%d\n", config.payload_length);
    printf("tx_power=%d\n", config.tx_power);
    printf("---------------\n");
}

//Send LoRa packet
int send(netdev_t *netdev, char *data)
{
    printf("sending \"%s\" payload (%" PRIuSIZE " bytes)\n", data, strlen(data) + 1);

    iolist_t iolist = {
        .iol_base = data,
        .iol_len = (strlen(data) + 1)
    };

    if (netdev->driver->send(netdev, &iolist) == -ENOTSUP) {
        return -1;
    }

    return 0;
}


int main(void)
{
    //Init
    sx126x_setup(&sx126x, &sx126x_params[0], 0);
    netdev_t *netdev = &sx126x.netdev;
    netdev->driver = &sx126x_driver;

    netdev->event_callback = _event_cb;

    if (netdev->driver->init(netdev) < 0) {
        return 1;
    }

    //Read / set config
    struct s_config config;

    read_config(netdev, &config);
    print_config(config);

    if (false) {

        config.frequency = 868000000; //, 125, 7, 1, true, 8;
        write_config(netdev, config);
        read_config(netdev, &config);
        print_config(config);
    }

    //start listening thread
    _recv_pid = thread_create(stack, sizeof(stack), THREAD_PRIORITY_MAIN - 1, 0, _recv_thread,
                              netdev, "recv_thread");

    if (_recv_pid <= KERNEL_PID_UNDEF) {
        return 1;
    }

    //enable listening
    netopt_state_t state = NETOPT_STATE_IDLE;
    netdev->driver->set(netdev, NETOPT_STATE, &state, sizeof(state));

    //send a message
    char msg[] = "hello !";
    send(netdev, msg);



    /*
       //RIOT/build/pkg/driver_sx126x/src/sx126x.c

       sx126x_status_t sx126x_write_buffer( const void* context, const uint8_t offset, const uint8_t* buffer,
                                         const uint8_t size );

       sx126x_status_t sx126x_read_buffer( const void* context, const uint8_t offset, uint8_t* buffer, const uint8_t size );
     */

    //Infinite loop to avoid terminate program
    while (1) {
        ztimer_sleep(ZTIMER_MSEC, 1);
    }

    return 0;
}
