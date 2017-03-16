#!/bin/sh

gcc_version() {
    local cc
    if [ -z "$1" ]; then
        cc=gcc
    else
        cc=$1-gcc
    fi
    local ver=
    if command -v "$cc" 2>&1 >/dev/null; then
        ver=$("$cc" --version | head -n 1)
    fi
    if [ -z "$ver" ]; then
        ver=missing/error
    fi
    printf "%s" "$ver"
}

get_define() {
    local cc="$1"
    local line=
    if command -v "$cc" 2>&1 >/dev/null; then
        line=$(echo "$3" | "$cc" -x c -include "$2" -E -o - - 2>&1 | sed -e '/^[   ]*#/d' -e '/^[  ]*$/d')
    fi
    if [ -z "$line" ]; then
        line=missing/error
    fi
    printf "%s" "$line"
}

newlib_version() {
    if [ -z "$1" ]; then
        cc=gcc
    else
        cc=$1-gcc
    fi
    printf "%s" "$(get_define "$cc" newlib.h _NEWLIB_VERSION)"
}

avr_libc_version() {
    if [ -z "$1" ]; then
        cc=gcc
    else
        cc=$1-gcc
    fi
    printf "%s (%s)" "$(get_define "$cc" avr/version.h __AVR_LIBC_VERSION_STRING__)" "$(get_define "$cc" avr/version.h __AVR_LIBC_DATE_STRING__)"
}

printf "%s\n" "Installed toolchain versions"
printf "%s\n" "----------------------------"
VER=$(gcc --version | head -n 1)
if [ -n "$VER" ]; then
    printf "%20s: %s\n" "native gcc" "$VER"
fi
for p in msp430 avr arm-none-eabi mips-mti-elf; do
    printf "%20s: %s\n" "$p-gcc" "$(gcc_version "$p")"
done
VER=$(clang --version | head -n 1)
if [ -n "$VER" ]; then
    printf "%20s: %s\n" "clang" "$VER"
fi

for p in arm-none-eabi mips-mti-elf; do
    printf "%20s: %s\n" "$p-newlib" "$(newlib_version "$p")"
done
for p in avr; do
    printf "%20s: %s\n" "$p-libc" "$(avr_libc_version "$p")"
done
exit 0
