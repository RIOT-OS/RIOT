#!/usr/bin/env bash

MAKE=${MAKE:-make}

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
        line=$(echo "$3" | "$cc" -x c -include "$2" -E -o - - 2>/dev/null | sed -e '/^[   ]*#/d' -e '/^[  ]*$/d')
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
    elif [ "$os" = "FreeBSD" ]; then
        osname="$os"
        osvers="$(freebsd-version)"
    fi
    printf "%s %s" "$osname" "$osvers"
}

extract_shell_version() {
    SHELL_NAME=$"(basename $1)"
    SHELL_VERSION="$($1 --version 2>/dev/null)"
    ERR=$?
    if [ $ERR -ne 0 ] ; then # if it does not like the --version switch, it is probably dash
        printf "%s" "$1"
        # we do not say "probably dash" if we are sure it IS dash
        if [ "$SHELL_NAME" != dash ] ; then
            printf " (probably dash)"
        fi
    else
        printf "%s" "$(echo "$SHELL_VERSION" | head -n 1)"
    fi
}

get_sys_shell() {
    case "$(uname -s)" in
        MINGW*)
            # MINGW has no realpath, but also no (meaningful) symlinks
            SH_PATH=/bin/sh
            ;;
        *)
            SH_PATH="$(realpath /bin/sh)"
            ;;
    esac
    extract_shell_version "$SH_PATH"
}

_get_make_shell() {
    ${MAKE} -sf - --no-print-directory 2>/dev/null <<MAKEFILE
\$(info \$(realpath \$(SHELL)))
MAKEFILE
}

get_make_shell() {
    extract_shell_version "$(_get_make_shell)"
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
printf "%s\n" "RIOT version information"
printf "%s\n" "----------------------------"
if [ -d .git ]; then
    RIOT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
    RIOT_COMMIT_HASH=$(git rev-parse HEAD)
    RIOT_COMMIT_DATE=$(git log -1 --format=%cd --date=short)
else
    RIOT_BRANCH="unknown (not a git repository)"
    RIOT_COMMIT_HASH="unknown (not a git repository)"
    RIOT_COMMIT_DATE="unknown (not a git repository)"
fi
printf "%25s: %s\n" "RIOT branch" "$RIOT_BRANCH"
printf "%25s: %s\n" "RIOT commit hash" "$RIOT_COMMIT_HASH"
printf "%25s: %s\n" "RIOT commit date" "$RIOT_COMMIT_DATE"
printf "\n"
# print operating system information
printf "%s\n" "Operating System Environment"
printf "%s\n" "----------------------------"
printf "%25s: %s\n" "Operating System" "$(get_os_info)"
printf "%25s: %s\n" "Kernel" "$(get_kernel_info)"
printf "%25s: %s\n" "System shell" "$(get_sys_shell)"
printf "%25s: %s\n" "make's shell" "$(get_make_shell)"
printf "\n"

printf "%s\n" "Installed compiler toolchains"
printf "%s\n" "-----------------------------"
printf "%25s: %s\n" "native gcc" "$(get_cmd_version gcc)"
for p in \
         arm-none-eabi \
         avr \
         msp430-elf \
         riscv-none-elf \
         riscv64-unknown-elf \
         riscv32-esp-elf \
         xtensa-esp32-elf \
         xtensa-esp32s2-elf \
         xtensa-esp32s3-elf \
         xtensa-esp8266-elf \
         ; do
    printf "%25s: %s\n" "$p-gcc" "$(get_cmd_version ${p}-gcc)"
done
printf "%25s: %s\n" "clang" "$(get_cmd_version clang)"
printf "\n"
printf "%s\n" "Installed compiler libs"
printf "%s\n" "-----------------------"
# platform specific newlib version
for p in \
         arm-none-eabi \
         msp430-elf \
         riscv-none-elf \
         riscv64-unknown-elf \
         riscv32-esp-elf \
         xtensa-esp32-elf \
         xtensa-esp32s2-elf \
         xtensa-esp32s3-elf \
         xtensa-esp8266-elf \
         ; do
    printf "%25s: %s\n" "$p-newlib" "$(newlib_version ${p}-gcc)"
done
# avr libc version
printf "%25s: %s\n" "avr-libc" "$(avr_libc_version avr-gcc)"
# tools
printf "\n"
printf "%s\n" "Installed development tools"
printf "%s\n" "---------------------------"
for c in \
         ccache \
         cmake \
         cppcheck \
         doxygen \
         git \
         ${MAKE} \
         openocd \
         python \
         python2 \
         python3 \
         rustup \
         cargo \
         rustc \
         c2rust \
         node \
         npm \
         ; do
    printf "%25s: %s\n" "$c" "$(get_cmd_version "${c}")"
done
printf "%25s: %s\n" "flake8" "$(get_cmd_version "python3 -Wignore -m flake8")"
printf "%25s: %s\n" "coccinelle" "$(get_cmd_version spatch)"

exit 0
