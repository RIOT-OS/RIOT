OPENOCD_SCRIPTS_PATH=${OPENOCD_SCRIPTS_PATH:-/usr/share/openocd/scripts/}
DOCKER=${DOCKER:-docker}

# Assume that boards are only in the `boards` folder of the current git repo,
# or in any gitsubmodule under the current repo. This works both in RIOT and
# in external projects that do use RIOT as git submodule.
function _boards {
    local -a _boards_available
    local -a _board_dirs
    _board_dirs=( ${(s[ ])EXTERNAL_BOARD_DIRS} )
    if git rev-parse --is-inside-work-tree &> /dev/null; then
        local _repo_root="$(git rev-parse --show-toplevel)"
        if [ -d "$_repo_root/boards" ]; then
            _board_dirs+=("$_repo_root/boards")
        fi

        for submodule in $(git submodule status --recursive | cut -d ' ' -f 3); do
            if [ -d "$submodule/boards" ]; then
                _board_dirs+=("$submodule/boards")
            fi
        done
    fi
    for dir ("$_board_dirs[@]") \
        _boards_available+=($(ls "$dir" | grep -v common))

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

function _serials {
    local -a _serial_vals

    if [[ $OSTYPE == darwin* ]]; then
        _serial_vals=(/dev/cu.*(N))
    else
        _serial_vals=(/dev/ttyUSB*(N) /dev/ttyACM*(N))
    fi

    _describe 'serial' _serial_vals
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

function _ftdi_adapter {
    local -a _adapter_vals=($(find "$OPENOCD_SCRIPTS_PATH"/interface/ftdi -type f -name '*.cfg' -exec basename --suffix=.cfg {} \;))
    _describe 'ftdi_adapter' _adapter_vals
}

function _docker_image {
    local -a _docker_image_vals=($("$DOCKER" image ls --format '{{.Repository}}'))
    _describe 'docker_image' _docker_image_vals
}

function _riot_terminals {
    local -a _riot_terminal_vals=(
        "pyterm:RIOT's custom terminal with command line editing and history emulation"
        "jlink:Connect to a virtual stdio provided by stdio_rtt using JLink"
        "openocd-rtt:Connect to a virtual stdio provided by stdio_rtt using OpenOCD"
        "semihosting:Connect to a virtual stdio provided by stdio_semihosting using GDB + OpenOCD/JLink/..."
        "bootterm:Use the host system's bootterm to connect to the serial console"
        "miniterm:Use the host system's miniterm to connect to the serial console"
        "picocom:Use the host system's picocom to connect to the serial console"
        "socat:Use the host system's socat to provide a rather raw terminal (suitable for testing/scripting)"
        "native:Only for native32/native64 boards: Run the app natively, without a terminal in front"
    )

    _describe 'terminal' _riot_terminal_vals
}

function _riot_emulators {
    local -a _riot_emulator_vals=(
        "renode:embedded system emulator framework modelling MCUs, on-chip peripherals, and external devices"
        "qemu:generic and open source machine emulator and virtualizer"
        "mgba:Game Boy Advance emulator"
    )

    _describe 'emulator' _riot_emulator_vals
}

function _renode_log_levels {
    local -a _levels=(
        "-1:Noisy"
        "0:Debug"
        "1:Info"
        "2:Warning"
        "3:Error"
    )

    _describe 'log_level' _levels
}

# Complete to either an executable in $PATH (including shell builtins), or
# to a file that is executable
_executables() {
    _alternative \
        'commands:command:_command_names' \
        'files:executable:_path_files -g "*(x)"'
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
        "info-rust:list versions of the used rust toolchain"
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
        '(-C --directory=)'{-C,--directory}'[dir of the app to build]:dir:_directories'

    local -a vars
    vars=(
        'BOARD[Select the board to build for]:board:_boards'
        'BUILD_IN_DOCKER[Build inside docker container]:bool:_bools'
        'DOCKER_IMAGE[The docker image to use with BUILD_IN_DOCKER=1]:docker_image:_docker_image'
        'EMULATE[Run firmware in emulator selected by RIOT_EMULATOR]:bool:_bools'
        'GDB_PORT[Port for the GDB server to use in "make debug" or "make debug-sever"]:port'
        'LTO[Enable link time optimization]:bool:_bools'
        'OPENOCD_DEBUG_ADAPTER[Select the programmer hardware to use with OpenOCD]:hw_programmer:_hw_programmers'
        'OPENOCD_FTDI_ADAPTER[Select the FTDI adapter config to use with OpenOCD]:ftdi_adapter:_ftdi_adapter'
        'OPENOCD_RESET_USE_CONNECT_ASSERT_SRST[Let OpenOCD attach while reset signal is asserted]:bool:_bools'
        'PORT[Serial port connected to the board]:serial:_serials'
        'PROGRAMMER[Select the programmer software to flash (debug) with]:programmer:_programmers'
        'QUIET[Reduce verbosity of build output]:bool:_bools'
        'RENODE[Renode executable to run, defaults to "renode"]:renode:_executables'
        'RENODE_LOG_LEVEL[The log verbosity of renode]:log_level:_renode_log_levels'
        'RENODE_SHOW_GUI[Whether to show the renode GUI]:bool:_bools'
        'RENODE_SHOW_LOG[Whether to show the renode log]:bool:_bools'
        'RENODE_TELNET_PORT[The port on which renode will accept telnet sessions]:port'
        'RIOT_CI_BUILD[Behave as in the CI: Less verbose output, reproducible builds, ...]:bool:_bools'
        'RIOT_EMULATOR[Emulator run the generated firmware, renode by default]:emulator:_riot_emulators'
        'RIOT_TERMINAL[Select the terminal program to use]:terminal:_riot_terminals'
        'STATIC_ANALYSIS[Enable static analysis for modules that claim support]:bool:_bools'
        'TOOLCHAIN[Select the toolchain to use]:toolchain:_toolchains'
        'VERBOSE_ASSERT[Print source file and line on blown assertion]:bool:_bools'
        'WERROR[Enable/disable -Werror flag]:bool:_bools'
    )

    _values -w 'variables' $vars
}

_is_riot() {
    if ! git rev-parse --is-inside-work-tree &> /dev/null; then
        return 1
    fi
    local _repo_root="$(git rev-parse --show-toplevel)"
    if [ -f "$_repo_root/.murdock" ]; then
        return 0
    fi
    for submodule in $(git submodule status --recursive | cut -d ' ' -f 3); do
        if [ -f "$submodule/.murdock" ]; then
            return 0
        fi
    done
    return 1
}

compdef '
if _is_riot; then
    _riot
else
    _make
fi
' make
