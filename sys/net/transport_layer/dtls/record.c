#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "ringbuffer.h"
#include "sys/socket.h"
#include "record.h"
#include "common.h"




int dtls_fragment_encrypt(dtls_connection_t *conn, uint8_t* data, size_t size)
{
    (void) conn;
    (void) data;
    (void) size;

    printf("dtls_record_encrypt() not yet implemented\n");

    return size;
}


int dtls_fragment_decrypt(dtls_connection_t *conn, uint8_t* data, size_t size)
{
    (void) conn;
    (void) data;
    (void) size;

    printf("dtls_record_decrypt() not yet implemented\n");

    return size;
}


int dtls_fragment_compress(dtls_connection_t *conn, uint8_t* data, size_t size)
{
    (void) conn;
    (void) data;
    (void) size;

    printf("dtls_record_compress() not yet implemented\n");

    return size;
}

int dtls_fragment_decompress(dtls_connection_t *conn, uint8_t* data, size_t size)
{
    (void) conn;
    (void) data;
    (void) size;

    printf("dtls_record_decompress() not yet implemented\n");

    return size;
}


int dtls_record_read(dtls_record_t *record, dtls_connection_t *conn, ringbuffer_t* rb)
{
    size_t len;

    // Copy Header
    if (rb->avail < DTLS_RECORD_HEADER_SIZE)
      return -1;
    ringbuffer_get(rb, (char*) &record->header, DTLS_RECORD_HEADER_SIZE);

    // Copy payload
    if (record->header.length < rb->avail)
      return -1;
    ringbuffer_get(rb, (char*) record->fragment, record->header.length);

    // DTLCompressed - Decryption and MAC verification
    len = dtls_fragment_decrypt(conn, record->fragment, record->header.length);
    if (len < 0)
        return DTLS_RECORD_ERR_DECRYPT;
    record->header.length = len;

    // DTLSPlaintext - Decompression
    len = dtls_fragment_decompress(conn, record->fragment, record->header.length);
    if (len < 0)
        return DTLS_RECORD_ERR_DECOMPRESSION;
    record->header.length = len;

    return len;
}


int dtls_record_receive_raw(dtls_connection_t *conn, void *buffer,
        size_t buffer_size)
{
    uint32_t len = sizeof(conn->socket_addr);
    return recvfrom(conn->socket, (void *)buffer, buffer_size, 0,
               (struct sockaddr *)&conn->socket_addr, &len);
}


int dtls_record_listen(dtls_connection_t *conn, dtls_record_cb_t cb)
{
    uint8_t buffer[DTLS_RECORD_HEADER_SIZE];
    ringbuffer_t rb;
    int recv_size, size;
    dtls_record_t record = {DTLS_RECORD_HEADER_INIT, buffer};

    if (conn->socket < 0)
      return -1;

    ringbuffer_init(&rb, buffer, DTLS_LISTEN_BUFFER_SIZE);

    while (1)
    {
        recv_size = dtls_record_receive_raw(conn, buffer, DTLS_LISTEN_BUFFER_SIZE);
        if (recv_size < 0) {
            printf("ERROR: recsize < 0!\n");
        }

        ringbuffer_add(&rb, buffer, recv_size);

        size = dtls_record_read(&record, conn, &rb);
        if (size < 0)
            continue;

        if(cb(conn, &record) < 0)
            return 0;
    }
}


int dtls_record_init(uint8_t* buffer, tls_content_type_t type,
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


int dtls_send_raw(dtls_connection_t* conn, uint8_t* data, size_t size)
{
    return sendto(conn->socket, data, size, 0,
           (struct sockaddr*) &conn->socket_addr, sizeof(conn->socket_addr));
}


int dtls_record_send(dtls_connection_t *conn, tls_content_type_t type, 
    uint8_t* data, size_t size)
{
    uint8_t buffer[32] = {0};
    int len;

    // DTLSPlaintext - Setup Header and copy data
    if (dtls_record_init(buffer, type, conn, data, size) < 0)
        return DTLS_RECORD_ERR_INIT;

    // DTLSCompressed - Compression
    len = dtls_fragment_compress(conn, buffer+DTLS_RECORD_HEADER_SIZE, size);
    if (len < 0)
        return DTLS_RECORD_ERR_COMPRESSION;

    // DTLSCiphertext - MAC calculation and Encryption
    len = dtls_fragment_encrypt(conn, buffer+DTLS_RECORD_HEADER_SIZE, len);
    if (len < 0)
        return DTLS_RECORD_ERR_ENCRYPT;

    if (dtls_send_raw(conn, buffer, len+DTLS_RECORD_HEADER_SIZE) < 0)
        return DTLS_RECORD_ERR_SEND;

    dtls_record_print(buffer);
    printf("TLS RECORD LAYER: Sent %d Bytes\n\n", len);

    ++conn->sequence_number;

    return DTLS_OK;
}


int dtls_record_stream_send(dtls_connection_t *conn, tls_content_type_t type, 
    uint8_t* input, size_t size)
{
    size_t len, offset = 0;

    while (offset < size)
    {
        len = size - offset;
        if (len > TLS_PLAINTEXT_MAX_SIZE)
            len = TLS_PLAINTEXT_MAX_SIZE;

        if(dtls_record_send(conn, type, input, len) < 0)
            return -1;

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
