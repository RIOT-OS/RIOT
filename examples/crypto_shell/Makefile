export PROJECT = crypto-shell
export BOARD ?= native
export RIOTBASE ?= $(CURDIR)/../..

CFLAGS += -DDEVELHELP -DNATIVE_AUTO_EXIT

# calloc does not exist in these devices, see issue #1061
BOARD_BLACKLIST := chronos msb-430 msb-430h \
                   telosb wsn430-v1_3b wsn430-v1_4 z1 \
                   arduino-due mbed_lpc1768 redbee-econotag udoo


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
