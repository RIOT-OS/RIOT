include ../Makefile.sys_common

USEMODULE += posix_headers
USEMODULE += pthread

# The test compiles, but these boards have too little RAM for
# dynamic allocation of thread stacks
BOARDS_UNSUPPORTED := arduino-nano arduino-duemilanove \
                   arduino-uno nucleo-f031k6 stm32f030f4-demo

include $(RIOTBASE)/Makefile.include
