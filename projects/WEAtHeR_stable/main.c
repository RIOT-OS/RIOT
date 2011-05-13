/* stdlib includes */
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* core includes */
#include <msg.h>
#include <thread.h>
#include <hwtimer.h>
#include <cib.h>
#include <scheduler.h>

//#define ENABLE_DEBUG
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
#include <cc1100-defaultSettings.h>

/* real time clock */
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
#define DEFAULT_INTERVAL    (30 * SECOND)

/* stack space for threads */
#define SHELL_STACK_SIZE    (4048)
#define PH_STACK_SIZE    (4048)


char shell_stack_buffer[SHELL_STACK_SIZE];
char ph_stack_buffer[PH_STACK_SIZE];

/* per default acting only as relay */
static uint8_t data_sink = 0;
static uint8_t data_src = 0;

cib_t cib;
uint16_t pkts_processed = 0;

static uint32_t sending_interval = DEFAULT_INTERVAL;

extern uint8_t gossip_probability;
extern void _cc110x_get_set_address_handler(char *addr);
extern void _cc110x_get_set_channel_handler(char *chan);
extern void _date_handler(char* c);

/* function prototypes */
static void weather_send(char* unused);
static void weather_sink(char* unused);
static void set_interval(char* interval);
static void set_probability(char* prob);
static void print_cc1100_info(char* unused);
static void stat_handler(char* unused);

int shell_readc() {
    char c = 0;
    posix_read(uart0_handler_pid, &c, 1);
    return c;
}

void shell_putchar(int c) {
    putchar(c);
}

/* shell commands */
shell_t shell;
const shell_command_t sc[] = {
    {"sender", weather_send},
    {"sink", weather_sink},
    {"int", set_interval},
    {"prob", set_probability},
    {"cc1100", print_cc1100_info},
    {"addr", _cc110x_get_set_address_handler},
    {"date", _date_handler},
    {"stats", stat_handler},
    {NULL, NULL}};

static void shell_runner(void) {
    posix_open(uart0_handler_pid, 0);
    shell_init(&shell, shell_readc, shell_putchar);
    shell.command_list = sc;
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

static void stat_handler(char* unused) {
    extern uint16_t pkts_received;
    extern uint16_t pkts_success;
    extern uint16_t pkts_dup;
    extern uint16_t pkts_dropped;
    printf("   :::::::::::::::::::::::::::   \n");
    printf("   :: Packets received:  %u ::\n", pkts_received);
    printf("   :: Packets processed: %u ::\n", pkts_processed);
    printf("   :: Packets success:   %u ::\n", pkts_success);
    printf("   :: Packets dup:       %u ::\n", pkts_dup);
    printf("   :: Packets dropped:   %u ::\n", pkts_dropped);
    printf("   :::::::::::::::::::::::::::   \n");
}

/* packet handling */
static void handle_packet(void* msg, int msg_size, packet_info_t* packet_info) {
    pkts_processed++;
    weather_packet_header_t *header = (weather_packet_header_t*) msg;

    printf("\n\t Pkt received from phy: %u (%u bytes)\n"
            "\t -> SEQ: %u | TYPE: %u | SRC: %hu \n\n", 
            packet_info->phy_src,
            msg_size,
            header->seq_nr,
            header->type,
            header->src
          );
    /* packet origins at current node, just ignore it */
    if (header->src == cc1100_get_address()) {
        return;
    }
    /* while not acting as sink and packet contains data, route the packet */
    if (!data_sink) {
        if (header->type == WEATHER_DATA) {
            weather_data_pkt_t* wdp = (weather_data_pkt_t*) msg;
            DEBUG("$0;%hu;%hu;%lu;%u;%.2f;%.2f;%.2f;%.2f;%.2f\n",
                    header->src,
                    0,
                    wdp->timestamp,
                    0,
                    wdp->temperature,
                    wdp->relhum,
                    wdp->relhum_temp,
                    wdp->energy,
                    pidlist[0].energy,
                    pidlist[1].energy);
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
                if (update_sources(header->src, wdp->timestamp)) {
                    printf("$1;%hu;%u;%lu;%lu;%.2f;%.2f;%.2f;%.2f;",
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
                }
                break;
            }
        default: {
                     printf("Unknown packet type \"%i\" received.\n", header->type);
            }
    }
}

/* endless loop for packet handling 
static void protocol_handler_thread(void) {
    msg m;
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
*/

static void protocol_handler_thread(void) {
    packet_t packet;
    int n = -1;
    while (1) {
        n = cib_get(&cib);
        if (n >= 0) {
            packet = packet_buffer[n];
            handle_packet(packet.payload, packet.msg_size, &(packet.packet_info));
        }
        else {
            thread_sleep();
        }
    }
}

int main(void) {
    weather_data_pkt_t wdp;
    sht11_val_t sht11_val;

    /* initialize variables */
    uint8_t success = 0;
    int sending_state = 0;
    gossip_probability = FLOODING_PROB;
    cib_init(&cib, PACKET_BUFFER_SIZE);

    /* fill some fields of the packet */
    wdp.header.seq_nr = 0;
    wdp.header.type = WEATHER_DATA;
    wdp.hop_counter = 1;

    /* set initial channel */
    cc1100_set_channel(15);

    /* boot screen */
    puts("");
    puts("WEAtHeR: Wireless Energy-Aware mulTi-Hop sEnsor Reading. V.4.4");
    puts("");
    printf("Sending interval: %lu\n", sending_interval / SECOND);
    extern int cc1100_phy_calc_wor_settings(uint16_t m);
    printf("Burst count: %i\n", cc1100_phy_calc_wor_settings(T_RX_INTERVAL));
   
    /* start shell */
    thread_create(SHELL_STACK_SIZE, PRIORITY_MAIN-1, CREATE_STACKTEST, shell_runner, "shell");

    /* initialize message gateway */
    init_protocol_msg_gateway();
    /* create thread for radio packet handling */
    int pid = thread_create(PH_STACK_SIZE, PRIORITY_MAIN-2, CREATE_STACKTEST, protocol_handler_thread, "protocol_handler");
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

        wdp.timestamp = rtc_time(NULL);
        DEBUG("Timestamp filled\n");

        if (data_src) {
            wdp.header.src = cc1100_get_address();
            DEBUG("Src filled\n");
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
        }
        else {
            LED_RED_TOGGLE;
        }

        if (!success) {
                printf("error;error;error\n");
        }
        else {
#ifdef ENABLE_DEBUG_CC1100
            puts("==================================================");
            cc1100_print_statistic();
            puts("--------------------------------------------------");
            cc1100_print_config();
            puts("==================================================");
#endif

          //thread_print_all();
             printf("$0;%hu;%hu;%lu;%u;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f\n",
                    cc1100_get_address(),
                    0,
                    wdp.timestamp,
                    0,
                    sht11_val.temperature,
                    sht11_val.relhum,
                    sht11_val.relhum_temp,
                    ltc4150_get_total_mAh(),
                    pidlist[0].energy,
                    pidlist[1].energy);
        }
        hwtimer_wait(sending_interval);
    }
    puts("Something went wrong.");
}
