#include "test_utils.h"

int main(void)
{
    TESTS_START();
    psa_crypto_init();
    TESTS_RUN(tests_psa_import_and_seal_key());
    TESTS_RUN(tests_psa_generate_sealed_key());
    TESTS_RUN(tests_psa_aes_128_encrypt_decrypt());
    TESTS_RUN(tests_psa_hash_sha256_gen());
    TESTS_END();
    return 0;
}
