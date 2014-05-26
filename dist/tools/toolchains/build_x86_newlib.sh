#!/bin/bash

# directory to download source files and store intermediates
[[ -z $TARGET ]] && TARGET=i586-none-elf
[[ -z $TMP_DIR ]] && TMP_DIR=/var/tmp/RIOT-toolchain-${USER}
[[ -z $BUILDDIR ]] && BUILDDIR=${TMP_DIR}/x86
[[ -z $PREFIX ]] && PREFIX=${PWD}/toolchain/x86
[[ -z $MAKE_THREADS ]] && MAKE_THREADS=

NEWLIB_VER=2.1.0
NEWLIB_MD5=c6559d83ecce4728a52f0ce7ec80de97
NEWLIB_CONFIGURE_FLAGS=(
    --target=${TARGET}
    --prefix=${PREFIX}
    --with-gmp=${PREFIX}
    --with-mpfr=${PREFIX}
    --with-mpc=${PREFIX}
    --enable-interwork
    --enable-multilib

    --enable-target-optspace
    --enable-lto
    --disable-newlib-supplied-syscalls
    --enable-newlib-reent-small
    --enable-newlib-io-long-long
    --enable-newlib-io-float
    --enable-newlib-io-pos-args
    --enable-newlib-io-c99-formats
    --enable-newlib-multithread
)
NEWLIB_PATCHES=(
libgloss/i386/cygmon-gmon.c
'64d
59a
#include <string.h>

.'

libgloss/arm/_exit.c
'14a
  __builtin_unreachable ();
.'

newlib/libc/stdlib/mallocr.c
'3700d
3434,3440d
3422,3424d
3400,3404d
3357,3370d
3278,3355d
1764,1898d
388d
384,386d
1,3d'

newlib/libc/stdlib/mlock.c
'63,64d
1d'
)
NEWLIB_TARGET_CFLAGS=(
    -DREENTRANT_SYSCALLS_PROVIDED
    -DMALLOC_PROVIDED
    -DSIGNAL_PROVIDED

    -ggdb3
    -Os
    -flto
    -fomit-frame-pointer
    -ffunction-sections
    -fdata-sections
)

DOWNLOADER=wget
DOWNLOADER_OPTS="-nv -c"

build_newlib() {
    cd ${BUILDDIR}
    echo
    echo "Building newlib (${NEWLIB_VER}) ..."
    [[ -e .newlib_built ]] && return

    rm -rf newlib-build && mkdir -p newlib-build && cd newlib-build &&
    CFLAGS="${CFLAGS} -DREENTRANT_SYSCALLS_PROVIDED -DMALLOC_PROVIDED" \
    ../newlib-${NEWLIB_VER}/configure "${NEWLIB_CONFIGURE_FLAGS[@]}"
    make ${MAKE_THREADS} TARGET_CFLAGS="${NEWLIB_TARGET_CFLAGS[*]}" all &&
    make install &&

    cd ${BUILDDIR} &&
    date -uIns > .newlib_built
}

clean() {
    echo
    echo "Cleaning up..."
    for F in newlib; do
        rm -rf ./.${F}_extracted ./.${F}_built ./${F}_build || return $?
    done
}

download() {
    echo
    echo "Downloading TAR files."

    download_file ftp://sources.redhat.com/pub/newlib newlib-${NEWLIB_VER}.tar.gz ${NEWLIB_MD5}
}

extract() {
    echo
    echo "Extracting TAR files."

    if [[ ! -x ./newlib-${NEWLIB_VER}/configure ]]; then
        tar xzf ${TMP_DIR}/newlib-${NEWLIB_VER}.tar.gz &&
        for (( I=0; I < ${#NEWLIB_PATCHES[@]}; I+=2 )); do
            echo "Applying Newlib patch $((${I} / 2 + 1))"
            echo "${NEWLIB_PATCHES[$I+1]}" | patch -e ./newlib-${NEWLIB_VER}/"${NEWLIB_PATCHES[$I]}" || return $?
        done
    fi
}

download_file() {
    echo "Downloading ${1}/${2}..."
    cd ${TMP_DIR} &&
    ${DOWNLOADER} ${DOWNLOADER_OPTS} $1/$2 &&
    echo -n "Checking MD5 of " &&
    echo "${3}  ${2}" | md5sum -c - &&
    cd ${BUILDDIR}
}

all() {
    echo
    echo "Starting in ${BUILDDIR}. Installing to ${PREFIX}."

    download &&
    extract &&
    build_newlib &&

    echo &&
    echo 'Success!' &&
    echo "Insert \"export PATH=${PREFIX}/bin:\$PATH\" to your .bashrc"
}

usage() {
    echo "usage: ${0} [all | download | extract | clean | build_newlib]"
    echo "example: ${0} all"
    echo ""
    echo "Builds a(n) \"${TARGET}\" toolchain,"
    echo "installs into \"${PREFIX}\", and"
    echo "uses \"${TMP_DIR}\" as temp."
    echo ""
    echo "Use PREFIX=... and TMP_DIR=... to change these directories."
    echo "Use TARGET=... to change the target (affects PREFIX)."
    echo ""
    echo "Run like \"MAKE_THREADS=-j$(nproc) ${0} all\" to speed up on multicore systems."
}

if [[ -z $1 ]]; then
    usage
    exit 1
fi

mkdir -p ${PREFIX} &&
mkdir -p ${TMP_DIR} &&
mkdir -p ${BUILDDIR} &&
cd ${BUILDDIR} &&
for arg; do
    ${arg} || exit $?
done
