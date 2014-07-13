#include "dtls.h"
#include "record.h"

int dtls_change_cipher(dtls_connection_t *conn)
{
  uint8_t data[] = {1};
  cipher_init(&conn->cipher, CIPHER_AES_128, (uint8_t*) "asd", 3);

  if (dtls_record_send(conn, TLS_CONTENT_TYPE_CHANGE_CIPHER_SPEC, data, 1) < 0)
  {
    return -1;
  }

  return 1;
}
