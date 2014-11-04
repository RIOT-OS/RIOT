/*
 * Copyright (C) 2010 - 2014 Oliver Hahm
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_transceiver Transceiver
 * @ingroup     sys
 * @{
 *
 * @file        transceiver.h
 * @brief       Transceiver library
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include "kernel_types.h"
#include "radio/types.h"

/* supported transceivers *
 * NOTE: necessary to include here again due to
 * https://github.com/RIOT-OS/RIOT/issues/117 */
#ifdef MODULE_CC110X_LEGACY_CSMA
#include "cc110x_legacy_csma.h"
#ifndef TRANSCEIVER_DEFAULT
#define TRANSCEIVER_DEFAULT TRANSCEIVER_CC1100
#endif
#endif

#ifdef MODULE_CC110X_LEGACY
#include "cc110x_legacy.h"
#ifndef TRANSCEIVER_DEFAULT
#define TRANSCEIVER_DEFAULT TRANSCEIVER_CC1100
#endif
#endif

#ifdef MODULE_CC11020
#include "cc1020.h"
#ifndef TRANSCEIVER_DEFAULT
#define TRANSCEIVER_DEFAULT TRANSCEIVER_CC1020
#endif
#endif

#ifdef MODULE_CC2420
#include "cc2420.h"
#ifndef TRANSCEIVER_DEFAULT
#define TRANSCEIVER_DEFAULT TRANSCEIVER_CC2420
#endif
#endif

#ifdef MODULE_MC1322X
#include "mc1322x.h"
#include "maca.h"
#include "maca_packet.h"
#ifndef TRANSCEIVER_DEFAULT
#define TRANSCEIVER_DEFAULT TRANSCEIVER_MC1322X
#endif
#endif

#ifdef MODULE_NATIVENET
#include "nativenet.h"
#include "nativenet_internal.h"
#ifndef TRANSCEIVER_DEFAULT
#define TRANSCEIVER_DEFAULT TRANSCEIVER_NATIVE
#endif
#endif

#ifdef MODULE_AT86RF231
#include "at86rf231.h"
#ifndef TRANSCEIVER_DEFAULT
#define TRANSCEIVER_DEFAULT TRANSCEIVER_AT86RF231
#endif
#endif

/* Stack size for transceiver thread */
#ifndef TRANSCEIVER_STACK_SIZE
#define TRANSCEIVER_STACK_SIZE      (1024)
#endif

#ifndef PAYLOAD_SIZE
#define PAYLOAD_SIZE  (0)
#endif
#ifdef MODULE_CC110X_LEGACY_CSMA
#if (CC1100_MAX_DATA_LENGTH > PAYLOAD_SIZE)
#undef PAYLOAD_SIZE
#define PAYLOAD_SIZE (CC1100_MAX_DATA_LENGTH)
#endif
#endif
#ifdef MODULE_CC110X_LEGACY
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
#ifdef MODULE_AT86RF231
#if (AT86RF231_MAX_DATA_LENGTH > PAYLOAD_SIZE)
#undef PAYLOAD_SIZE
#define PAYLOAD_SIZE  (AT86RF231_MAX_DATA_LENGTH)
#endif
#endif
#ifdef MODULE_MC1322X
#if (MACA_MAX_PAYLOAD_SIZE > PAYLOAD_SIZE)
#undef PAYLOAD_SIZE
#define PAYLOAD_SIZE (MACA_MAX_PAYLOAD_SIZE)
#endif
#endif
#ifdef MODULE_NATIVENET
#if (NATIVE_MAX_DATA_LENGTH > PAYLOAD_SIZE)
#undef PAYLOAD_SIZE
#define PAYLOAD_SIZE (NATIVE_MAX_DATA_LENGTH)
#endif
#endif
/* The maximum of threads to register */
#define TRANSCEIVER_MAX_REGISTERED  (4)

/* The size of the message queue between driver and transceiver (must be power
 * of two */
#define TRANSCEIVER_MSG_BUFFER_SIZE     (32)

/** The maximum number of ignored addresses */
#define TRANSCEIVER_MAX_IGNORED_ADDR     (10)

/**
 * @brief All supported transceivers
 */
#define TRANSCEIVER_NONE        (0x0)       ///< Invalid
#define TRANSCEIVER_CC1100      (0x01)      ///< CC110X transceivers
#define TRANSCEIVER_CC1020      (0x02)      ///< CC1020 transceivers
#define TRANSCEIVER_CC2420      (0x04)      ///< CC2420 transceivers
#define TRANSCEIVER_MC1322X     (0x08)      ///< MC1322X transceivers
#define TRANSCEIVER_NATIVE      (0x10)      ///< NATIVE transceivers
#define TRANSCEIVER_AT86RF231   (0x20)      ///< AT86RF231 transceivers

/**
 * @brief Data type for transceiver specification
 */
typedef uint16_t transceiver_type_t;

/**
 * @brief Data type to represent the transceiver's EUI-64.
 */
typedef uint64_t transceiver_eui64_t;

/**
 * @brief Message types for transceiver interface
 */
enum transceiver_msg_type_t {
    /* Message types for driver <-> transceiver communication */
    RCV_PKT_CC1020,        ///< packet was received by CC1020 transceiver
    RCV_PKT_CC1100,        ///< packet was received by CC1100 transceiver
    RCV_PKT_CC2420,        ///< packet was received by CC2420 transceiver
    RCV_PKT_MC1322X,       ///< packet was received by mc1322x transceiver
    RCV_PKT_NATIVE,        ///< packet was received by native transceiver
    RCV_PKT_AT86RF231,     ///< packet was received by AT86RF231 transceiver

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
    GET_LONG_ADDR,  ///< Get the long radio address, if existing
    SET_LONG_ADDR,  ///< Set the long radio address, if supported by hardware
    SET_MONITOR,    ///< Set transceiver to monitor mode (disable address checking)
    GET_PAN,        ///< Get current pan
    SET_PAN,        ///< Set a new pan

    /* debug message types */
    DBG_IGN,        ///< add a physical address to the ignore list

    /* Error messages */
    ENOBUFFER,      ///< No buffer left

    /* reserve message types for higher layer notifications */
    UPPER_LAYER_1,  ///< reserved
    UPPER_LAYER_2,  ///< reserved
    UPPER_LAYER_3,  ///< reserved
    UPPER_LAYER_4,  ///< reserved
    UPPER_LAYER_5,  ///< reserved
};

/**
 * @brief Manage registered threads per transceiver
 */
typedef struct {
    transceiver_type_t transceivers;   ///< the tranceivers the thread is registered for
    kernel_pid_t pid;                  ///< the thread's pid
} registered_t;

typedef struct {
    transceiver_type_t transceivers;
    void *data;
} transceiver_command_t;

/* The transceiver thread's pid */
extern volatile kernel_pid_t transceiver_pid;

/** An array of ignored link layer addresses */
extern radio_address_t transceiver_ignored_addr[TRANSCEIVER_MAX_IGNORED_ADDR];

/**
 * @brief Initializes the transceiver module for certain transceiver types
 *
 * @param transceivers  Specifies all transceivers to init
 **/
void transceiver_init(transceiver_type_t transceivers);

/**
 * @brief Runs the transceiver thread
 *
 * @return              The transceiver thread's pid
 */
kernel_pid_t transceiver_start(void);

/**
 * @brief register a thread for events from certain transceivers
 *
 * @param transceivers  The transceiver types to register for
 * @param pid           The pid of the thread to register
 *
 * @return              1 on success, 0 otherwise
 */
uint8_t transceiver_register(transceiver_type_t transceivers, kernel_pid_t pid);

/**
 * @brief unregister a thread for events from certain transceivers
 *
 * @param transceivers  The transceiver types to register for
 * @param pid           The pid of the thread to register
 *
 * @return              1 on success, 0 otherwise
 */
uint8_t transceiver_unregister(transceiver_type_t transceivers, kernel_pid_t pid);

#endif /* TRANSCEIVER_H */
/** @} */
