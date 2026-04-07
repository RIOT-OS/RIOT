COINES_INSTALL_PATH ?= ../../../..

EXAMPLE_FILE ?= fifo_full_headerless_mode.c

API_LOCATION ?= ../..

COMMON_LOCATION ?= ..

C_SRCS += \
$(API_LOCATION)/bmi2.c \
$(API_LOCATION)/bmi270_maximum_fifo.c \
$(COMMON_LOCATION)/common/common.c

INCLUDEPATHS += \
$(API_LOCATION) \
$(COMMON_LOCATION)/common

TARGET = MCU_APP30

include $(COINES_INSTALL_PATH)/coines.mk