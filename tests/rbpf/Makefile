include ../Makefile.tests_common

BLOBS += rbpf/fletcher32_rbpf.bin

USEMODULE += embunit
USEMODULE += rbpf

pkg-prepare: rbpf/$(RBPF_BINS)

RBPF_SOURCES ?= $(wildcard rbpf/*.c)
RBPF_BINS = $(RBPF_SOURCES:.c=.bin)

$(RBPF_BINS): %.bin:%.c
	$(MAKE) -C $(CURDIR)/rbpf

all: $(RBPF_BINS)
include $(RIOTBASE)/Makefile.include
