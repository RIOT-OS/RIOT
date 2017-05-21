#!/bin/bash

set -e
MYDIR="$(dirname "$(which "$0")")"

usage()
{
cat << EOF
usage: $0 options

OPTIONS:
   -h      Show this message
   -f	   riot elf file
   -b	   xilinx bit file
EOF
}


ELF=""
BIT=""
while getopts “hf:b:” OPTION; do
	case $OPTION in
	h)
		usage
		exit 1
		;;
	f)
		ELF=${OPTARG}
		;;
	b)
		BIT=${OPTARG}
		;;
	?)
		usage
		exit
		;;
	esac
done

if [ -z $ELF ]
then
	echo "Please provide the riot application elf file"
	usage
	exit 1
fi

if [ ! -e cache ]; then
	mkdir cache
fi

rm -rf cache/*

echo "Build FSBL bootloader $PWD"
source /opt/Xilinx/SDK/2014.2/settings64.sh

export WS=cache/ws

rm -fr $WS
xsdk -wait -script $MYDIR/my_SDKproj.xml -workspace $WS
xsdk -wait -eclipseargs -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -build all -data $WS -vmargs -Dorg.eclipse.cdt.core.console=org.eclipse.cdt.core.systemConsole
echo "FSBL build done"

echo "Create boot.bin"
cp $ELF cache/riot.elf
bootgen -image $MYDIR/boot.bif -w -o i boot.bin

echo "Done"
