/*
 * Copyright (C) 2013  Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup dtls
 * @{
 * @file
 * @brief   add foo here
 * @author  Jonatan Zint <j.zint@fu-berlin.com>
 * @author  Nico von Geyso <nico.geyso@fu-berlin.com>
 */

#ifndef DTLS_HANDSHAKE_H_
#define DTLS_HANDSHAKE_H_

#include "dtls.h"
#include "record.h"

#define DTLS_HANDSHAKE_CLIENT_HELLO_SIZE_MIN 37


int dtls_handshake_start(dtls_connection_t *conn, dtls_record_t *record);
int dtls_handshake_verify(dtls_connection_t *conn, dtls_record_t *record,
                          dtls_handshake_type_t expected);

#endif
