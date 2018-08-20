# set default values for selected global variables
FLASH_ADDR ?= 0x0

export Q                     # Used in front of Makefile lines to suppress the printing of the command if user did not opt-in to see them.
export QQ                    # as Q, but be more quiet
export QUIET                 # The parameter to use whether to show verbose makefile commands or not.

export APPLICATION           # The application, set in the Makefile which is run by the user.
export APPLICATION_MODULE    # The application module name.
export BOARD                 # The board to compile the application for.
export CPU                   # The CPU, set by the board's Makefile.include.
export CPU_MODEL             # The specific identifier of the used CPU, used for some CPU implementations to differentiate between different memory layouts
export MCU                   # The MCU, set by the board's Makefile.include, or defaulted to the same value as CPU.
export INCLUDES              # The extra include paths, set by the various Makefile.include files.

export USEMODULE             # Sys Module dependencies of the application. Set in the application's Makefile.
export USEPKG                # Pkg dependencies (third party modules) of the application. Set in the application's Makefile.
export DISABLE_MODULE        # Used in the application's Makefile to suppress DEFAULT_MODULEs.
export APPDEPS               # Files / Makefile targets that need to be created before the application can be build. Set in the application's Makefile.
# BUILDDEPS                  # Files / Makefile targets that need to be created before starting to build.

export RIOTBASE              # The root folder of RIOT. The folder where this very file lives in.
export RIOTCPU               # For third party CPUs this folder is the base of the CPUs.
export RIOTBOARD             # For third party BOARDs this folder is the base of the BOARDs.
export RIOTPKG               # For overriding RIOT's pkg directory
export RIOTTOOLS             # Location of host machine tools
export RIOTPROJECT           # Top level git root of the project being built, or PWD if not a git repository
export RIOTMAKE              # Location of all supplemental Makefiles (such as this file)
export BINDIRBASE            # This is the folder where the application should be built in. For each BOARD a different subfolder is used.
export BINDIR                # This is the folder where the application should be built in.
export APPDIR                # The base folder containing the application
export PKGDIRBASE            # The base folder for building packages

export PYTHONPATH            # Python default search path for module filesi, with RIOT specific packages

export FEATURES_REQUIRED     # List of required features by the application
export FEATURES_PROVIDED     # List of provided features by the board
export FEATURES_OPTIONAL     # List of nice to have features
# TOOLCHAINS_SUPPORTED       # List of supported toolchains by an MCU (gnu/llvm/...).
# TOOLCHAINS_BLACKLISTED     # List of unspported toolchains for a module or an application.

export TARGET_ARCH           # The target platform name, in GCC triple notation, e.g. "arm-none-eabi", "i686-elf", "avr"
export PREFIX                # The prefix of the toolchain commands, usually "$(TARGET_ARCH)-", e.g. "arm-none-eabi-" or "msp430-".
export CC                    # The C compiler to use.
export CXX                   # The CXX compiler to use.
export CCAS                  # The C compiler to use for assembler files, typically the same as CC.
export CFLAGS                # The compiler flags. Must only ever be used with `+=`.
export CXXUWFLAGS            # (Patterns of) flags in CFLAGS that should not be passed to CXX.
export CXXEXFLAGS            # Additional flags that should be passed to CXX.
export CCASUWFLAGS           # (Patterns of) flags in CFLAGS that should not be passed to CCAS.
export CCASEXFLAGS           # Additional flags that should be passed to CCAS.
export AR                    # The command to create the object file archives.
export ARFLAGS               # Command-line options to pass to AR, default `rcs`.
export AS                    # The assembler.
export ASFLAGS               # Flags for the assembler.
export LINK                  # The command used to link the files. Must take the same parameters as GCC, i.e. "ld" won't work.
export LINKFLAGS             # Flags to supply in the linking step.
export LTOFLAGS              # extra CFLAGS for compiling with link time optimization
export OBJCOPY               # The command used to create the HEXFILE.
export OFLAGS                # The parameter for OBJCOPY, e.g. to strip the debug information.
export OBJDUMP               # The command used to create the assembly listing.
export OBJDUMPFLAGS          # The parameter for OBJDUMP.
export SIZE                  # The command to read to size of the ELF sections.
export SIZEFLAGS             # The optional size flags.
export UNDEF                 # Object files that the linker must include in the ELFFILE even if no call to the functions or symbols (ex: interrupt vectors).
export WERROR                # Treat all compiler warnings as errors if set to 1 (see -Werror flag in GCC manual)

export GITCACHE              # path to git-cache executable
export FLASHER               # The command to call on "make flash".
export FFLAGS                # The parameters to supply to FLASHER.
export FLASH_ADDR            # Define an offset to flash code into ROM memory.
export TERMPROG              # The command to call on "make term".
export TERMFLAGS             # Additional parameters to supply to TERMPROG.
export PORT                  # The port to connect the TERMPROG to.
export ELFFILE               # The unstripped result of the compilation.
export HEXFILE               # The stripped result of the compilation.
export DEBUGGER              # The command to call on "make debug", usually a script starting the GDB front-end.
export DEBUGGER_FLAGS        # The parameters to supply to DEBUGGER.
export DEBUGSERVER           # The command to call on "make debug-server", usually a script starting the GDB server.
export DEBUGSERVER_FLAGS     # The parameters to supply to DEBUGSERVER.
export RESET                 # The command to call on "make reset", this command resets/reboots the target.
export RESET_FLAGS           # The parameters to supply to RESET.

export CCACHE_BASEDIR        # ccache basedir, allows multiple riot build
                             # directories to share a ccache directory

export DLCACHE               # directory used to cache http downloads
export DOWNLOAD_TO_FILE      # Use `$(DOWNLOAD_TO_FILE) $(DESTINATION) $(URL)` to download `$(URL)` to `$(DESTINATION)`.
export DOWNLOAD_TO_STDOUT    # Use `$(DOWNLOAD_TO_STDOUT) $(URL)` to download `$(URL)` output `$(URL)` to stdout, e.g. to be piped into `tar xz`.
export UNZIP_HERE            # Use `cd $(SOME_FOLDER) && $(UNZIP_HERE) $(SOME_FILE)` to extract the contents of the zip file `$(SOME_FILE)` into `$(SOME_FOLDER)`.

export LAZYSPONGE            # Command saving stdin to a file only on content update.
export LAZYSPONGE_FLAGS      # Parameters supplied to LAZYSPONGE.
