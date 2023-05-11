#!/bin/sh

ESP32_GCC_RELEASE="esp-12.2.0_20230208"
ESP32_GCC_VERSION_DIR="12.2.0"
ESP32_GCC_VERSION_DOWNLOAD="12.2.0_20230208"

ESP32_OPENOCD_VERSION="v0.12.0-esp32-20230313"
ESP32_OPENOCD_VERSION_TGZ="0.12.0-esp32-20230313"

# set the tool path to the default if not already set
if [ -z "${IDF_TOOLS_PATH}" ]; then
    IDF_TOOLS_PATH="${HOME}/.espressif"
fi

TOOLS_PATH="${IDF_TOOLS_PATH}/tools"

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
PLATFORM_SYSTEM="$(python3 -c "import platform; print(platform.system())")"
PLATFORM_MACHINE="$(python3 -c "import platform; print(platform.machine())")"
PLATFORM="${PLATFORM_SYSTEM}-${PLATFORM_MACHINE}"

# map different platform names to a unique OS name
case "${PLATFORM}" in
    linux-amd64|linux64|Linux-x86_64|FreeBSD-amd64|x86_64-linux-gnu)
        OS="x86_64-linux-gnu"
        OS_OCD="linux-amd64"
        ;;
    linux-arm64|Linux-arm64|Linux-aarch64|Linux-armv8l|aarch64)
        OS="aarch64-linux"
        OS_OCD="linux-arm64"
        ;;
    linux-armel|Linux-arm|Linux-armv7l|arm-linux-gnueabi)
        OS="arm-linux-gnueabi"
        OS_OCD="linux-armel"
        ;;
    linux-armhf|arm-linux-gnueabihf)
        OS="arm-linux-gnueabihf"
        OS_OCD="linux-armhf"
        ;;
    linux-i686|linux32|Linux-i686|FreeBSD-i386|i586-linux-gnu|i686-linux-gnu)
        OS="i686-linux-gnu"
        ;;
    macos|osx|darwin|Darwin-x86_64|x86_64-apple-darwin)
        OS="x86_64-apple-darwin"
        OS_OCD="macos"
        ;;
    macos-arm64|Darwin-arm64|aarch64-apple-darwin|arm64-apple-darwin)
        OS="aarch64-apple-darwin"
        OS_OCD="macos-arm64"
        ;;
    *)
        echo "error: OS architecture ${PLATFORM} not supported"
        exit 1
        ;;
esac

download()
{
    if [ "${URL_GET}" = "curl" ]; then
        curl -L "$1" --retry 10 -o "$2"
    elif [ "${URL_GET}" = "wget" ]; then
        wget "$1" -O "$2"
    else
        exit 1
    fi
}

install_arch()
{
    case "$1" in
        esp32)
            TARGET_ARCH="xtensa-esp32-elf"
            ;;
        esp32c3)
            TARGET_ARCH="riscv32-esp-elf"
            ;;
        esp32s2)
            TARGET_ARCH="xtensa-esp32s2-elf"
            ;;
        esp32s3)
            TARGET_ARCH="xtensa-esp32s3-elf"
            ;;
        *)
            echo "error: Unknown architecture $1"
            exit 1
    esac

    TOOLS_DIR="${TOOLS_PATH}/${TARGET_ARCH}/${ESP32_GCC_RELEASE}"

    URL_PATH="https://github.com/espressif/crosstool-NG/releases/download"
    URL_TGZ="${TARGET_ARCH}-${ESP32_GCC_VERSION_DOWNLOAD}-${OS}.tar.xz"
    URL="${URL_PATH}/${ESP32_GCC_RELEASE}/${URL_TGZ}"

    echo "Creating directory ${TOOLS_DIR} ..." && \
    mkdir -p "${TOOLS_DIR}" && \
    cd "${TOOLS_DIR}" && \
    echo "Downloading ${URL_TGZ} ..." && \
    download "${URL}" "${URL_TGZ}" && \
    echo "Extracting ${URL_TGZ} in ${TOOLS_DIR} ..." && \
    tar xfJ "${URL_TGZ}" && \
    echo "Removing ${URL_TGZ} ..." && \
    rm -f "${URL_TGZ}" && \
    echo "$1 toolchain installed in ${TOOLS_DIR}/$TARGET_ARCH"
}

install_openocd()
{
    if [ "${OS_OCD}" = "" ]; then
        echo "OS not supported by this OpenOCD version"
        exit 1
    fi

    TOOLS_DIR="${TOOLS_PATH}/openocd-esp32/${ESP32_OPENOCD_VERSION}"

    URL_PATH="https://github.com/espressif/openocd-esp32/releases/download"
    URL_TGZ="openocd-esp32-${OS_OCD}-${ESP32_OPENOCD_VERSION_TGZ}.tar.gz"
    URL="${URL_PATH}/${ESP32_OPENOCD_VERSION}/${URL_TGZ}"

    echo "Creating directory ${TOOLS_DIR} ..." && \
    mkdir -p "${TOOLS_DIR}" && \
    cd "${TOOLS_DIR}" && \
    echo "Downloading ${URL_TGZ} ..." && \
    download "${URL}" "${URL_TGZ}" && \
    echo "Extracting ${URL_TGZ} in ${TOOLS_DIR} ..." && \
    tar xfz "${URL_TGZ}" && \
    echo "Removing ${URL_TGZ} ..." && \
    rm -f "${URL_TGZ}" && \
    echo "OpenOCD for ESP32x SoCs installed in ${TOOLS_DIR}/$TARGET_ARCH"
}

install_qemu()
{
    if [ "${OS}" != "x86_64-linux-gnu" ]; then
        echo "error: QEMU for ESP32 does not support OS ${OS}"
        exit 1
    fi

    # qemu version depends on the version of ncurses lib
    if [ "$(ldconfig -p | grep libncursesw.so.6)" != "" ]; then
        ESP32_QEMU_VERSION="esp-develop-7.2.0-20230223"
        URL_TGZ="esp-qemu-xtensa-softmmu-develop_7.2.0_20230223-${OS}.tar.bz2"
    else
        ESP32_QEMU_VERSION="esp-develop-20210220"
        URL_TGZ="qemu-${ESP32_QEMU_VERSION}.tar.bz2"
    fi

    TOOLS_DIR="${TOOLS_PATH}/qemu-esp32/${ESP32_QEMU_VERSION}"

    URL_PATH="https://github.com/espressif/qemu/releases/download"
    URL="${URL_PATH}/${ESP32_QEMU_VERSION}/${URL_TGZ}"

    echo "Creating directory ${TOOLS_DIR} ..." && \
    mkdir -p "${TOOLS_DIR}" && \
    cd "${TOOLS_DIR}" && \
    echo "Downloading ${URL_TGZ} ..." && \
    download "${URL}" "${URL_TGZ}" && \
    echo "Extracting ${URL_TGZ} in ${TOOLS_DIR} ..." && \
    tar xfj "${URL_TGZ}" && \
    echo "Removing ${URL_TGZ} ..." && \
    rm -f "${URL_TGZ}" && \
    echo "QEMU for ESP32 installed in ${TOOLS_DIR}"
}

install_gdb()
{
    case $1 in
        xtensa)
            GDB_ARCH="xtensa-esp-elf-gdb"
            ;;
        riscv)
            GDB_ARCH="riscv32-esp-elf-gdb"
            ;;
        *)
            echo "error: Unknown platform $1, use xtensa or riscv"
            exit 1
    esac

    GDB_VERSION="12.1_20221002"

    TOOLS_DIR="${TOOLS_PATH}/${GDB_ARCH}/${GDB_VERSION}"

    URL_PATH="https://github.com/espressif/binutils-gdb/releases/download"
    URL_TGZ="${GDB_ARCH}-${GDB_VERSION}-${OS}.tar.gz"
    URL="${URL_PATH}/esp-gdb-v${GDB_VERSION}/${URL_TGZ}"
echo $URL_GET $URL
    echo "Creating directory ${TOOLS_DIR} ..." && \
    mkdir -p "${TOOLS_DIR}" && \
    cd "${TOOLS_DIR}" && \
    echo "Downloading ${URL_TGZ} ..." && \
    download "${URL}" "${URL_TGZ}" && \
    echo "Extracting ${URL_TGZ} in ${TOOLS_DIR} ..." && \
    tar xfz "${URL_TGZ}" && \
    echo "Removing ${URL_TGZ} ..." && \
    rm -f "${URL_TGZ}" && \
    echo "GDB for $1 installed in ${TOOLS_DIR}"
}

if [ -z "$1" ]; then
    echo "Usage: install.sh <tool>"
    echo "       install.sh gdb <platform>"
    echo "<tool> = all | esp32 | esp32c3 | esp32s2 | esp32s3 | gdb | openocd | qemu"
    echo "<platform> = xtensa | riscv"
    exit 1
elif [ "$1" = "all" ]; then
    ARCH_ALL="esp32 esp32c3 esp32s2 esp32s3"
    for arch in ${ARCH_ALL}; do
        install_arch "$arch"
    done
    install_gdb xtensa
    install_gdb riscv
    install_openocd
    install_qemu
elif [ "$1" = "gdb" ]; then
    if [ -z "$2" ]; then
        echo "platform required: xtensa | riscv"
        exit 1
    fi
    install_gdb "$2"
elif [ "$1" = "openocd" ]; then
    install_openocd
elif [ "$1" = "qemu" ]; then
    install_qemu
else
    install_arch "$1"
fi

echo "Use following command to extend the PATH variable:"
echo ". $(dirname "$0")/export.sh"
