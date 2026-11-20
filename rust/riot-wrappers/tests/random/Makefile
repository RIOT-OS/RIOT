# name of your application
APPLICATION = riot-wrappers-test-random
BOARD ?= native
APPLICATION_RUST_MODULE = riot_wrappers_test_random

USEMODULE += random
# So that the RNG can be Default-constructed
FEATURES_REQUIRED += periph_hwrng
# So that the RNG also satisfies the CryptoRng requirements
USEMODULE += prng_sha256prng

include $(RIOTBASE)/Makefile.include
