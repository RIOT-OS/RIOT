#include <stdlib.h>
#include <stdio.h>
#include <cc1100.h>
#include <time.h>
#include "weather_protocol.h"
#include "weather_routing.h"

uint8_t gossip_probability;

static source_timestamp_t sources[MAX_SOURCES];

static uint8_t update_sources(uint8_t id, time_t timestamp) {
    uint8_t i;

    puts("updating sources list");
    for (i = 0; i < MAX_SOURCES; i++) {
        /* source id found */
        if (sources[i].id == id) {
            printf("source already known, comparing timestamps: %04lX : %04lX\n", sources[i].timestamp, timestamp);
            /* more current timestamp received, updating */
            if (sources[i].timestamp < timestamp) {
                sources[i].timestamp = timestamp;
                return 1;
            }
            /* timestamp too old, discard this packet */
            else {
                puts("Timestamp too old, not routing");
                return 0;
            }
        }
        /* source id not yet stored creating new entry */
        else if (!sources[i].id) {
            puts("got to know a new source");
            sources[i].id = id;
            sources[i].timestamp = timestamp;
            return 1;
        }
    }
    puts("No more sources could be stored!");
    return 0;
}

void route_packet(void* msg, int msg_size) {
   weather_packet_header_t *header = (weather_packet_header_t*) msg;
   weather_data_pkt_t* wdp = NULL;
   if (header->type == WEATHER_DATA) {
        wdp = (weather_data_pkt_t*) msg;
        if (!update_sources(wdp->header.src, wdp->timestamp)) {
            return;
        }
   }

   if ((100.0 * rand()/(double) RAND_MAX) <= gossip_probability) {
       printf("Broadcasting packet...");
       /* if broadcasting weather data, append current hop */
       if (wdp != NULL) {
           if (wdp->hop_counter < MAX_HOP_LIST) {
               wdp->hops[wdp->hop_counter] = cc1100_get_address();
               wdp->hop_counter++;
               msg_size++;
           }
       }
       if (cc1100_send_csmaca(0, WEATHER_PROTOCOL_NR, 0, (char*)msg, msg_size)) {
               puts("successful!");
       }
       else {
           puts("failed!");
       }
   }
}

