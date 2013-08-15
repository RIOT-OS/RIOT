#include <net/ethernet.h>

#define RX_BUF_SIZE (10)
#define TRANSCEIVER_BUFFER_SIZE (3)

#ifndef NATIVE_MAX_DATA_LENGTH
/* TODO: set this properly: */
#define NATIVE_MAX_DATA_LENGTH (ETHER_MAX_LEN - (ETHER_HDR_LEN+2))
#else
#warning be careful not to exceed (ETHER_MAX_LEN) with NATIVE_MAX_DATA_LENGTH
#endif /* NATIVE_MAX_DATA_LENGTH */

void nativenet_init(int transceiver_pid);
void nativenet_powerdown();

void nativenet_set_monitor(uint8_t mode);

uint8_t nativenet_send(radio_packet_t *packet);

int16_t nativenet_set_address(radio_address_t address);
int16_t nativenet_get_address();

int16_t nativenet_set_channel(uint8_t channel);
int16_t nativenet_get_channel();

uint16_t nativenet_set_pan(uint16_t pan);
uint16_t nativenet_get_pan();

void nativenet_switch_to_rx();
