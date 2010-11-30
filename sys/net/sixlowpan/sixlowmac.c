/* 6LoWPAN MAC - layer 2 implementations */

#include "sixlowmac.h"
#include "sixlownd.h"
#include "radio/radio.h"
#include "drivers/cc110x/cc1100.h"

#define ipv6_buf ((struct ipv6_hdr_t*)&buffer[LL_HDR_LEN])

/* TODO: payload pointer, payload length */
void send(void){
    RADIO.send(get_radio_address(get_eui(&ipv6_buf->destaddr)),NULL,NULL,NULL,NULL);
}

uint16_t get_radio_address(ieee_802154_long_t *lla){
    return ((lla->uint8[6] << 8) + lla->uint8[7]);
}
