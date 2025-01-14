#!/bin/sh

ESP32_GCC_RELEASE="esp-14.2.0_20241119"
ESP8266_GCC_RELEASE="esp-5.2.0_20191018"

ESP32_OPENOCD_VERSION="v0.12.0-esp32-20241016"

ESP32_QEMU_VERSION="esp-develop-9.0.0-20240606"

GDB_VERSION="14.2_20240403"

if [ -z "${IDF_TOOLS_PATH}" ]; then
    IDF_TOOLS_PATH="${HOME}/.espressif"
fi

TOOLS_PATH="${IDF_TOOLS_PATH}/tools"

export_arch()
{
    case $1 in
        esp8266)
            TARGET_ARCH="xtensa-esp8266-elf"
            ESP_GCC_RELEASE="${ESP8266_GCC_RELEASE}"
            ;;
        esp32|esp32s2|esp32s3)
            TARGET_ARCH="xtensa-esp-elf"
            ESP_GCC_RELEASE="${ESP32_GCC_RELEASE}"
            ;;
        esp32c3)
            TARGET_ARCH="riscv32-esp-elf"
            ESP_GCC_RELEASE="${ESP32_GCC_RELEASE}"
            ;;
        *)
            echo "Unknown architecture $1"
            return
    esac

    TOOLS_DIR="${TOOLS_PATH}/${TARGET_ARCH}/${ESP_GCC_RELEASE}/${TARGET_ARCH}"
    TOOLS_DIR_IN_PATH="$(echo $PATH | grep "${TOOLS_DIR}")"

    if [ ! -e "${TOOLS_DIR}" ]; then
        echo "${TOOLS_DIR} does not exist - please run"
        echo "\${RIOTBASE}/dist/tools/esptools/install.sh $1"
        return
    fi

    if [ -z "${TOOLS_DIR_IN_PATH}" ]; then
        echo "Extending PATH by ${TOOLS_DIR}/bin"
        export PATH="${TOOLS_DIR}/bin:${PATH}"
    fi

    echo "To make this permanent, add this line to your ~/.bashrc or ~/.profile:"
    echo PATH="\$PATH:${TOOLS_DIR}/bin"

    unset TOOLS_DIR
}

export_openocd()
{
    TOOLS_DIR="${TOOLS_PATH}/openocd-esp32/${ESP32_OPENOCD_VERSION}"
    TOOLS_DIR_IN_PATH="$(echo $PATH | grep "${TOOLS_DIR}")"
    OPENOCD_DIR="${TOOLS_DIR}/openocd-esp32"

    if [ -e "${OPENOCD_DIR}" ] && [ -z "${TOOLS_DIR_IN_PATH}" ]; then
        echo "Extending PATH by ${TOOLS_DIR}/bin"
        export PATH="${OPENOCD_DIR}/bin:${PATH}"
        export OPENOCD="${OPENOCD_DIR}/bin/openocd -s ${OPENOCD_DIR}/share/openocd/scripts"
    fi

    unset TOOLS_DIR
    unset OPENOCD_DIR
}

export_qemu()
{
    # determine the platform using python
    PLATFORM_SYSTEM="$(python3 -c "import platform; print(platform.system())")"
    PLATFORM_MACHINE="$(python3 -c "import platform; print(platform.machine())")"
    PLATFORM="${PLATFORM_SYSTEM}-${PLATFORM_MACHINE}"

    # map different platform names to a unique OS name
    case "${PLATFORM}" in
        linux-amd64|linux64|Linux-x86_64|FreeBSD-amd64)
            OS="linux-amd64"
            ;;
        *)
            echo "error: OS architecture ${PLATFORM} not supported"
            return
            ;;
    esac

    case $1 in
        riscv)
            QEMU_ARCH="qemu-riscv32-softmmu"
            ;;
        *)
            QEMU_ARCH="qemu-xtensa-softmmu"
            ;;
    esac

    # qemu version depends on the version of ncurses lib
    if [ "$(ldconfig -p | grep -c libncursesw.so.6)" == "0" ]; then
        ESP32_QEMU_VERSION="esp-develop-20210220"
    fi

    if [ -z "${IDF_TOOLS_PATH}" ]; then
        IDF_TOOLS_PATH="${HOME}/.espressif"
    fi

    TOOLS_DIR="${TOOLS_PATH}/${QEMU_ARCH}/${ESP32_QEMU_VERSION}/qemu"
    TOOLS_DIR_IN_PATH="$(echo $PATH | grep "${TOOLS_DIR}")"

    if [ -e "${TOOLS_DIR}" ] && [ -z "${TOOLS_DIR_IN_PATH}" ]; then
        echo "Extending PATH by ${TOOLS_DIR}/bin"
        export PATH="${TOOLS_DIR}/bin:${PATH}"
    fi

    unset TOOLS_DIR
}

export_gdb()
{
    case "$1" in
        xtensa)
            GDB_ARCH="xtensa-esp-elf-gdb"
            ;;
        riscv)
            GDB_ARCH="riscv32-esp-elf-gdb"
            ;;
        *)
            echo "error: Unknown platform $1, use xtensa or riscv"
            return
    esac

    TOOLS_DIR="${TOOLS_PATH}/${GDB_ARCH}/${GDB_VERSION}/${GDB_ARCH}"
    TOOLS_DIR_IN_PATH="$(echo $PATH | grep "${TOOLS_DIR}")"

    if [ -e "${TOOLS_DIR}" ] && [ -z "${TOOLS_DIR_IN_PATH}" ]; then
        echo "Extending PATH by ${TOOLS_DIR}/bin"
        export PATH="${TOOLS_DIR}/bin:${PATH}"
    fi

    unset TOOLS_DIR
}

if [ -z "$1" ]; then
    echo "Usage: export.sh <tool>"
    echo "       export.sh gdb <platform>"
    echo "       export.sh qemu <platform>"
    echo "<tool> = all | esp8266 | esp32 | esp32c3 | esp32s2 | esp32s3 | gdb | openocd | qemu"
    echo "<platform> = xtensa | riscv"
elif [ "$1" = "all" ]; then
    ARCH_ALL="esp8266 esp32 esp32c3 esp32s2 esp32s3"
    for arch in ${ARCH_ALL}; do
        export_arch "$arch"
    done
    export_gdb xtensa
    export_gdb riscv
    export_openocd
    export_qemu xtensa
    export_qemu riscv
    export_gdb xtensa
    export_gdb riscv
elif [ "$1" = "gdb" ]; then
    if [ -z "$2" ]; then
        echo "platform required: xtensa | riscv"
    else
        export_gdb "$2"
    fi
elif [ "$1" = "openocd" ]; then
    export_openocd
elif [ "$1" = "qemu" ]; then
    export_qemu $2
else
    export_arch $1
fi

unset ESP32_GCC_RELEASE
unset ESP32_GCC_VERSION_DOWNLOAD
unset ESP32_GCC_VERSION_DIR

unset ESP32_OPENOCD_VERSION
unset ESP32_OPENOCD_VERSION_FILE

unset ESP32_QEMU_VERSION
