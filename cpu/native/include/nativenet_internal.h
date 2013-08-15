#include "tap.h"

#define NNEV_PWRDWN     0x01
#define NNEV_PWRUP      0x02
#define NNEV_MONITOR    0x03
#define NNEV_GETCHAN    0x04
#define NNEV_SETCHAN    0x05
#define NNEV_GETADDR    0x06
#define NNEV_SETADDR    0x07
#define NNEV_GETPAN     0x08
#define NNEV_SETPAN     0x09
#define NNEV_SEND       0x0a
#define NNEV_SWTRX      0x0b
#define NNEV_MAXEV      0x0b

struct rx_buffer_s {
    radio_packet_t packet;
    char data[NATIVE_MAX_DATA_LENGTH];
};

extern struct rx_buffer_s _nativenet_rx_buffer[];
extern volatile uint8_t rx_buffer_next;

void _nativenet_handle_packet(radio_packet_t *packet);
