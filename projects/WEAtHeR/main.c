/* stdlib includes */
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* core includes */
#include <msg.h>
#include <thread.h>
#include <hwtimer.h>

#define ENABLE_DEBUG
#include <debug.h>

/* sensors and actors */
#include <board.h>
#include <sht11.h>
#include <ltc4150.h>

/* shell */
#include <shell.h>
#include <board_uart0.h>
#include <posix_io.h>

/* transceiver */
#include <cc1100.h>

/* real time clock */
#include <rtc.h>
#include <lpc2387-rtc.h>

/* application header */
#include "weather_routing.h"
#include "weather_protocol.h"
#include "protocol_msg_gateway.h"

/* some local defines */
#define SECOND  (1000 * 1000)
#define MINUTE  (1 * SECOND)

/* size of weather data packet without hop list */
#define EMPTY_WDP_SIZE  (sizeof(weather_data_pkt_t) - MAX_HOP_LIST)

/* default values */
#define DEFAULT_INTERVAL    (5 * SECOND)

/* stack space for threads */
#define SHELL_STACK_SIZE    (4048)
#define PH_STACK_SIZE    (4048)

char shell_stack_buffer[SHELL_STACK_SIZE];
char ph_stack_buffer[PH_STACK_SIZE];

/* per default acting only as relay */
static uint8_t data_sink = 0;
static uint8_t data_src = 0;

static uint32_t sending_interval = DEFAULT_INTERVAL;

extern uint8_t gossip_probability;

/* function prototypes */
static void weather_send(char* unused);
static void weather_sink(char* unused);
static void set_interval(char* interval);
static void set_probability(char* prob);
static void print_cc1100_info(char* unused);

/* shell commands */
shell_t shell;
const shell_command_t sc[] = {
    {"sender", "Enables node as data source.", weather_send},
    {"sink", "Enables node as data sink.", weather_sink},
    {"int", "Set the sending interval in seconds", set_interval},
    {"prob", "Set the gossiping probability", set_probability},
    {"cc1100", "Show state, statistics and config of cc1100", print_cc1100_info},
    {NULL, NULL, NULL}};

static void shell_runner(void) {
    shell_init(&shell, sc, uart0_readc, uart0_putc);
    posix_open(uart0_handler_pid, 0);
    shell_run(&shell);
}

static void weather_send(char* unused) {
    if (data_src) {
        data_src = 0;
        puts("Disabling data source mode.");
    }
    else {
        data_src = 1;
        puts("Enabling data source mode.");
    }
}

static void weather_sink(char* unused) {
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

static void print_cc1100_info(char* unused) {
            puts("==================================================");
            cc1100_print_statistic();
            puts("--------------------------------------------------");
            cc1100_print_config();
            puts("==================================================");
}

/* packet handling */
static void handle_packet(void* msg, int msg_size, packet_info_t* packet_info) {
    weather_packet_header_t *header = (weather_packet_header_t*) msg;

    /* packet origins at current node, just ignore it */
    if (header->src == cc1100_get_address()) {
        return;
    }
    DEBUG("\n\t Pkt received from phy: %u (%u bytes)\n"
            "\t -> SEQ: %u | TYPE: %u | SRC: %hu \n\n", 
            packet_info->phy_src,
            msg_size,
            header->seq_nr,
            header->type,
            header->src
          );
    /* while not acting as sink and packet contains data, route the packet */
    if (!data_sink) {
        if (header->type == WEATHER_DATA) {
            weather_data_pkt_t* wdp = (weather_data_pkt_t*) msg;
            DEBUG("$0;%hu;%hu;%04lX;%04X;%.2f;%.2f;%.2f;%.2f\n",
                    header->src,
                    0,
                    wdp->timestamp,
                    0,
                    wdp->temperature,
                    wdp->relhum,
                    wdp->relhum_temp,
                    wdp->energy);
            DEBUG("Not for me, routing, baby!\n");
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
                uint8_t i;
                time_t local_time = rtc_time(NULL);
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
        DEBUG("Packet handler done\n");
    }
}

int main(void) {
    weather_data_pkt_t wdp;
    sht11_val_t sht11_val;

    /* initialize variables */
    uint8_t success = 0;
    int sending_state = 0;
    gossip_probability = FLOODING_PROB;

    /* fill some fields of the packet */
    wdp.header.seq_nr = 0;
    wdp.header.type = WEATHER_DATA;
    wdp.hop_counter = 1;

    /* set initial channel */
    cc1100_set_channel(10);

    /* boot screen */
    puts("");
    puts("WEAtHeR: Wireless Energy-Aware mulTi-Hop sEnsor Reading.");
    puts("");
    printf("Sending interval: %lu\n", sending_interval / SECOND);
   
    /* start shell */
    thread_create(shell_stack_buffer, SHELL_STACK_SIZE, PRIORITY_MAIN-1, CREATE_STACKTEST, shell_runner, "shell");

    /* initialize message gateway */
    init_protocol_msg_gateway();
    /* create thread for radio packet handling */
    int pid = thread_create(ph_stack_buffer, PH_STACK_SIZE, PRIORITY_MAIN-2, CREATE_STACKTEST, protocol_handler_thread, "protocol_handler");
    set_protocol_handler_thread(pid);

    /* start coulomb counter and RTC */
    ltc4150_start();
    rtc_enable();

    /* loop forever */
    while (1) {
        DEBUG("Measurement in progress...\n");
#ifndef ENABLE_DEBUG
        success = sht11_read_sensor(&sht11_val, HUMIDITY|TEMPERATURE);
#else
        success = 1;
        sht11_val.temperature = 1;
        sht11_val.relhum = 2;
        sht11_val.relhum_temp = 3;
#endif
        DEBUG("...done.\n");

        if (data_src) {
            wdp.header.src = cc1100_get_address();
            DEBUG("Src filled\n");
            wdp.timestamp = rtc_time(NULL);
            DEBUG("Timestamp filled\n");
            wdp.energy = ltc4150_get_total_mAh();
            DEBUG("Energy filled\n");

            if (!success) {
                printf("error;error;error\n");
            }
            else {
                wdp.temperature = sht11_val.temperature;
                wdp.relhum = sht11_val.relhum;
                wdp.relhum_temp = sht11_val.relhum_temp;
                wdp.hops[0] = cc1100_get_address();
                DEBUG("Ready for sending\n");
                /* send packet with one entry in hop list */
                sending_state = cc1100_send_csmaca(0, WEATHER_PROTOCOL_NR, 0, (char*)&wdp, (EMPTY_WDP_SIZE + 1));
                if (sending_state > 0) {
                    DEBUG("Sending %lu bytes.\n", (EMPTY_WDP_SIZE + 1));
                    wdp.header.seq_nr++;
                }
                else {
                    printf("Error on sending packet with code %i!\n", sending_state);
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
#ifdef ENABLE_DEBUG
            puts("==================================================");
            cc1100_print_statistic();
            puts("--------------------------------------------------");
            cc1100_print_config();
            puts("==================================================");
#endif

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
