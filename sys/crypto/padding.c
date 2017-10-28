#include "crypto/padding.h"
#include <string.h>

int32_t pkcs7_padding (uint8_t *data, uint32_t data_len, uint8_t blocksize, uint8_t *padded_data, uint32_t padded_data_len) {
  /* Validate the input data */
  if(blocksize <= 1) /* No need to check the maximum blocksize, since the max value supported by PKCS#7 is the same as that of the datatpye (255) */
    return PADDING_INVALID_BLOCKSIZE;
  if(padded_data_len < data_len)
    return PADDING_NOT_ENOUGH_SPACE_AVAILABLE;

  uint8_t padding_len = blocksize - (data_len % blocksize);

  if(padded_data_len < data_len + padding_len)
    return PADDING_NOT_ENOUGH_SPACE_AVAILABLE;

  /* Apply PKCS#7 padding */
  memcpy(padded_data, data, data_len);
  memset(padded_data+data_len, padding_len, padding_len);
  return data_len+padding_len;
}
