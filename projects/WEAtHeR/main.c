#include <stdio.h>
#include <sht11.h>
#include <board.h>
#include <hwtimer.h>
#include <ltc4150.h>
#include <board_uart0.h>
#include <posix_io.h>
#include <msg.h>
#include <thread.h>
#include <board.h>
#include <shell.h>
#include <cc1100.h>
#include <rtc.h>
#include <lpc2387-rtc.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "weather_routing.h"
#include "weather_protocol.h"
#include "protocol_msg_gateway.h"

#define SECOND  (1000 * 1000)
#define MINUTE  (60 * SECOND)

#define DEFAULT_INTERVAL    (60 * SECOND)

/* size of weather data packet without hop list */
#define EMPTY_WDP_SIZE  (sizeof(weather_data_pkt_t) - MAX_HOP_LIST)

#define SHELL_STACK_SIZE    (4048)
#define PH_STACK_SIZE    (4048)

/* per default not acting as data sink */
static uint8_t data_sink = 0;
static uint8_t data_src = 0;

static uint32_t sending_interval = DEFAULT_INTERVAL;
extern uint8_t gossip_probability;

char shell_stack_buffer[SHELL_STACK_SIZE];
char ph_stack_buffer[PH_STACK_SIZE];

void weather_send(char* unused);
void weather_sink(char* unused);
static void set_interval(char* interval);
static void set_probability(char* prob);

shell_t shell;
const shell_command_t sc[] = {
    {"sender", "Enables node as data source.", weather_send},
    {"sink", "Enables node as data sink.", weather_sink},
    {"int", "Set the sending interval in seconds", set_interval},
    {"prob", "Set the gossiping probability", set_probability},
    {NULL, NULL, NULL}};

void shell_runner(void) {
    shell_init(&shell, sc, uart0_readc, uart0_putc);
    posix_open(uart0_handler_pid, 0);
    shell_run(&shell);
}

void weather_send(char* unused) {
    if (data_src) {
        data_src = 0;
        puts("Disabling data source mode.");
    }
    else {
        data_src = 1;
        puts("Enabling data source mode.");
    }
}

void weather_sink(char* unused) {
    if (data_sink) {
        data_sink = 0;
        puts("Disabling data sink mode.");
    }
    else {
        data_sink = 1;
        puts("Enabling data sink mode.");
    }
}

static void set_interval(char* interval) {
    uint16_t a;

    a = atoi(interval+4);
    if (strlen(interval) > 4) {
        printf("[WEAtHeR] Set interval to %u\n ", a);
        sending_interval = a * SECOND;
    }
    else {
        printf("[WEAtHeR] Current interval is %lu\n ", (sending_interval / SECOND));
    }
}

static void set_probability(char* prob) {
    uint16_t a;

    a = atoi(prob+4);
    if (strlen(prob) > 4) {
        printf("[WEAtHeR] Set probability to %hu\n ", a);
        gossip_probability = a;
    }
    else {
        printf("[WEAtHeR] Current probability is %hu\n ", gossip_probability);
    }
}

static void handle_packet(void* msg, int msg_size, packet_info_t* packet_info) {
    weather_packet_header_t *header = (weather_packet_header_t*) msg;

    /* packet origins at current node, just ignore it */
    if (header->src == cc1100_get_address()) {
        return;
    }
    /*
    printf("\n\t Pkt received from phy: %u (%u bytes)\n"
            "\t -> SEQ: %u | TYPE: %u | SRC: %hu \n\n", 
            packet_info->phy_src,
            msg_size,
            header->seq_nr,
            header->type,
            header->src
          );
          */
    /* while not acting as sink route the packet */
    if (!data_sink) {
        if (header->type == WEATHER_DATA) {
            //weather_data_pkt_t* wdp = (weather_data_pkt_t*) msg;
            /* <node_id_source>;<node_id_sink>;<timestamp_source>;<timestamp_sink>;<temperature>;<humidity_relative>;<humitidy_absolut>;<energy_counter> */
       /*     printf("$0;%hu;%hu;%04lX;%04X;%.2f;%.2f;%.2f;%.2f\n",
                    header->src,
                    0,
                    wdp->timestamp,
                    0,
                    wdp->temperature,
                    wdp->relhum,
                    wdp->relhum_temp,
                    wdp->energy);
                    */
            puts("Not for me, routing, baby!");
            route_packet(msg, msg_size);
        }

        return;
    }

    /* if current node acts as sink, handle packet */
    switch (header->type) {
        case WEATHER_HELLO: {
                if (msg_size < sizeof(weather_hello_pkt_t)) {
                    puts("Bad hello packet received.");
                } else {
                    puts("Hello packet received - no handler implemented");
                }
                break;
            }
        case WEATHER_CHAT: {
                puts("\n*================================================================================*");
                printf("\tCHAT MESSAGE from %hu: %s\n\n", packet_info->phy_src, ((weather_chat_pkt_t*) msg)->mesg);
                puts("*================================================================================*\n");
                break;
            }
        case WEATHER_DATA: {
                weather_data_pkt_t* wdp = (weather_data_pkt_t*) msg;
                time_t local_time = rtc_time(NULL);
                uint8_t i;
                /* <node_id_source>;<node_id_sink>;<timestamp_source>;<timestamp_sink>;<temperature>;<humidity_relative>;<humitidy_absolut>;<energy_counter> */
                printf("$1;%hu;%u;%04lX;%04lX;%.2f;%.2f;%.2f;%.2f;",
                        header->src,
                        cc1100_get_address(),
                        wdp->timestamp,
                        local_time,
                        wdp->temperature,
                        wdp->relhum,
                        wdp->relhum_temp,
                        wdp->energy);
                for (i = 0; i < wdp->hop_counter; i++) {
                    printf("%03u-", wdp->hops[i]);
                }
                puts("");
                break;
            }
        default: {
                     printf("Unknown packet type \"%i\" received.\n", header->type);
            }
    }
}

/* endless loop for packet handling */
static void protocol_handler_thread(void) {
    msg_t m;
    puts("Protocol handler thread started.");

    while(1) {
        msg_receive(&m);

        packet_t packet;
        int pos = m.content.value;
        packet = packet_buffer[pos];
        
        handle_packet(packet.payload, packet.msg_size, &(packet.packet_info));
    }
}

int main(void) {
    weather_data_pkt_t wdp;
    sht11_val_t sht11_val;

    uint8_t success = 0;
    gossip_probability = FLOODING_PROB;

    wdp.header.seq_nr = 0;
    wdp.header.type = WEATHER_DATA;
    wdp.hop_counter = 1;

    cc1100_set_channel(10);
    puts("");
    puts("WEAtHeR: Wireless Energy-Aware mulTi-Hop sEnsor Reading.");
    /* <node_id_source>;<node_id_sink>;<timestamp_source>;<timestamp_sink>;<temperature>;<humidity_relative>;<humitidy_absolut>;<energy_counter> */
    puts("Printing \"node id of data source;node id of sink;timestamp of measurement;timestamp at data sink;temperature in °C;relative humidity;temperature compensated relative humidity;energy value\".");
    puts("");
    printf("Sending interval: %lu\n", sending_interval);
    
    thread_create(shell_stack_buffer, SHELL_STACK_SIZE, PRIORITY_MAIN-1, CREATE_STACKTEST, shell_runner, "shell");

    init_protocol_msg_gateway();
    /* create thread for radio packet handling */
    int pid = thread_create(ph_stack_buffer, PH_STACK_SIZE, PRIORITY_MAIN-2, CREATE_STACKTEST, protocol_handler_thread, "protocol_handler");
    set_protocol_handler_thread(pid);

    ltc4150_start();
    rtc_enable();

    while (1) {
        success = sht11_read_sensor(&sht11_val, HUMIDITY|TEMPERATURE);
        if (data_src) {
            wdp.header.src = cc1100_get_address();
            wdp.timestamp = rtc_time(NULL);
            wdp.energy = ltc4150_get_total_mAh();
            success = sht11_read_sensor(&sht11_val, HUMIDITY|TEMPERATURE);
            if (!success) {
                printf("error;error;error\n");
            }
            else {
                wdp.temperature = sht11_val.temperature;
                wdp.relhum = sht11_val.relhum;
                wdp.relhum_temp = sht11_val.relhum_temp;
                wdp.hops[0] = cc1100_get_address();
                /* send packet with one entry in hop list */
                if (cc1100_send_csmaca(0, WEATHER_PROTOCOL_NR, 0, (char*)&wdp, (EMPTY_WDP_SIZE + 1)) > 0) {
                    printf("Sending %lu bytes.\n", (EMPTY_WDP_SIZE + 1));
                    wdp.header.seq_nr++;
                }
                else {
                    puts("Error on sending packet!");
                }
            }
            LED_GREEN_TOGGLE;
        }
        else {
            LED_RED_TOGGLE;
        }

        if (!success) {
                printf("error;error;error\n");
        }
        else {
            printf("$0;%hu;%hu;%04lX;%04X;%.2f;%.2f;%.2f;%.2f\n",
                    cc1100_get_address(),
                    0,
                    rtc_time(NULL),
                    0,
                    sht11_val.temperature,
                    sht11_val.relhum,
                    sht11_val.relhum_temp,
                    ltc4150_get_total_mAh());
        }
        hwtimer_wait(sending_interval);
    }
    puts("Something went wrong.");
}
