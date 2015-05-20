#/bin/bash

if [ $# -le 0 ]; then
    echo "usage: $0 [openocd interface name] [openocd args]" >&2
    echo "  common interfaces: olimex-arm-usb-ocd olimex-jtag-tiny olimex-jtag-tiny-a"
    echo ""
    echo "using default olimex-jtag-tiny-a"
    INTERFACE=olimex-jtag-tiny-a
else
    INTERFACE=$1
    shift
fi

if [ $# -ge 1 ]; then
    COMMAND=$@
else
    COMMAND="debug_level 1"
fi

if [ "${OS}" = "Windows_NT" ]; then
    WINDOWS=1
fi

if [ "x${WINDOWS}x" = "xx" ]; then
    xterm -e "openocd -s ../../.. -f interface/${INTERFACE}.cfg -f board/pttu/tools/openocd-pttu.cfg -c \"${COMMAND}\"|| read" &
else
    echo ${COMMAND}
    #cmd /C start "OpenOCD PTTU using ${INTERFACE}"
    openocd-ftd2xx.exe -s ../../.. -f interface/${INTERFACE}.cfg -f board/pttu/tools/openocd-pttu.cfg -c "${COMMAND}"
fi
