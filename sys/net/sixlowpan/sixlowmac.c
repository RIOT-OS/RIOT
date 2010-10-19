/* 6LoWPAN MAC - layer 2 implementations */

#include "sixlowmac.h"
#include "sixlownd.h"
#include "drivers/cc110x/cc1100.h"
#include "radio/radio.h"

#define IP_BUFFER ((struct ipv6_hdr*)&buffer[LL_HEADER_LENGTH])

#define PRINTF(...) printf(__VA_ARGS__) 
#define PRINT6ADDR(addr) PRINTF("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])


/* TODO: payload pointer, payload length */
void send(void){
    RADIO.send(get_radio_address(get_eui(&IP_BUFFER->destaddr)),NULL,NULL,NULL,NULL);
}

uint16_t get_radio_address(link_layer_addr *lla){
    return ((lla->uint8[6] << 8) + lla->uint8[7]);
}
