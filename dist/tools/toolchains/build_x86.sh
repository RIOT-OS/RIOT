#!/bin/bash

# directory to download source files and store intermediates
[[ -z $TARGET ]] && TARGET=i586-none-elf
[[ -z $TMP_DIR ]] && TMP_DIR=/var/tmp/RIOT-toolchain-${USER}
[[ -z $BUILDDIR ]] && BUILDDIR=${TMP_DIR}/${TARGET}
[[ -z $PREFIX ]] && PREFIX=${PWD}/toolchain/${TARGET}
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
    --disable-newlib-supplied-syscalls
    --disable-newlib-reent-small
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
2129c
#if defined (DEFINE_MALLOC) && !defined (MALLOC_PROVIDED)
.
1764,1898d
388d
384,386d
1,3d'

newlib/libc/stdlib/mlock.c
'63,64d
1d'

newlib/libc/include/stdio.h
'681,683d
658,668d
616,645c
int _EXFUN(__sputc_r, (struct _reent *, int, FILE *));
.'

newlib/libc/stdio/putc.c
"91a
int __sputc_r(struct _reent *_ptr, int _c, FILE *_p) {
    if (--_p->_w >= 0 || (_p->_w >= _p->_lbfsize && (char)_c != '\\n'))
        return (*_p->_p++ = _c);
    else
        return (__swbuf_r(_ptr, _c, _p));
}

."
)
NEWLIB_TARGET_CFLAGS=(
    -DREENTRANT_SYSCALLS_PROVIDED
    -DMALLOC_PROVIDED
    -DSIGNAL_PROVIDED
    -DPREFER_SIZE_OVER_SPEED

    -ggdb3
    -Os
    -fomit-frame-pointer
    -ffunction-sections
    -fdata-sections
)

GCC_VER=4.8.2
GCC_MD5=a3d7d63b9cb6b6ea049469a0c4a43c9d
GCC_CONFIGURE_FLAGS=(
    --target=${TARGET}
    --prefix=${PREFIX}
    --with-gmp=${PREFIX}
    --with-mpfr=${PREFIX}
    --with-mpc=${PREFIX}

    --enable-interwork
    --enable-multilib
    --enable-languages="c,c++"
    --with-newlib
    --disable-libssp
    --with-headers=${BUILDDIR}/newlib-${NEWLIB_VER}/newlib/libc/include
    --enable-obsolete
    --enable-target-optspace
    --disable-nls
)

BINUTILS_VER=2.24
BINUTILS_MD5=e0f71a7b2ddab0f8612336ac81d9636b
BINUTILS_CONFIGURE_FLAGS=(
    --prefix=${PREFIX}

    --enable-interwork
    --enable-multilib
    --disable-nls
)

GDB_VER=7.6
GDB_MD5=fda57170e4d11cdde74259ca575412a8
GDB_CONFIGURE_FLAGS=(
    --target=${TARGET}
    --prefix=${PREFIX}
    --with-gmp=${PREFIX}
    --with-mpfr=${PREFIX}
    --with-mpc=${PREFIX}

    --enable-interwork
    --enable-multilib
    --disable-nls
)

GMP_VER=5.1.3
GMP_MD5=e5fe367801ff067b923d1e6a126448aa
GMP_CONFIGURE_FLAGS=(
    --prefix=${PREFIX}

    --enable-cxx
    --disable-nls
)

MPFR_VER=3.1.2
MPFR_MD5=e3d203d188b8fe60bb6578dd3152e05c
MPFR_CONFIGURE_FLAGS=(
    --prefix=${PREFIX}
    --with-gmp=${PREFIX}

    --disable-nls
)

MPC_VER=1.0.2
MPC_MD5=68fadff3358fb3e7976c7a398a0af4c3
MPC_CONFIGURE_FLAGS=(
    --prefix=${PREFIX}
    --with-gmp=${PREFIX}
    --with-mpfr=${PREFIX}

    --disable-nls
)

DOWNLOADER=wget
DOWNLOADER_OPTS="-nv -c"

SPACE_NEEDED=3264960

build_binutils() {
    cd ${BUILDDIR}
    echo
    [[ -e .binutils_built ]] && return

    local TARGETS=(${TARGET} i386-efi-pe)
    for i in 0 1; do
        echo "Building binutils (${BINUTILS_VER}) for ${TARGETS[$i]} ..."

        rm -rf binutils-${TARGETS[$i]}-build &&
        mkdir -p binutils-${TARGETS[$i]}-build &&
        cd binutils-${TARGETS[$i]}-build &&

        ../binutils-${BINUTILS_VER}/configure "${BINUTILS_CONFIGURE_FLAGS[@]}" --target=${TARGETS[$i]} --enable-targets=${TARGETS[$i]},${TARGETS[1-$i]} &&
        make ${MAKE_THREADS} all &&
        make install &&
        cd ${BUILDDIR} ||
        return $?
    done

    date -uIns > .binutils_built
}

build_gcc() {
    cd ${BUILDDIR}
    echo
    echo "Building gcc (${GCC_VER}) ..."
    [[ -e .gcc_built ]] && return

    rm -rf gcc-build && mkdir -p gcc-build && cd gcc-build &&
    ../gcc-${GCC_VER}/configure "${GCC_CONFIGURE_FLAGS[@]}" &&
    make ${MAKE_THREADS} all &&
    make install &&

    cd ${BUILDDIR} &&
    date -uIns > .gcc_built
}

build_newlib() {
    cd ${BUILDDIR}
    echo
    echo "Building newlib (${NEWLIB_VER}) ..."
    [[ -e .newlib_built ]] && return

    rm -rf newlib-build && mkdir -p newlib-build && cd newlib-build &&

    CFLAGS="${NEWLIB_TARGET_CFLAGS[*]}" \
    CXXFLAGS="${NEWLIB_TARGET_CFLAGS[*]}" \
    CFLAGS_FOR_TARGET="${NEWLIB_TARGET_CFLAGS[*]}" \
    CXXFLAGS_FOR_TARGET="${NEWLIB_TARGET_CFLAGS[*]}" \
    ../newlib-${NEWLIB_VER}/configure "${NEWLIB_CONFIGURE_FLAGS[@]}" &&
    sed -i ./Makefile -e '/C\(XX\)\?FLAGS_FOR_TARGET = -g /d' &&

    make ${MAKE_THREADS} all &&

    make install &&

    cd ${BUILDDIR} &&
    date -uIns > .newlib_built
}

build_gdb() {
    cd ${BUILDDIR}
    echo
    echo "Building gdb (${GDB_VER}) ..."
    [[ -e .gdb_built ]] && return

    rm -rf gdb-build && mkdir -p gdb-build && cd gdb-build &&
    ../gdb-${GDB_VER}/configure "${GDB_CONFIGURE_FLAGS[@]}" &&
    make ${MAKE_THREADS} all CFLAGS="-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0" &&
    make install &&

    cd ${BUILDDIR} &&
    date -uIns > .gdb_built
}

build_gmp() {
    cd ${BUILDDIR}
    echo
    echo "Building gmp (${GMP_VER}) ..."
    [[ -e .gmp_built ]] && return

    rm -rf gmp-build && mkdir -p gmp-build && cd gmp-build &&
    ../gmp-${GMP_VER}/configure "${GMP_CONFIGURE_FLAGS[@]}" &&
    make ${MAKE_THREADS} all &&
    make install &&

    cd ${BUILDDIR} &&
    date -uIns > .gmp_built
}

build_mpfr() {
    cd ${BUILDDIR}
    echo
    echo "Building mpfr (${MPFR_VER}) ..."
    [[ -e .mpfr_built ]] && return

    rm -rf mpfr-build && mkdir -p mpfr-build && cd mpfr-build &&
    ../mpfr-${MPFR_VER}/configure "${MPFR_CONFIGURE_FLAGS[@]}" &&
    make ${MAKE_THREADS} all &&
    make install &&

    cd ${BUILDDIR} &&
    date -uIns > .mpfr_built
}

build_mpc() {
    cd ${BUILDDIR}
    echo
    echo "Building mpc (${MPC_VER}) ..."
    [[ -e .mpc_built ]] && return

    rm -rf mpc-build && mkdir -p mpc-build && cd mpc-build &&
    ../mpc-${MPC_VER}/configure "${MPC_CONFIGURE_FLAGS[@]}" &&
    make ${MAKE_THREADS} all &&
    make install &&

    cd ${BUILDDIR} &&
    date -uIns > .mpc_built
}

clean() {
    echo
    echo "Cleaning up..."
    for F in binutils gcc newlib gdb gmp mpfr mpc; do
        rm -rf ./.${F}_extracted ./.${F}_built ./${F}_build || return $?
    done
}

export PATH=$PATH:${PREFIX}/bin

download() {
    echo
    echo "Downloading TAR files."

    download_file ftp://ftp.cs.tu-berlin.de/pub/gnu/binutils binutils-${BINUTILS_VER}.tar.bz2 ${BINUTILS_MD5} &&
    download_file ftp://ftp.cs.tu-berlin.de/pub/gnu/gcc/gcc-${GCC_VER} gcc-${GCC_VER}.tar.bz2 ${GCC_MD5} &&
    download_file ftp://ftp.cs.tu-berlin.de/pub/gnu/gdb gdb-${GDB_VER}.tar.bz2 ${GDB_MD5} &&
    download_file ftp://ftp.cs.tu-berlin.de/pub/gnu/gmp gmp-${GMP_VER}.tar.xz ${GMP_MD5} &&
    download_file ftp://ftp.cs.tu-berlin.de/pub/gnu/mpfr mpfr-${MPFR_VER}.tar.xz ${MPFR_MD5} &&
    download_file ftp://ftp.cs.tu-berlin.de/pub/gnu/mpc mpc-${MPC_VER}.tar.gz ${MPC_MD5} &&
    download_file ftp://sources.redhat.com/pub/newlib newlib-${NEWLIB_VER}.tar.gz ${NEWLIB_MD5}
}

extract() {
    echo
    echo "Extracting TAR files."

    if [[ ! -x ./binutils-${BINUTILS_VER}/configure ]]; then
        tar xjf ${TMP_DIR}/binutils-${BINUTILS_VER}.tar.bz2 || return $?
    fi
    if [[ ! -x ./gcc-${GCC_VER}/configure ]]; then
        tar xjf ${TMP_DIR}/gcc-${GCC_VER}.tar.bz2 || return $?
    fi
    if [[ ! -x ./gdb-${GDB_VER}/configure ]]; then
        tar xjf ${TMP_DIR}/gdb-${GDB_VER}.tar.bz2 || return $?
    fi
    if [[ ! -x ./gmp-${GMP_VER}/configure ]]; then
        tar xJf ${TMP_DIR}/gmp-${GMP_VER}.tar.xz || return $?
    fi
    if [[ ! -x ./mpfr-${MPFR_VER}/configure ]]; then
        tar xJf ${TMP_DIR}/mpfr-${MPFR_VER}.tar.xz || return $?
    fi
    if [[ ! -x ./mpc-${MPC_VER}/configure ]]; then
        tar xzf ${TMP_DIR}/mpc-${MPC_VER}.tar.gz || return $?
    fi
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

check_space() {
    echo
    echo "Checking disk space in ${TMP_DIR}"

    FREETMP=$(df ${BUILDDIR} | awk '{ if (NR == 2) print $4}')
    if [[ $FREETMP -lt $SPACE_NEEDED ]]; then
        echo "Not enough available space in ${TMP_DIR}."
        echo "Minimum ${SPACE_NEEDED} free bytes required."
        return 1
    fi
}

all() {
    echo
    echo "Starting in ${BUILDDIR}. Installing to ${PREFIX}."

    check_space &&
    download &&
    extract &&
    build_binutils &&
    build_gmp &&
    build_mpfr &&
    build_mpc &&
    build_gcc &&
    build_newlib &&
    build_gdb &&

    echo &&
    echo 'Success!' &&
    echo "Insert \"export PATH=${PREFIX}/bin:\$PATH\" to your .bashrc"
}

usage() {
    echo "usage: ${0} [all | download | extract | clean | build_[binutils|gcc|newlib|gdb|gmp|mpfr|mpc]]"
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
