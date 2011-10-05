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
#include "ohm.h"
#include "ohm_routing.h"
#include "ohm_protocol.h"
#include "protocol_msg_gateway.h"

/* some local defines */
#define SECOND  (1000 * 1000)
#define MINUTE  (1 * SECOND)

/* size of ohm data packet without hop list */
#define EMPTY_WDP_SIZE  (sizeof(ohm_data_pkt_t) - MAX_HOP_LIST) 

/* default values */
#define DEFAULT_INTERVAL    (5 * SECOND)

/* stack space for threads */
#define SHELL_STACK_SIZE    (2048)
#define PH_STACK_SIZE    (2048)

/* debugging pins */
#define P10_TOGGLE (FIO1PIN ^= BIT0)
#define P11_TOGGLE (FIO1PIN ^= BIT1)
#define P14_TOGGLE (FIO1PIN ^= BIT4)

char shell_stack_buffer[SHELL_STACK_SIZE];
char ph_stack_buffer[PH_STACK_SIZE];

/* Coulomb, thread and function measurement buffers */
volatile uint32_t cb_buf[MAX_TICK_BUF_SIZE];
volatile uint32_t *cb_buf_ptr;
volatile ohm_measurement_t thread_buf[MAX_THREAD_BUF_SIZE];
volatile ohm_measurement_t *thread_buf_ptr;
volatile ohm_measurement_t func_buf[MAX_FUNC_BUF_SIZE];
volatile ohm_measurement_t *func_buf_ptr;

uint32_t otto_says = 0;
uint8_t measure_pause = 1;

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
static void ohm_send(char* unused);
static void ohm_sink(char* unused);
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
    {"sender", ohm_send},
    {"sink", ohm_sink},
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

static void ohm_send(char* unused) {
    if (data_src) {
        data_src = 0;
        puts("Disabling data source mode.");
    }
    else {
        data_src = 1;
        puts("Enabling data source mode.");
    }
}

static void ohm_sink(char* unused) {
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

void otto(uint32_t timestamp, uint32_t value) {
    uint16_t i, j;

    if (measure_pause) {
        return;
    }
//    printf("tbp: %p\n", thread_buf_ptr);
    thread_buf_ptr->timestamp  = timestamp;
    thread_buf_ptr->value      = value;
    if (((thread_buf_ptr++) - thread_buf)  > CRIT_THREAD_BUF_SIZE) {
        /* output */
        otto_says = 1;
    }
    if (otto_says) {
        measure_pause = 1;
        /*
        printf("Otto says: %lX\n", otto_says);

        printf("Coulomb buffer is filled with %lu entries\n", (cb_buf_ptr - cb_buf));
        printf("Thread buffer is filled with %lu entries\n", (thread_buf_ptr - thread_buf));
        j = 0;
        for (i = 0; i < (cb_buf_ptr - cb_buf); i++) {
            for (; j < (thread_buf_ptr - thread_buf); j++) {
                if (thread_buf[j].timestamp > cb_buf[i]) break;
                printf("T;%04lX;%04lX\n", thread_buf[j].timestamp, thread_buf[j].value);
            }
            printf("C;%04lX\n", cb_buf[i]);
         }
         for (; j < (thread_buf_ptr - thread_buf); j++) {
             printf("T;%04lX;%04lX\n", thread_buf[j].timestamp, thread_buf[j].value);
         }
        cb_buf_ptr = cb_buf;
        thread_buf_ptr = thread_buf;

        printf("Function buffer is filled with %lu entries\n", (func_buf_ptr - func_buf));
        */
/*        for (i = 0; i < (func_buf_ptr - func_buf); i++) {
            printf("F;%04lX;%04lX\n", func_buf[i].timestamp, func_buf[i].value);
        }*/
        func_buf_ptr = func_buf;

        otto_says = 0;
        measure_pause = 0;
    }

    //printf("tbp: %p\n", thread_buf_ptr);
}

/* packet handling */
static void handle_packet(void* msg, int msg_size, packet_info_t* packet_info) {
    pkts_processed++;
    ohm_packet_header_t *header = (ohm_packet_header_t*) msg;

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
        if (header->type == OHM_DATA) {
            ohm_data_pkt_t* odp = (ohm_data_pkt_t*) msg;
            DEBUG("$0;%hu;%hu;%lu;%u;%.2f;%.2f;%.2f\n",
                    header->src,
                    0,
                    odp->timestamp,
                    0,
                    odp->temperature,
                    odp->relhum,
                    odp->relhum_temp,
                    odp->energy
                 )
            DEBUG("Not for me, routing, baby!\n");
            route_packet(msg, msg_size);
        }

        return;
    }

    /* if current node acts as sink, handle packet */
    switch (header->type) {
        case OHM_HELLO: {
                if (msg_size < sizeof(ohm_hello_pkt_t)) {
                    puts("Bad hello packet received.");
                } else {
                    puts("Hello packet received - no handler implemented");
                }
                break;
            }
        case OHM_CHAT: {
                puts("\n*================================================================================*");
                printf("\tCHAT MESSAGE from %hu: %s\n\n", packet_info->phy_src, ((ohm_chat_pkt_t*) msg)->mesg);
                puts("*================================================================================*\n");
                break;
            }
        case OHM_DATA: {
                ohm_data_pkt_t* odp = (ohm_data_pkt_t*) msg;
                uint8_t i;
                time_t local_time = rtc_time(NULL);
                if (update_sources(header->src, odp->timestamp)) {
                    printf("$1;%hu;%u;%lu;%lu;%.2f;%.2f;%.2f;%.2f;",
                            header->src,
                            cc1100_get_address(),
                            odp->timestamp,
                            local_time,
                            odp->temperature,
                            odp->relhum,
                            odp->relhum_temp,
                            odp->energy);
                    for (i = 0; i < odp->hop_counter; i++) {
                        printf("%03u-", odp->hops[i]);
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

static void protocol_handler_thread(void) {
    puts("Start protocol handler thread");
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
    FIO1DIR |= BIT0;
    FIO1DIR |= BIT1;
    FIO1DIR |= BIT4;

    ohm_data_pkt_t odp;
    sht11_val_t sht11_val;

    /* initialize variables */
    uint8_t success = 0;
    int sending_state = 0;
    gossip_probability = FLOODING_PROB;
    cib_init(&cib, PACKET_BUFFER_SIZE);

    cb_buf_ptr = cb_buf;
    thread_buf_ptr = thread_buf;
    func_buf_ptr = func_buf;

    /* register thread switch callback */
    void sched_register_cb(void (*callback)(uint32_t, uint32_t));
    sched_register_cb(otto);

    /* fill some fields of the packet */
    odp.header.seq_nr = 0;
    odp.header.type = OHM_DATA;
    odp.hop_counter = 1;

    /* set initial channel */
    cc1100_set_channel(15);

    /* boot screen */
    puts("");
    puts("WEAtHeR: Wireless Energy-Aware mulTi-Hop sEnsor Reading. V.4.4");
    puts("");
    printf("Sending interval: %lu\n", sending_interval / SECOND);
    extern int cc1100_phy_calc_wor_settings(uint16_t m);
    printf("Burst count: %i\n", cc1100_phy_calc_wor_settings(T_RX_INTERVAL));

    printf(" Coulomb buffer size: %u\n", CRIT_TICK_BUF_SIZE); 
    printf(" Thread buffer size: %u\n", CRIT_THREAD_BUF_SIZE); 
    printf(" Function  buffer size: %u\n", CRIT_FUNC_BUF_SIZE); 
   
    /* start shell */
    thread_create(SHELL_STACK_SIZE, PRIORITY_MAIN-1, CREATE_STACKTEST, shell_runner, "shell");

    measure_pause = 0;

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
        P14_TOGGLE;
        success = sht11_read_sensor(&sht11_val, HUMIDITY|TEMPERATURE);
        P14_TOGGLE;
#else
        success = 1;
        sht11_val.temperature = 1;
        sht11_val.relhum = 2;
        sht11_val.relhum_temp = 3;
#endif
        DEBUG("...done.\n");

        odp.timestamp = rtc_time(NULL);
        DEBUG("Timestamp filled\n");

        if (data_src) {
            odp.header.src = cc1100_get_address();
            DEBUG("Src filled\n");
            odp.energy = ltc4150_get_total_mAh();
            DEBUG("Energy filled\n");

            if (!success) {
                printf("error;error;error\n");
            }
            else {
                odp.temperature = sht11_val.temperature;
                odp.relhum = sht11_val.relhum;
                odp.relhum_temp = sht11_val.relhum_temp;
                odp.hops[0] = cc1100_get_address();
                DEBUG("Ready for sending\n");
                /* send packet with one entry in hop list */
                P11_TOGGLE;
                sending_state = cc1100_send_csmaca(0, OHM_PROTOCOL_NR, 0, (char*)&odp, (EMPTY_WDP_SIZE + 1));
                P11_TOGGLE;
                if (sending_state > 0) {
                    DEBUG("Sending %lu bytes.\n", (EMPTY_WDP_SIZE + 1));
                    odp.header.seq_nr++;
                }
                else {
                    printf("Error on sending packet with code %i!\n", sending_state);
                }
            }
        }
        else {
            //LED_RED_TOGGLE;
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

          thread_print_all();
          printf("$0;%hu;%hu;%lu;%u;%.2f;%.2f;%.2f;%.2f\n",
                    cc1100_get_address(),
                    0,
                    odp.timestamp,
                    0,
                    sht11_val.temperature,
                    sht11_val.relhum,
                    sht11_val.relhum_temp,
                    ltc4150_get_total_mAh()
                   );
        }
        hwtimer_wait(sending_interval);
    }
    puts("Something went wrong.");
}
