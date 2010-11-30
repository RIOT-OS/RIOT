#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H 

#include <radio/types.h>

/* Packets to buffer */
#define TRANSCEIVER_BUFFER_SIZE      (10)
/* Stack size for transceiver thread */
#define TRANSCEIVER_STACK_SIZE      (2048)

/* The maximum of threads to register */
#define TRANSCEIVER_MAX_REGISTERED  (10)

/* The size of the message queue between driver and transceiver (must be power
 * of two */
#define TRANSCEIVER_MSG_BUFFER_SIZE     (64)

/**
 * @brief Message types for transceiver interface
 */
enum transceiver_msg_type_t {
    /* Packet types for driver <-> transceiver communication */
    RCV_PKT_CC1020,        ///< packet was received by CC1020 transceiver
    RCV_PKT_CC1100,        ///< packet was received by CC1100 transceiver

    /* Packet types for transceiver <-> upper layer communication */
    PKT_PENDING,    ///< packet pending in transceiver buffer
    SND_PKT,        ///< request for sending a packet
    SND_ACK,        ///< request for sending an acknowledgement
    SWITCH_RX,      ///< switch transceiver to RX sate
    POWERDOWN,      ///< power down transceiver
    GET_CHANNEL,    ///< Get current channel
    SET_CHANNEL,    ///< Set a new channel
    GET_ADDRESS,    ///< Get the radio address
    SET_ADDRESS,    ///< Set the radio address
    SET_MONITOR,    ///< Set transceiver to monitor mode (disable address checking)

    /* Error messages */
    ENOBUFFER,      ///< No buffer left
};

/**
 * @brief All supported transceivers
 */
typedef enum {
    TRANSCEIVER_NONE,       ///< Invalid
    TRANSCEIVER_CC1100,     ///< CC110X transceivers
    TRANSCEIVER_CC1020      ///< CC1020 transceivers
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
    void *data;
} transceiver_command_t;;

/* The transceiver thread's pid */
extern int transceiver_pid;

/**
 * @brief Initializes the transceiver module for certain transceiver types 
 *
 * @param transceivers  Specifies all transceivers to init
 **/
void transceiver_init(transceiver_type_t transceivers);

/**
 * @brief Runs the transceiver thread
 */
int transceiver_start(void);

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
