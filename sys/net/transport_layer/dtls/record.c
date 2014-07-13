#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "inet_pton.h"
#include "ringbuffer.h"
#include "sys/socket.h"
#include "dtls.h"
#include "ciphers.h"
#include "record.h"


int dtls_fragment_encrypt(dtls_connection_t *conn, dtls_record_t* record)
{
    return dtls_cipher_aes_128_ccm_8(conn, record, DTLS_CIPHER_ENCRYPT); 
}


int dtls_fragment_decrypt(dtls_connection_t *conn, dtls_record_t* record)
{
    return dtls_cipher_aes_128_ccm_8(conn, record, DTLS_CIPHER_DECRYPT); 
}


int dtls_fragment_compress(dtls_connection_t *conn, dtls_record_t* record)
{
    (void) conn;
    (void) record;

    //printf("dtls_record_compress() not yet implemented\n");

    return record->header->length;
}

int dtls_fragment_decompress(dtls_connection_t *conn, dtls_record_t* record)
{
    (void) conn;
    (void) record;

    //printf("dtls_record_decompress() not yet implemented\n");

    return record->header->length;
}


int dtls_record_read(dtls_record_t *record, dtls_connection_t *conn)
{
    int len;

    // Copy Header
    if (conn->buffer.avail < DTLS_RECORD_HEADER_SIZE)
      return -1;
    ringbuffer_get(&conn->buffer, (char*) record->header, DTLS_RECORD_HEADER_SIZE);

    // Copy payload - TODO: check max length
    if (record->header->length < conn->buffer.avail)
      return -1;
    ringbuffer_get(&conn->buffer, (char*) record->fragment, record->header->length);

    // DTLCompressed - Decryption and MAC verification
    len = dtls_fragment_decrypt(conn, record);
    if (len < 0)
        return DTLS_RECORD_ERR_DECRYPT;
    record->header->length = len;

    // DTLSPlaintext - Decompression
    len = dtls_fragment_decompress(conn, record);
    if (len < 0)
        return DTLS_RECORD_ERR_DECOMPRESSION;
    record->header->length = len;

    return len;
}


int dtls_record_receive_raw(dtls_connection_t *conn, void *buffer,
        size_t buffer_size)
{
    uint32_t len = sizeof(conn->socket_addr);

    return recvfrom(conn->socket, (void *)buffer, buffer_size, 0,
               (struct sockaddr *)&conn->socket_addr, &len);
}


int dtls_record_receive(dtls_connection_t *conn, dtls_record_t *record)
{
    uint8_t buffer[DTLS_RECORD_BUFFER_SIZE];
    int recv_size, size;
    char addr_str[IPV6_MAX_ADDR_STR_LEN];

    if (conn->socket < 0)
      return -1;

    recv_size = dtls_record_receive_raw(conn, buffer, DTLS_RECORD_BUFFER_SIZE);
    if (recv_size < 0) {
        printf("ERROR: recsize < 0!\n");
    }

    /** NDP BUG WORKAROUND START**/
    inet_ntop(AF_INET6, &conn->socket_addr.sin6_addr, addr_str,
            IPV6_MAX_ADDR_STR_LEN);
    ipv6_addr_t dest;
    inet_pton(AF_INET6, addr_str, &dest);
    printf("record - %d Bytes received from %s\n", recv_size, addr_str);
    if (!ndp_neighbor_cache_search(&dest)) {
        ndp_neighbor_cache_add(0, &dest, &(dest.uint16[7]), 2, 0,
                           NDP_NCE_STATUS_REACHABLE,
                           NDP_NCE_TYPE_TENTATIVE,
                           0xffff);
    }
    /** NDP BUG WORKAROUND END **/

    ringbuffer_add(&conn->buffer, buffer, recv_size);
    return dtls_record_read(record, conn);
}


int dtls_record_init(dtls_connection_t *conn, dtls_record_t* record,
                      tls_content_type_t type, uint8_t *data, size_t size)
{
    if (size > DTLS_RECORD_MAX_SIZE)
        return DTLS_RECORD_ERR_LENGTH;

    if (pow(2,49) < conn->sequence_number)
        return DTLS_RECORD_ERR_SEQUENCE;

    record->header->type = type;
    record->header->version.major = DTLS_VERSION_MAJOR;
    record->header->version.minor = DTLS_VERSION_MINOR;
    record->header->epoch = conn->epoch;
    record->header->sequence_number.uint64 = conn->sequence_number;
    record->header->length = size;

    memcpy(record->fragment, data, size);

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
    dtls_record_t record;
    uint8_t buffer[DTLS_RECORD_BUFFER_SIZE] = {0};
    int len;
    char addr_str[IPV6_MAX_ADDR_STR_LEN];

    record.header = (dtls_record_header_t*) buffer;
    record.fragment = buffer + DTLS_RECORD_HEADER_SIZE;

    // DTLSPlaintext - Setup Header and copy data
    if (dtls_record_init(conn, &record, type, data, size) < 0)
        return DTLS_RECORD_ERR_INIT;

    // DTLSCompressed - Compression
    len = dtls_fragment_compress(conn, &record);
    if (len < 0)
        return DTLS_RECORD_ERR_COMPRESSION;

    // DTLSCiphertext - MAC calculation and Encryption
    len = dtls_fragment_encrypt(conn, &record);
    if (len < 0)
        return DTLS_RECORD_ERR_ENCRYPT;

    inet_ntop(AF_INET6, &conn->socket_addr.sin6_addr, addr_str,
            IPV6_MAX_ADDR_STR_LEN);

    len += DTLS_RECORD_HEADER_SIZE;
    int err = dtls_send_raw(conn, buffer, len);
    if (err < 0) {
        return DTLS_RECORD_ERR_SEND;
    }

    inet_ntop(AF_INET6, &conn->socket_addr.sin6_addr, addr_str,
            IPV6_MAX_ADDR_STR_LEN);
    printf("record - %d Bytes sent to %s\n", len, addr_str);

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


void dtls_record_print(dtls_record_t *record)
{
    printf("TLS RECORD PACKET\n");
    printf("\tHeader:\n");
    printf("\t\tType: %d\n", record->header->type);
    printf("\t\tVersion: %d %d\n", record->header->version.major, record->header->version.minor);
    printf("\t\tEpoch: %d\n", record->header->epoch);
    printf("\t\tSequence: %d\n", record->header->sequence_number.uint64);
    printf("\t\tLength: %d\n", record->header->length);
    printf("\tFragment:\n");

    for (uint32_t i=0; i < record->header->length; ++i)
        printf("\t\t%d: %d (%X)\n", i, record->fragment[i], record->fragment[i]);
}
