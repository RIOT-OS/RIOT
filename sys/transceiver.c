#include <stdio.h>
#include <errno.h>

#include <transceiver.h>

/* supported transceivers */
#include <cc1100.h>

transceiver_type_t transceiver = NONE;
int transceiver_pid = EINVAL;
radio_packet_t transceiver_buffer[TRANSCEIVER_BUFFER_SIZE];

static volatile uint8_t rx_buffer_pos = 0;
static volatile uint8_t transceiver_buffer_pos = 0;

const char transceiver_stack[TRANSCEIVER_STACK_SIZE];

void run(void);
void receive_packet(void);

void transceiver_init(transceiver_type_t t) {
    switch (t) {
        case CC110:
            transceiver = t;
            cc1100_init();
            break;
        default:
            puts("Invalid transceiver type");
            break;
    }
}

void transceiver_start(void) {
    transceiver_pid = thread_create(transceiver_stack, TRANSCEIVER_STACK_SIZE, PRIORITY_MAIN-1, CREATE_STACKTEST | CREATE_SLEEPING, run, "Transceiver");
    if (transceiver < 0) {
        puts("Error creating transceiver thread");
    }
}

void run(void) {
    msg m;
    while (1) {
        msg_receive(&m);
        switch (m) {
            case RCV_PKT:
                receive_packet();
                break;
            default:
                DEBUG("Unknown message received\n");
                break;
        }
    }
}


void receive_packet(void) {
    switch (transveiver) {
        case CC1100:
            dINT();
            rx_buffer_pos = cc1100_rx_buffer_next - 1;
            cc1100_packet_t p = cc1100_rx_buffer[rx_buffer_pos].packet;
            radio_info_t info = cc1100_rx_buffer[rx_buffer_pos].info;
            radio_packet_t trans_p = transceiver_buffer[transceiver_buffer_pos];

            trans_p.src = p.phy_src;
            trans_p.dst = p.address;
            trans_p.rssi = info.rssi;
            trans_p.lqi = info.lqi;
            trans_p.length = p.length;

            /* TODO: copy payload */
            eINT();
            break;
        default:
            puts("Invalid transceiver type");
            break;
    }
}
