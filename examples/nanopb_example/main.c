#include <stdio.h>

#include "pb_common.h"
#include "pb_decode.h"
#include "pb_encode.h"

#include "proto_compiled/test.pb.h"

uint8_t encode_buffer[400];
size_t message_length;
bool status;

int encode_test_message(void){
    TestMessage message = TestMessage_init_zero;

    pb_ostream_t stream = pb_ostream_from_buffer(encode_buffer, sizeof(encode_buffer));

    message.test = 13;

    status = pb_encode(&stream, TestMessage_fields, &message);
    message_length = stream.bytes_written;

    if (!status)
    {
        printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    return 0;
}

int decode_test_message(void){
    TestMessage message = TestMessage_init_zero;

    pb_istream_t stream = pb_istream_from_buffer(encode_buffer, message_length);

    status = pb_decode(&stream, TestMessage_fields, &message);

    if (!status)
    {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    printf("test number %d!\n", (int)message.test);

    return 0;
}

int main(void)
{
    puts("RIOT nanopb example application");

    puts("Starts encoding...");
    encode_test_message();
    for(unsigned int i = 0; i < message_length; i++){
        printf("%c", encode_buffer[i]);
    }
    puts("Done encoding.");

    puts("Starts decoding...");
    decode_test_message();
    puts("Decoding done.");

    /* should be never reached */
    return 0;
}
