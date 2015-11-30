#/usr/bin/env bash

linux_checkid() {
    udevinfo -a -n ${1} | grep -q "ATTRS{product}==\"${2}\""
}

windows_flash_fm() {
    echo "Checking FTDI device on COM${1}"
    PORTINFO=`${BASEDIR}/../../../tools/windows/ftdiinfo/bin/Debug/ftdiinfo.exe /l COM${1}`
    PORTCHECK=`echo ${PORTINFO} | awk '{ print $1 }'`
    BOARDCHECK=`echo ${PORTINFO} | awk '{ print $3 }'`
    SERIAL=`echo ${PORTINFO} | awk '{ print $2 }'`
    if [ "${PORTCHECK}" != "COM${1}" ]; then
        echo "  port mismatch / ftdiinfo failed"
        exit 1
    fi
    if [ "${BOARDCHECK}" != "\"${FTDI_ID}\"" ]; then
        echo "  target mismatch: target board is \"${FTDI_ID}\", connected is ${BOARDCHECK}"
        exit 1
    fi

    echo "Flashing ${HEXFILE} to COM${1} (${BOARDCHECK} serial ${SERIAL})"
    # Using FlashMagic on Windows (in separate window)
    cmd /C start "FlashMagic ${HEXFILE} to ${BOARDCHECK} on COM${1}" fm.exe "COM(${1}, 230400) DEVICE(LPC2387, 16.000000) HARDWARE(BOOTEXEC, 50, 100) HIGHSPEED(0, 230400) ERASEUSED(${HEXFILE}, PROTECTISP) HEXFILE(${HEXFILE}, NOCHECKSUMS, NOFILL, PROTECTISP) RESET"
}

windows_flash_openocd() {
    echo "Flashing ${HEXFILE} through JTAG"
    # Using OpenOcd on Windows
    #cmd /C start "OpenOCD ${HEXFILE} to ${BOARDCHECK}"
    bash -x ${OPENOCD} ${OPENOCD_IF} "mt_flash ${HEXFILE}; reset run; shutdown"
}

TEMP=`getopt -a -o b:i:p:f:: --long basedir:,id:,ports:,file:,openocd:,openocd-if:,xxx:: \
     -n 'flashutil.sh' -- "$@"`

if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

while true ; do
    echo $1: $2
    case "$1" in
        -b|--basedir)   BASEDIR=$2 ; shift 2 ;;
        -i|--id)        FTDI_ID=$2; shift 2 ;;
        -p|--ports)     PORTS=`echo $2 | sed -e 's:,: :g'`; shift 2 ;;
        --openocd)      OPENOCD=$2; shift 2 ;;
        --openocd-if)   OPENOCD_IF=$2; shift 2 ;;

        --) HEXFILE=$2 ; shift ; break ;;
        *) echo "Internal error!" ; exit 1 ;;
    esac
done

if [ "${OS}" = "Windows_NT" ]; then
    WINDOWS=1
fi

FLASHUTIL_SHELL=${FLASHUTIL_SHELL:-"xterm -e"}

if [ "x${WINDOWS}x" = "xx" ]; then
    echo Pausing terminal
    ${BASEDIR}/termctrl.sh pause
else
    HEXFILE=`echo ${HEXFILE} | sed -e 's:/:\\\\:g'`
    BASEDIRWIN=`echo ${BASEDIR} | sed -e 's:/:\\\\:g'`
fi

pids=""

#
# for OpenOCD let the user verify, that the correct board is connected
#
if [ ${PORTS} = "openocd" ]; then
    [ "$OPENOCD" != "" ] || exit 1
    [ "$OPENOCD_IF" != "" ] || exit 1
#    echo -n "Is the board connected to the JTAG a '${FTDI_ID}' (y/n)? "
#   read REPLY
#   [ "$REPLY" = "y" ] || exit 1
fi
#
# start a flasher for each port
#
for PORT in $PORTS; do
    if [ "x${WINDOWS}x" != "xx" ]; then
        if [ "${PORT}" = "openocd" ]; then
            windows_flash_openocd
        else
            windows_flash_fm ${PORT}
        fi
    else
        if [ "${PORT}" = "openocd" ]; then
            ${OPENOCD} ${OPENOCD_IF} "mt_flash ${HEXFILE}; reset run; shutdown"
        else
        echo Flashing ${HEXFILE} to ${PORT}
        # using homemade lpc2k_pgm else
            ${FLASHUTIL_SHELL} "${BASEDIR}/bin/lpc2k_pgm ${PORT} ${HEXFILE}; sleep 2" &
            pids="${pids} $!"
        fi
    fi
done

### wait for all flasher processes to finish
echo Waiting until all devices have been programmed...
for pid in "${pids}"; do
    wait ${pid}
done

if [ "x${WINDOWS}x" = "xx" ]; then
    echo Resuming terminal
    ${BASEDIR}/termctrl.sh continue
fi
