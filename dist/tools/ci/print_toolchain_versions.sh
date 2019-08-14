#!/bin/sh

get_cmd_version() {
    if [ -z "$1" ]; then
        return
    fi

    VERSION_RAW=$( ($@ --version) 2>&1)
    ERR=$?
    VERSION=$(echo "$VERSION_RAW" | head -n 1)

    if [ $ERR -eq 127 ] ; then # 127 means command not found
        VERSION="missing"
    elif [ $ERR -ne 0 ] ; then
        VERSION="error: ${VERSION}"
    fi

    printf "%s" "$VERSION"
}

get_define() {
    local cc="$1"
    local line=
    if command -v "$cc" 2>&1 >/dev/null; then
        line=$(echo "$3" | "$cc" -x c -include "$2" -E -o - - 2>&1 | sed -e '/^[   ]*#/d' -e '/^[  ]*$/d')
    fi
    if [ -z "$line" ]; then
        line=missing
    fi
    printf "%s" "$line"
}

get_kernel_info() {
    uname -mprs
}

get_os_info() {
    local os="$(uname -s)"
    local osname="unknown"
    local osvers="unknown"
    if [ "$os" = "Linux" ]; then
        osname="$(cat /etc/os-release | grep ^NAME= | awk -F'=' '{print $2}')"
        osvers="$(cat /etc/os-release | grep ^VERSION= | awk -F'=' '{print $2}')"
    elif [ "$os" = "Darwin" ]; then
        osname="$(sw_vers -productName)"
        osvers="$(sw_vers -productVersion)"
    fi
    printf "%s %s" "$osname" "$osvers"
}

newlib_version() {
    if [ -z "$1" ]; then
        printf "%s" "error"
    else
        local cc="$1"
        printf "%s" "$(get_define "$cc" newlib.h _NEWLIB_VERSION)"
    fi
}

avr_libc_version() {
    if [ -z "$1" ]; then
        printf "%s" "error"
    else
        local cc="$1"
        printf "%s (%s)" "$(get_define "$cc" avr/version.h __AVR_LIBC_VERSION_STRING__)" "$(get_define "$cc" avr/version.h __AVR_LIBC_DATE_STRING__)"
    fi
}

printf "\n"
# print operating system information
printf "%s\n" "Operating System Environment"
printf "%s\n" "-----------------------------"
printf "%23s: %s\n" "Operating System" "$(get_os_info)"
printf "%23s: %s\n" "Kernel" "$(get_kernel_info)"
printf "\n"

printf "%s\n" "Installed compiler toolchains"
printf "%s\n" "-----------------------------"
printf "%23s: %s\n" "native gcc" "$(get_cmd_version gcc)"
for p in \
         arm-none-eabi \
         avr mips-mti-elf \
         msp430 \
         riscv-none-embed \
         xtensa-esp32-elf \
         xtensa-lx106-elf \
         ; do
    printf "%23s: %s\n" "$p-gcc" "$(get_cmd_version ${p}-gcc)"
done
printf "%23s: %s\n" "clang" "$(get_cmd_version clang)"
printf "\n"
printf "%s\n" "Installed compiler libs"
printf "%s\n" "-----------------------"
# platform specific newlib version
for p in \
         arm-none-eabi \
         mips-mti-elf \
         riscv-none-embed \
         xtensa-esp32-elf \
         xtensa-lx106-elf \
         ; do
    printf "%23s: %s\n" "$p-newlib" "$(newlib_version ${p}-gcc)"
done
# avr libc version
printf "%23s: %s\n" "avr-libc" "$(avr_libc_version avr-gcc)"
# tools
printf "\n"
printf "%s\n" "Installed development tools"
printf "%s\n" "---------------------------"
for c in \
         cmake \
         cppcheck \
         doxygen \
         git \
         make \
         openocd \
         python \
         python2 \
         python3 \
         ; do
    printf "%23s: %s\n" "$c" "$(get_cmd_version "${c}")"
done
printf "%23s: %s\n" "flake8" "$(get_cmd_version "python3 -Wignore -m flake8")"
printf "%23s: %s\n" "coccinelle" "$(get_cmd_version spatch)"

exit 0
