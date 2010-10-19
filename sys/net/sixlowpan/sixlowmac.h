/* 6LoWPAN MAC header file */

#ifndef SIXLOWMAC_H
#define SIXLOWMAC_H

#include <stdio.h>
#include <stdint.h>
#include "sixlowip.h"

uint16_t get_radio_address(link_layer_addr *lla);

void send(void);

#endif /* SIXLOWMAC_H*/
