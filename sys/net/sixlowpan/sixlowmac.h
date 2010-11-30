/* 6LoWPAN MAC header file */

#ifndef SIXLOWMAC_H
#define SIXLOWMAC_H

#include <stdio.h>
#include <stdint.h>
#include "sixlowip.h"
#include "radio/radio.h"

//extern const radio_t *radio_driver;

uint16_t get_radio_address(ieee_802154_long_t *lla);

void send(void);

#endif /* SIXLOWMAC_H*/
