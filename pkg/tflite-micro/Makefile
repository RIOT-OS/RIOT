PKG_NAME=tflite-micro
PKG_URL=https://github.com/tensorflow/tflite-micro
# sync from 2022.04.07
PKG_VERSION=1501b574b74fd7877aba30aa9d8b667f41b139c3
PKG_LICENSE=Apache 2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-cast-align
CFLAGS += -Wno-maybe-uninitialized
CFLAGS += -Wno-pedantic
CFLAGS += -Wno-unused-parameter

TFLITE_MODULES := \
    tflite-c \
    tflite-core-api \
    tflite-kernels \
    tflite-kernels-internal \
    tflite-kernels-internal-reference \
    tflite-micro \
    tflite-micro-kernels \
    tflite-micro-memory-planner \
    tflite-schema \
    #

DIR_tflite-c                            := tensorflow/lite/c
DIR_tflite-core-api                     := tensorflow/lite/core/api
DIR_tflite-kernels                      := tensorflow/lite/kernels
DIR_tflite-kernels-internal             := tensorflow/lite/kernels/internal
DIR_tflite-kernels-internal-reference   := tensorflow/lite/kernels/internal/reference
DIR_tflite-micro                        := tensorflow/lite/micro
DIR_tflite-micro-kernels                := tensorflow/lite/micro/kernels
DIR_tflite-micro-memory-planner         := tensorflow/lite/micro/memory_planner
DIR_tflite-schema                       := tensorflow/lite/schema

TFLITE_MODULES_USED := $(filter $(TFLITE_MODULES),$(USEMODULE))

all: $(TFLITE_MODULES_USED)
	@:

.PHONY: tflite-%

.PHONY: $(PKG_SOURCE_DIR)/tensorflow/lite/schema/schema_generated.h

$(PKG_SOURCE_DIR)/tensorflow/lite/schema/schema_generated.h: $(PKG_SOURCE_DIR)/tensorflow/lite/schema/schema.fbs $(FLATC)
	$(FLATC) --cpp -o "$(dir $@)" "$<"

tflite-%: $(PKG_SOURCE_DIR)/tensorflow/lite/schema/schema_generated.h
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(DIR_$@) -f $(CURDIR)/$@.mk

$(FLATC):
	$(Q)make -C "$(dir $@)"
