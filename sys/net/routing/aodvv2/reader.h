/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     aodvv2
 * @{
 *
 * @file        reader.h
 * @brief       reading and handling of RFC5444 aodvv2 messages
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 */

#ifndef AODVV2_READER_H_
#define AODVV2_READER_H_

#include <string.h>
#include <stdio.h>

#include "common/netaddr.h"
#include "rfc5444/rfc5444_reader.h"

#include "utils.h"
#include "routingtable.h"
#include "constants.h"
#include "seqnum.h"
#include "aodv.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize reader.
 */
void aodv_packet_reader_init(void);

/**
 * @brief Clean up after reader. Only needs to be called upon shutdown.
 */
void aodv_packet_reader_cleanup(void);

/**
 * @brief Read data buffer as RFC5444 packet and handle the data it contains
 *
 * @param[in] buffer  Data to be read and handled
 * @param[in] length  Length of data
 * @param[in] sender  Address of the node from which the packet was received
 */
int aodv_packet_reader_handle_packet(void *buffer, size_t length, struct netaddr *sender);

#ifdef  __cplusplus
}
#endif

#endif /* AODVV2_READER_H_ */
