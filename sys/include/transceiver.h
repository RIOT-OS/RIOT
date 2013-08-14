#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include "radio/types.h"

/* supported transceivers *
 * NOTE: necessary to include here again due to
 * https://github.com/RIOT-OS/RIOT/issues/117 */
#ifdef MODULE_CC110X
#include "cc1100-interface.h"
#endif

#ifdef MODULE_CC110X_NG
#include "cc110x_ng.h"
#endif

#ifdef MODULE_CC2420
#include "cc2420.h"
#endif

#ifdef MODULE_MC1322X
#include "mc1322x.h"
#include "maca.h"
#include "maca_packet.h"
#endif

/* Stack size for transceiver thread */
#ifndef TRANSCEIVER_STACK_SIZE
#define TRANSCEIVER_STACK_SIZE      (KERNEL_CONF_STACKSIZE_DEFAULT)
#endif

#define PAYLOAD_SIZE  (0)
#ifdef MODULE_CC110X
#if (CC1100_MAX_DATA_LENGTH > PAYLOAD_SIZE)
#undef PAYLOAD_SIZE
#define PAYLOAD_SIZE (CC1100_MAX_DATA_LENGTH)
#endif
#endif
#ifdef MODULE_CC110X_NG
#if (CC1100_MAX_DATA_LENGTH > PAYLOAD_SIZE)
#undef PAYLOAD_SIZE
#define PAYLOAD_SIZE (CC1100_MAX_DATA_LENGTH)
#endif
#endif
#ifdef MODULE_CC2420
#if (CC2420_MAX_DATA_LENGTH > PAYLOAD_SIZE)
#undef PAYLOAD_SIZE
#define PAYLOAD_SIZE (CC2420_MAX_DATA_LENGTH)
#endif
#endif
#ifdef MODULE_MC1322X
#if (MC1322X_MAX_DATA_LENGTH > PAYLOAD_SIZE)
#undef PAYLOAD_SIZE
#define PAYLOAD_SIZE (MC1322X_MAX_DATA_LENGTH)
#endif
#endif
/* The maximum of threads to register */
#define TRANSCEIVER_MAX_REGISTERED  (4)

/* The size of the message queue between driver and transceiver (must be power
 * of two */
#define TRANSCEIVER_MSG_BUFFER_SIZE     (32)

/**
 * @brief All supported transceivers
 */
#define TRANSCEIVER_NONE    (0x0)       ///< Invalid
#define TRANSCEIVER_CC1100  (0x01)      ///< CC110X transceivers
#define TRANSCEIVER_CC1020  (0x02)      ///< CC1020 transceivers
#define TRANSCEIVER_CC2420  (0x04)      ///< CC2420 transceivers
#define TRANSCEIVER_MC1322X (0x08)      ///< MC1322X transceivers

/**
 * @brief Data type for transceiver specification
 */
typedef uint16_t transceiver_type_t;

/**
 * @brief Message types for transceiver interface
 */
enum transceiver_msg_type_t {
    /* Message types for driver <-> transceiver communication */
    RCV_PKT_CC1020,        ///< packet was received by CC1020 transceiver
    RCV_PKT_CC1100,        ///< packet was received by CC1100 transceiver
    RCV_PKT_CC2420,        ///< packet was received by CC2420 transceiver
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
    GET_PAN,        ///< Get current pan
    SET_PAN,        ///< Set a new pan

    /* debug message types */
    DBG_IGN,        ///< add a physical address to the ignore list

    /* Error messages */
    ENOBUFFER,      ///< No buffer left
};

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
} transceiver_command_t;

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
