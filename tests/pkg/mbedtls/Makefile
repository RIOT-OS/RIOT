include ../Makefile.pkg_common

USEPKG += mbedtls
USEMODULE += mbedtls_self_test
# enable RIOT mbedtls contrib modules
USEMODULE += mbedtls_crypto
USEMODULE += mbedtls_entropy
USEMODULE += mbedtls_random
USEMODULE += mbedtls_ssl
USEMODULE += mbedtls_theading
USEMODULE += mbedtls_x509

#USEMODULE += mbedtls_aes
#USEMODULE += mbedtls_aria
#USEMODULE += mbedtls_base64
#USEMODULE += mbedtls_bignum
#USEMODULE += mbedtls_camellia
#USEMODULE += mbedtls_ccm
#USEMODULE += mbedtls_cmac
#USEMODULE += mbedtls_ctr_drbg
#USEMODULE += mbedtls_des
#USEMODULE += mbedtls_dhm
ifneq (,$(filter mbedtls_dhm,$(USEMODULE)))
# self test of DHM needs mbadtls_asn1_parse
  USEMODULE += mbedtls_asn1_parse
endif
#USEMODULE += mbedtls_ecjpake
#USEMODULE += mbedtls_ecp
#USEMODULE += mbedtls_gcm
#USEMODULE += mbedtls_hmac_drbg
ifneq (,$(filter mbedtls_hmac_drbg,$(USEMODULE)))
# self test of HMAC DRBG needs SHA1
  USEMODULE += mbedtls_sha1
endif
#USEMODULE += mbedtls_md5
#USEMODULE += mbedtls_nist_kw
#USEMODULE += mbedtls_pkcs5
ifneq (,$(filter mbedtls_pkcs5,$(USEMODULE)))
# self test of PKCS5 needs SHA1
  USEMODULE += mbedtls_sha1
endif
#USEMODULE += mbedtls_rsa
ifneq (,$(filter mbedtls_rsa,$(USEMODULE)))
# self test (needs) PKCS1 v1.5
  USEMODULE += mbedtls_pkcs1_v15
endif
#USEMODULE += mbedtls_sha1

# Increase stack size of main thread.
CFLAGS += -DTHREAD_STACKSIZE_MAIN=2*THREAD_STACKSIZE_LARGE
# mbedtls_*_self_test() may have __attribute__((warn_unused_result))
CFLAGS += -Wno-unused-result

# You can disable single entropy source to be used with the entropy module
# DISABLE_MODULE += mbedtls_entropy_source_hwrng

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter mbedtls_entropy_source_adc,$(USEMODULE)))
  ifndef CONFIG_KCONFIG_USEMODULE_ENTROPY_SOURCE_ADC_NOISE
    # This is an exemplary entropy value of 1 bit/sample provided by
    # the ADC noise source. To avoid float, this value has to be
    # multiplied by 65536 beforehand.
    CFLAGS += -DCONFIG_ENTROPY_SOURCE_ADC_HMIN=65536
  endif
endif
