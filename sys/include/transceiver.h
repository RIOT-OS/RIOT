#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H 

/* Packets to buffer */
#define TRANSCEIVER_BUFFER_SIZE      (10)
/* Stack size for transceiver thread */
#define TRANSCEIVER_STACK_SIZE      (4096)

/* The maximum of threads to register */
#define TRANSCEIVER_MAX_REGISTERED  (10)

/**
 * @brief Message types for transceiver interface
 */
enum transceiver_msg_type_t {
    RCV_PKT,        ///< packet was received
    SND_PKT,        ///< request for sending a packet
    SND_ACK,        ///< request for sending an acknowledgement
    SWITCH_RX,      ///< switch receiver to RX sate
    POWERDOWN,      ///< power down receiver
};

/**
 * @brief All supported transceivers
 */
typedef enum {
    NONE,       ///< Invalid
    CC1100,     ///< CC110X transceivers
    CC1020      ///< CC1020 transceivers
} transceiver_type_t;

/**
 * @brief Manage registered threads per transceiver
 */
typedef struct {
    transceiver_type_t transceivers;   ///< the tranceivers the thread is registered for
    int pid;                ///< the thread's pid
} registered_t;

typedef struct {
    transceiver_type_t transceivers;
    radio_packet_t *packet;
} send_packet_t;

extern int transceiver_pid;
extern void *transceiver_rx_buffer;

/**
 * @brief Initializes the transceiver module for certain transceiver types 
 *
 * @param transceivers  Specifies all transceivers to init
 **/
void transceiver_init(transceiver_type_t transceivers);

/**
 * @brief Runs the transceiver thread
 */
void transceiver_start(void);

/**
 * @brief register a thread for events from certain transceivers
 *
 * @param transceivers  The transceiver types to register for
 * @param pid           The pid of the thread to register
 *
 * return               1 on success, 0 otherwise
 */
uint8_t transceiver_register(transceiver_type_t transceivers, int pid);

#endif /* TRANSCEIVER_H */
