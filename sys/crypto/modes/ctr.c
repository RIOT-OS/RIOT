#include "crypto/modes/ctr.h"
#include "crypto/modes/helper.h"

int cipher_encrypt_ctr(cipher_t* cipher, uint8_t nonce_counter[16],
                               uint8_t nonce_len, uint8_t* input, size_t length,
                               uint8_t* output)
{
    size_t offset = 0;
    uint8_t stream_block[16] = {0}, block_size_input, block_size;

    block_size = cipher_get_block_size(cipher);
    do {
        if (cipher_encrypt(cipher, nonce_counter, stream_block) != 1)
            return CIPHER_ERR_ENC_FAILED;

        block_size_input = (length - offset > block_size) ?
                                  block_size : length - offset;
        for (uint8_t i=0; i < block_size_input; ++i)
            output[offset+i] = stream_block[i] ^ input[offset + i];

        offset += block_size_input;
        block_inc_ctr(nonce_counter, block_size - nonce_len);
    } while (offset < length);

    return offset;
}

int cipher_decrypt_ctr(cipher_t* cipher, uint8_t nonce_counter[16],
                       uint8_t nonce_len, uint8_t* input, size_t length,
                       uint8_t* output)
{
    return cipher_encrypt_ctr(cipher, nonce_counter, nonce_len, input,
                                      length, output);
}
