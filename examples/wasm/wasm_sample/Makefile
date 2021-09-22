#sometimes there might not be a wasm-ld (Ubuntu:focal)
#lets check if we can find a specific version

ifneq ($(shell (command -v wasm-ld)),)
  WASM-LD ?= wasm-ld
  else
    ifneq ($(shell (command -v wasm-ld-11)),)
    LLVM_VERSION = 11
    else
      ifneq ($(shell (command -v wasm-ld-10)),)
      LLVM_VERSION = 10
      else
        ifneq ($(shell (command -v wasm-ld-9)),)
        LLVM_VERSION = 9
        else
          ifneq ($(shell (command -v wasm-ld-8)),)
          LLVM_VERSION = 8
          endif
      endif
    endif
  endif
endif

ifneq ($(LLVM_VERSION),)
  CLANG ?= clang-$(LLVM_VERSION)
  CLANGPP ?= clang++-$(LLVM_VERSION)
  WASM-LD ?= wasm-ld-$(LLVM_VERSION)
else
  CLANG ?= clang
  CLANGPP ?= clang++
endif

WASM-LD ?= echo !! NO wasm-ld(-VERSION) found !!; false

LINK_FLAGS = -z stack-size=4096 \
		--export main \
		--export=__heap_base \
		--export=__data_end \
		--allow-undefined \
		--no-entry \
		--strip-all \
		--export-dynamic \
		-error-limit=0 \
		--lto-O3 \
		-O3 \
		--gc-sections\
		--initial-memory=65536 \

# --initial-memory may only be set in 64kB steps (pagesize of WASM)
# even though this one page is 64kB
#	- data starts at 0, (1024 is chosen by lld)
#	- stack starts at 4kB down
#	- heap at 4kB up (see stack-size option)
#	-> memory can be smaller than first page
# without stack-size option stack will start at 64kB
#	-> heap needs a second page
# wasm-ld 8 and 11 do not need --initial-memory=64kB

COMPILE_FLAGS = -Wall \
		--target=wasm32-unknown-unknown-wasm \
		-emit-llvm \
		-Os \
		-flto \
		-fvisibility=hidden \
		-ffunction-sections \
		-fdata-sections \

#one might consider adding these
#		-nostartfiles \
#		--nostdlib \
#		--nostdinc \
#		-std=c++14 \

%.show: %.wasm
	wasm2wat $<

%.wasm: %.o Makefile
	$(WASM-LD) -o $@ $(LINK_FLAGS) $<


%.o: %.cpp Makefile FORCE
	$(CLANGPP) \
		-c \
		$(COMPILE_FLAGS) \
		-o $@ \
		$<

%.o: %.c Makefile FORCE
	$(CLANG)\
		-c \
		$(COMPILE_FLAGS) \
		-o $@ \
		$<

%.wat: %.wasm Makefile
	wasm2wat -o $@ $<

.PHONY: FORCE
