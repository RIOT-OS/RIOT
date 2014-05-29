export PROJECT = crypto-shell
export BOARD ?= native
export RIOTBASE ?= $(CURDIR)/../..

CFLAGS += -DDEVELHELP -DNATIVE_AUTO_EXIT

BOARD_INSUFFICIENT_RAM := chronos msb-430h redbee-econotag \
                          telosb wsn430-v1_3b wsn430-v1_4 z1
BOARD_BLACKLIST := arduino-due mbed_lpc1768 msb-430 pttu udoo qemu-i386
# mbed_lpc1768: see https://github.com/RIOT-OS/RIOT/issues/675
# msb-430: see https://github.com/RIOT-OS/RIOT/issues/658
# pttu: see https://github.com/RIOT-OS/RIOT/issues/659
# qemu-i386: no tranceiver, yet

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
