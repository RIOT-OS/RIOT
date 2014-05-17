export PROJECT = crypto-shell
export BOARD ?= native
export RIOTBASE ?= $(CURDIR)/../..

CFLAGS += -DDEVELHELP -DNATIVE_AUTO_EXIT

# Change this to 0 show compiler invocation lines by default:
export QUIET ?= 0

# Modules to include:

USEMODULE += posix
USEMODULE += uart0
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += vtimer
USEMODULE += ltc4150
USEMODULE += rtc
USEMODULE += config
USEMODULE += random
USEMODULE += hashes
USEMODULE += crypto_ciphers_aes
USEMODULE += crypto_ciphers_3des
USEMODULE += crypto_ciphers_rc5
USEMODULE += crypto_ciphers_skipjack
USEMODULE += crypto_ciphers_twofish
USEMODULE += crypto_ciphers
USEMODULE += crypto_modes

include $(RIOTBASE)/Makefile.include
