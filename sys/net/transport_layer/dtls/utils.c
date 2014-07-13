#include <unistd.h>
#include "crypto/hashes/sha256.h"
#include "dtls.h"
#include "random.h"
#include "handshake.h"


void dtls_version_init(tls_protocol_version_t* version)
{
    version->major = DTLS_VERSION_MAJOR;
    version->minor = DTLS_VERSION_MINOR;
}


int dtls_version_check(tls_protocol_version_t* version)
{
  return version->major == DTLS_VERSION_MAJOR &&
         version->minor == DTLS_VERSION_MINOR;
}


void dtls_random_init(dtls_random_t *random)
{
  timex_t now;

  vtimer_now(&now);
  random->gmt_unix_time = now.seconds + now.microseconds;

  genrand_init(random->gmt_unix_time);
  for (uint8_t i = 0; i < 28; i++)
  {
    random->random_bytes[i] = genrand_uint32();
  }
}


int dtls_random_check(dtls_random_t* random)
{
  /* TODO: implement check */
  return 0;
}


int dtls_cipher_suites_init(tls_cipher_suite_t **suites)
{
  size_t size = sizeof(dtls_cipher_suites);
  memcpy(suites, dtls_cipher_suites, size);
  return size;
}


int dtls_cipher_suites_check(tls_cipher_suite_t **suites)
{
  /* TODO: implement check */
  return 0;
}


int dtls_compression_methods_init(tls_compression_method_t **methods)
{
  size_t size = sizeof(dtls_compression_methods);
  memcpy(methods, dtls_compression_methods, size);
  return size;
}


int dtls_compression_methods_check(tls_compression_method_t **methods)
{
  /* TODO: implement check */
  return 0;
}


int dtls_cookie_init(dtls_connection_t *conn, uint8_t *data, size_t size,
                     uint8_t *secret, size_t secret_size)
{
  sha256_context_t sha256;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  /* HMAC(Secret, Client-IP, Client-Parameters) see RFC6347 page 17*/
  sha256_init(&sha256);
  sha256_update(&sha256, secret, secret_size);
  sha256_final(hash, &sha256);

  memcpy(data, hash, size);
}
