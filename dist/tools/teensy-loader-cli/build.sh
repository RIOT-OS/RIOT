#!/usr/bin/env sh

#
# Copyright (c) 2015 Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
#
# A simple script to fetch and build the teensy_loader_cli tool used for the
# Teensy boards.
# For more information see: https://www.pjrc.com/teensy/loader_cli.html
#

# get sources from repository
git clone https://github.com/PaulStoffregen/teensy_loader_cli.git
cd teensy_loader_cli

# resolv build host in a hacky way
UNAME=`uname`
TARGET="WINDOWS";
if [ "x$UNAME" = "xLinux" ]; then
    TARGET="LINUX";
elif [ "x$UNAME" = "xDarwin" ]; then
    TARGET="MACOSX";
fi;

# check if the library exists
if [ "x$TARGET" = "xLINUX" ] && ! ldconfig -p | grep 'libusb' > /dev/null; then
    echo "please install libusb-dev";
    exit 1;
elif [ "x$TARGET" = "xMACOSX" ] && ! pkg-config --list-all | grep 'libusb' > /dev/null; then
    echo "please install libusb-dev";
    exit 1;
elif [ "x$TARGET" = "xWINDOWS" ]; then
    echo "can't build for windows... yet";
    exit 1;
fi;

# build the application
OS=$TARGET make

# copy the tool to the base directory
mv teensy_loader_cli ../teensy_loader
cd ..
rm -rf teensy_loader_cli
