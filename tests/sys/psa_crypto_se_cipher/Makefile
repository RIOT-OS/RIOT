BOARD ?= nrf52840dk

include ../Makefile.sys_common

# Test fails systematically on the CI
TEST_ON_CI_BLACKLIST += esp32-wroom-32

USEMODULE += ztimer
USEMODULE += ztimer_usec

USEMODULE += psa_crypto

USEMODULE += psa_secure_element
USEMODULE += psa_secure_element_ateccx08a
USEMODULE += psa_secure_element_ateccx08a_cipher_aes_128


ifneq (1, $(TEST_KCONFIG))
  CFLAGS += -DCONFIG_PSA_PROTECTED_KEY_COUNT=1
endif

CFLAGS += -DSECURE_ELEMENT
CFLAGS += -DCUSTOM_ATCA_PARAMS

INCLUDES += -I$(APPDIR) # Application specific (not needed by PSA)

SHOULD_RUN_KCONFIG :=

include $(RIOTBASE)/Makefile.include
