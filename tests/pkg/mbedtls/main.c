/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test the correct loading and linking of the mbedtls package and
                execute built-in selftests.
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdbool.h>
#include <stdio.h>

#include "mbedtls/aes.h"
#include "mbedtls/aria.h"
#include "mbedtls/base64.h"
#include "mbedtls/bignum.h"
#include "mbedtls/camellia.h"
#include "mbedtls/ccm.h"
#include "mbedtls/cmac.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/des.h"
#include "mbedtls/dhm.h"
#include "mbedtls/ecjpake.h"
#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#include "mbedtls/gcm.h"
#include "mbedtls/hmac_drbg.h"
#include "mbedtls/md5.h"
#include "mbedtls/nist_kw.h"
#include "mbedtls/pkcs5.h"
#include "mbedtls/rsa.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"

#include "entropy_mbedtls_riot.h"
#include "modules.h"

/*
 *  Uncovered mbedtls self-tests due to missing modules which we do not supporting at the moment:
 *
 *  mbedtls_arc4_self_test
 *  mbedtls_chacha20_self_test
 *  mbedtls_chachapoly_self_test
 *  mbedtls_entropy_source_self_test
 *  mbedtls_md2_self_test
 *  mbedtls_md4_self_test
 *  mbedtls_memory_buffer_alloc_self_test
 *  mbedtls_poly1305_self_test
 *  mbedtls_ripemd160_self_test
 *  mbedtls_timing_self_test
 *  mbedtls_xtea_self_test
 */

int main(void)
{
    puts("mbedtls test\n");

#if IS_USED(MODULE_MBEDTLS_SHA256)
    mbedtls_sha256_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_SHA512)
    mbedtls_sha512_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_AES)
    mbedtls_aes_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_ARIA)
    mbedtls_aria_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_BASE64)
    mbedtls_base64_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_BIGNUM)
    mbedtls_mpi_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_CAMELLIA)
    mbedtls_camellia_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_CCM)
    mbedtls_ccm_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_CMAC)
    mbedtls_cmac_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_CTR_DRBG)
    mbedtls_ctr_drbg_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_DES)
    mbedtls_des_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_DHM)
    mbedtls_dhm_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_ECJPAKE)
    mbedtls_ecjpake_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_ECP)
    mbedtls_ecp_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_ENTROPY)
    bool adc_noise_single_entropy = false;
    /* NOTE, the following express needs adjustment when further entropy
       sources added by RIOT */
#if IS_USED(MODULE_MBEDTLS_ENTROPY_SOURCE_ADC) && !IS_USED(MODULE_MBEDTLS_ENTROPY_SOURCE_HWRNG)
    /* If the adc noise entropy source is compiled and is the only source
       utilized with the mbedtls entropy module, indicate it */
    adc_noise_single_entropy = true;
#endif
    /* Used by the testrunner to ignore potential test failures it the case of
       a single adc noise entropy source */
    printf("adc_noise_single_entropy: %d\n", adc_noise_single_entropy);

    /* Initializes available entropy sources */
    entropy_mbedtls_riot_init();

    /* Execute mbedtls built-in entropy tests*/
    mbedtls_entropy_self_test(1);

    /* Execute convenience entropy functions */
    unsigned char out[4];
    /* This function requires entropy_mbedtls_riot_init() beforehand */
    entropy_mbedtls_riot_retrieve(out, sizeof(out));
    /* Clears the internal context again */
    entropy_mbedtls_riot_uninit();

    /* This function does not require initialization */
    entropy_mbedtls_riot_get(out, sizeof(out));
#endif
#if IS_USED(MODULE_MBEDTLS_GCM)
    mbedtls_gcm_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_HMAC_DRBG)
    mbedtls_hmac_drbg_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_MD5)
    mbedtls_md5_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_NIST_KW)
    mbedtls_nist_kw_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_PKCS5)
    mbedtls_pkcs5_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_RSA)
    mbedtls_rsa_self_test(1);
#endif
#if IS_USED(MODULE_MBEDTLS_SHA1)
    mbedtls_sha1_self_test(1);
#endif
    return 0;
}
