#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "sys/socket.h"
#include "record.h"
#include "common.h"


int dtls_record_read(dtls_record_t *record, uint8_t* input, size_t size)
{
    size_t len;
    dtls_record_header_t *header = (dtls_record_header_t*) input;

    if (size < DTLS_RECORD_HEADER_SIZE)
      return -1;

    len = header->length + DTLS_RECORD_HEADER_SIZE;
    if (len < size)
      return -1;

    memcpy(record, input, len);

    return len;
}


int dtls_record_listen_raw(dtls_connection_t *conn, void *buffer,
        size_t buffer_size)
{
    uint32_t len = sizeof(conn->socket_addr);
    return recvfrom(conn->socket, (void *)buffer, buffer_size, 0,
               (struct sockaddr *)&conn->socket_addr, &len);
}


int dtls_record_listen(dtls_connection_t *conn, int (*cb)(dtls_record_t*))
{
    uint8_t buffer[32];
    int recv_size;
    dtls_record_t record;

    if (conn->socket < 0)
      return 0;

    while (1)
    {
        recv_size = dtls_record_listen_raw(conn, buffer, 32);
        if (recv_size < 0) {
            printf("ERROR: recsize < 0!\n");
        }

        // Copy not used buffer bytes
        // Maybe use ringbuffer for incoming data
        dtls_record_read(&record, buffer, recv_size);
        cb(&record);
    }
}


int dtls_record_write(uint8_t* buffer, tls_content_type_t type,
      dtls_connection_t* conn, uint8_t *fragment, size_t size)
{
    dtls_record_header_t *header = (dtls_record_header_t*) buffer;

    if (size > DTLS_RECORD_MAX_SIZE)
        return DTLS_RECORD_ERR_LENGTH;

    if (pow(2,49) < conn->sequence_number)
        return DTLS_RECORD_ERR_SEQUENCE;

    memset(buffer, 0, DTLS_RECORD_HEADER_SIZE + size);

    header->type = type;
    header->version.major = DTLS_VERSION_MAJOR;
    header->version.minor = DTLS_VERSION_MINOR;
    header->epoch = conn->epoch;
    header->sequence_number.uint64 = conn->sequence_number;
    header->length = size;
    memcpy(buffer+DTLS_RECORD_HEADER_SIZE, fragment, size);

    return 0;
}


int dtls_record_compress(uint8_t *buffer, tls_compression_method_t meth)
{
    (void) buffer;
    (void) meth;

    printf("dtls_record_compress() not yet implemented");

    return 0;
}


int dtls_send_raw(dtls_connection_t* conn, uint8_t* data, size_t size)
{
    return sendto(conn->socket, data, size, 0,
           (struct sockaddr*) &conn->socket_addr, sizeof(conn->socket_addr));
}


int dtls_record_send(dtls_connection_t *conn, tls_content_type_t type, 
    uint8_t* input, size_t size)
{
    uint8_t buffer[32] = {0}; // ADJUST MAX_BUFFER SIZE, maybe dynamic

    // TLSPlaintext
    if (dtls_record_write(buffer, type, conn, input, size) < 0)
    {
        printf("Could not init DTLS RECORD PACKET\n");
        return -1;
    }

    // TLSCompressed - Compression
    tls_compression_method_t compression = TLS_COMPRESSION_NULL; 
    if(dtls_record_compress(buffer, compression) < 0)
        return -1;

    // TLSCiphertext - MAC calculation and Encryption
    //result = dtls_record_encrypt(&fragment, conn->cipher);
    //if(result < 0)
    //  return -1;

    if (dtls_send_raw(conn, buffer, size+DTLS_RECORD_HEADER_SIZE) < 0)
    {
        printf("Could not send data\n");
        return -1;
    }

    dtls_record_print(buffer);
    printf("TLS RECORD LAYER: Sent %d Bytes\n\n", size);

    ++conn->sequence_number;

    return 0;
}


int dtls_record_stream_send(dtls_connection_t *conn, tls_content_type_t type, 
    uint8_t* input, size_t size)
{
    int result;
    size_t len, offset = 0;

    while (offset < size)
    {
        len = size - offset;
        if (len > TLS_PLAINTEXT_MAX_SIZE)
            len = TLS_PLAINTEXT_MAX_SIZE;

        if(dtls_record_send(conn, type, input, len) < 0)
        {
            printf("Could not send record\n");
            return -1;
        }

        offset += len;
    }

    return 0;
}


void dtls_record_print(uint8_t *data)
{
    dtls_record_header_t *header = (dtls_record_header_t*) data;
    size_t size = header->length + DTLS_RECORD_HEADER_SIZE;

    printf("TLS RECORD PACKET\n");
    printf("\tHeader:\n");
    printf("\t\tType: %d\n", header->type);
    printf("\t\tVersion: %d %d\n", header->version.major, header->version.minor);
    printf("\t\tEpoch: %d\n", header->epoch);
    printf("\t\tSequence: %d\n", header->sequence_number.uint64);
    printf("\t\tLength: %d\n", header->length);
    printf("\tFragment:\n");

    for (uint32_t i=DTLS_RECORD_HEADER_SIZE; i < size; ++i)
        printf("\t\t%d: %d (%X)\n", i, data[i], data[i]);
}
