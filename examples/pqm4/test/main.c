#include <stdio.h>
#include <string.h>

#include "pq_api.h"
#include "api.h"
#include "randombytes.h"
#include <string.h>
#include "stdio.h"


#define NTESTS 1
#define MLEN 32

void hexdump(const void* data, size_t size) {
  char ascii[17];
  size_t i, j;
  ascii[16] = '\0';
  for (i = 0; i < size; ++i) {
    printf("%02X ", ((unsigned char*)data)[i]);
    if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
      ascii[i % 16] = ((unsigned char*)data)[i];
    } else {
      ascii[i % 16] = '.';
    }
    if ((i+1) % 8 == 0 || i+1 == size) {
      printf(" ");
      if ((i+1) % 16 == 0) {
        printf("|  %s \n", ascii);
      } else if (i+1 == size) {
        ascii[(i+1) % 16] = '\0';
        if ((i+1) % 16 <= 8) {
          printf(" ");
        }
        for (j = (i+1) % 16; j < 16; ++j) {
          printf("   ");
        }
        printf("|  %s \n", ascii);
      }
    }
  }
}

#ifdef PQ_KEM
static int test_keys(void)
{
  unsigned char key_a[CRYPTO_BYTES], key_b[CRYPTO_BYTES];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sendb[CRYPTO_CIPHERTEXTBYTES];
  unsigned char sk_a[CRYPTO_SECRETKEYBYTES];
  int i;

  for(i=0; i<NTESTS; i++)
  {
    //Alice generates a public key
    crypto_kem_keypair(pk, sk_a);
    printf("PASS key pair generation!\n");

    //Bob derives a secret key and creates a response
    crypto_kem_enc(sendb, key_b, pk);
    printf("PASS encapsulation!\n");

    //Alice uses Bobs response to get her secret key
    crypto_kem_dec(key_a, sendb, sk_a);
    printf("PASS decapsulation!\n");

    if(memcmp(key_a, key_b, CRYPTO_BYTES)){
      printf("ERROR KEYS\n");
    }
    printf("Key A:\n");
    hexdump(key_a, CRYPTO_BYTES);
    printf("Key B:\n");
    hexdump(key_b, CRYPTO_BYTES);
  }

  return 0;
}


static int test_invalid_sk_a(void)
{
  unsigned char sk_a[CRYPTO_SECRETKEYBYTES];
  unsigned char key_a[CRYPTO_BYTES], key_b[CRYPTO_BYTES];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sendb[CRYPTO_CIPHERTEXTBYTES];
  int i;

  for(i=0; i<NTESTS; i++)
  {
    //Alice generates a public key
    crypto_kem_keypair(pk, sk_a);

    //Bob derives a secret key and creates a response
    crypto_kem_enc(sendb, key_b, pk);

    //Replace secret key with random values
    randombytes(sk_a, CRYPTO_SECRETKEYBYTES);

    //Alice uses Bobs response to get her secre key
    crypto_kem_dec(key_a, sendb, sk_a);

    if(!memcmp(key_a, key_b, CRYPTO_BYTES))
      printf("ERROR invalid sk_a\n");
  }

  return 0;
}


static int test_invalid_ciphertext(void)
{
  unsigned char sk_a[CRYPTO_SECRETKEYBYTES];
  unsigned char key_a[CRYPTO_BYTES], key_b[CRYPTO_BYTES];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sendb[CRYPTO_CIPHERTEXTBYTES];
  int i;
  size_t pos;

  for(i=0; i<NTESTS; i++)
  {
    randombytes((unsigned char *)&pos, sizeof(size_t));

    //Alice generates a public key
    crypto_kem_keypair(pk, sk_a);

    //Bob derives a secret key and creates a response
    crypto_kem_enc(sendb, key_b, pk);

    //Change some byte in the ciphertext (i.e., encapsulated key)
    sendb[pos % CRYPTO_CIPHERTEXTBYTES] ^= 23;

    //Alice uses Bobs response to get her secret key
    crypto_kem_dec(key_a, sendb, sk_a);

    if(!memcmp(key_a, key_b, CRYPTO_BYTES))
      printf("ERROR invalid ciphertext\n");
  }

  return 0;
}
#endif


#ifdef PQ_SIGN
int test_sign(void)
{
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];
    unsigned char sm[MLEN + CRYPTO_BYTES];
    unsigned char m[MLEN];

    unsigned long long mlen;
    unsigned long long smlen;

    //int i;

    //for (i = 0; i < NTESTS; i++) {
        crypto_sign_keypair(pk, sk);
        printf("crypto_sign_keypair DONE.\n");

        randombytes(m, MLEN);
        crypto_sign(sm, &smlen, m, MLEN, sk);
        printf("crypto_sign DONE.\n");

        // By relying on m == sm we prevent having to allocate CRYPTO_BYTES twice
        if (crypto_sign_open(sm, &mlen, sm, smlen, pk)) {
            printf("Signature did not verify correctly!\n");
        }
        printf("crypto_sign_open DONE.\n");
    //}

    return 0;
}

int test_wrong_pk(void)
{
    unsigned char pk[CRYPTO_PUBLICKEYBYTES];
    unsigned char pk2[CRYPTO_PUBLICKEYBYTES];
    unsigned char sk[CRYPTO_SECRETKEYBYTES];
    unsigned char sm[MLEN + CRYPTO_BYTES];
    unsigned char m[MLEN];

    unsigned long long mlen;
    unsigned long long smlen;

    int i;

    for (i = 0; i < NTESTS; i++) {
        crypto_sign_keypair(pk2, sk);
        printf("crypto_sign_keypair DONE.\n");

        crypto_sign_keypair(pk, sk);
        printf("crypto_sign_keypair DONE.\n");


        randombytes(m, MLEN);
        crypto_sign(sm, &smlen, m, MLEN, sk);
        printf("crypto_sign DONE.\n");

        // By relying on m == sm we prevent having to allocate CRYPTO_BYTES twice
        if (crypto_sign_open(sm, &mlen, sm, smlen, pk2)) {
            printf("OK Signature did not verify correctly under wrong public key!\n");
        } else {
            printf("ERROR Signature did verify correctly under wrong public key!\n");
        }
        printf("crypto_sign_open DONE.\n");
    }

    return 0;
}
#endif


int main(void)
{

#ifdef PQ_KEM
  printf("Running KEM tests.\n\n");
  test_keys();
  test_invalid_sk_a();
  test_invalid_ciphertext();
  printf("All KEM tests run.\n\n");
#endif

#ifdef PQ_SIGN
  printf("Running SIGN tests.\n\n");
  test_sign();
  test_wrong_pk();
  printf("All SIGN tests run.\n\n");
#endif

  return 0;
}
