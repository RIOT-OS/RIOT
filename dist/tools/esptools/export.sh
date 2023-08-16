#!/bin/sh

ESP32_GCC_RELEASE="esp-2021r2-patch3"
ESP32_GCC_VERSION_DIR="8.4.0"

ESP32_OPENOCD_VERSION="v0.11.0-esp32-20211220"

# qemu version depends on the version of ncurses lib
if [ "$(ldconfig -p | grep libncursesw.so.6)" != "" ]; then
    ESP32_QEMU_VERSION="esp-develop-20220203"
else
    ESP32_QEMU_VERSION="esp-develop-20210220"
fi

if [ -z ${IDF_TOOLS_PATH} ]; then
    IDF_TOOLS_PATH=${HOME}/.espressif
fi

TOOLS_PATH=${IDF_TOOLS_PATH}/tools

export_arch()
{
    case $1 in
        esp32)
            TARGET_ARCH="xtensa-esp32-elf"
            ;;
        *)
            echo "Unknown architecture $1"
            exit 1
    esac

    TOOLS_DIR=${TOOLS_PATH}/${TARGET_ARCH}/${ESP32_GCC_RELEASE}-${ESP32_GCC_VERSION_DIR}/${TARGET_ARCH}
    TOOLS_DIR_IN_PATH=`echo $PATH | grep ${TOOLS_DIR}`

    if [ -e ${TOOLS_DIR} ] && [ -z ${TOOLS_DIR_IN_PATH} ]; then
        echo "Extending PATH by ${TOOLS_DIR}/bin"
        export PATH=${TOOLS_DIR}/bin:${PATH}
    fi

    unset TOOLS_DIR
}

export_openocd()
{
    TOOLS_DIR=${TOOLS_PATH}/openocd-esp32/${ESP32_OPENOCD_VERSION}
    TOOLS_DIR_IN_PATH=`echo $PATH | grep ${TOOLS_DIR}`
    OPENOCD_DIR=${TOOLS_DIR}/openocd-esp32

    if [ -e ${OPENOCD_DIR} ] && [ -z ${TOOLS_DIR_IN_PATH} ]; then
        echo "Extending PATH by ${TOOLS_DIR}/bin"
        export PATH=${OPENOCD_DIR}/bin:${PATH}
        export OPENOCD="${OPENOCD_DIR}/bin/openocd -s ${OPENOCD_DIR}/share/openocd/scripts"
    fi

    unset TOOLS_DIR
    unset OPENOCD_DIR
}

export_qemu()
{
    TOOLS_DIR=${TOOLS_PATH}/qemu-esp32/${ESP32_QEMU_VERSION}/qemu
    TOOLS_DIR_IN_PATH=`echo $PATH | grep ${TOOLS_DIR}`

    if [ -e ${TOOLS_DIR} ] && [ -z ${TOOLS_DIR_IN_PATH} ]; then
        echo "Extending PATH by ${TOOLS_DIR}/bin"
        export PATH=${TOOLS_DIR}/bin:${PATH}
    fi

    unset TOOLS_DIR
}

if [ -z $1 ]; then
    echo "Usage: export.sh <tool>"
    echo "tool = all | esp32 | openocd | qemu"
elif [ "$1" = "all" ]; then
    ARCH_ALL="esp32"
    for arch in ${ARCH_ALL}; do
        export_arch $arch
    done
    export_openocd
    export_qemu
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
