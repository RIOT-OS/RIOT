#ifndef SIXLOWPAN_H
#define SIXLOWPAN_H

#define IP_PROCESS_STACKSIZE    2048

/* fragment size in bytes*/
#define FRAG_PART_ONE_HDR_LEN   4
#define FRAG_PART_N_HDR_LEN     5

#define DISPATCH_IPV6           0x41

#include "transceiver.h"

void sixlowpan_init(transceiver_type_t trans);

void output(uint8_t *addr, uint8_t *data);
void input(uint8_t *data, uint8_t length);

#endif
