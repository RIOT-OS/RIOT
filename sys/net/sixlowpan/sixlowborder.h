/* 6LoWPAN Border Router header file */

#ifndef SIXLOWBORDER_H
#define SIXLOWBORDER_H

#include <stdint.h>
#include <mutex.h>
#include <transceiver.h>

#include "sixlowip.h"
#include "semaphore.h"

extern ipv6_addr_t abr_addr;

uint16_t border_get_serial_reader();

uint8_t *get_serial_out_buffer(int offset);
uint8_t *get_serial_in_buffer(int offset);

uint8_t border_initialize(transceiver_type_t trans,ipv6_addr_t *border_router_addr);
void border_send_ipv6_over_lowpan(struct ipv6_hdr_t *packet, uint8_t aro_flag, uint8_t sixco_flag);
void border_process_lowpan(void);

#endif /* SIXLOWBORDER_H*/
