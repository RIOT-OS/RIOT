#ifndef EXAMPLES_CRYPTO_H_
#define EXAMPLES_CRYPTO_H_

#include "kernel.h"

#define CIPHER_STR_ID_AES 313
#define CIPHER_STR_ID_3DES 367
#define CIPHER_STR_ID_TWOFISH 772
#define CIPHER_STR_ID_RC5 266
#define CIPHER_STR_ID_SKIPJACK 848

#define BLOCK_MODE_ECB 298
#define BLOCK_MODE_CBC 296
#define BLOCK_MODE_CTR 329
#define BLOCK_MODE_CCM 307

void shell_cipher_encrypt(int argc, char **argv);
void shell_cipher_decrypt(int argc, char **argv);

#endif
