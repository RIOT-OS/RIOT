#
# Copyright (C) 2020-2021 Université Grenoble Alpes, ANS Innovation
#

include ../Makefile.tests_common

BOARD_WHITELIST := esp32-wroom-32 thingsat-l0 nucleo-l053r8 nucleo-l073rz nucleo-f411re nucleo-f401re nucleo-f446re nucleo-f446ze nucleo-f103rb olimexino-stm32
# BOARD_WHITELIST += esp32-wroom-32 lora-e5-dev bluepill bluepill-128kib


ifeq ($(BOARD),esp32-wroom-32)
SX1280_MODULE = lambda80_mikrobus0
endif

USEMODULE += sx1280
# USEMODULE += hashes
# USEMODULE += crypto

USEMODULE += shell
USEMODULE += shell_commands

USEMODULE += fmt
USEMODULE += printf_float

USEMODULE += xtimer
USEMODULE += random


# -----------------------------
# sx1280_utils
# -----------------------------

ifneq ($(BOARD),esp32-wroom-32) 
CFLAGS += -DISR_STACKSIZE=2048U
endif


ifdef DEVADDR
CFLAGS += -DDEVADDR=$(DEVADDR)
endif

ifdef FPORT
CFLAGS += -DFPORT=$(FPORT)
endif

ifdef RECEIVE_WINDOWS_IN_SECONDS
CFLAGS += -DRECEIVE_WINDOWS_IN_SECONDS=$(RECEIVE_WINDOWS_IN_SECONDS)
endif

ifdef RECEIVE_WINDOWS_TIMEOUT_IN_SECONDS
CFLAGS += -DRECEIVE_WINDOWS_TIMEOUT_IN_SECONDS=$(RECEIVE_WINDOWS_TIMEOUT_IN_SECONDS)
endif

ifdef APPSKEY
CFLAGS += -DAPPSKEY=$(APPSKEY)
endif

ifdef NWKSKEY
CFLAGS += -DNWKSKEY=$(NWKSKEY)
endif

# -----------------------------
# Default Frequency and parameters
# -----------------------------
CFLAGS += -DMULTITECH_ISM2400_PARAMS=1

ifdef RANGETEST
CFLAGS += -DRANGETEST=$(RANGETEST)
endif

# -----------------------------
# For TinyGS 2G4
# See https://github.com/thingsat/tinygs_2g4station/blob/main/Firmware/Arduino/SX1280/TinyGS_2G4_LoRa_Receiver_Detailed_Setup/SettingsModule.h
# -----------------------------

ifeq ($(BOARD),esp32-wroom-32)

ifeq ($(SX1280_MODULE),lambda80_mikrobus0)
# SPI CS
CFLAGS += -DSX1280_PARAM_SPI_NSS=GPIO5
# Optional RESET
CFLAGS += -DSX1280_PARAM_RESET=GPIO32
# Busy PIN
CFLAGS += -DSX1280_PARAM_DIO0=GPIO26
# IRQ
CFLAGS += -DSX1280_PARAM_DIO1=GPIO33
endif

ifeq ($(SX1280_MODULE),lambda80_mikrobus1)
# SPI CS
CFLAGS += -DSX1280_PARAM_SPI_NSS=GPIO5
# Optional RESET
CFLAGS += -DSX1280_PARAM_RESET=GPIO14
# Busy PIN
CFLAGS += -DSX1280_PARAM_DIO0=GPIO13
# IRQ
CFLAGS += -DSX1280_PARAM_DIO1=GPIO27
endif

ifeq ($(SX1280_MODULE),e28_mikrobus0)
# SPI CS
CFLAGS += -DSX1280_PARAM_SPI_NSS=GPIO5
# Optional RESET
CFLAGS += -DSX1280_PARAM_RESET=GPIO32
# Busy PIN
CFLAGS += -DSX1280_PARAM_DIO0=GPIO33
# IRQ
CFLAGS += -DSX1280_PARAM_DIO1=GPIO26
endif

ifeq ($(SX1280_MODULE),e28_mikrobus1)
# SPI CS
CFLAGS += -DSX1280_PARAM_SPI_NSS=GPIO5
# Optional RESET
CFLAGS += -DSX1280_PARAM_RESET=GPIO14
# Busy PIN
CFLAGS += -DSX1280_PARAM_DIO0=GPIO27
# IRQ
CFLAGS += -DSX1280_PARAM_DIO1=GPIO13
endif

endif


# -----------------------------
# Flags
# -----------------------------
# Should be removed for production
#CFLAGS += -Wno-unused-variable
#CFLAGS += -Wno-unused-function
#CFLAGS += -Wno-error=unused-parameter
#CFLAGS += -Wno-sign-compare
#CFLAGS += -Wno-multistatement-macros

include $(RIOTBASE)/Makefile.include
