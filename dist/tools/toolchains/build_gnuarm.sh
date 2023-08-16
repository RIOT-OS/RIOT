#!/usr/bin/env bash

# directory to install compiled binaries into
PREFIX=${HOME}/gnuarm

# directory to download source files and store intermediates
TMP_DIR=/var/tmp
GNUARM_BUILDDIR=${GNUARM_BUILDDIR:-"${TMP_DIR}/gnuarm-${USER}"}

GCC_VER=4.7.2
GCC_MD5=cc308a0891e778cfda7a151ab8a6e762

BINUTILS_VER=2.20.1
BINUTILS_MD5=2b9dc8f2b7dbd5ec5992c6e29de0b764

NEWLIB_VER=1.20.0
NEWLIB_MD5=e5488f545c46287d360e68a801d470e8

GDB_VER=7.3.1
GDB_MD5=b89a5fac359c618dda97b88645ceab47

#uncomment to support multi-threaded compile
MAKE_THREADS=-j4

DOWNLOADER=wget
DOWNLOADER_OPTS="-nv -c"

if [ `uname` = "Linux" ]; then
  MD5=md5sum
  MD5_OPTS="-c -"
elif [ `uname` = "Darwin" ]; then
  MD5=md5
  MD5_OPTS=""
else
    echo "CAUTION: No 'md5' tool for your host system found!"
fi

#
# Build targets
#
FILES=.

HOST_GCC_VER=`gcc --version | awk '/gcc/{print $NF}'`

SPACE_NEEDED=2641052
FREETMP=`df ${TMP_DIR} | awk '{ if (NR == 2) print $4}'`

build_binutils() {
    echo "Building binutils..."
    if [ ! -e .binutils_extracted ] ; then
        tar -xjf ${FILES}/binutils-${BINUTILS_VER}.tar.bz2
        touch .binutils_extracted
    fi
    if [[ $HOST_GCC_VER == 4.6* || $HOST_GCC_VER == 4.7* ]]
    then
        CFLAGS="-Wno-error=unused-but-set-variable -Wno-error=unused-but-set-parameter"
    else
        CFLAGS="-Wno-error=unused"
    fi
    rm -rf binutils-build && mkdir -p binutils-build && cd binutils-build &&
    ../binutils-${BINUTILS_VER}/configure --target=arm-elf --prefix=${PREFIX} --enable-interwork --enable-multilib &&
    make ${MAKE_THREADS} all CFLAGS="${CFLAGS}" &&
    make install &&
    cd ${GNUARM_BUILDDIR}
}

build_gcc() {
    echo "Building gcc..."
    if [ ! -e .gcc_extracted ] ; then
        tar -xjf ${FILES}/gcc-${GCC_VER}.tar.bz2 &&
        touch .gcc_extracted
    fi
    rm -rf gcc-build && mkdir -p gcc-build && cd gcc-build &&
    ../gcc-${GCC_VER}/configure --target=arm-elf --prefix=${PREFIX} --enable-interwork --enable-multilib --enable-languages="c,c++" --with-newlib --enable-lto --disable-libssp --with-headers=${GNUARM_BUILDDIR}/newlib-${NEWLIB_VER}/newlib/libc/include --enable-obsolete &&

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
    ../newlib-${NEWLIB_VER}/configure --target=arm-elf --prefix=${PREFIX} --enable-interwork --enable-multilib --disable-newlib-supplied-syscalls --enable-newlib-reent-small --enable-newlib-io-long-long --enable-newlib-io-float &&
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

    make ${MAKE_THREADS} all CFLAGS="-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0" &&
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
    download_file ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-${GCC_VER} gcc-${GCC_VER}.tar.bz2 ${GCC_MD5} &&
    download_file ftp://sources.redhat.com/pub/newlib newlib-${NEWLIB_VER}.tar.gz ${NEWLIB_MD5} &&
    download_file http://ftp.gnu.org/gnu/gdb gdb-${GDB_VER}.tar.bz2 ${GDB_MD5}
}

download_file() {
    echo "Downloading ${1}/${2}..."
    ${DOWNLOADER} ${DOWNLOADER_OPTS} $1/$2

    echo -n "Checking MD5 of "
    echo "${3}  ${2}" | ${MD5} ${MD5_OPTS}
}

check_space() {
    echo "Checking disk space in ${TMP_DIR}"
    if [ $FREETMP -lt $SPACE_NEEDED ]
    then
        echo "Not enough available space in ${TMP_DIR}. Minimum ${SPACE_NEEDED} free bytes required."
        exit 1
    fi
}

build_all() {
    echo "Starting in ${GNUARM_BUILDDIR}. Installing to ${PREFIX}."
    check_space &&
    download &&
    build_binutils &&
    extract_newlib &&
    build_gcc &&
    build_newlib &&
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
