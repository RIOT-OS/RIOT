include ../Makefile.sys_common

USEMODULE += cpp11-compat
USEMODULE += ztimer64_usec
USEMODULE += timex

include $(RIOTBASE)/Makefile.include

# Boards with little RAM (<=8KB) can't handle threads with the large default
# stack size. Since cpp11_thread allocates that at runtime, the allocation
# failure will either cause a panic (no exception supported) or exit with
# error (exceptions supported)
BOARDS_UNSUPPORTED := \
    nucleo-c031c6 \
    nucleo-f042k6 \
    nucleo-g031k8 \
    nucleo-l031k6 \
    stm32g0316-disco \
    weact-g030f6 \
    #
