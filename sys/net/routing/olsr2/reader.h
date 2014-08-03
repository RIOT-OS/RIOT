/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup olsr2
 * @{
 * @author  Benjamin Valentin <benpicco@zedat.fu-berlin.de>
 * @}
 */

#ifndef OLSR2_READER_H_
#define OLSR2_READER_H_

#include "common/common_types.h"
#include "rfc5444/rfc5444_reader.h"

void reader_init(void);
int reader_handle_packet(void *buffer, size_t length, struct netaddr *src, uint8_t metric_in);
void reader_cleanup(void);

#endif /* OLSR2_READER_H_ */
