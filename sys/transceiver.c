#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <thread.h>
#include <msg.h>

#include <transceiver.h>

#define PAYLOAD_SIZE  (0)

/* supported transceivers */
#ifdef MODULE_CC110X_NG
#include <cc1100_ng.h>
#if (CC1100_MAX_DATA_LENGTH > PAYLOAD_SIZE)
    #undef PAYLOAD_SIZE
    #define PAYLOAD_SIZE (CC1100_MAX_DATA_LENGTH;)
#endif
#endif

//#define ENABLE_DEBUG (1)
#include <debug.h>

/* used transceiver types */
transceiver_type_t transceivers = TRANSCEIVER_NONE;

registered_t reg[TRANSCEIVER_MAX_REGISTERED];

/* packet buffers */
radio_packet_t transceiver_buffer[TRANSCEIVER_BUFFER_SIZE];
uint8_t data_buffer[TRANSCEIVER_BUFFER_SIZE * PAYLOAD_SIZE];

static volatile uint8_t rx_buffer_pos = 0;
static volatile uint8_t transceiver_buffer_pos = 0;

/* transceiver stack */
const char transceiver_stack[TRANSCEIVER_STACK_SIZE];

/* function prototypes */
static void run(void);
static void receive_packet(transceiver_type_t t);
static void receive_cc1100_packet(radio_packet_t *trans_p);
static uint8_t send_packet(transceiver_type_t t, radio_packet_t *pkt);

void transceiver_init(transceiver_type_t t) {
    uint8_t i;
    for (i = 0; i < TRANSCEIVER_MAX_REGISTERED; i++) {
        reg[i].transceivers = TRANSCEIVER_NONE;
        reg[i].pid          = 0;
    }
    if (t & TRANSCEIVER_CC1100) {
        transceivers |= t;
    }
    else {
        puts("Invalid transceiver type");
    }
}

int transceiver_start(void) {
    int pid = thread_create((char*) transceiver_stack, TRANSCEIVER_STACK_SIZE, PRIORITY_MAIN-1, CREATE_STACKTEST | CREATE_SLEEPING, run, "Transceiver");
    if (pid < 0) {
        puts("Error creating transceiver thread");
    }
    else if (transceivers & TRANSCEIVER_CC1100) {
        cc1100_init(pid);
    }
    return pid;
}

uint8_t transceiver_register(transceiver_type_t t, int pid) {
   uint8_t i;
   for (i = 0; ((i < TRANSCEIVER_MAX_REGISTERED) && (reg[i].transceivers != TRANSCEIVER_NONE)); i++);

   if (i >= TRANSCEIVER_MAX_REGISTERED) {
       return ENOMEM;
   }
   else {
       reg[i].transceivers |= t;
       reg[i].pid         = pid;
       return 1;
   }
}

/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/
void run(void) {
    msg m;
    send_packet_t *spkt;
    while (1) {
        msg_receive(&m);
        switch (m.type) {
            case RCV_PKT:
                receive_packet(m.content.value);
                break;
            case SND_PKT:
                spkt = (send_packet_t*) m.content.ptr;
                send_packet(spkt->transceivers, spkt->packet);
                break;
            default:
                DEBUG("Unknown message received\n");
                break;
        }
    }
}


static void receive_packet(transceiver_type_t t) {
    uint8_t i = 0;
    msg m;
    
    transceiver_buffer_pos = 0;
    while ((transceiver_buffer[transceiver_buffer_pos].processing) && (transceiver_buffer_pos < TRANSCEIVER_BUFFER_SIZE))
    {
        transceiver_buffer_pos++;
    }
    if (transceiver_buffer_pos >= TRANSCEIVER_BUFFER_SIZE) {
        m.type = ENOBUFFER;
        m.content.value = t;
    }
    else {
        radio_packet_t trans_p = transceiver_buffer[transceiver_buffer_pos];
        m.type = PKT_PENDING;

        if (t & TRANSCEIVER_CC1100) {
            receive_cc1100_packet(&trans_p); 
        }
        else {
            puts("Invalid transceiver type");
            return;
        }
    }

    while (reg[i].transceivers != TRANSCEIVER_NONE) {
        if (reg[i].transceivers & t) {
            m.content.value = transceiver_buffer_pos;
            msg_send(&m, reg[i].pid, false);
        }
        i++;
    }
}

static void receive_cc1100_packet(radio_packet_t *trans_p) {
    /* disable interrupts while copying packet */
    dINT();
    rx_buffer_pos = cc1100_get_buffer_pos();
    cc1100_packet_t p = cc1100_rx_buffer[rx_buffer_pos].packet;
    
    trans_p->src = p.phy_src;
    trans_p->dst = p.address;
    trans_p->rssi = cc1100_rx_buffer[rx_buffer_pos].rssi;
    trans_p->lqi = cc1100_rx_buffer[rx_buffer_pos].lqi;
    trans_p->length = p.length;
    memcpy((void*) &(cc1100_data_buffer[transceiver_buffer_pos]), p.data, CC1100_MAX_DATA_LENGTH);
    eINT();

    trans_p->data = (uint8_t*) &(data_buffer[transceiver_buffer_pos * CC1100_MAX_DATA_LENGTH]);
}
 

static uint8_t send_packet(transceiver_type_t t, radio_packet_t *pkt) {

    return res;
}
