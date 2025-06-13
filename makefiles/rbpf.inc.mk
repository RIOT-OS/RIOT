RBPF_SOURCES ?= $(wildcard $(CURDIR)/*.c)
RBPF_GENRBF ?= $(RIOTBASE)/dist/tools/rbpf/gen_rbf.py

RBPF_BINS = $(RBPF_SOURCES:.c=.bin)
RBPF_OBJS = $(RBPF_SOURCES:.c=.o)

LLC ?= llc
CLANG ?= clang
INC_FLAGS = -nostdinc -isystem `$(CLANG) -print-file-name=include`
EXTRA_CFLAGS ?= -Os -emit-llvm

RBPFINCLUDE =  -I$(RIOTBASE)/drivers/include \
	       -I$(RIOTBASE)/core/include \
	       -I$(RIOTBASE)/sys/include \
	       #

all: $(RBPF_BINS)

.PHONY: clean

clean:
	rm -f $(RBPF_OBJS)

INC_FLAGS = -nostdinc -isystem `$(CLANG) -print-file-name=include`

$(RBPF_OBJS):  %.o:%.c
	$(CLANG) $(INC_FLAGS) \
	        $(RBPFINCLUDE) \
	        -Wno-unused-value -Wno-pointer-sign -g3\
	        -Wno-compare-distinct-pointer-types \
	        -Wno-gnu-variable-sized-type-not-at-end \
	        -Wno-address-of-packed-member -Wno-tautological-compare \
	        -Wno-unknown-warning-option \
	        $(EXTRA_CFLAGS) -c $< -o -| $(LLC) -march=bpf -mcpu=v2 -filetype=obj -o $@

$(RBPF_BINS): %.bin:%.o
	$(RBPF_GENRBF) generate $< $@
