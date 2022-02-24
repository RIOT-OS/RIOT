include ../Makefile.tests_common

TEST_ON_CI_WHITELIST += native

USEPKG += libcose
# crypto backend.
USEMODULE += libcose_crypt_riot
# USEMODULE += libcose_crypt_hacl
# USEMODULE += libcose_crypt_monocypher
# USEMODULE += libcose_crypt_tinycrypt
USEMODULE += memarray
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include
