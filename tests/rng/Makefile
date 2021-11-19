include ../Makefile.tests_common

# Default stack with printf + the tests buffer uint32_t[32]/uint8_t[256]
MAIN_THREAD_SIZE = THREAD_STACKSIZE_DEFAULT+THREAD_EXTRA_STACKSIZE_PRINTF+256
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\($(MAIN_THREAD_SIZE)\)

# override PRNG if desired (see sys/random for alternatives)
# USEMODULE += prng_minstd

USEMODULE += fmt
USEMODULE += random
USEMODULE += random_cmd
USEMODULE += shell
USEMODULE += xtimer

FEATURES_OPTIONAL += periph_hwrng

TEST_ON_CI_WHITELIST += native

include $(RIOTBASE)/Makefile.include
