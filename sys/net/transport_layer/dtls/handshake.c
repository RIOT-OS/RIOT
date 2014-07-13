#include "dtls.h"
#include "utils.h"
#include "record.h"
#include "handshake.h"


// Packet structure: RFC6347 Page 16
int dtls_handshake_client_hello_init(dtls_connection_t *conn, uint8_t *data,
                                     uint8_t *cookie, size_t cookie_size)
{
  uint8_t *start;

  start = data;
  dtls_version_init((tls_protocol_version_t*) data);
  data += sizeof(tls_protocol_version_t);

  dtls_random_init((dtls_random_t*) data);
  data += sizeof(dtls_random_t);

  /* set session id to zero; it's not supported yet */
  *data++ = 0;

  data += dtls_cipher_suites_init((tls_cipher_suite_t **) data);
  data += dtls_compression_methods_init((tls_compression_method_t **) data);

  return data - start;
}


int dtls_handshake_client_hello_verify(dtls_connection_t* conn, uint8_t *data,
                                       size_t size)
{
  dtls_random_t *random;
  int cipher, compression;

  if (size < DTLS_HANDSHAKE_CLIENT_HELLO_SIZE_MIN) {
    return -1;
  }

  if (dtls_version_check((tls_protocol_version_t*) data) < 0)
    return -1;
  data += sizeof(tls_protocol_version_t);

  random = (dtls_random_t*) data;
  if (dtls_random_check(random) < 0)
    return -1;

  cipher = dtls_cipher_suites_check((tls_cipher_suite_t **) data);
  if (cipher < 0)
    return -1;

  compression = dtls_compression_methods_check((tls_compression_method_t **) data);
  if (compression < 0)
    return -1;

  /* data seems fine - save it*/
  //memcpy(conn->last_timestamp, data->gmt_unix_time,
  //      sizeof(dtls_random_t.gmt_unix_time));
  memcpy(conn->client_random, random->random_bytes, 28);
  memcpy(conn->cipher_suite, dtls_cipher_suites[cipher],
        sizeof(tls_cipher_suite_t));
  conn->compression = dtls_compression_methods[compression];

  printf("Handshake - ClientHello received\n");

  return DTLS_HANDSHAKE_HELLO_VERIFY_REQUEST;
}


int dtls_handshake_hello_verify_init(dtls_connection_t *conn, uint8_t *data)
{
  uint8_t *start;

  start = data;
  /* TODO: fix version - initial handshake has to be TLS 1.0 see rfc6347 page 16*/
  dtls_version_init((tls_protocol_version_t*) data);
  data += sizeof(tls_protocol_version_t);

  dtls_cookie_init(conn, data, 8, "secret", strlen("secret"));
  data += 8;

  return data - start;
}


int dtls_handshake_verify_state(dtls_connection_t *conn,
                                dtls_handshake_type_t expected,
                                dtls_record_t *record)
{
  size_t size;
  int state_next = -1, err;

  switch(expected)
  {
    case DTLS_HANDSHAKE_CLIENT_HELLO:
      state_next = dtls_handshake_client_hello_verify(conn, record->fragment,
                                                      record->header->length);
      /* reuse buffer memory of fragment to save memory consumption */
      size = dtls_handshake_hello_verify_init(conn, record->fragment);
      err = dtls_record_send(conn, TLS_CONTENT_TYPE_HANDSHAKE,
                             record->fragment, size); 
      if (err < 0) {
        printf("err: %d\n", err);
      } else {
        printf("Handshake - HelloVerify sent\n");
      }

      break;
  }

  return state_next;
}

int dtls_handshake_verify(dtls_connection_t *conn, dtls_record_t *record,
                          dtls_handshake_type_t expected)
{
  int state;

  while(1) {
    if (dtls_record_receive(conn, record) >= 0)
    {
      state = dtls_handshake_verify_state(conn, expected, record);
      if (state < 0) {
        printf("Handshake failed!\n");
        return -1;
      }

      expected = state;

      if (state == DTLS_HANDSHAKE_FINISHED)
        break;
    }
  }

  printf("Handshake successful!\n");

  return 0;
}


int dtls_handshake_start(dtls_connection_t *conn, dtls_record_t *record)
{
  size_t len = 0;
  int err, expected = DTLS_HANDSHAKE_HELLO_VERIFY_REQUEST;

  /* Send ClientHello; use memory of record to lower memory consumption*/
  len = dtls_handshake_client_hello_init(conn, record->fragment, NULL, 0);
  dtls_record_send(conn, TLS_CONTENT_TYPE_HANDSHAKE, record->fragment, len); 

  printf("Handshake - ClientHello sent\n");

  err = dtls_handshake_verify(conn, record, DTLS_HANDSHAKE_HELLO_VERIFY_REQUEST);
  if (err < 0) {
    return -1;
  }

  printf("Handshake successful!\n");
  return 0;
}
