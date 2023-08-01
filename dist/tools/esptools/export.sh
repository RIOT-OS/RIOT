#!/bin/sh

ESP32_GCC_RELEASE="esp-12.2.0_20230208"

ESP32_OPENOCD_VERSION="v0.12.0-esp32-20230313"

if [ -z "${IDF_TOOLS_PATH}" ]; then
    IDF_TOOLS_PATH="${HOME}/.espressif"
fi

TOOLS_PATH="${IDF_TOOLS_PATH}/tools"

export_arch()
{
    case $1 in
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
            echo "Unknown architecture $1"
            return
    esac

    TOOLS_DIR="${TOOLS_PATH}/${TARGET_ARCH}/${ESP32_GCC_RELEASE}/${TARGET_ARCH}"
    TOOLS_DIR_IN_PATH="$(echo $PATH | grep "${TOOLS_DIR}")"

    if [ -e "${TOOLS_DIR}" ] && [ -z "${TOOLS_DIR_IN_PATH}" ]; then
        echo "Extending PATH by ${TOOLS_DIR}/bin"
        export PATH="${TOOLS_DIR}/bin:${PATH}"
    fi

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

    # qemu version depends on the version of ncurses lib
    if [ "$(ldconfig -p | grep libncursesw.so.6)" != "" ]; then
        ESP32_QEMU_VERSION="esp-develop-20220203"
    else
        ESP32_QEMU_VERSION="esp-develop-20210220"
    fi

    if [ -z "${IDF_TOOLS_PATH}" ]; then
        IDF_TOOLS_PATH="${HOME}/.espressif"
    fi

    TOOLS_DIR="${TOOLS_PATH}/qemu-esp32/${ESP32_QEMU_VERSION}/qemu"
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

    GDB_VERSION="12.1_20221002"

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
    echo "<tool> = all | esp32 | esp32c3 | esp32s2 | esp32s3 | gdb | openocd | qemu"
    echo "<platform> = xtensa | riscv"
elif [ "$1" = "all" ]; then
    ARCH_ALL="esp32 esp32c3 esp32s2 esp32s3"
    for arch in ${ARCH_ALL}; do
        export_arch "$arch"
    done
    export_gdb xtensa
    export_gdb riscv
    export_openocd
    export_qemu
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
    export_qemu
else
    export_arch $1
fi

unset ESP32_GCC_RELEASE
unset ESP32_GCC_VERSION_DOWNLOAD
unset ESP32_GCC_VERSION_DIR

unset ESP32_OPENOCD_VERSION
unset ESP32_OPENOCD_VERSION_FILE

unset ESP32_QEMU_VERSION
