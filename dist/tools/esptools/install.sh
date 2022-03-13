#!/bin/sh

ESP32_GCC_RELEASE="esp-2021r2-patch3"
ESP32_GCC_VERSION_DIR="8.4.0"
ESP32_GCC_VERSION_DOWNLOAD="gcc8_4_0"

ESP32_OPENOCD_VERSION="v0.11.0-esp32-20211220"
ESP32_OPENOCD_VERSION_TGZ="0.11.0-esp32-20211220"

# qemu version depends on the version of ncurses lib
if [ "$(ldconfig -p | grep libncursesw.so.6)" != "" ]; then
    ESP32_QEMU_VERSION="esp-develop-20220203"
else
    ESP32_QEMU_VERSION="esp-develop-20210220"
fi

# set the tool path to the default if not already set
if [ -z ${IDF_TOOLS_PATH} ]; then
    IDF_TOOLS_PATH=${HOME}/.espressif
fi

TOOLS_PATH=${IDF_TOOLS_PATH}/tools

# check the existence of either wget or curl and set the download tool
if [ "$(which curl)" != "" ]; then
    URL_GET="curl"
elif [ "$(which wget)" != "" ]; then
    URL_GET="wget"
else
    echo "error: wget or curl has to be installed"
    exit 1
fi

# check whether python3 is installed as prerequisite
if [ "$(which python3)" = "" ]; then
    echo "error: python3 not found"
    exit 1
fi

# determine the platform using python
PLATFORM_SYSTEM=$(python3 -c "import platform; print(platform.system())")
PLATFORM_MACHINE=$(python3 -c "import platform; print(platform.machine())")
PLATFORM=${PLATFORM_SYSTEM}-${PLATFORM_MACHINE}

# map different platform names to a unique OS name
case ${PLATFORM} in
    linux-amd64|linux64|Linux-x86_64|FreeBSD-amd64)
        OS=linux-amd64
        ;;
    linux-arm64|Linux-arm64|Linux-aarch64|Linux-armv8l)
        OS=linux-arm64
        ;;
    linux-armel|Linux-arm|Linux-armv7l)
        OS=linux-armel
        ;;
    linux-armhf)
        OS=linux-armhf
        ;;
    linux-i686|linux32|Linux-i686|FreeBSD-i386)
        OS=linux-i686
        ;;
    *)
        echo "error: OS architecture ${PLATFORM} not supported"
        exit 1
        ;;
esac

download()
{
    if [ "${URL_GET}" = "curl" ]; then
        curl -L $1 -o $2
    elif [ "${URL_GET}" = "wget" ]; then
        wget $1 -O $2
    else
        exit 1
    fi
}

install_arch()
{
    case $1 in
        esp32)
            TARGET_ARCH="xtensa-esp32-elf"
            ;;
        *)
            echo "error: Unknown architecture $1"
            exit 1
    esac

    TOOLS_DIR=${TOOLS_PATH}/${TARGET_ARCH}/${ESP32_GCC_RELEASE}-${ESP32_GCC_VERSION_DIR}

    URL_PATH=https://github.com/espressif/crosstool-NG/releases/download
    URL_TGZ=${TARGET_ARCH}-${ESP32_GCC_VERSION_DOWNLOAD}-${ESP32_GCC_RELEASE}-${OS}.tar.gz
    URL=${URL_PATH}/${ESP32_GCC_RELEASE}/${URL_TGZ}

    echo "Creating directory ${TOOLS_DIR} ..." && \
    mkdir -p ${TOOLS_DIR} && \
    cd ${TOOLS_DIR} && \
    echo "Downloading ${URL_TGZ} ..." && \
    download ${URL} ${URL_TGZ} && \
    echo "Extracting ${URL_TGZ} in ${TOOLS_DIR} ..." && \
    tar xfz ${URL_TGZ} && \
    echo "Removing ${URL_TGZ} ..." && \
    rm -f ${URL_TGZ} && \
    echo "$1 toolchain installed in ${TOOLS_DIR}/$TARGET_ARCH"
}

install_openocd()
{
    TOOLS_DIR=${TOOLS_PATH}/openocd-esp32/${ESP32_OPENOCD_VERSION}

    URL_PATH=https://github.com/espressif/openocd-esp32/releases/download
    URL_TGZ=openocd-esp32-${OS}-${ESP32_OPENOCD_VERSION_TGZ}.tar.gz
    URL=${URL_PATH}/${ESP32_OPENOCD_VERSION}/${URL_TGZ}

    echo "Creating directory ${TOOLS_DIR} ..." && \
    mkdir -p ${TOOLS_DIR} && \
    cd ${TOOLS_DIR} && \
    echo "Downloading ${URL_TGZ} ..." && \
    download ${URL} ${URL_TGZ} && \
    echo "Extracting ${URL_TGZ} in ${TOOLS_DIR} ..." && \
    tar xfz ${URL_TGZ} && \
    echo "Removing ${URL_TGZ} ..." && \
    rm -f ${URL_TGZ} && \
    echo "OpenOCD for ESP32x SoCs installed in ${TOOLS_DIR}/$TARGET_ARCH"
}

install_qemu()
{
    if [ ${OS} != "linux-amd64" ]; then
        echo "error: QEMU for ESP32 does not support OS ${OS}"
        exit 1
    fi

    TOOLS_DIR=${TOOLS_PATH}/qemu-esp32/${ESP32_QEMU_VERSION}

    URL_PATH=https://github.com/espressif/qemu/releases/download
    URL_TGZ=qemu-${ESP32_QEMU_VERSION}.tar.bz2
    URL=${URL_PATH}/${ESP32_QEMU_VERSION}/${URL_TGZ}

    echo "Creating directory ${TOOLS_DIR} ..." && \
    mkdir -p ${TOOLS_DIR} && \
    cd ${TOOLS_DIR} && \
    echo "Downloading ${URL_TGZ} ..." && \
    download ${URL} ${URL_TGZ} && \
    echo "Extracting ${URL_TGZ} in ${TOOLS_DIR} ..." && \
    tar xfj ${URL_TGZ} && \
    echo "Removing ${URL_TGZ} ..." && \
    rm -f ${URL_TGZ} && \
    echo "QEMU for ESP32 installed in ${TOOLS_DIR}"
}

if [ -z $1 ]; then
    echo "Usage: install.sh <tool>"
    echo "tool = all | esp32 | openocd | qemu"
    exit 1
elif [ "$1" = "all" ]; then
    ARCH_ALL="esp32"
    for arch in ${ARCH_ALL}; do
        install_arch $arch
    done
    install_openocd
    install_qemu
elif [ "$1" = "openocd" ]; then
    install_openocd
elif [ "$1" = "qemu" ]; then
    install_qemu
else
    install_arch $1
fi

echo "Use following command to extend the PATH variable:"
echo ". $(dirname "$0")/export.sh"
