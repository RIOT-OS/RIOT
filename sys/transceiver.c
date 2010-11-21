#include <stdio.h>
#include <errno.h>
#include <malloc.h>

#include <transceiver.h>

/* supported transceivers */
#include <cc1100.h>

/* used transceiver types */
uint8_t transceivers = NONE;
int transceiver_pid = EINVAL;

registered_t reg[TRANSCEIVER_MAX_REGISTERED];

/* packet buffers */
radio_packet_t transceiver_buffer[TRANSCEIVER_BUFFER_SIZE];
uint8_t *cc1100_data_buffer;

static volatile uint8_t rx_buffer_pos = 0;
static volatile uint8_t transceiver_buffer_pos = 0;

/* transceiver stack */
const char transceiver_stack[TRANSCEIVER_STACK_SIZE];

/* function prototypes */
void run(void);
void receive_packet(transceiver_type_t t);

void transceiver_init(transceiver_type_t t) {
    uint8_t i;
    for (i = 0; i < TRANSCEIVER_MAX_REGISTERED; i++) {
        reg[i].transceiver  = NULL;
        reg[i].pid          = NULL;
    }
    if (t & CC1100) {
        transceiver |= t;
        cc1100_init();
        cc1100_data_buffer = malloc(TRANSCEIVER_BUFFER_SIZE * CC100_MAX_DATA_LENGTH);
    }
    else {
        puts("Invalid transceiver type");
    }
}

void transceiver_start(void) {
    transceiver_pid = thread_create(transceiver_stack, TRANSCEIVER_STACK_SIZE, PRIORITY_MAIN-1, CREATE_STACKTEST | CREATE_SLEEPING, run, "Transceiver");
    if (transceiver_pid < 0) {
        puts("Error creating transceiver thread");
    }
}

uint8_t transceiver_register(transceiver_type_t t, int pid) {
   uint8_t i;
   for (i = 0; ((i < TRANSCEIVER_MAX_REGISTERED) && (reg[i].transceiver != NULL)); i++);

   if (i >= TRANSCEIVER_MAX_REGISTERED) {
       return ENOMEM;
   }
   else {
       reg[i].transceiver = t:
       reg[i].pid         = pid;
   }
}

/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/
void run(void) {
    msg m;
    while (1) {
        msg_receive(&m);
        switch (m) {
            case RCV_PKT:
                receive_packet();
                break;
            case SND_PKT:
                send_packet_t spkt = (send_packet_t*) m.cotent.ptr;
                send_packet(spkt.transceiver, spkt.packet);
                break;
            default:
                DEBUG("Unknown message received\n");
                break;
        }
    }
}


void receive_packet(transceiver_type_t t) {
    uint8_t i = 0;
    msg m;

    if (t & CC1100) {
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
        memcpy(cc1100_data_buffer[transceiver_buffer_pos], p.data, CC1100_MAX_DATA_LENGTH);
        eINT();
        
        while (reg[i].transceiver != NULL) {
            if (reg[i].transceiver & t) {
                msg_send(&m, reg[i].pid, false);
            }
            i++:
        }
    }
    else {
        puts("Invalid transceiver type");
    }
}

uint8_t send_packet(transceiver_type_t t, radio_packet_t *pkt) {
}
