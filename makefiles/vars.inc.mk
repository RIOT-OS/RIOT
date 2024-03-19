# set default values for selected global variables
FLASH_ADDR ?= 0x0

export Q                     # Used in front of Makefile lines to suppress the printing of the command if user did not opt-in to see them.
export QQ                    # as Q, but be more quiet
export QUIET                 # The parameter to use whether to show verbose makefile commands or not.
export QUIETER               # The parameter to use to hide most makefile output

export OS                    # The operating system of the build host
export OS_ARCH               # The build host's hardware architecture

export APPLICATION           # The application, set in the Makefile which is run by the user.
export APPLICATION_MODULE    # The application module name.
export BOARD                 # The board to compile the application for.
export CPU                   # The CPU, set by the board's Makefile.features.
export CPU_MODEL             # The specific identifier of the used CPU, used for some CPU implementations to differentiate between different memory layouts. Set by the board's Makefile.features.
export CPU_CORE              # The specific identifier of the core present in the CPU. Needed for depency resolution.
export CPU_ARCH              # The specific identifier of the architecture of the core defined in CPU_CORE.
export CPU_FAM               # An intermediate identifier between CPU and CPU_MODEL that represents a sub-group of a Manufacturers CPU's.
export INCLUDES              # The extra include paths, set by the various Makefile.include files.
export CXXINCLUDES           # The extra include paths for c++, set by the various Makefile.include files.
export NATIVEINCLUDES        # The native include paths, set by the various native Makefile.include files.

export GCC_C_INCLUDES        # system include dirs implicitly used by GCC's c compiler, only defined with TOOLCHAIN=llvm
export GCC_CXX_INCLUDES      # system include dirs implicitly used by GCC's c++ compiler, only defined with TOOLCHAIN=llvm

export USEMODULE             # Sys Module dependencies of the application. Set in the application's Makefile.
export BIN_USEMODULE         # Modules specific to bindist (see bindist.ink.mk). Set in the application's Makefile.
export USEPKG                # Pkg dependencies (third party modules) of the application. Set in the application's Makefile.
export DISABLE_MODULE        # Used in the application's Makefile to suppress DEFAULT_MODULEs.
# APPDEPS                    # Files / Makefile targets that need to be created before the application can be build. Set in the application's Makefile.
# BUILDDEPS                  # Files / Makefile targets that need to be created before starting to build.
# DEBUGDEPS                  # Files / Makefile targets that need to be created before starting a debug session.

export RIOTBASE              # The root folder of RIOT. The folder where this very file lives in.
export RIOTCPU               # For third party CPUs this folder is the base of the CPUs.
export RIOTBOARD             # This folder is the base of the riot boards.
export BOARDSDIR             # This is the folder containing the board dir
export EXTERNAL_BOARD_DIRS   # List of folders containing external board dirs
export BOARDDIR              # This folder contains the board
export RIOTPKG               # For overriding RIOT's pkg directory
export RIOTTOOLS             # Location of host machine tools
export RIOTPROJECT           # Top level git root of the project being built, or PWD if not a git repository
export RIOTMAKE              # Location of all supplemental Makefiles (such as this file)
export RIOTKCONFIG           # Location of all supplemental Kconfig files
export BINDIRBASE            # This is the folder where the application should be built in. For each BOARD a different subfolder is used.
export BINDIR                # This is the folder where the application should be built in.
export RIOT_TEST_HASH_DIR    # The dir to generate the test-input-hash.sha1 file for checking if a test has changed, uses BINDIR by default.
export CARGO_TARGET_DIR      # This is the folder where Rust parts of the application should be built in.
export BUILD_DIR             # This is the base folder to store common build files and artifacts, e.g. test results.
export APPDIR                # The base folder containing the application
export PKG_PATHS             # List of absolute paths where packages of $(USEPKG) can be found
export PKGDIRBASE            # The base folder for building packages

export PYTHONPATH            # Python default search path for module filesi, with RIOT specific packages

export FEATURES_REQUIRED     # List of required features by the application
export FEATURES_PROVIDED     # List of provided features by the board
export FEATURES_OPTIONAL     # List of nice to have features
export FEATURES_USED         # List of features used
# TOOLCHAINS_SUPPORTED       # List of supported toolchains by a CPU (gnu/llvm/...).
# TOOLCHAINS_BLACKLISTED     # List of unspported toolchains for a module or an application.
export TOOLCHAIN             # Base build toolchain, i.e. GNU or LLVM

export TARGET_ARCH           # The target platform name, in GCC triple notation, e.g. "arm-none-eabi", "i686-elf", "avr"
export TARGET_ARCH_LLVM      # The target platform name, in LLVM triple notation, e.g. "arm-none-eabi"
export PREFIX                # The prefix of the toolchain commands, usually "$(TARGET_ARCH)-", e.g. "arm-none-eabi-" or "msp430-".
export CC                    # The C compiler to use.
export CXX                   # The CXX compiler to use.
export CCAS                  # The C compiler to use for assembler files, typically the same as CC.
export CFLAGS                # The compiler flags. Must only ever be used with `+=`.
export CFLAGS_CPU            # CPU architecture specific compiler flags
export CXXUWFLAGS            # (Patterns of) flags in CFLAGS that should not be passed to CXX.
export CXXEXFLAGS            # Additional flags that should be passed to CXX.
export CCASUWFLAGS           # (Patterns of) flags in CFLAGS that should not be passed to CCAS.
export CCASEXFLAGS           # Additional flags that should be passed to CCAS.
export AS                    # The assembler.
export ASFLAGS               # Flags for the assembler.
export LINK                  # The command used to link the files. Must take the same parameters as GCC, i.e. "ld" won't work.
export NM                    # The command used to list symbols from objet files
export RANLIB                # The command used to generate an index to the contents of an archive
# LINKFLAGS                  # Flags to supply in the linking step.
export ARCHIVES              # List of archives to add in the linking step
export LTOFLAGS              # extra CFLAGS for compiling with link time optimization
export OBJCOPY               # The command used to create the HEXFILE and BINFILE.
export OFLAGS                # The parameter for OBJCOPY, e.g. to strip the debug information.
export OBJDUMP               # The command used to create the assembly listing.
export OBJDUMPFLAGS          # The parameter for OBJDUMP.
export SIZE                  # The command to read to size of the ELF sections.
export SIZEFLAGS             # The optional size flags.
export UNDEF                 # Object files that the linker must include in the ELFFILE even if no call to the functions or symbols (ex: interrupt vectors).
export WERROR                # Treat all compiler warnings as errors if set to 1 (see -Werror flag in GCC manual)
export WPEDANTIC             # Issue all (extensive) compiler warnings demanded by strict C/C++
# EEPROM_FILE                # (Native only!) file path where the content of the EEPROM is stored

# GITCACHE                   # path to git-cache executable
# GIT_CACHE_DIR              # path to git-cache cache directory, only used with packages
# FLASHER                    # The command to call on "make flash".
# PROG_DEV                   # The device to connect the FLASHER and DEBUGGER
# FFLAGS                     # The parameters to supply to FLASHER.
export FLASH_ADDR            # Define an offset to flash code into ROM memory.
# TERMPROG                   # The command to call on "make term".
# TERMFLAGS                  # Additional parameters to supply to TERMPROG.
# TERMENV                    # Environment variables passed to TERMPROG
# TERMLOG                    # Optional file to log "make term" output to.
# TERMTEE                    # Optional pipe to redirect "make term" output. Default: '| tee -a ${TERMLOG}' when TERMLOG is defined else undefined.
# PORT                       # The port to connect the TERMPROG to.
export ELFFILE               # The unstripped result of the compilation.
export HEXFILE               # The 'intel hex' stripped result of the compilation.
# BINFILE                    # The 'binary' stripped result of the compilation.
# FLASHFILE                  # The output file used for flashing
# BUILD_FILES                # The list of files to be built
# DEBUGGER                   # The command to call on "make debug", usually a script starting the GDB front-end.
# DEBUGGER_FLAGS             # The parameters to supply to DEBUGGER.
# DEBUGSERVER                # The command to call on "make debug-server", usually a script starting the GDB server.
# DEBUGSERVER_FLAGS          # The parameters to supply to DEBUGSERVER.
# DEBUGCLIENT                # The command to call on "make debug-client", usually a script starting the GDB client.
# DEBUGCLIENT_FLAGS          # The parameters to supply to DEBUGCLIENT.
# DEVELHELP                  # Set to 1 to spend ROM, RAM and CPU time for help during development (e.g. enable asserts())
# RESET                      # The command to call on "make reset", this command resets/reboots the target.
# RESET_FLAGS                # The parameters to supply to RESET.
# PROGRAMMER                 # The programmer to use when flashing, resetting or debugging
# PROGRAMMERS_SUPPORTED      # The list of programmers supported by a board
# PROGRAMMER_QUIET           # Change verbosity of programmer output (only used with flash and reset targets).
                             # Default is 1, not verbose. Use 0 to get normal programmer output.
# USE_PROGRAMMER_WRAPPER_SCRIPT     # Use the programmer wrapper Python script. Default is 0 (wrapper not used).


export DLCACHE               # directory used to cache http downloads
export DOWNLOAD_TO_FILE      # Use `$(DOWNLOAD_TO_FILE) $(DESTINATION) $(URL)` to download `$(URL)` to `$(DESTINATION)`.
export DOWNLOAD_TO_STDOUT    # Use `$(DOWNLOAD_TO_STDOUT) $(URL)` to download `$(URL)` output `$(URL)` to stdout, e.g. to be piped into `tar xz`.
export UNZIP_HERE            # Use `cd $(SOME_FOLDER) && $(UNZIP_HERE) $(SOME_FILE)` to extract the contents of the zip file `$(SOME_FILE)` into `$(SOME_FOLDER)`.

export LAZYSPONGE            # Command saving stdin to a file only on content update.
export LAZYSPONGE_FLAGS      # Parameters supplied to LAZYSPONGE.

export FLAGS_FOR_AFL         # Additional command-line flags passed to afl during fuzzing.

# LOG_LEVEL                  # Logging level as integer (NONE: 0, ERROR: 1, WARNING: 2, INFO: 3, DEBUG: 4, default: 3)
# KCONFIG_ADD_CONFIG         # List of .config files to be merged used by Boards and CPUs. See kconfig.mk
# VERBOSE_ASSERT             # Set to 1 to print the file and line of a failed assert when assertions blow

export RUST_TARGET           # Rust's own version of the target triple / quadruple.
                             #
                             # It is set by the architecture (and thus eventually the CPU), and exported to
                             # be available when building Rust modules.
