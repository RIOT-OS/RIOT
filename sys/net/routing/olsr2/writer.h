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

#ifndef OLSR2_WRITER_H_
#define OLSR2_WRITER_H_

#include "common/common_types.h"
#include "rfc5444/rfc5444_writer.h"

typedef void (*write_packet_func_ptr)(
    struct rfc5444_writer *wr, struct rfc5444_writer_target *iface, void *buffer, size_t length);

/* these are also used in reader.c, just acces them directly instead of passing a pointer */
struct rfc5444_writer writer;
struct rfc5444_writer_target interface;

void writer_init(write_packet_func_ptr ptr);
void writer_send_hello(void);
void writer_send_tc(void);
void writer_cleanup(void);

#endif /* OLSR2_WRITER_H_ */
