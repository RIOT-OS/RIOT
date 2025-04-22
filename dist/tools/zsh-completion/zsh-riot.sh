function _boards {
    local -a _boards_available
    local -a _board_dirs
    _board_dirs=( ${(s[ ])EXTERNAL_BOARD_DIRS} )
    local _repo_root
    if git rev-parse --is-inside-work-tree &> /dev/null; then
        local _repo_root="$(git rev-parse --show-toplevel)"
        _board_dirs+=("$_repo_root/boards")
    fi
    for dir ("$_board_dirs[@]") \
        _boards_available+=($(ls $dir | grep -v common))

    _describe 'board' _boards_available
}

function _toolchains {
    local -a _toolchains_available=(
        "gnu"
        "llvm"
    )

    _describe 'toolchain' _toolchains_available
}

function _bools {
    local -a _bool_vals=(
        "0"
        "1"
    )

    _describe 'bool' _bool_vals
}

function _programmers {
    local -a _programmer_vals=(
        "openocd:use OpenOCD for programming via JTAG/SWD (default for most boards)"
        "jlink:use Segger's JLinkExe for programming (requires original Segger programmers)"
        "esptool:program ESP32 (S2,S3,C2,C3,C6,...) ESP8266 via UART"
        "avrdude:default for AVR boards"
        "lpc2k_pgm:default for lpc23xx boards"
        "bossa:for SAM based Arduino boards"
        "edbg:for SAM based evaluation board"
        "nrfutil:for nRF5x boards with nRF bootloader"
        "stm32flash:for STM32 boards via UART bootloader"
        "uniflash:for CC13xx / CC26xx boards"
        "cc2538-bsl:for CC2538 boards"
        "mspdebug:for MSP430 boards"
        "goodfet:for some MSP430 boards"
        "uf2conv:program via UF2 bootloader (except RP2040)"
        "picotool:program via UF2 bootloader (Raspberry Pi Pico Devices)"
        "cpy2remed:program via ST-Link using filesystem disk interface"
        "adafruit-nrfutil:program via Adafruits nRF5x bootloader"
        "bmp:Black Magic Probe"
        "dfu-util:Flash using DFU Bootloader"
        "pyocd:Python based tool and API for debugging, programming, and exploring Arm Cortex microcontrollers"
        "robotis-loader:Flash using ROBOTIS Bootloader"
        "stm32loader:Flash using STM32 Bootloader"
    )

    _describe 'programmer' _programmer_vals
}

function _hw_programmers {
    local -a _hw_programmer_vals=(
        "stlink:STMicroelectronics' ST-LINK/V2 or similar"
        "jlink:Segger's J-Link programmer (e.g. J-Link Edu Mini)"
        "dap:CMSIS DAP compatible programmer"
        "ftdi:Bit-banging SWD/JTAG via FTDI chip"
        "xds110:TI XDS110 programmer"
        "sysfs_gpio:Bit-banging SWD/JTAG via GPIOs using the sysfs interface"
        "buspirate:Bit-banging SWD/JTAG via Bus Pirate GPIOs"
        "iotlab:Debugging adapter used by the FIT IoT lab"
        "mulle:Mulle programmer board"
        "raspi:Bit-banging SWD/JTAG via GPIOs on the Raspberry Pi (direct register access)"
        "stlink-dap:STMicroelectronics' ST-LINK/V3 and ST-LINK/V2 (from firmware V2J24)"
    )

    _describe 'hw_programmer' _hw_programmer_vals
}

function _riot {
    local -a _std_targets=(
        "all:build the application"
        "clean:remove the current build"
        "compile-commands:create a compile_commands.json"
        "cosy:open a webserver detailing the size of the firmware as interactive diagram"
        "debug:open GDB and connect to the embedded board, launching a debug server in background"
        "debug-client:open GDB and connect to an already running debug server"
        "debug-server:launch a debug server for GDB to connect to"
        "flash:build and flash the app on the board"
        "flash-only:only flash the most recently built firmware (even if it is stale)"
        "info-boards-supported:list boards supported by the app"
        "info-build:show details to debug the build"
        "info-build-json:show details of the build as JSON"
        "info-buildsize:print the size of the firmware for the given board"
	"info-buildsizes-diff:compare the size of two firmware builds for two given directories"
        "info-cpu:print the CPU family for the given board"
        "info-features-missing:list features missing by the given board in regard to the given app"
        "info-features-provided:list features provided by the given board"
        "info-features-required:list features required by the given app"
        "info-features-used:list features of the given board used by the given app"
        "info-modules:list modules used by the given app when build for the given board"
        "info-objsize:list the size of the individual modules (prior garbage collection)"
        "info-packages:list packages used by the given app when build for the given board"
        "info-programmers-supported:list programmers supported by the given board"
        "info-rust:list versions of the used rust toolcahin"
        "info-toolchains-supported:list toolchains supported by the given board"
        "list-ttys:list TTYs connected to the machine"
        "lstfile:dump lots of details of the build firmware in a lstfile"
        "reset:reset the given board (if supported)"
        "term:connect to the serial of the given board"
        "test:runs the python test script(s) of the current app (assumes the current app is flashed)"
        "test-with-config:runs the python test script in the tests-with-config folder"
    )

    _describe 'target' _std_targets
    _arguments -A '*' \
        '(-C --directory=)'{-C,--directory}'[dir of the app to build]:dir:_directories' \
        #

    local -a vars
    vars=(
        'BOARD[Select the board to build for]:board:_boards'
        'TOOLCHAIN[Select the toolchain to use]:toolcahin:_toolchains'
        'BUILD_IN_DOCKER[Build inside docker container]:bool:_bools'
        'LTO[Enable link time optimization]:bool:_bools'
        'VERBOSE_ASSERT[Print source file and line on blown assertion]:bool:_bools'
        'QUIET[Reduce verbosity of build output]:bool:_bools'
        'WERROR[Enable/disable -Werror flag]:bool:_bools'
        'RIOT_CI_BUILD[Behave as in the CI: Less verbose output, reproducible builds, ...]:bool:_bools'
        'PROGRAMMER[Select the programmer software to flash (debug) with]:programmer:_programmers'
        'OPENOCD_DEBUG_ADAPTER[Select the programmer hardware to use with OpenOCD]:hw_programmer:_hw_programmers'
        'OPENOCD_RESET_USE_CONNECT_ASSERT_SRST[Let OpenOCD attach while reset signal is asserted]:bool:_bools'
        'STATIC_ANALYSIS[Enable static analysis for modules that claim support]:bool:_bools'
    )

    _values -w 'variables' $vars
}
