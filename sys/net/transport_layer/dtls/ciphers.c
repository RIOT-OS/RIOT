#include <stdint.h>
#include <unistd.h>
#include "crypto/modes/ccm.h"
#include "dtls.h"
#include "record.h"
#include "ciphers.h"


int dtls_cipher_aes_128_ccm(dtls_connection_t *conn, dtls_record_t* record,
                            dtls_cipher_mode_t mode)
{
  uint8_t *adata = NULL, *nonce = NULL;
  int len, adata_len = 0, nonce_len = 0;

  /* TODO: Nonce - | IV (low order 32 bits) + epoch + seq_num  |*/

  if (mode == DTLS_CIPHER_ENCRYPT)
  {
    len = cipher_encrypt_ccm(&conn->cipher, adata, adata_len, 16, 2, nonce,
                             nonce_len, record->fragment, record->header->length,
                             record->fragment);
  } else {
    len = cipher_decrypt_ccm(&conn->cipher, adata, adata_len, 16, 2, nonce,
                             nonce_len, record->fragment, record->header->length,
                             record->fragment);
  }

  record->header->length = len;

  return len;
}


int dtls_cipher_aes_128_ccm_8(dtls_connection_t *conn, dtls_record_t* record,
                              dtls_cipher_mode_t mode)
{
  uint8_t *adata = NULL, *nonce = NULL;
  int len, adata_len = 0, nonce_len = 0;

  /* TODO: Nonce - | IV (low order 32 bits) + epoch + seq_num  |*/

  if (mode == DTLS_CIPHER_ENCRYPT)
  {
    len = cipher_encrypt_ccm(&conn->cipher, adata, adata_len, 8, 2, nonce,
                             nonce_len, record->fragment, record->header->length,
                             record->fragment);
    record->header->length = len;
  } else {
    len = cipher_decrypt_ccm(&conn->cipher, adata, adata_len, 8, 2, nonce,
                             nonce_len, record->fragment, record->header->length,
                             record->fragment);
    record->header->length = len;
  }

  return len;
}
