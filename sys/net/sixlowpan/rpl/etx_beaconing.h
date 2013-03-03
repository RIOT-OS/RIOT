/*
 * etx_beaconing.h
 *
 *  Created on: Feb 26, 2013
 *      Author: stephan
 */

#ifndef ETX_BEACONING_H_
#define ETX_BEACONING_H_

#include "rpl_structs.h"

#define ETX_BEACON_STACKSIZE 2048

//[option|length|ipaddr.|packetcount] with up to 10 ipaddr|packetcount pairs
// 1 Byte 1 Byte  2 Byte  1 Byte
#define ETX_BUF_SIZE   (32)

#define ETX_RCV_BUFFER_SIZE     (64)

//ETX beaconing type (!ATTENTION! this is non-standard)
#define ETX_BEACON                  0x20

//prototypes
void init_etx_beaconing(void);
void etx_beacon(void);
void etx_radio(void);

typedef struct __attribute__((packed)) {
    uint8_t code;
    uint8_t length;
    uint8_t* data;
} etx_probe_t;


#endif /* ETX_BEACONING_H_ */
