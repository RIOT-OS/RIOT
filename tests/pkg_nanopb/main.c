/*
 * Copyright (c) 2011 Petteri Aimonen <jpa at nanopb.mail.kapsi.fi>
 *
 * This software is provided 'as-is', without any express or
 * implied warranty. In no event will the authors be held liable
 * for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 *    must not claim that you wrote the original software. If you use
 *    this software in a product, an acknowledgment in the product
 *    documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 *    must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 *
 * This file has been taken from
 * https://github.com/nanopb/nanopb/tree/master/examples/simple
 * (commit 5866b34) and changed to integrate well with RIOT (and comply to
 * RIOT's coding conventions).
 *
 *
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       nanopb test application
 *
 * @author      Petteri Aimonen <jpa at nanopb.mail.kapsi.fi>
 * @author      Kaspar Schleiser <kaspar@schleiser.de> (RIOT adaption)
 *
 * @}
 */

#include <stdio.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "simple.pb.h"

int main(void)
{
    /* This is the buffer where we will store our message. */
    uint8_t buffer[128];
    size_t message_length;
    bool status;

    /* Encode our message */
    {
        /* Allocate space on the stack to store the message data.
         *
         * Nanopb generates simple struct definitions for all the messages.
         * - check out the contents of simple.pb.h!
         * It is a good idea to always initialize your structures
         * so that you do not have garbage data from RAM in there.
         */
        SimpleMessage message = SimpleMessage_init_zero;

        /* Create a stream that will write to our buffer. */
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

        /* Fill in the lucky number */
        message.lucky_number = 13;

        /* Now we are ready to encode the message! */
        status = pb_encode(&stream, SimpleMessage_fields, &message);
        message_length = stream.bytes_written;

        /* Then just check for any errors.. */
        if (!status)
        {
            printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
            return 1;
        }
    }

    /* Now we could transmit the message over network, store it in a file or
     * wrap it to a pigeon's leg.
     */

    /* But because we are lazy, we will just decode it immediately. */

    {
        /* Allocate space for the decoded message. */
        SimpleMessage message = SimpleMessage_init_zero;

        /* Create a stream that reads from the buffer. */
        pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

        /* Now we are ready to decode the message. */
        status = pb_decode(&stream, SimpleMessage_fields, &message);

        /* Check for errors... */
        if (!status)
        {
            printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
            return 1;
        }

        /* Print the data contained in the message. */
        printf("Your lucky number was %d!\n", (int)message.lucky_number);
    }

    return 0;
}
