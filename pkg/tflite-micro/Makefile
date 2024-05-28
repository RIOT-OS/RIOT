PKG_NAME=tflite-micro
PKG_URL=https://github.com/tensorflow/tflite-micro
# sync from 2024.05.21
PKG_VERSION=8e22946b3faa51564df5dd9194f7540b2694892c
PKG_LICENSE=Apache 2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-cast-align
CFLAGS += -Wno-maybe-uninitialized
CFLAGS += -Wno-pedantic
CFLAGS += -Wno-unused-parameter

TFLITE_MODULES := \
    tflite-c \
    tflite-core-api \
    tflite-core-c \
    tflite-kernels \
    tflite-kernels-internal \
    tflite-kernels-internal-reference \
    tflite-micro \
    tflite-micro-arena_allocator \
    tflite-micro-kernels \
    tflite-micro-memory-planner \
    tflite-micro-tflite_bridge \
    tflite-schema \
    #

DIR_tflite-c                            := tensorflow/lite/c
DIR_tflite-core-api                     := tensorflow/lite/core/api
DIR_tflite-core-c                       := tensorflow/lite/core/c
DIR_tflite-kernels                      := tensorflow/lite/kernels
DIR_tflite-kernels-internal             := tensorflow/lite/kernels/internal
DIR_tflite-kernels-internal-reference   := tensorflow/lite/kernels/internal/reference
DIR_tflite-micro                        := tensorflow/lite/micro
DIR_tflite-micro-arena_allocator        := tensorflow/lite/micro/arena_allocator
DIR_tflite-micro-kernels                := tensorflow/lite/micro/kernels
DIR_tflite-micro-memory-planner         := tensorflow/lite/micro/memory_planner
DIR_tflite-micro-tflite_bridge          := tensorflow/lite/micro/tflite_bridge
DIR_tflite-schema                       := tensorflow/lite/schema

TFLITE_MODULES_USED := $(filter $(TFLITE_MODULES),$(USEMODULE))

all: $(TFLITE_MODULES_USED)
	@:

.PHONY: tflite-%

$(PKG_SOURCE_DIR)/tensorflow/lite/schema/schema_generated.h: $(PKG_SOURCE_DIR)/tensorflow/lite/schema/schema.fbs $(FLATC)
	$(FLATC) --cpp --reflect-types --reflect-names --no-union-value-namespacing --gen-object-api -o "$(dir $@)" "$<"

tflite-%: $(PKG_SOURCE_DIR)/tensorflow/lite/schema/schema_generated.h
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/$(DIR_$@) -f $(CURDIR)/$@.mk

$(FLATC):
	$(Q)make -C "$(dir $@)"
