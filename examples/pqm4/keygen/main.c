#include <stdio.h>
#include <string.h>

#include "pq_api.h"
#include "api.h"
#include "randombytes.h"
#include <string.h>
#include "stdio.h"


#define NTESTS 1
#define MLEN 32

void cdump(const void* data, const char* name, size_t size) {
  size_t i;
  printf("static const unsigned char %s[%zu] = {\n\t", name, size);
  for (i = 0; i < size; ++i) {
    printf("0x%02X, ", ((unsigned char*)data)[i]);
    if ((i+1) % 8 == 0) {
      printf("\n\t");
      if (i+1 == size) {
        printf("\n");
      }
    }
  }
  printf("\n};\n\n");
}

#ifdef PQ_KEM
static int keygen_kem(void)
{
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];
  crypto_kem_keypair(pk, sk);

  printf("/* Post-quantum Key Encapsulation */\n\n");
  printf("/* Public key */\n" );
  cdump(pk, "public_key", CRYPTO_PUBLICKEYBYTES);
  printf("\n\n/* Secret key */\n");
  cdump(sk, "secret_key", CRYPTO_SECRETKEYBYTES);

  return 0;
}
#endif


#ifdef PQ_SIGN
int keygen_sign(void)
{
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];
  crypto_sign_keypair(pk, sk);
  printf("/* Post-quantum Signature Scheme*/\n\n");
  printf("/* Public key */\n");
  cdump(pk, "public_key", CRYPTO_PUBLICKEYBYTES);
  printf("\n\n/* Secret key */\n");
  cdump(sk, "secret_key", CRYPTO_SECRETKEYBYTES);
  return 0;
}
#endif


int main(void)
{

#ifdef PQ_KEM
  keygen_kem();
#endif

#ifdef PQ_SIGN
  keygen_sign();
#endif

  return 0;
}
