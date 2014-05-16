export PROJECT = test_bloom
include ../Makefile.tests_common

BOARD_BLACKLIST := chronos msb-430 msb-430h \
                   telosb wsn430-v1_3b wsn430-v1_4 z1
# chronos: https://github.com/RIOT-OS/RIOT/issues/1061
# msb-430: https://github.com/RIOT-OS/RIOT/issues/1061
# msb-430h: https://github.com/RIOT-OS/RIOT/issues/1061
# telosb: https://github.com/RIOT-OS/RIOT/issues/1061
# wsn430-v1_3b: https://github.com/RIOT-OS/RIOT/issues/1061
# wsn430-v1_4: https://github.com/RIOT-OS/RIOT/issues/1061
# z1: https://github.com/RIOT-OS/RIOT/issues/1061

USEMODULE += hashes
USEMODULE += bloom
USEMODULE += random

DISABLE_MODULE += auto_init

include $(RIOTBASE)/Makefile.include
