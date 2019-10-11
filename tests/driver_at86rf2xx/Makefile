include ../Makefile.tests_common

# exclude boards with insufficient memory
BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove arduino-leonardo arduino-nano \
                             arduino-uno nucleo-f031k6

DISABLE_MODULE += auto_init

USEMODULE += od
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

FEATURES_REQUIRED += driver_at86rf2xx

# Use 'at86rf231' as fallback device
# Just copied the previous behavior of the test
FEATURES_OPTIONAL += driver_at86rf231

include $(RIOTBASE)/Makefile.include
