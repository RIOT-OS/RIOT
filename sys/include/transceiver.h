#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H 

#define TRANSCEIVER_BUFFER_SIZE      (10)
#define TRANSCEIVER_STACK_SIZE      (4096)

enum transceiver_msg_type_t {
    RCV_PKT,
    SND_PKT,
    SND_ACK,
    SWITCH_RX,
    POWERDOWN,
};

enum transceiver_type_t {
    NONE,
    CC1100,
    CC1020
};

void transceiver_init(transceiver_type_t transceiver);

void transceiver_start(void);

extern int transceiver_pid;
extern void *transceiver_rx_buffer;

#endif /* TRANSCEIVER_H */
