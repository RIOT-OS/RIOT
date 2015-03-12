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
 * @brief       Writer interface for message generation in NHDP
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef NHDP_WRITER_H_
#define NHDP_WRITER_H_

#include "rfc5444/rfc5444_iana.h"
#include "rfc5444/rfc5444_writer.h"

#include "nhdp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief                   Set up the NHDP writer
 *
 * Sets the writer's buffers, initializes the writer and registers HELLO messages
 * as new message type.
 */
void nhdp_writer_init(void);

/**
 * @brief                   Clean up the NHDP writer
 */
void nhdp_writer_cleanup(void);

/**
 * @brief                   Register a new target interface in the writer
 *
 * @param[in] new_if        Pointer to the writer target of the NHDP interface entry
 */
void nhdp_writer_register_if(struct rfc5444_writer_target *new_if);

/**
 * @brief                   Construct and send a HELLO message using the given interface
 *
 * @param[in] if_entry      Pointer to NHDP interface entry the message must be created for
 */
void nhdp_writer_send_hello(nhdp_if_entry_t *if_entry);

/**
 * @brief                   Add a NHDP address to the currently constructed message
 *
 * @note
 * Must not be called from outside the NHDP writer's message creation process.
 *
 * @param[in] wr            Pointer to the current NHDP writer used for message creation
 * @param[in] addr          Pointer to a NHDP address to add to the HELLO message
 * @param[in] type          TLV type for the address
 * @param[in] value         TLV value for the address
 * @param[in] metric_in     Encoded incoming link metric value
 * @param[in] metric_out    Encoded outgoing link metric value
 */
void nhdp_writer_add_addr(struct rfc5444_writer *wr, nhdp_addr_t *addr,
                          enum rfc5444_addrtlv_iana type, uint8_t value,
                          uint16_t metric_in, uint16_t metric_out);

#ifdef __cplusplus
}
#endif

#endif /* NHDP_WRITER_H_ */
/** @} */
