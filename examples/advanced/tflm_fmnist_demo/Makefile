APPLICATION = tflm_fmnist_demo
BOARD ?= nrf52840dk
RIOTBASE ?= $(CURDIR)/../../..
FEATURES_REQUIRED += cpp
USEPKG += tflite-micro
USEMODULE += ztimer
CXXSRC += main.cpp
CXXSRC += model_data.cpp
CFLAGS  += -Wno-error=unused-parameter
CXXFLAGS += -Wno-error=unused-parameter
include $(RIOTBASE)/Makefile.include