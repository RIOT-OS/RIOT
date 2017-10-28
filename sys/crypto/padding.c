#include "crypto/padding.h"
#include <string.h>

int32_t pkcs7_padding (uint8_t *data, uint32_t data_len, uint8_t blocksize, uint8_t *padded_data, uint32_t padded_data_len) {
  /* Validate the input data */
  if(blocksize <= 1) /* No need to check the maximum blocksize, since the max value supported by PKCS#7 is the same as that of the datatpye (255) */
    return PADDING_INVALID_BLOCKSIZE;
  if(padded_data_len < data_len)
    return PADDING_NOT_ENOUGH_SPACE_AVAILABLE;
  if(data_len + blocksize>= INT32_MAX)
    /* We cannot handle that long, since we need to return the padded length in a int32_t */
    return PADDING_DATA_TOO_LONG;

  uint8_t padding_len = blocksize - (data_len % blocksize);

  if(padded_data_len < data_len + padding_len)
    return PADDING_NOT_ENOUGH_SPACE_AVAILABLE;

  /* Apply PKCS#7 padding */
  memcpy(padded_data, data, data_len);
  memset(padded_data+data_len, padding_len, padding_len);
  return data_len+padding_len;
}

int32_t pkcs7_padding_remove (uint8_t *data, uint32_t data_len, uint8_t blocksize, uint8_t *unpadded_data, uint32_t unpadded_data_len) {
  /* Validate the input data */
  if(blocksize <= 1) /* No need to check the maximum blocksize, since the max value supported by PKCS#7 is the same as that of the datatpye (255) */
    return PADDING_INVALID_BLOCKSIZE;
  if(data_len % blocksize != 0)
      return PADDING_INVALID_PADDING;
  if(data_len >= INT32_MAX)
    /* We cannot handle that long, since we need to return the unpadded length in a int32_t */
    return PADDING_DATA_TOO_LONG;
  
  /* Length of the padding is determined by the last byte */
  uint8_t padding_len = data[data_len - 1];
  /* Check if the padding is correct */
  for(uint8_t i = data_len-1; i >= data_len-padding_len;--i)
    if(data[i] != padding_len)
      return PADDING_INVALID_PADDING;

  uint32_t unpadded_length = data_len - padding_len;
 
  if(unpadded_data_len < unpadded_length)
    return PADDING_NOT_ENOUGH_SPACE_AVAILABLE;

  /* Remove the padding */
  memcpy(unpadded_data, data, unpadded_length);
  return unpadded_length;
}
