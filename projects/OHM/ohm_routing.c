#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <cc1100.h>
#include <hwtimer.h>

#include "ohm_protocol.h"
#include "ohm_routing.h"

//#define ENABLE_DEBUG
#include <debug.h>

uint8_t gossip_probability;

uint16_t pkts_dup = 0;
uint16_t pkts_success = 0;
uint16_t pkts_dropped = 0;

static source_timestamp_t sources[MAX_SOURCES];

uint8_t update_sources(uint8_t id, time_t timestamp) {
    uint8_t i;

    DEBUG("updating sources list\n");
    for (i = 0; i < MAX_SOURCES; i++) {
        /* source id found */
        if (sources[i].id == id) {
            DEBUG("source already known, comparing timestamps: %04lX : %04lX\n", sources[i].timestamp, timestamp);
            /* more current timestamp received, updating */
            if (sources[i].timestamp < timestamp) {
                sources[i].timestamp = timestamp;
                pkts_success++;
                return 1;
            }
            /* timestamp too old, discard this packet */
            else {
                pkts_dup++;
                DEBUG("Timestamp too old, discarding");
                return 0;
            }
        }
        /* source id not yet stored creating new entry */
        else if (!sources[i].id) {
            puts("got to know a new source");
            sources[i].id = id;
            sources[i].timestamp = timestamp;
            pkts_success++;
            return 1;
        }
    }
    pkts_dropped++;
    puts("No more sources could be stored!");
    return 0;
}

void route_packet(void* msg, int msg_size) {
   ohm_packet_header_t *header = (ohm_packet_header_t*) msg;
   ohm_data_pkt_t* wdp = NULL;
   int state = 0;

   if (header->type == OHM_DATA) {
        wdp = (ohm_data_pkt_t*) msg;
        if (!update_sources(wdp->header.src, wdp->timestamp)) {
            return;
        }
   }

   if ((100.0 * rand()/(double) RAND_MAX) <= gossip_probability) {
       DEBUG("Broadcasting packet...");
       /* if broadcasting ohm data, append current hop */
       if (wdp != NULL) {
           if (wdp->hop_counter < MAX_HOP_LIST) {
               wdp->hops[wdp->hop_counter] = cc1100_get_address();
               wdp->hop_counter++;
           }
       }
       state = cc1100_send_csmaca(0, OHM_PROTOCOL_NR, 0, (char*)msg, msg_size);
       if (state > 0) {
               DEBUG("successful!");
       }
       else {
           DEBUG("failed with code %i!\n", state);
       }
   }
}

