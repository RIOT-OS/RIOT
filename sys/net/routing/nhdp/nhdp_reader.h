/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     nhdp
 * @{
 *
 * @file
 * @brief       Reader interface for message processing in NHDP
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef NHDP_READER_H
#define NHDP_READER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief                   Set up the NHDP reader
 *
 * Initializes the reader and registers HELLO messages as a message
 * type consumed by this reader.
 */
void nhdp_reader_init(void);

/**
 * @brief                   Clean up the NHDP reader
 */
void nhdp_reader_cleanup(void);

/**
 * @brief                   Handle a packet received from a registered interface
 *
 * @note
 * HELLO message processing is triggered from this function.
 *
 * @param[in] rcvg_if_pid   PID of the interface the packet was received on
 * @param[in] buffer        Pointer to start of packet data
 * @param[in] length        Length in bytes of the packet data
 *
 * @return                  rfc5444_result of the packet handling process
 */
int nhdp_reader_handle_packet(kernel_pid_t rcvg_if_pid, void *buffer, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* NHDP_READER_H */
/** @} */
