/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include "../../sys/net/transport_layer/dtls/dtls.h"
#include "../../sys/net/transport_layer/dtls/record.h"

int main(void)
{
    uint8_t data[] = {1,2,3,4,5,6,7,8};
    int result;
    dtls_connection_t conn = {0};

    puts("Hello World!");

    uint64_t sequence = 255<<8;
    uint16_t epoch = 99;
    result = dtls_record_stream(&conn, TLS_CONTENT_TYPE_APPLICATION_DATA,
               data, 8);

    printf("result: %d\n", result);

    return 0;
}
