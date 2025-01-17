include ../Makefile.sys_common

USEMODULE += ztimer
USEMODULE += ztimer_usec

USEMODULE += psa_crypto

USEMODULE += psa_hash
USEMODULE += psa_hash_sha3_256
USEMODULE += psa_hash_sha3_384
USEMODULE += psa_hash_sha3_512

USEMODULE += psa_hash_sha_224
USEMODULE += psa_hash_sha_256
USEMODULE += psa_hash_sha_384
USEMODULE += psa_hash_sha_512
USEMODULE += psa_hash_sha_512_224
USEMODULE += psa_hash_sha_512_256

include $(RIOTBASE)/Makefile.include

CFLAGS += -DTHREAD_STACKSIZE_MAIN=2048
