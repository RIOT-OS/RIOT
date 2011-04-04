#include <stdio.h>
#include <sht11.h>
#include <board.h>
#include <swtimer.h>
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

#include "weather_routing.h"
#include "weather_protocol.h"
#include "protocol_msg_gateway.h"

#define SHELL_STACK_SIZE    (2048)
#define PH_STACK_SIZE    (2048)

/* per default not acting as data sink */
static uint8_t data_sink = 0;
static uint8_t data_src = 0;

char shell_stack_buffer[SHELL_STACK_SIZE];
char ph_stack_buffer[PH_STACK_SIZE];

void weather_send(char* unused);
void weather_sink(char* unused);

shell_t shell;
const shell_command_t sc[] = {
    {"sender", "Enables node as data source.", weather_send},
    {"sink", "Enables node as data sink.", weather_sink},
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

static void handle_packet(void* msg, int msg_size, packet_info_t* packet_info) {
    weather_packet_header_t *header = (weather_packet_header_t*) msg;

    printf("\n\t Pkt received from phy: %u\n"
            "\t -> SEQ: %u | TYPE: %u | SRC: %hu \n\n", 
            packet_info->phy_src,
            header->seq_nr,
            header->type,
            header->src
          );
   
    /* when destination is set, but I'm not the receiver, pass to routing */
    if (!data_sink) {
        if (header->type == WEATHER_DATA) {
            weather_data_pkt_t* wdp = (weather_data_pkt_t*) msg;
            /* <node_id_source>;<node_id_sink>;<timestamp_source>;<timestamp_sink>;<temperature>;<humidity_relative>;<humitidy_absolut>;<energy_counter> */
            printf("%hu;%hu;%04lX;%04X;%.2f;%.2f;%.2f;%.2f\n",
                    header->src,
                    0,
                    wdp->timestamp,
                    0,
                    wdp->temperature,
                    wdp->relhum,
                    wdp->relhum_temp,
                    wdp->energy);
        }

        puts("Not for me, routing, baby!");
        route_packet(msg, msg_size);
        return;
    }

    /* in all other cases handle the packet */
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
                /* <node_id_source>;<node_id_sink>;<timestamp_source>;<timestamp_sink>;<temperature>;<humidity_relative>;<humitidy_absolut>;<energy_counter> */
                printf("%hu;%u;%04lX;%04lX;%.2f;%.2f;%.2f;%.2f\n",
                        header->src,
                        1,
                        wdp->timestamp,
                        local_time,
                        wdp->temperature,
                        wdp->relhum,
                        wdp->relhum_temp,
                        wdp->energy);
                break;
            }
        default: {
                     printf("Unknown packet type \"%i\" received.", header->type);
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

    wdp.header.seq_nr = 0;
    wdp.header.type = WEATHER_DATA;
    
    puts("");
    puts("WEAtHeR: Wireless Energy-Aware mulTi-Hop sEnsor Reading.");
    /* <node_id_source>;<node_id_sink>;<timestamp_source>;<timestamp_sink>;<temperature>;<humidity_relative>;<humitidy_absolut>;<energy_counter> */
    puts("Printing \"node id of data source;node id of sink;timestamp of measurement;timestamp at data sink;temperature in °C;relative humidity;temperature compensated relative humidity;energy value\".");
    puts("");
    
    thread_create(shell_stack_buffer, SHELL_STACK_SIZE, PRIORITY_MAIN-1, CREATE_STACKTEST, shell_runner, "shell");

    /* create thread for radio packet handling */
    int pid = thread_create(ph_stack_buffer, PH_STACK_SIZE, PRIORITY_MAIN-5, CREATE_STACKTEST, protocol_handler_thread, "protocol_handler");
    set_protocol_handler_thread(pid);

    ltc4150_start();
    rtc_enable();

    while (1) {
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
                if (cc1100_send_csmaca(0, WEATHER_PROTOCOL_NR, 0, (char*)&wdp, sizeof(weather_data_pkt_t))) {
                    printf("Successfully sent packet: \n");
                    wdp.header.seq_nr++;
                }
                else {
                    puts("Error on sending packet!");
                }
            }
            LED_RED_TOGGLE;
        }
        swtimer_usleep(60 * 1000*1000);
    }
}
