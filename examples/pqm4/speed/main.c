#include <stdio.h>
#include <string.h>

#include "xtimer.h"
#include "pq_api.h"
#include "api.h"
#include "randombytes.h"


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

static void printcycles(const char *s, xtimer_ticks32_t ticks)
{
  printf(s);
  printf("\n\tticks: %d\n", ticks.ticks32);
  printf("\tusec: %d\n", xtimer_usec_from_ticks(ticks));
}


int main(void)
{
#ifdef PQ_KEM
  printf("Running KEM speed test.\n\n");
  unsigned char ss[CRYPTO_BYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char ct[CRYPTO_CIPHERTEXTBYTES];
  xtimer_ticks32_t t0, t1;

  printf("==========================\n");

  // Key-pair generation
  t0 = xtimer_now();
  crypto_kem_keypair(pk, sk);
  t1 = xtimer_now();
  printcycles("keypair gen",  xtimer_diff(t1, t0));

  // Encapsulation
  t0 = xtimer_now();
  crypto_kem_enc(ct, ss, pk);
  t1 = xtimer_now();
  printcycles("encaps", xtimer_diff(t1, t0));

  // Decapsulation
  t0 = xtimer_now();
  crypto_kem_dec(ss, ct, sk);
  t1 = xtimer_now();
  printcycles("decaps", xtimer_diff(t1, t0));

  printf("All KEM tests run.\n\n");
#endif

#ifdef PQ_SIGN
  printf("Running Sign speed test.\n\n");
  unsigned char pk[CRYPTO_PUBLICKEYBYTES];
  unsigned char sk[CRYPTO_SECRETKEYBYTES];
  unsigned char sm[MLEN + CRYPTO_BYTES];
  unsigned char m[MLEN];

  unsigned long long mlen;
  unsigned long long smlen;
  xtimer_ticks32_t t0, t1;

  printf("==========================\n");
  // Key-pair generation
  t0 = xtimer_now();
  crypto_sign_keypair(pk, sk);
  t1 = xtimer_now();
  printcycles("keypair gen",  xtimer_diff(t1, t0));

  //Signing
  randombytes(m, MLEN);
  t0 = xtimer_now();
  crypto_sign(sm, &smlen, m, MLEN, sk);
  t1 = xtimer_now();
  printcycles("signing",  xtimer_diff(t1, t0));

  //Verification
  t0 = xtimer_now();
  if (crypto_sign_open(sm, &mlen, sm, smlen, pk)) {
    printf("Signature did not verify correctly!\n");
  }
  t1 = xtimer_now();
  printcycles("signing",  xtimer_diff(t1, t0));
#endif

  return 0;
}
