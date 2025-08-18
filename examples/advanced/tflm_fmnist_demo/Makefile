APPLICATION = tflm_fmnist_demo
BOARD ?= nrf52840dk
RIOTBASE ?= $(CURDIR)/../../..

FEATURES_REQUIRED += cpp
USEPKG += tflite-micro
USEMODULE += xtimer

# ✅ List ALL C++ sources here
CXXSRC += main.cpp
CXXSRC += model_data.cpp

# Keep TFLM warnings non-fatal on MCUs
WERROR ?= 0

include $(RIOTBASE)/Makefile.include
