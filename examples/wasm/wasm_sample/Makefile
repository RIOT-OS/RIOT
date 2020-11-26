LLVM_VERSION = 11

#LINK_FLAGS_bak =\
#		-nostartfiles \
#		--nostdlib \
#		--nostdinc \
#		--print-map\
#		--initial-memory= \
#		--allow-undefined-file=<value>
#		--initial-memory=65536

# --export-all           Export all symbols (normally combined with --no-gc-sections)
# --export-dynamic       Put symbols in the dynamic symbol table
# --export-table         Export function table to the environment
# --export=<value>       Force a symbol to be exported
# --fatal-warnings       Treat warnings as errors
# --import-memory        Import memory from the environment
# --import-table         Import function table from the environment
# --initial-memory=<value>
#                        Initial size of the linear memory
# --lto-O<opt-level>     Optimization level for LTO
# --lto-partitions=<value>
#                        Number of LTO codegen partitions
# -L <dir>               Add a directory to the library search path
# -l <libName>           Root name of library to use
# --max-memory=<value>   Maximum size of the linear memory
# --merge-data-segments  Enable merging data segments
# --mllvm <value>        Options to pass to LLVM
# --pie                  Create a position independent executable
# --print-gc-sections    List removed unused sections
# --relocatable          Create relocatable object file
# --shared-memory        Use shared linear memory
# --shared               Build a shared object
# --stack-first          Place stack at start of linear memory rather than after data
# --strip-all            Strip all symbols
# --strip-debug          Strip debugging information
# -S                     Alias for --strip-debug
# -s                     Alias for --strip-all
# --thinlto-cache-dir=<value>
#                        Path to ThinLTO cached object file directory
# --thinlto-cache-policy=<value>
#                        Pruning policy for the ThinLTO cache
# --thinlto-jobs=<value> Number of ThinLTO jobs
# --threads              Run the linker multi-threaded
# --undefined=<value>    Force undefined symbol during linking
# --verbose              Verbose mode
# --version              Display the version number and exit
# -z <option>            Linker option extensions


LINK_FLAGS = --export main \
		--allow-undefined \
		--export-dynamic \
		--no-entry \
		--strip-all \
		-error-limit=0 \
		--lto-O3 \
		-O3 \
		--gc-sections\
		-z stack-size=4096 \

#COMPILE_FLAGS_mov=\
		-Wl,--allow-undefined\
		-Wl,--strip-debug \
		-nostartfiles \
		-std=c++14 \

COMPILE_FLAGS = -Wall \
		--target=wasm32-unknown-unknown-wasm \
		-emit-llvm \
		-Os \
		-flto \
		-fvisibility=hidden \
		-ffunction-sections \
		-fdata-sections \

%.show: %.wasm
	wasm2wat $<

%.wasm: %.o Makefile
	wasm-ld-$(LLVM_VERSION) -o $@ $(LINK_FLAGS) $<


%.o: %.cpp Makefile FORCE
	clang++-$(LLVM_VERSION) \
		-c \
		$(COMPILE_FLAGS) \
		-o $@ \
		$<

%.o: %.c Makefile FORCE
	clang-$(LLVM_VERSION) \
		-c \
		$(COMPILE_FLAGS) \
		-o $@ \
		$<

%.wat: %.wasm Makefile
	wasm2wat -o $@ $<

.PHONY: FORCE
