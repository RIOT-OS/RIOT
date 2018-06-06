MODULE = nimble_tinycrypt

SRC += aes_decrypt.c
SRC += aes_encrypt.c
SRC += cmac_mode.c
SRC += ecc.c
SRC += ecc_dh.c
SRC += utils.c

include $(RIOTBASE)/Makefile.base
