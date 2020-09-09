MODULE = mnist

SRCXXEXT = cc

CXXEXFLAGS += -Wno-unused-parameter
CXXEXFLAGS += -Wno-type-limits

CFLAGS += -Wno-pedantic

# Add the tensorflow lite quantized model as a blob
BLOBS += model.tflite

# Add the input digit image as blob
BLOBS += digit

include $(RIOTBASE)/Makefile.base
