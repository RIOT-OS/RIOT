#!/bin/bash

# directory to download source files and store intermediates
[[ -z $TARGET ]] && TARGET=i586-none-elf
[[ -z $TMP_DIR ]] && TMP_DIR=/var/tmp/RIOT-toolchain-${USER}
[[ -z $BUILDDIR ]] && BUILDDIR=${TMP_DIR}/${TARGET}
[[ -z $PREFIX ]] && PREFIX=${PWD}/toolchain/${TARGET}
[[ -z $MAKE_THREADS ]] && MAKE_THREADS=
PATCHDIR=$(readlink -e "$(dirname $0)/patches")

NEWLIB_VER=2.2.0.20150623
NEWLIB_ARCHIVE=newlib-${NEWLIB_VER}.tar.gz
NEWLIB_MD5=5cf6cd9ded91bca10c9f0a22a9da8e02
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
newlib-2.2.0.20150623-RIOT-i586-none-elf.patch
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

GCC_VER=4.8.5
GCC_ARCHIVE=gcc-${GCC_VER}.tar.bz2
GCC_MD5=80d2c2982a3392bb0b89673ff136e223
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

BINUTILS_VER=2.25
BINUTILS_ARCHIVE=binutils-${BINUTILS_VER}.tar.bz2
BINUTILS_MD5=d9f3303f802a5b6b0bb73a335ab89d66
BINUTILS_CONFIGURE_FLAGS=(
    --prefix=${PREFIX}

    --enable-interwork
    --enable-multilib
    --disable-nls
)

GDB_VER=7.9.1
GDB_ARCHIVE=gdb-${GDB_VER}.tar.xz
GDB_MD5=35374c77a70884eb430c97061053a36e
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
GMP_ARCHIVE=gmp-${GMP_VER}.tar.xz
GMP_MD5=e5fe367801ff067b923d1e6a126448aa
GMP_CONFIGURE_FLAGS=(
    --prefix=${PREFIX}

    --enable-cxx
    --disable-nls
)

MPFR_VER=3.1.2
MPFR_ARCHIVE=mpfr-${MPFR_VER}.tar.xz
MPFR_MD5=e3d203d188b8fe60bb6578dd3152e05c
MPFR_CONFIGURE_FLAGS=(
    --prefix=${PREFIX}
    --with-gmp=${PREFIX}

    --disable-nls
)

MPC_VER=1.0.2
MPC_ARCHIVE=mpc-${MPC_VER}.tar.gz
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

    download_file ftp://ftp.cs.tu-berlin.de/pub/gnu/binutils ${BINUTILS_ARCHIVE} ${BINUTILS_MD5} &&
    download_file ftp://ftp.cs.tu-berlin.de/pub/gnu/gcc/gcc-${GCC_VER} ${GCC_ARCHIVE} ${GCC_MD5} &&
    download_file ftp://ftp.cs.tu-berlin.de/pub/gnu/gdb ${GDB_ARCHIVE} ${GDB_MD5} &&
    download_file ftp://ftp.cs.tu-berlin.de/pub/gnu/gmp ${GMP_ARCHIVE} ${GMP_MD5} &&
    download_file ftp://ftp.cs.tu-berlin.de/pub/gnu/mpfr ${MPFR_ARCHIVE} ${MPFR_MD5} &&
    download_file ftp://ftp.cs.tu-berlin.de/pub/gnu/mpc ${MPC_ARCHIVE} ${MPC_MD5} &&
    download_file ftp://sources.redhat.com/pub/newlib ${NEWLIB_ARCHIVE} ${NEWLIB_MD5}
}

extract() {
    echo
    echo "Extracting TAR files."

    if [[ ! -x ./binutils-${BINUTILS_VER}/configure ]]; then
        tar axf ${TMP_DIR}/${BINUTILS_ARCHIVE} || return $?
    fi
    if [[ ! -x ./gcc-${GCC_VER}/configure ]]; then
        tar axf ${TMP_DIR}/${GCC_ARCHIVE} || return $?
    fi
    if [[ ! -x ./gdb-${GDB_VER}/configure ]]; then
        tar axf ${TMP_DIR}/${GDB_ARCHIVE} || return $?
    fi
    if [[ ! -x ./gmp-${GMP_VER}/configure ]]; then
        tar axf ${TMP_DIR}/${GMP_ARCHIVE} || return $?
    fi
    if [[ ! -x ./mpfr-${MPFR_VER}/configure ]]; then
        tar axf ${TMP_DIR}/${MPFR_ARCHIVE} || return $?
    fi
    if [[ ! -x ./mpc-${MPC_VER}/configure ]]; then
        tar axf ${TMP_DIR}/${MPC_ARCHIVE} || return $?
    fi
    if [[ ! -x ./newlib-${NEWLIB_VER}/configure ]]; then
        tar axf ${TMP_DIR}/${NEWLIB_ARCHIVE} &&
        for (( I=0; I < ${#NEWLIB_PATCHES[@]}; I+=1 )); do
            echo "Applying Newlib patch ${NEWLIB_PATCHES[$I]}"
            for (( P=0; P < 4; P+=1 )); do
                patch -p${P} --dry-run -f < "${PATCHDIR}/${NEWLIB_PATCHES[$I]}" 2>&1 > /dev/null || continue
                patch -p${P} -f < "${PATCHDIR}/${NEWLIB_PATCHES[$I]}" || return $?
                break
            done
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
