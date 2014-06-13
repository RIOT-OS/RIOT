#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "record.h"



int dtls_record_init(dtls_record_t *record, tls_content_type_t type,
   uint16_t epoch, uint64_t sequence_number, uint8_t *fragment, size_t size)
{
  if (size > DTLS_RECORD_MAX_SIZE)
    return -1;

  if (sequence_number > pow(2,49))
    return -1;

  memset(record, 0, sizeof(dtls_record_t));

  record->type = type;
  record->version.major = DTLS_VERSION_1_2_MAJOR;
  record->version.minor = DTLS_VERSION_1_2_MINOR;
  record->epoch = epoch;
  ASSIGN_UINT64_TO_UINT48(sequence_number, record->sequence_number);
  record->fragment = fragment;
  record->length = size;

  return 0;
}

int dtls_record_compress(dtls_record_t *fragment, tls_compression_method_t meth)
{
  (void) fragment;
  (void) meth;

  printf("dtls_record_compress() not yet implemented");

  return 0;
}


int dtls_record_stream_write(dtls_connection_t *conn, tls_content_type_t type, 
    uint8_t* input, size_t size)
{
  int result;
  size_t len, offset = 0;
  dtls_record_t fragment;

  while (offset < size)
  {
    len = size - offset;
    if (len > TLS_PLAINTEXT_MAX_SIZE)
      len = TLS_PLAINTEXT_MAX_SIZE;

    // TLSPlaintext
    result = dtls_record_init(&fragment, type, conn->epoch,
                 conn->sequence_number, input, len);
    if(result < 0)
      return -1;

    // TLSCompressed
    tls_compression_method_t compression = TLS_COMPRESSION_NULL; 
    result = dtls_record_compress(&fragment, compression);
    if(result < 0)
      return -1;

    // TLSCiphertext
    // MAC calculatin and Encryption
    //to add

    // send it! for now just print
    printf("\nnew TLS RECORD packet\nHeader:\n");
    for (uint8_t i=0; i < DTLS_RECORD_HEADER_SIZE; ++i) {
      uint8_t chr = ((uint8_t*)&fragment)[i];
      printf("\t%d: %d (%X)\n", i, chr, chr);
    }
    printf("Fragment:\n");
    for (uint64_t i=0; i < len; ++i) {
      printf("\t%" PRId64 ": %d (%X)\n", i, input[offset+i], input[offset+i]);
    }

    ++conn->sequence_number;
    offset += len;

  }

  return 0;
}
