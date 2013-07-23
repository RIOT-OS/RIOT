#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H 

#include <radio/types.h>

/* Stack size for transceiver thread */
#ifdef ENABLE_DEBUG
#define TRANSCEIVER_STACK_SIZE      (2048)
#else
#define TRANSCEIVER_STACK_SIZE      (512)
#endif

#define PAYLOAD_SIZE  (58)

/* The maximum of threads to register */
#define TRANSCEIVER_MAX_REGISTERED  (4)

/* The size of the message queue between driver and transceiver (must be power
 * of two */
#define TRANSCEIVER_MSG_BUFFER_SIZE     (32)

/**
 * @brief Message types for transceiver interface
 */
enum transceiver_msg_type_t {
    /* Message types for driver <-> transceiver communication */
    RCV_PKT_CC1020,        ///< packet was received by CC1020 transceiver
    RCV_PKT_CC1100,        ///< packet was received by CC1100 transceiver
    RCV_PKT_MC1322X,       ///< packet was received by mc1322x transceiver

    /* Message types for transceiver <-> upper layer communication */
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

    /* debug message types */
    DBG_IGN,        ///< add a physical address to the ignore list

    /* Error messages */
    ENOBUFFER,      ///< No buffer left
};

/**
 * @brief All supported transceivers
 */
typedef enum {
    TRANSCEIVER_NONE = 0,       ///< Invalid
    TRANSCEIVER_CC1100 = 1,     ///< CC110X transceivers
    TRANSCEIVER_CC1020 = 2,     ///< CC1020 transceivers
    TRANSCEIVER_MC1322X = 4     ///< cm1322x transceiver
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
