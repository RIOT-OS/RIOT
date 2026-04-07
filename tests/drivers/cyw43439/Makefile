BOARD ?= infineon-cy8ckit-062s2-ai

include ../Makefile.drivers_common

ifneq (,$(filter cortex-m4f,$(CPU_CORE)))
USEMODULE += periph_uart stdio_uart periph_gpio nimble_host psoc6_ble_nimble
USEMODULE += nimble_transport_hci_h4 nimble_host core_thread nimble_svc_gap nimble_svc_gatt
USEMODULE += nimble xtimer ztimer periph_timer periph_i2c bmi270
USEPKG += nimble

SRC += main.c gatt_svr.c

CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=8192
CFLAGS += -DTHREAD_STACKSIZE_MAIN=8192
CFLAGS += -DISR_STACKSIZE=2048

CFLAGS += -DMYNEWT_VAL_MSYS_1_BLOCK_COUNT=384
CFLAGS += -DMYNEWT_VAL_MSYS_1_BLOCK_SIZE=292

# HS runs natively, LL uses external UART H4 transport.
CFLAGS += -DMYNEWT_VAL_BLE_TRANSPORT_LL=0
CFLAGS += -DMYNEWT_VAL_BLE_TRANSPORT_LL__custom=1
CFLAGS += -DMYNEWT_VAL_BLE_TRANSPORT_LL__native=0
CFLAGS += -DMYNEWT_VAL_BLE_TRANSPORT_LL__psco6=0
CFLAGS += -DMYNEWT_VAL_BLE_TRANSPORT_HS=1
CFLAGS += -DMYNEWT_VAL_BLE_TRANSPORT_HS__native=1
CFLAGS += -DMYNEWT_VAL_BLE_TRANSPORT_ACL_FROM_LL_COUNT=192
CFLAGS += -DMYNEWT_VAL_BLE_TRANSPORT_ACL_FROM_HS_COUNT=64
CFLAGS += -DMYNEWT_VAL_BLE_TRANSPORT_EVT_COUNT=96
endif

ifneq (,$(filter cortex-m0plus,$(CPU_CORE)))
SRC = main.c
endif

include $(RIOTBASE)/Makefile.include
