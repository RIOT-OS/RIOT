#include "test_utils.h"

int main(void)
{
    TESTS_START();
    psa_crypto_init();
    // TESTS_RUN(tests_psa_import_key());
    TESTS_RUN(tests_psa_generate_sealed_key());
    TESTS_END();
    return 0;
}
