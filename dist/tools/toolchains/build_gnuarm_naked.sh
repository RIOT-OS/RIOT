#!/bin/bash

# directory to install compiled binaries into
PREFIX=${HOME}/gnuarm-naked

# directory to download source files and store intermediates
GNUARM_BUILDDIR=${GNUARM_BUILDDIR:-"/tmp/gnuarm-naked-${USER}"}

#GCC_VER=4.5.1
#GCC_MD5=dc8959e31b01a65ce10d269614815054
GCC_VER=4.3.4
GCC_MD5=575b3220bb8e7060939c429fc8608d22

BINUTILS_VER=2.20.1
BINUTILS_MD5=9cdfb9d6ec0578c166d3beae5e15c4e5

NEWLIB_VER=1.18.0
NEWLIB_MD5=3dae127d4aa659d72f8ea8c0ff2a7a20

GDB_VER=7.2
GDB_MD5=64260e6c56979ee750a01055f16091a5

#uncomment to support multi-threaded compile
MAKE_THREADS=-j4

DOWNLOADER=wget
DOWNLOADER_OPTS="-nv -c"

#
# Build targets
#
FILES=.

build_binutils() {
    echo "Building binutils..."
    if [ ! -e .binutils_extracted ] ; then
        tar -xjf ${FILES}/binutils-${BINUTILS_VER}.tar.bz2
        touch .binutils_extracted
    fi
    rm -rf binutils-build && mkdir -p binutils-build && cd binutils-build &&
    ../binutils-${BINUTILS_VER}/configure --target=arm-elf --prefix=${PREFIX} --enable-interwork --enable-multilibi --with-float=soft --with-fpu=vfp &&
    make ${MAKE_THREADS} all CFLAGS=-Wformat=0 &&
    make install &&
    cd ${GNUARM_BUILDDIR}
}

build_gcc() {
    echo "Building gcc..."
    if [ ! -e .gcc_extracted ] ; then
        tar -xjf ${FILES}/gcc-core-${GCC_VER}.tar.bz2 &&
        touch .gcc_extracted
    fi
    rm -rf gcc-build && mkdir -p gcc-build && cd gcc-build &&
    ../gcc-${GCC_VER}/configure --target=arm-elf --prefix=${PREFIX} --enable-interwork --enable-multilib --enable-languages="c" --enable-lto --disable-libssp --disable-hardfloat --with-float=soft --with-fpu=vfp --without-headers &&

    make ${MAKE_THREADS} all &&
    make install &&

    cd ${GNUARM_BUILDDIR}
}

extract_newlib() {
    if [ ! -e .newlib_extracted ] ; then
        echo -n "Extracting newlib..."
        tar -xzf ${FILES}/newlib-${NEWLIB_VER}.tar.gz &&
        touch .newlib_extracted &&
        echo " Done."
    fi
}

build_newlib() {
    cd ${GNUARM_BUILDDIR} &&

    if [ ! -e .newlib_extracted ] ; then
        extract_newlib
    fi

    rm -rf newlib-build && mkdir -p newlib-build && cd newlib-build &&
    ../newlib-${NEWLIB_VER}/configure --target=arm-elf --prefix=${PREFIX} --enable-interwork --enable-multilib --disable-newlib-supplied-syscalls --enable-newlib-reent-small --enable-newlib-io-long-long --enable-newlib-io-float --with-float=soft --with-fpu=vfp &&
    #--enable-newlib-supplied-syscalls &&
    # options to try: --enable-newlib-reent-small
    make ${MAKE_THREADS} TARGET_CFLAGS=-DREENTRANT_SYSCALLS_PROVIDED all &&
    make install &&

    # generate zip-file to provide binary download
    cd ${PREFIX}/arm-elf &&

    #
    # package compiled newlib for windows users. any new version must be uploaded to the
    # webserver. see manual arm/toolchain/windows for paths and documentation.
    #
    zip -ru newlib-${NEWLIB_VER}.zip include sys-include lib/*.a lib/thumb/*.a

    cd ${GNUARM_BUILDDIR}
}

build_gdb() {
    echo "Building gdb..."
    if [ ! -e .gdb_extracted ] ; then
        tar -xjf ${FILES}/gdb-${GDB_VER}.tar.bz2 &&
        touch .gdb_extracted
    fi
    rm -rf gdb-build && mkdir -p gdb-build && cd gdb-build &&
    ../gdb-${GDB_VER}/configure --target=arm-elf --prefix=${PREFIX} --enable-interwork --enable-multilib &&

    make ${MAKE_THREADS} all CFLAGS=-D_FORTIFY_SOURCE=0 &&
    make install &&

    cd ${GNUARM_BUILDDIR}
}

clean() {
    echo "Cleaning up..."
    rm -rf .gdb_extracted .newlib_extracted .gcc_extracted .binutils_extracted
    rm -rf binutils-build gcc-build newlib-build gdb-build
}

export PATH=$PATH:${PREFIX}/bin

download() {
    download_file http://ftp.gnu.org/gnu/binutils binutils-${BINUTILS_VER}.tar.bz2 ${BINUTILS_MD5} &&
    download_file ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-${GCC_VER} gcc-core-${GCC_VER}.tar.bz2 ${GCC_MD5} &&
    download_file http://ftp.gnu.org/gnu/gdb gdb-${GDB_VER}.tar.bz2 ${GDB_MD5}
}

download_file() {
    echo "Downloading ${1}/${2}..."
    ${DOWNLOADER} ${DOWNLOADER_OPTS} $1/$2

    echo -n "Checking MD5 of "
    echo "${3}  ${2}" | md5sum -c -
}

build_all() {
    echo "Starting in ${GNUARM_BUILDDIR}. Installing to ${PREFIX}."
    download &&
    build_binutils &&
    build_gcc &&
    build_gdb &&

    echo "Build complete."
}

usage() {
    echo "usage: ${0} build_[binutils|gcc|newlib|gdb|all]"
    echo "example: ./build_gnuarm build_all"
    echo ""
    echo "Builds a GNU ARM toolchain. installs to HOME/gnuarm, uses /tmp/gnuarm-USER as temp."
    echo "Edit to change these directories."
    echo "Run like \"MAKE_THREADS=-j4 ${0} build_all\" to speed up on multicore systems."
}

if [ -z "${1}" ]; then
    usage
    exit 1
fi

mkdir -p ${GNUARM_BUILDDIR}

cd ${GNUARM_BUILDDIR}

$*
